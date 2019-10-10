/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/  
  
#ifndef AthenaMonitoringKernel_HistogramFiller_HistogramFiller1D_h
#define AthenaMonitoringKernel_HistogramFiller_HistogramFiller1D_h

#include "TH1.h"

#include "AthenaMonitoringKernel/HistogramFiller.h"
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
      if (m_monVariables.size() != 1) { return 0; }

      std::lock_guard<std::mutex> lock(*(this->m_mutex));

      return fillHistogram();
    } 
    
  protected:
    unsigned fillHistogram() {
      if (shouldUseWeightedFill()) {
        return weightedFill();
      } else {
        return simpleFill();
      }
    }

    bool shouldUseWeightedFill() {
      if (!m_monWeight) { return false; }

      auto valuesVector = m_monVariables[0].get().getVectorRepresentation();
      auto weightsVector = m_monWeight->getVectorRepresentation();

      return std::size(valuesVector) == std::size(weightsVector);
    }

    unsigned simpleFill() {
      auto histogram = this->histogram<TH1>();
      if ( not m_monVariables[0].get().hasStringRepresentation() ) {
	auto valuesVector = m_monVariables[0].get().getVectorRepresentation();
	for ( double value : valuesVector) {
	  histogram->Fill(value);
	}
	return std::size(valuesVector);
      } 

      auto valuesVector = m_monVariables[0].get().getStringVectorRepresentation();
      for (const std::string& value : valuesVector) {
	histogram->Fill(value.c_str(), 1.);
      }
      return std::size(valuesVector);	
    }

    unsigned weightedFill() {
      if ( m_monVariables[0].get().hasStringRepresentation() )
	throw std::runtime_error("Weighted filling with strings is not supported");
      auto histogram = this->histogram<TH1>();
      auto valuesVector = m_monVariables[0].get().getVectorRepresentation();
      auto weightVector = m_monWeight->getVectorRepresentation();
      double value, weight;

      for (const auto& zipped : boost::combine(valuesVector, weightVector)) {
        boost::tie(value, weight) = zipped;
        histogram->Fill(value, weight);
      }

      return std::size(valuesVector);
    }
  };
}

#endif /* AthenaMonitoringKernel_HistogramFiller_HistogramFiller1D_h */

