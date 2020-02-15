/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef AthenaMonitoringKernel_HistogramFiller_HistogramFiller1D_h
#define AthenaMonitoringKernel_HistogramFiller_HistogramFiller1D_h

#include "TH1.h"

#include "AthenaMonitoringKernel/HistogramFiller.h"
#include "CxxUtils/AthUnlikelyMacros.h"
#include "GaudiKernel/MsgStream.h"

namespace Monitored {
  /**
   * @brief Filler for plain 1D histograms
   */
  class HistogramFiller1D : public HistogramFiller {
  public:
    HistogramFiller1D(const HistogramDef& definition, std::shared_ptr<IHistogramProvider> provider)
      : HistogramFiller(definition, provider) {
    }

    virtual HistogramFiller1D* clone() const override {
      return new HistogramFiller1D( *this );
    }

    virtual unsigned fill() override {
      if (m_monVariables.size() != 1) { return 0; }
      size_t varVecSize = m_monVariables.at(0).get().size();

      // handling of the cutmask
      auto cutMaskValuePair = getCutMaskFunc();
      if (cutMaskValuePair.first == 0) { return 0; }
      if (ATH_UNLIKELY(cutMaskValuePair.first > 1 && cutMaskValuePair.first != varVecSize)) {
        MsgStream log(Athena::getMessageSvc(), "HistogramFiller1D");
        log << MSG::ERROR << "CutMask does not match the size of plotted variable: " 
            << cutMaskValuePair.first << " " << varVecSize << endmsg;
      }
      auto cutMaskAccessor = cutMaskValuePair.second;

      std::function<double(size_t)> weightValue = [] (size_t ){ return 1.0; };  // default is always 1.0
      const std::vector<double> weightVector{m_monWeight ? m_monWeight->getVectorRepresentation() : std::vector<double>{}};
      if ( m_monWeight != nullptr ) {
        if (weightVector.size() == 1) {
          weightValue = [=](size_t){ return weightVector[0]; };
        } else {
          weightValue = [&](size_t i){ return weightVector[i]; };
          if (ATH_UNLIKELY(weightVector.size() != varVecSize)) {
            MsgStream log(Athena::getMessageSvc(), "HistogramFiller1D");
            log << MSG::ERROR << "Weight does not match the size of plotted variable: " 
                << weightVector.size() << " " << varVecSize << endmsg;
          }
        }
      }

      if ( not m_monVariables.at(0).get().hasStringRepresentation() ) {
        const auto valuesVector{m_monVariables.at(0).get().getVectorRepresentation()};
        fill( std::size( valuesVector), [&](size_t i){ return valuesVector[i]; }, weightValue, cutMaskAccessor );
        return std::size( valuesVector );
      } else {
        const auto valuesVector{m_monVariables.at(0).get().getStringVectorRepresentation()};
        fill( std::size( valuesVector ), [&](size_t i){ return valuesVector[i].c_str(); }, weightValue, cutMaskAccessor );
        return std::size( valuesVector );
      }
    }

  protected:

    // The following method takes the length of the vector of values and three functions as arguments:
    // a function that returns a value, one that returns a weight, and one that functions as a cut mask
    // template allows us to support both floating point and string variable fill calls
    template<typename F1, typename F2, typename F3>
    void fill( size_t n, F1 f1, F2 f2, F3 f3 ) {
      std::lock_guard<std::mutex> lock(*(this->m_mutex));

      auto histogram = this->histogram<TH1>();
      for ( size_t i = 0; i < n; ++i ) {
        if (f3(i)) {
          histogram->Fill( f1(i), f2(i) );
        }
      }
    }
  };
}

#endif /* AthenaMonitoringKernel_HistogramFiller_HistogramFiller1D_h */
