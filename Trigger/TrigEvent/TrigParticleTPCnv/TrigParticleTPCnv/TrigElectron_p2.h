/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**********************************************************************************
 * @Project: Trigger
 * @Package: TrigParticleTPCnv
 * @Class  : TrigElectron_p2
 *
 * @brief persistent partner for TrigElectron
 *
 * @author Andrew Hamilton  <Andrew.Hamilton@cern.ch>  - U. Geneva
 * @author Francesca Bucci  <f.bucci@cern.ch>          - U. Geneva
 * @author Ricardo Goncalo  <r.goncalo@rhul.ac.uk>     - Royal Holloway 
 *
 * File and Version Information:
 * $Id: TrigElectron_p2.h,v 1.2 2009-06-14 14:23:00 jgoncalo Exp $
 **********************************************************************************/
#ifndef TRIGPARTICLETPCNV_TRIGELECTRON_P2_H
#define TRIGPARTICLETPCNV_TRIGELECTRON_P2_H

#include "AthenaPoolUtilities/TPObjRef.h"
#include "DataModelAthenaPool/ElementLink_p1.h"

#include "TrigInDetEvent/TrigInDetTrack.h"

class TrigElectron_p2 
{

 public:
  
  TrigElectron_p2() :
    m_roiWord(0), 
    m_valid(false),
    m_tr_Algo(TrigInDetTrack::NULLID),
    m_tr_Zvtx(0), 
    m_tr_nr_trt_hits(-999),
    m_tr_nr_trt_hithresh_hits(-999),
    m_tr_eta_at_calo(-9999.9),  
    m_tr_phi_at_calo(-9999.9),
    m_etoverpt(-9999.9),
    m_cl_eta(-9999.9), 
    m_cl_phi(-9999.9),
    m_cl_Rcore(-9999.9),  
    m_cl_Eratio(-9999.9), 
    m_cl_EThad(-9999.9),
    m_cl_e_frac_S0(-999.9),
    m_cl_e_frac_S1(-999.9),
    m_cl_e_frac_S2(-999.9),
    m_cl_e_frac_S3(-999.9)
    {}
  virtual ~TrigElectron_p2() {}
  friend class TrigElectronCnv_p2;
  
  //private:

  unsigned int m_roiWord; 
  bool  m_valid; 
  TrigInDetTrack::AlgoId m_tr_Algo; 
  float m_tr_Zvtx; 
  int   m_tr_nr_trt_hits;
  int   m_tr_nr_trt_hithresh_hits;
  float m_tr_eta_at_calo;  
  float m_tr_phi_at_calo;
  float m_etoverpt;
  float m_cl_eta; 
  float m_cl_phi;
  float m_cl_Rcore;  
  float m_cl_Eratio; 
  float m_cl_EThad;
  float m_cl_e_frac_S0;
  float m_cl_e_frac_S1;
  float m_cl_e_frac_S2;
  float m_cl_e_frac_S3;

  // ElementLinks to track and cluster
  ElementLinkInt_p1 m_cluster;
  ElementLinkInt_p1 m_track;

  //the P4PtEtaPhiM base class
  TPObjRef       m_p4PtEtaPhiM;

};

#endif
