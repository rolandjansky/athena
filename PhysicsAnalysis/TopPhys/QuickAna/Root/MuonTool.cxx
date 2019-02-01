/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

/// @author Nils Krumnack



//
// includes
//

#include <QuickAna/MuonTool.h>

#include <MuonEfficiencyCorrections/MuonEfficiencyScaleFactors.h>
#include <MuonMomentumCorrections/MuonCalibrationAndSmearingTool.h>
#include <MuonSelectorTools/MuonSelectionTool.h>
#include <QuickAna/AnaToolRetrieve.h>
#include <QuickAna/DefinitionArgs.h>
#include <QuickAna/DefinitionMaker.h>
#include <QuickAna/InternalConfiguration.h>
#include <QuickAna/MessageCheck.h>

#include <xAODTracking/TrackParticle.h>

#include <xAODTracking/TrackParticlexAODHelpers.h>
#include <EventPrimitives/EventPrimitivesHelpers.h>

#include <IsolationSelection/IsolationSelectionTool.h>
#include "PileupReweighting/PileupReweightingTool.h"

//
// method implementations
//

namespace ana
{
  const float GeV = 1000.;

  MuonToolCorrect ::
  MuonToolCorrect (const std::string& name)
    : AsgTool (name), AnaToolCorrect<xAOD::MuonContainer> (name),
      m_calib_smear_16 ("MuonCalibrationAndSmearingTool_16", nullptr),
      m_calib_smear_17 ("MuonCalibrationAndSmearingTool_17", nullptr)
  {
  }



  StatusCode MuonToolCorrect ::
  initialize()
  {
    ATH_MSG_DEBUG("initialize");

    ATH_CHECK (ASG_MAKE_ANA_TOOL (m_calib_smear_16, CP::MuonCalibrationAndSmearingTool));
    ATH_CHECK (m_calib_smear_16.setProperty ("SagittaCorr", true));
    ATH_CHECK (m_calib_smear_16.initialize());
    registerTool (&*m_calib_smear_16);

    ATH_CHECK (ASG_MAKE_ANA_TOOL (m_calib_smear_17, CP::MuonCalibrationAndSmearingTool));
    ATH_CHECK (m_calib_smear_17.setProperty ("Year", "Data17"));
    ATH_CHECK (m_calib_smear_17.setProperty ("SagittaRelease", "sagittaBiasDataAll_30_07_18"));
    ATH_CHECK (m_calib_smear_17.setProperty ("SagittaCorr", true));
    ATH_CHECK (m_calib_smear_17.initialize());
    registerTool (&*m_calib_smear_17);

    registerCut (SelectionStep::MET, "calib_tool", cut_calib_tool);

    return StatusCode::SUCCESS;
  }



  StatusCode MuonToolCorrect ::
  correctObject (xAOD::Muon& muon)
  {
    ATH_MSG_DEBUG("correctObject");
    const xAOD::EventInfo* eventInfo = 0;
    ATH_CHECK (evtStore()->retrieve( eventInfo, "EventInfo"));

    unsigned int run = 0;
    if ( eventInfo->eventType( xAOD::EventInfo::IS_SIMULATION ) ) 
       run = eventInfo->auxdata<unsigned int>( "RandomRunNumber" );
    else 
       run = eventInfo->runNumber();

    // Apply the CP calibration
    if(run>320000) 
      QA_CHECK_CUT (cut_calib_tool, m_calib_smear_17->applyCorrection (muon));
    
    if(run<320000)
      QA_CHECK_CUT (cut_calib_tool, m_calib_smear_16->applyCorrection (muon));

    return StatusCode::SUCCESS;
  }



  MuonToolSelect ::
  MuonToolSelect (const std::string& name)
    : AsgTool (name), AnaToolSelect<xAOD::MuonContainer> (name),
      m_quality (xAOD::Muon::Medium),
      m_selection ("MuonSelectionTool", this),
      m_isolationTool ("IsolationSelectionTool", this)
  {
    // declareProperty("Quality", m_quality);

    declareProperty( "IsolationOn", m_isolationOn = true );
    declareProperty( "IsolationWP", m_isolationWP = "Undefined", "Working point to use" );
    // The interface for adding "UserDefined" working points has changed
    // -- for now supporting single standard working points only;
    //    will add other options when needed
    declareProperty( "InclusiveMET", m_inclusiveMET = false );
    declareProperty( "LooseImpactCut", m_looseImpactCut = false );
    declareProperty( "HighPt", m_high_pT = -1. );
    declareProperty( "NoTRT" , m_noTRT=false , "Apply the TRT cut in selection?");
    declareProperty( "BadMuVeto", m_doBadMuVeto=false , "Apply the bad muon veto?");
  }



