/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//
// includes
//

#include <QuickAna/TopElectronTool.h>

#include <ElectronPhotonSelectorTools/AsgElectronLikelihoodTool.h>
#include <QuickAna/AnaToolRetrieve.h>
#include <QuickAna/DefinitionArgs.h>
#include <QuickAna/DefinitionMaker.h>
#include <QuickAna/ElectronTool.h>
#include <QuickAna/InternalConfiguration.h>
#include <QuickAna/IsolationTools.h>
#include <QuickAna/MessageCheck.h>

//
// method implementations
//

namespace ana
{
  //--------------------------------------------------------------------------
  // TopElectronToolPreCorrect
  //--------------------------------------------------------------------------

  TopElectronToolPreCorrect ::
  TopElectronToolPreCorrect (const std::string& name)
    : AsgTool(name), AnaToolCorrect<xAOD::ElectronContainer>(name)
  {
  }


  AnalysisStep TopElectronToolPreCorrect ::
  step () const
  {
    return STEP_PRECORRECT;
  }


  StatusCode TopElectronToolPreCorrect ::
  initialize()
  {
    return StatusCode::SUCCESS;
  }


  StatusCode TopElectronToolPreCorrect ::
  correctObject (xAOD::Electron& electron)
  {
    // "miniIso" (approximation)

    const xAOD::TrackParticleContainer* tp(0);
    ATH_CHECK(evtStore()->retrieve(tp, "InDetTrackParticles") );

    double iso(0.);
    if( // !m_isPrimaryxAOD &&
        electron.isAvailable<float>("ptvarcone40") )
    {
      iso = electron.auxdataConst<float>("ptvarcone40");
    }
    else {
      if (tp && electron.pt() > 20e3) { // tracks thinned for pt <= 15 GeV in DxAODs
        for (const auto* const trkPtr : *tp)
        {
          if ( electron.nTrackParticles() >= 1 &&
               electron.trackParticleLink(0).isValid() &&
               (*electron.trackParticleLink(0)) != nullptr &&
               trkPtr->p4().DeltaR((*electron.trackParticleLink(0))->p4()) < 1e-3 )
          {
            continue;
          }
          if ( trackSelector(*trkPtr) &&
               trkPtr->p4().DeltaR(electron.p4()) < 10e3 / electron.pt() )
          {
            iso += trkPtr->pt();
          }
        }
      }
    }

    electron.auxdata<float>("miniIso") = iso;

    // z0 wrt primary vertex calculation

    const xAOD::VertexContainer *vertexContainer(0);
    ATH_CHECK( evtStore()->retrieve(vertexContainer, "PrimaryVertices" ) );

    double z0_wrt_pv(-99.);
    const xAOD::TrackParticle* eltrk = electron.trackParticle();
    const xAOD::Vertex *pv(0);
    for ( const auto* const vtx_itr : *vertexContainer )
    {
      if (vtx_itr->vertexType() != xAOD::VxType::VertexType::PriVtx) continue;
      else { pv = vtx_itr; break; }
    }
    if (pv && eltrk) z0_wrt_pv = ( eltrk->z0() + eltrk->vz() ) - pv->z();
    electron.auxdata<float>("z0_wrt_pv") = z0_wrt_pv;

    return StatusCode::SUCCESS;
  }


  bool TopElectronToolPreCorrect::trackSelector(const xAOD::TrackParticle &trk) {
    // for reference:
    // http://acode-browser2.usatlas.bnl.gov/lxr-rel19/source/atlas/InnerDetector/InDetRecTools/InDetTrackSelectorTool/src/InDetDetailedTrackSelectorTool.cxx?v=release_19_1_3
    unsigned char nb = 0;
    trk.summaryValue(nb,xAOD::numberOfBLayerHits);
    unsigned char np = 0;
    unsigned char npd = 0;
    trk.summaryValue(np,xAOD::numberOfPixelHits);
    trk.summaryValue(npd,xAOD::numberOfPixelDeadSensors);
    unsigned char ns = 0;
    unsigned char nsd = 0;
    trk.summaryValue(ns,xAOD::numberOfSCTHits);
    trk.summaryValue(nsd,xAOD::numberOfSCTDeadSensors);
    unsigned char nss = 0;
    trk.summaryValue(nss,xAOD::numberOfSCTSharedHits);

    //ATH_MSG_INFO ("pt, eta, nSi, nSiHoles, nPixHoles = " << trk.pt() << ", " << trk.eta() << ", " << ns+nsd+np+npd << ", " << nsd+npd << ", " << (int) npd);
    // for Loose in https://svnweb.cern.ch/trac/atlasoff/browser/InnerDetector/InDetRecTools/InDetTrackSelectionTool/trunk/Root/InDetTrackSelectionTool.cxx
    if (trk.pt() > 400 &&
        std::fabs(trk.eta()) < 2.5 &&
        ns+nsd+np+npd >= 7 &&
        nsd+npd <= 2 &&
        npd <= 1)
        return true;
    return false;
  }


  //--------------------------------------------------------------------------
  // TopElectronToolSelect
  //--------------------------------------------------------------------------

  TopElectronToolSelect ::
  TopElectronToolSelect(const std::string &name)
    : AsgTool(name), AnaToolSelect<xAOD::ElectronContainer>(name)
  {
    declareProperty("IDString",   m_idStr       = "LHTight" );
    declareProperty("PtMin",      m_ptMin       = 25.e3     );
    declareProperty("EtaMax",     m_etaMax      = 2.47      );
    declareProperty("VetoCrack",  m_vetoCrack   = true      );
    declareProperty("Z0Cut",      m_z0cut       = -1.       );

    // declareProperty("AsgElectronLikelihoodTool", m_likelihood = nullptr);
  }


