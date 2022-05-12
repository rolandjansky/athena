/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/
#ifndef TRIGLONGLIVEDPARTICLESHYPO_TRIGDISTRACKHYPOALG_H
#define TRIGLONGLIVEDPARTICLESHYPO_TRIGDISTRACKHYPOALG_H

#include <string>

#include "Gaudi/Property.h"
#include "DecisionHandling/HypoBase.h"
#include "TrigDisappearingTrackHypoTool.h"
#include "AthenaMonitoringKernel/GenericMonitoringTool.h"
#include "AthenaKernel/SlotSpecificObj.h"
#include "CxxUtils/checker_macros.h"
#include "TMVA/Reader.h"

/**
 * @class TrigDisappearingTrackHypoAlg
 * @brief Implements Hypo selection on triggering disappearing tracks
 * @author Kunihiro Nagano <kunihiro.nagano@cern.ch> - KEK
 **/

class TrigDisappearingTrackHypoAlg : public ::HypoBase 
{ 
public: 

   TrigDisappearingTrackHypoAlg( const std::string& name, ISvcLocator* pSvcLocator );
   
   virtual StatusCode  initialize() override;
   virtual StatusCode  execute(const EventContext& context) const override;
   
private: 

   ToolHandleArray< TrigDisappearingTrackHypoTool >  m_hypoTools        {this, "HypoTools", {}, "Tools to perfrom selection"};
   SG::ReadHandleKey< xAOD::TrigCompositeContainer > m_DisTrkCandKey    {this, "DisTrkCand",  "HLT_DisTrkCand",   ""};
   SG::WriteHandleKey<xAOD::TrigCompositeContainer>  m_DisTrkBDTSelKey  {this, "DisTrkBDTSel","HLT_DisTrkBDTSel", ""};

   StatusCode createCandidates(const EventContext&, const xAOD::TrigCompositeContainer*, xAOD::TrigCompositeContainer*) const;

   ToolHandle<GenericMonitoringTool> m_monTool{ this, "MonTool", "", "Monitoring tool" };

   // BDT selection methods
   float bdt_eval_pix4l_sct0 (const EventContext&, float, float, float, float, float, float, float, float, int,   float, float, int) const;
   float bdt_eval_pix4l_sct1p(const EventContext&, float, float, float, float, int,   float, float, float, float, int,   float, int) const;
   float bdt_eval_pix3l_sct0 (const EventContext&, float, float, float, float, float, float, float, float, float, float, int,   int) const;
   float bdt_eval_pix3l_sct1p(const EventContext&, float, float, float, float, float, float, int,   int,   float, int,   int,   float, float, float, float) const;
   inline float BDTinput(float) const;

   /// TMVA reader and associated variables for pix4l_sct0
   struct TMVA_pix4l_sct0 {
      std::unique_ptr<TMVA::Reader> tmva;
      float pt;
      float z0;
      float d0;
      float trkiso3_dr01;
      float trkiso3_dr0201;
      float chi2ndof;
      float chi2ndof_pix;
      float refit_pt;
      float n_pix;
      float refit_ptratio;
      float refit_chi2ndof;
      float n_bl;
   };
   mutable SG::SlotSpecificObj<TMVA_pix4l_sct0> m_tmva_pix4l_sct0_reader ATLAS_THREAD_SAFE;

   /// TMVA reader and associated variables for pix4l_sct1p
   struct TMVA_pix4l_sct1p {
      std::unique_ptr<TMVA::Reader> tmva;
      float pt;
      float refit_pt;
      float refit_z0;
      float refit_d0;
      float n_sct;
      float refit_ptratio;
      float refit_chi2ndof_ratio;
      float trkiso3_dr01;
      float trkiso3_dr0201;
      float is_fail;
      float chi2ndof_pix;
      float n_pix;
   };
   mutable SG::SlotSpecificObj<TMVA_pix4l_sct1p> m_tmva_pix4l_sct1p_reader ATLAS_THREAD_SAFE;

   /// TMVA reader and associated variables for pix3l_sct0
   struct TMVA_pix3l_sct0 {
      std::unique_ptr<TMVA::Reader> tmva;
      float pt;
      float z0;
      float d0;
      float chi2ndof;
      float chi2ndof_pix;
      float trkiso3_dr01;
      float trkiso3_dr0201;
      float refit_pt;
      float refit_z0;
      float refit_d0;
      float n_pix;
      float n_bl;
   };
   mutable SG::SlotSpecificObj<TMVA_pix3l_sct0> m_tmva_pix3l_sct0_reader ATLAS_THREAD_SAFE;

   /// TMVA reader and associated variables for pix3l_sct1p
   struct TMVA_pix3l_sct1p {
      std::unique_ptr<TMVA::Reader> tmva;
      float pt;
      float z0;
      float d0;
      float refit_pt;
      float refit_z0;
      float refit_d0;
      float n_pix;
      float n_sct;
      float refit_ptratio;
      float is_fail;
      float n_bl;
      float chi2ndof;
      float trkiso3_dr01;
      float trkiso3_dr0201;
      float refit_chi2ndof;
   };
   mutable SG::SlotSpecificObj<TMVA_pix3l_sct1p> m_tmva_pix3l_sct1p_reader ATLAS_THREAD_SAFE;
}; 

#endif //> !TRIGLONGLIVEDPARTICLESHYPO_TRIGDISTRACKHYPOALG_H