  StatusCode MuonToolSelect ::
  initialize()
  {
    ATH_MSG_DEBUG("initialize");
    ATH_CHECK (ASG_MAKE_ANA_TOOL (m_selection, CP::MuonSelectionTool));
    ATH_CHECK (m_selection.setProperty ("MuQuality", int (m_quality)));
    // In case we do not require TRT cuts in the derivation, this helps with PU stability
    if (m_noTRT){
      ATH_CHECK (m_selection.setProperty ("TrtCutOff", true) );
    }
    ATH_CHECK (m_selection.initialize());
    registerTool(&*m_selection);

    // high-pT working point
    ATH_CHECK (ASG_MAKE_ANA_TOOL (m_selectionHPT, CP::MuonSelectionTool));
    ATH_CHECK (m_selectionHPT.setProperty( "MaxEta", 2.5 ) );
    ATH_CHECK (m_selectionHPT.setProperty(  "MuQuality", 4 ) );
    if (m_noTRT){
      ATH_CHECK (m_selectionHPT.setProperty ("TrtCutOff", true) );
    }
    ATH_CHECK (m_selectionHPT.initialize() );
    // Only add systematics if we are using this thing to pick muons
    if (m_high_pT>0.) registerTool(&*m_selectionHPT);

    // Initialize the isolation selection tool
    if (m_isolationOn)
    {
      ATH_CHECK (ASG_MAKE_ANA_TOOL (m_isolationTool, CP::IsolationSelectionTool));
      ATH_CHECK (m_isolationTool.setProperty("MuonWP", m_isolationWP));
      ATH_CHECK (m_isolationTool.initialize());
      registerTool(&*m_isolationTool);
    }

    // Note: for now, use isolated objects (if isolation is applied) to build MET

    registerCut (SelectionStep::MET, "selection", cut_selection);

    if (!m_inclusiveMET)
    {
      registerCut (SelectionStep::MET, "d0", cut_d0);
      registerCut (SelectionStep::MET, "z0", cut_z0);

      registerCut (SelectionStep::MET, "isolationTool", cut_isolationTool);
    }
    else
    {
      registerCut (SelectionStep::ANALYSIS, "d0", cut_d0);
      registerCut (SelectionStep::ANALYSIS, "z0", cut_z0);

      registerCut (SelectionStep::ANALYSIS, "isolationTool", cut_isolationTool);
    }
    if (m_high_pT>0.) registerCut (SelectionStep::MET, "HighPt", cut_high_pT);

    return StatusCode::SUCCESS;
  }