  StatusCode TopElectronToolSelect ::
  initialize()
  {
    registerCut (SelectionStep::MET, "author", cut_author);
    registerCut (SelectionStep::MET, "pt", cut_pt);
    registerCut (SelectionStep::MET, "id", cut_id);
    registerCut (SelectionStep::MET, "etaMax", cut_etaMax);
    registerCut (SelectionStep::MET, "etaCrack", cut_etaCrack);
    registerCut (SelectionStep::MET, "z0", cut_z0);
    registerCut (SelectionStep::MET, "isolation", cut_isolation);

    return StatusCode::SUCCESS;
  }


  StatusCode TopElectronToolSelect ::
  selectObject (xAOD::Electron& electron)
  {
    /*cut_author.setPassedIf
      (electron.author() == xAOD::EgammaParameters::AuthorElectron ||
       electron.author() == xAOD::EgammaParameters::AuthorSofte);*/

    cut_pt.setPassedIf (electron.pt() > m_ptMin);

    // Apply selection tool. See comments in ElectronToolPreCorrect.

    // The naming convention in derivation framework is different
    std::string idStr_DFCommon = m_idStr.find("LH") != std::string::npos ?
                                 "DFCommonElectrons"+m_idStr :
                                 "DFCommonElectronsIsEM"+m_idStr;

    if (electron.isAvailable<SelectType>("QuickAna_"+m_idStr))
    {
      cut_id.setPassedIf (electron.auxdataConst<SelectType>("QuickAna_"+m_idStr));
    }
    else if (electron.isAvailable<int>(idStr_DFCommon))
    {
      cut_id.setPassedIf (electron.auxdataConst<int>(idStr_DFCommon));
    }
    else
    {
      cut_id.setPassedIf (electron.passSelection(m_idStr));
    }

    // WARNING: Not all electrons keep clusters in the derivation
    // But "good" electrons should always have a cluster. Therefore,
    // move the calo cluster cut after the likelihood cut
    if (electron.caloCluster())  // protect against missing calo clusters in first round DxAODs
    {
      double cluster_eta = electron.caloCluster()->etaBE(2);
      cut_etaMax.setPassedIf (std::fabs(cluster_eta) < m_etaMax);
      cut_etaCrack.setFailedIf ( m_vetoCrack &&
			     std::fabs(cluster_eta) > 1.37 &&
			     std::fabs(cluster_eta) < 1.52 );
    }

    // Requirements on longitudinal impact parameter
    if (m_z0cut>0)
    {
      float z0 = electron.auxdataConst<float>("z0_wrt_pv");
      cut_z0.setPassedIf (std::fabs(z0) < m_z0cut);
    }

    // isolation, if m_isolation != nullptr
    cut_isolation.setFailedIf (m_isolation && !m_isolation->passSelection(electron));

    return StatusCode::SUCCESS;
  }


  StatusCode makeTopElectronTool (DefinitionArgs& args,
                                  const std::string& idStr,
                                  IsolationBase* isolation,
                                  double z0cut,
                                  bool vetoCrack)
  {
    using namespace msgObjectDefinition;

    const std::string inputContainer = "Electrons";
    // Retrieval tool
    std::unique_ptr<IAnaTool> retrieveTool
      ( new AnaToolRetrieve(args.prefix() + "_retrieve", inputContainer) );
    args.add( std::move(retrieveTool) );

    // Pre Correction tool
    std::unique_ptr<IAnaTool> precorrectTool
      ( new TopElectronToolPreCorrect(args.prefix() + "_precorrect") );
    args.add( std::move(precorrectTool) );

    // Correction tool
    std::unique_ptr<IAnaTool> correctTool
      (new ElectronToolCorrect (args.prefix() + "_correct"));
    args.add (std::move (correctTool));

    // Selection tool
    std::unique_ptr<TopElectronToolSelect> selectTool
      ( new TopElectronToolSelect(args.prefix() + "_select") );
    ANA_CHECK( selectTool->setProperty("IDString", idStr) );
    selectTool->m_isolation.reset(isolation);
    ANA_CHECK( selectTool->setProperty("Z0Cut", z0cut) );
    ANA_CHECK( selectTool->setProperty("VetoCrack", vetoCrack) );

    args.add( std::move(selectTool) );

    if (args.configuration()->isData() == false)
    {
      // Weight tool
      std::unique_ptr<IAnaTool> weightTool
	( new ElectronToolWeight(args.prefix() + "_weight") );
      args.add( std::move(weightTool) );
    }

    return StatusCode::SUCCESS;
  }

  // Note: cluster_eta cut disabled in top default, as in
  // TopObjectSelectionTools-00-00-54
  //   -- Reason for that: electron cluster info missing in Round 1 DxAODs
  //   -- fixed in TopObjectSelectionTools-00-00-56

  // Define the wrapper functions with these macros
  QUICK_ANA_ELECTRON_DEFINITION_MAKER( "top_MMtight",
    makeTopElectronTool(args, "LHMedium", new StandardIsolation("Electron")) )
  // loose not exactly the Run 2 top "default". well, doesn't matter for many
  // reasons. will remove IsolationTools soon anyway
  QUICK_ANA_ELECTRON_DEFINITION_MAKER( "top_MMloose",
    makeTopElectronTool(args, "LHLoose") )
  QUICK_ANA_ELECTRON_DEFINITION_MAKER( "SSbjets_MMtight",
    makeTopElectronTool(args, "Tight", new ApproxPTVarCone(0.05, -1.), 2) )
  QUICK_ANA_ELECTRON_DEFINITION_MAKER( "SSbjets_MMloose",
    makeTopElectronTool(args, "Medium", nullptr, 2) )
} // namespace ana
