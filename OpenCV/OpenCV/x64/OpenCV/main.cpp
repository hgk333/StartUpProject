#include "cv.h"
#include "highgui.h"
#include <stdio.h>
#include <stdlib.h>


void PrintMat(const CvMat *mat, const char *strName);

typedef struct rect{
	CvPoint min;
	CvPoint max;
	int width;
	int height;
	rect* next;
}rect;

void init_rect(rect* init){
	init->min.x = 10000;
	init->min.y = 10000;
	init->max.x = 0;
	init->max.y = 0;
	init->width = 0;
	init->height = 0;
	init->next=NULL ;
}

int main()
{
	rect* rectData,* rectData_link;
	rectData = rectData_link = NULL;
	IplImage    *srcImage;
	if ((srcImage = cvLoadImage("number.jpg", CV_LOAD_IMAGE_GRAYSCALE)) == NULL)
		return -1;
	IplImage* dstImage = cvCreateImage(cvGetSize(srcImage), IPL_DEPTH_8U, 3);

	CvMemStorage* storage = cvCreateMemStorage(0);
	CvSeq *first_contour = 0;
	int header_size = sizeof(CvContour);

	int mode = CV_RETR_LIST;
	//	int method=CV_CHAIN_APPROX_NONE;
	int method = CV_CHAIN_APPROX_SIMPLE;

	CvPoint offset = cvPoint(0, 0);

	cvThreshold(srcImage, srcImage, 200, 255, CV_THRESH_BINARY);
	cvCvtColor(srcImage, dstImage, CV_GRAY2BGR);

	int nContour = cvFindContours(srcImage, storage, &first_contour, header_size,
		mode, method, offset);
	printf("nContour=%d\n", nContour);

	CvScalar external_color = CV_RGB(0, 0, 255);
	CvScalar hole_color = CV_RGB(255, 0, 0);
	int max_level = 0;

	int i, k;
	CvSeq* contour;

	rectData = rectData_link = (rect*)malloc(sizeof(rect));

	init_rect(rectData_link);

	for (contour = first_contour, k = 0; contour != 0; contour = contour->h_next, k++)
	{

		cvDrawContours(dstImage, contour, CV_RGB(0, 0, 255), CV_RGB(255, 0, 0), 0, 2);
		
		for (i = 0; i<contour->total; i++)
		{
			CvPoint* pt = (CvPoint*)cvGetSeqElem(contour, i);
			if (rectData_link->max.x < pt->x)
				rectData_link->max.x = pt->x;
			if (rectData_link->max.y < pt->y)
				rectData_link->max.y = pt->y;
			if (rectData_link->min.x > pt->x)
				rectData_link->min.x = pt->x;
			if (rectData_link->min.y > pt->y)
				rectData_link->min.y = pt->y;
			//printf("pt[%d] = (%d, %d)\n", i, pt->x, pt->y);
			
		}/*		if (k != 3){
			printf("\nmin pt[%d] = (%d, %d)\n", i, min.x, min.y);
			printf("\nmax pt[%d] = (%d, %d)\n", i, max.x, max.y);
			cvRectangle(dstImage, min, max, CV_RGB(255, 255, 0), 1, 8, 0);
		
			CvMat matHeader, *pSrcMat;
			cvSetImageROI(srcImage, cvRect(min.x,min.y,max.x-min.x,max.y-min.y));
			pSrcMat = cvGetMat(srcImage, &matHeader);
			PrintMat(pSrcMat, "pSrcMat");
			min.x = 1000; min.y = 1000;
			max.x = 0; max.y = 0;
		}*/
		rectData_link->width = rectData_link->max.x - rectData_link->min.x;
		rectData_link->height = rectData_link->max.y - rectData_link->min.y;
		if (rectData == rectData_link){
			rectData->next = (rect*)malloc(sizeof(rect));
			rectData_link = rectData->next;
		}
		else{
			rectData_link->next = (rect*)malloc(sizeof(rect));
			rectData_link = rectData_link->next;
		}

		init_rect(rectData_link);
	}

	int innerContourCount = 0;
	
	rect * temp1,*temp2 , *temp3=NULL;
	temp1 = rectData;
	while(temp1){
		temp2 = rectData;
		innerContourCount = 0;
		while (temp2){
			if (temp1->max.x > temp2->max.x && temp1->max.y > temp2->max.y && temp1->min.x < temp2->min.x && temp1->min.y < temp2->min.y){
				innerContourCount++;
				if (innerContourCount == 1){
					temp3 = temp2;
				}
			}

			temp2=temp2->next;
		}
		if (innerContourCount == 3){
			printf("\n8 = min(%d,%d)\tmax(%d,%d)\n\n", temp1->min.x, temp1->min.y, temp1->max.x, temp1->max.y);

		}
		else if (innerContourCount == 2){
			int y1,y3;
			y1 = (temp1->max.y + temp1->min.y) / 2;
			y3 = (temp3->max.y + temp3->min.y) / 2;
			printf("1 = min(%d,%d)\tmax(%d,%d)\n", temp1->min.x, temp1->min.y, temp1->max.x, temp1->max.y);
			printf("3 = min(%d,%d)\tmax(%d,%d)\n", temp3->min.x, temp3->min.y, temp3->max.x, temp3->max.y);
			printf("%d\t%d\t%d\t%d\n", y1, y3,temp1->height,temp3->height);
			
			if (y1<y3)
				printf("\n9 = min(%d,%d)\tmax(%d,%d)\n\n", temp1->min.x, temp1->min.y, temp1->max.x, temp1->max.y);
			else if (y1>y3 && temp3->min.y < 45)
				printf("\n6 = min(%d,%d)\tmax(%d,%d)\n\n", temp1->min.x, temp1->min.y, temp1->max.x, temp1->max.y);
			else if (temp3->height>temp1->height/2)
				printf("\n0 = min(%d,%d)\tmax(%d,%d)\n\n", temp1->min.x, temp1->min.y, temp1->max.x, temp1->max.y);
			else 
				printf("\n4 = min(%d,%d)\tmax(%d,%d)\n\n", temp1->min.x, temp1->min.y, temp1->max.x, temp1->max.y);
			
		}


		temp1 = temp1->next;
	}

	
	cvNamedWindow("dstImage", CV_WINDOW_AUTOSIZE);
	cvShowImage("dstImage", dstImage);

	cvWaitKey(0);
	cvDestroyAllWindows();
	cvReleaseMemStorage(&storage);
	cvReleaseImage(&srcImage);
	cvReleaseImage(&dstImage);
	return 0;
}

void PrintMat(const CvMat *mat, const char *strName)
{
	int   x, y;
	double   fValue;
	printf(" %s  \n =  \n", strName);
	for (y = 0; y<mat->rows; y++)
	{
		for (x = 0; x<mat->cols; x++)
		{
			fValue = cvGetReal2D(mat, y, x);
			printf("%3d", cvRound(fValue));
		}
		printf("\n");
	}
	printf("\n\n");
}