  StatusCode MuonToolSelect ::
  selectObject (xAOD::Muon& muon)
  {
    ATH_MSG_DEBUG("selectObject");
    cut_selection.setPassedIf (m_selection->accept (muon));
    if (m_high_pT>0. && muon.pt()>m_high_pT){
      cut_selection.setPassedIf (m_selectionHPT->accept (muon));
    } else {
      // Decorate for analyses to play
      muon.auxdata<char>("high_pT") = m_selectionHPT->accept(muon);
    }

    if (m_doBadMuVeto){
      cut_selection.setPassedIf (m_selection->accept (muon) && !m_selection->isBadMuon (muon));
      if (m_high_pT>0. && muon.pt()>m_high_pT){
        cut_selection.setPassedIf (m_selectionHPT->accept (muon) && !m_selectionHPT->isBadMuon (muon) && !m_selection->isBadMuon (muon));
      }
    }

    if (muon.muonType() != xAOD::Muon::MuonStandAlone)
    {
      // Z0 and D0 requirements
      const xAOD::TrackParticle *mutrk = muon.primaryTrackParticle();

      // obtain primary vertex
      const xAOD::Vertex *pv(0);
      const xAOD::VertexContainer *vertexContainer(0);
      ATH_CHECK( evtStore()->retrieve(vertexContainer, "PrimaryVertices" ) );
      for ( const auto* const vtx_itr : *vertexContainer )
      {
        if (vtx_itr->vertexType() != xAOD::VxType::VertexType::PriVtx) continue;
        else { pv = vtx_itr; break;}
      }

      const xAOD::EventInfo* evt = 0;
      ATH_CHECK( evtStore()->retrieve( evt, "EventInfo" ) );
      double mu_d0sig = -999;
      if(mutrk && evt)
        mu_d0sig = xAOD::TrackingHelpers::d0significance
          ( mutrk, evt->beamPosSigmaX(), evt->beamPosSigmaY(), evt->beamPosSigmaXY() );

      muon.auxdata<double>("d0Sig") = mutrk && pv ? mu_d0sig : -999;
      muon.auxdata<double>("z0sintheta") = mutrk && pv ? (mutrk->z0() + mutrk->vz() - pv->z() ) * sin(mutrk->theta()) : -999;

      if (mutrk && pv)
      {
        // Cuts recommended by the tracking CP group
        if(m_looseImpactCut){
          // looser cuts for fakes est
          cut_d0.setPassedIf( 1 );
          cut_z0.setPassedIf( 1 );
          //muon.auxdata<double>("d0Sig") = mu_d0sig;
          //muon.auxdata<double>("z0sintheta") = (mutrk->z0() + mutrk->vz() - pv->z() ) * sin(mutrk->theta());
        }else{
          // Cuts recommended by the tracking CP group
          cut_d0.setPassedIf( fabs(mu_d0sig) < 3. );
          cut_z0.setPassedIf( fabs((mutrk->z0() + mutrk->vz() - pv->z() ) * sin(mutrk->theta())) < 0.5 );
        }
      }
      else
      {
        cut_d0.setPassedIf(false);
        cut_z0.setPassedIf(false);
      }

      // Extra is bad decoration based on SUSYTools muon isBad function
      if (mutrk){
        float Rerr = Amg::error(mutrk->definingParametersCovMatrix(), 4) / fabs(mutrk->qOverP());
        muon.auxdecor<char>("badQoverPErr") = Rerr>0.2;
      }
    }
    else
    {
      cut_d0.setPassedIf(true);
      cut_z0.setPassedIf(true);
    }

    // Apply isolation tool
    if (m_isolationOn)
    {
      cut_isolationTool.setPassedIf (m_isolationTool->accept(muon));
    }

    // MCP recommendation to check bad muons
    muon.auxdecor<char>("badMuon") = m_selection->isBadMuon(muon);
    if (m_high_pT>0. && muon.pt()>m_high_pT){
      muon.auxdecor<char>("badMuon") = m_selectionHPT->isBadMuon(muon);
    }

    return StatusCode::SUCCESS;
  }



  MuonToolWeight ::
  MuonToolWeight (const std::string& name)
    : AsgTool (name), AnaToolWeight<xAOD::MuonContainer> (name),
      m_efficiency_scale("MuonEfficiencyScaleFactors", this),
      m_efficiency_scaleHPT("MuonEfficiencyScaleFactorsHPT", this),
      m_ttva_efficiency_scale("MuonTTVAEfficiencyScaleFactors", this),
      m_isolation_scale("MuonIsolationScaleFactors", this),
      m_badmuonveto_scale("BadMuonVetoScaleFactors", this)
  {
    declareProperty("IsolationWP", m_isolationWP = "", "Working point to use");
    declareProperty("HighPt", m_high_pT = -1. );
    declareProperty("NoTRT" , m_noTRT=false , "Apply the TRT cut in selection?");
    declareProperty("BadMuVeto", m_doBadMuVeto=false , "Apply the bad muon veto?");
  }



