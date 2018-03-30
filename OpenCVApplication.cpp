// OpenCVApplication.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "common.h"
#include <queue>
#include <random>


void testOpenImage()
{
	char fname[MAX_PATH];
	while (openFileDlg(fname))
	{
		Mat src;
		src = imread(fname);
		imshow("image", src);
		waitKey();
	}
}

void testOpenImagesFld()
{
	char folderName[MAX_PATH];
	if (openFolderDlg(folderName) == 0)
		return;
	char fname[MAX_PATH];
	FileGetter fg(folderName, "bmp");
	while (fg.getNextAbsFile(fname))
	{
		Mat src;
		src = imread(fname);
		imshow(fg.getFoundFileName(), src);
		if (waitKey() == 27) //ESC pressed
			break;
	}
}

void testImageOpenAndSave()
{
	Mat src, dst;

	src = imread("Images/Lena_24bits.bmp", CV_LOAD_IMAGE_COLOR);	// Read the image

	if (!src.data)	// Check for invalid input
	{
		printf("Could not open or find the image\n");
		return;
	}

	// Get the image resolution
	Size src_size = Size(src.cols, src.rows);

	// Display window
	const char* WIN_SRC = "Src"; //window for the source image
	namedWindow(WIN_SRC, CV_WINDOW_AUTOSIZE);
	cvMoveWindow(WIN_SRC, 0, 0);

	const char* WIN_DST = "Dst"; //window for the destination (processed) image
	namedWindow(WIN_DST, CV_WINDOW_AUTOSIZE);
	cvMoveWindow(WIN_DST, src_size.width + 10, 0);

	cvtColor(src, dst, CV_BGR2GRAY); //converts the source image to a grayscale one

	imwrite("Images/Lena_24bits_gray.bmp", dst); //writes the destination to file

	imshow(WIN_SRC, src);
	imshow(WIN_DST, dst);

	printf("Press any key to continue ...\n");
	waitKey(0);
}

void testNegativeImage()
{
	char fname[MAX_PATH];
	while (openFileDlg(fname))
	{
		double t = (double)getTickCount(); // Get the current time [s]

		Mat src = imread(fname, CV_LOAD_IMAGE_GRAYSCALE);
		int height = src.rows;
		int width = src.cols;
		Mat dst = Mat(height, width, CV_8UC1);
		// Asa se acceseaaza pixelii individuali pt. o imagine cu 8 biti/pixel
		// Varianta ineficienta (lenta)
		for (int i = 0; i < height; i++)
		{
			for (int j = 0; j < width; j++)
			{
				uchar val = src.at<uchar>(i, j);
				uchar neg = MAX_PATH - val;
				dst.at<uchar>(i, j) = neg;
			}
		}

		// Get the current time again and compute the time difference [s]
		t = ((double)getTickCount() - t) / getTickFrequency();
		// Print (in the console window) the processing time in [ms] 
		printf("Time = %.3f [ms]\n", t * 1000);

		imshow("input image", src);
		imshow("negative image", dst);
		waitKey();
	}
}

void testParcurgereSimplaDiblookStyle()
{
	char fname[MAX_PATH];
	while (openFileDlg(fname))
	{
		Mat src = imread(fname, CV_LOAD_IMAGE_GRAYSCALE);
		int height = src.rows;
		int width = src.cols;
		Mat dst = src.clone();

		double t = (double)getTickCount(); // Get the current time [s]

		// the fastest approach using the “diblook style”
		uchar *lpSrc = src.data;
		uchar *lpDst = dst.data;
		int w = (int)src.step; // no dword alignment is done !!!
		for (int i = 0; i < height; i++)
			for (int j = 0; j < width; j++) {
				uchar val = lpSrc[i*w + j];
				lpDst[i*w + j] = 255 - val;
			}

		// Get the current time again and compute the time difference [s]
		t = ((double)getTickCount() - t) / getTickFrequency();
		// Print (in the console window) the processing time in [ms] 
		printf("Time = %.3f [ms]\n", t * 1000);

		imshow("input image", src);
		imshow("negative image", dst);
		waitKey();
	}
}

void testColor2Gray()
{
	char fname[MAX_PATH];
	while (openFileDlg(fname))
	{
		Mat src = imread(fname);

		int height = src.rows;
		int width = src.cols;

		Mat dst = Mat(height, width, CV_8UC1);

		// Asa se acceseaaza pixelii individuali pt. o imagine RGB 24 biti/pixel
		// Varianta ineficienta (lenta)
		for (int i = 0; i < height; i++)
		{
			for (int j = 0; j < width; j++)
			{
				Vec3b v3 = src.at<Vec3b>(i, j);
				uchar b = v3[0];
				uchar g = v3[1];
				uchar r = v3[2];
				dst.at<uchar>(i, j) = (r + g + b) / 3;
			}
		}

		imshow("input image", src);
		imshow("gray image", dst);
		waitKey();
	}
}

