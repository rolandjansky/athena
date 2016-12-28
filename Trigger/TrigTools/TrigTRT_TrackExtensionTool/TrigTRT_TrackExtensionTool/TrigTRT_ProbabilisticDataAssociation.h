/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGTRTTRACKEXTENSIONTOOL_TRIGTRT_PROBABILISTICDATAASSOCIATION_H
#define TRIGTRTTRACKEXTENSIONTOOL_TRIGTRT_PROBABILISTICDATAASSOCIATION_H

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
  virtual void update(Trk::TrkTrackState*, TrigTRT_Info*, Trk::TrkTrackState*)=0;
  virtual bool validateTRT_Hit(Trk::TrkTrackState*,TrigTRT_Straw*)=0;
  void clear();
 protected:
  void runKalmanFilter(Trk::TrkTrackState*);
  double errorFunction(double);
  double calculatePDetect(double,double);

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
  virtual void update(Trk::TrkTrackState*,TrigTRT_Info*,Trk::TrkTrackState* pTS=NULL);
  virtual bool validateTRT_Hit(Trk::TrkTrackState*,TrigTRT_Straw*);
};

class TrigTRT_EndcapPDAF : public TrigTRT_BasePDAF
{
 public:
  TrigTRT_EndcapPDAF(double,double,double,double,double,bool);
  ~TrigTRT_EndcapPDAF();
  virtual void update(Trk::TrkTrackState*,TrigTRT_Info*,Trk::TrkTrackState* pTS=NULL);
  virtual bool validateTRT_Hit(Trk::TrkTrackState*,TrigTRT_Straw*);
};

#endif
