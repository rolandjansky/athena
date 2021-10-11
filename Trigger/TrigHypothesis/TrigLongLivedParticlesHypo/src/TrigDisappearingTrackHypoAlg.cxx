/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration

  * Trigger Hypo Tool, that is aimed at triggering disappearing tracks
  * author Kunihiro Nagano <kunihiro.nagano@cern.ch> 
*/
#include "TrigDisappearingTrackHypoAlg.h"
#include "AthViews/ViewHelper.h"
#include "TrigCompositeUtils/TrigCompositeUtils.h"
#include "GaudiKernel/SystemOfUnits.h"
#include "AthenaMonitoringKernel/Monitored.h"
#include "PathResolver/PathResolver.h"

using TrigCompositeUtils::createAndStore; 
using TrigCompositeUtils::DecisionContainer;
using TrigCompositeUtils::DecisionAuxContainer;
using TrigCompositeUtils::DecisionIDContainer;
using TrigCompositeUtils::decisionIDs;
using TrigCompositeUtils::newDecisionIn;
using TrigCompositeUtils::linkToPrevious;
using TrigCompositeUtils::viewString;
using TrigCompositeUtils::featureString;
using TrigCompositeUtils::hypoAlgNodeName;
using TrigCompositeUtils::findLink;
using TrigCompositeUtils::LinkInfo;
using TrigCompositeUtils::Decision;
using TrigCompositeUtils::allFailed;

// ------------------------------------------------------------------------------------------------
// ------------------------------------------------------------------------------------------------

TrigDisappearingTrackHypoAlg::TrigDisappearingTrackHypoAlg( const std::string& name, 
							    ISvcLocator* pSvcLocator ) : 
   ::HypoBase( name, pSvcLocator ) {}

// ------------------------------------------------------------------------------------------------
// ------------------------------------------------------------------------------------------------

