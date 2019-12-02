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
      : HistogramFiller(definition, provider) {
    }

    virtual unsigned fill() override {

      if (m_monVariables.size() != 1) { return 0; }

      std::function<double(size_t)> weightValue = [] (size_t ){ return 1.0; };  // default is always 1.0
      std::vector<double> weightVector;
      if ( m_monWeight != nullptr ) {
	weightVector = m_monWeight->getVectorRepresentation();
	weightValue = [&](size_t i){ return weightVector[i]; };
      }

      if ( not m_monVariables.at(0).get().hasStringRepresentation() ) {
	const auto valuesVector = m_monVariables.at(0).get().getVectorRepresentation();
	fill( std::size( valuesVector), [&](size_t i){ return valuesVector[i]; }, weightValue );
	return std::size( valuesVector );
      } else {
	const auto valuesVector = m_monVariables.at(0).get().getStringVectorRepresentation();
	fill( std::size( valuesVector ), [&](size_t i){ return valuesVector[i].c_str(); }, weightValue );
	return std::size( valuesVector );
      }
    }

  protected:

    template<typename F1, typename F2>
    void fill( size_t n, F1 f1, F2 f2 ) {

      std::lock_guard<std::mutex> lock(*(this->m_mutex));
      auto histogram = this->histogram<TH1>();
      for ( size_t i = 0; i < n; ++i ) {
	histogram->Fill( f1(i), f2(i) );
      }
    }
  };
}

#endif /* AthenaMonitoringKernel_HistogramFiller_HistogramFiller1D_h */
