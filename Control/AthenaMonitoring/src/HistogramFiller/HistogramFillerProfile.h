/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/  
  
#ifndef AthenaMonitoring_HistogramFiller_HistogramFillerProfile_h
#define AthenaMonitoring_HistogramFiller_HistogramFillerProfile_h

#include "TProfile.h"

#include "AthenaMonitoring/HistogramFiller/HistogramFiller.h"

namespace Monitored {
  /**
   * @brief Filler for profile 1D histogram 
   */
  class HistogramFillerProfile : public HistogramFiller {
  public:
    HistogramFillerProfile(TProfile* hist, const HistogramDef& histDef)
      : HistogramFiller(hist, histDef) {};

    virtual HistogramFillerProfile* clone() override { return new HistogramFillerProfile(*this); };

    virtual unsigned fill() override {
      using namespace std;

      if (m_monVariables.size() != 2) {
        return 0;
      }

      unsigned i(0);
      auto hist = histogram();
      auto valuesVector1 = m_monVariables[0].get().getVectorRepresentation();
      auto valuesVector2 = m_monVariables[1].get().getVectorRepresentation();
      lock_guard<mutex> lock(*(this->m_mutex));

      if (valuesVector1.size() != valuesVector2.size()) {
        if (valuesVector1.size() == 1) {
          // first variable is scalar -- loop over second
          for (auto value2 : valuesVector2) {
            hist->Fill(valuesVector1[0], value2);
            ++i;
          }
        } else if (valuesVector2.size() == 1)  {
          // second variable is scalar -- loop over first
          for (auto value1 : valuesVector1) {
            hist->Fill(value1, valuesVector2[0]); 
            ++i;
          } 
        }
      } else {
        for (i = 0; i < valuesVector1.size(); ++i) {
          hist->Fill(valuesVector1[i], valuesVector2[i]);
        }
      }
      
      return i;
    }
  protected:
    virtual TProfile* histogram() override { return static_cast<TProfile*>(m_hist); }
  };
}

#endif /* AthenaMonitoring_HistogramFiller_HistogramFillerProfile_h */