void testBGR2HSV()
{
	char fname[MAX_PATH];
	while (openFileDlg(fname))
	{
		Mat src = imread(fname);
		int height = src.rows;
		int width = src.cols;

		// Componentele d eculoare ale modelului HSV
		Mat H = Mat(height, width, CV_8UC1);
		Mat S = Mat(height, width, CV_8UC1);
		Mat V = Mat(height, width, CV_8UC1);

		// definire pointeri la matricele (8 biti/pixeli) folosite la afisarea componentelor individuale H,S,V
		uchar* lpH = H.data;
		uchar* lpS = S.data;
		uchar* lpV = V.data;

		Mat hsvImg;
		cvtColor(src, hsvImg, CV_BGR2HSV);

		// definire pointer la matricea (24 biti/pixeli) a imaginii HSV
		uchar* hsvDataPtr = hsvImg.data;

		for (int i = 0; i < height; i++)
		{
			for (int j = 0; j < width; j++)
			{
				int hi = i * width * 3 + j * 3;
				int gi = i * width + j;

				lpH[gi] = hsvDataPtr[hi] * 510 / 360;		// lpH = 0 .. 255
				lpS[gi] = hsvDataPtr[hi + 1];			// lpS = 0 .. 255
				lpV[gi] = hsvDataPtr[hi + 2];			// lpV = 0 .. 255
			}
		}

		imshow("input image", src);
		imshow("H", H);
		imshow("S", S);
		imshow("V", V);

		waitKey();
	}
}

void testResize()
{
	char fname[MAX_PATH];
	while (openFileDlg(fname))
	{
		Mat src;
		src = imread(fname);
		Mat dst1, dst2;
		//without interpolation
		resizeImg(src, dst1, 320, false);
		//with interpolation
		resizeImg(src, dst2, 320, true);
		imshow("input image", src);
		imshow("resized image (without interpolation)", dst1);
		imshow("resized image (with interpolation)", dst2);
		waitKey();
	}
}

void testCanny()
{
	char fname[MAX_PATH];
	while (openFileDlg(fname))
	{
		Mat src, dst, gauss;
		src = imread(fname, CV_LOAD_IMAGE_GRAYSCALE);
		double k = 0.4;
		int pH = 50;
		int pL = (int)k*pH;
		GaussianBlur(src, gauss, Size(5, 5), 0.8, 0.8);
		Canny(gauss, dst, pL, pH, 3);
		imshow("input image", src);
		imshow("canny", dst);
		waitKey();
	}
}

void testVideoSequence()
{
	VideoCapture cap("Videos/rubic.avi"); // off-line video from file
	//VideoCapture cap(0);	// live video from web cam
	if (!cap.isOpened()) {
		printf("Cannot open video capture device.\n");
		waitKey(0);
		return;
	}

	Mat edges;
	Mat frame;
	char c;

	while (cap.read(frame))
	{
		Mat grayFrame;
		cvtColor(frame, grayFrame, CV_BGR2GRAY);
		Canny(grayFrame, edges, 40, 100, 3);
		imshow("source", frame);
		imshow("gray", grayFrame);
		imshow("edges", edges);
		c = cvWaitKey(0);  // waits a key press to advance to the next frame
		if (c == 27) {
			// press ESC to exit
			printf("ESC pressed - capture finished\n");
			break;  //ESC pressed
		};
	}
}


void testSnap()
{
	VideoCapture cap(0); // open the deafult camera (i.e. the built in web cam)
	if (!cap.isOpened()) // openenig the video device failed
	{
		printf("Cannot open video capture device.\n");
		return;
	}

	Mat frame;
	char numberStr[256];
	char fileName[256];

	// video resolution
	Size capS = Size((int)cap.get(CV_CAP_PROP_FRAME_WIDTH),
		(int)cap.get(CV_CAP_PROP_FRAME_HEIGHT));

	// Display window
	const char* WIN_SRC = "Src"; //window for the source frame
	namedWindow(WIN_SRC, CV_WINDOW_AUTOSIZE);
	cvMoveWindow(WIN_SRC, 0, 0);

	const char* WIN_DST = "Snapped"; //window for showing the snapped frame
	namedWindow(WIN_DST, CV_WINDOW_AUTOSIZE);
	cvMoveWindow(WIN_DST, capS.width + 10, 0);

	char c;
	int frameNum = -1;
	int frameCount = 0;

	for (;;)
	{
		cap >> frame; // get a new frame from camera
		if (frame.empty())
		{
			printf("End of the video file\n");
			break;
		}

		++frameNum;

		imshow(WIN_SRC, frame);

		c = cvWaitKey(10);  // waits a key press to advance to the next frame
		if (c == 27) {
			// press ESC to exit
			printf("ESC pressed - capture finished");
			break;  //ESC pressed
		}
		if (c == 115) { //'s' pressed - snapp the image to a file
			frameCount++;
			fileName[0] = NULL;
			sprintf(numberStr, "%d", frameCount);
			strcat(fileName, "Images/A");
			strcat(fileName, numberStr);
			strcat(fileName, ".bmp");
			bool bSuccess = imwrite(fileName, frame);
			if (!bSuccess)
			{
				printf("Error writing the snapped image\n");
			}
			else
				imshow(WIN_DST, frame);
		}
	}

}

void MyCallBackFunc(int event, int x, int y, int flags, void* param)
{
	//More examples: http://opencvexamples.blogspot.com/2014/01/detect-mouse-clicks-and-moves-on-image.html
	Mat* src = (Mat*)param;
	if (event == CV_EVENT_LBUTTONDOWN)
	{
		printf("Pos(x,y): %d,%d  Color(RGB): %d,%d,%d\n",
			x, y,
			(int)(*src).at<Vec3b>(y, x)[2],
			(int)(*src).at<Vec3b>(y, x)[1],
			(int)(*src).at<Vec3b>(y, x)[0]);
	}
}

