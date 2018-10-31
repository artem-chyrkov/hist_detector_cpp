#ifndef _SLIDINGSTRIPEROIS_H_
#define _SLIDINGSTRIPEROIS_H_

#include <vector>


struct Roi
{
    int x, y, w, h;
    Roi(int _x, int _y, int _w, int _h): x(_x), y(_y), w(_w), h(_h) {}
};


class SlidingStripeRois
{
public:
    SlidingStripeRois();
    void init(int frameWidth, int frameHeight, int roiWidth, int roiHeight);
    const std::vector<Roi>& getCurrentRois();

private:
    int rows;
    int currentRoiRow;
    std::vector<std::vector<Roi> > rois;
};

#endif // _SLIDINGSTRIPEROIS_H_