  StatusCode MuonToolWeight ::
  initialize()
  {
    ATH_MSG_DEBUG("initialize");

    ATH_CHECK( ASG_MAKE_ANA_TOOL(m_efficiency_scale, CP::MuonEfficiencyScaleFactors) );

    // Set working point
    std::string wpString = "";
    if      (m_quality==xAOD::Muon::Loose)  wpString = "Loose";
    else if (m_quality==xAOD::Muon::Medium) wpString = "Medium";
    else if (m_quality==xAOD::Muon::Tight)  wpString = "Tight";
    ATH_CHECK( m_efficiency_scale.setProperty("WorkingPoint", wpString) );
    // For the case of no TRT cut, we need a different calibration release
    if (m_noTRT){
      ATH_CHECK( m_efficiency_scale.setProperty("CalibrationRelease", "170209_Moriond_noTRT") );
    }
    ATH_CHECK( m_efficiency_scale.initialize() );
    registerTool( &*m_efficiency_scale );

    // Set working point for the HPT tool
    if (m_high_pT>0.){
      ATH_CHECK( ASG_MAKE_ANA_TOOL(m_efficiency_scaleHPT, CP::MuonEfficiencyScaleFactors) );
      ATH_CHECK( m_efficiency_scaleHPT.setProperty("WorkingPoint", "HighPt") );
      // For the case of no TRT cut, we need a different calibration release
      if (m_noTRT){
        ATH_CHECK( m_efficiency_scaleHPT.setProperty("CalibrationRelease", "170209_Moriond_noTRT") );
      }
      ATH_CHECK( m_efficiency_scaleHPT.initialize() );
      registerTool( &*m_efficiency_scaleHPT );
    }

    ATH_CHECK( ASG_MAKE_ANA_TOOL(m_ttva_efficiency_scale, CP::MuonEfficiencyScaleFactors) );
    ATH_CHECK( m_ttva_efficiency_scale.setProperty("WorkingPoint", "TTVA") );
    if (m_noTRT)
      ATH_CHECK( m_ttva_efficiency_scale.setProperty("CalibrationRelease", "170303_Moriond_noTRT") );
    ATH_CHECK( m_ttva_efficiency_scale.initialize() );
    registerTool( &*m_ttva_efficiency_scale );

    // Isolation working points
    if (m_isolationWP!="") {
      ATH_CHECK( ASG_MAKE_ANA_TOOL(m_isolation_scale, CP::MuonEfficiencyScaleFactors) );
      ATH_CHECK( m_isolation_scale.setProperty("WorkingPoint", m_isolationWP+"Iso") );
      ATH_CHECK( m_isolation_scale.initialize() );
      registerTool( &*m_isolation_scale );
    }

    if (m_doBadMuVeto){
      ATH_CHECK( ASG_MAKE_ANA_TOOL(m_badmuonveto_scale, CP::MuonEfficiencyScaleFactors) );
      ATH_CHECK( m_badmuonveto_scale.setProperty("WorkingPoint", "BadMuonVeto_HighPt") );
      ATH_CHECK( m_badmuonveto_scale.initialize() );
      registerTool( &*m_badmuonveto_scale );
    }

    return StatusCode::SUCCESS;
  }



  StatusCode MuonToolWeight ::
  objectWeight (const xAOD::Muon& muon, float& weight)
  {
    ATH_MSG_DEBUG("objectWeight");
    weight = 1.;
    float reco_weight=1., ttva_weight=1., iso_weight = 1.;

    // Reconstruction efficiency scale factor
    if (m_high_pT<0. || muon.pt()<=m_high_pT){
      if (m_efficiency_scale->getEfficiencyScaleFactor(muon, reco_weight) ==
          CP::CorrectionCode::Ok)
        weight *= reco_weight;
    } else {
      if (m_efficiency_scaleHPT->getEfficiencyScaleFactor(muon, reco_weight) ==
          CP::CorrectionCode::Ok)
        weight *= reco_weight;
    }

    // These next two do not depend on any high-pT working point issues
    // Track to vertex association efficiency scale factor
    if (m_ttva_efficiency_scale->getEfficiencyScaleFactor(muon, ttva_weight) ==
        CP::CorrectionCode::Ok)
      weight *= ttva_weight;

    // Isolation efficiency scale factor
    if (m_isolation_scale.isInitialized() &&
        m_isolation_scale->getEfficiencyScaleFactor(muon, iso_weight) ==
        CP::CorrectionCode::Ok)
      weight *= iso_weight;

    // Bad muon veto scale factor
    // Buyer beware -- this only applies to the high pT working point
    // Right now we can't select the high pT working point on its own
    if (m_doBadMuVeto &&
        (m_high_pT>0. && muon.pt()>m_high_pT) ){
      float badMu_weight = 1.;
      if (m_badmuonveto_scale->getEfficiencyScaleFactor(muon, badMu_weight) ==
          CP::CorrectionCode::Ok)
        weight *= badMu_weight;
    }

    return StatusCode::SUCCESS;
  }



