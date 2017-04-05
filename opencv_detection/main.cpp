//XIE TIANLEI
//2016-7-22

// Ref: http://docs.opencv.org/2.4/doc/tutorials/imgproc/shapedescriptors/find_contours/find_contours.html
#include <opencv2/opencv.hpp>
#include <dirent.h>
using namespace cv;
using namespace std;

struct MatAdv {
	Mat mPicture;
	Mat mContour;
	int iRectNumber;
	Point pAvgRectHW;
};

using namespace std;

const String keys =       
		"{help h    || $ ./main -image=./input.image}"
        "{image i   || path to image file }"
        ;

/*
===========================================================================================================================
===========================================================================================================================
===========================================================================================================================
===========================================================================================================================
*/

MatAdv thresh_callback(Mat pic, Mat src_gray, double dMinThresh, double dMaxThresh){
   RNG rng(12345);
   int iNumberBound = 1;
   int iSumWide = 0;
   int iSumHeight = 0;
   int iMatSize[] = {100,4};
   int iSizeAvgHW[] = {100,2};
   Mat mAllContour(2, iMatSize, CV_32F, Scalar::all(0));
   Mat mContourHW(2, iSizeAvgHW, CV_32F, Scalar::all(0)); //mContourHW.at<int>(1,1) for wide , mContourHW.at<int>(1,2) for height
   Mat threshold_output;
   Mat src;
   double dPictureArea = 0.0;
   double dBoundArea = 0.0;
   double dCompareRatio = 0.0;
   vector<vector<Point> > contours;
   vector<Vec4i> hierarchy;
   Point pAvgRectHW(0,0);
   pic.copyTo(src);// deep copy

   //Get Picture size
   IplImage iplimage = src;
   int iPictureSizeW = iplimage.width;
   int iPictureSizeH = iplimage.height;
   int iNumberRect = 0;



   /// Use 'Threshold' method to detect edges
   threshold( src_gray, threshold_output, 0, 255, THRESH_OTSU+THRESH_BINARY);

   /// Find contours
   findContours( threshold_output, contours, hierarchy, CV_RETR_TREE, CV_CHAIN_APPROX_SIMPLE, Point(0, 0) );

   vector<vector<Point> > contours_poly( contours.size() );
   vector<Rect> boundRect( contours.size() );

   for( int i = 0; i < contours.size(); i++ ){
      approxPolyDP( Mat(contours[i]), contours_poly[i], 3, true );
      boundRect[i] = boundingRect( Mat(contours_poly[i]) );
   }

   //Draw contours and detection-rectangles
   //boundRect.tl().x for left-top x-coordinate information,　'.br()' for right-bottom x-coordinate information
   for( int i = 0; i< contours.size(); i++ ){
      //Calculate the area for comparison
	  dPictureArea = iPictureSizeW*iPictureSizeH;
	  dBoundArea = (boundRect[i].br().x-boundRect[i].tl().x)*(boundRect[i].br().y-boundRect[i].tl().y);
	  dCompareRatio = dBoundArea / dPictureArea;

	  //Only draw Bounding-box whose area are between dMinThresh and dMaxThresh
      if(dCompareRatio > dMinThresh && dCompareRatio < dMaxThresh){
         Scalar color = Scalar( rng.uniform(0, 255), rng.uniform(0,255), rng.uniform(0,255) );
         rectangle( src, boundRect[i].tl(), boundRect[i].br(), color, 3, 8, 0 );

         //Store all contours coordinator information
         mAllContour.at<int>(iNumberBound,1) = boundRect[i].tl().x;
         mAllContour.at<int>(iNumberBound,2) = boundRect[i].tl().y;
         mAllContour.at<int>(iNumberBound,3) = boundRect[i].br().x;
         mAllContour.at<int>(iNumberBound,4) = boundRect[i].br().y;

         //Get width
         mContourHW.at<int>(iNumberBound,1) = (mAllContour.at<int>(iNumberBound,3) - mAllContour.at<int>(iNumberBound,1));
         //Get height
         mContourHW.at<int>(iNumberBound,2) = ( mAllContour.at<int>(iNumberBound,4) - mAllContour.at<int>(iNumberBound,2));
         //count the number of contours
         iNumberBound = iNumberBound + 1;

         //Reset parameters
         dPictureArea = 0.0;
         dBoundArea = 0.0;
         dCompareRatio = 0.0;
      }
   }

   //Get the average width and height
   for (int i = 0; i < iNumberBound; i++){
	  iSumWide = mContourHW.at<int>(i,1)+ iSumWide;
      iSumHeight = mContourHW.at<int>(i,2)+ iSumHeight;;
   }

   pAvgRectHW.x = iSumWide/iNumberBound;
   pAvgRectHW.y = iSumHeight/iNumberBound;

   // to find how many contours
   // the real number of rect. should be "iNumberBound - 1"
   iNumberRect = iNumberBound;

   cout<< "Total Number of Contours  =  "<<iNumberRect-1 <<endl;

   // no contours
   if(iNumberBound == 1){
      cout<<"No contours!"<<endl;
      iNumberRect = iNumberBound;
   }


   // Display results
   namedWindow( "Contours", CV_WINDOW_NORMAL );
   imshow( "Contours", src );

   //Store data to structure
   MatAdv ret = {
		   src,
		   mAllContour,
		   iNumberRect,
		   pAvgRectHW
   };
   return ret;
}