StatusCode TrigDisappearingTrackHypoAlg::initialize()
{
   CHECK( m_hypoTools.retrieve() );
   CHECK( m_DisTrkCandKey.initialize() );
   CHECK( m_DisTrkBDTSelKey.initialize());

   if ( !m_monTool.empty() ) CHECK( m_monTool.retrieve() );

   for (size_t slot = 0; slot < SG::getNSlots(); ++slot) {
      EventContext dummyContext(/*dummyEventNumber*/0, slot);

      m_tmva_pix4l_sct0_reader.get(dummyContext)->reset( new TMVA::Reader( "!Color:!Silent" ));
      m_tmva_pix4l_sct1p_reader.get(dummyContext)->reset(new TMVA::Reader( "!Color:!Silent" ));
      m_tmva_pix3l_sct0_reader.get(dummyContext)->reset( new TMVA::Reader( "!Color:!Silent" ));
      m_tmva_pix3l_sct1p_reader.get(dummyContext)->reset(new TMVA::Reader( "!Color:!Silent" ));

      // --- declare variables to the reader
      // pix4l_sct0
      (**m_tmva_pix4l_sct0_reader.get(dummyContext)).AddVariable("pt",             m_tmva_pix4l_sct0_pt.get(dummyContext));
      (**m_tmva_pix4l_sct0_reader.get(dummyContext)).AddVariable("z0",             m_tmva_pix4l_sct0_z0.get(dummyContext));
      (**m_tmva_pix4l_sct0_reader.get(dummyContext)).AddVariable("d0",             m_tmva_pix4l_sct0_d0.get(dummyContext));
      (**m_tmva_pix4l_sct0_reader.get(dummyContext)).AddVariable("trkiso3_dr01",   m_tmva_pix4l_sct0_trkiso3_dr01.get(dummyContext));
      (**m_tmva_pix4l_sct0_reader.get(dummyContext)).AddVariable("trkiso3_dr0201", m_tmva_pix4l_sct0_trkiso3_dr0201.get(dummyContext));
      (**m_tmva_pix4l_sct0_reader.get(dummyContext)).AddVariable("chi2ndof",       m_tmva_pix4l_sct0_chi2ndof.get(dummyContext));
      (**m_tmva_pix4l_sct0_reader.get(dummyContext)).AddVariable("chi2ndof_pix",   m_tmva_pix4l_sct0_chi2ndof_pix.get(dummyContext));
      (**m_tmva_pix4l_sct0_reader.get(dummyContext)).AddVariable("refit_pt",       m_tmva_pix4l_sct0_refit_pt.get(dummyContext));
      (**m_tmva_pix4l_sct0_reader.get(dummyContext)).AddVariable("n_pix",          m_tmva_pix4l_sct0_n_pix.get(dummyContext));
      (**m_tmva_pix4l_sct0_reader.get(dummyContext)).AddVariable("refit_ptratio",  m_tmva_pix4l_sct0_refit_ptratio.get(dummyContext));
      (**m_tmva_pix4l_sct0_reader.get(dummyContext)).AddVariable("refit_chi2ndof", m_tmva_pix4l_sct0_refit_chi2ndof.get(dummyContext));
      (**m_tmva_pix4l_sct0_reader.get(dummyContext)).AddVariable("n_bl",           m_tmva_pix4l_sct0_n_bl.get(dummyContext));

      // pix4l_sct1p
      (**m_tmva_pix4l_sct1p_reader.get(dummyContext)).AddVariable("pt",                   m_tmva_pix4l_sct1p_pt.get(dummyContext));
      (**m_tmva_pix4l_sct1p_reader.get(dummyContext)).AddVariable("refit_pt",             m_tmva_pix4l_sct1p_refit_pt.get(dummyContext));
      (**m_tmva_pix4l_sct1p_reader.get(dummyContext)).AddVariable("refit_z0",             m_tmva_pix4l_sct1p_refit_z0.get(dummyContext));
      (**m_tmva_pix4l_sct1p_reader.get(dummyContext)).AddVariable("refit_d0",             m_tmva_pix4l_sct1p_refit_d0.get(dummyContext));
      (**m_tmva_pix4l_sct1p_reader.get(dummyContext)).AddVariable("n_sct",                m_tmva_pix4l_sct1p_n_sct.get(dummyContext));
      (**m_tmva_pix4l_sct1p_reader.get(dummyContext)).AddVariable("refit_ptratio",        m_tmva_pix4l_sct1p_refit_ptratio.get(dummyContext));
      (**m_tmva_pix4l_sct1p_reader.get(dummyContext)).AddVariable("refit_chi2ndof_ratio", m_tmva_pix4l_sct1p_refit_chi2ndof_ratio.get(dummyContext));
      (**m_tmva_pix4l_sct1p_reader.get(dummyContext)).AddVariable("trkiso3_dr01",         m_tmva_pix4l_sct1p_trkiso3_dr01.get(dummyContext));
      (**m_tmva_pix4l_sct1p_reader.get(dummyContext)).AddVariable("trkiso3_dr0201",       m_tmva_pix4l_sct1p_trkiso3_dr0201.get(dummyContext));
      (**m_tmva_pix4l_sct1p_reader.get(dummyContext)).AddVariable("is_fail",              m_tmva_pix4l_sct1p_is_fail.get(dummyContext));
      (**m_tmva_pix4l_sct1p_reader.get(dummyContext)).AddVariable("chi2ndof_pix",         m_tmva_pix4l_sct1p_chi2ndof_pix.get(dummyContext));
      (**m_tmva_pix4l_sct1p_reader.get(dummyContext)).AddVariable("n_pix",                m_tmva_pix4l_sct1p_n_pix.get(dummyContext));

      // pix3l_sct0
      (**m_tmva_pix3l_sct0_reader.get(dummyContext)).AddVariable("pt",             m_tmva_pix3l_sct0_pt.get(dummyContext));
      (**m_tmva_pix3l_sct0_reader.get(dummyContext)).AddVariable("z0",             m_tmva_pix3l_sct0_z0.get(dummyContext));
      (**m_tmva_pix3l_sct0_reader.get(dummyContext)).AddVariable("d0",             m_tmva_pix3l_sct0_d0.get(dummyContext));
      (**m_tmva_pix3l_sct0_reader.get(dummyContext)).AddVariable("chi2ndof",       m_tmva_pix3l_sct0_chi2ndof.get(dummyContext));
      (**m_tmva_pix3l_sct0_reader.get(dummyContext)).AddVariable("chi2ndof_pix",   m_tmva_pix3l_sct0_chi2ndof_pix.get(dummyContext));
      (**m_tmva_pix3l_sct0_reader.get(dummyContext)).AddVariable("trkiso3_dr01",   m_tmva_pix3l_sct0_trkiso3_dr01.get(dummyContext));
      (**m_tmva_pix3l_sct0_reader.get(dummyContext)).AddVariable("trkiso3_dr0201", m_tmva_pix3l_sct0_trkiso3_dr0201.get(dummyContext));
      (**m_tmva_pix3l_sct0_reader.get(dummyContext)).AddVariable("refit_pt",       m_tmva_pix3l_sct0_refit_pt.get(dummyContext));
      (**m_tmva_pix3l_sct0_reader.get(dummyContext)).AddVariable("refit_z0",       m_tmva_pix3l_sct0_refit_z0.get(dummyContext));
      (**m_tmva_pix3l_sct0_reader.get(dummyContext)).AddVariable("refit_d0",       m_tmva_pix3l_sct0_refit_d0.get(dummyContext));
      (**m_tmva_pix3l_sct0_reader.get(dummyContext)).AddVariable("n_pix",          m_tmva_pix4l_sct0_n_pix.get(dummyContext));
      (**m_tmva_pix3l_sct0_reader.get(dummyContext)).AddVariable("n_bl",           m_tmva_pix3l_sct0_n_bl.get(dummyContext));

      // pix3l_sct1p
      (**m_tmva_pix3l_sct1p_reader.get(dummyContext)).AddVariable("pt",             m_tmva_pix3l_sct1p_pt.get(dummyContext));
      (**m_tmva_pix3l_sct1p_reader.get(dummyContext)).AddVariable("z0",             m_tmva_pix3l_sct1p_z0.get(dummyContext));
      (**m_tmva_pix3l_sct1p_reader.get(dummyContext)).AddVariable("d0",             m_tmva_pix3l_sct1p_d0.get(dummyContext));
      (**m_tmva_pix3l_sct1p_reader.get(dummyContext)).AddVariable("refit_pt",       m_tmva_pix3l_sct1p_refit_pt.get(dummyContext));
      (**m_tmva_pix3l_sct1p_reader.get(dummyContext)).AddVariable("refit_z0",       m_tmva_pix3l_sct1p_refit_z0.get(dummyContext));
      (**m_tmva_pix3l_sct1p_reader.get(dummyContext)).AddVariable("refit_d0",       m_tmva_pix3l_sct1p_refit_d0.get(dummyContext));
      (**m_tmva_pix3l_sct1p_reader.get(dummyContext)).AddVariable("n_pix",          m_tmva_pix3l_sct1p_n_pix.get(dummyContext));
      (**m_tmva_pix3l_sct1p_reader.get(dummyContext)).AddVariable("n_sct",          m_tmva_pix3l_sct1p_n_sct.get(dummyContext));
      (**m_tmva_pix3l_sct1p_reader.get(dummyContext)).AddVariable("refit_ptratio",  m_tmva_pix3l_sct1p_refit_ptratio.get(dummyContext));
      (**m_tmva_pix3l_sct1p_reader.get(dummyContext)).AddVariable("is_fail",        m_tmva_pix3l_sct1p_is_fail.get(dummyContext));
      (**m_tmva_pix3l_sct1p_reader.get(dummyContext)).AddVariable("n_bl",           m_tmva_pix3l_sct1p_n_bl.get(dummyContext));
      (**m_tmva_pix3l_sct1p_reader.get(dummyContext)).AddVariable("chi2ndof",       m_tmva_pix3l_sct1p_chi2ndof.get(dummyContext));
      (**m_tmva_pix3l_sct1p_reader.get(dummyContext)).AddVariable("trkiso3_dr01",   m_tmva_pix3l_sct1p_trkiso3_dr01.get(dummyContext));
      (**m_tmva_pix3l_sct1p_reader.get(dummyContext)).AddVariable("trkiso3_dr0201", m_tmva_pix3l_sct1p_trkiso3_dr0201.get(dummyContext));
      (**m_tmva_pix3l_sct1p_reader.get(dummyContext)).AddVariable("refit_chi2ndof", m_tmva_pix3l_sct1p_refit_chi2ndof.get(dummyContext));

      // --- Book the MVA methods
      std::string file;
      std::string weightfile;
      std::string tuningVer;
      const std::string methodName = "BDT method";
      //
      tuningVer  = "v21a";
      file = "TrigDisappearingTrackTriggerHypo/pix4l_sct0.BDT.weights." + tuningVer + ".xml";
      weightfile = PathResolver::find_calib_file(file.c_str());
      ATH_MSG_INFO( "opening weightfile for Pix4l_SCT0 = " << weightfile );
      (**m_tmva_pix4l_sct0_reader.get(dummyContext)).BookMVA(methodName, weightfile); 
      //
      tuningVer  = "v21b"; // "b" only for this category
      file = "TrigDisappearingTrackTriggerHypo/pix4l_sct1p.BDT.weights." + tuningVer + ".xml";
      weightfile = PathResolver::find_calib_file(file.c_str());
      ATH_MSG_INFO( "opening weightfile for Pix4l_SCT1p = " << weightfile );
      (**m_tmva_pix4l_sct1p_reader.get(dummyContext)).BookMVA(methodName, weightfile); 
      //
      tuningVer  = "v21a";
      file = "TrigDisappearingTrackTriggerHypo/pix3l_sct0.BDT.weights." + tuningVer + ".xml";
      weightfile = PathResolver::find_calib_file(file.c_str());
      ATH_MSG_INFO( "opening weightfile for Pix3l_SCT0 = " << weightfile );
      (**m_tmva_pix3l_sct0_reader.get(dummyContext)).BookMVA(methodName, weightfile); 
      //
      tuningVer  = "v21a";
      file = "TrigDisappearingTrackTriggerHypo/pix3l_sct1p.BDT.weights." + tuningVer + ".xml";
      weightfile = PathResolver::find_calib_file(file.c_str());
      ATH_MSG_INFO( "opening weightfile for Pix3l_SCT1p = " << weightfile );
      (**m_tmva_pix3l_sct1p_reader.get(dummyContext)).BookMVA(methodName, weightfile); 
   }

   return StatusCode::SUCCESS;
}

