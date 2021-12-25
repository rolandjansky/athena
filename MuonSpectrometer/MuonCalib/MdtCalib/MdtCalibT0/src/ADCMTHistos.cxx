/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#include "MdtCalibT0/ADCMTHistos.h"

#include <cmath>

#include "TDirectory.h"
#include "TF1.h"
#include "TH1.h"

namespace MuonCalib {

    /** skew normal ADC fitting
     */
    inline Double_t adcfitf_skewnormal(Double_t *x, Double_t *par) {
        // par[0] = skew gauss norm
        // par[1] = skew gauss mean (i.e. mu)
        // par[2] = skew gauss sigma (i.e sigma)
        // par[3] = skew factor  (i.e. alpha)
        // Numeric constants
        Double_t invsq2pi = 1.0 / (std::sqrt(2 * M_PI));
        Double_t twoPi = 2 * M_PI;

        Double_t delta_value = par[3] / (std::sqrt(1. + par[3] * par[3]));
        Double_t omega_square = (par[2] * par[2]) / (1. - 4. * delta_value * delta_value / (twoPi));
        Double_t omega_value = std::sqrt(omega_square);
        Double_t xi_value = par[1] - delta_value * omega_value * 2. * invsq2pi;
        Double_t Gauss_part =
            (invsq2pi / omega_value) * std::exp(-((x[0] - xi_value) * (x[0] - xi_value)) / (2.0 * omega_square));  // phi(x)

        Double_t Erf_part = 0.5 * (1 + std::erf(par[3] * (x[0] - xi_value) / (omega_value)));

        Double_t SkewNormal_Value = par[0] * 2. * Gauss_part * Erf_part;

        Double_t MyGaussFuncValue = SkewNormal_Value;

        return (MyGaussFuncValue);
    }

    void ADCMTHistos::FillA(double a) { m_adc->Fill(static_cast<Axis_t>(a)); }

    //////////////////////////////////////////////////////////
    // Initialize(int id, const T0MTSettings & settings)	//
    //////////////////////////////////////////////////////////

    void ADCMTHistos::Initialize(int id, const T0MTSettings *settings, const char *hname) {
        m_settings = settings;
        char buf[100];
        if (hname == nullptr)
            snprintf(buf, 100, "adc_spec_%d", id);
        else
            snprintf(buf, 100, "adc_spec_%s", hname);
        m_adc = new TH1F(buf, "", settings->NBinsADC(), settings->ADCMin(), settings->ADCMax());
        m_id = id;
        m_adc_ok = false;
    }

    bool ADCMTHistos::FitAdc() {
        m_adc_fit = new TF1("adc_fun", adcfitf_skewnormal, 50, 320, 4);
        Double_t average = m_adc->GetMean();
        Double_t max = m_adc->GetMaximum();
        m_adc_fit->SetParameters(max, average, 40, 1);  // initialize value
        m_adc_fit->SetLineColor(kRed);
        m_adc_fit->SetParNames("Max", "Mean", "Sigma", "Skew_factor");
        m_adc_fit->SetParLimits(0, 0, 1000000);
        m_adc_fit->SetParLimits(1, 100, 200);
        m_adc_fit->SetParLimits(2, 0, 200);
        m_adc->Fit("adc_fun", "Q+", "", 50, 320);
        m_adc_ok = true;
        return true;
    }

}  // namespace MuonCalib
