/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/  
  
#ifndef AthenaMonitoring_HistogramFiller_HistogramFillerProfile_h
#define AthenaMonitoring_HistogramFiller_HistogramFillerProfile_h

#include "TProfile.h"

#include "AthenaMonitoring/HistogramFiller.h"

namespace Monitored {
  /**
   * @brief Filler for profile 1D histogram 
   */
  class HistogramFillerProfile : public HistogramFiller {
  public:
    HistogramFillerProfile(const HistogramDef& definition, std::shared_ptr<IHistogramProvider> provider)
      : HistogramFiller(definition, provider) {}

    virtual HistogramFillerProfile* clone() override { return new HistogramFillerProfile(*this); };

    virtual unsigned fill() override {
      if (m_monVariables.size() != 2) {
        return 0;
      }

      unsigned i(0);
      auto histogram = this->histogram<TProfile>();
      auto valuesVector1 = m_monVariables[0].get().getVectorRepresentation();
      auto valuesVector2 = m_monVariables[1].get().getVectorRepresentation();
      const unsigned size1 = std::size(valuesVector1);
      const unsigned size2 = std::size(valuesVector2);
      const bool isAnyVectorEmpty = size1 == 0 || size2 == 0;
      const bool isAnyVectorScalar = size1 == 1 || size2 == 1;
      const bool areVectorsSameSize = size1 == size2;
      const bool areVectorsValid = !isAnyVectorEmpty && (areVectorsSameSize || isAnyVectorScalar);
      if (!areVectorsValid) return 0;

      std::lock_guard<std::mutex> lock(*(this->m_mutex));

      if (areVectorsSameSize) { // Two equal-size vectors
        if ( m_monWeight && m_monWeight->getVectorRepresentation().size()==valuesVector1.size() ) {
          // Weighted fill
          auto weightVector = m_monWeight->getVectorRepresentation();
          double value1,value2,weight;
          for (const auto& zipped : boost::combine(valuesVector1,valuesVector2,weightVector)) {
            boost::tie(value1,value2,weight) = zipped;
            histogram->Fill(value1,value2,weight);
          }
        } else {
          // Unweighted fill
          for (i = 0; i < std::size(valuesVector1); ++i) {
            histogram->Fill(valuesVector1[i], valuesVector2[i]);
          }
        }

      } else if (size1==1) { // first variable is scalar -- loop over second
        if ( m_monWeight && m_monWeight->getVectorRepresentation().size()==valuesVector2.size() ) {
          // Weighted fill
          auto weightVector = m_monWeight->getVectorRepresentation();
          double value2,weight;
          for (const auto& zipped : boost::combine(valuesVector2,weightVector)) {
            boost::tie(value2,weight) = zipped;
            histogram->Fill(valuesVector1[0],value2,weight);
          }
        } else {
          // Unweighted fill
          for (auto value2 : valuesVector2) {
            histogram->Fill(valuesVector1[0], value2);
            ++i;
          }
        }

      } else if (size2==1) { // second variable is scalar -- loop over first
        if ( m_monWeight && m_monWeight->getVectorRepresentation().size()==valuesVector1.size() ) {
          // Weighted fill
          auto weightVector = m_monWeight->getVectorRepresentation();
          double value1,weight;
          for (const auto& zipped : boost::combine(valuesVector1,weightVector)) {
            boost::tie(value1,weight) = zipped;
            histogram->Fill(value1,valuesVector2[0],weight);
          }
        } else {
          // Unweighted fill
          for (auto value1 : valuesVector1) {
            histogram->Fill(value1, valuesVector2[0]); 
            ++i;
          }
        }
      }
      return i;
    }
  };
}

#endif /* AthenaMonitoring_HistogramFiller_HistogramFillerProfile_h */