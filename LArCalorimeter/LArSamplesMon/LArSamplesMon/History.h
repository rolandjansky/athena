/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

/**
   @class LArSamplesHistory
   @brief storage of a collection of digits for one cell
*/

#ifndef LArSamples_History_H
#define LArSamples_History_H

#include "LArCafJobs/Definitions.h"
#include "LArCafJobs/HistoryContainer.h"
#include "LArCafJobs/CellInfo.h"
#include "LArSamplesMon/Chi2Calc.h"
#include "LArSamplesMon/FilterParams.h"
#include "LArCafJobs/EventData.h"
#include "TH1D.h"
#include "TString.h"
#include "TArrayI.h"
#include "TVectorD.h"
#include <vector>
//#include <ext/hash_map>

//namespace std { using namespace __gnu_cxx; }

namespace LArSamples {
  
  class Interface;
  class Data;
  class Averager;
  class GraphShape;
  class AbsShapeErrorGetter;
  class ShapeErrorData;
  class ScaledErrorData;
  class Residual;
  class Residuals;
  
  class History {
  
    public:
   
      /** @brief Constructor  */
      History(const HistoryContainer& container, 
              const std::vector<const EventData*>& eventData, unsigned int hash, 
              const AbsShapeErrorGetter* shapeErrorGetter = 0);

      History(const std::vector<const Data*>& data, const CellInfo& info, const std::vector<const EventData*>& eventData, 
              unsigned int hash, const AbsShapeErrorGetter* shapeErrorGetter = 0);
      
      virtual ~History();

      HistoryContainer* dissolve();

      unsigned int nData() const { return m_data.size(); }
      const Data* data(unsigned int i) const;
      const Data* data_for_event(int event, int run = -1) const;
      const Data* data_for_event(const EventData& eventData) const;

      const CellInfo* cellInfo() const { return &m_cellInfo; }
    
      const std::vector<const EventData*>& eventData() const { return m_eventData; }
      
      bool sum(SimpleShape*& sum, SimpleShape*& reference) const;              
      bool allShape(GraphShape*& allData, SimpleShape*& allRef) const;
      double allChi2(Chi2Params pars) const;
    
      // helpers
      double chi2(int i, int lwb = -1, int upb = -1, int chi2Params = DefaultChi2, ShapeErrorType shapeErrorType = BestShapeError, unsigned int* nDof = 0) const;
      double chi2_k(int i, double k, int lwb = -1, int upb = -1, int chi2Params = DefaultChi2) const;
      double maxChi2(int lwb = -1, int upb = -1, int chi2Params = DefaultChi2) const;
      
      OFC* ofc(unsigned int i, int lwb = -1, int upb = -1, double time = Definitions::none, bool useCorrs = true) const;
      History* refit(Chi2Params pars = DefaultChi2) const;
      History* adjust() const;
      History* filter(const TString& cuts) const;
      
      bool refVal(unsigned int i, unsigned int sample, double& val, double& err) const;
      bool delta(unsigned int i, unsigned int sample, double& del) const;
      TVectorD deltas(unsigned int i, int lwb = -1, int upb = -1, bool correct = true) const;
      bool isValid() const;
      
      TString description(unsigned int verbosity = 1) const;
      
      SimpleShape* referenceShape(unsigned int k, double adcMax = -1, double time = Definitions::none, 
                                  bool samplesOnly = false) const;
                                  
      SimpleShape* deltaShape(unsigned int k, int lwb = -1, int upb = -1) const;
      
      unsigned int hash() const { return m_hash; }
      void setShapeErrorGetter(const AbsShapeErrorGetter* err) const { m_shapeErrorGetter = err; }
      const AbsShapeErrorGetter* shapeErrorGetter() const { return m_shapeErrorGetter; }
      const ShapeErrorData* shapeErrorData(CaloGain::CaloGain gain, ShapeErrorType shapeErrorType = BestShapeError, const Residual* res = 0) const;
      const ScaledErrorData* scaledErrorData(unsigned int i, double adcMax = -1, double time = Definitions::none, ShapeErrorType shapeErrorType = BestShapeError) const;

      bool residualOffset(unsigned int i, short sample, double& offset, double adcMax = -1, double time = Definitions::none) const;
      bool residualError(unsigned int i, short sample1, short sample2, double& offset, double adcMax = -1, double time = Definitions::none) const;
      
      bool drawWithReference(int i, const TString& atlasTitle = "") const;
      bool drawSumWithReference() const;
      bool drawAllWithReference(bool refit = false) const;
      bool drawResiduals(int k = -1, bool errors = true, bool rescale = true) const;
      
      Averager* calculatePedestal(int i) const;
      Residual* residual(unsigned int k, bool correct = true, bool zeroTime = false) const;
      Residuals* residuals(CaloGain::CaloGain gain = CaloGain::LARNGAIN, double absResTrunc = -1, bool correct = true, bool zeroTime = false) const;

      double upstreamEnergy(unsigned int k) const;
      double chi2Anomaly(double chi2Cut, unsigned int nDof = 3) const;
      
      void setInterface(const Interface* interface) const { m_interface = interface; }
      
    private:
      
     std::vector<const Data*> m_data;
     CellInfo m_cellInfo;
     std::vector<const EventData*> m_eventData;
     unsigned int m_hash;
     mutable const AbsShapeErrorGetter* m_shapeErrorGetter;
     mutable const Interface* m_interface = nullptr;
     //mutable std::hash_map<int, std::pair<int, const Data*> > m_dataForEvent;
  };
}
#endif
