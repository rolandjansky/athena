/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**********************************************************************************
 * @Project: Trigger
 * @Package: TrigParticleTPCnv
 * @Class  : TrigTau_p3
 *
 * @brief persistent partner for TrigTau
 *
 * @author Andrew Hamilton  <Andrew.Hamilton@cern.ch>  - U. Geneva
 * @author Francesca Bucci  <f.bucci@cern.ch>          - U. Geneva
 * last changes by Olga Igonkina (Nikhef)
 *
 * File and Version Information:
 * $Id: TrigTau_p3.h,v 1.2 2009-04-01 22:13:31 salvator Exp $
 **********************************************************************************/
#ifndef TRIGPARTICLETPCNV_TRIGTAU_P3_H
#define TRIGPARTICLETPCNV_TRIGTAU_P3_H

//#include "AthenaPoolUtilities/TPObjRef.h"
#include "EventCommonTPCnv/P4PtEtaPhiMCnv_p1.h"

class TrigTau_p3
{
  friend class TrigTauCnv_p3;

 public:
  
  TrigTau_p3():     
    m_roiID(0),
    m_Zvtx(0), 
    m_err_Zvtx(0),
    m_etCalibCluster(0),
    m_simpleEtFlow(0),
    m_nMatchedTracks(0),
    m_trkAvgDist(0),
    m_etOverPtLeadTrk(0),
    m_p4PtEtaPhiM()
  {};

  ~TrigTau_p3() {};
  
  //private:

  
  int   m_roiID;
  float m_Zvtx; 
  float m_err_Zvtx;
  float m_etCalibCluster;
  float m_simpleEtFlow;
  int   m_nMatchedTracks;
  float m_trkAvgDist;
  float m_etOverPtLeadTrk;

  //the P4PtEtaPhiM base class
  P4PtEtaPhiM_p1 m_p4PtEtaPhiM;

};

#endif