// ------------------------------------------------------------------------------------------------
// ------------------------------------------------------------------------------------------------

StatusCode TrigDisappearingTrackHypoAlg::execute( const EventContext& context ) const
{
   // previous decisions
   ATH_MSG_DEBUG( "Retrieving pervious decision: \"" << decisionInput().key() << "\"" );
   auto previousDecisionsHandle = SG::makeHandle( decisionInput(), context );
   ATH_CHECK( previousDecisionsHandle.isValid() );

   ATH_MSG_DEBUG( "Running with " << previousDecisionsHandle->size() << " previous decisions" ); 
   if( previousDecisionsHandle->size()!=1 ) {
      ATH_MSG_ERROR( "Previous decision handle size is not 1. It is" << previousDecisionsHandle->size() );
      return StatusCode::FAILURE;
   }
   const Decision * previousDecision = previousDecisionsHandle->at(0);
   
   TrigCompositeUtils::DecisionIDContainer previousDecisionIDs;
   TrigCompositeUtils::decisionIDs(previousDecision, previousDecisionIDs);
   ATH_MSG_VERBOSE( "IDs of active legs:" );
   for(auto decisionID: previousDecisionIDs) { ATH_MSG_VERBOSE( "    " << decisionID ); }

   // new output decisions
   ATH_MSG_DEBUG( "Creating new output decision handle" ); 
   SG::WriteHandle<DecisionContainer> outputHandle = createAndStore(decisionOutput(), context ); 
   auto outputDecisions = outputHandle.ptr();

   // input disappearing track objects
   auto DisTrkCandHandle = SG::makeHandle(m_DisTrkCandKey, context );
   ATH_CHECK( DisTrkCandHandle.isValid() );
   ATH_MSG_DEBUG( "DisTrkCand handle size: " << DisTrkCandHandle->size() );

   const xAOD::TrigCompositeContainer * disTrkCandContainer = DisTrkCandHandle.get();
   if( disTrkCandContainer == nullptr ) {
      ATH_MSG_ERROR( "ERROR Cannot get DisappearingTrks container" );
      return StatusCode::FAILURE;
   }

   // output EDM object
   auto DisTrkBDTSelContainer    = std::make_unique<xAOD::TrigCompositeContainer>();
   auto DisTrkBDTSelContainerAux = std::make_unique<xAOD::TrigCompositeAuxContainer>();
   DisTrkBDTSelContainer->setStore(DisTrkBDTSelContainerAux.get());

   xAOD::TrigCompositeContainer* disTrkContainer = DisTrkBDTSelContainer.get();

   // Prepare inputs to HypoTool
   ATH_CHECK( createCandidates(context, disTrkCandContainer, disTrkContainer) );

   std::vector<TrigDisappearingTrackHypoTool::DisTrkHypoInfo> disTrkHypoInputs;

   for ( auto distrk : *disTrkContainer ) {
      Decision* newDecision = TrigCompositeUtils::newDecisionIn( outputDecisions, previousDecision, TrigCompositeUtils::hypoAlgNodeName(), context);

      ElementLink<xAOD::TrigCompositeContainer> distrkEL = ElementLink<xAOD::TrigCompositeContainer>(*outputHandle, distrk->index());
      ATH_CHECK( distrkEL.isValid() );

      ATH_CHECK( newDecision->setObjectLink<xAOD::TrigCompositeContainer>(TrigCompositeUtils::featureString(), distrkEL) );

      TrigDisappearingTrackHypoTool::DisTrkHypoInfo hypoInfo{ newDecision, distrk, previousDecisionIDs };
      disTrkHypoInputs.push_back( hypoInfo );
   }

   // Loop over all hypoToolinputs and get their decisions
   for ( auto & tool: m_hypoTools ) {
      ATH_MSG_VERBOSE( "+++++ Now computing decision for " << tool->name() );
      ATH_CHECK( tool->decide( disTrkHypoInputs ) );    
   } 
   
   DecisionContainer::iterator it = outputDecisions->begin();
   while(it != outputDecisions->end()) {
      ATH_MSG_VERBOSE( "+++++ outputDecision: " << *it << " +++++" );
      if ( allFailed( *it ) ) {
	 ATH_MSG_VERBOSE( "---> all failed, erasing" );
	 it = outputDecisions->erase(it);
      } else {
	 ATH_MSG_VERBOSE( "---> not all failed" );
	 ++it;
      }
   }

   // record
   SG::WriteHandle<xAOD::TrigCompositeContainer> DisTrkBDTSelHandle(m_DisTrkBDTSelKey, context);
   ATH_CHECK( DisTrkBDTSelHandle.record( std::move( DisTrkBDTSelContainer ), std::move( DisTrkBDTSelContainerAux ) ) );
   ATH_MSG_VERBOSE( "recorded DisTrkBDTSel object to SG" );

   //
   ATH_CHECK( hypoBaseOutputProcessing(outputHandle) );

   //
   return StatusCode::SUCCESS;
}

// ------------------------------------------------------------------------------------------------
// ------------------------------------------------------------------------------------------------

