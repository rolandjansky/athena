/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#include "MM_Digitization/VMM_Shaper.h"
#include <cmath>
#include <algorithm>

namespace {
    // VMM shaper parameters provided by G. Iakovidis
    constexpr double K0 = 1.584;
    constexpr double Re_K1 = -0.792;
    constexpr double Im_K1 = -0.115;

    constexpr double chargeScaleFactor = 1/0.411819;

    constexpr double mmIonFlowTime = 150.;  // ns
}

VMM_Shaper::VMM_Shaper(const float peakTime,const float lowerTimeWindow,const float upperTimeWindow):m_peakTime(peakTime),
m_lowerTimeWindow(lowerTimeWindow),
m_upperTimeWindow(upperTimeWindow),
m_timeStep(0.1)
{
    m_inverseTimeStep = 1./m_timeStep;
    initialize();
}

void VMM_Shaper::initialize() {
    // hardcoded vmm shaper values are provided by G. Iakovidis
    m_a = (m_peakTime*(1e-9))/1.5;
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

    m_pole0_ns = m_pole0*(1e-9);
    m_re_pole1_ns = m_re_pole1*(1e-9);
    m_im_pole1_ns = m_im_pole1*(1e-9);
}

double VMM_Shaper::vmmResponse(const std::vector<float> &effectiveCharge, const std::vector<float> &electronsTime, double time) const{
    double response = 0;
    for (unsigned int i_electron = 0; i_electron < effectiveCharge.size(); i_electron++) {
            if (time < electronsTime.at(i_electron)) continue;
            double t = (time-electronsTime.at(i_electron));
            // now follows the vmm shaper response function provided by G. Iakovidis
            // It is described in section 7.1.3 of https://cds.cern.ch/record/1955475
            double st = effectiveCharge.at(i_electron)*m_preCalculationVMMShaper*((K0*std::exp(-t*m_pole0_ns))+(2.*m_k1_abs*std::exp(-t*m_re_pole1_ns)*std::cos(-t*m_im_pole1_ns+m_argK1)));
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


void VMM_Shaper::vmmThresholdResponse(const std::vector<float> &effectiveCharge, const std::vector<float> &electronsTime, const double electronicsThreshold, double &amplitudeAtFirstPeak, double &timeAtThreshold) const {
    if (!aboveThresholdSimple(effectiveCharge, electronsTime, electronicsThreshold)) return;

    if (effectiveCharge.empty()) return;  // protect min_element
    double startTime = m_lowerTimeWindow;
    double minElectronTime = *std::min_element(electronsTime.begin(), electronsTime.end());
    if (startTime < minElectronTime) startTime = minElectronTime;  // if smallest strip times are higher then the lower time window, just start the loop from the smallest electron time
    
    double tmpTimeAtThreshold = -9999;
    
    for (double time = startTime; time < minElectronTime + 0.9*m_peakTime; time += 10*m_timeStep) { // quick search till the first possible peak
      if(vmmResponse(effectiveCharge, electronsTime, time) <= electronicsThreshold ) continue;
      for(double fineTime = time; fineTime >= time - 10*m_timeStep; fineTime -= m_timeStep) { // since value above threshold was found, loop back in time to find the crossing with the timeStep precission
          if(vmmResponse(effectiveCharge, electronsTime, fineTime) >= electronicsThreshold) continue;
              tmpTimeAtThreshold = fineTime + 0.5*m_timeStep; //  get time between time above and time below threshold 
              break;
          }
      break;
    } 
    
    if(tmpTimeAtThreshold == -9999) {  // threshold crossing not yet found 
        // check if first possible peak was before start time
        double tmpStartTime = std::max(minElectronTime + 0.9*m_peakTime, startTime); 
        for (double time = tmpStartTime; time < m_upperTimeWindow; time += m_timeStep) {
          if (vmmResponse(effectiveCharge, electronsTime, time) >= electronicsThreshold) {
              tmpTimeAtThreshold = time;
              break;
          }
       } 
    
    }

    if (tmpTimeAtThreshold == -9999) return;

    double t_peak = findPeak(effectiveCharge, electronsTime, electronicsThreshold);
    if (t_peak == -9999) return;

    timeAtThreshold = tmpTimeAtThreshold;
    amplitudeAtFirstPeak = vmmResponse(effectiveCharge, electronsTime, t_peak);
}


double VMM_Shaper::findPeak(const std::vector<float> &effectiveCharge, const std::vector<float> &electronsTime, const double electronicsThreshold) const{
    if(effectiveCharge.empty()) return -9999; // protect min_element
    double startTime = m_lowerTimeWindow;
    double minElectronTime = *std::min_element(electronsTime.begin(), electronsTime.end());

    minElectronTime += 0.8*m_peakTime;  // only start looking for the peak close to the first possible peak
    if(startTime < minElectronTime) startTime = minElectronTime; // if smallest strip times are higher then the lower time window, just start the loop from the smallest electron time

    double oldResponse = 0;
    //double currentDerivative = 0;

    double timeStepScaleFactor = 5.0;

    for (double time = startTime; time < m_upperTimeWindow; time += m_timeStep*timeStepScaleFactor) {

        double response = vmmResponse(effectiveCharge, electronsTime, time);
        if(oldResponse < response){oldResponse=response; continue;}
        oldResponse = response;

        int searchWindow = 5;

        std::vector<double> tmpTime, tmpResponse;

        tmpTime.reserve(2*timeStepScaleFactor);
        tmpResponse.reserve(2*timeStepScaleFactor);


        for(double fineTime = (time-1.5*m_timeStep*timeStepScaleFactor); fineTime < time+0.5*m_timeStep*timeStepScaleFactor; fineTime += m_timeStep) {
          tmpTime.push_back(fineTime);
          tmpResponse.push_back(vmmResponse(effectiveCharge,electronsTime,fineTime));     
        }

        int nBins = tmpTime.size();

        for(int i_time = 1; i_time < nBins-1;i_time++){
            if(tmpResponse.at(i_time)<tmpResponse.at(i_time+1)) continue;

            if(tmpResponse.at(i_time) < electronicsThreshold) break; 
            
            bool checkTimeWindow = false;
            for(int i_timeOfPeak = i_time - searchWindow + 1; i_timeOfPeak <= i_time + searchWindow; i_timeOfPeak++) {
              if(i_timeOfPeak < 1 || i_timeOfPeak == nBins - 1) continue;
              
              double oldDerivative = (tmpResponse.at(i_time) - tmpResponse.at(i_time-1));
              double newDerivative = (tmpResponse.at(i_time+1) - tmpResponse.at(i_time));
              if (newDerivative > oldDerivative){
                checkTimeWindow = false;
                break; 
              } else {
                checkTimeWindow = true;
              }
            
           }
           if(checkTimeWindow) return tmpTime.at(i_time);

        }
    }
    return -9999;  // no peak found
}


bool VMM_Shaper::hasChargeAboveThreshold(const std::vector<float> &effectiveCharge, const std::vector<float> &electronsTime, const double electronicsThreshold) const{
    if (!aboveThresholdSimple(effectiveCharge, electronsTime, electronicsThreshold)) return false;

    if (effectiveCharge.empty()) return false;  // protect min_element
    double startTime = m_lowerTimeWindow;
    double minElectronTime = *std::min_element(electronsTime.begin(), electronsTime.end());
    // since we are only checking if signal is above threshold, we can start searching close to the peak
    minElectronTime += m_peakTime * 0.8;
    if (startTime < minElectronTime) startTime = minElectronTime;  // if smallest strip times are higher then the lower time window, just start the loop from the smallest electron time

    for (double time = startTime; time < m_upperTimeWindow; time += m_timeStep) {
        if (vmmResponse(effectiveCharge, electronsTime, time) >= electronicsThreshold) {
           return true;
        }
    }
    return false;

}


bool VMM_Shaper::aboveThresholdSimple(const std::vector<float> &effectiveCharge, const std::vector<float> &electronsTime, const double electronicsThreshold) const {
    // check if total strip charge is above threshold, otherwise skip VMM
    float chargeSum = 0;
    for (unsigned int i_elec = 0; i_elec < effectiveCharge.size(); i_elec++) {
        if (electronsTime.at(i_elec) >= m_lowerTimeWindow - m_peakTime && electronsTime.at(i_elec) <= m_upperTimeWindow) {
            chargeSum += effectiveCharge.at(i_elec)*m_peakTimeChargeScaling;
        }
    }
    return chargeSum >= electronicsThreshold;
}