/*
===========================================================================================================================
===========================================================================================================================
===========================================================================================================================
===========================================================================================================================
*/

void judge_point(int iClickPointX,int iClickPointY, void* param){
   Mat partImg;// partial picture
   Mat src;
   Mat mContour;
   int iCounter = 0;
   int iIndicator = 0;
   int iNewSquareEdge;
   int iNumberRect;
   Point pCenterHit;
   Point pLeftTopNotHit(-1,-1);
   Point pRightDownNotHit(-1,-1);
   Point pDrawLeftTop(-1,-1);
   Point pDrawRightDown(-1,-1);
   Point pAvgRectHW(-1,-1);

   //Get data from MatAdv structure
   MatAdv matAdv = *(MatAdv*) param;
   src = matAdv.mPicture;
   iNumberRect = matAdv.iRectNumber;
   mContour = matAdv.mContour;
   pAvgRectHW = matAdv.pAvgRectHW;
   int iSizeHit[] = {iNumberRect,4};
   Mat mHitContour(2, iSizeHit, CV_32F, Scalar::all(0));

   IplImage iplimage = src;
   int iPictureSizeW = iplimage.width;
   int iPictureSizeH = iplimage.height;

   //it means more than 0 rectangle.
   if(iNumberRect>1){
      for(int i = 1; i <iNumberRect;i++){
         //find all hit rectangles
         if(iClickPointX>mContour.at<int>(i,1) && iClickPointX<mContour.at<int>(i,3) && iClickPointY>mContour.at<int>(i,2) && iClickPointY<mContour.at<int>(i,4)){
            iIndicator = 2;//indicate already hit the rectangle

            cout << endl;
            cout<<"\n\n\n" <<endl;
            cout << "------>Point in No."<< i <<" rectangle<------"<< endl;
            cout << "------>Click points are:("<< iClickPointX <<","<< iClickPointY <<")<------"<< endl;
            cout<<"\n\n\n"<<endl;
            cout << endl;

            mHitContour.at<int>(iCounter,1) = mContour.at<int>(i,1);
               if (mHitContour.at<int>(i,1)<0){
                  mHitContour.at<int>(i,1) = 1;
               }
            mHitContour.at<int>(iCounter,2) = mContour.at<int>(i,2);
               if (mHitContour.at<int>(i,2)<0){
                  mHitContour.at<int>(i,2) = 1;
               }
            mHitContour.at<int>(iCounter,3) = mContour.at<int>(i,3);
               if (mHitContour.at<int>(i,3)>iPictureSizeW){
                  mHitContour.at<int>(i,3) = iPictureSizeW;
               }
            mHitContour.at<int>(iCounter,4) = mContour.at<int>(i,4);
               if (mHitContour.at<int>(i,4)>iPictureSizeH){
                  mHitContour.at<int>(i,4) = iPictureSizeH;
               }
	    iCounter = iCounter+1;// for counting the number of hit
		}

        else {// not hit, set the iIndicator to be 0
           iIndicator = iIndicator|0;
        }
      }

   	  // When there are more than 1 rectangles.
   	  // Find minimum area rectangles
	  // Initialize the minimum rectangles
      int iMinContourArea = (mHitContour.at<int>(0,3)-mHitContour.at<int>(0,1))*(mHitContour.at<int>(0,4)-mHitContour.at<int>(0,2));
      int iNextContourArea;
      int iMinContourNumber = 0;  // to tell which contour is minimum
      if(iCounter>1||iCounter==1){
         for (int i = 0; i<iCounter;i++){
            iNextContourArea = (mHitContour.at<int>(i,3)-mHitContour.at<int>(i,1))*(mHitContour.at<int>(i,4)-mHitContour.at<int>(i,2));
            //cout<< " iNextContourArea = " << iNextContourArea<<endl;
	        if(iNextContourArea < iMinContourArea) {
	           iMinContourArea = iNextContourArea;
	           iMinContourNumber = i;
		    }
	     }
      }

   //cout<<"iMinContourNumber = "<<iMinContourNumber <<endl;

   //when it hit
   // Take the average value of height and width as the new edge of square
   iNewSquareEdge = ( ( mHitContour.at<int>(iMinContourNumber,3) - mHitContour.at<int>(iMinContourNumber,1) ) + ( mHitContour.at<int>(iMinContourNumber,4)- mHitContour.at<int>(iMinContourNumber,2) ) )/2;

   /*
   iIndicator = 2 means hit
   iIndicator = 0 means not hit
   */

   //ready to draw partial picture
      if (iIndicator < 1){// if not hit
	      pLeftTopNotHit.x = iClickPointX - ((pAvgRectHW.x+pAvgRectHW.y)/2) - 15;//((pAvgRectHW.x+pAvgRectHW.y)/10);
	      pLeftTopNotHit.y = iClickPointY - ((pAvgRectHW.x+pAvgRectHW.y)/2) - 15;//((pAvgRectHW.x+pAvgRectHW.y)/10);
	      pRightDownNotHit.x = iClickPointX + ((pAvgRectHW.x+pAvgRectHW.y)/2) + 15;//((pAvgRectHW.x+pAvgRectHW.y)/10);
	      pRightDownNotHit.y = iClickPointY + ((pAvgRectHW.x+pAvgRectHW.y)/2) + 15;//((pAvgRectHW.x+pAvgRectHW.y)/10);

	      if (pLeftTopNotHit.x<0){
		     pRightDownNotHit.x = pRightDownNotHit.x-pLeftTopNotHit.x;
		     pLeftTopNotHit.x = 0;
	      }
	      if (pLeftTopNotHit.y<0){
		     pRightDownNotHit.y = pRightDownNotHit.y-pLeftTopNotHit.y;
		     pLeftTopNotHit.y = 0;
	      }
      //If it beyond the picture then set it to be the edge of picture
	      if (pRightDownNotHit.x>iPictureSizeW){
		     pLeftTopNotHit.x = pLeftTopNotHit.x-(pRightDownNotHit.x-iPictureSizeW);
		     if (pLeftTopNotHit.x<0){
		   	     pLeftTopNotHit.x = 0;
		     }
		     pRightDownNotHit.x = iPictureSizeW;
	      }
	      if (pRightDownNotHit.y>iPictureSizeH){
	         pLeftTopNotHit.y = pLeftTopNotHit.y-(pRightDownNotHit.y-iPictureSizeH);
	         if (pLeftTopNotHit.y<0){
	   	        pLeftTopNotHit.y = 0;
	         }
	         pRightDownNotHit.y = iPictureSizeH;
	      }
      pDrawLeftTop.x = pLeftTopNotHit.x;
      pDrawLeftTop.y = pLeftTopNotHit.y;
      pDrawRightDown.x = pRightDownNotHit.x;
      pDrawRightDown.y = pRightDownNotHit.y;
      cout<<"****************************************"<<endl;
      cout<<"Clikc point did not hit the rectangle"<<endl;
      }

      else{// if hit
         pCenterHit.x = (mHitContour.at<int>(iMinContourNumber,1)+mHitContour.at<int>(iMinContourNumber,3))/2;
         pCenterHit.y = (mHitContour.at<int>(iMinContourNumber,2)+mHitContour.at<int>(iMinContourNumber,4))/2;

         pDrawLeftTop.x = pCenterHit.x - (iNewSquareEdge/2)-15; //- (iNewSquareEdge/8);
         pDrawLeftTop.y = pCenterHit.y - (iNewSquareEdge/2)-15; //- (iNewSquareEdge/8);
         pDrawRightDown.x = pCenterHit.x + (iNewSquareEdge/2)+15; //+ (iNewSquareEdge/8);
         pDrawRightDown.y = pCenterHit.y + (iNewSquareEdge/2)+15; //+ (iNewSquareEdge/8);
         //cout<<"Before ltx lty rdx rdy =  "<<pDrawLeftTop.x<<","<<pDrawLeftTop.y<<","<<pDrawRightDown.x<<","<<pDrawRightDown.y <<endl;
            if (pDrawLeftTop.x<0){
               pDrawRightDown.x = pDrawRightDown.x-pDrawLeftTop.x;
               pDrawLeftTop.x = 0;
            }
            if (pDrawLeftTop.y<0){
               pDrawRightDown.y = pDrawRightDown.y-pDrawLeftTop.y;
               pDrawLeftTop.y = 0;
            }
            if (pDrawRightDown.x>iPictureSizeW){
            	pDrawLeftTop.x = pDrawLeftTop.x - (pDrawRightDown.x - iPictureSizeW);
            	if(pDrawLeftTop.x<0){
            		pDrawLeftTop.x = 0;
            	}
            	pDrawRightDown.x = iPictureSizeW;
            }
            if (pDrawRightDown.y>iPictureSizeH){
            	pDrawLeftTop.y = pDrawLeftTop.y - (pDrawRightDown.y - iPictureSizeH);
            	if(pDrawLeftTop.y<0){
            		pDrawLeftTop.y = 0;
            	}
            	pDrawRightDown.y = iPictureSizeH;
            }
      cout<<"****************************************"<<endl;
      cout<<"Clikc point hit the rectangle"<<endl;
      cout<<"[ltx,lty] [rdx,rdy] =  ["<<pDrawLeftTop.x<<","<<pDrawLeftTop.y<<"] ["<<pDrawRightDown.x<<","<<pDrawRightDown.y<<"] " <<endl;
   }
      cout<<"wide = "  << pDrawRightDown.x - pDrawLeftTop.x <<endl;
      cout<<"height = "<< pDrawRightDown.y - pDrawLeftTop.y <<endl;
   }// End of if there is more than 0 contours




   //gpNumberRect = 1, it means no contours.
   else{
      pDrawLeftTop.x = iClickPointX-((iPictureSizeW/10+iPictureSizeH/10)/2);
      pDrawLeftTop.y = iClickPointY-((iPictureSizeW/10+iPictureSizeH/10)/2);
      pDrawRightDown.x = iClickPointX + ((iPictureSizeW/10+iPictureSizeH/10)/2);
      pDrawRightDown.y = iClickPointY + ((iPictureSizeW/10+iPictureSizeH/10)/2);


      if (pDrawLeftTop.x<0){
	     pDrawRightDown.x = pDrawRightDown.x-pDrawLeftTop.x;
	     pDrawLeftTop.x = 0;
      }
      if (pDrawLeftTop.y<0){
	     pDrawRightDown.y = pDrawRightDown.y-pDrawLeftTop.y;
	     pDrawLeftTop.y = 0;
      }
      //If it beyond the picture then set it to be the edge of picture
      if (pDrawRightDown.x>iPictureSizeW){
	     pDrawLeftTop.x = pDrawLeftTop.x-(pDrawRightDown.x-iPictureSizeW);
	     if (pDrawLeftTop.x<0){
	   	     pDrawLeftTop.x = 0;
	     }
	     pDrawRightDown.x = iPictureSizeW;
      }
      if (pDrawRightDown.y>iPictureSizeH){
         pDrawLeftTop.y = pDrawLeftTop.y-(pDrawRightDown.y-iPictureSizeH);
         if (pDrawLeftTop.y<0){
   	        pDrawLeftTop.y = 0;
         }
         pDrawRightDown.y = iPictureSizeH;
      }

      cout<<"wide = "  << pDrawRightDown.x - pDrawLeftTop.x <<endl;
      cout<<"height = "<< pDrawRightDown.y - pDrawLeftTop.y <<endl;

   }

   //cut the rectangle
   Rect rect(pDrawLeftTop,pDrawRightDown);

   partImg = src(rect);

   namedWindow("Detection Result",0);
   imshow("Detection Result", partImg);

   //reset corners
   pDrawLeftTop.x = -1; pDrawLeftTop.y = -1;
   pDrawRightDown.x = -1; pDrawRightDown.y = -1;
   cout<<"****************************************"<<endl;
}



