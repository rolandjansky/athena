/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

/**
   @class LArSamplesData
   @brief Liquid Argon class for compact storage of samples data
*/

#ifndef LArSamples_Data_H
#define LArSamples_Data_H

#include "LArCafJobs/Definitions.h"
#include "LArCafJobs/AbsShape.h"
#include "LArCafJobs/DataContainer.h"
#include "LArCafJobs/ShapeInfo.h"

#include "LArRawEvent/LArFebErrorSummary.h"

#include <vector>
#include "TString.h"
#include "TVectorD.h"
#include "TMath.h"

#include <iostream>
#include <algorithm>
#include <fstream>

class TH1D;


namespace LArSamples {
  
  class SimpleShape;
  class History;
  class EventData;
  class ScaledErrorData;

  struct DataFuncArgs {
    DataFuncArgs() { init(); }
    DataFuncArgs(int a_i1, int a_i2 = -1, 
                 double a_x1 = Definitions::none, double a_x2 = Definitions::none,
                 const TString& a_str = "") { init(); i1 = a_i1; i2 = a_i2; x1 = a_x1; x2 = a_x2; str = a_str; }
    DataFuncArgs(double a_x1, double a_x2 = Definitions::none) { init(); x1 = a_x1; x2 = a_x2; }
    DataFuncArgs(const TString& a_str) { init();  str = a_str; }
    
    void init() { i1 = i2 = -1; x1 = x2 = Definitions::none; str = ""; }

    int i1, i2;
    double x1, x2;
    TString str;
  };

#ifndef __GCCXML__
  class Data;
  template<class T> struct DataFunc {
    typedef T (Data::*Type)(const DataFuncArgs&) const;  
  };
  struct DataFuncSet {
    DataFuncSet() : intFunc (0), doubleFunc(0) { }
    DataFuncSet(DataFunc<int>::Type intF) : intFunc(intF), doubleFunc(0) { }
    DataFuncSet(DataFunc<double>::Type doubleF) : intFunc (0), doubleFunc(doubleF) { }
    
    double doubleVal(const Data& data, const DataFuncArgs& args) const { return doubleFunc ? (data.*doubleFunc)(args) : -999; }
    double intVal   (const Data& data, const DataFuncArgs& args) const { return    intFunc ?    (data.*intFunc)(args) : -999; }
    double val      (const Data& data, const DataFuncArgs& args) const { return doubleFunc ? (data.*doubleFunc)(args) : (intFunc ? (data.*intFunc)(args) : -999); }
    
    bool isNull() { return (!intFunc && !doubleFunc); }
    bool isInt()  { return (intFunc && !doubleFunc); }

    DataFunc<int>::Type intFunc;
    DataFunc<double>::Type doubleFunc;
  };
#endif


  class Data : public AbsShape {
    
    public:
      
      /** @brief Constructor  */
      
      Data(const DataContainer& container, const EventData& eventData, const History* history, int index, bool takeOwnership = false);      
      Data(const Data& other, const EventData* eventData = 0, const History* history = 0, int index = -1);
      
      /** @brief Destructor */
      virtual ~Data();
        
      /** @return gain value */
      CaloGain::CaloGain gain() const { return container().gain(); }
  
      /** @return number of samples */
      unsigned int nSamples() const { return container().nSamples(); }
  
      /** @return a sample value */
      const short& sample(unsigned int i) const { return container().sample(i); }
      float pedestalSubtractedSample(unsigned int i) const { return sample(i) - pedestal(); }
      std::vector<float> pedestalSubtractedSamples() const;

      const EventData* eventData() const { return m_eventData; }
      
      /** @return run */
      int run() const;

      /** @return event */
      int event() const;

      /** @return lumi block */
      int lumiBlock() const;
      int bunchId() const;

      /** @return energy */
      double energy() const { return container().energy(); }
      
      /** @return time */
      double ofcTime() const { return container().ofcTime() + m_timeShift; }
  
      /** @return pedestal */
      double quality() const { return container().quality(); }
      
      /** @return pedestal */
      double pedestal() const { return container().pedestal(); }
      double pedestalRMS() const { return container().pedestalRMS(); }
      
      /** @return status */
      int status() const { return m_container->status(); }
      
      /** @return OFC peak parameters */
      double adcMax() const { return container().adcMax(); }
      double meV2ADC() const { return (adcMax() > 0 && energy() > 0 ? adcMax()/energy() : -1); }
      double adc2MeV() const { return (adcMax() > 0 && energy() > 0 ? energy()/adcMax() : -1); }
      
