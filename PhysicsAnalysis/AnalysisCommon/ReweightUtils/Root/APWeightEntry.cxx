/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#define APWeightEntry_cxx
#include "ReweightUtils/APWeightEntry.h"
#include "ReweightUtils/MathTools.h"
#include <cmath>
#include <limits>
#include <vector>
#include "TH1F.h"
#include "TRandom.h"
#include "TMath.h"

using namespace std;

APWeightEntry::APWeightEntry()
  : m_val_denominator(0),
    m_val_numerator(0),
    m_expectancy_val(0),
    m_variance(0),
    m_stat_uncert_low(0),
    m_stat_uncert_high(0),
    m_sys_uncert(0),
    m_sys_uncert2(0),
    m_is_trig(0),
    m_is_nan(0),
    m_integral(0)
{
  m_hist = 0;
  m_cumul = 0;
  m_pdf = 0;
  m_bins = 0;
  m_ID = 0;
  //m_coords = 0;
}

APWeightEntry::APWeightEntry(unsigned int val_denominator, unsigned int val_numerator, double scale, bool isTrig)
  : m_stat_uncert_low(0),
    m_stat_uncert_high(0)
{
  m_val_denominator = val_denominator;
  double val_denominator_f = (double) val_denominator;
  m_val_numerator = val_numerator;
  double val_numerator_f = (double) val_numerator;
  m_sys_uncert = 0.;
  m_sys_uncert2 = 0.;
  m_integral = 0.;
  m_hist = 0;
  m_cumul = 0;
  m_bins = 0;
  m_pdf = 0;
  m_is_trig = isTrig;
  m_ID = 0;
  //m_coords = 0;

  if (!isTrig) {
    if (m_val_numerator == 0 || m_val_denominator == 0) {
      m_expectancy_val = 0.;
      m_variance = 0.;
      m_stat_uncert_low = 0.;
      m_stat_uncert_high = 0.;
      m_is_nan = true;
    } else {
      m_is_nan = false;
      m_pdf = new double[1000];
      m_bins = new double[1001];

      m_expectancy_val = scale * (val_numerator_f / val_denominator_f);
      m_variance = (val_numerator_f / (val_denominator_f * val_denominator_f)) + ((val_numerator_f * val_numerator_f) / (val_denominator_f * val_denominator_f * val_denominator_f));
      m_stat_uncert_low = m_variance;
      m_stat_uncert_high = m_variance;

      double sig_low = (std::max((double) 0.0, (m_expectancy_val - 5. * scale * sqrt(m_variance))));
      double sig_high = (m_expectancy_val + 5. * scale * sqrt(m_variance));
      double step = (sig_high - sig_low) * 1e-3;
      const double inv_scale = 1. / scale;
      double shift = val_numerator_f * log(m_expectancy_val * inv_scale)+(-val_numerator_f - val_denominator_f - 2.) * log((m_expectancy_val * inv_scale) + 1.);
      //double shift = ln_factorialApp(m_val_numerator);

      double temp_value = sig_low;
      double arr_position = sig_low;

      for (int i = 0; i < 1000; ++i) {
        temp_value = exp(m_val_numerator * log(arr_position * inv_scale)+(-val_numerator_f - val_denominator_f - 2.) * log((arr_position * inv_scale) + 1.) - shift);
        if (temp_value != temp_value) temp_value = 0.;
        m_pdf[i] = temp_value;
        m_integral += temp_value;
        m_bins[i] = arr_position;
        arr_position += step;
      }
      m_bins[1000] = arr_position;
    }
  } else {
    if (m_val_denominator == 0) {
      m_expectancy_val = 0.;
      m_variance = 0.;
      m_stat_uncert_low = 0.;
      m_stat_uncert_high = 0.;
      m_is_nan = true;
    } else {
      m_is_nan = false;
      m_pdf = new double[1000];
      m_bins = new double[1001];

      const double inv_val_denominator_f = 1. / val_denominator_f;
      m_expectancy_val = val_numerator_f * inv_val_denominator_f;
      m_variance = (m_expectancy_val * (1. - m_expectancy_val)) * inv_val_denominator_f;
      double var_sqrt = sqrt(m_variance);
      double sig_low = std::max(0., m_expectancy_val - 5. * var_sqrt);
      double sig_high = std::min(1., m_expectancy_val + 5. * var_sqrt);
      if (val_numerator_f == 0) {
        sig_low = 0.;
        sig_high = std::min(1., (8. * inv_val_denominator_f));
      }
      if (val_numerator_f == val_denominator_f) {
        sig_low = std::max(0., (1. - 8. * inv_val_denominator_f));
        sig_high = 1.;
      }

      double step = fabs(sig_high - sig_low) * 1e-3;
      double shift = 0.;
      if (val_numerator_f == 0 || val_numerator_f == val_denominator_f) {
        shift = 0.;
      } else {
        shift = val_numerator_f * log(m_expectancy_val) + (val_denominator_f - val_numerator_f) * log(1. - m_expectancy_val);
      }

      double temp_value = sig_low;
      double arr_position = sig_low;

      if (val_numerator_f == 0) {
        for (int i = 0; i < 1000; ++i) {
          temp_value = exp(val_denominator_f * log(1. - arr_position) - shift);
          m_pdf[i] = temp_value;
          m_integral += temp_value;
          m_bins[i] = arr_position;
          arr_position += step;
        }
      } else {
        if (val_numerator_f == val_denominator_f) {
          for (int i = 0; i < 1000; ++i) {
            temp_value = exp(val_numerator_f * log(arr_position) - shift);
            m_pdf[i] = temp_value;
            m_integral += temp_value;
            m_bins[i] = arr_position;
            arr_position += step;
          }
        } else {
          for (int i = 0; i < 1000; ++i) {
            temp_value = exp(val_numerator_f * log(arr_position) + (val_denominator_f - val_numerator_f) * log(1. - arr_position) - shift);
            m_pdf[i] = temp_value;
            m_integral += temp_value;
            m_bins[i] = arr_position;
            arr_position += step;
          }
        }
      }
      m_bins[1000] = arr_position;
    }
  }

}

