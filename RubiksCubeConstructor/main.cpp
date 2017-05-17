#include <opencv2/opencv.hpp>
#include <opencv2/core/core.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/videoio/videoio.hpp>
#include <opencv2/cudaarithm.hpp>
#include <opencv2/cudaimgproc.hpp>
#include <opencv2/cudafilters.hpp>
#include <opencv2/cudacodec.hpp>
#include <opencv2/core/cuda.hpp>
//#include <opencv2/core/private.cuda.hpp>
#include <opencv2/core/cuda_types.hpp>
#include <opencv2/photo/cuda.hpp>

#include <iostream>
#include <chrono>
#include <vector>

#include "Cube.h"

const char * WIN_VIDEO = "Video Feed";
const char * WIN_TEST = "Test";
const char * WIN_TEST1 = "Test 1";

int thresh = 50, N = 11;

// helper function:
// finds a cosine of angle between vectors
// from pt0->pt1 and from pt0->pt2
static double angle(cv::Point pt1, cv::Point pt2, cv::Point pt0)
{
	double dx1 = pt1.x - pt0.x;
	double dy1 = pt1.y - pt0.y;
	double dx2 = pt2.x - pt0.x;
	double dy2 = pt2.y - pt0.y;
	return (dx1*dx2 + dy1*dy2) / sqrt((dx1*dx1 + dy1*dy1)*(dx2*dx2 + dy2*dy2) + 1e-10);
}

// returns sequence of squares detected on the image.
// the sequence is stored in the specified memory storage
static void findSquares(const cv::Mat & image, std::vector<std::vector<cv::Point> >& squares)
{
	squares.clear();

	cv::Mat pyr, timg, gray0(image.size(), CV_8U), gray;

	// down-scale and upscale the image to filter out the noise
	pyrDown(image, pyr, cv::Size(image.cols / 2, image.rows / 2));
	pyrUp(pyr, timg, image.size());
	std::vector<std::vector<cv::Point> > contours;
	std::vector<cv::Vec4i> hierarchy;

	// find squares in every color plane of the image
	//mixChannels(&timg, 1, &gray0, 1, ch, 1);
	//GaussianBlur(timg, timg, Size(7, 7), 20, 20, BORDER_DEFAULT);
	
	cvtColor(timg, gray0, CV_BGR2GRAY);

	// Apply theshold
	//cv::threshold(gray0, gray0, thresh, 255, CV_THRESH_TRUNC);
	cv::imshow(WIN_TEST1, gray0);

	cv::Mat imageHSV;
	cvtColor(timg, imageHSV, CV_BGR2HSV);

	// Denoise using cuda
	cv::cuda::GpuMat gray0Device, gray0DeviceDst;
	gray0Device.upload(gray0);
	cv::cuda::fastNlMeansDenoising(gray0Device, gray0DeviceDst, 10);
	gray0DeviceDst.download(gray0);
	//fastNlMeansDenoising(gray0, gray0);
	
	// try several threshold levels
	//for (int l = 0; l < N; l++)
	//{
	// hack: use Canny instead of zero threshold level.
	// Canny helps to catch squares with gradient shading
	Canny(gray0, gray, 0, thresh, 5);
	//dilate(gray, gray, Mat(), Point(-1, -1));
	imshow(WIN_VIDEO, gray);
	

	// find contours and store them all as a list
	findContours(gray, contours, hierarchy, cv::RETR_LIST, cv::CHAIN_APPROX_SIMPLE);

	std::vector<cv::Point> approx;

	// test each contour
	//std::cout << contours.size() << std::endl;
	for (size_t i = 0; i < contours.size(); i++)
	{
		// approximate contour with accuracy proportional
		// to the contour perimeter
		approxPolyDP(cv::Mat(contours[i]), approx, arcLength(cv::Mat(contours[i]), true)*0.02, true);
		
		// square contours should have 4 vertices after approximation
		// relatively large area (to filter out noisy contours)
		// and be convex.
		// Note: absolute value of an area is used because
		// area may be positive or negative - in accordance with the
		// contour orientation
		if (approx.size() == 4 &&
			fabs(cv::contourArea(cv::Mat(approx))) > 100
			&& cv::isContourConvex(cv::Mat(approx)))
		{
			double maxCosine = 0;

			for (int j = 2; j < 5; j++)
			{
				// find the maximum cosine of the angle between joint edges
				double cosine = fabs(angle(approx[j % 4], approx[j - 2], approx[j - 1]));
				maxCosine = MAX(maxCosine, cosine);
			}

			// if cosines of all angles are small
			// (all angles are ~90 degree) then write quandrange
			// vertices to resultant sequence
			//if (maxCosine < 0.5)
				squares.push_back(approx);
		}
	}
}