/*
===========================================================================================================================
===========================================================================================================================
===========================================================================================================================
===========================================================================================================================
*/


void onMouse(int Event,int iClickPointX,int iClickPointY,int flags,void* param){
   if(Event==CV_EVENT_LBUTTONDOWN){
      //Judge the point is in or not the rectangle
      judge_point(iClickPointX,iClickPointY,param);

   }
}



/*
===========================================================================================================================
===========================================================================================================================
===========================================================================================================================
===========================================================================================================================
*/
void readimgErr(){
	cout<<endl;
	cout<<"Can not read Picture"<<endl;
}
/*
===========================================================================================================================
===========================================================================================================================
===========================================================================================================================
===========================================================================================================================
*/

int main( int argc, char** argv )
{
   Mat src;
   Mat src_gray;
   double dMinThresh = 0.001;; // the minimum area ratio
   double dMaxThresh = 0.8;;// the maximum area ratio


   cv::CommandLineParser parser(argc, argv, keys);
   if (parser.has("help"))
   {
        parser.printMessage();
        return -1;
   }


   //Read Image
   String imageFile = parser.get<String>("image");
   src = imread(imageFile);
   if (src.empty())
   {
		readimgErr();
		return -1;
   }

   //Change Picture to gravy level
   cvtColor( src, src_gray, CV_BGR2GRAY );

   //normalize picture
   blur( src_gray, src_gray, Size(3,3) );

   //Detect contour
   MatAdv threshMatAdv = thresh_callback(src,src_gray,dMinThresh,dMaxThresh);

   //Call onMouse function
   setMouseCallback("Contours",onMouse,&threshMatAdv);

   waitKey(0);
   return(0);
}

