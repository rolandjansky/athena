/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef __TRIG_TRT_PDAF_H__
#define __TRIG_TRT_PDAF_H__

class TrigTRT_Straw;
class TrigTRT_Info;

namespace Trk {
  class TrkTrackState;
}

struct WeightedStraw
{
  TrigTRT_Straw* m_pS;
  double m_PD,m_P;
  int m_nComponents;
  double m_Resid[2];
  double m_Weight[2];
};

class TrigTRT_BasePDAF
{
 public:
  TrigTRT_BasePDAF(double,double,double,double,double,bool);
  virtual ~TrigTRT_BasePDAF();
  virtual void m_update(Trk::TrkTrackState*, TrigTRT_Info*, Trk::TrkTrackState*)=0;
  virtual bool m_validateTRT_Hit(Trk::TrkTrackState*,TrigTRT_Straw*)=0;
  void m_clear();
 protected:
  void m_runKalmanFilter(Trk::TrkTrackState*);
  double m_errorFunction(double);
  double m_calculatePDetect(double,double);

  //  double m_combinedResidual;
  // double m_combinedVariance;
  double m_H[5];
  double m_hitEfficiency;
  double m_noiseProbability;
  double m_P0;
  double m_strawRadius;
  double m_hitResolution;
  double m_validationCut;
  double m_associationCut;
  bool m_makeWireHits;
  std::vector<WeightedStraw*> m_vpSW;
  std::vector<TrigTRT_Straw*> m_VS;
};

class TrigTRT_BarrelPDAF : public TrigTRT_BasePDAF
{
 public:
  TrigTRT_BarrelPDAF(double,double,double,double,double,bool);
  ~TrigTRT_BarrelPDAF();
  virtual void m_update(Trk::TrkTrackState*,TrigTRT_Info*,Trk::TrkTrackState* pTS=NULL);
  virtual bool m_validateTRT_Hit(Trk::TrkTrackState*,TrigTRT_Straw*);
};

class TrigTRT_EndcapPDAF : public TrigTRT_BasePDAF
{
 public:
  TrigTRT_EndcapPDAF(double,double,double,double,double,bool);
  ~TrigTRT_EndcapPDAF();
  virtual void m_update(Trk::TrkTrackState*,TrigTRT_Info*,Trk::TrkTrackState* pTS=NULL);
  virtual bool m_validateTRT_Hit(Trk::TrkTrackState*,TrigTRT_Straw*);
};

#endif
