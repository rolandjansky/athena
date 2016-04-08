/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**********************************************************************************
 * @Project: Trigger
 * @Package: TrigParticleTPCnv
 * @Class  : TrigEFBphys_p1
 *
 * @brief persistent partner for TrigEFBphys
 *
 * @author Andrew Hamilton  <Andrew.Hamilton@cern.ch>  - U. Geneva
 * @author Francesca Bucci  <F.Bucci@cern.ch>          - U. Geneva
 *
 * File and Version Information:
 * $Id: TrigEFBphys_p1.h,v 1.2 2009-04-01 22:13:30 salvator Exp $
 **********************************************************************************/
#ifndef TRIGPARTICLETPCNV_TRIGEFBPHYS_P1_H
#define TRIGPARTICLETPCNV_TRIGEFBPHYS_P1_H

#include "AthenaPoolUtilities/TPObjRef.h"

class TrigEFBphys_p1 
{
  friend class TrigEFBphysCnv_p1;

 public:
  
  TrigEFBphys_p1() {}
  virtual ~TrigEFBphys_p1(){}
  
  enum pType_p1{PHIKK=0, DSPHIPI=1, BMUMU=2, BMUMUX=3, JPSIEE=4, JPSIMUMU=5, MULTIMU=6, BKMUMU=7, BDKSTMUMU=8, BSPHIMUMU=9, LBLMUMU=10, BCDSMUMU=11};
  
  //private:

  int   m_roiID;
  pType_p1   m_particleType;
  float m_eta; 
  float m_phi;
  float m_mass;
  bool  m_valid;
  TPObjRef  m_secondaryDecay;

};

#endif
