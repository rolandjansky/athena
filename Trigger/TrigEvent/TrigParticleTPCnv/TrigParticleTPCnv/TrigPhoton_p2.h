/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**********************************************************************************
 * @Project: Trigger
 * @Package: TrigParticleTPCnv
 * @Class  : TrigPhoton_p2
 *
 * @brief persistent partner for TrigPhoton
 *
 * @author Andrew Hamilton  <Andrew.Hamilton@cern.ch>  - U. Geneva
 * @author Francesca Bucci  <F.Bucci@cern.ch>          - U. Geneva
 *
 * File and Version Information:
 * $Id: TrigPhoton_p2.h,v 1.2 2009-04-01 22:13:31 salvator Exp $
 **********************************************************************************/
#ifndef TRIGPARTICLETPCNV_TRIGPHOTON_P2_H
#define TRIGPARTICLETPCNV_TRIGPHOTON_P2_H

#include "DataModelAthenaPool/ElementLink_p1.h"

#include "AthenaPoolUtilities/TPObjRef.h"


class TrigPhoton_p2 
{
  friend class TrigPhotonCnv_p2;

 public:
  
  TrigPhoton_p2() : 
    m_roiID(0),
    m_HadEt(-9999.9), 
    m_energyRatio(-9999.9),
    m_rCore(-9999.9),
    m_dPhi(-9999.9),
    m_dEta(-9999.9),
    m_cl_e_frac_S0(-9999.9),
    m_cl_e_frac_S1(-9999.9),
    m_cl_e_frac_S2(-9999.9),
    m_cl_e_frac_S3(-9999.9),
    m_Fside(-9999.9),       
    m_Weta2(-9999.9),       
    m_valid(false)
  {}

  virtual ~TrigPhoton_p2(){}
  
  //private:

  int   m_roiID;
  float m_HadEt; 
  float m_energyRatio;
  float m_rCore;
  float m_dPhi;
  float m_dEta;
  float m_cl_e_frac_S0;
  float m_cl_e_frac_S1;
  float m_cl_e_frac_S2;
  float m_cl_e_frac_S3;
  float m_Fside;       
  float m_Weta2;       
  bool  m_valid;

  //TPObjRef  m_cluster;
  ElementLinkInt_p1 m_cluster;

  TPObjRef  m_p4PtEtaPhiM;

};

#endif
