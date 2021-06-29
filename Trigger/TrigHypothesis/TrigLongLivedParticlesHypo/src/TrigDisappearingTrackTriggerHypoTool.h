/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/
#ifndef TRIGLONGLIVEDPARTICLESHYPO_TRIGDISTRACKTRIGGERHYPOTOOL_H
#define TRIGLONGLIVEDPARTICLESHYPO_TRIGDISTRACKTRIGGERHYPOTOOL_H

#include "Gaudi/Property.h"
#include "AthenaBaseComps/AthAlgTool.h"
#include "AthenaMonitoringKernel/GenericMonitoringTool.h"
#include "TrigCompositeUtils/TrigCompositeUtils.h"
#include "xAODTracking/TrackParticlexAODHelpers.h"
#include "xAODTracking/TrackParticleContainer.h"
#include <string>

#include "TMVA/Reader.h"

/**
 * @class TrigDisappearingTrackTriggerHypoTool
 * @brief Generated the decision for hypo thats triggers disappearing track
 * @author Kunihiro Nagano <kunihiro.nagano@cern.ch>
 **/

class TrigDisappearingTrackTriggerHypoTool : virtual public ::AthAlgTool
{ 
public: 
   TrigDisappearingTrackTriggerHypoTool( const std::string& type, 
					 const std::string& name, 
					 const IInterface* parent );
   
   virtual ~TrigDisappearingTrackTriggerHypoTool();
   virtual StatusCode initialize() override;
   
   struct TrackInfo {
      TrigCompositeUtils::Decision*       decision;
      const xAOD::TrigCompositeContainer* disTrkCandContainer;
      xAOD::TrigCompositeContainer&       disTrkBDTSelContainer;
      const TrigCompositeUtils::DecisionIDContainer previousDecisionsIDs;
   };
   
   /**
    * @brief decides upon a collection of tracks
    **/
   StatusCode decide( TrackInfo& input ) const;

private:

   HLT::Identifier m_decisionId;
   Gaudi::Property< std::vector<float> >  m_cutTrackPtGeV   { this, "cutTrackPtGeV",  { float(10.0) }, "Track pT requirement in GeV" };
   Gaudi::Property< std::vector<float> >  m_cutBDTPix4Sct0  { this, "cutBDTPix4Sct0", { float(0.05) }, "BDT score thershold for Pix4l_Sct0 category" };
   Gaudi::Property< std::vector<float> >  m_cutBDTPix4Sct1p { this, "cutBDTPix4Sct1p",{ float(0.04) }, "BDT score thershold for Pix4l_Sct1p category" };
   Gaudi::Property< std::vector<float> >  m_cutBDTPix3Sct0  { this, "cutBDTPix3Sct0", { float(0.05) }, "BDT score thershold for Pix3l_Sct0 category" };
   Gaudi::Property< std::vector<float> >  m_cutBDTPix3Sct1p { this, "cutBDTPix3Sct1p",{ float(0.04) }, "BDT score thershold for Pix3l_Sct1p category" };
   //
   ToolHandle<GenericMonitoringTool> m_monTool{ this, "MonTool", "", "Monitoring tool" };

   // BDT selection methods
   float bdt_eval_pix4l_sct0 (float, float, float, float, float, float, float, float, int,   float, float, int) const;
   float bdt_eval_pix4l_sct1p(float, float, float, float, int,   float, float, float, float, int,   float, int) const;
   float bdt_eval_pix3l_sct0 (float, float, float, float, float, float, float, float, float, float, int,   int) const;
   float bdt_eval_pix3l_sct1p(float, float, float, float, float, float, int,   int,   float, int,   int,   float, float, float, float) const;
   inline float BDTinput(float) const;

   // TMVA reader
   TMVA::Reader* m_tmva_pix4l_sct0_reader;
   TMVA::Reader* m_tmva_pix4l_sct1p_reader;
   TMVA::Reader* m_tmva_pix3l_sct0_reader;
   TMVA::Reader* m_tmva_pix3l_sct1p_reader;

   // TMVA variables
   // pix4l_sct0
   mutable float m_tmva_pix4l_sct0_pt ATLAS_THREAD_SAFE;
   mutable float m_tmva_pix4l_sct0_z0 ATLAS_THREAD_SAFE;
   mutable float m_tmva_pix4l_sct0_d0 ATLAS_THREAD_SAFE;
   mutable float m_tmva_pix4l_sct0_trkiso3_dr01 ATLAS_THREAD_SAFE;
   mutable float m_tmva_pix4l_sct0_trkiso3_dr0201 ATLAS_THREAD_SAFE;
   mutable float m_tmva_pix4l_sct0_chi2ndof ATLAS_THREAD_SAFE;
   mutable float m_tmva_pix4l_sct0_chi2ndof_pix ATLAS_THREAD_SAFE;
   mutable float m_tmva_pix4l_sct0_refit_pt ATLAS_THREAD_SAFE;
   mutable float m_tmva_pix4l_sct0_n_pix ATLAS_THREAD_SAFE;
   mutable float m_tmva_pix4l_sct0_refit_ptratio ATLAS_THREAD_SAFE;
   mutable float m_tmva_pix4l_sct0_refit_chi2ndof ATLAS_THREAD_SAFE;
   mutable float m_tmva_pix4l_sct0_n_bl ATLAS_THREAD_SAFE;

