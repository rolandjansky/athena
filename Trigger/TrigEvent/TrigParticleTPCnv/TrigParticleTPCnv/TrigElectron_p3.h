/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**********************************************************************************
 * @Project: Trigger
 * @Package: TrigParticleTPCnv
 * @Class  : TrigElectron_p3
 *
 * @brief persistent partner for TrigElectron
 *
 * @author Andrew Hamilton  <Andrew.Hamilton@cern.ch>  - U. Geneva
 * @author Francesca Bucci  <f.bucci@cern.ch>          - U. Geneva
 * @author Ricardo Goncalo  <r.goncalo@rhul.ac.uk>     - Royal Holloway 
 *
 * File and Version Information:
 * $Id: TrigElectron_p3.h 724426 2016-02-16 23:38:10Z ssnyder $
 **********************************************************************************/
#ifndef TRIGPARTICLETPCNV_TRIGELECTRON_P3_H
#define TRIGPARTICLETPCNV_TRIGELECTRON_P3_H

//#include "AthenaPoolUtilities/TPObjRef.h"
#include "DataModelAthenaPool/ElementLink_p3.h"

#include "EventCommonTPCnv/P4PtEtaPhiMCnv_p1.h"

#include "TrigInDetEvent/TrigInDetTrack.h"

class TrigElectron_p3
{

 public:
  
  TrigElectron_p3() :
    m_roiWord(0), 
    m_valid(false),
    m_tr_Algo(TrigInDetTrack::NULLID),
    m_tr_nr_trt_hits(-999),
    m_tr_nr_trt_hithresh_hits(-999)
    {}
  virtual ~TrigElectron_p3() {}
  friend class TrigElectronCnv_p3;
  
  //private:

  unsigned int m_roiWord; 
  bool  m_valid; 
  TrigInDetTrack::AlgoId m_tr_Algo; 
  int   m_tr_nr_trt_hits;
  int   m_tr_nr_trt_hithresh_hits;

/*  
  float m_tr_Zvtx; 
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
*/

  float m_allTheFloats[13];

  // ElementLinks to track and cluster
  ElementLinkInt_p3 m_cluster;
  ElementLinkInt_p3 m_track;

  //the P4PtEtaPhiM base class
//  TPObjRef       m_p4PtEtaPhiM;
  P4PtEtaPhiM_p1 m_P4PtEtaPhiM;

};

#endif
