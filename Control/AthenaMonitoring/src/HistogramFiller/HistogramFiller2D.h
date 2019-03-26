/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/  
  
#ifndef AthenaMonitoring_HistogramFiller_HistogramFiller2D_h
#define AthenaMonitoring_HistogramFiller_HistogramFiller2D_h

#include "TH2.h"

#include "AthenaMonitoring/HistogramFiller/HistogramFiller.h"

namespace Monitored {
  /**
   * @brief Filler for plain 2D histogram
   */
  class HistogramFiller2D : public HistogramFiller {
  public:
    HistogramFiller2D(TH2* const hist, const HistogramDef& histDef)
      : HistogramFiller(histDef), m_histogram(hist) {};

    HistogramFiller2D(const HistogramFiller2D& hf) 
      : HistogramFiller(hf), m_histogram(hf.m_histogram) {}
    
    virtual HistogramFiller2D* clone() override { return new HistogramFiller2D(*this); };

    virtual unsigned fill() override {
      if (m_monVariables.size() != 2) {
        return 0;
      }

      unsigned result = 0;
      const auto vector1 = m_monVariables[0].get().getVectorRepresentation();
      const auto vector2 = m_monVariables[1].get().getVectorRepresentation();
      const unsigned size1 = std::size(vector1);
      const unsigned size2 = std::size(vector2);
      const bool isAnyVectorEmpty = size1 == 0 || size2 == 0;
      const bool isAnyVectorScalar = size1 == 1 || size2 == 1;
      const bool areVectorsSameSize = size1 == size2;
      const bool areVectorsValid = !isAnyVectorEmpty && (areVectorsSameSize || isAnyVectorScalar);

      if (!areVectorsValid) {
        return 0;
      }

      std::lock_guard<std::mutex> lock(*(this->m_mutex));

      if (areVectorsSameSize) {
        for (unsigned i = 0; i < size1; ++i) {
          m_histogram->Fill(vector1[i], vector2[i]);
        }

        result = size1;
      } else if (size1 == 1) {
        for (auto value : vector2) {
          m_histogram->Fill(vector1[0], value);
        }

        result = size2;
      } else {
        for (auto value : vector1) {
          m_histogram->Fill(value, vector2[0]);
        }

        result = size1;
      }

      return result;
    }
  protected:
    TH2 * const m_histogram;
  };
}

#endif /* AthenaMonitoring_HistogramFiller_HistogramFiller2D_h */