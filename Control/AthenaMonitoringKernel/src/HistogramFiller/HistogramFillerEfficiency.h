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
      auto efficiency = this->histogram<TEfficiency>();
      std::lock_guard<std::mutex> lock(*(this->m_mutex));

      int nMonVar = m_monVariables.size();
      if ( nMonVar==2 ) { // Single observable (1D TEfficiency)
        auto valuesVector0 = m_monVariables[0].get().getVectorRepresentation();
        auto valuesVector1 = retrieveVariable(efficiency, 1);
        for (unsigned i = 0; i < std::size(valuesVector0); ++i) {
          efficiency->Fill(valuesVector0[i], valuesVector1[i]);
        }
        return std::size(valuesVector0);
      } else if ( nMonVar==3 ) { // Two observables (2D TEfficiency)
        auto valuesVector0 = m_monVariables[0].get().getVectorRepresentation();
        auto valuesVector1 = retrieveVariable(efficiency, 1);
        auto valuesVector2 = retrieveVariable(efficiency, 2);
        for (unsigned i = 0; i < std::size(valuesVector0); ++i) {
          efficiency->Fill(valuesVector0[i], valuesVector1[i], valuesVector2[i]);
        }
        return std::size(valuesVector0);
      } else if ( nMonVar==4 ) { // Three observables (3D Efficiency)
        auto valuesVector0 = m_monVariables[0].get().getVectorRepresentation();
        auto valuesVector1 = retrieveVariable(efficiency, 1);
        auto valuesVector2 = retrieveVariable(efficiency, 2);
        auto valuesVector3 = retrieveVariable(efficiency, 3);
        for (unsigned i = 0; i < std::size(valuesVector0); ++i) {
          efficiency->Fill(valuesVector0[i], valuesVector1[i], valuesVector2[i], valuesVector3[i]);
        }
        return std::size(valuesVector0);
      } else {
        return 0;
      }
    }

    const std::vector<double> retrieveVariable(TEfficiency* efficiency, int iVariable) {
      auto valueVariable = m_monVariables[iVariable];
      auto valuesVector = valueVariable.get().getVectorRepresentation();
      if ( valueVariable.get().hasStringRepresentation() ) {
        valuesVector.clear();
        TH1* tot ATLAS_THREAD_SAFE  = const_cast<TH1*>(efficiency->GetTotalHistogram());
        const TAxis* axis = getAxis(tot, iVariable);
        for ( const std::string& value : valueVariable.get().getStringVectorRepresentation() ) {
          const int binNumber = axis->FindFixBin( value.c_str() );
          const double binCenter ATLAS_THREAD_SAFE = axis->GetBinCenter(binNumber);
          valuesVector.push_back(binCenter);
        }
      }
      return valuesVector;
    }

    const TAxis* getAxis(TH1* hist, int iAxis) {
      if ( iAxis==1 ) {
        return hist->GetXaxis();
      } else if ( iAxis==2 ) {
        return hist->GetYaxis();
      } else if ( iAxis==3 ) {
        return hist->GetZaxis();
      } else {
        HistogramException("Invalid request for axis when defining TEfficiency.");
        return nullptr;
      }
    }

  };
}

#endif /* AthenaMonitoringKernel_HistogramFiller_HistogramFillerEfficiency_h */