void APWeightEntry::ReadEfficiency(double efficiency, double err_low, double err_high) {
  m_val_denominator = 0;
  m_val_numerator = 0;
  m_sys_uncert = 0.;
  m_sys_uncert2 = 0.;
  m_expectancy_val = efficiency;
  m_stat_uncert_low = fabs(err_low);
  m_stat_uncert_high = fabs(err_high);
  m_variance = (m_stat_uncert_low + m_stat_uncert_high) / 2.0;
  m_variance *= m_variance;
}

void APWeightEntry::SetCoordinates(std::vector<int> coords, std::vector<int> n_dim_origin) {
  m_coords.clear();
  m_n_dim_origin.clear();
  m_coords = coords;
  m_n_dim_origin = n_dim_origin;
}

APWeightEntry::~APWeightEntry() {
  delete [] m_pdf;
  m_pdf = 0;
  delete [] m_bins;
  m_bins = 0;
  delete [] m_cumul;
  m_cumul = 0;
  delete m_hist;
}

unsigned int APWeightEntry::GetValDenominator() const {
  return m_val_denominator;
}

unsigned int APWeightEntry::GetValNumerator() const {
  return m_val_numerator;
}

double APWeightEntry::GetExpectancy() const {
  return m_expectancy_val;
}

double APWeightEntry::GetVariance() const {
  return m_variance;
}

double APWeightEntry::GetStatUncertLow() const {
  return m_stat_uncert_low;
}

double APWeightEntry::GetStatUncertHigh() const {
  return m_stat_uncert_high;
}

double APWeightEntry::GetSysUncert() const {
  return m_sys_uncert;
}

double APWeightEntry::GetSysUncert2() const {
  return m_sys_uncert2;
}

double APWeightEntry::GetRandom() {
  if (m_is_nan || m_integral < std::numeric_limits<double>::epsilon()) {
    return 1e-100;
  }
  if (!m_cumul) _ComputeCum();
  double rn = gRandom->Rndm();
  int ibin = TMath::BinarySearch(1000, m_cumul, rn);
  return m_bins[ibin];
  //if (rn > m_cumul[ibin]) x += fabs(m_bins[1]-m_bins[0]) * (rn-m_cumul[ibin])/(m_cumul[ibin+1] - m_cumul[ibin]);
}

TH1F* APWeightEntry::GetPDF() {
  if (!m_hist) _CreateHist();
  return m_hist;
}

unsigned int APWeightEntry::GetID() const {
  return m_ID;
}

vector<int> APWeightEntry::GetCoords() const {
  return m_coords;
}

vector< int > APWeightEntry::GetOriginalDimensions() const {
  return m_n_dim_origin;
}

bool APWeightEntry::IsNaN() const {
  return m_is_nan;
}

bool APWeightEntry::IsTrig() const {
  return m_is_trig;
}

void APWeightEntry::SetSystUncert(double rel_uncert) {
  m_sys_uncert = m_expectancy_val*rel_uncert;
  m_sys_uncert2 = m_sys_uncert*m_sys_uncert;
}

void APWeightEntry::SetID(unsigned int id) {
  m_ID = id;
}

void APWeightEntry::_CreateHist() {
  m_hist = new TH1F("", "", 1000, m_bins);
  if (m_is_nan == false) {
    for (int i = 0; i < 1000; ++i) {
      m_hist->SetBinContent(i + 1, m_pdf[i]);
    }
  }
}

void APWeightEntry::_ComputeCum() {
  if (m_cumul) delete [] m_cumul;
  m_cumul = new double[1001];
  m_cumul[0] = 0.;
  for (int i = 0; i < 1000; ++i) {
    m_cumul[i + 1] = m_cumul[i] + m_pdf[i];
  }
  const double inv_normalize = 1. / m_cumul[1000];
  for (int i = 1; i < 1000; ++i) {
    m_cumul[i] *= inv_normalize;
  }
}
