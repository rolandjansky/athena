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


// An overview of MC15 recommendations from the following talk and note:
//   https://indico.cern.ch/event/330375/session/5/contribution/25/material/slides/0.pdf
//   https://cds.cern.ch/record/2013489/files/ATL-COM-PHYS-2015-347.pdf
//     Note the difference between track-based soft term and track-based MET.
//     (The latter uses tracks instead of jets for building MET.)
// Technical details and some examples from:
//   https://twiki.cern.ch/twiki/bin/viewauth/AtlasProtected/METUtilities
//
// Current recommendations described on these pages:
//   https://twiki.cern.ch/twiki/bin/view/AtlasProtected/JetEtmissRecommendationsMC15
//   https://twiki.cern.ch/twiki/bin/view/AtlasProtected/EtmissSubgroup#Current_recommendations
// Moriod 2016 recommendations:
//   https://twiki.cern.ch/twiki/bin/view/AtlasProtected/EtmissRecommendationsMoriond2016

//
// includes
//

#include <QuickAna/MetTool.h>

#include <QuickAna/DefinitionArgs.h>
#include <QuickAna/DefinitionMaker.h>
#include <QuickAna/IEventObjects.h>
#include <QuickAna/InternalConfiguration.h>
#include <QuickAna/MessageCheck.h>
#include <RootCoreUtils/Assert.h>

#include <METUtilities/METMaker.h>
#include <METInterface/IMETSystematicsTool.h>
#ifdef ROOTCORE
#include <METUtilities/METSystematicsTool.h>
#endif
#include <METUtilities/METHelpers.h>

// For the fJVT working point
#include <JetMomentTools/JetForwardJvtTool.h>

static const float GeV = 1000.;

//
// method implementations
//

namespace ana
{
  MetTool ::
  MetTool (const std::string& name)
    : AsgTool (name), AnaTool (name),
      m_doFJVT(false),
      m_metutil ("maker", this),
      m_metSystTool ("systTool", this),
      m_fjvtTool ("fjvtTool", this),
      m_accessor ("dummy")
  {
    // Note: these are only used with METMaker
    declareProperty("IncludeTauTerm", m_includeTauTerm=true);
    declareProperty("DoTST", m_doTST=true);
    declareProperty("DoJVTCut", m_doJVTCut=true);
    declareProperty("DoTrackMet", m_doTrackMET=false);
    declareProperty("DoSetMuonJetEMScale", m_doMuJetEMScale=false);
    declareProperty("JetSelection", m_jetSelection="Tight");
    declareProperty("ORCaloTaggedMuons", m_doORCaloTaggedMuons=true);
    declareProperty("JetUniqueFrac", m_uniqueFrac=-1.);
    declareProperty("JetUniqueCut", m_jetCut=-1.);
  }



  StatusCode MetTool ::
  setObjectType (ObjectType type, const std::string& workingPoint)
  {
    if (type != OBJECT_MET && type != OBJECT_MET2)
    {
      ATH_MSG_ERROR ("unsupported object type");
      return StatusCode::FAILURE;
    }
    m_type = type;

    if (!workingPoint.empty())
    {
      ATH_MSG_ERROR ("can't have working point for met tool");
      return StatusCode::FAILURE;
    }
    return StatusCode::SUCCESS;
  }



  AnalysisStep MetTool ::
  step () const
  {
    return STEP_MET;
  }



  unsigned MetTool ::
  inputTypes () const
  {
    return (1 << OBJECT_ELECTRON) | (1 << OBJECT_PHOTON) | (1 << OBJECT_TAU) |
           (1 << OBJECT_MUON) | (1 << OBJECT_JET);
  }



  unsigned MetTool ::
  outputTypes () const
  {
    return 1 << m_type;
  }



  StatusCode MetTool ::
  useInitialConfiguration (const InternalConfiguration& conf)
  {
    ATH_CHECK (AnaTool::useInitialConfiguration (conf));

    m_isData = conf.isData();
    m_isAF2  = conf.isAFII();
    m_jetContainer = conf.inputName (OBJECT_JET);
    if (m_jetContainer.empty())
    {
      ATH_MSG_ERROR ("can't use MET without jets");
      return StatusCode::FAILURE;
    }
    return StatusCode::SUCCESS;
  }



