/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**********************************************************************************
 * @Project: Trigger
 * @Package: TrigParticleTPCnv
 * @Class  : TrigPhoton_p3
 *
 * @brief persistent partner for TrigPhoton
 *
 * @author Andrew Hamilton  <Andrew.Hamilton@cern.ch>  - U. Geneva
 * @author Francesca Bucci  <F.Bucci@cern.ch>          - U. Geneva
 *
 * File and Version Information:
 * $Id: TrigPhoton_p3.h 724426 2016-02-16 23:38:10Z ssnyder $
 **********************************************************************************/
#ifndef TRIGPARTICLETPCNV_TRIGPHOTON_P3_H
#define TRIGPARTICLETPCNV_TRIGPHOTON_P3_H

#include "DataModelAthenaPool/ElementLink_p3.h"

#include "EventCommonTPCnv/P4PtEtaPhiMCnv_p1.h"
//#include "AthenaPoolUtilities/TPObjRef.h"


class TrigPhoton_p3 
{
  friend class TrigPhotonCnv_p3;

 public:
  
  TrigPhoton_p3() : 
    m_roiID(0),
    m_valid(false)
  {}

  virtual ~TrigPhoton_p3(){}
  
  //private:

  int   m_roiID;
  float m_allTheFloats[11];

/*
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
*/
  bool  m_valid;

  //TPObjRef  m_cluster;
  ElementLinkInt_p3 m_cluster;

  //TPObjRef  m_p4PtEtaPhiM;
  P4PtEtaPhiM_p1 m_P4PtEtaPhiM;

};

#endif
