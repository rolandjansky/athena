/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/
#ifndef TRIGLONGLIVEDPARTICLESHYPO_TRIGDISTRACKHYPOALG_H
#define TRIGLONGLIVEDPARTICLESHYPO_TRIGDISTRACKHYPOALG_H

#include <string>

#include "Gaudi/Property.h"
#include "DecisionHandling/HypoBase.h"
#include "TrigDisappearingTrackHypoTool.h"
#include "AthenaMonitoringKernel/GenericMonitoringTool.h"
#include "AthenaKernel/SlotSpecificObj.h"
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

   // TMVA reader
   mutable SG::SlotSpecificObj<std::unique_ptr<TMVA::Reader> > m_tmva_pix4l_sct0_reader  ATLAS_THREAD_SAFE;
   mutable SG::SlotSpecificObj<std::unique_ptr<TMVA::Reader> > m_tmva_pix4l_sct1p_reader ATLAS_THREAD_SAFE;
   mutable SG::SlotSpecificObj<std::unique_ptr<TMVA::Reader> > m_tmva_pix3l_sct0_reader  ATLAS_THREAD_SAFE;
   mutable SG::SlotSpecificObj<std::unique_ptr<TMVA::Reader> > m_tmva_pix3l_sct1p_reader ATLAS_THREAD_SAFE;

   // TMVA variables
   // pix4l_sct0
   mutable SG::SlotSpecificObj<float> m_tmva_pix4l_sct0_pt             ATLAS_THREAD_SAFE;
   mutable SG::SlotSpecificObj<float> m_tmva_pix4l_sct0_z0             ATLAS_THREAD_SAFE;
   mutable SG::SlotSpecificObj<float> m_tmva_pix4l_sct0_d0             ATLAS_THREAD_SAFE;
   mutable SG::SlotSpecificObj<float> m_tmva_pix4l_sct0_trkiso3_dr01   ATLAS_THREAD_SAFE;
   mutable SG::SlotSpecificObj<float> m_tmva_pix4l_sct0_trkiso3_dr0201 ATLAS_THREAD_SAFE;
   mutable SG::SlotSpecificObj<float> m_tmva_pix4l_sct0_chi2ndof       ATLAS_THREAD_SAFE;
   mutable SG::SlotSpecificObj<float> m_tmva_pix4l_sct0_chi2ndof_pix   ATLAS_THREAD_SAFE;
   mutable SG::SlotSpecificObj<float> m_tmva_pix4l_sct0_refit_pt       ATLAS_THREAD_SAFE;
   mutable SG::SlotSpecificObj<float> m_tmva_pix4l_sct0_n_pix          ATLAS_THREAD_SAFE;
   mutable SG::SlotSpecificObj<float> m_tmva_pix4l_sct0_refit_ptratio  ATLAS_THREAD_SAFE;
   mutable SG::SlotSpecificObj<float> m_tmva_pix4l_sct0_refit_chi2ndof ATLAS_THREAD_SAFE;
   mutable SG::SlotSpecificObj<float> m_tmva_pix4l_sct0_n_bl           ATLAS_THREAD_SAFE;

   // pix4l_sct1p
   mutable SG::SlotSpecificObj<float> m_tmva_pix4l_sct1p_pt                   ATLAS_THREAD_SAFE;
   mutable SG::SlotSpecificObj<float> m_tmva_pix4l_sct1p_refit_pt             ATLAS_THREAD_SAFE;
   mutable SG::SlotSpecificObj<float> m_tmva_pix4l_sct1p_refit_z0             ATLAS_THREAD_SAFE;
   mutable SG::SlotSpecificObj<float> m_tmva_pix4l_sct1p_refit_d0             ATLAS_THREAD_SAFE;
   mutable SG::SlotSpecificObj<float> m_tmva_pix4l_sct1p_n_sct                ATLAS_THREAD_SAFE;
   mutable SG::SlotSpecificObj<float> m_tmva_pix4l_sct1p_refit_ptratio        ATLAS_THREAD_SAFE;
   mutable SG::SlotSpecificObj<float> m_tmva_pix4l_sct1p_refit_chi2ndof_ratio ATLAS_THREAD_SAFE;
   mutable SG::SlotSpecificObj<float> m_tmva_pix4l_sct1p_trkiso3_dr01         ATLAS_THREAD_SAFE;
   mutable SG::SlotSpecificObj<float> m_tmva_pix4l_sct1p_trkiso3_dr0201       ATLAS_THREAD_SAFE;
   mutable SG::SlotSpecificObj<float> m_tmva_pix4l_sct1p_is_fail              ATLAS_THREAD_SAFE;
   mutable SG::SlotSpecificObj<float> m_tmva_pix4l_sct1p_chi2ndof_pix         ATLAS_THREAD_SAFE;
   mutable SG::SlotSpecificObj<float> m_tmva_pix4l_sct1p_n_pix                ATLAS_THREAD_SAFE;

   // pix3l_sct0
   mutable SG::SlotSpecificObj<float> m_tmva_pix3l_sct0_pt             ATLAS_THREAD_SAFE;
   mutable SG::SlotSpecificObj<float> m_tmva_pix3l_sct0_z0             ATLAS_THREAD_SAFE;
   mutable SG::SlotSpecificObj<float> m_tmva_pix3l_sct0_d0             ATLAS_THREAD_SAFE;
   mutable SG::SlotSpecificObj<float> m_tmva_pix3l_sct0_chi2ndof       ATLAS_THREAD_SAFE;
   mutable SG::SlotSpecificObj<float> m_tmva_pix3l_sct0_chi2ndof_pix   ATLAS_THREAD_SAFE;
   mutable SG::SlotSpecificObj<float> m_tmva_pix3l_sct0_trkiso3_dr01   ATLAS_THREAD_SAFE;
   mutable SG::SlotSpecificObj<float> m_tmva_pix3l_sct0_trkiso3_dr0201 ATLAS_THREAD_SAFE;
   mutable SG::SlotSpecificObj<float> m_tmva_pix3l_sct0_refit_pt       ATLAS_THREAD_SAFE;
   mutable SG::SlotSpecificObj<float> m_tmva_pix3l_sct0_refit_z0       ATLAS_THREAD_SAFE;
   mutable SG::SlotSpecificObj<float> m_tmva_pix3l_sct0_refit_d0       ATLAS_THREAD_SAFE;
   mutable SG::SlotSpecificObj<float> m_tmva_pix3l_sct0_n_pix          ATLAS_THREAD_SAFE;
   mutable SG::SlotSpecificObj<float> m_tmva_pix3l_sct0_n_bl           ATLAS_THREAD_SAFE;

   // pix3l_sct1p
   mutable SG::SlotSpecificObj<float> m_tmva_pix3l_sct1p_pt             ATLAS_THREAD_SAFE;
   mutable SG::SlotSpecificObj<float> m_tmva_pix3l_sct1p_z0             ATLAS_THREAD_SAFE;
   mutable SG::SlotSpecificObj<float> m_tmva_pix3l_sct1p_d0             ATLAS_THREAD_SAFE;
   mutable SG::SlotSpecificObj<float> m_tmva_pix3l_sct1p_refit_pt       ATLAS_THREAD_SAFE;
   mutable SG::SlotSpecificObj<float> m_tmva_pix3l_sct1p_refit_z0       ATLAS_THREAD_SAFE;
   mutable SG::SlotSpecificObj<float> m_tmva_pix3l_sct1p_refit_d0       ATLAS_THREAD_SAFE;
   mutable SG::SlotSpecificObj<float> m_tmva_pix3l_sct1p_n_pix          ATLAS_THREAD_SAFE;
   mutable SG::SlotSpecificObj<float> m_tmva_pix3l_sct1p_n_sct          ATLAS_THREAD_SAFE;
   mutable SG::SlotSpecificObj<float> m_tmva_pix3l_sct1p_refit_ptratio  ATLAS_THREAD_SAFE;
   mutable SG::SlotSpecificObj<float> m_tmva_pix3l_sct1p_is_fail        ATLAS_THREAD_SAFE;
   mutable SG::SlotSpecificObj<float> m_tmva_pix3l_sct1p_n_bl           ATLAS_THREAD_SAFE;
   mutable SG::SlotSpecificObj<float> m_tmva_pix3l_sct1p_chi2ndof       ATLAS_THREAD_SAFE;
   mutable SG::SlotSpecificObj<float> m_tmva_pix3l_sct1p_trkiso3_dr01   ATLAS_THREAD_SAFE;
   mutable SG::SlotSpecificObj<float> m_tmva_pix3l_sct1p_trkiso3_dr0201 ATLAS_THREAD_SAFE;
   mutable SG::SlotSpecificObj<float> m_tmva_pix3l_sct1p_refit_chi2ndof ATLAS_THREAD_SAFE;
}; 

#endif //> !TRIGLONGLIVEDPARTICLESHYPO_TRIGDISTRACKHYPOALG_H
