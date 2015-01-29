/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**********************************************************************************
 * @Project: Trigger
 * @Package: TrigParticleTPCnv
 * @Class  : TrigElectron_p1
 *
 * @brief persistent partner for TrigElectron
 *
 * @author Andrew Hamilton  <Andrew.Hamilton@cern.ch>  - U. Geneva
 * @author Francesca Bucci  <f.bucci@cern.ch>          - U. Geneva
 *
 * File and Version Information:
 * $Id: TrigElectron_p1.h,v 1.2 2009-04-01 22:13:30 salvator Exp $
 **********************************************************************************/
#ifndef TRIGPARTICLETPCNV_TRIGELECTRON_P1_H
#define TRIGPARTICLETPCNV_TRIGELECTRON_P1_H

#include "AthenaPoolUtilities/TPObjRef.h"

#include "TrigInDetEvent/TrigInDetTrack.h"

class TrigElectron_p1 
{

 public:
  
  TrigElectron_p1() {}
  virtual ~TrigElectron_p1() {}
  friend class TrigElectronCnv_p1;
  
 private:

  int   m_roiID; 
  TrigInDetTrack::AlgoId m_trackAlgo; 
  int m_trackIndx;
  int m_charge;
  float m_Zvtx; 
  float m_err_Zvtx;
  float m_err_eta;
  float m_err_phi;
  float m_err_Pt; 
  float m_dEta;  
  float m_dPhi;
  float m_etoverpt;
  bool  m_valid; 

  //the P4PtEtaPhiM base class
  TPObjRef m_p4PtEtaPhiM;

};

#endif