      bool febError(LArFebErrorSummary::LArFebErrorType error) const;
      bool isDisconnected() const;
      TString problems(bool sayNone = false) const;
      
      static TString str(LArFebErrorSummary::LArFebErrorType error);
      static TString gainStr(CaloGain::CaloGain gain);
      TString description(unsigned int verbosity = 1) const;

      bool isValid() const;
  
      double footprint() const { return sizeof(*this) + nSamples()*sizeof(short); }
    
      short maxSample() const;
      short minSample() const;
      
      // Interface to LArAbsShape
      unsigned int nPoints()  const { return nSamples(); }
      double time(unsigned int i) const { return Definitions::samplingTime(i); }
      double value(unsigned int i) const { return pedestalSubtractedSample(i); }
      double covariance(unsigned int i, unsigned int j) const;
      
      unsigned int nCorrs() const { return container().corrs().size(); }
      double noise() const;

      double adcMax_new() const;

      double covarianceMeV(unsigned int i, unsigned int j) const;
      
      const DataContainer& container() const { return *m_container; } 
      
      const DataContainer* dissolve();

      SimpleShape* referenceShape() const; 
      SimpleShape* timeAlignedShape() const;

      const ScaledErrorData* scaledErrorData() const; 
      
      const History* history() const { return m_history; }
      short index() const { return m_index; }
      
      TVectorD deltas() const;
      TVectorD residuals() const;
      
      double delta(short sample) const;
      double residual(short sample) const;
      
      double residualOffset(short sample, bool scale = true) const;
      double resCorrN(ShapeErrorType shapeErrorType = BestShapeError) const;
      double xi(short sample, ShapeErrorType shapeErrorType = BestShapeError, CaloGain::CaloGain g = CaloGain::UNKNOWNGAIN, bool xip = false) const;
      double xiNorm(ShapeErrorType shapeErrorType = BestShapeError) const;
      double xip(short sample, ShapeErrorType shapeErrorType = BestShapeError, CaloGain::CaloGain g = CaloGain::UNKNOWNGAIN) const;
      double residualError(short sample1, short sample2 = -1, bool scale = true) const;
      double chi2() const;

      bool calcRefit(double& chi2, double& k, double& dT) const;
      bool calcAdjust(double& k, double& dT) const;

      bool isPassed(const TString& bitName) const;
      bool goodForShapeCorr() const;
      double upstreamEnergy() const;
      
      double _sample(const DataFuncArgs& args) const { return sample(args.i1); }
      double _pedestalSubstractedSample(const DataFuncArgs& args) const { return pedestalSubtractedSample(args.i1); }
      double _energy(const DataFuncArgs&) const { return energy(); }
      double _ofcTime(const DataFuncArgs&) const { return ofcTime(); }
      double _quality(const DataFuncArgs&) const { return quality(); }
      double _pedestal(const DataFuncArgs&) const { return pedestal(); }
      double _pedestalRMS(const DataFuncArgs&) const { return pedestalRMS(); }
      double _adcMax(const DataFuncArgs&) const { return adcMax(); }
      double _adcMax_new(const DataFuncArgs&) const { return adcMax_new(); }
      double _noise(const DataFuncArgs&) const { return noise(); }
      double _maxValue(const DataFuncArgs&) const { return maxValue(); }
      double _minValue(const DataFuncArgs&) const { return minValue(); }
      
