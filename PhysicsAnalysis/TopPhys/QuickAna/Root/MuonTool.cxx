/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//        Copyright Iowa State University 2014.
//                  Author: Nils Krumnack
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

// Please feel free to contact me (nils.erik.krumnack@cern.ch) for bug
// reports, feature suggestions, praise and complaints.


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
      m_calib_smear ("MuonCalibrationAndSmearingTool", this)
  {
  }



  StatusCode MuonToolCorrect ::
  initialize()
  {
    ATH_MSG_DEBUG("initialize");
    ATH_CHECK (ASG_MAKE_ANA_TOOL (m_calib_smear, CP::MuonCalibrationAndSmearingTool));
    // Take the default properties
    ATH_CHECK (m_calib_smear.initialize());
    registerTool (&*m_calib_smear);

    registerCut (SelectionStep::MET, "calib_tool", cut_calib_tool);

    return StatusCode::SUCCESS;
  }



  StatusCode MuonToolCorrect ::
  correctObject (xAOD::Muon& muon)
  {
    ATH_MSG_DEBUG("correctObject");
    // Apply the CP calibration
    QA_CHECK_CUT (cut_calib_tool, m_calib_smear->applyCorrection (muon));

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
  }



  StatusCode MuonToolSelect ::
  initialize()
  {
    ATH_MSG_DEBUG("initialize");
    ATH_CHECK (ASG_MAKE_ANA_TOOL (m_selection, CP::MuonSelectionTool));
    ATH_CHECK (m_selection.setProperty ("MuQuality", int (m_quality)));
    ATH_CHECK (m_selection.initialize());

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
      cut_selection.setPassedIf (muon.passesHighPtCuts());
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

    return StatusCode::SUCCESS;
  }



  MuonToolWeight ::
  MuonToolWeight (const std::string& name)
    : AsgTool (name), AnaToolWeight<xAOD::MuonContainer> (name),
      m_efficiency_scale("MuonEfficiencyScaleFactors", this),
      m_ttva_efficiency_scale("MuonTTVAEfficiencyScaleFactors", this),
      m_isolation_scale("MuonIsolationScaleFactors", this)
  {
    declareProperty("IsolationWP", m_isolationWP = "", "Working point to use");
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
    ATH_CHECK( m_efficiency_scale.initialize() );
    registerTool( &*m_efficiency_scale );

    ATH_CHECK( ASG_MAKE_ANA_TOOL(m_ttva_efficiency_scale, CP::MuonEfficiencyScaleFactors) );
    ATH_CHECK( m_ttva_efficiency_scale.setProperty("WorkingPoint", "TTVA") );
    ATH_CHECK( m_ttva_efficiency_scale.initialize() );
    registerTool( &*m_ttva_efficiency_scale );

    // Isolation working points
    if (m_isolationWP!="") {
      ATH_CHECK( ASG_MAKE_ANA_TOOL(m_isolation_scale, CP::MuonEfficiencyScaleFactors) );
      ATH_CHECK( m_isolation_scale.setProperty("WorkingPoint", m_isolationWP+"Iso") );
      ATH_CHECK( m_isolation_scale.initialize() );
      registerTool( &*m_isolation_scale );
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
    if (m_efficiency_scale->getEfficiencyScaleFactor(muon, reco_weight) ==
        CP::CorrectionCode::Ok)
      weight *= reco_weight;

    // Track to vertex association efficiency scale factor
    if (m_ttva_efficiency_scale->getEfficiencyScaleFactor(muon, ttva_weight) ==
        CP::CorrectionCode::Ok)
      weight *= ttva_weight;

    // Isolation efficiency scale factor
    if (m_isolation_scale.isInitialized() &&
        m_isolation_scale->getEfficiencyScaleFactor(muon, iso_weight) ==
        CP::CorrectionCode::Ok)
      weight *= iso_weight;

    return StatusCode::SUCCESS;
  }



  StatusCode makeMuonTool (DefinitionArgs& args,
                           const xAOD::Muon::Quality& quality,
                           const bool& isolationOn,
                           const std::string& isolationWP,
                           const bool& inclusiveMET,
                           const bool& looseImpactCut,
                           const float& high_pT)
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
    args.add( std::move(selectTool) );

    if (args.configuration()->isData() == false)
    {
      // Weight tool
      std::unique_ptr<MuonToolWeight> weightTool
	(new MuonToolWeight (args.prefix() + "_weight"));
      // TODO: Change back to using properties!
      weightTool->m_quality = quality;

      ANA_CHECK( weightTool->setProperty("IsolationWP", isolationWP) );
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
    ("SUSYIso", makeMuonTool (args, xAOD::Muon::Medium, true, "GradientLoose", true))
  QUICK_ANA_MUON_DEFINITION_MAKER
    ("SUSYloose", makeMuonTool (args, xAOD::Muon::Medium, false,"",true, true))
  QUICK_ANA_MUON_DEFINITION_MAKER
    ("SUSYIsoHighPt", makeMuonTool (args, xAOD::Muon::Medium, true, "GradientLoose", true, false, 300.*GeV))


}
