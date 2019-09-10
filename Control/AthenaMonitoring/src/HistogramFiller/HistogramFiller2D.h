/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/  
  
#ifndef AthenaMonitoring_HistogramFiller_HistogramFiller2D_h
#define AthenaMonitoring_HistogramFiller_HistogramFiller2D_h

#include "TH2.h"

#include "AthenaMonitoring/HistogramFiller.h"
#include <boost/range/combine.hpp>

namespace Monitored {
  /**
   * @brief Filler for plain 2D histogram
   */
  class HistogramFiller2D : public HistogramFiller {
  public:
    HistogramFiller2D(const HistogramDef& definition, std::shared_ptr<IHistogramProvider> provider)
      : HistogramFiller(definition, provider) {}
    
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

      auto histogram = this->histogram<TH2>();
      std::lock_guard<std::mutex> lock(*(this->m_mutex));

      if (areVectorsSameSize) { // Two equal-size vectors
        if ( m_monWeight && m_monWeight->getVectorRepresentation().size()==size1 ) {
          // Weighted fill
          auto weightVector = m_monWeight->getVectorRepresentation();
          double value1,value2,weight;
          for (const auto& zipped : boost::combine(vector1,vector2,weightVector)) {
            boost::tie(value1,value2,weight) = zipped;
            histogram->Fill(value1,value2,weight);
          }
        } else {
          // Unweighted fill
          for (unsigned i = 0; i < size1; ++i) {
            histogram->Fill(vector1[i], vector2[i]);
          }
        }
        result = size1;

      } else if (size1 == 1) { // Scalar vector1 and vector vector2
        if ( m_monWeight && m_monWeight->getVectorRepresentation().size()==size2 ) {
          // Weighted fill
          auto weightVector = m_monWeight->getVectorRepresentation();
          double value,weight;
          for (const auto& zipped : boost::combine(vector2,weightVector)) {
            boost::tie(value,weight) = zipped;
            histogram->Fill(vector1[0],value,weight);
          }
        } else {
          // Unweighted fill
          for (auto value : vector2) {
            histogram->Fill(vector1[0], value);
          }
        }
        result = size2;

      } else { // Vector vector1 and scalar vector2
        if ( m_monWeight && m_monWeight->getVectorRepresentation().size()==size1 ) {
          // Weighted fill
          auto weightVector = m_monWeight->getVectorRepresentation();
          double value,weight;
          for (const auto& zipped : boost::combine(vector1,weightVector)) {
            boost::tie(value,weight) = zipped;
            histogram->Fill(value,vector2[0],weight);
          }
        } else {
          // Unweighted fill
          for (auto value : vector1) {
            histogram->Fill(value, vector2[0]);
          }
        }
        result = size1;
      }

      return result;
    }
  };
}

#endif /* AthenaMonitoring_HistogramFiller_HistogramFiller2D_h */