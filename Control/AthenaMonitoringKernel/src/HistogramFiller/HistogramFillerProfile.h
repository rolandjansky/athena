/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/  
  
#ifndef AthenaMonitoringKernel_HistogramFiller_HistogramFillerProfile_h
#define AthenaMonitoringKernel_HistogramFiller_HistogramFillerProfile_h

#include "TProfile.h"

#include "AthenaMonitoringKernel/HistogramFiller.h"
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

      std::lock_guard<std::mutex> lock(*(this->m_mutex));

      if (m_histDef->opt.find("kAddBinsDynamically") != std::string::npos) {
	const auto xmax = std::max_element(begin(valuesVector1), end(valuesVector1));
	if (shouldRebinHistogram(*xmax)) { rebinHistogram(*xmax); }
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
    
  private:
        /**
     * Method checks if histogram should be rebinned. It should happen when the new value 
     * is greater or equal to the greatest value of the histogram.
     */
    bool shouldRebinHistogram(const double value) {
      const auto histogram = this->histogram<TProfile>();
      const double xMax = histogram->GetXaxis()->GetXmax();

      return xMax <= value;
    }
    
    /**
     * Method that actualy rebin entire histogram.
     * 
     * Algorithm will increase a size of the histogram until `value` can be included in new range of values.
     * Examples for xMin=1.0, xMax=3.0;
     *  value=2.9, xMax will be unchanged
     *  value=3.0, xMax will be doubled
     *  value=4.9, xMax will be doubled
     *  value=5.0, xMax will be quadrupled
     *  value=5.1, xMax will be quadrupled
     *  value=8.9, xMax will be quadrupled
     *  value=9.0, xMax will be octupled
     *  value=9.1, xMax will be octupled
     */
    void rebinHistogram(const double value) {
      auto histogram = this->histogram<TProfile>();
      
      histogram->SetCanExtend(TH1::kAllAxes);

      do {
        histogram->LabelsInflate("X");
      } while (shouldRebinHistogram(value));
    }
  };
}

#endif /* AthenaMonitoringKernel_HistogramFiller_HistogramFillerProfile_h */
