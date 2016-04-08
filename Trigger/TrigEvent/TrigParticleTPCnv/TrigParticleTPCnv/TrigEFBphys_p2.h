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
 * $Id: TrigEFBphys_p2.h,v 1.2 2009-04-01 22:13:30 salvator Exp $
 **********************************************************************************/
#ifndef TRIGPARTICLETPCNV_TRIGEFBPHYS_P2_H
#define TRIGPARTICLETPCNV_TRIGEFBPHYS_P2_H

#include "DataModelAthenaPool/ElementLink_p1.h"
#include "DataModelAthenaPool/ElementLinkVector_p1.h"


class TrigEFBphys_p2 
{
  friend class TrigEFBphysCnv_p2;

 public:
  
  TrigEFBphys_p2() {}
  virtual ~TrigEFBphys_p2(){}

  enum pType_p1{PHIKK=0, DSPHIPI=1, BMUMU=2, BMUMUX=3, JPSIEE=4, JPSIMUMU=5, MULTIMU=6, BKMUMU=7, BDKSTMUMU=8, BSPHIMUMU=9, LBLMUMU=10, BCDSMUMU=11};
  
  //private:

  int   m_roiID = 0;
  pType_p1   m_particleType = PHIKK;
  float m_eta = 0; 
  float m_phi = 0;
  float m_mass = 0;
  float m_fitmass = 0;
  float m_fitchi2 = 0;
  int m_fitndof = 0;
  float m_fitx = 0;
  float m_fity = 0;
  float m_fitz = 0;

  ElementLinkInt_p1  m_secondaryDecay;
  ElementLinkIntVector_p1 m_trackVector;

};

#endif