void testMouseClick()
{
	Mat src;
	// Read image from file 
	char fname[MAX_PATH];
	while (openFileDlg(fname))
	{
		src = imread(fname);
		//Create a window
		namedWindow("My Window", 1);

		//set the callback function for any mouse event
		setMouseCallback("My Window", MyCallBackFunc, &src);

		//show the image
		imshow("My Window", src);

		// Wait until user press some key
		waitKey(0);
	}
}

/* Histogram display function - display a histogram using bars (simlilar to L3 / PI)
Input:
name - destination (output) window name
hist - pointer to the vector containing the histogram values
hist_cols - no. of bins (elements) in the histogram = histogram image width
hist_height - height of the histogram image
Call example:
showHistogram ("MyHist", hist_dir, 255, 200);
*/
void showHistogram(const std::string& name, int* hist, const int  hist_cols, const int hist_height)
{
	Mat imgHist(hist_height, hist_cols, CV_8UC3, CV_RGB(255, 255, 255)); // constructs a white image

	//computes histogram maximum
	int max_hist = 0;
	for (int i = 0; i < hist_cols; i++)
		if (hist[i] > max_hist)
			max_hist = hist[i];
	double scale = 1.0;
	scale = (double)hist_height / max_hist;
	int baseline = hist_height - 1;

	for (int x = 0; x < hist_cols; x++) {
		Point p1 = Point(x, baseline);
		Point p2 = Point(x, baseline - cvRound(hist[x] * scale));
		line(imgHist, p1, p2, CV_RGB(255, 0, 255)); // histogram bins colored in magenta
	}

	imshow(name, imgHist);
}

/******************************************************
						 LAB 1
*******************************************************/

void createImage() {
	//simple image
	Mat img = Mat::zeros(200, 200, CV_8UC1);
	imshow("Image", img);
	waitKey(0);
}

void upperRows() {
	//upper rows dark
	Mat img = Mat::zeros(200, 200, CV_8UC1);
	for (int i = 0; i < 50; i++) {
		for (int j = 0; j < img.cols; j++) {
			img.at<uchar>(i, j) = 200;
		}
	}
	imshow("Square", img);
	waitKey(0);
}

void whiteSquare() {
	//white square in middle
	Mat img = Mat::zeros(200, 200, CV_8UC1);
	for (int i = 50; i < 150; i++) {
		for (int j = 50; j < 150; j++) {
			img.at<uchar>(i, j) = 255;
		}
	}
	imshow("White square", img);
	waitKey(0);
}

void negativeImage() {
	//image negative
	char fname[MAX_PATH];
	while (openFileDlg(fname)) {
		Mat src = imread(fname, CV_LOAD_IMAGE_GRAYSCALE);
		Mat dst;
		src.copyTo(dst);
		for (int i = 0; i < src.rows; i++) {
			for (int j = 0; j < src.cols; j++) {
				dst.at<uchar>(i, j) = 255 - src.at<uchar>(i, j);
			}
		}
		imshow("SRC", src);
		imshow("DST", dst);
		waitKey(0);
	}
}

void modifyBrightness(int val) {
	//modify brightness
	char fname[MAX_PATH];
	while (openFileDlg(fname)) {
		Mat src = imread(fname, CV_LOAD_IMAGE_GRAYSCALE);
		Mat dst1;
		Mat dst2;
		src.copyTo(dst1);
		src.copyTo(dst2);
		for (int i = 0; i < src.rows; i++) {
			for (int j = 0; j < src.cols; j++) {

				if (dst1.at<uchar>(i, j) + val > 255)
					dst1.at<uchar>(i, j) = 255;
				else
					dst1.at<uchar>(i, j) = src.at<uchar>(i, j) + val;

				if (dst2.at<uchar>(i, j) - val < 0)
					dst2.at<uchar>(i, j) = 0;
				else
					dst2.at<uchar>(i, j) = src.at<uchar>(i, j) - val;
			}
		}
		imshow("SRC", src);
		imshow("DST1", dst1);
		imshow("DST2", dst2);
		waitKey(0);
	}
}

void createColorImage() {
	//color image
	Mat src1 = Mat::zeros(300, 300, CV_8UC3);
	Vec3b pixel;
	//up left red
	pixel[0] = 0;
	pixel[1] = 0;
	pixel[2] = 200;
	for (int i = 0; i < src1.rows / 2; i++) {
		for (int j = 0; j < src1.cols / 2; j++) {
			src1.at<Vec3b>(i, j) = pixel;
		}
	}
	//down left blue
	pixel[0] = 200;
	pixel[1] = 0;
	pixel[2] = 0;
	for (int i = src1.rows / 2; i < src1.rows; i++) {
		for (int j = 0; j < src1.cols / 2; j++) {
			src1.at<Vec3b>(i, j) = pixel;
		}
	}
	//up right green
	pixel[0] = 0;
	pixel[1] = 200;
	pixel[2] = 0;
	for (int i = 0; i < src1.rows / 2; i++) {
		for (int j = src1.cols / 2; j < src1.cols; j++) {
			src1.at<Vec3b>(i, j) = pixel;
		}
	}
	//down right yellow
	pixel[0] = 0;
	pixel[1] = 200;
	pixel[2] = 200;
	for (int i = src1.rows / 2; i < src1.rows; i++) {
		for (int j = src1.cols / 2; j < src1.cols; j++) {
			src1.at<Vec3b>(i, j) = pixel;
		}
	}
	imshow("Color", src1);
	waitKey(0);
}

