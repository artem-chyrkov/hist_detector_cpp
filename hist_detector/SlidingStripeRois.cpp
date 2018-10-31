#include "SlidingStripeRois.h"

SlidingStripeRois::SlidingStripeRois()
{
}

void SlidingStripeRois::init(int frameWidth, int frameHeight, int roiWidth, int roiHeight)
{
    int cols = frameWidth / roiWidth;
    rows = frameHeight / roiHeight;
    rois.clear();
    for (int i = 0; i < rows; ++i)
    {
        std::vector<Roi> roiCol;
        for (int j = 0; j < cols; ++j)
        {
            roiCol.push_back(Roi(j * roiWidth, i * roiHeight, roiWidth, roiHeight));
        }
        rois.push_back(roiCol);
    }
    currentRoiRow = -1;
}

const std::vector<Roi>& SlidingStripeRois::getCurrentRois()
{
    currentRoiRow++;
    if (currentRoiRow >= rows) currentRoiRow = 0;
    return rois[currentRoiRow];
}
