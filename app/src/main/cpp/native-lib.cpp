#include <jni.h>
#include <string>
#include <opencv2/imgcodecs/imgcodecs.hpp>
#include <opencv2/imgproc.hpp>
#include "marker.h"

extern "C"{

JNIEXPORT void JNICALL Java_it_signoretto_marco_opencv_1test_PictureAR_applyAR(
        JNIEnv *env,
        jobject, /* this */
        jlong j_img_0p,
        jlong j_img_1p,
        jlong j_img_0m_th,
        jlong j_img_1m_th,
        jlong j_frame,
        jboolean debug_info){

    cv::Mat& img_0p=*(cv::Mat*) j_img_0p;
    cv::Mat& img_1p=*(cv::Mat*) j_img_1p;
    cv::Mat& img_0m_th=*(cv::Mat*) j_img_0m_th;
    cv::Mat& img_1m_th=*(cv::Mat*) j_img_1m_th;
    cv::Mat& frame=*(cv::Mat*) j_frame;

    mcv::marker::apply_AR(img_0p, img_1p, img_0m_th, img_1m_th, frame, debug_info);

}


}