  StatusCode makeMuonTool (DefinitionArgs& args,
                           const xAOD::Muon::Quality& quality,
                           const bool& isolationOn,
                           const std::string& isolationWP,
                           const bool& inclusiveMET,
                           const bool& looseImpactCut,
                           const float& high_pT,
                           const bool noTRT,
                           const bool badMuVeto)
  {
    using namespace msgObjectDefinition;

    // Global configuration
    if (args.firstWP())
    {
      switch (quality)
      {
      case xAOD::Muon::Tight:
        args.configuration()->setMuonWP ("Tight");
        break;
      case xAOD::Muon::Medium:
        args.configuration()->setMuonWP ("Medium");
        break;
      case xAOD::Muon::Loose:
        args.configuration()->setMuonWP ("Loose");
        break;
      case xAOD::Muon::VeryLoose:
        args.configuration()->setMuonWP ("VeryLoose");
        break;
      }
    }

    if (isolationOn) args.configuration()->setMuonIsolationWP (isolationWP);

    // Retrieval tool
    std::unique_ptr<IAnaTool> retrieveTool
      (new AnaToolRetrieve (args.prefix() + "_retrieve", "Muons"));
    args.add (std::move (retrieveTool));

    // Correction tool
    std::unique_ptr<IAnaTool> correctTool
      (new MuonToolCorrect (args.prefix() + "_correct"));
    args.add (std::move (correctTool));

    // Selection tool
    std::unique_ptr<MuonToolSelect> selectTool
      (new MuonToolSelect (args.prefix() + "_select"));
    // TODO: Change back to using properties!
    // ANA_CHECK (selectTool->setProperty("Quality", quality));
    selectTool->m_quality = quality;
    ANA_CHECK( selectTool->setProperty("IsolationOn", isolationOn) );
    ANA_CHECK( selectTool->setProperty("IsolationWP", isolationWP) );
    ANA_CHECK( selectTool->setProperty("InclusiveMET", inclusiveMET) );
    ANA_CHECK( selectTool->setProperty("LooseImpactCut", looseImpactCut) );
    ANA_CHECK( selectTool->setProperty("HighPt", high_pT) );
    ANA_CHECK( selectTool->setProperty("NoTRT", noTRT) );
    ANA_CHECK( selectTool->setProperty("BadMuVeto", badMuVeto) );
    args.add( std::move(selectTool) );

    if (args.configuration()->isData() == false)
    {
      // Weight tool
      std::unique_ptr<MuonToolWeight> weightTool
	(new MuonToolWeight (args.prefix() + "_weight"));
      // TODO: Change back to using properties!
      weightTool->m_quality = quality;

      ANA_CHECK( weightTool->setProperty("IsolationWP", isolationWP) );
      ANA_CHECK( weightTool->setProperty("HighPt", high_pT) );
      ANA_CHECK( weightTool->setProperty("NoTRT", noTRT) );
      ANA_CHECK( weightTool->setProperty("BadMuVeto", badMuVeto) );
      args.add( std::move(weightTool) );
    }

    return StatusCode::SUCCESS;
  }

  // Macros expand to wrapper functions for making the tools

  QUICK_ANA_MUON_DEFINITION_MAKER
    ("tight", makeMuonTool (args, xAOD::Muon::Tight))
  QUICK_ANA_MUON_DEFINITION_MAKER
    ("medium", makeMuonTool (args, xAOD::Muon::Medium))
  QUICK_ANA_MUON_DEFINITION_MAKER
    ("loose", makeMuonTool (args, xAOD::Muon::Loose))
  QUICK_ANA_MUON_DEFINITION_MAKER
    ("very_loose", makeMuonTool (args, xAOD::Muon::VeryLoose))

  QUICK_ANA_MUON_DEFINITION_MAKER
    ("tightIso", makeMuonTool (args, xAOD::Muon::Tight, true))
  QUICK_ANA_MUON_DEFINITION_MAKER
    ("mediumIso", makeMuonTool (args, xAOD::Muon::Medium, true))
  QUICK_ANA_MUON_DEFINITION_MAKER
    ("looseIso", makeMuonTool (args, xAOD::Muon::Loose, true))
  QUICK_ANA_MUON_DEFINITION_MAKER
    ("very_looseIso", makeMuonTool (args, xAOD::Muon::VeryLoose, true))

  QUICK_ANA_MUON_DEFINITION_MAKER
    ("loose_Isoloose", makeMuonTool (args, xAOD::Muon::Loose, true, "Loose"))

  QUICK_ANA_MUON_DEFINITION_MAKER
    ("Zprime", makeMuonTool (args, xAOD::Muon::Medium, true, "LooseTrackOnly"))

  QUICK_ANA_MUON_DEFINITION_MAKER
    ("SUSYIso", makeMuonTool (args, xAOD::Muon::Medium, true, "GradientLoose", true, false, -1., true))
  QUICK_ANA_MUON_DEFINITION_MAKER
    ("SUSYloose", makeMuonTool (args, xAOD::Muon::Medium, false,"",true, true, -1., true))
  QUICK_ANA_MUON_DEFINITION_MAKER
    ("MediumGLIso500BMV", makeMuonTool (args, xAOD::Muon::Medium, true, "GradientLoose", true, false, 500.*GeV, true, true))


}