  StatusCode MetTool ::
  initialize()
  {
    // Initialize the MET maker. Config from:
    // https://twiki.cern.ch/twiki/bin/view/AtlasProtected/EtmissRecommendationsMoriond2016
    ATH_CHECK( ASG_MAKE_ANA_TOOL(m_metutil, met::METMaker) );
    ATH_CHECK( m_metutil.setProperty("ORCaloTaggedMuons", m_doORCaloTaggedMuons) );
    ATH_CHECK( m_metutil.setProperty("DoSetMuonJetEMScale", m_doMuJetEMScale) );

    if (m_jetSelection!="passFJVT"){
      ATH_CHECK( m_metutil.setProperty("JetSelection", m_jetSelection) );
    } else {
      // Special forward JVT working point - requires the forward JVT tool to be enabled!
      m_doFJVT = true;
      ATH_CHECK( m_metutil.setProperty("JetRejectionDec", m_jetSelection) );
      ATH_CHECK( ASG_MAKE_ANA_TOOL( m_fjvtTool, JetForwardJvtTool) );
      ATH_CHECK( m_fjvtTool.setProperty("CentralMaxPt",60e3) );
    }

    if (m_uniqueFrac>=0.) ATH_CHECK( m_metutil.setProperty("JetMinEFrac", m_uniqueFrac) );
    if (m_jetCut>=0.)     ATH_CHECK( m_metutil.setProperty("JetMinWeightedPt", m_jetCut) );
    if (!m_doTST)
    {
      ATH_CHECK( m_metutil.setProperty("DoIsolMuonEloss",true) );
      ATH_CHECK( m_metutil.setProperty("DoMuonEloss",true) );
      if (m_doJVTCut)
      {
        ATH_MSG_WARNING("Running with CST, but JVT cut was set to true.  Will change JVT setting; please check your configuration.");
        m_doJVTCut = false;
      }
      if ("AntiKt4EMTopoJets"==m_jetContainer)
      {
        // Recommendation from TJ: if we are using EM topo jets, make sure the clusters are considered at LC scale
        ATH_CHECK( m_metutil.setProperty("JetConstitScaleMom","JetLCScaleMomentum") );
      }
    }
    ATH_CHECK( m_metutil.initialize() );

    if (m_isData == false)
    {
      ATH_CHECK( ASG_MAKE_ANA_TOOL(m_metSystTool, met::METSystematicsTool) );
      RCU_ASSERT( !m_jetContainer.empty() );

      // Empty if we are using the calo soft term
      if (!m_doTST) ATH_CHECK( m_metSystTool.setProperty("ConfigSoftTrkFile","") );
      // No calo soft term systematics recommendations, so an empty string for now
      if (m_doTST) ATH_CHECK( m_metSystTool.setProperty("ConfigSoftCaloFile", "") );
      // not sure what this should be, commenting it out for now.
      // if (m_doTrackMET) ATH_CHECK( m_metSystTool.setProperty("ConfigJetTrkFile","JetTrackSyst.config") );
      // No other non-default settings
      ATH_CHECK( m_metSystTool.initialize() );
      registerTool(&*m_metSystTool);
    }

    return StatusCode::SUCCESS;
  }



  StatusCode MetTool ::
  useConfiguration (const InternalConfiguration& configuration)
  {
    ATH_CHECK (AnaTool::useConfiguration (configuration));
    m_accessor = configuration.selectionName (SelectionStep::MET);
    return StatusCode::SUCCESS;
  }



  StatusCode MetTool ::
  execute (IEventObjects& objects)
  {
    // Create a new, empty MET container to be filled by the METRebuilder
    ATH_CHECK (objects.addNew (m_type));

    auto met = objects.get<xAOD::MissingETContainer> (m_type);

    // Retrieve the container of object weights. These were filled during
    // reconstruction and will be used to recalculate the MET with our
    // calibrated objects.
    const xAOD::MissingETContainer* metcore(0);
    std::string coreContainer = "MET_Core_" + m_jetContainer;
    if (coreContainer.rfind ("Jets") == coreContainer.size()-4)
      coreContainer = coreContainer.substr (0, coreContainer.size()-4);

    ANA_CHECK( evtStore()->retrieve(metcore, coreContainer) );

    const xAOD::MissingETAssociationMap* metMap = 0;
    std::string assocContainer = "METAssoc_" + m_jetContainer;
    if (assocContainer.rfind ("Jets") == assocContainer.size()-4)
      assocContainer = assocContainer.substr (0, assocContainer.size()-4);
    ANA_CHECK (evtStore()->retrieve(metMap, assocContainer));

    metMap->resetObjSelectionFlags();

    // Setup ghost association for mu-jet OR
    //if(m_doMuJetOR || m_doMuJetEMScale) {
    if(m_doORCaloTaggedMuons || m_doMuJetEMScale) {
      if(!objects.muons() || !objects.jets()) {
        ATH_MSG_ERROR("Configured mu-jet OR for MET but container(s) NULL!");
        return StatusCode::FAILURE;
      }
      met::addGhostMuonsToJets( *objects.muons(), *objects.jets() );
    }

    // TODO: remove hardcoded options and strings

    std::string softTerm = "SoftClus";
    if (m_doTST)
    {
      softTerm = "PVSoftTrk";
    }

    // Rebuild electron term
    if (objects.electrons())
    {
      xAOD::ElectronContainer metelectrons(SG::VIEW_ELEMENTS);
      for (auto el : *objects.electrons())
      {
        if (m_accessor (*el))
        {
          metelectrons.push_back(el);
        }
      }

      ATH_CHECK (m_metutil->rebuildMET("RefEle", xAOD::Type::Electron, met,
                                       &metelectrons, metMap));
    }

    // Rebuild photon term
    if (objects.photons())
    {
      xAOD::PhotonContainer metphotons(SG::VIEW_ELEMENTS);
      for (auto ph : *objects.photons())
      {
        if (m_accessor (*ph))
        {
          metphotons.push_back(ph);
        }
      }

      ATH_CHECK (m_metutil->rebuildMET("RefGamma", xAOD::Type::Photon, met,
                                       &metphotons, metMap));
    }

    // Rebuild tau term
    if (m_includeTauTerm && objects.taus())
    {
      xAOD::TauJetContainer mettaus(SG::VIEW_ELEMENTS);
      for (auto tau : *objects.taus())
      {
        if (m_accessor (*tau))
        {
          mettaus.push_back(tau);
        }
      }

      ATH_CHECK (m_metutil->rebuildMET("RefTau", xAOD::Type::Tau, met,
                                       &mettaus, metMap));
    }

    // Rebuild muon term
    if (objects.muons())
    {
      xAOD::MuonContainer metmuons(SG::VIEW_ELEMENTS);
      for (auto mu : *objects.muons())
      {
        if (m_accessor (*mu))
        {
          metmuons.push_back(mu);
        }
      }

      ATH_CHECK (m_metutil->rebuildMET("RefMuon", xAOD::Type::Muon, met,
                                       &metmuons, metMap));
    }

    // Rebuild jet and soft term
    if (!objects.jets())
    {
      ATH_MSG_WARNING("Invalid jet container specified for MET rebuilding!");
      return StatusCode::SUCCESS;
    } else if (m_doFJVT) {
      // Make sure for forward jet working point we apply the fJVT tool
      // This always returns zero, so no need for value checking right now
      m_fjvtTool->modify( *objects.jets() );
    }

    if (m_doTrackMET)
    {
      ATH_CHECK (m_metutil->rebuildTrackMET("RefJet", softTerm, met, objects.jets(),
                                            metcore, metMap, m_doJVTCut));
    } else
    {
      ATH_CHECK (m_metutil->rebuildJetMET("RefJet", softTerm, met, objects.jets(),
                                          metcore, metMap, m_doJVTCut));
    }

    if (m_isData == false)
    {
      if ( m_metSystTool->applyCorrection( *(*met)[softTerm] ) !=
           CP::CorrectionCode::Ok )
      {
        ATH_MSG_WARNING("GetMET: Failed to apply MET soft term systematics.");
      }
    }

    ATH_CHECK( m_metutil->buildMETSum("Final", met, (*met)[softTerm]->source()) );

    return StatusCode::SUCCESS;
  }