      double _null(const DataFuncArgs&) const { return 0; }
      int    _run(const DataFuncArgs&) const { return run(); }
      int    _event(const DataFuncArgs&) const { return event(); }
      int    _lumiBlock(const DataFuncArgs&) const { return lumiBlock(); }
      int    _bunchId(const DataFuncArgs&) const { return bunchId(); }
      int    _index(const DataFuncArgs&) const { return m_index; }
      int    _gain(const DataFuncArgs&) const { return gain(); }
      double _delta(const DataFuncArgs& args) const { return delta(args.i1); }
      double _residual(const DataFuncArgs& args) const { return residual(args.i1); }
      double _residualOffset(const DataFuncArgs& args) const { return residualOffset(args.i1); }
      double _resCorrN(const DataFuncArgs&) const { return resCorrN(CellShapeError); }
      double _xi(const DataFuncArgs& args) const { return xi(args.i1); }
      double _xiCell(const DataFuncArgs& args) const { return xi(args.i1, CellShapeError, args.i2 >= 0 ? (CaloGain::CaloGain)args.i2 : CaloGain::UNKNOWNGAIN); }
      double _xiRing(const DataFuncArgs& args) const { return xi(args.i1, RingShapeError, args.i2 >= 0 ? (CaloGain::CaloGain)args.i2 : CaloGain::UNKNOWNGAIN); }
      double _xiNormCell(const DataFuncArgs&) const { return xiNorm(CellShapeError); }
      double _xiNormRing(const DataFuncArgs&) const { return xiNorm(RingShapeError); }
      double _xip(const DataFuncArgs& args) const { return xip(args.i1); }
      double _residualError(const DataFuncArgs& args) const { return residualError(args.i1); }
      double _normResidualOffset(const DataFuncArgs& args) const { return residualOffset(args.i1, false); }
      double _normResidualError(const DataFuncArgs& args) const { return residualError(args.i1, -1, false); }
      double _ofcSigma(const DataFuncArgs& args) const;
      double _ofcGamma(const DataFuncArgs& args) const;
      int    _iEta(const DataFuncArgs& args) const;
      int    _iPhi(const DataFuncArgs& args) const;
      int    _region(const DataFuncArgs& args) const;
      int    _layer(const DataFuncArgs& args) const;
      int    _calo(const DataFuncArgs& args) const;
      int    _feedThrough(const DataFuncArgs& args) const;
      int    _slot(const DataFuncArgs& args) const;
      int    _channel(const DataFuncArgs& args) const;
      int    _hash(const DataFuncArgs& args) const;
      int    _ring(const DataFuncArgs& args) const;
      int    _nData(const DataFuncArgs& args) const;
      int    _maxPosition(const DataFuncArgs&) const { return maxPosition(); }

      double _count(const DataFuncArgs&) const { return 1; }
      
      double _chi2(const DataFuncArgs&) const { return chi2(); }
      double _chi2_noCorr(const DataFuncArgs& args) const;
      double _chi2_ringCorr(const DataFuncArgs& args) const;
      double _chi2_cellCorr(const DataFuncArgs& args) const;
      double _chi2_k(const DataFuncArgs& args) const;
      double _refitDeltaT(const DataFuncArgs& args) const;
      double _refitScale(const DataFuncArgs& args) const;
      double _refitChi2(const DataFuncArgs& args) const;
      double _adjDeltaT(const DataFuncArgs& args) const;
      double _adjScale(const DataFuncArgs& args) const;
      double _x(const DataFuncArgs& args) const;
      double _y(const DataFuncArgs& args) const;
      double _z(const DataFuncArgs& args) const;
      double _rt(const DataFuncArgs& args) const;
      double _eta(const DataFuncArgs& args) const;
      double _phi(const DataFuncArgs& args) const;
      double _timeNoTOF(const DataFuncArgs& args) const;
      double _timeForSplash1(const DataFuncArgs& args) const;
      double _timeForSplash2(const DataFuncArgs& args) const;
      double _peakSignif(const DataFuncArgs& args) const;
      double _energySignif(const DataFuncArgs& args) const;
      int    _goodForShapeCorr(const DataFuncArgs& /*args*/) const { return (goodForShapeCorr() ? 1 : 0); }
      double _upstreamE(const DataFuncArgs&) const { return upstreamEnergy(); }
      double _upstreamERatio(const DataFuncArgs& args) const;
      double _chi2Anomaly(const DataFuncArgs& args) const;
      double timeClCorr() const; 
          
      double timeECorr() const;

      double _timeECorr(const DataFuncArgs& args) const;
      
      double _pass(const DataFuncArgs& args) const { return isPassed(args.str); }

      void setCallBacks(const History* history, unsigned int index) const { m_history = history; m_index = index; }
      friend class DataTweaker;

      static double timeShift() { return m_timeShift; }
      static void setTimeShift(double shift = 0) { m_timeShift = shift; }
      
    private:
     
      //bool fit(const AbsShape& reference, double& k, double& deltaT, double& chi2) const;
      const DataContainer* const m_container;
      const EventData* const m_eventData;
      mutable const History* m_history;
      mutable unsigned int m_index;
      static double m_timeShift; // specify a global time shift between first sample time and reported ofc time
 };
}
#endif

