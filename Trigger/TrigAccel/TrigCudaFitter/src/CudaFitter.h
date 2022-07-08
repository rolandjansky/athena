#pragma once
#include "DataTypes.h"
class RecTrack;

class CudaFitter
{
  public:
    CudaFitter(void);
    ~CudaFitter(void);
    void fit(std::vector<const RecTrack*>&);
    void runFixedPointSmoother(INPUT_TRACK_INFO_TYPE*,
                               OUTPUT_TRACK_INFO_TYPE*,
                               DETECTOR_SURFACE_TYPE*,
                               HIT_INFO_TYPE*,
                               int);
};
