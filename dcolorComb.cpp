

#include"dcolorComb.h"

int * dcolorComb::m_Data = NULL;


dcolorComb::dcolorComb(Mat &src,float rad,float h, float v)
{
	this->src = src;
	this->rad = rad;
	this->hor_rate = h;
	this->ver_rate = v;
	hei = src.size().height;
	wid = src.size().width;

	m_Data = new int[hei*(wid+1)/2];
	memset(m_Data, 0, hei*wid);
	makeMask();

}

dcolorComb::~dcolorComb()
{
	delete[] m_Data;
}

void dcolorComb::makeMask()
{
	double a = tan(rad * 3.1415926 / 360);
	dhei = wid*a;

	int *pMask;
	int offset;
	double m;

	int * p_mask = m_Data;
	int p = hei / 2 + dhei;
	for (int i = 0; i < wid/2; i++)
	{

		for (int j = 0; j < hei; j++)
		{
			if ((j>dhei) && j<(hei - dhei))
			{
				m = (hei*i / 2 - i*j)*a;
				*p_mask = (j-int(m / p))*3;

			}
			p_mask++;
		}
	}
}


Mat dcolorComb::T_adjust(Mat &sr)
{

	Mat imgL = Mat(src, Rect(0, 0, wid / 2, hei));
	Mat imgR = Mat(src, Rect(wid / 2, 0, wid/2 , hei));

	transpose(imgL, imgL);
	flip(imgL, imgL, 0);
	transpose(imgR, imgR);
	flip(imgR, imgR, 1);

	Mat r_imgL = Mat::zeros(wid/2, hei, CV_8UC3);
	Mat r_imgR = Mat::zeros(wid/2, hei, CV_8UC3);

	uchar * dataL, *ptL, *dataR, *ptR;
	int * p_mask = m_Data;
	for (int i = 0; i < wid/2; i++)
	{
		dataL = (uchar*)imgL.ptr<uchar>(i);
		ptL = (uchar*)r_imgL.ptr<uchar>(i);
		dataR = (uchar*)imgR.ptr<uchar>(i);
		ptR = (uchar*)r_imgR.ptr<uchar>(i);

		for (int j = 0; j < hei; j++)
		{
			if ((j>dhei+1) && j<(hei - dhei)-1)
			{
				*(ptL + 0 + j * 3) = *(dataL + (*p_mask) +0);
				*(ptL + 1 + j * 3) = *(dataL + (*p_mask) +1);
				*(ptL + 2 + j * 3) = *(dataL + (*p_mask) +2);
				*(ptR + 0 + j * 3) = *(dataR + (*p_mask) + 0);
				*(ptR + 1 + j * 3) = *(dataR + (*p_mask) + 1);
				*(ptR + 2 + j * 3) = *(dataR + (*p_mask) + 2);
			}
			p_mask++;

		}
	}

	transpose(r_imgR, r_imgR);
	flip(r_imgR, r_imgR, 0);
	transpose(r_imgL, r_imgL);
	flip(r_imgL, r_imgL, 1);

	//	transpose(imgL,imgL);
	//	transpose(imgR,imgR);
	/*	if(imgL.size().height == r_imgL.size().width){

	  printf("YEs!!\n");
	   }
	else{

	  printf("ima:%d, r_img:%d",imgL.size().width,r_imgL.size().width);
	  printf("dfdfdfdf!!!!!!\n");
	  
	  }*/

	Mat dis = C_adjust(r_imgL, r_imgR);
	return dis;



}

Mat dcolorComb::C_adjust(Mat & inputL, Mat & inputR)
{
	int dh = int(((float)hei- dhei*2)*hor_rate);
	int dw = int((float)wid*ver_rate);
	inputL = Mat(inputL, Rect(dw, dh+ dhei, wid/2-2*dw, hei - 2 * (dh+dhei)));
	inputR = Mat(inputR, Rect(dw, dh + dhei, wid/2 - 2 * dw, hei - 2 * (dh + dhei)));
	
	int totalCols = inputL.cols + inputL.cols;
	Mat mergedDescriptors(inputL.rows, totalCols, inputL.type());
	Mat submat = mergedDescriptors.colRange(0, inputL.cols);
	inputL.copyTo(submat);
	submat = mergedDescriptors.colRange(inputL.cols, totalCols);
	inputR.copyTo(submat);

	return mergedDescriptors;

}

void dcolorComb::process(Mat &out)
{

	out = T_adjust(this->src);

}
