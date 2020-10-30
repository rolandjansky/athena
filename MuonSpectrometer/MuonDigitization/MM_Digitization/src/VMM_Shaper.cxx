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

VMM_Shaper::VMM_Shaper(float peakTime):m_peakTime(peakTime),
m_timeStep(0.1),
m_maxTime(700.0)
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

    m_nBins = m_maxTime/m_timeStep;
    // scale factor for charge taking into account the mm ion flow time of ~150ns
    // if the peaking time is lower then that, only a fration of the total charge is integrated
    m_peakTimeChargeScaling = (m_peakTime < mmIonFlowTime ? 1.0*m_peakTime/mmIonFlowTime : 1.0);
}

void VMM_Shaper::vmmResponse(const std::vector<float> &effectiveCharge, const std::vector<float> &electronsTime, TH1F &response) const {
    for (uint i_electron = 0; i_electron < effectiveCharge.size(); i_electron++) {
        for (double ti = electronsTime[i_electron]; ti < m_maxTime; ti += m_timeStep) {
            double t = (ti-electronsTime.at(i_electron))*(10^-9);
            // now follows the vmm shaper response function provided by G. Iakovidis
            // It is described in section 7.1.3 of https://cds.cern.ch/record/1955475
            double st = effectiveCharge.at(i_electron)*chargeScaleFactor*m_peakTimeChargeScaling*std::pow(m_a, 3)*m_pole0*m_pole1_square*((K0*std::exp(-t*m_pole0))+(2.*m_k1_abs*std::exp(-t*m_re_pole1)*std::cos(-t*m_im_pole1+m_argK1)));
            response.Fill(ti, st);
        }
    }
}

void VMM_Shaper::vmmPeakResponse(const std::vector<float> effectiveCharge, const std::vector<float> electronsTime, const double electronicsThreshold, double &amplitudeFirstPeak, double &timeFirstPeak) const {
    TH1F response("response", "response", m_nBins, 0, m_maxTime);

    vmmResponse(effectiveCharge, electronsTime, response);
    int i_bin_peak = findPeak(response, electronicsThreshold);

    if (i_bin_peak == -1) return;  // no peak found

    timeFirstPeak = response.GetXaxis()->GetBinCenter(i_bin_peak);
    amplitudeFirstPeak = response.GetBinContent(i_bin_peak);
}


void VMM_Shaper::vmmThresholdResponse(const std::vector<float> effectiveCharge, const std::vector<float> electronsTime, const double electronicsThreshold, double &amplitudeAtFirstPeak, double &timeAtThreshold) const {
    TH1F response("response", "response", m_nBins, 0, m_maxTime);

    vmmResponse(effectiveCharge, electronsTime, response);
    int i_bin_peak = findPeak(response, electronicsThreshold);

    int binAboveThreshold = response.FindFirstBinAbove(electronicsThreshold);
    if (binAboveThreshold == -1) return;
    timeAtThreshold = response.GetXaxis()->GetBinCenter(binAboveThreshold);
    amplitudeAtFirstPeak = response.GetBinContent(i_bin_peak);
}


int VMM_Shaper::findPeak(const TH1F &response, const double &electronicsThreshold) const{
    TH1F derivative("derivative", "derivative", m_nBins, 0, m_maxTime);

    // First the derivative gets calculated, then its zero crossing is searched for
    // if in 4 bins around the 0 crossing the derivative is falling, it is accepted as peak

    for(int i_bin = 1; i_bin<m_nBins; i_bin++){
        derivative.SetBinContent(i_bin, (response.GetBinContent(i_bin+1) - response.GetBinContent(i_bin)) / response.GetXaxis()->GetBinWidth(i_bin));
    }

    for (int i_bin = 1; i_bin < m_nBins; i_bin++) {
        if (derivative.GetBinContent(i_bin) * derivative.GetBinContent(i_bin + 1) > 0.0) continue;  //  continue loop if no 0 crossing was there
        if (response.GetBinContent(i_bin + 1) < electronicsThreshold) continue;  //  continue if peak is below threshold
        bool derivativeCut = true;
        for (int j_bin = i_bin-2; j_bin <= i_bin + 2; j_bin++) {
            if (derivative.GetBinContent(j_bin) <= derivative.GetBinContent(j_bin+1)) {  // check that the derivative is falling for 4 bins around the 0 crossing
                derivativeCut = false;
            }
            if (derivativeCut) {
                return i_bin + 1;
            }
        }
    }
    return -1;
}


bool VMM_Shaper::hasChargeAboveThreshold(const std::vector<float> &effectiveCharge, const std::vector<float> &electronsTime, const double &electronicsThreshold) const {
    TH1F response("response", "response", m_nBins, 0, m_maxTime);
    vmmResponse(effectiveCharge, electronsTime, response);
    int i_aboveThreshold = response.FindFirstBinAbove(electronicsThreshold);
    return i_aboveThreshold > 0;
}