// the function draws all the squares in the image
static void drawSquares(cv::Mat& image, const std::vector<std::vector<cv::Point> >& squares)
{
	for (size_t i = 0; i < squares.size(); i++)
	{
		const cv::Point* p = &squares[i][0];
		int n = (int)squares[i].size();
		polylines(image, &p, &n, 1, true, cv::Scalar(0, 255, 0), 3, cv::LINE_AA);
	}

	imshow(WIN_TEST, image);
}

int main(int arg, char ** argv) {

	Cube * goalCube = new Cube(3);
	Cube * cube = new Cube(3);
	cube->shuffle(20);
	std::cout << cube->getHeuristic(HeuristicType::MISPLACED, *goalCube) << std::endl;
	while (true)
	{

	}
	char keyIn;

	cv::VideoCapture cam(0);

	cv::Size camS = cv::Size((int)cam.get(cv::CAP_PROP_FRAME_WIDTH), (int)cam.get(cv::CAP_PROP_FRAME_HEIGHT));
	int ex = static_cast<int>(cam.get(cv::CAP_PROP_FOURCC));

	// Windows
	cv::namedWindow(WIN_VIDEO, cv::WINDOW_AUTOSIZE);
	cv::moveWindow(WIN_VIDEO, 0, 0);
	cv::namedWindow(WIN_TEST, cv::WINDOW_AUTOSIZE);
	cv::moveWindow(WIN_TEST, 660, 0);
	cv::namedWindow(WIN_TEST1, cv::WINDOW_AUTOSIZE);
	cv::moveWindow(WIN_TEST1, 1220, 0);
	

	cv::Mat curFrame, curFrameGray, curFrameBlurred, curGrad;

	int scale = 1, delta = 0, ddepth = CV_16S;

	cv::Mat kernelDilation = cv::getStructuringElement(cv::MORPH_RECT, cv::Size(7, 7));

	while (true)
	{
		cam >> curFrame;
		if (curFrame.empty())
		{
			std::cout << "No input";
			break;
		}

		// Reduce noise amd convert to gray
		GaussianBlur(curFrame, curFrameBlurred, cv::Size(5, 5), 0, 0, cv::BORDER_DEFAULT);
		cvtColor(curFrameBlurred, curFrameGray, CV_BGR2GRAY);

		// Generate gradX and gradY
		cv::Mat gradX, gradY;
		cv::Mat absGradX, absGradY;

		// Gradient X
		//Scharr( src_gray, grad_x, ddepth, 1, 0, scale, delta, BORDER_DEFAULT );
		cv::Sobel(curFrameGray, gradX, ddepth, 1, 0, 3, scale, delta, cv::BORDER_DEFAULT);
		cv::convertScaleAbs(gradX, absGradX);

		// Gradient Y
		//Scharr( src_gray, grad_y, ddepth, 0, 1, scale, delta, BORDER_DEFAULT );
		Sobel(curFrameGray, gradY, ddepth, 0, 1, 3, scale, delta, cv::BORDER_DEFAULT);
		convertScaleAbs(gradY, absGradY);

		addWeighted(absGradX, 0.5, absGradY, 0.5, 0, curGrad);

		//dilate(curGrad, curGrad, kernelDilation);

		//imshow(WIN_TEST, curGrad);

		std::vector<std::vector<cv::Point> > squares;

		findSquares(curFrame, squares);
		drawSquares(curFrame, squares);

		

		keyIn = (char)cv::waitKey(1);
		if (keyIn == 27)
		{
			break;
		}
	}

	return 0;
}