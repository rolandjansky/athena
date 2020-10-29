/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MM_DIGITIZATION_VMM_SHAPER_H
#define MM_DIGITIZATION_VMM_SHAPER_H

#include <vector>


class VMM_Shaper{
 public:
    VMM_Shaper(const float peakTime, const float lowerTimeWindow, const float upperTimeWindow);
    virtual ~VMM_Shaper() {}

    void initialize();

    void vmmPeakResponse(const std::vector<float> &effectiveCharge, const std::vector<float> &electronsTime, const double electronicsThreshold, double &amplitudeFirstPeak, double &timeFirstPeak) const;
    void vmmThresholdResponse(const std::vector<float> &effectiveCharge, const std::vector<float> &electronsTime, const double electronicsThreshold, double &amplitudeAtFirstPeak, double &timeAtThreshold) const;

    bool hasChargeAboveThreshold(const std::vector<float> &effectiveCharge, const std::vector<float> &electronsTime, const double electronicsThreshold) const;


 private:
     double m_peakTime;
     double m_lowerTimeWindow, m_upperTimeWindow;
     
     double m_timeStep, m_inverseTimeStep;

     double m_preCalculationVMMShaper; 
     // shaper parameters
     double m_a, m_pole0, m_re_pole1, m_im_pole1, m_pole1_square, m_k1_abs, m_argK1;
     int m_nBins;

     double m_peakTimeChargeScaling;

     double vmmResponse(const std::vector<float> &effectiveCharge, const std::vector<float> &electronsTime, double time) const;
     double findPeak(const std::vector<float> &effectiveCharge, const std::vector<float> &electronsTime, const double electronicsThreshold) const;
     bool aboveThresholdSimple(const std::vector<float> &effectiveCharge, const std::vector<float> &electronsTime, const double electronicsThreshold) const;


};

#endif  // MM_DIGITIZATION_VMM_SHAPER_H