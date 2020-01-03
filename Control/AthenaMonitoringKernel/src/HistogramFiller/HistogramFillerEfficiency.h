/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/  
  
#ifndef AthenaMonitoringKernel_HistogramFiller_HistogramFillerEfficiency_h
#define AthenaMonitoringKernel_HistogramFiller_HistogramFillerEfficiency_h

#include "CxxUtils/checker_macros.h"
#include "TEfficiency.h"

#include "AthenaMonitoringKernel/HistogramFiller.h"

namespace Monitored {
  /**
   * @brief Filler for TEfficiency graphs
   */
  class HistogramFillerEfficiency : public HistogramFiller {
  public:
    HistogramFillerEfficiency(const HistogramDef& definition, std::shared_ptr<IHistogramProvider> provider)
      : HistogramFiller(definition, provider) {}

    virtual HistogramFillerEfficiency* clone() override {
      return new HistogramFillerEfficiency( *this );
    }

    virtual unsigned fill() override {
      if (m_monVariables.size() != 2) {
        return 0;
      }

      auto histogram = this->histogram<TEfficiency>();
      auto valuesVector1 = m_monVariables[0].get().getVectorRepresentation();
      auto valuesVector2 = m_monVariables[1].get().getVectorRepresentation();

      std::lock_guard<std::mutex> lock(*(this->m_mutex));

      if ( m_monVariables.at(1).get().hasStringRepresentation() ) {
	valuesVector2.clear();
	TH1* tot ATLAS_THREAD_SAFE  = const_cast<TH1*>(histogram->GetTotalHistogram());
	const TAxis* xaxis = tot->GetXaxis();
	for ( const std::string& value: m_monVariables[1].get().getStringVectorRepresentation() ) {
	  const int binNumber = xaxis->FindFixBin( value.c_str() );
	  const double binCenter ATLAS_THREAD_SAFE  = xaxis->GetBinCenter( binNumber );
	  valuesVector2.push_back(binCenter);
	}
      }

      for (unsigned i = 0; i < std::size(valuesVector1); ++i) {
	histogram->Fill(valuesVector1[i],valuesVector2[i]);
      }
      return std::size(valuesVector1);
    }
  };
}

#endif /* AthenaMonitoringKernel_HistogramFiller_HistogramFillerEfficiency_h */
