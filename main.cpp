#include <opencv2/opencv.hpp>
#include <iostream>
#include <algorithm> 
using namespace cv;
using namespace std;

class seamcar{
    private:
        Mat img;
    public:
    seamcar(const string& path){
        img = imread(path);
        if (img.empty()) {
            throw runtime_error("Image not found or unable to open");
        }
    }
    Mat calcEnergy(Mat& gray) {
        Mat gx, gy;
        Sobel(gray, gx, CV_32F, 1, 0, 3);
        Sobel(gray, gy, CV_32F, 0, 1, 3);
        Mat mag;
        magnitude(gx, gy, mag);
        return mag;
    }
    Mat applydpforRow(Mat& energy) {
        long long row=energy.rows;
        long long  col=energy.cols;
        Mat path = energy.clone();
        float cleft,up,cright;
        for(long long i=1;i<row;i++){
            for (long long j=0;j<col;j++){
                if(j>0){
                    cleft=path.at<float>(i-1,j-1);
                }else{
                    cleft = INT_MAX; // large value for out of bounds
                }
                up=path.at<float>(i-1,j);
                if(j<col-1){
                    cright=path.at<float>(i-1,j+1);
                } else {
                    cright=INT_MAX; // large value for out of bounds
                }
                path.at<float>(i,j) =path.at<float>(i,j)+ min(up,min(cleft,cright));
            }
        }
        return path;
    }

    // Backtrack seam
    int*  verticalseam(Mat& M) {
        int rows=M.rows,cols=M.cols;
        int* seam=new int[rows];

        // min in bottom row
        int idx = 0;
        float minVal = M.at<float>(rows-1,0);
        for (int j = 1; j < cols; j++) {
            if (M.at<float>(rows-1,j) < minVal) {
                minVal = M.at<float>(rows-1,j);
                idx = j;
            }
        }
        seam[rows-1] = idx;

        // backtrack
        for (int i = rows-2; i>=0; i--) {
            int upindex= seam[i+1];
            float minivale= M.at<float>(i+1, upindex);
            int miniindex= upindex;
            if(seam[i+1]>0 && minivale>M.at<float>(i+1,seam[i+1]-1)){
                minivale = M.at<float>(i+1, seam[i+1]-1);
                miniindex = seam[i+1]-1;
            }
            
            if(seam[i+1]+1<cols){
                if(minivale>M.at<float>(i+1,seam[i+1]+1)){
                    minivale=M.at<float>(i+1,seam[i+1]+1);
                    miniindex=seam[i+1]+1;
                }
            }    
            seam[i] = miniindex;
        }

        return seam;
    }

    Mat removeSeam(Mat& img,int* seam,bool isrow) {
        int rows=img.rows;
        int cols=img.cols;
        Mat outimg;
        if(isrow){
            outimg = Mat(rows - 1, cols, CV_8UC3);
            for (int j = 0; j < cols; j++) {
                int k = 0;
                for (int i = 0; i < rows; i++) {
                    if (i == seam[j]) continue;  // remove this row pixel
                    outimg.at<Vec3b>(k++, j) = img.at<Vec3b>(i, j);
                }
            }
        }else{
            outimg = Mat(rows, cols - 1, CV_8UC3);
            for (int i = 0; i < rows; i++) {
                int k = 0;
                for (int j = 0; j < cols; j++) {
                    if (j == seam[i]) continue;  // remove this column pixel
                    outimg.at<Vec3b>(i, k++) = img.at<Vec3b>(i, j);
                }
            }
        }
        
        return outimg;
    }
    int * horizontalseam(Mat& M) {
        int rows=M.rows,cols=M.cols;
        int* seam=new int[cols];
        int minvalue= M.at<float>(0,cols-1);
        int miniindex=0;
        for(int i=0;i<rows;i++){
            if(M.at<float>(i,cols-1)<minvalue){
                minvalue=M.at<float>(i,cols-1);
                miniindex=i;
            }
        }
        seam[cols-1]=miniindex;
        for(int i=cols-2;i>=0;i--){
            int sideindex=seam[i+1];
            float minivale=M.at<float>(sideindex,i);
            int miniindex=sideindex;
            if(sideindex>0 && minivale>M.at<float>(sideindex-1,i)){
                minivale=M.at<float>(sideindex-1,i);
                miniindex=sideindex-1;
            }
            if(sideindex+1<rows && minivale>M.at<float>(sideindex+1,i)){
                minivale=M.at<float>(sideindex+1,i);
                miniindex=sideindex+1;
            }
            seam[i]=miniindex;
        }
        return seam;
    }
    Mat applydpforCol(Mat& energy) {
        long long row=energy.rows;
        long long  col=energy.cols;
        Mat path = energy.clone();
        float sleft,up,sright;
        for(long long j=1;j<col;j++){
            for (long long i=0;i<row;i++){
                if(i>0){
                    sleft=path.at<float>(i-1,j-1);
                }else{
                    sleft = INT_MAX; // large value for out of bounds
                }
                if(i<row-1){
                    sright=path.at<float>(i+1,j-1);
                }else{
                    sright=INT_MAX; // large value for out of bounds
                }
                up=path.at<float>(i,j-1);
                path.at<float>(i,j) =path.at<float>(i,j)+ min(up,min(sleft,sright));
            }
        }
        return path;
    }
    Mat shrink(Mat img, int removerow,int removecol) {
        //run for calculation coloum
        for (int s=0;s<removecol;s++) {
            Mat gray;
            //convert image into gray 
            cvtColor(img, gray, COLOR_BGR2GRAY);
            Mat energy=calcEnergy(gray);
            Mat M=applydpforRow(energy);
            int* seam = verticalseam(M);
            img = removeSeam(img, seam,false);
            delete[] seam; // free memory
        }
        //run for calculation row
        for (int s=0;s<removerow;s++) {
            Mat gray;
            cvtColor(img, gray, COLOR_BGR2GRAY);
            Mat energy=calcEnergy(gray);
            Mat M=applydpforCol(energy);
            int* seam = horizontalseam(M);
            img = removeSeam(img,seam,true);
            delete[] seam; // free memory
        }
        return img;
    }
    Mat findImage() {
        return img;
    }

};
int main() {
    //Take image and call constrcutor
    seamcar sc("D:/DSA Assignmnet/A!/sample1.jpeg");
    Mat img = sc.findImage();
    try{
        if (img.empty()) {
            throw runtime_error("not open Image");
        }
        cout<< "Image size: \n" <<"Row:"<< img.rows << "\n" <<"Cols:"<< img.cols << endl;
        cout<<"Enter your desired image size\n";
        int row,col;
        cout<<"Row: ";
        cin>>row;
        cout<<"Col: ";
        cin>>col;
        if(row>img.rows){
            throw runtime_error("Invalid size row");
        }
        if(col>img.cols){
            throw runtime_error("Invalid size col");
        }
        Mat carved =sc.shrink(img,img.rows-row,img.cols-col); 

        imshow("Original", img);
        imshow("Seam Carved", carved);
        waitKey(0);
    }catch (const exception& e) {
        cerr << "Error: "<< e.what() << endl;
        return -1;
    }
    return 0;
}