void negativeColorImage() {
	//negative of a color image
	char fname[MAX_PATH];
	while (openFileDlg(fname)) {
		Mat src2 = imread(fname);
		Mat dst2;
		src2.copyTo(dst2);
		Vec3b pixel;
		pixel[0] = 255;
		pixel[1] = 255;
		pixel[2] = 255;
		for (int i = 0; i < src2.rows; i++) {
			for (int j = 0; j < src2.cols; j++) {
				dst2.at<Vec3b>(i, j) = pixel - src2.at<Vec3b>(i, j);
			}
		}
		imshow("SRC", src2);
		imshow("DST", dst2);
		waitKey(0);
	}
}

void modifyColorImageBrightness(int val) {
	//color image brightness
	char fname[MAX_PATH];
	while (openFileDlg(fname)) {
		Mat src = imread(fname);
		Mat dst1;
		Mat dst2;
		src.copyTo(dst1);
		src.copyTo(dst2);

		for (int i = 0; i < src.rows; i++) {
			for (int j = 0; j < src.cols; j++) {
				for (int k = 0; k < 3; k++) {
					if (dst1.at<Vec3b>(i, j)[k] + val > 255)
						dst1.at<Vec3b>(i, j)[k] = 255;
					else
						dst1.at<Vec3b>(i, j)[k] = src.at<Vec3b>(i, j)[k] + val;

					if (dst2.at<Vec3b>(i, j)[k] - val < 0)
						dst2.at<Vec3b>(i, j)[k] = 0;
					else
						dst2.at<Vec3b>(i, j)[k] = src.at<Vec3b>(i, j)[k] - val;
				}
			}
		}
		imshow("SRC", src);
		imshow("DST1", dst1);
		imshow("DST2", dst2);
		waitKey(0);
	}
}

void matrixInverse() {
	//matrix inverse
	float vals[] = { 1,7,9,10,5,3,2,20,7 };
	Mat m = Mat(3, 3, CV_32FC1, vals);
	std::cout << "Matrix:" << m << std::endl;
	getchar();
	Mat rez = m.inv();
	std::cout << "Result" << rez << std::endl;
	getchar();
}

/******************************************************
						  LAB 2
*******************************************************/

//1. color to grayscale by r,g,b
void channelGrayscale() {
	char fname[MAX_PATH];
	while (openFileDlg(fname)) {
		Mat src = imread(fname);
		Mat dstR = Mat(src.rows, src.cols, CV_8UC1);
		Mat dstG = Mat(src.rows, src.cols, CV_8UC1);
		Mat dstB = Mat(src.rows, src.cols, CV_8UC1);
		for (int i = 0; i < src.rows; i++) {
			for (int j = 0; j < src.cols; j++) {
				dstR.at<uchar>(i, j) = src.at<Vec3b>(i, j)[2];
				dstG.at<uchar>(i, j) = src.at<Vec3b>(i, j)[1];
				dstB.at<uchar>(i, j) = src.at<Vec3b>(i, j)[0];
			}
		}
		imshow("SRC", src);
		imshow("DSTR", dstR);
		imshow("DSTG", dstG);
		imshow("DSTB", dstB);
		waitKey(0);
	}
}

//2. color to grayscale
void colorToGrayscale() {
	char fname[MAX_PATH];
	while (openFileDlg(fname)) {
		Mat src = imread(fname, CV_LOAD_IMAGE_COLOR);
		Mat dst = Mat(src.rows, src.cols, CV_8UC1);
		for (int i = 0; i < src.rows; i++) {
			for (int j = 0; j < src.cols; j++) {
				dst.at<uchar>(i, j) = (src.at<Vec3b>(i, j)[2] + src.at<Vec3b>(i, j)[1] + src.at<Vec3b>(i, j)[0]) / 3;
			}
		}
		imshow("SRC", src);
		imshow("DST", dst);
		waitKey(0);
	}
}

//3. grayscale to black and white
void grayscaleToBW() {
	char fname[MAX_PATH];
	while (openFileDlg(fname)) {
		Mat src = imread(fname, CV_LOAD_IMAGE_GRAYSCALE);
		Mat dst = Mat(src.rows, src.cols, CV_8UC1);

		int th;
		printf("Treshold: ");
		scanf("%d", &th);

		for (int i = 0; i < src.rows; i++) {
			for (int j = 0; j < src.cols; j++) {
				if (src.at<uchar>(i, j) < th) {
					dst.at<uchar>(i, j) = 0;
				}
				else {
					dst.at<uchar>(i, j) = 255;
				}

			}
		}
		imshow("SRC", src);
		imshow("DST", dst);
		waitKey(0);
	}
}

