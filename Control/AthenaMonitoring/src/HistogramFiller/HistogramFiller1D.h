/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/  
  
#ifndef AthenaMonitoring_HistogramFiller_HistogramFiller1D_h
#define AthenaMonitoring_HistogramFiller_HistogramFiller1D_h

#include "TH1.h"

#include "AthenaMonitoring/HistogramFiller.h"
#include <boost/range/combine.hpp>

namespace Monitored {
  /**
   * @brief Filler for plain 1D histograms
   */
  class HistogramFiller1D : public HistogramFiller {
  public: 
    HistogramFiller1D(const HistogramDef& definition, std::shared_ptr<IHistogramProvider> provider)
      : HistogramFiller(definition, provider) {}

    HistogramFiller1D* clone() override { return new HistogramFiller1D(*this); };

    virtual unsigned fill() override {
      if (m_monVariables.size() != 1) {
        return 0;
      }

      auto histogram = this->histogram<TH1>();
      auto valuesVector = m_monVariables[0].get().getVectorRepresentation();

      std::lock_guard<std::mutex> lock(*(this->m_mutex));

      if ( m_monWeight && m_monWeight->getVectorRepresentation().size()==valuesVector.size() ) {
        // When using weights, get the weight vector and fill
        auto weightVector = m_monWeight->getVectorRepresentation();
        double value,weight;
        for (const auto& zipped : boost::combine(valuesVector,weightVector)) {
          boost::tie(value,weight) = zipped;
          histogram->Fill(value,weight);
        }
      } else {
        // Otherwise, simply loop over the values and fill
        for (auto value : valuesVector) {
          histogram->Fill(value);
        }
      }

      return std::size(valuesVector);
    } 
  };
}

#endif /* AthenaMonitoring_HistogramFiller_HistogramFiller1D_h */