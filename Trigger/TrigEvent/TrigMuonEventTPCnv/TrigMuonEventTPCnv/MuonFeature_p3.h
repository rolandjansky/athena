/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**********************************************************************************
 * @Project: Trigger
 * @Package: TrigMuonEventTPCnv
 * @Class  : MuonFeature_p3
 *
 * @brief persistent partner for MuonFeature
 *
 * @author Andrew Hamilton  <Andrew.Hamilton@cern.ch>  - U. Geneva
 * @author Francesca Bucci  <F.Bucci@cern.ch>          - U. Geneva
 *
 * File and Version Information:
 * $Id: MuonFeature_p3.h,v 1.2 2009-04-01 22:12:15 salvator Exp $
 **********************************************************************************/
#ifndef TRIGMUONEVENTTPCNV_MUONFEATURE_P3_H
#define TRIGMUONEVENTTPCNV_MUONFEATURE_P3_H

#include <stdint.h>
#include <string>
#include "TrigMuonEvent/MuonFeature.h"

class MuonFeature_p3 
{
      friend class MuonFeatureCnv;

   public:
  
      MuonFeature_p3() {}
      virtual ~MuonFeature_p3(){}
  
      //private:

      MuonFeature::AlgoId m_algoId;
      //int    m_RoIId;
      //int    m_saddress;
      int    m_allTheInts[2];
      //float  m_pt;
      //float  m_radius;
      //float  m_eta;
      //float  m_phi;
      //float  m_dir_phi;
      //float  m_zeta;
      //float  m_dir_zeta;
      //float  m_beta;
      //float  m_sp1_r;
      //float  m_sp1_z;
      //float  m_sp1_slope;
      //float  m_sp2_r;
      //float  m_sp2_z;
      //float  m_sp2_slope;
      //float  m_sp3_r;
      //float  m_sp3_z;
      //float  m_sp3_slope;
      //float  m_br_radius;
      //float  m_br_sagitta;
      //float  m_ec_alpha;
      //float  m_ec_beta;
      //double m_dq_var1;
      //double m_dq_var2;
      float m_allTheFloats[23];
};

#endif
