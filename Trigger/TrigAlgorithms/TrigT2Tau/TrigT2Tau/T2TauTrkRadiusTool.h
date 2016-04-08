/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// ********************************************************************
// 
// NAME:     T2TauTrkRadiusTool.h
// PACKAGE:  Trigger/TrigAlgorithms/TrigT2Tau
// 
// AUTHOR:   Olya Igonkina
// 20120215, ccuenca, |dZ0| selection implemented and dPhi bug fixed
// 
// ********************************************************************
 
#ifndef TRIGT2TAU_T2TAUTRKRADIUSTOOL_H 
#define TRIGT2TAU_T2TAUTRKRADIUSTOOL_H
 
#include "TrigT2Tau/IAlgToolTau.h"
#include "AthenaBaseComps/AthAlgTool.h"

class TrigTauCluster;
class TrigTauTracksInfo;
class TrigTau;

/** Tool which propogates tracks to the calorimeneter surface and calculates 
number of tracks matched to the cluster*/
class T2TauTrkRadiusTool: public IAlgToolTau, public AthAlgTool{
public:
  
  /** constructor */
  T2TauTrkRadiusTool(const std::string & type, const std::string & name, 
                     const IInterface* parent);
  /** destructor */
  virtual ~T2TauTrkRadiusTool();
  /** initialize */
  StatusCode initialize();
  /** finalize */
  StatusCode finalize();
  /** propogates tracks to the calorimeneter surface and calculates 
      number of tracks matched to the cluster*/
  StatusCode execute(const TrigTauCluster *pTrigTauCluster,
                     const TrigTauTracksInfo* pTracks,
                     TrigTau& pTrigTau);
private:
  /** only tracks with pt > m_pTCut are counted for matching */
  double m_pTCut;
  /** only tracks within m_coneCut radius with respect to the cluster center
      are counted for the track average distance */
  double m_coneCut;
  /** only tracks with |DeltaZ0|< m_dZ0Max are selected*/
  double m_dZ0Max;
  
};
#endif
