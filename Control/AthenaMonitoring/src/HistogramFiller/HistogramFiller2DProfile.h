/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/  
  
#ifndef AthenaMonitoring_HistogramFiller_HistogramFiller2DProfile_h
#define AthenaMonitoring_HistogramFiller_HistogramFiller2DProfile_h

#include "TProfile2D.h"

#include "AthenaMonitoring/HistogramFiller/HistogramFiller.h"

namespace Monitored {
  /**
   * @brief filler for profile 2D histogram
   */
  class HistogramFiller2DProfile : public HistogramFiller {
  public:
    HistogramFiller2DProfile(TProfile2D* hist, const HistogramDef& histDef)
      : HistogramFiller(hist, histDef) {};

    virtual HistogramFiller2DProfile* clone() override { return new HistogramFiller2DProfile(*this); };
    
    virtual unsigned fill() override {
      using namespace std; 

      if (m_monVariables.size() != 3) {
        return 0;
      }

      unsigned i(0);
      auto hist = histogram();
      auto valuesVector1 = m_monVariables[0].get().getVectorRepresentation();
      auto valuesVector2 = m_monVariables[1].get().getVectorRepresentation();
      auto valuesVector3 = m_monVariables[2].get().getVectorRepresentation();
      lock_guard<mutex> lock(*(this->m_mutex));
      /*HERE NEED TO INCLUDE CASE IN WHICH SOME VARIABLES ARE SCALAR AND SOME VARIABLES ARE VECTORS
      unsigned i(0);
      if (m_variable1->size() != m_variable2->size() || m_variable1->size() != m_variable3->size() || m_variable2->size() != m_variable3->size() ) {

      }*/

      //For now lets just consider the case in which all variables are of the same length
      for (i = 0; i < valuesVector1.size(); ++i) {
        hist->Fill(valuesVector1[i], valuesVector2[i], valuesVector3[i]);
      }
      
      return i;
    }
  protected:
    virtual TProfile2D* histogram() override { return static_cast<TProfile2D*>(m_hist); }
  };
}

#endif /* AthenaMonitoring_HistogramFiller_HistogramFiller2DProfile_h */