//4. RGB to HSV
void rgbToHsv() {
	char fname[MAX_PATH];
	while (openFileDlg(fname)) {
		Mat src = imread(fname);
		Mat dstH = Mat(src.rows, src.cols, CV_8UC1);
		Mat dstS = Mat(src.rows, src.cols, CV_8UC1);
		Mat dstV = Mat(src.rows, src.cols, CV_8UC1);

		float r, g, b;
		float m, M, C;
		float h, s, v;

		for (int i = 0; i < src.rows; i++) {
			for (int j = 0; j < src.cols; j++) {
				r = (float)src.at<Vec3b>(i, j)[2] / 255;
				g = (float)src.at<Vec3b>(i, j)[1] / 255;
				b = (float)src.at<Vec3b>(i, j)[0] / 255;

				M = max(r, max(g, b));
				m = min(r, min(g, b));

				C = M - m;

				//Value
				v = M;

				//Saturation
				if (C) {
					s = C / v;
				}
				else {
					s = 0;
				}

				//Hue
				if (C) {
					if (M == r) {
						h = 60 * (g - b) / C;
					}
					if (M == g) {
						h = 120 + 60 * (b - r) / C;
					}
					if (M == b) {
						h = 240 + 60 * (r - g) / C;
					}
				}
				else {
					h = 0;
				}
				if (h < 0) {
					h += 360;
				}

				dstH.at<uchar>(i, j) = h * 255 / 360;
				dstS.at<uchar>(i, j) = s * 255;
				dstV.at<uchar>(i, j) = v * 255;
			}
		}
		imshow("SRC", src);
		imshow("DSTH", dstH);
		imshow("DSTS", dstS);
		imshow("DSTV", dstV);
		waitKey(0);
	}
}

//5**. filter out values in an interval
void filterHSV() {
	char fname[MAX_PATH];
	while (openFileDlg(fname)) {
		Mat src = imread(fname);
		Mat dst = Mat(src.rows, src.cols, CV_8UC1);

		float r, g, b;
		float m, M, C;
		float h, s, v;

		for (int i = 0; i < src.rows; i++) {
			for (int j = 0; j < src.cols; j++) {
				r = (float)src.at<Vec3b>(i, j)[2] / 255;
				g = (float)src.at<Vec3b>(i, j)[1] / 255;
				b = (float)src.at<Vec3b>(i, j)[0] / 255;

				M = max(r, max(g, b));
				m = min(r, min(g, b));

				C = M - m;

				//Value
				v = M;

				//Saturation
				if (C) {
					s = C / v;
				}
				else {
					s = 0;
				}

				//Hue
				if (C) {
					if (M == r) {
						h = 60 * (g - b) / C;
					}
					if (M == g) {
						h = 120 + 60 * (b - r) / C;
					}
					if (M == b) {
						h = 240 + 60 * (r - g) / C;
					}
				}
				else {
					h = 0;
				}
				if (h < 0) {
					h += 360;
				}

				//everything that is yellow will be white
				if (h >= 50 && h <= 70 && s > 0.3) {
					dst.at<uchar>(i, j) = 255;
				}
				else {
					dst.at<uchar>(i, j) = 0;
				}
			}
		}
		imshow("SRC", src);
		imshow("DST", dst);
		waitKey(0);
	}
}

//Lab 3 was skipped

/******************************************************
					  LAB 4
*******************************************************/

//1-> Compute geometric features of a clicked object callback function
void objectClickedCallback(int event, int x, int y, int flags, void* param)
{
	Mat* src = (Mat*)param;
	if (event == CV_EVENT_LBUTTONDOWN)
	{
		//The clicked point
		Vec3b pixel;
		pixel[2] = (int)(*src).at<Vec3b>(y, x)[2];
		pixel[1] = (int)(*src).at<Vec3b>(y, x)[1];
		pixel[0] = (int)(*src).at<Vec3b>(y, x)[0];

		int area = 0;
		int rsum = 0;
		int csum = 0;
		int perimeter = 0;
		int rmin = src->rows, rmax = 0, cmin = src->cols, cmax = 0;

		//Color of the background
		Vec3b background = Vec3b(255, 255, 255);

		//Create the new display window
		Mat projection = Mat(src->rows, src->cols, CV_8UC3);
		for (int i = 0; i < src->rows; i++) {
			for (int j = 0; j < src->cols; j++) {
				projection.at<Vec3b>(i, j) = background;
			}
		}

		if (pixel != background) {
			printf("Object detected!\n");
			for (int i = 0; i < src->rows; i++) {
				for (int j = 0; j < src->cols; j++) {
					if (src->at<Vec3b>(i, j) == pixel) {
						//Area                 
						area++;
						//Center of mass
						rsum += i;
						csum += j;

						//Display the object
						projection.at<Vec3b>(i, j) = src->at<Vec3b>(i, j);

						//Perimeter
						if ((i == 0) || (j == 0) || (i == src->rows - 1) || (j == src->cols - 1) ||
							(src->at<Vec3b>(i - 1, j) == background) || (src->at<Vec3b>(i + 1, j) == background) ||
							(src->at<Vec3b>(i, j - 1) == background) || (src->at<Vec3b>(i, j + 1) == background)) {
							perimeter++;

							//Contour points
							projection.at<Vec3b>(i, j) = Vec3b(0, 0, 255);
						}

						//Aspect ratio
						if (i < rmin) {
							rmin = i;
						}
						if (i > rmax) {
							rmax = i;
						}
						if (j < cmin) {
							cmin = j;
						}
						if (j > rmax) {
							cmax = j;
						}
					}
				}
			}
			//Some printing
			printf("Area: %d\n", area);
			printf("Perimeter: %d\n", perimeter);
			rsum /= area;
			csum /= area;
			printf("Center of mass: (%d, %d)\n", rsum, csum);

			//Thinnes and aspect ratio
			float thinnessRatio, aspectRatio;
			thinnessRatio = 4 * PI*(area / pow(perimeter, 2));
			aspectRatio = (float)(cmax - cmin + 1) / (rmax - rmin + 1);
			printf("Thinness ratio: %f\n", thinnessRatio);
			printf("Aspect ratio: %f\n", aspectRatio);

			float nominator = 0;
			float denominator = 0;
			float angle;


			//Some computations
			int projRows[1000] = { 0 };
			int projCols[1000] = { 0 };
			for (int i = 0; i < src->rows; i++) {
				for (int j = 0; j < src->cols; j++) {
					if (src->at<Vec3b>(i, j) == pixel) {
						//Axis elongation
						nominator += (i - rsum)*(j - csum);
						denominator += pow((j - csum), 2) - pow((i - rsum), 2);

						//Projection
						projRows[i]++;
						projCols[j]++;
					}
				}
			}

			//Projection
			for (int i = 0; i < src->rows; i++) {
				for (int j = 0; j < projRows[i]; j++) {
					projection.at<Vec3b>(i, j) = Vec3b(150, 150, 150);
				}
			}
			for (int j = 0; j < src->cols; j++) {
				for (int i = src->rows - 1; i >= src->rows - projCols[j]; i--) {
					projection.at<Vec3b>(i, j) = Vec3b(150, 150, 150);
				}
			}

			//Elongation axis
			angle = atan2(2 * nominator, denominator) / 2;
			//angle = round(180 * angle / PI);
			printf("Angle: %f\n", round(180 * angle / PI));
			Point centerMass = Point(csum, rsum);
			float width, height;
			Point endP = Point(csum + 100 * cos(angle), rsum + 100 * sin(angle));
			line(projection, centerMass, endP, 0, 2);

			//Display center of mass
			projection.at<Vec3b>(rsum, csum) = Vec3b(0, 0, 255);

			imshow("Projection", projection);
			waitKey(0);
		}
		else {
			printf("No object in this area!\n");
		}
	}
}

