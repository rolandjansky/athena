/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#include "MM_Digitization/VMM_Shaper.h"
#include <cmath>

namespace {
    // VMM shaper parameters provided by G. Iakovidis
    static constexpr double K0 = 1.584;
    static constexpr double Re_K1 = -0.792;
    static constexpr double Im_K1 = -0.115;

    static constexpr double chargeScaleFactor = 1/0.411819;

    static constexpr double mmIonFlowTime = 150.;  // ns
}

VMM_Shaper::VMM_Shaper(float peakTime, float lowerTimeWindow, float upperTimeWindow):m_peakTime(peakTime),
m_lowerTimeWindow(lowerTimeWindow),
m_upperTimeWindow(upperTimeWindow),
m_timeStep(0.1)
{
    initialize();
}

void VMM_Shaper::initialize() {
    // hardcoded vmm shaper values are provided by G. Iakovidis
    m_a = (m_peakTime*(10^-9))/1.5;
    m_pole0 = 1.263/m_a;
    m_re_pole1 = 1.149/m_a;
    m_im_pole1 = -0.786/m_a;
    m_pole1_square = m_re_pole1*m_re_pole1 + m_im_pole1*m_im_pole1;
    m_k1_abs = std::sqrt(Re_K1*Re_K1 + Im_K1*Im_K1);
    m_argK1 = std::atan2(Im_K1, Re_K1);

    // scale factor for charge taking into account the mm ion flow time of ~150ns
    // if the peaking time is lower then that, only a fration of the total charge is integrated
    m_peakTimeChargeScaling = (m_peakTime < mmIonFlowTime ? 1.0*m_peakTime/mmIonFlowTime : 1.0);
    
    // preCalculate factor to avoid recalculating for each electron
    m_preCalculationVMMShaper = chargeScaleFactor*m_peakTimeChargeScaling*std::pow(m_a, 3)*m_pole0*m_pole1_square;
}

double VMM_Shaper::vmmResponse(const std::vector<float> &effectiveCharge, const std::vector<float> &electronsTime, double time) const{
    double response = 0;
    for (unsigned int i_electron = 0; i_electron < effectiveCharge.size(); i_electron++) {
            if (time < electronsTime.at(i_electron)) continue;
            double t = (time-electronsTime.at(i_electron))*(10^-9);
            // now follows the vmm shaper response function provided by G. Iakovidis
            // It is described in section 7.1.3 of https://cds.cern.ch/record/1955475
            double st = effectiveCharge.at(i_electron)*m_preCalculationVMMShaper*((K0*std::exp(-t*m_pole0))+(2.*m_k1_abs*std::exp(-t*m_re_pole1)*std::cos(-t*m_im_pole1+m_argK1)));
            response += st;
    }
    return response;
}

void VMM_Shaper::vmmPeakResponse(const std::vector<float> &effectiveCharge, const std::vector<float> &electronsTime, const double electronicsThreshold, double &amplitudeFirstPeak, double &timeFirstPeak) const{
    double t_peak = findPeak(effectiveCharge, electronsTime, electronicsThreshold);

    if (t_peak == -9999 ) return;  // no peak found

    amplitudeFirstPeak = vmmResponse(effectiveCharge, electronsTime, t_peak);
    timeFirstPeak = t_peak;
}


void VMM_Shaper::vmmThresholdResponse(const std::vector<float> &effectiveCharge, const std::vector<float> &electronsTime, const double electronicsThreshold, double &amplitudeAtFirstPeak, double &timeAtThreshold) const{

    double startTime = m_lowerTimeWindow;
    double minElectronTime = 9999; // find minimum of electrons times
    for(float t:electronsTime){if(t < minElectronTime){minElectronTime = t;}} //  find minimum of electrons time

    if(startTime < minElectronTime) startTime = minElectronTime; // if smallest strip times are higher then the lower time window, just start the loop from the smallest electron time

    double tmpTimeAtThreshold = -9999;
    for (double time = startTime; time < m_upperTimeWindow; time += m_timeStep) {
        if (vmmResponse(effectiveCharge, electronsTime, time) >= electronicsThreshold) {
            tmpTimeAtThreshold = time;
        }
    }

    if (tmpTimeAtThreshold == -9999) return;

    double t_peak = findPeak(effectiveCharge, electronsTime, electronicsThreshold);
    if (t_peak == -9999) return;

    timeAtThreshold = tmpTimeAtThreshold;
    amplitudeAtFirstPeak = vmmResponse(effectiveCharge, electronsTime, t_peak);
}


double VMM_Shaper::findPeak(const std::vector<float> &effectiveCharge, const std::vector<float> &electronsTime, const double electronicsThreshold) const{
    double startTime = m_lowerTimeWindow;
    double minElectronTime = 9999; // find minimum of electrons times
    for(float t:electronsTime){if(t < minElectronTime){minElectronTime = t;}} //  find minimum of electrons time

    if(startTime < minElectronTime) startTime = minElectronTime; // if smallest strip times are higher then the lower time window, just start the loop from the smallest electron time

    double oldResponse = 0;
    double oldDerivative = 0 , currentDerivative = 0;

    for (double time = startTime; time < m_upperTimeWindow; time += m_timeStep) {

        double response = vmmResponse(effectiveCharge, electronsTime, time);

        oldDerivative = currentDerivative;
        currentDerivative = (response-oldResponse)/m_timeStep;
        //  check if sign of derivative has not changed ==> no peak;  or if response is below threshold
        if (oldDerivative*currentDerivative >= 0 || oldResponse < electronicsThreshold) {oldResponse = response; continue;}

        // from here one its assumed that a peak above threshold was found

        //  check if the derivative is monoton falling within the given window of 5 bins
        bool checkDerivative = true;
        double tmp_checkOldDerivative = 0, tmp_checkCurrentDerivative = 0;
        double tmp_checkOldResponse = 0;

        int searchWindow = 5;

        for (int i_timeOfPeak = -searchWindow; i_timeOfPeak <= searchWindow; i_timeOfPeak ++) {
           double response = vmmResponse(effectiveCharge, electronsTime, time + i_timeOfPeak * m_timeStep);

           tmp_checkOldDerivative = tmp_checkCurrentDerivative;
           tmp_checkCurrentDerivative = (response - tmp_checkOldResponse)/m_timeStep;
           tmp_checkOldResponse = response;

           if (i_timeOfPeak >= -searchWindow + 2  // needs two iterations to fill the variables
                && tmp_checkOldDerivative < tmp_checkCurrentDerivative) {  // derivative is not falling monothonic
               checkDerivative = false;
               break;
           }
        }
        if (!checkDerivative) continue;
        return time - m_timeStep;
    }
    return -9999;  // no peak found
}


bool VMM_Shaper::hasChargeAboveThreshold(const std::vector<float> &effectiveCharge, const std::vector<float> &electronsTime, const double electronicsThreshold) const{
    double startTime = m_lowerTimeWindow;
    double minElectronTime = 9999; // find minimum of electrons times
    for(float t:electronsTime){if(t < minElectronTime){minElectronTime = t;}} //  find minimum of electrons time

    if(startTime < minElectronTime) startTime = minElectronTime; // if smallest strip times are higher then the lower time window, just start the loop from the smallest electron time

    for (double time = startTime; time < m_upperTimeWindow; time += m_timeStep) {
        if (vmmResponse(effectiveCharge, electronsTime, time) >= electronicsThreshold) {
           return true;
        }
    }
    return false;

}