StatusCode TrigDisappearingTrackHypoAlg::createCandidates( const EventContext& context,  const xAOD::TrigCompositeContainer* disTrkCandContainer,
							   xAOD::TrigCompositeContainer* disTrkBDTSelContainer) const
{
   // monitoring
   std::vector<int>   mnt_category;
   auto mon_category = Monitored::Collection("category", mnt_category);
   auto monDisTrk    = Monitored::Group(m_monTool, mon_category);

   std::vector<float> mnt_cat1_pt;
   std::vector<float> mnt_cat1_z0;
   std::vector<float> mnt_cat1_d0;
   std::vector<float> mnt_cat1_iso01;
   std::vector<float> mnt_cat1_iso0201;
   std::vector<float> mnt_cat1_chi2ndof;
   std::vector<float> mnt_cat1_chi2ndof_pix;
   std::vector<float> mnt_cat1_refit_pt;
   std::vector<int>   mnt_cat1_n_pix;
   std::vector<float> mnt_cat1_refit_ptratio;
   std::vector<float> mnt_cat1_refit_chi2ndof;
   std::vector<int>   mnt_cat1_n_bl;
   std::vector<float> mnt_cat1_bdtscore;
   auto mon_cat1_pt             = Monitored::Collection("pix4_sct0_pt",             mnt_cat1_pt);
   auto mon_cat1_z0             = Monitored::Collection("pix4_sct0_z0",             mnt_cat1_z0);
   auto mon_cat1_d0             = Monitored::Collection("pix4_sct0_d0",             mnt_cat1_d0);
   auto mon_cat1_iso01          = Monitored::Collection("pix4_sct0_iso01",          mnt_cat1_iso01);
   auto mon_cat1_iso0201        = Monitored::Collection("pix4_sct0_iso0201",        mnt_cat1_iso0201);
   auto mon_cat1_chi2ndof       = Monitored::Collection("pix4_sct0_chi2ndof",       mnt_cat1_chi2ndof);
   auto mon_cat1_chi2ndof_pix   = Monitored::Collection("pix4_sct0_chi2ndof_pix",   mnt_cat1_chi2ndof_pix);
   auto mon_cat1_refit_pt       = Monitored::Collection("pix4_sct0_refit_pt",       mnt_cat1_refit_pt);
   auto mon_cat1_n_pix          = Monitored::Collection("pix4_sct0_n_pix",          mnt_cat1_n_pix);
   auto mon_cat1_refit_ptratio  = Monitored::Collection("pix4_sct0_refit_ptratio",  mnt_cat1_refit_ptratio);
   auto mon_cat1_refit_chi2ndof = Monitored::Collection("pix4_sct0_refit_chi2ndof", mnt_cat1_refit_chi2ndof);
   auto mon_cat1_n_bl           = Monitored::Collection("pix4_sct0_n_bl",           mnt_cat1_n_bl);
   auto mon_cat1_bdtscore       = Monitored::Collection("pix4_sct0_bdtscore",       mnt_cat1_bdtscore);
   auto monDisTrkCat1 = Monitored::Group(m_monTool, mon_cat1_pt, mon_cat1_z0, mon_cat1_d0, mon_cat1_iso01, mon_cat1_iso0201, mon_cat1_chi2ndof,
					 mon_cat1_chi2ndof_pix, mon_cat1_refit_pt, mon_cat1_n_pix, mon_cat1_refit_ptratio,
					 mon_cat1_refit_chi2ndof, mon_cat1_n_bl, mon_cat1_bdtscore);
   //
   std::vector<float> mnt_cat2_pt;
   std::vector<float> mnt_cat2_refit_pt;
   std::vector<float> mnt_cat2_refit_z0;
   std::vector<float> mnt_cat2_refit_d0;
   std::vector<int>   mnt_cat2_n_sct;
   std::vector<float> mnt_cat2_refit_ptratio;
   std::vector<float> mnt_cat2_refit_chi2ndof_ratio;
   std::vector<float> mnt_cat2_iso01;
   std::vector<float> mnt_cat2_iso0201;
   std::vector<float> mnt_cat2_chi2ndof_pix;
   std::vector<int>   mnt_cat2_n_pix;
   std::vector<float> mnt_cat2_bdtscore;
   auto mon_cat2_pt             = Monitored::Collection("pix4_sct1p_pt",             mnt_cat2_pt);
   auto mon_cat2_refit_pt       = Monitored::Collection("pix4_sct1p_refit_pt",       mnt_cat2_refit_pt);
   auto mon_cat2_refit_z0       = Monitored::Collection("pix4_sct1p_refit_z0",       mnt_cat2_refit_z0);
   auto mon_cat2_refit_d0       = Monitored::Collection("pix4_sct1p_refit_d0",       mnt_cat2_refit_d0);
   auto mon_cat2_n_sct          = Monitored::Collection("pix4_sct1p_n_sct",          mnt_cat2_n_sct);
   auto mon_cat2_refit_ptratio  = Monitored::Collection("pix4_sct1p_refit_ptratio",  mnt_cat2_refit_ptratio);
   auto mon_cat2_refit_chi2ndof_ratio = Monitored::Collection("pix4_sct1p_refit_chi2ndof_ratio", mnt_cat2_refit_chi2ndof_ratio);
   auto mon_cat2_iso01          = Monitored::Collection("pix4_sct1p_iso01",          mnt_cat2_iso01);
   auto mon_cat2_iso0201        = Monitored::Collection("pix4_sct1p_iso0201",        mnt_cat2_iso0201);
   auto mon_cat2_chi2ndof_pix   = Monitored::Collection("pix4_sct1p_chi2ndof_pix",   mnt_cat2_chi2ndof_pix);
   auto mon_cat2_n_pix          = Monitored::Collection("pix4_sct1p_n_pix",          mnt_cat2_n_pix);
   auto mon_cat2_bdtscore       = Monitored::Collection("pix4_sct1p_bdtscore",       mnt_cat2_bdtscore);
   auto monDisTrkCat2 = Monitored::Group(m_monTool, mon_cat2_pt, mon_cat2_refit_pt, mon_cat2_refit_z0, mon_cat2_refit_d0, mon_cat2_n_sct,
					 mon_cat2_refit_ptratio, mon_cat2_refit_chi2ndof_ratio, mon_cat2_iso01, mon_cat2_iso0201,
					 mon_cat2_chi2ndof_pix, mon_cat2_n_pix, mon_cat2_bdtscore);
   //
   std::vector<float> mnt_cat3_pt;
   std::vector<float> mnt_cat3_z0;
   std::vector<float> mnt_cat3_d0;
   std::vector<float> mnt_cat3_chi2ndof;
   std::vector<float> mnt_cat3_chi2ndof_pix;
   std::vector<float> mnt_cat3_iso01;
   std::vector<float> mnt_cat3_iso0201;
   std::vector<float> mnt_cat3_refit_pt;
   std::vector<float> mnt_cat3_refit_z0;
   std::vector<float> mnt_cat3_refit_d0;
   std::vector<int>   mnt_cat3_n_pix;
   std::vector<int>   mnt_cat3_n_bl;
   std::vector<float> mnt_cat3_bdtscore;
   auto mon_cat3_pt             = Monitored::Collection("pix3_sct0_pt",             mnt_cat3_pt);
   auto mon_cat3_z0             = Monitored::Collection("pix3_sct0_z0",             mnt_cat3_z0);
   auto mon_cat3_d0             = Monitored::Collection("pix3_sct0_d0",             mnt_cat3_d0);
   auto mon_cat3_chi2ndof       = Monitored::Collection("pix3_sct0_chi2ndof",       mnt_cat3_chi2ndof);
   auto mon_cat3_chi2ndof_pix   = Monitored::Collection("pix3_sct0_chi2ndof_pix",   mnt_cat3_chi2ndof_pix);
   auto mon_cat3_iso01          = Monitored::Collection("pix3_sct0_iso01",          mnt_cat3_iso01);
   auto mon_cat3_iso0201        = Monitored::Collection("pix3_sct0_iso0201",        mnt_cat3_iso0201);
   auto mon_cat3_refit_pt       = Monitored::Collection("pix3_sct0_refit_pt",       mnt_cat3_refit_pt);
   auto mon_cat3_refit_z0       = Monitored::Collection("pix3_sct0_refit_z0",       mnt_cat3_refit_z0);
   auto mon_cat3_refit_d0       = Monitored::Collection("pix3_sct0_refit_d0",       mnt_cat3_refit_d0);
   auto mon_cat3_n_pix          = Monitored::Collection("pix3_sct0_n_pix",          mnt_cat3_n_pix);
   auto mon_cat3_n_bl           = Monitored::Collection("pix3_sct0_n_bl",           mnt_cat3_n_bl);
   auto mon_cat3_bdtscore       = Monitored::Collection("pix3_sct0_bdtscore",       mnt_cat3_bdtscore);
   auto monDisTrkCat3 = Monitored::Group(m_monTool, mon_cat3_pt, mon_cat3_z0, mon_cat3_d0, mon_cat3_chi2ndof, mon_cat3_chi2ndof_pix, 
					 mon_cat3_iso01, mon_cat3_iso0201, mon_cat3_refit_pt, mon_cat3_refit_z0, mon_cat3_refit_d0,
					 mon_cat3_n_pix, mon_cat3_n_bl, mon_cat3_bdtscore);
   //
   std::vector<float> mnt_cat4_pt;
   std::vector<float> mnt_cat4_z0;
   std::vector<float> mnt_cat4_d0;
   std::vector<float> mnt_cat4_refit_pt;
   std::vector<float> mnt_cat4_refit_z0;
   std::vector<float> mnt_cat4_refit_d0;
   std::vector<int>   mnt_cat4_n_pix;
   std::vector<int>   mnt_cat4_n_sct;
   std::vector<float> mnt_cat4_refit_ptratio;
   std::vector<int>   mnt_cat4_n_bl;
   std::vector<float> mnt_cat4_chi2ndof;
   std::vector<float> mnt_cat4_iso01;
   std::vector<float> mnt_cat4_iso0201;
   std::vector<float> mnt_cat4_refit_chi2ndof;
   std::vector<float> mnt_cat4_bdtscore;
   auto mon_cat4_pt             = Monitored::Collection("pix3_sct1p_pt",             mnt_cat4_pt);
   auto mon_cat4_z0             = Monitored::Collection("pix3_sct1p_z0",             mnt_cat4_z0);
   auto mon_cat4_d0             = Monitored::Collection("pix3_sct1p_d0",             mnt_cat4_d0);
   auto mon_cat4_refit_pt       = Monitored::Collection("pix3_sct1p_refit_pt",       mnt_cat4_refit_pt);
   auto mon_cat4_refit_z0       = Monitored::Collection("pix3_sct1p_refit_z0",       mnt_cat4_refit_z0);
   auto mon_cat4_refit_d0       = Monitored::Collection("pix3_sct1p_refit_d0",       mnt_cat4_refit_d0);
   auto mon_cat4_n_pix          = Monitored::Collection("pix3_sct1p_n_pix",          mnt_cat4_n_pix);
   auto mon_cat4_n_sct          = Monitored::Collection("pix3_sct1p_n_sct",          mnt_cat4_n_sct);
   auto mon_cat4_refit_ptratio  = Monitored::Collection("pix3_sct1p_refit_ptratio",  mnt_cat4_refit_ptratio);
   auto mon_cat4_n_bl           = Monitored::Collection("pix3_sct1p_n_bl",           mnt_cat4_n_bl);
   auto mon_cat4_chi2ndof       = Monitored::Collection("pix3_sct1p_chi2ndof",       mnt_cat4_chi2ndof);
   auto mon_cat4_iso01          = Monitored::Collection("pix3_sct1p_iso01",          mnt_cat4_iso01);
   auto mon_cat4_iso0201        = Monitored::Collection("pix3_sct1p_iso0201",        mnt_cat4_iso0201);
   auto mon_cat4_refit_chi2ndof = Monitored::Collection("pix3_sct1p_refit_chi2ndof", mnt_cat4_refit_chi2ndof);
   auto mon_cat4_bdtscore       = Monitored::Collection("pix3_sct1p_bdtscore",       mnt_cat4_bdtscore);
   auto monDisTrkCat4 = Monitored::Group(m_monTool, mon_cat4_pt, mon_cat4_z0, mon_cat4_d0, mon_cat4_refit_pt, mon_cat4_refit_z0, mon_cat4_refit_d0,
					 mon_cat4_n_pix, mon_cat4_n_sct, mon_cat4_refit_ptratio, mon_cat4_n_bl, mon_cat4_chi2ndof,
					 mon_cat4_iso01, mon_cat4_iso0201, mon_cat4_refit_chi2ndof, mon_cat4_bdtscore);

   //

   const std::string prefixIn  = "disTrkCand";
   const std::string prefixOut = "disTrk";

   unsigned int i_cand=0;

   for ( const xAOD::TrigComposite* cand : *disTrkCandContainer ) {

      ATH_MSG_VERBOSE("===== i_cand: " << i_cand++ << " =====" );

      // category
      int category = cand->getDetail<int>(prefixIn+"_category");
      if( category<=0 || category>=5 ) {
	 ATH_MSG_VERBOSE("category is out of range = " << category);
	 continue;
      }
      mnt_category.push_back(category);
      
      // get values
      float pt              = cand->getDetail<float>(prefixIn+"_pt");
      float refit_pt        = cand->getDetail<float>(prefixIn+"_refit_pt");
      pt       /= 1000.0;
      refit_pt /= 1000.0;
      int   is_fail         = cand->getDetail<int>  (prefixIn+"_is_fail");
      float d0_wrtVtx       = cand->getDetail<float>(prefixIn+"_d0_wrtVtx");
      float z0_wrtVtx       = cand->getDetail<float>(prefixIn+"_z0_wrtVtx");
      float chi2            = cand->getDetail<float>(prefixIn+"_chi2");
      float ndof            = cand->getDetail<float>(prefixIn+"_ndof");
      float chi2ndof        = ( std::abs(ndof) > 1e-4 ) ? chi2 / ndof : 20;
      int   n_hits_pix      = cand->getDetail<int>  (prefixIn+"_n_hits_pix");
      int   n_hits_sct      = cand->getDetail<int>  (prefixIn+"_n_hits_sct");
      int   n_hits_bl       = cand->getDetail<int>  (prefixIn+"_n_hits_innermost");
      float iso3_dr01       = cand->getDetail<float>(prefixIn+"_iso3_dr01");
      iso3_dr01 /= 1000;
      float iso3_dr02       = cand->getDetail<float>(prefixIn+"_iso3_dr02");
      iso3_dr02 /= 1000;
      float refit_ptratio   = ( std::abs(pt) > 1e-4 ) ? refit_pt / pt : 0;
      float refit_d0_wrtVtx = cand->getDetail<float>(prefixIn+"_refit_d0_wrtVtx");
      float refit_z0_wrtVtx = cand->getDetail<float>(prefixIn+"_refit_z0_wrtVtx");
      float refit_chi2      = cand->getDetail<float>(prefixIn+"_refit_chi2");
      float refit_ndof      = cand->getDetail<float>(prefixIn+"_refit_ndof");
      float refit_chi2ndof  = ( std::abs(refit_ndof) > 1e-4 ) ? refit_chi2 / refit_ndof : 20;
      float refit_chi2ndof_ratio = (std::abs(chi2ndof) > 1e-4 ) ? refit_chi2ndof / chi2ndof : 0;
      float chi2_pix=0; float ndof_pix=0;
      chi2_pix += cand->getDetail<float>(prefixIn+"_chi2sum_br_ibl");
      chi2_pix += cand->getDetail<float>(prefixIn+"_chi2sum_br_pix1");
      chi2_pix += cand->getDetail<float>(prefixIn+"_chi2sum_br_pix2");
      chi2_pix += cand->getDetail<float>(prefixIn+"_chi2sum_br_pix3");
      ndof_pix += cand->getDetail<float>(prefixIn+"_ndofsum_br_ibl");
      ndof_pix += cand->getDetail<float>(prefixIn+"_ndofsum_br_pix1");
      ndof_pix += cand->getDetail<float>(prefixIn+"_ndofsum_br_pix2");
      ndof_pix += cand->getDetail<float>(prefixIn+"_ndofsum_br_pix3");
      float chi2ndof_pix = ( std::abs(ndof_pix) > 1e-4 ) ? chi2_pix / ndof_pix : 0;
      
      // BDT score
      float bdt_score = 0.0;
      if( category==1 ) {
	 bdt_score = bdt_eval_pix4l_sct0(context, pt, z0_wrtVtx, d0_wrtVtx, iso3_dr01, iso3_dr02-iso3_dr01, chi2ndof, chi2ndof_pix,
					 refit_pt, n_hits_pix, refit_ptratio, refit_chi2ndof, n_hits_bl);
	 mnt_cat1_pt.push_back(pt);
	 mnt_cat1_z0.push_back(z0_wrtVtx);
	 mnt_cat1_d0.push_back(d0_wrtVtx);
	 mnt_cat1_iso01.push_back(iso3_dr01);
	 mnt_cat1_iso0201.push_back(iso3_dr02-iso3_dr01);
	 mnt_cat1_chi2ndof.push_back(chi2ndof);
	 mnt_cat1_chi2ndof_pix.push_back(chi2ndof_pix);
	 mnt_cat1_refit_pt.push_back(refit_pt);
	 mnt_cat1_n_pix.push_back(n_hits_pix);
	 mnt_cat1_refit_ptratio.push_back(refit_ptratio);
	 mnt_cat1_refit_chi2ndof.push_back(refit_chi2ndof);
	 mnt_cat1_n_bl.push_back(n_hits_bl);
	 mnt_cat1_bdtscore.push_back(bdt_score);
      }
      else if( category==2 ) {
	 bdt_score = bdt_eval_pix4l_sct1p(context, pt, refit_pt, refit_z0_wrtVtx, refit_d0_wrtVtx, n_hits_sct, refit_ptratio,
					  refit_chi2ndof_ratio, iso3_dr01, iso3_dr02-iso3_dr01, is_fail, chi2ndof_pix, n_hits_pix);
	 mnt_cat2_pt.push_back(pt);
	 mnt_cat2_refit_pt.push_back(refit_pt);
	 mnt_cat2_refit_z0.push_back(refit_z0_wrtVtx);
	 mnt_cat2_refit_d0.push_back(refit_d0_wrtVtx);
	 mnt_cat2_n_sct.push_back(n_hits_sct);
	 mnt_cat2_refit_ptratio.push_back(refit_ptratio);
	 mnt_cat2_refit_chi2ndof_ratio.push_back(refit_chi2ndof_ratio);
	 mnt_cat2_iso01.push_back(iso3_dr01);
	 mnt_cat2_iso0201.push_back(iso3_dr02-iso3_dr01);
	 mnt_cat2_chi2ndof_pix.push_back(chi2ndof_pix);
	 mnt_cat2_n_pix.push_back(n_hits_pix);
	 mnt_cat2_bdtscore.push_back(bdt_score);
      }
      else if( category==3 ) {
	 bdt_score = bdt_eval_pix3l_sct0(context, pt, z0_wrtVtx, d0_wrtVtx, chi2ndof, chi2ndof_pix, iso3_dr01, iso3_dr02-iso3_dr01,
					 refit_pt, refit_z0_wrtVtx, refit_d0_wrtVtx, n_hits_pix, n_hits_bl);
	 mnt_cat3_pt.push_back(pt);
	 mnt_cat3_z0.push_back(z0_wrtVtx);
	 mnt_cat3_d0.push_back(d0_wrtVtx);
	 mnt_cat3_chi2ndof.push_back(chi2ndof);
	 mnt_cat3_chi2ndof_pix.push_back(chi2ndof_pix);
	 mnt_cat3_iso01.push_back(iso3_dr01);
	 mnt_cat3_iso0201.push_back(iso3_dr02-iso3_dr01);
	 mnt_cat3_refit_pt.push_back(refit_pt);
	 mnt_cat3_refit_z0.push_back(refit_z0_wrtVtx);
	 mnt_cat3_refit_d0.push_back(refit_d0_wrtVtx);
	 mnt_cat3_n_pix.push_back(n_hits_pix);
	 mnt_cat3_n_bl.push_back(n_hits_bl);
	 mnt_cat3_bdtscore.push_back(bdt_score);
      }
      else if( category==4 ) {
	 bdt_score = bdt_eval_pix3l_sct1p(context, pt, z0_wrtVtx, d0_wrtVtx, refit_pt, refit_z0_wrtVtx, refit_d0_wrtVtx, n_hits_pix, n_hits_sct,
					  refit_ptratio, is_fail, n_hits_bl, chi2ndof, iso3_dr01, iso3_dr02-iso3_dr01, refit_chi2ndof);
	 mnt_cat4_pt.push_back(pt);
	 mnt_cat4_z0.push_back(z0_wrtVtx);
	 mnt_cat4_d0.push_back(d0_wrtVtx);
	 mnt_cat4_refit_pt.push_back(refit_pt);
	 mnt_cat4_refit_z0.push_back(refit_z0_wrtVtx);
	 mnt_cat4_refit_d0.push_back(refit_d0_wrtVtx);
	 mnt_cat4_n_pix.push_back(n_hits_pix);
	 mnt_cat4_n_sct.push_back(n_hits_sct);
	 mnt_cat4_refit_ptratio.push_back(refit_ptratio);
	 mnt_cat4_n_bl.push_back(n_hits_bl);
	 mnt_cat4_chi2ndof.push_back(chi2ndof);
	 mnt_cat4_iso01.push_back(iso3_dr01);
	 mnt_cat4_iso0201.push_back(iso3_dr02-iso3_dr01);
	 mnt_cat4_refit_chi2ndof.push_back(refit_chi2ndof);
	 mnt_cat4_bdtscore.push_back(bdt_score);
      } 
      ATH_MSG_VERBOSE("BDT score = " << bdt_score);

      // create EDM object
      xAOD::TrigComposite *distrk = new xAOD::TrigComposite();
      distrk->makePrivateStore();
      disTrkBDTSelContainer->push_back(distrk);

      distrk->setDetail<int>  (prefixOut+"_category",         category);
      distrk->setDetail<float>(prefixOut+"_pt",               pt*1000.0);
      float eta = cand->getDetail<float>(prefixIn+"_eta");
      float phi = cand->getDetail<float>(prefixIn+"_phi");
      distrk->setDetail<float>(prefixOut+"_eta",              eta);
      distrk->setDetail<float>(prefixOut+"_phi",              phi);
      distrk->setDetail<float>(prefixOut+"_refit_pt",         refit_pt*1000.0);
      distrk->setDetail<int>  (prefixOut+"_is_fail",          is_fail);
      distrk->setDetail<float>(prefixOut+"_d0_wrtVtx",        d0_wrtVtx);
      distrk->setDetail<float>(prefixOut+"_z0_wrtVtx",        z0_wrtVtx);
      distrk->setDetail<float>(prefixOut+"_chi2",             chi2);
      distrk->setDetail<float>(prefixOut+"_ndof",             ndof);
      distrk->setDetail<int>  (prefixOut+"_n_hits_pix",       n_hits_pix);
      distrk->setDetail<int>  (prefixOut+"_n_hits_sct",       n_hits_sct);
      distrk->setDetail<int>  (prefixOut+"_n_hits_innermost", n_hits_bl);
      distrk->setDetail<float>(prefixOut+"_iso3_dr01",        iso3_dr01*1000.0);
      distrk->setDetail<float>(prefixOut+"_iso3_dr02",        iso3_dr02*1000.0);
      distrk->setDetail<float>(prefixOut+"_refit_d0_wrtVtx",  refit_d0_wrtVtx);
      distrk->setDetail<float>(prefixOut+"_refit_z0_wrtVtx",  refit_z0_wrtVtx);
      distrk->setDetail<float>(prefixOut+"_refit_chi2",       refit_chi2);
      distrk->setDetail<float>(prefixOut+"_refit_ndof",       refit_ndof);
      distrk->setDetail<float>(prefixOut+"_chi2ndof_pix",     chi2ndof_pix);
      distrk->setDetail<float>(prefixOut+"_bdtscore",         bdt_score);

      ATH_MSG_VERBOSE("Created a new entry EDM" );
   }

   return StatusCode::SUCCESS;
}

