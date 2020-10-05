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


    virtual unsigned fill( const HistogramFiller::VariablesPack& vars ) const override {
	if ( vars.size() != 3) {
        return 0;
      }

      // handling of the cutmask
      auto cutMaskValuePair = getCutMaskFunc(vars.cut);
      if (cutMaskValuePair.first == 0) { return 0; }
      auto cutMaskAccessor = cutMaskValuePair.second;

      auto histogram = this->histogram<TProfile2D>();
      /*HERE NEED TO INCLUDE CASE IN WHICH SOME VARIABLES ARE SCALAR AND SOME VARIABLES ARE VECTORS
      unsigned i(0);
      if (m_variable1->size() != m_variable2->size() || m_variable1->size() != m_variable3->size() || m_variable2->size() != m_variable3->size() ) {

      }*/

      //For now lets just consider the case in which all variables are of the same length
      if ( vars.weight && vars.weight->size() == vars.var[0]->size() ) {
        // Weighted fill
        for (unsigned i = 0; i < vars.var[0]->size(); ++i) {
          if (cutMaskAccessor(i)) {
            histogram->Fill(vars.var[0]->get(i), vars.var[1]->get(i), vars.var[2]->get(i), vars.weight->get(i));
          }
        }
      } else {
        // Unweighted fill
        for (unsigned i = 0; i < vars.var[0]->size(); ++i) {
          if (cutMaskAccessor(i)) {
            histogram->Fill(vars.var[0]->get(i), vars.var[1]->get(i), vars.var[2]->get(i));
          }
        }
      }
      return vars.var[0]->size();
    }
  };
}

#endif /* AthenaMonitoringKernel_HistogramFiller_HistogramFiller2DProfile_h */
