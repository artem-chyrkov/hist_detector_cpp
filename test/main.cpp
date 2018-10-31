#include <iostream>
#include <vector>
#include "opencv.hpp"

#include "hist_detector/HistDetector.h"
#include "hist_detector/SlidingStripeRois.h"
#include "hist_detector/statistics.h"

typedef enum { STAGE_TRAIN, STAGE_DETECT } Stage;

int main()
{
    cv::VideoCapture videoCapture(0);
    if (!videoCapture.isOpened())
    {
        std::cout << "!videoCapture.isOpened()\n";
        return 1;
    }
    int frameWidth = int(videoCapture.get(cv::CAP_PROP_FRAME_WIDTH));
    int frameHeight = int(videoCapture.get(cv::CAP_PROP_FRAME_HEIGHT));

    SlidingStripeRois slidingStripeRois;
    slidingStripeRois.init(frameWidth, frameHeight, 50, 50);

    int BINS_COUNT = 32;  // only 2^n are supported
    Float ALPHA_D4 = 0.99;
    Float K1_ALPHA = 0.95;  // multiplier for D1
    Float K2_ALPHA = 0.9;  // multiplier for delta_m
    Float Q_ALPHA = 0.3;  // multiplier for n_T

    HistDetector histDetector(BINS_COUNT, ALPHA_D4, K1_ALPHA, K2_ALPHA, Q_ALPHA);
    histDetector.init(9000);

    cv::Mat frame;
    Stage stage = STAGE_TRAIN;
    HistDetector::PatchParams patchParams;  // { (m, D) for all patches }
    int frameNo = 1;
    const int FRAMES_FOR_TRAINING = 400;
    while (true)
    {
        if (!videoCapture.read(frame)) break;
        cv::Mat frameToShow = frame.clone();

        if (stage == STAGE_TRAIN)
        {
            if (frameNo > FRAMES_FOR_TRAINING)
            {
                histDetector.train(patchParams);
                stage = STAGE_DETECT;
                std::cout << "D1 == " << histDetector.D1 << "\nD4 == " << histDetector.D4 << "\n\n";
            }
            ++frameNo;
            std::vector<Roi> rois = slidingStripeRois.getCurrentRois();
            for (std::vector<Roi>::iterator roi = rois.begin();
                 roi != rois.end(); ++roi)
            {
                cv::Mat patchBgr = frame(cv::Rect(roi->x, roi->y, roi->w, roi->h));
                cv::Mat patchGray;
                cv::cvtColor(patchBgr, patchGray, cv::COLOR_BGR2GRAY);
                std::vector<Float> hist = histDetector.calculateHistogram(patchGray);
                std::pair<Float, Float> m_D = calculateMeanAndDispersionI(hist);
                patchParams.push_back(m_D);
            }
            cv::putText(frameToShow, "Train", cv::Point(1, 14), cv::FONT_HERSHEY_PLAIN,
                        1.0, cv::Scalar(0, 255, 0));
        }

        else  // STAGE_DETECT
        {
            std::vector<Roi> rois = slidingStripeRois.getCurrentRois();
            std::vector<Target> targets = histDetector.detect(frame, rois);
            for (std::vector<Target>::iterator target = targets.begin();
                 target != targets.end(); ++target)
            {
                cv::Scalar color = (target->suspicionLevel == SL_UNKNOWN) ?
                            cv::Scalar(0, 255, 255) : cv::Scalar(0, 255, 0);
                int thickness = (target->suspicionLevel == SL_SUSPICIOUS) ? 2 : 1;
                cv::rectangle(frameToShow,
                              cv::Rect(target->roi.x, target->roi.y, target->roi.w, target->roi.h),
                              color, thickness);
            }
        }

        cv::imshow("frameToShow", frameToShow);
        if (cv::waitKey(1) > 0) break;
    }

    return 0;
}