// ------------------------------------------------------------------------------------------------
// ------------------------------------------------------------------------------------------------

inline float TrigDisappearingTrackHypoAlg::BDTinput(float var) const { return ( std::abs(var) < 1e-5 ) ? 1e-5 : var; }

float TrigDisappearingTrackHypoAlg::bdt_eval_pix4l_sct0
(const EventContext& context,
 float pt, float z0, float d0, float trkiso3_dr01, float trkiso3_dr0201, float chi2ndof, float chi2ndof_pix,
 float refit_pt, int n_pix, float refit_ptratio, float refit_chi2ndof, int n_bl) const
{
   *m_tmva_pix4l_sct0_pt.get(context)             = BDTinput(pt);
   *m_tmva_pix4l_sct0_z0.get(context)             = BDTinput(z0);
   *m_tmva_pix4l_sct0_d0.get(context)             = BDTinput(d0);
   *m_tmva_pix4l_sct0_trkiso3_dr01.get(context)   = BDTinput(trkiso3_dr01);
   *m_tmva_pix4l_sct0_trkiso3_dr0201.get(context) = BDTinput(trkiso3_dr0201);
   *m_tmva_pix4l_sct0_chi2ndof.get(context)       = BDTinput(chi2ndof);
   *m_tmva_pix4l_sct0_chi2ndof_pix.get(context)   = BDTinput(chi2ndof_pix);
   *m_tmva_pix4l_sct0_refit_pt.get(context)       = BDTinput(refit_pt);
   *m_tmva_pix4l_sct0_n_pix.get(context)          = BDTinput((float)n_pix);
   *m_tmva_pix4l_sct0_refit_ptratio.get(context)  = BDTinput(refit_ptratio);
   *m_tmva_pix4l_sct0_refit_chi2ndof.get(context) = BDTinput(refit_chi2ndof);
   *m_tmva_pix4l_sct0_n_bl.get(context)           = BDTinput((float)n_bl);
   //
   return (**m_tmva_pix4l_sct0_reader.get(context)).EvaluateMVA("BDT method");
}

