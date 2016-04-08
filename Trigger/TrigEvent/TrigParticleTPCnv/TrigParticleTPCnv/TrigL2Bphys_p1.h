/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**********************************************************************************
 * @Project: Trigger
 * @Package: TrigParticleTPCnv
 * @Class  : TrigL2Bphys_p1
 *
 * @brief persistent partner for TrigL2Bphys
 *
 * @author Andrew Hamilton  <Andrew.Hamilton@cern.ch>  - U. Geneva
 * @author Francesca Bucci  <F.Bucci@cern.ch>          - U. Geneva
 *
 * File and Version Information:
 * $Id: TrigL2Bphys_p1.h,v 1.2 2009-04-01 22:13:31 salvator Exp $
 **********************************************************************************/
#ifndef TRIGPARTICLETPCNV_TRIGL2BPHYS_P1_H
#define TRIGPARTICLETPCNV_TRIGL2BPHYS_P1_H

#include "AthenaPoolUtilities/TPObjRef.h"

class TrigL2Bphys_p1 
{
  friend class TrigL2BphysCnv_p1;

 public:
  
  TrigL2Bphys_p1() {}
  virtual ~TrigL2Bphys_p1(){}

  enum pType_p1{PHIKK=0, DSPHIPI=1, BMUMU=2, BMUMUX=3, JPSIEE=4, JPSIMUMU=5, MULTIMU=6, BKMUMU=7, BDKSTMUMU=8, BSPHIMUMU=9, LBLMUMU=10, BCDSMUMU=11};

  //private:

  int   m_roiID = 0;
  pType_p1   m_particleType = PHIKK;
  float m_eta = 0; 
  float m_phi = 0;
  float m_mass = 0;
  float m_dist = 0;
  bool  m_valid = false;
  TPObjRef  m_pVertex;
  TPObjRef  m_secondaryDecay;

};

#endif