//1. Compute geometric features of a clicked object
void computeFeatures() {
	char fname[MAX_PATH];
	while (openFileDlg(fname)) {
		Mat src = imread(fname);

		namedWindow("Image", 1);
		setMouseCallback("Image", objectClickedCallback, &src);

		imshow("Image", src);
		waitKey(0);
	}
}

//2. Cut the input image
void cutImage() {
	char fname[MAX_PATH];
	while (openFileDlg(fname)) {
		Mat srcOrig = imread(fname);
		Mat src;
		srcOrig.copyTo(src);

		int TH_area, phi_LOW, phi_HIGH;
		printf("Area treshold:");
		scanf("%d", &TH_area);
		printf("Phi low:");
		scanf("%d", &phi_LOW);
		printf("Phi high:");
		scanf("%d", &phi_HIGH);

		int area = 0;
		int rsum = 0;
		int csum = 0;

		float nominator = 0;
		float denominator = 0;
		float angle;

		//Color of the background
		Vec3b background = Vec3b(255, 255, 255);

		//Create the new display window
		Mat projection = Mat(src.rows, src.cols, CV_8UC3);
		for (int i = 0; i < src.rows; i++) {
			for (int j = 0; j < src.cols; j++) {
				projection.at<Vec3b>(i, j) = background;
			}
		}

		Vec3b pixel;
		for (int n = 0; n < src.rows; n++) {
			for (int m = 0; m < src.cols; m++) {
				if (src.at<Vec3b>(n, m) != background) {
					//We found an object
					pixel = src.at<Vec3b>(n, m);
					for (int i = 0; i < src.rows; i++) {
						for (int j = 0; j < src.cols; j++) {
							//Find the entire object
							if (src.at<Vec3b>(i, j) == pixel) {
								//Area                 
								area++;
								//Center of mass
								rsum += i;
								csum += j;
							}
						}
					}
					rsum /= area;
					csum /= area;


					for (int i = 0; i < src.rows; i++) {
						for (int j = 0; j < src.cols; j++) {
							if (src.at<Vec3b>(i, j) == pixel) {
								//Axis elongation
								nominator += (i - rsum)*(j - csum);
								denominator += pow((j - csum), 2) - pow((i - rsum), 2);

								//Keep the object if area is ok
								if (area < TH_area) {
									projection.at<Vec3b>(i, j) = src.at<Vec3b>(i, j);
								}

								//Delete processed object from source
								src.at<Vec3b>(i, j) = background;
							}
						}
					}

				}
			}
		}

		imshow("Source", srcOrig);
		imshow("Projection", projection);
		waitKey(0);

	}
}

/******************************************************
						 LAB 5
*******************************************************/