float TrigDisappearingTrackHypoAlg::bdt_eval_pix4l_sct1p
(const EventContext& context,
 float pt, float refit_pt, float refit_z0, float refit_d0, int n_sct, float refit_ptratio,
 float refit_chi2ndof_ratio, float trkiso3_dr01, float trkiso3_dr0201, int is_fail, float chi2ndof_pix, int n_pix) const
{
   *m_tmva_pix4l_sct1p_pt.get(context)                   = BDTinput(pt);
   *m_tmva_pix4l_sct1p_refit_pt.get(context)             = BDTinput(refit_pt);
   *m_tmva_pix4l_sct1p_refit_z0.get(context)             = BDTinput(refit_z0);
   *m_tmva_pix4l_sct1p_refit_d0.get(context)             = BDTinput(refit_d0);
   *m_tmva_pix4l_sct1p_n_sct.get(context)                = BDTinput((float)n_sct);
   *m_tmva_pix4l_sct1p_refit_ptratio.get(context)        = BDTinput(refit_ptratio);
   *m_tmva_pix4l_sct1p_refit_chi2ndof_ratio.get(context) = BDTinput(refit_chi2ndof_ratio);
   *m_tmva_pix4l_sct1p_trkiso3_dr01.get(context)         = BDTinput(trkiso3_dr01);
   *m_tmva_pix4l_sct1p_trkiso3_dr0201.get(context)       = BDTinput(trkiso3_dr0201);
   *m_tmva_pix4l_sct1p_is_fail.get(context)              = BDTinput((float)is_fail);
   *m_tmva_pix4l_sct1p_chi2ndof_pix.get(context)         = BDTinput(chi2ndof_pix);
   *m_tmva_pix4l_sct1p_n_pix.get(context)                = BDTinput((float)n_pix);

   return (**m_tmva_pix4l_sct1p_reader.get(context)).EvaluateMVA("BDT method");
}

