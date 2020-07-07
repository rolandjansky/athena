/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/  
  
#ifndef AthenaMonitoringKernel_HistogramFiller_HistogramFiller2DProfile_h
#define AthenaMonitoringKernel_HistogramFiller_HistogramFiller2DProfile_h

#include "TProfile2D.h"
#include "boost/range/combine.hpp"

#include "AthenaMonitoringKernel/HistogramFiller.h"

namespace Monitored {
  /**
   * @brief Filler for profile 2D histogram
   */
  class HistogramFiller2DProfile : public HistogramFiller {
  public:
    HistogramFiller2DProfile(const HistogramDef& definition, std::shared_ptr<IHistogramProvider> provider)
      : HistogramFiller(definition, provider) {}

    virtual HistogramFiller2DProfile* clone() const override {
      return new HistogramFiller2DProfile( *this );
    }


    virtual unsigned fill() const override {
      if (m_monVariables.size() != 3) {
        return 0;
      }

      // handling of the cutmask
      auto cutMaskValuePair = getCutMaskFunc();
      if (cutMaskValuePair.first == 0) { return 0; }
      auto cutMaskAccessor = cutMaskValuePair.second;

      auto histogram = this->histogram<TProfile2D>();
      const IMonitoredVariable& var1 = m_monVariables[0].get();
      const IMonitoredVariable& var2 = m_monVariables[1].get();
      const IMonitoredVariable& var3 = m_monVariables[2].get();

      /*HERE NEED TO INCLUDE CASE IN WHICH SOME VARIABLES ARE SCALAR AND SOME VARIABLES ARE VECTORS
      unsigned i(0);
      if (m_variable1->size() != m_variable2->size() || m_variable1->size() != m_variable3->size() || m_variable2->size() != m_variable3->size() ) {

      }*/

      //For now lets just consider the case in which all variables are of the same length
      if ( m_monWeight && m_monWeight->size()==var1.size() ) {
        // Weighted fill
        for (unsigned i = 0; i < var1.size(); ++i) {
          if (cutMaskAccessor(i)) {
            histogram->Fill(var1.get(i), var2.get(i), var3.get(i), m_monWeight->get(i));
          }
        }
      } else {
        // Unweighted fill
        for (unsigned i = 0; i < var1.size(); ++i) {
          if (cutMaskAccessor(i)) {
            histogram->Fill(var1.get(i), var2.get(i), var3.get(i));
          }
        }
      }
      return var1.size();
    }
  };
}

#endif /* AthenaMonitoringKernel_HistogramFiller_HistogramFiller2DProfile_h */
