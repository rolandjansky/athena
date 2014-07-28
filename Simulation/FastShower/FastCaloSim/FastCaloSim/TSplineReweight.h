/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TSplineReweight_h
#define TSplineReweight_h

#include "FastCaloSim/TLateralShapeCorrection.h"

class TSpline;
class TSpline3;


//////////////////////////////////////////
// Spline reweight base class
//////////////////////////////////////////

class TSplineReweight:public TLateralShapeCorrection {
protected:
  TSpline3* m_spline;
  
  virtual std::string prefix() const {return "SplineReweight";};
public:
  TSplineReweight();
  virtual ~TSplineReweight();
  
  void SetSpline(TSpline3* spline);
  inline const TSpline3* GetSpline() const {return m_spline;};
  float Eval(float x) const;
  
  virtual std::string str() const;

  virtual TCanvas* DrawCorr() const;

  ClassDef(TSplineReweight,1)  //TSplineReweight
};


//////////////////////////////////////////
// Cell unit based reweighting function
//////////////////////////////////////////

class TEtaSplineReweight:public TSplineReweight {
protected:
  virtual std::string prefix() const {return "EtaSplineReweight";};
public:
  TEtaSplineReweight():TSplineReweight() {};
  
  virtual float shapefactor(float deta_mm,float dphi_mm,float deta_cellunits,float dphi_cellunits) const;

  ClassDef(TEtaSplineReweight,1)  //TEtaSplineReweight
};

class TPhiSplineReweight:public TSplineReweight {
protected:
  virtual std::string prefix() const {return "PhiSplineReweight";};
public:
  TPhiSplineReweight():TSplineReweight() {};
  
  virtual float shapefactor(float deta_mm,float dphi_mm,float deta_cellunits,float dphi_cellunits) const;

  ClassDef(TPhiSplineReweight,1)  //TPhiSplineReweight
};

class TRSplineReweight:public TSplineReweight {
protected:
  virtual std::string prefix() const {return "RSplineReweight";};
public:
  TRSplineReweight():TSplineReweight() {};
  
  virtual float shapefactor(float deta_mm,float dphi_mm,float deta_cellunits,float dphi_cellunits) const;

  ClassDef(TRSplineReweight,1)  //TRSplineReweight
};


//////////////////////////////////////////
// Metric unit based reweighting function
//////////////////////////////////////////

class TMetricEtaSplineReweight:public TSplineReweight {
protected:
  virtual std::string prefix() const {return "MetricEtaSplineReweight";};
public:
  TMetricEtaSplineReweight():TSplineReweight() {};
  
  virtual float shapefactor(float deta_mm,float dphi_mm,float deta_cellunits,float dphi_cellunits) const;

  ClassDef(TMetricEtaSplineReweight,1)  //TMetricEtaSplineReweight
};

class TMetricPhiSplineReweight:public TSplineReweight {
protected:
  virtual std::string prefix() const {return "MetricPhiSplineReweight";};
public:
  TMetricPhiSplineReweight():TSplineReweight() {};
  
  virtual float shapefactor(float deta_mm,float dphi_mm,float deta_cellunits,float dphi_cellunits) const;

  ClassDef(TMetricPhiSplineReweight,1)  //TMetricPhiSplineReweight
};

class TMetricRSplineReweight:public TSplineReweight {
protected:
  virtual std::string prefix() const {return "MetricRSplineReweight";};
public:
  TMetricRSplineReweight():TSplineReweight() {};
  
  virtual float shapefactor(float deta_mm,float dphi_mm,float deta_cellunits,float dphi_cellunits) const;

  ClassDef(TMetricRSplineReweight,1)  //TMetricRSplineReweight
};

#endif
