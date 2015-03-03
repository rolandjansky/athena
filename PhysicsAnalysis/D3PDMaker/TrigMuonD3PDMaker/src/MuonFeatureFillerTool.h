// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: MuonFeatureFillerTool.h 314012 2010-08-04 12:59:40Z kanno $
#ifndef TRIGMUOND3PDMAKER_MUONFEATUREFILLERTOOL_H
#define TRIGMUOND3PDMAKER_MUONFEATUREFILLERTOOL_H

// EDM include(s):
#include "TrigMuonEvent/MuonFeature.h"

// D3PD include(s):
#include "D3PDMakerUtils/BlockFillerTool.h"

namespace D3PD {

   /**
    *  @short Tool filling information about MuonFeature objects
    *
    * @author Takayuki Kanno
    *
    * $Revision: 314012 $
    * $Date: 2010-08-04 14:59:40 +0200 (Wed, 04 Aug 2010) $
    */
   class MuonFeatureFillerTool : public BlockFillerTool< MuonFeature > {

   public:
      /// Regular AlgTool constructor
      MuonFeatureFillerTool( const std::string& type, const std::string& name,
                                 const IInterface* parent );

      /// Function declaring the output variables
      virtual StatusCode book();
      /// Function filling the output variables
      virtual StatusCode fill( const MuonFeature& muon );

   private:
      bool    m_writeRecoInfo;
      bool    m_writeHitsInfo;

      float*  m_dir_phi;
      float*  m_zeta;
      float*  m_dir_zeta;
      float*  m_radius;
      float*  m_beta;
      float*  m_br_radius;
      float*  m_br_sagitta;
      float*  m_ec_alpha;
      float*  m_ec_beta;
      int*    m_RoIId;
      int*    m_saddress;
      int*    m_algoId;

      float*  m_sp1_r;
      float*  m_sp1_z;
      float*  m_sp1_slope;
      float*  m_sp2_r;
      float*  m_sp2_z;
      float*  m_sp2_slope;
      float*  m_sp3_r;
      float*  m_sp3_z;
      float*  m_sp3_slope;
      double* m_dq_var1;
      double* m_dq_var2;


   }; // class MuonFeatureFillerTool

} // namespace D3PD

#endif // TRIGMUOND3PDMAKER_MUONFEATUREFILLERTOOL_H