//1. BFS algorithm for labeling
//2. Create color image based on labeling
//4. Create intermediary results after labeling
void bfsObjectLabeling() {
	char fname[MAX_PATH];

	int neighborNr;
	printf("How many neighbors? 4 or 8?\n");
	scanf("%d", &neighborNr);
	if (neighborNr != 8 && neighborNr != 4) {
		printf("Wrong input!!\n");
		return;
	}
	bool pause = false;
	printf("Do you want to see intermediary results? y/n\n");
	char c;
	scanf(" %c", &c);
	if (c == 'y' || c == 'Y') {
		pause = true;
	}
	else if (c != 'n' && c != 'N') {
		printf("Wrong input!!\n");
		return;
	}

	while (openFileDlg(fname)) {
		Mat img = imread(fname, CV_LOAD_IMAGE_GRAYSCALE);
		Mat labels = Mat::zeros(img.rows, img.cols, CV_8UC1);
		Mat dst = Mat(img.rows, img.cols, CV_8UC3);

		int label = 0;

		int di[8] = { -1,0,1,0,1,1,-1,-1 };
		int dj[8] = { 0,-1,0,1,1,-1,1,-1 };

		Vec3b randPixel;
		int nbX, nbY;

		/*std::default_random_engine gen;
		std::uniform_int_distribution<int> d(0, 255);
		uchar r, g, b;*/

		//Initialize destination image to white
		for (int i = 0; i < dst.rows; i++) {
			for (int j = 0; j < dst.cols; j++) {
				dst.at<Vec3b>(i, j) = Vec3b(255, 255, 255);
			}
		}

		for (int i = 0; i < img.rows; i++) {
			for (int j = 0; j < img.cols; j++) {
				if (img.at<uchar>(i, j) == 0 && labels.at<uchar>(i, j) == 0) {
					label++;
					labels.at<uchar>(i, j) = label;

					//Generate random color
					/*uchar r = d(gen);
					uchar g = d(gen);
					uchar b = d(gen);*/
					randPixel = Vec3b(rand() % 256, rand() % 256, rand() % 256);
					dst.at<Vec3b>(i, j) = randPixel;

					std::queue<Point2i> Q;
					Q.push({ i,j });			// add element onto the top of the queue (newest)

					while (!Q.empty()) {
						Point2i p = Q.front();  // acces the element from the bottom of the queue (oldest element)
						Q.pop();				// removes the element from the top of the queue (newest)

						//Check all neighbors
						for (int k = 0; k < neighborNr; k++) {
							nbX = p.x + di[k];
							nbY = p.y + dj[k];
							if (nbX >= 0 && nbX < img.rows && nbY >= 0 && nbY < img.cols) {
								if (img.at<uchar>(nbX, nbY) == 0 && labels.at<uchar>(nbX, nbY) == 0) {
									labels.at<uchar>(nbX, nbY) = label;
									dst.at<Vec3b>(nbX, nbY) = randPixel;

									Q.push({ nbX, nbY });
								}
							}
						}
					}

					if (pause) {
						//Display inermediary results
						imshow("Objects", dst);
						waitKey(0);
					}
				}
			}
		}

		imshow("Source", img);
		imshow("Objects", dst);
		waitKey(0);
	}
}

/******************************************************
						  LAB 6
*******************************************************/
void borderTracing8Conn() {
	char fname[MAX_PATH];
	while (openFileDlg(fname)) {
		Mat img = imread(fname, CV_LOAD_IMAGE_GRAYSCALE);
		Mat dst = Mat::zeros(img.rows, img.cols, CV_8UC1);

		//neighbor coordinate difference arrays
		int di[8] = { 0, -1, -1, -1, 0, 1, 1, 1 };
		int dj[8] = { 1, 1, 0, -1, -1, -1, 0, 1 };

		//iteration points
		Point startPoint;
		Point secondPoint;
		Point previousPoint;
		Point currentPoint;

		//some definitions
		int dir_prev = 7;
		int i, j;

		int dir;
		int border[100000];
		int borderDer[100000];
		int borderIndex = 0;

		uchar background = 255;

		/****find first point******/
		bool isFound = false;
		for (i = 0; i < img.rows; i++) {
			for (j = 0; j < img.cols; j++) {
				if (img.at<uchar>(i, j) != background) {
					startPoint = Point(i, j);
					isFound = true;
					break;

				}
			}
			if (isFound) {
				break;
			}
		}
		/**********/

		//***find second point***
		if (dir_prev % 2 == 0) {
			//previous direction nr is even
			dir = (dir_prev + 7) % 8;
		}
		else {
			//previous direction nr is odd
			dir = (dir_prev + 6) % 8;
		}
		for (i = dir; i < dir + 8; i++) {
			if (img.at<uchar>(startPoint.x + di[i % 8], startPoint.y + dj[i % 8]) != background) {
				secondPoint = Point(startPoint.x + di[i % 8], startPoint.y + dj[i % 8]);
				dir_prev = i % 8;
				border[borderIndex++] = dir_prev;
				break;
			}
		}
		/************/

		previousPoint = startPoint;
		currentPoint = secondPoint;

		do {
			if (dir_prev % 2 == 0) {
				//previous direction nr is even
				dir = (dir_prev + 7) % 8;
			}
			else {
				//previous direction nr is odd
				dir = (dir_prev + 6) % 8;
			}
			for (i = dir; i < dir + 8; i++) {
				if (img.at<uchar>(currentPoint.x + di[i % 8], currentPoint.y + dj[i % 8]) != background) {
					previousPoint = currentPoint;
					currentPoint = Point(currentPoint.x + di[i % 8], currentPoint.y + dj[i % 8]);
					dir_prev = i % 8;
					border[borderIndex++] = dir_prev;
					break;
				}
			}
		} while (!(currentPoint == secondPoint && previousPoint == startPoint));

		//drawing the final result from the border array
		dst.at<uchar>(startPoint.x, startPoint.y) = 255;
		currentPoint = startPoint;
		
		for (i = 0; i < borderIndex; i++) {
			currentPoint = Point(currentPoint.x + di[border[i]], currentPoint.y + dj[border[i]]);
			dst.at<uchar>(currentPoint.x, currentPoint.y) = 255;
		}

		//calculating derivative chain
		for (i = 0; i<borderIndex; i++)
		{
			borderDer[i] = border[(i + 1) % borderIndex] - border[i];
			if (borderDer[i] < 0) {
				borderDer[i] += 8;
			}
		}

		//writing chain code to txt file
		std::ofstream arrayData("array.txt");
		arrayData << "Chain code:" << std::endl;
		for (i = 0; i<borderIndex; i++)
		{
			arrayData << border[i] << " "; //Outputs array to txtFile
		}
		arrayData << "Derivative chain code:" << std::endl;
		for (i = 0; i<borderIndex; i++)
		{
			arrayData << borderDer[i] << " "; //Outputs array to txtFile
		}
		arrayData.close();

		imshow("Image", img);
		imshow("Border", dst);

		waitKey(0);
	}
}