float TrigDisappearingTrackHypoAlg::bdt_eval_pix3l_sct0
(const EventContext& context,
 float pt, float z0, float d0, float chi2ndof, float chi2ndof_pix, float trkiso3_dr01, float trkiso3_dr0201,
 float refit_pt, float refit_z0, float refit_d0, int n_pix, int n_bl) const
{
   *m_tmva_pix3l_sct0_pt.get(context)             = BDTinput(pt);
   *m_tmva_pix3l_sct0_z0.get(context)             = BDTinput(z0);
   *m_tmva_pix3l_sct0_d0.get(context)             = BDTinput(d0);
   *m_tmva_pix3l_sct0_chi2ndof.get(context)       = BDTinput(chi2ndof);
   *m_tmva_pix3l_sct0_chi2ndof_pix.get(context)   = BDTinput(chi2ndof_pix);
   *m_tmva_pix3l_sct0_trkiso3_dr01.get(context)   = BDTinput(trkiso3_dr01);
   *m_tmva_pix3l_sct0_trkiso3_dr0201.get(context) = BDTinput(trkiso3_dr0201);
   *m_tmva_pix3l_sct0_refit_pt.get(context)       = BDTinput(refit_pt);
   *m_tmva_pix3l_sct0_refit_z0.get(context)       = BDTinput(refit_z0);
   *m_tmva_pix3l_sct0_refit_d0.get(context)       = BDTinput(refit_d0);
   *m_tmva_pix3l_sct0_n_pix.get(context)          = BDTinput((float)n_pix);
   *m_tmva_pix3l_sct0_n_bl.get(context)           = BDTinput((float)n_bl);

   return (**m_tmva_pix3l_sct0_reader.get(context)).EvaluateMVA("BDT method");
}

