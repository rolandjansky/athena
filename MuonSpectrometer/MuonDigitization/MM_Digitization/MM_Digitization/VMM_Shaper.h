/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MM_DIGITIZATION_VMM_SHAPER_H
#define MM_DIGITIZATION_VMM_SHAPER_H

#include <vector>


class VMM_Shaper{
 public:
    VMM_Shaper(const float peakTime, const float lowerTimeWindow, const float upperTimeWindow);
    virtual ~VMM_Shaper()=default;

    void initialize();

    void vmmPeakResponse(const std::vector<float> &effectiveCharge, const std::vector<float> &electronsTime, const double electronicsThreshold, double &amplitudeFirstPeak, double &timeFirstPeak) const;
    void vmmThresholdResponse(const std::vector<float> &effectiveCharge, const std::vector<float> &electronsTime, const double electronicsThreshold, double &amplitudeAtFirstPeak, double &timeAtThreshold) const;

    bool hasChargeAboveThreshold(const std::vector<float> &effectiveCharge, const std::vector<float> &electronsTime, const double electronicsThreshold) const;


 private:
     double m_peakTime;
     double m_lowerTimeWindow, m_upperTimeWindow;
     
     double m_timeStep, m_inverseTimeStep;

     double m_preCalculationVMMShaper = 0.0; 
     // shaper parameters
     double m_a = 0.0, m_pole0 = 0.0, m_re_pole1 = 0.0, m_im_pole1 = 0.0, m_pole1_square = 0.0, m_k1_abs = 0.0, m_argK1 = 0.0;
     double m_peakTimeChargeScaling = 0.0;

     double vmmResponse(const std::vector<float> &effectiveCharge, const std::vector<float> &electronsTime, double time) const;
     double findPeak(const std::vector<float> &effectiveCharge, const std::vector<float> &electronsTime, const double electronicsThreshold) const;
     bool aboveThresholdSimple(const std::vector<float> &effectiveCharge, const std::vector<float> &electronsTime, const double electronicsThreshold) const;
     double m_pole0_ns = 0.0;
     double m_re_pole1_ns = 0.0; 
     double m_im_pole1_ns = 0.0;
};

#endif  // MM_DIGITIZATION_VMM_SHAPER_H
