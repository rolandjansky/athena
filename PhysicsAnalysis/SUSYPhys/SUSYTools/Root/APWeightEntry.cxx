/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#define APWeightEntry_cxx
#include "SUSYTools/APWeightEntry.h"
//#include "MathTools.h"
#include <cmath>
#include <limits>
#include "TH1F.h"
#include "TRandom.h"
#include "TMath.h"

#ifndef SUSY_ATHENA
ClassImp(APWeightEntry)
#endif

using namespace std;

APWeightEntry::APWeightEntry() {
  _hist = 0;
  _cumul = 0;
  _pdf = 0;
  _bins = 0;
}

APWeightEntry::APWeightEntry(unsigned int val_denominator, unsigned int val_numerator, double scale, bool isTrig) {
  _val_denominator = val_denominator;
  double val_denominator_f = (double) val_denominator;
  _val_numerator = val_numerator;
  double val_numerator_f = (double) val_numerator;
  _sys_uncert = 0.;
  _sys_uncert2 = 0.;
  _integral = 0.;
  _hist = 0;
  _cumul = 0;
  _pdf = new double[1000];
  _bins = new double[1001];

  if (!isTrig) {
    if (_val_numerator == 0 || _val_denominator == 0) {
      _expectancy_val = 0.;
      _variance = 0.;
      _stat_uncert_low = 0.;
      _stat_uncert_high = 0.;
      _is_nan = true;
    } else {
      _is_nan = false;
      
      _expectancy_val = scale * (val_numerator_f / val_denominator_f);
      _variance = (val_numerator_f / (val_denominator_f * val_denominator_f)) + ((val_numerator_f * val_numerator_f) / (val_denominator_f * val_denominator_f * val_denominator_f));
      _stat_uncert_low = _variance;
      _stat_uncert_high = _variance;

      double sig_low = (std::max((double) 0.0, (_expectancy_val - 5. * scale * sqrt(_variance))));
      double sig_high = (_expectancy_val + 5. * scale * sqrt(_variance));
      double step = (sig_high - sig_low) / 1000.;
      double shift = val_numerator_f * log(_expectancy_val / scale)+(-val_numerator_f - val_denominator_f - 2.) * log((_expectancy_val / scale) + 1.);
      //double shift = ln_factorialApp(_val_numerator);

      double temp_value = sig_low;
      double arr_position = sig_low;

      for (int i = 0; i < 1000; ++i) {
        temp_value = exp(_val_numerator * log(arr_position / scale)+(-val_numerator_f - val_denominator_f - 2.) * log((arr_position / scale) + 1.) - shift);
        if (temp_value != temp_value) temp_value = 0.;
        _pdf[i] = temp_value;
        _integral += temp_value;
        _bins[i] = arr_position;
        arr_position += step;
      }
      _bins[1000] = arr_position;
    }
  } else {
    if (_val_denominator == 0) {
      _expectancy_val = 0.;
      _variance = 0.;
      _stat_uncert_low = 0.;
      _stat_uncert_high = 0.;
      _is_nan = true;
    } else {
      _is_nan = false;

      _expectancy_val = val_numerator_f / val_denominator_f;
      _variance = (_expectancy_val * (1. - _expectancy_val)) / val_denominator_f;
      double _var_sqrt = sqrt(_variance);
      double sig_low = std::max(0., _expectancy_val - 5. * _var_sqrt);
      double sig_high = std::min(1., _expectancy_val + 5. * _var_sqrt);
      if (val_numerator_f == 0) {
        sig_low = 0.;
        sig_high = std::min(1., (8. / val_denominator_f));
      }
      if (val_numerator_f == val_denominator_f) {
        sig_low = std::max(0., (1. - 8. / val_denominator_f));
        sig_high = 1.;
      }

      double step = fabs(sig_high - sig_low) / 1000.;
      double shift = 0.;
      if (val_numerator_f == 0 || val_numerator_f == val_denominator_f) {
        shift = 0.;
      } else {
        shift = val_numerator_f * log(_expectancy_val) + (val_denominator_f - val_numerator_f) * log(1. - _expectancy_val);
      }

      double temp_value = sig_low;
      double arr_position = sig_low;

      if (val_numerator_f == 0) {
        for (int i = 0; i < 1000; ++i) {
          temp_value = exp(val_denominator_f * log(1. - arr_position) - shift);
          _pdf[i] = temp_value;
          _integral += temp_value;
          _bins[i] = arr_position;
          arr_position += step;
        }
      } else {
        if (val_numerator_f == val_denominator_f) {
          for (int i = 0; i < 1000; ++i) {
            temp_value = exp(val_numerator_f * log(arr_position) - shift);
            _pdf[i] = temp_value;
            _integral += temp_value;
            _bins[i] = arr_position;
            arr_position += step;
          }
        } else {
          for (int i = 0; i < 1000; ++i) {
            temp_value = exp(val_numerator_f * log(arr_position) + (val_denominator_f - val_numerator_f) * log(1. - arr_position) - shift);
            _pdf[i] = temp_value;
            _integral += temp_value;
            _bins[i] = arr_position;
            arr_position += step;
          }
        }
      }
      _bins[1000] = arr_position;
    }
  }

}