float TrigDisappearingTrackHypoAlg::bdt_eval_pix3l_sct1p
(const EventContext& context,
 float pt, float z0, float d0, float refit_pt, float refit_z0, float refit_d0,
 int n_pix, int n_sct, float refit_ptratio, int is_fail, int n_bl,
 float chi2ndof, float trkiso3_dr01, float trkiso3_dr0201, float refit_chi2ndof) const
{
   *m_tmva_pix3l_sct1p_pt.get(context)             = BDTinput(pt);
   *m_tmva_pix3l_sct1p_z0.get(context)             = BDTinput(z0);
   *m_tmva_pix3l_sct1p_d0.get(context)             = BDTinput(d0);
   *m_tmva_pix3l_sct1p_refit_pt.get(context)       = BDTinput(refit_pt);
   *m_tmva_pix3l_sct1p_refit_z0.get(context)       = BDTinput(refit_z0);
   *m_tmva_pix3l_sct1p_refit_d0.get(context)       = BDTinput(refit_d0);
   *m_tmva_pix3l_sct1p_n_pix.get(context)          = BDTinput((float)n_pix);
   *m_tmva_pix3l_sct1p_n_sct.get(context)          = BDTinput((float)n_sct);
   *m_tmva_pix3l_sct1p_refit_ptratio.get(context)  = BDTinput(refit_ptratio);
   *m_tmva_pix3l_sct1p_is_fail.get(context)        = BDTinput((float)is_fail);
   *m_tmva_pix3l_sct1p_n_bl.get(context)           = BDTinput((float)n_bl);
   *m_tmva_pix3l_sct1p_chi2ndof.get(context)       = BDTinput(chi2ndof);
   *m_tmva_pix3l_sct1p_trkiso3_dr01.get(context)   = BDTinput(trkiso3_dr01);
   *m_tmva_pix3l_sct1p_trkiso3_dr0201.get(context) = BDTinput(trkiso3_dr0201);
   *m_tmva_pix3l_sct1p_refit_chi2ndof.get(context) = BDTinput(refit_chi2ndof);

   return (**m_tmva_pix3l_sct1p_reader.get(context)).EvaluateMVA("BDT method");
}

// ------------------------------------------------------------------------------------------------
// ------------------------------------------------------------------------------------------------

