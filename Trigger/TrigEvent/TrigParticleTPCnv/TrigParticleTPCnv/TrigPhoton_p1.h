/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**********************************************************************************
 * @Project: Trigger
 * @Package: TrigParticleTPCnv
 * @Class  : TrigPhoton_p1
 *
 * @brief persistent partner for TrigPhoton
 *
 * @author Andrew Hamilton  <Andrew.Hamilton@cern.ch>  - U. Geneva
 * @author Francesca Bucci  <F.Bucci@cern.ch>          - U. Geneva
 *
 * File and Version Information:
 * $Id: TrigPhoton_p1.h,v 1.2 2009-04-01 22:13:31 salvator Exp $
 **********************************************************************************/
#ifndef TRIGPARTICLETPCNV_TRIGPHOTON_P1_H
#define TRIGPARTICLETPCNV_TRIGPHOTON_P1_H

#include <stdint.h>
#include <string>
#include "FourMom/P4PtEtaPhiM.h"
#include "TrigParticle/TrigPhoton.h"
#include "AthenaPoolUtilities/TPObjRef.h"

class TrigPhoton_p1 
{
  friend class TrigPhotonCnv_p1;

 public:
  
  TrigPhoton_p1() {}
  virtual ~TrigPhoton_p1(){}
  
 private:

  int   m_roiID;
  float m_HadEt; 
  float m_energyRatio;
  float m_rCore;
  float m_dPhi;
  float m_dEta;
  bool  m_valid;
  TPObjRef  m_cluster;

  TPObjRef  m_P4PtEtaPhiM;

};

#endif
