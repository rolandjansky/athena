/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
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

    virtual HistogramFillerEfficiency* clone() const override {
      return new HistogramFillerEfficiency( *this );
    }

    virtual unsigned fill() const override {
      if ( m_monVariables.size()<2 ) return 0;

      const IMonitoredVariable& var0 = m_monVariables[0].get();

      auto cutMaskValuePair = getCutMaskFunc();
      if (cutMaskValuePair.first == 0) { return 0; }
      if (ATH_UNLIKELY(cutMaskValuePair.first > 1 && cutMaskValuePair.first != var0.size())) {
        MsgStream log(Athena::getMessageSvc(), "HistogramFillerEfficiency");
        log << MSG::ERROR << "CutMask does not match the size of plotted variable: " 
            << cutMaskValuePair.first << " " << var0.size() << endmsg;
      }
      auto cutMaskAccessor = cutMaskValuePair.second;

      auto efficiency = this->histogram<TEfficiency>();

      int nMonVar = m_monVariables.size();
      if ( nMonVar==2 ) { // Single observable (1D TEfficiency)
        for (unsigned i = 0; i < var0.size(); ++i) {
          if (cutMaskAccessor(i)) {
            efficiency->Fill(var0.get(i), retrieveVariable(efficiency,1,i));
          }
        }
        return var0.size();
      } else if ( nMonVar==3 ) { // Two observables (2D TEfficiency)
        for (unsigned i = 0; i < var0.size(); ++i) {
          if (cutMaskAccessor(i)) {
            efficiency->Fill(var0.get(i),
                             retrieveVariable(efficiency,1,i),
                             retrieveVariable(efficiency,2,i));
          }
        }
        return var0.size();
      } else if ( nMonVar==4 ) { // Three observables (3D Efficiency)
        for (unsigned i = 0; i < var0.size(); ++i) {
          if (cutMaskAccessor(i)) {
            efficiency->Fill(var0.get(i),
                             retrieveVariable(efficiency,1,i),
                             retrieveVariable(efficiency,2,i),
                             retrieveVariable(efficiency,3,i));
          }
        }
        return var0.size();
      } else {
        return 0;
      }
    }

    double retrieveVariable(TEfficiency* efficiency, int iVariable, int i) const {
      const IMonitoredVariable& valueVariable = m_monVariables[iVariable].get();
      if ( valueVariable.hasStringRepresentation() ) {
        TH1* tot ATLAS_THREAD_SAFE  = const_cast<TH1*>(efficiency->GetTotalHistogram());
        const TAxis* axis = getAxis(tot, iVariable);
        const int binNumber = axis->FindFixBin( valueVariable.getString(i).c_str() );
        return axis->GetBinCenter(binNumber);
      } else {
        return valueVariable.get(i);
      }
    }

    const TAxis* getAxis(TH1* hist, int iAxis) const {
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
