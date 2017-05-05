#include <opencv2/opencv.hpp>
#include <opencv2/core/core.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/videoio/videoio.hpp>

#include <iostream>
#include <vector>

using namespace std;
using namespace cv;

const char * WIN_VIDEO = "Video Feed";
const char * WIN_TEST = "Test";

int main(int arg, char ** argv) {

	char keyIn;

	VideoCapture cam(0);

	Size camS = Size((int)cam.get(CAP_PROP_FRAME_WIDTH), (int)cam.get(CAP_PROP_FRAME_HEIGHT));
	int ex = static_cast<int>(cam.get(CAP_PROP_FOURCC));

	// Windows
	namedWindow(WIN_VIDEO, WINDOW_AUTOSIZE);
	moveWindow(WIN_VIDEO, 0, 0);
	namedWindow(WIN_TEST, WINDOW_AUTOSIZE);
	moveWindow(WIN_TEST, 650, 0);

	Mat curFrame, curFrameGray, curFrameBlurred, curGrad;

	int scale = 1, delta = 0, ddepth = CV_16S;

	Mat kerneErosion = getStructuringElement(MORPH_RECT, Size(10, 1));


	while (true)
	{
		cam >> curFrame;
		if (curFrame.empty())
		{
			cout << "No input";
			break;
		}

		// Reduce noise amd conver to gray
		GaussianBlur(curFrame, curFrameBlurred, Size(5, 5), 0, 0, BORDER_DEFAULT);
		cvtColor(curFrameBlurred, curFrameGray, CV_BGR2GRAY);

		// Generate gradX and gradY
		Mat gradX, gradY;
		Mat absGradX, absGradY;

		// Gradient X
		//Scharr( src_gray, grad_x, ddepth, 1, 0, scale, delta, BORDER_DEFAULT );
		Sobel(curFrameGray, gradX, ddepth, 1, 0, 3, scale, delta, BORDER_DEFAULT);
		convertScaleAbs(gradX, absGradX);

		// Gradient Y
		//Scharr( src_gray, grad_y, ddepth, 0, 1, scale, delta, BORDER_DEFAULT );
		Sobel(curFrameGray, gradY, ddepth, 0, 1, 3, scale, delta, BORDER_DEFAULT);
		convertScaleAbs(gradY, absGradY);

		addWeighted(absGradX, 0.5, absGradY, 0.5, 0, curGrad);

		imshow(WIN_TEST, curGrad);

		imshow(WIN_VIDEO, curFrame);

		keyIn = (char)waitKey(1);
		if (keyIn == 27)
		{
			break;
		}
	}


	return 0;
}