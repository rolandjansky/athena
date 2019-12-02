/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/  
  
#ifndef AthenaMonitoringKernel_HistogramFiller_HistogramFiller2DProfile_h
#define AthenaMonitoringKernel_HistogramFiller_HistogramFiller2DProfile_h

#include "TProfile2D.h"

#include "AthenaMonitoringKernel/HistogramFiller.h"

namespace Monitored {
  /**
   * @brief Filler for profile 2D histogram
   */
  class HistogramFiller2DProfile : public HistogramFiller {
  public:
    HistogramFiller2DProfile(const HistogramDef& definition, std::shared_ptr<IHistogramProvider> provider)
      : HistogramFiller(definition, provider) {}

    virtual unsigned fill() override {
      if (m_monVariables.size() != 3) {
        return 0;
      }

      auto histogram = this->histogram<TProfile2D>();
      auto valuesVector1 = m_monVariables[0].get().getVectorRepresentation();
      auto valuesVector2 = m_monVariables[1].get().getVectorRepresentation();
      auto valuesVector3 = m_monVariables[2].get().getVectorRepresentation();
      std::lock_guard<std::mutex> lock(*(this->m_mutex));
      /*HERE NEED TO INCLUDE CASE IN WHICH SOME VARIABLES ARE SCALAR AND SOME VARIABLES ARE VECTORS
      unsigned i(0);
      if (m_variable1->size() != m_variable2->size() || m_variable1->size() != m_variable3->size() || m_variable2->size() != m_variable3->size() ) {

      }*/

      //For now lets just consider the case in which all variables are of the same length
      if ( m_monWeight && m_monWeight->getVectorRepresentation().size()==valuesVector1.size() ) {
        // Weighted fill
        auto weightVector = m_monWeight->getVectorRepresentation();
        double value1,value2,value3,weight;
        for (const auto& zipped : boost::combine(valuesVector1,valuesVector2,valuesVector3,weightVector)) {
          boost::tie(value1,value2,value3,weight) = zipped;
          histogram->Fill(value1,value2,value3,weight);
        }
      } else {
        // Unweighted fill
        for (unsigned i = 0; i < std::size(valuesVector1); ++i) {
          histogram->Fill(valuesVector1[i], valuesVector2[i], valuesVector3[i]);
        }
      }
      return std::size(valuesVector1);
    }
  };
}

#endif /* AthenaMonitoringKernel_HistogramFiller_HistogramFiller2DProfile_h */
