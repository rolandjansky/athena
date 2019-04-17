/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/  
  
#ifndef AthenaMonitoring_HistogramFiller_HistogramFillerProfile_h
#define AthenaMonitoring_HistogramFiller_HistogramFillerProfile_h

#include "TProfile.h"

#include "AthenaMonitoring/HistogramFiller.h"

namespace Monitored {
  /**
   * @brief Filler for profile 1D histogram 
   */
  class HistogramFillerProfile : public HistogramFiller {
  public:
    HistogramFillerProfile(const HistogramDef& definition, std::shared_ptr<IHistogramProvider> provider)
      : HistogramFiller(definition, provider) {}

    virtual HistogramFillerProfile* clone() override { return new HistogramFillerProfile(*this); };

    virtual unsigned fill() override {
      if (m_monVariables.size() != 2) {
        return 0;
      }

      unsigned i(0);
      auto histogram = this->histogram<TProfile>();
      auto valuesVector1 = m_monVariables[0].get().getVectorRepresentation();
      auto valuesVector2 = m_monVariables[1].get().getVectorRepresentation();
      std::lock_guard<std::mutex> lock(*(this->m_mutex));

      if (valuesVector1.size() != valuesVector2.size()) {
        if (valuesVector1.size() == 1) {
          // first variable is scalar -- loop over second
          for (auto value2 : valuesVector2) {
            histogram->Fill(valuesVector1[0], value2);
            ++i;
          }
        } else if (valuesVector2.size() == 1)  {
          // second variable is scalar -- loop over first
          for (auto value1 : valuesVector1) {
            histogram->Fill(value1, valuesVector2[0]); 
            ++i;
          } 
        }
      } else {
        for (i = 0; i < std::size(valuesVector1); ++i) {
          histogram->Fill(valuesVector1[i], valuesVector2[i]);
        }
      }
      
      return i;
    }
  };
}

#endif /* AthenaMonitoring_HistogramFiller_HistogramFillerProfile_h */