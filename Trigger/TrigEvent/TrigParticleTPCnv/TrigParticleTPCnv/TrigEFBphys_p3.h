/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**********************************************************************************
 * @Project: Trigger
 * @Package: TrigParticleTPCnv
 * @Class  : TrigEFBphys_p3
 *
 * @brief persistent partner for TrigEFBphys
 *
 * @author Andrew Hamilton  <Andrew.Hamilton@cern.ch>  - U. Geneva
 * @author Francesca Bucci  <F.Bucci@cern.ch>          - U. Geneva
 *
 * File and Version Information:
 * $Id: TrigEFBphys_p3.h,v 1.1 2010-08-12 demelian Exp $
 **********************************************************************************/
#ifndef TRIGPARTICLETPCNV_TRIGEFBPHYS_P3_H
#define TRIGPARTICLETPCNV_TRIGEFBPHYS_P3_H

#include "DataModelAthenaPool/ElementLink_p3.h"
#include "DataModelAthenaPool/ElementLinkVector_p1.h"


class TrigEFBphys_p3 
{
  friend class TrigEFBphysCnv_p3;

 public:
  
  TrigEFBphys_p3() {}
  virtual ~TrigEFBphys_p3(){}
  
  //private:

  int m_allInts[3];// m_roiID,m_particleType,m_fitndof
  float m_allFloats[8];// m_eta,m_phi,m_mass,m_fitmass,m_fitchi2,m_fitx,m_fity,m_fitz  

  ElementLinkInt_p3  m_secondaryDecay;
  ElementLinkIntVector_p1 m_trackVector;

};

#endif