void APWeightEntry::ReadEfficiency(double efficiency, double err_low, double err_high) {
  _val_denominator = 0;
  _val_numerator = 0;
  _sys_uncert = 0.;
  _sys_uncert2 = 0.;
  _expectancy_val = efficiency;
  _variance = 0.;
  _stat_uncert_low = err_low;
  _stat_uncert_high = err_high;
}

APWeightEntry::~APWeightEntry() {
  delete [] _pdf;
  _pdf = 0;
  delete [] _bins;
  _bins = 0;
  delete [] _cumul;
  _cumul = 0;
  delete _hist;
}

unsigned int APWeightEntry::GetValDenominator() {
  return _val_denominator;
}

unsigned int APWeightEntry::GetValNumerator() {
  return _val_numerator;
}

double APWeightEntry::GetExpectancy() {
  return _expectancy_val;
}

double APWeightEntry::GetVariance() {
  return _variance;
}

double APWeightEntry::GetStatUncertLow() {
  return _stat_uncert_low;
}

double APWeightEntry::GetStatUncertHigh() {
  return _stat_uncert_high;
}

double APWeightEntry::GetSysUncert() {
  return _sys_uncert;
}

double APWeightEntry::GetSysUncert2() {
  return _sys_uncert2;
}

double APWeightEntry::GetRandom() {
  if (_is_nan || _integral < std::numeric_limits<double>::epsilon()) {
    return 1e-100;
  }
  if (!_cumul) _ComputeCum();
  double rn = gRandom->Rndm();
  int ibin = TMath::BinarySearch(1000, _cumul, rn);
  return _bins[ibin];
  //if (rn > _cumul[ibin]) x += fabs(_bins[1]-_bins[0]) * (rn-_cumul[ibin])/(_cumul[ibin+1] - _cumul[ibin]);
}

TH1F* APWeightEntry::GetPDF() {
  if (!_hist) _CreateHist();
  return _hist;
}

void APWeightEntry::SetSystUncert(double rel_uncert) {
  _sys_uncert = _expectancy_val*rel_uncert;
  _sys_uncert2 = _sys_uncert*_sys_uncert;
}

void APWeightEntry::_CreateHist() {
  _hist = new TH1F("", "", 1000, _bins);
  if (_is_nan == false) {
    for (int i = 0; i < 1000; ++i) {
      _hist->SetBinContent(i + 1, _pdf[i]);
    }
  }
}

void APWeightEntry::_ComputeCum() {
  if (_cumul) delete [] _cumul;
  _cumul = new double[1001];
  _cumul[0] = 0.;
  for (int i = 0; i < 1000; ++i) {
    _cumul[i + 1] = _cumul[i] + _pdf[i];
  }
  double normalize = _cumul[1000];
  for (int i = 1; i < 1000; ++i) {
    _cumul[i] /= normalize;
  }
}

#ifndef SUSY_ATHENA
void APWeightEntry::Streamer(TBuffer &b)
{
   // Stream an APWeightEntry object.
   if (b.IsReading()) {
      b >> _val_denominator;
      b >> _val_numerator;
      b >> _expectancy_val;
      b >> _variance;
      b >> _stat_uncert_low;
      b >> _stat_uncert_high;
      b >> _sys_uncert;
      b >> _sys_uncert2;
      b >> _is_nan;
      b >> _integral;
      if (_pdf == 0) _pdf = new double[1000];
      if (_bins == 0) _bins = new double[1001];
      for (int i = 0; i < 1000; ++i) {
        b >> _pdf[i];
      }
      for (int i = 0; i < 1001; ++i) {
        b >> _bins[i];
      }
   } else {
      b << _val_denominator;
      b << _val_numerator;
      b << _expectancy_val;
      b << _variance;
      b << _stat_uncert_low;
      b << _stat_uncert_high;
      b << _sys_uncert;
      b << _sys_uncert2;
      b << _is_nan;
      b << _integral;
      for (int i = 0; i < 1000; ++i) {
        b << _pdf[i];
      }
      for (int i = 0; i < 1001; ++i) {
        b << _bins[i];
      }
   }

}
#endif