  // Function for instantiating a MetTool
  StatusCode makeMetTool (DefinitionArgs& args,
                          const bool includeTauTerm=true,
                          const bool doTST=true,
                          const bool doJVTCut=true,
                          const bool doTrackMet=false,
                          const bool doORCaloTaggedMuons=true,
                          const bool doMuJetEMScale=false,
                          const std::string& jetSelection="Tight",
                          const double uniqueFrac=-1.,
                          const double jetCut=-1.)
  {
    using namespace msgObjectDefinition;

    std::unique_ptr<MetTool> metTool
      (new MetTool (args.prefix()));
    ANA_CHECK( metTool->setProperty("IncludeTauTerm", includeTauTerm) );
    ANA_CHECK( metTool->setProperty("DoTST", doTST) );
    ANA_CHECK( metTool->setProperty("DoJVTCut", doJVTCut) );
    ANA_CHECK( metTool->setProperty("DoTrackMet", doTrackMet) );
    ANA_CHECK( metTool->setProperty("ORCaloTaggedMuons", doORCaloTaggedMuons) );
    ANA_CHECK( metTool->setProperty("DoSetMuonJetEMScale", doMuJetEMScale) );
    ANA_CHECK( metTool->setProperty("JetSelection", jetSelection) );
    ANA_CHECK( metTool->setProperty("JetUniqueFrac", uniqueFrac) );
    ANA_CHECK( metTool->setProperty("JetUniqueCut", jetCut) );
    args.add ( std::move(metTool) );

    return StatusCode::SUCCESS;
  }

  // Macro for creating a MetTool using the provided function
  QUICK_ANA_MET_DEFINITION_MAKER( "default",   makeMetTool(args) )
  QUICK_ANA_MET_DEFINITION_MAKER( "noTauTerm", makeMetTool(args,false) )
  QUICK_ANA_MET_DEFINITION_MAKER( "trackmet",  makeMetTool(args,true,true,true,true) )
  QUICK_ANA_MET_DEFINITION_MAKER( "susy2L",    makeMetTool(args,true,true,true,false,true,true) )
  QUICK_ANA_MET_DEFINITION_MAKER( "metZHinv",  makeMetTool(args,true,true,true,false,true,true,"Loose") )
  QUICK_ANA_MET_DEFINITION_MAKER( "noTauCST",  makeMetTool(args,false,false) )
  QUICK_ANA_MET_DEFINITION_MAKER( "CST",       makeMetTool(args,true,false,false) )

  QUICK_ANA_MET_DEFINITION_MAKER( "Tight",    makeMetTool(args,true,true,true,false,true,false,"Tight") )
  QUICK_ANA_MET_DEFINITION_MAKER( "passFJVT",    makeMetTool(args,true,true,true,false,true,false,"passFJVT") )

}