   // pix4l_sct1p
   mutable float m_tmva_pix4l_sct1p_pt ATLAS_THREAD_SAFE;
   mutable float m_tmva_pix4l_sct1p_refit_pt ATLAS_THREAD_SAFE;
   mutable float m_tmva_pix4l_sct1p_refit_z0 ATLAS_THREAD_SAFE;
   mutable float m_tmva_pix4l_sct1p_refit_d0 ATLAS_THREAD_SAFE;
   mutable float m_tmva_pix4l_sct1p_n_sct ATLAS_THREAD_SAFE;
   mutable float m_tmva_pix4l_sct1p_refit_ptratio ATLAS_THREAD_SAFE;
   mutable float m_tmva_pix4l_sct1p_refit_chi2ndof_ratio ATLAS_THREAD_SAFE;
   mutable float m_tmva_pix4l_sct1p_trkiso3_dr01 ATLAS_THREAD_SAFE;
   mutable float m_tmva_pix4l_sct1p_trkiso3_dr0201 ATLAS_THREAD_SAFE;
   mutable float m_tmva_pix4l_sct1p_is_fail ATLAS_THREAD_SAFE;
   mutable float m_tmva_pix4l_sct1p_chi2ndof_pix ATLAS_THREAD_SAFE;
   mutable float m_tmva_pix4l_sct1p_n_pix ATLAS_THREAD_SAFE;

   // pix3l_sct0
   mutable float m_tmva_pix3l_sct0_pt ATLAS_THREAD_SAFE;
   mutable float m_tmva_pix3l_sct0_z0 ATLAS_THREAD_SAFE;
   mutable float m_tmva_pix3l_sct0_d0 ATLAS_THREAD_SAFE;
   mutable float m_tmva_pix3l_sct0_chi2ndof ATLAS_THREAD_SAFE;
   mutable float m_tmva_pix3l_sct0_chi2ndof_pix ATLAS_THREAD_SAFE;
   mutable float m_tmva_pix3l_sct0_trkiso3_dr01 ATLAS_THREAD_SAFE;
   mutable float m_tmva_pix3l_sct0_trkiso3_dr0201 ATLAS_THREAD_SAFE;
   mutable float m_tmva_pix3l_sct0_refit_pt ATLAS_THREAD_SAFE;
   mutable float m_tmva_pix3l_sct0_refit_z0 ATLAS_THREAD_SAFE;
   mutable float m_tmva_pix3l_sct0_refit_d0 ATLAS_THREAD_SAFE;
   mutable float m_tmva_pix3l_sct0_n_pix ATLAS_THREAD_SAFE;
   mutable float m_tmva_pix3l_sct0_n_bl ATLAS_THREAD_SAFE;

   // pix3l_sct1p
   mutable float m_tmva_pix3l_sct1p_pt ATLAS_THREAD_SAFE;
   mutable float m_tmva_pix3l_sct1p_z0 ATLAS_THREAD_SAFE;
   mutable float m_tmva_pix3l_sct1p_d0 ATLAS_THREAD_SAFE;
   mutable float m_tmva_pix3l_sct1p_refit_pt ATLAS_THREAD_SAFE;
   mutable float m_tmva_pix3l_sct1p_refit_z0 ATLAS_THREAD_SAFE;
   mutable float m_tmva_pix3l_sct1p_refit_d0 ATLAS_THREAD_SAFE;
   mutable float m_tmva_pix3l_sct1p_n_pix ATLAS_THREAD_SAFE;
   mutable float m_tmva_pix3l_sct1p_n_sct ATLAS_THREAD_SAFE;
   mutable float m_tmva_pix3l_sct1p_refit_ptratio ATLAS_THREAD_SAFE;
   mutable float m_tmva_pix3l_sct1p_is_fail ATLAS_THREAD_SAFE;
   mutable float m_tmva_pix3l_sct1p_n_bl ATLAS_THREAD_SAFE;
   mutable float m_tmva_pix3l_sct1p_chi2ndof ATLAS_THREAD_SAFE;
   mutable float m_tmva_pix3l_sct1p_trkiso3_dr01 ATLAS_THREAD_SAFE;
   mutable float m_tmva_pix3l_sct1p_trkiso3_dr0201 ATLAS_THREAD_SAFE;
   mutable float m_tmva_pix3l_sct1p_refit_chi2ndof ATLAS_THREAD_SAFE;
}; 

#endif //> !TRIGLONGLIVEDPARTICLESHYPO_TRIGDISTRACKTRIGGERHYPOTOOL_H