//EX3 - RECONSTRUCT
void reconstructBorder() {
	char* fname ="Images\\L6images\\gray_background.bmp";
	Mat dst = imread(fname, CV_LOAD_IMAGE_GRAYSCALE);

	Point startPoint;
	int length;
	int border[100000];

	//neighbor coordinate difference arrays
	int di[8] = { 0, -1, -1, -1, 0, 1, 1, 1 };
	int dj[8] = { 1, 1, 0, -1, -1, -1, 0, 1 };

	std::ifstream inFile;
	inFile.open("Images\\L6images\\reconstruct.txt");
	if (inFile.is_open()) {
		while (!inFile.eof()) {

			inFile >> startPoint.x >> startPoint.y;
			inFile >> length;
			for (int i = 0; i < length; i++) {
				inFile >> border[i];
			}
		}
	}
	inFile.close();

	//drawing the final result from the border array
	dst.at<uchar>(startPoint.x, startPoint.y) = 0;

	for (int i = 0; i < length; i++) {
		startPoint = Point(startPoint.x + di[border[i]], startPoint.y + dj[border[i]]);
		dst.at<uchar>(startPoint.x, startPoint.y) = 0;
	}

	imshow("Image", dst);
	waitKey(0);
}

int main()
{
	int op;
	int nghb;
	do
	{
		system("cls");
		destroyAllWindows();
		printf("Menu:\n");
		printf(" 1 - Open image\n");
		printf(" 2 - Open BMP images from folder\n");
		printf(" 3 - Image negative - diblook style\n");
		printf(" 4 - BGR->HSV\n");
		printf(" 5 - Resize image\n");
		printf(" 6 - Canny edge detection\n");
		printf(" 7 - Edges in a video sequence\n");
		printf(" 8 - Snap frame from live video\n");
		printf(" 9 - Mouse callback demo\n");
		printf(" 10 - Create image\n");
		printf(" 11 - Image with upper rows dark\n");
		printf(" 12 - Image with white square in middle\n");
		printf(" 13 - Negative of image\n");
		printf(" 14 - Modify image brightness\n");
		printf(" 15 - Create color image\n");
		printf(" 16 - Negative color image\n");
		printf(" 17 - Modify color image brightness\n");
		printf(" 18 - Calculate matrix inverse\n");
		printf(" 20 - Color to grayscale by channels\n");
		printf(" 21 - Color to grayscale\n");
		printf(" 22 - Grayscale to black and white\n");
		printf(" 23 - RGB to HSV\n");
		printf(" 24 - Filter yellow HSV\n");
		printf(" 40 - Compute geometric properties\n");
		printf(" 41 - Cut objects from image\n");
		printf(" 50 - Label objects and color them\n");
		printf(" 60 - Border tracing - 8 connectivity\n");
		printf(" 61 - Reconstruct border from file\n");
		printf(" 0 - Exit\n\n");
		printf("Option: ");
		scanf("%d", &op);
		switch (op)
		{
		case 1:
			testOpenImage();
			break;
		case 2:
			testOpenImagesFld();
			break;
		case 3:
			testParcurgereSimplaDiblookStyle(); //diblook style
			break;
		case 4:
			//testColor2Gray();
			testBGR2HSV();
			break;
		case 5:
			testResize();
			break;
		case 6:
			testCanny();
			break;
		case 7:
			testVideoSequence();
			break;
		case 8:
			testSnap();
			break;
		case 9:
			testMouseClick();
		case 10:
			createImage();
			break;
		case 11:
			upperRows();
			break;
		case 12:
			whiteSquare();
			break;
		case 13:
			negativeImage();
			break;
		case 14:
			modifyBrightness(50);
			break;
		case 15:
			createColorImage();
			break;
		case 16:
			negativeColorImage();
			break;
		case 17:
			modifyColorImageBrightness(50);
			break;
		case 18:
			matrixInverse();
			break;
		case 20:
			channelGrayscale();
			break;
		case 21:
			colorToGrayscale();
			break;
		case 22:
			grayscaleToBW();
			break;
		case 23:
			rgbToHsv();
			break;
		case 24:
			filterHSV();
			break;
		case 40:
			computeFeatures();
			break;
		case 41:
			cutImage();
			break;
		case 50:
			bfsObjectLabeling();
			break;
		case 60:
			borderTracing8Conn();
			break;
		case 61:
			reconstructBorder();
			break;
		}
	} while (op != 0);
	return 0;
}