/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/  
  
#ifndef AthenaMonitoringKernel_HistogramFiller_HistogramFillerProfile_h
#define AthenaMonitoringKernel_HistogramFiller_HistogramFillerProfile_h

#include "TProfile.h"

#include "AthenaMonitoringKernel/HistogramFiller.h"
#include "HistogramFillerUtils.h"
#include "boost/range/combine.hpp"

namespace Monitored {
  /**
   * @brief Filler for profile 1D histogram 
   */
  class HistogramFillerProfile : public HistogramFiller {
  public:
    HistogramFillerProfile(const HistogramDef& definition, std::shared_ptr<IHistogramProvider> provider)
      : HistogramFiller(definition, provider) {}

    virtual HistogramFillerProfile* clone() const override {
      return new HistogramFillerProfile( *this );
    }

    virtual unsigned fill() override {
      if (m_monVariables.size() != 2) {
        return 0;
      }

      // handling of the cutmask
      auto cutMaskValuePair = getCutMaskFunc();
      if (cutMaskValuePair.first == 0) { return 0; }
      auto cutMaskAccessor = cutMaskValuePair.second;

      // handling of the weight
      std::function<double(size_t)> weightAccessor = [] (size_t ){ return 1.0; };  // default is always 1.0
      const std::vector<double> weightVector{m_monWeight ? m_monWeight->getVectorRepresentation() : std::vector<double>{}};
      if ( m_monWeight != nullptr ) {
        if (weightVector.size() == 1) {
          weightAccessor = [=](size_t){ return weightVector[0]; };
        } else {
        	weightAccessor = [&](size_t i){ return weightVector[i]; }; 
        }
      }

      auto histogram = this->histogram<TProfile>();
      const auto valuesVector1{m_monVariables[0].get().getVectorRepresentation()};
      const auto valuesVector2{m_monVariables[1].get().getVectorRepresentation()};
      const size_t size1 = std::size(valuesVector1);
      const size_t size2 = std::size(valuesVector2);
      const bool isAnyVectorEmpty = size1 == 0 || size2 == 0;
      const bool isAnyVectorScalar = size1 == 1 || size2 == 1;
      const bool areVectorsSameSize = size1 == size2;
      const bool areVectorsValid = !isAnyVectorEmpty && (areVectorsSameSize || isAnyVectorScalar);
      if (!areVectorsValid) return 0;
      if (ATH_UNLIKELY(!isAnyVectorScalar 
                        && ((weightVector.size() > 1 && size1 != weightVector.size())
                            || (cutMaskValuePair.first > 1 && size1 != cutMaskValuePair.first)
                          )
                      )
        ) {
        MsgStream log(Athena::getMessageSvc(), "HistogramFillerProfile");
        log << MSG::ERROR << "Mismatch of provided vector sizes for " << m_histDef->alias << endmsg;
        return 0;
      }

      std::scoped_lock<std::mutex> lock(*m_mutex);

      if (m_histDef->kAddBinsDynamically) {
        const auto xmax = std::max_element(begin(valuesVector1), end(valuesVector1));
        if (Monitored::detail::shouldRebinHistogram(histogram->GetXaxis(), *xmax)) {
          Monitored::detail::rebinHistogram<Axis::X>(histogram, *xmax);
        }
      }

      std::function<double(size_t)> fillFunc1, fillFunc2;
      if (size1 == 1) {
        fillFunc1 = [=](size_t){ return valuesVector1[0]; };
      } else {
        fillFunc1 = [&](size_t i){ return valuesVector1[i]; };
      }

      if (size2 == 1) {
        fillFunc2 = [=](size_t){ return valuesVector2[0]; };
      } else {
        fillFunc2 = [&](size_t i){ return valuesVector2[i]; };
      }
                  
      size_t itrSize = std::max(size1, size2);
      
      for (size_t idx = 0; idx < itrSize; ++idx) {
        if (cutMaskAccessor(idx)) {
          histogram->Fill(fillFunc1(idx), fillFunc2(idx), weightAccessor(idx));
        }
      }
      return itrSize;
    }
  };
}

#endif /* AthenaMonitoringKernel_HistogramFiller_HistogramFillerProfile_h */
