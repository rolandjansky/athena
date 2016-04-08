/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**********************************************************************************
 * @Project: Trigger
 * @Package: TrigParticleTPCnv
 * @Class  : TrigL2Bphys_p3
 *
 * @brief persistent partner for TrigL2Bphys
 *
 * @author Andrew Hamilton  <Andrew.Hamilton@cern.ch>  - U. Geneva
 * @author Francesca Bucci  <F.Bucci@cern.ch>          - U. Geneva
 *
 * File and Version Information:
 * $Id: TrigL2Bphys_p3.h,v 1.2 2010-08-11 demelian Exp $
 **********************************************************************************/
#ifndef TRIGPARTICLETPCNV_TRIGL2BPHYS_P3_H
#define TRIGPARTICLETPCNV_TRIGL2BPHYS_P3_H

#include "DataModelAthenaPool/ElementLink_p3.h"
#include "DataModelAthenaPool/ElementLinkVector_p1.h"

class TrigL2Bphys_p3 
{
  friend class TrigL2BphysCnv_p3;

 public:
  
  TrigL2Bphys_p3() {}
  virtual ~TrigL2Bphys_p3(){}
  
  //private:

  float m_allFloats[8];//m_eta,m_phi,m_mass,m_fitmass,m_fitchi2,m_fitx,m_fity,m_fitz
  int m_allInts[3];//m_roiID,m_particleType,m_fitndof

  ElementLinkIntVector_p1 m_trackVector;
  ElementLinkInt_p3 m_secondaryDecay;

};

#endif
