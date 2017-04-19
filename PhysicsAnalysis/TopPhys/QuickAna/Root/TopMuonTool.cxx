/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//
// includes
//

#include <QuickAna/TopMuonTool.h>

#include <MuonSelectorTools/MuonSelectionTool.h>
#include <QuickAna/AnaToolRetrieve.h>
#include <QuickAna/DefinitionArgs.h>
#include <QuickAna/DefinitionMaker.h>
#include <QuickAna/InternalConfiguration.h>
#include <QuickAna/IsolationTools.h>
#include <QuickAna/MuonTool.h>

//
// method implementations
//

namespace ana
{
  TopMuonToolPreCorrect ::
  TopMuonToolPreCorrect (const std::string& name)
    : AsgTool (name), AnaToolCorrect<xAOD::MuonContainer> (name)
  {
  }



  AnalysisStep TopMuonToolPreCorrect ::
  step () const
  {
    return STEP_PRECORRECT;
  }



  StatusCode TopMuonToolPreCorrect ::
  initialize()
  {
    return StatusCode::SUCCESS;
  }



  StatusCode TopMuonToolPreCorrect ::
  correctObject (xAOD::Muon& muon)
  {
    // "miniIso" (approximation)

    const xAOD::TrackParticleContainer* tp(0);
    ATH_CHECK(evtStore()->retrieve(tp, "InDetTrackParticles") );

    double iso(0.);
    if ( // !m_isPrimaryxAOD &&
         muon.isAvailable<float>("ptvarcone40") )
    {
      iso = muon.auxdataConst<float>("ptvarcone40");
    }
    else
    {
      if (tp && muon.pt() > 20e3) { // tracks thinned for pt <= 15 GeV in DxAODs
        for (const auto* const trkPtr : *tp)
        {
          if ( muon.inDetTrackParticleLink().isValid() &&
               (muon.inDetTrackParticleLink() != nullptr) &&
               trkPtr->p4().DeltaR((*muon.inDetTrackParticleLink())->p4()) < 1e-3 )
          {
            continue;
          }
          if ( trackSelector(*trkPtr) &&
               trkPtr->p4().DeltaR(muon.p4()) < 10e3 / muon.pt() )
          {
            iso += trkPtr->pt();
          }
        }
      }
    }

    muon.auxdata<float>("miniIso") = iso;

    // z0 wrt primary vertex calculation

    const xAOD::VertexContainer *vertexContainer(0);
    ATH_CHECK( evtStore()->retrieve(vertexContainer, "PrimaryVertices" ) );

    float z0_wrt_pv(-99.);
    const xAOD::TrackParticle* mutrk = muon.primaryTrackParticle();
    const xAOD::Vertex *pv(0);
    for ( const auto* const vtx_itr : *vertexContainer )
    {
      if (vtx_itr->vertexType() != xAOD::VxType::VertexType::PriVtx) continue;
      else { pv = vtx_itr; break;}
    }
    if (pv && mutrk) z0_wrt_pv = ( mutrk->z0() + mutrk->vz() ) - pv->z();
    muon.auxdata<float>("z0_wrt_pv") = z0_wrt_pv;

    return StatusCode::SUCCESS;
  }



  bool TopMuonToolPreCorrect::trackSelector(const xAOD::TrackParticle &trk) {
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

    //ATH_MSG_DEBUG ("pt, eta, nSi, nSiHoles, nPixHoles = " << trk.pt() << ", " << trk.eta() << ", " << ns+nsd+np+npd << ", " << nsd+npd << ", " << (int) npd);
    // for Loose in https://svnweb.cern.ch/trac/atlasoff/browser/InnerDetector/InDetRecTools/InDetTrackSelectionTool/trunk/Root/InDetTrackSelectionTool.cxx
    if (trk.pt() > 400 &&
        std::fabs(trk.eta()) < 2.5 &&
        ns+nsd+np+npd >= 7 &&
        nsd+npd <= 2 &&
        npd <= 1)
        return true;
    return false;
  }



  TopMuonToolSelect ::
  TopMuonToolSelect (const std::string& name)
    : AsgTool (name), AnaToolSelect<xAOD::MuonContainer> (name),
      m_quality (xAOD::Muon::Medium),
      m_isolation (nullptr),
      m_z0cut (-1.),
      m_sd0cut (-1.),
      m_selection ("selection", this)
  {}



  StatusCode TopMuonToolSelect ::
  initialize()
  {
    ATH_CHECK (ASG_MAKE_ANA_TOOL (m_selection, CP::MuonSelectionTool));
    ATH_CHECK (m_selection.setProperty ("MaxEta", 2.5));
    ATH_CHECK (m_selection.setProperty ("MuQuality", int (m_quality)));

    ATH_CHECK (m_selection.initialize());

    registerCut (SelectionStep::MET, "selection", cut_selection);
    registerCut (SelectionStep::MET, "author", cut_author);
    registerCut (SelectionStep::MET, "minPt", cut_minPt);
    registerCut (SelectionStep::MET, "maxEta", cut_maxEta);
    registerCut (SelectionStep::MET, "z0", cut_z0);
    registerCut (SelectionStep::MET, "sd0", cut_sd0);
    registerCut (SelectionStep::MET, "isolation", cut_isolation);

    return StatusCode::SUCCESS;
  }



  StatusCode TopMuonToolSelect ::
  selectObject (xAOD::Muon& muon)
  {
    cut_selection.setPassedIf (m_selection->accept (muon));

    cut_author.setFailedIf (m_author!=xAOD::Muon::unknown && muon.author() != xAOD::Muon::MuidCo);

    cut_minPt.setPassedIf (muon.pt() > 25.e3);

    //cut_maxEta.setPassedIf (std::fabs(muon.eta()) < 2.5);

    // Additional Hit requirements -- included in MuonSelectionTools

    // Requirements on longitudinal impact parameter
    if (m_z0cut>0)
    {
      float z0 = muon.auxdataConst<float>("z0_wrt_pv");
      cut_z0.setFailedIf (std::fabs(z0) > m_z0cut);
    }

    // Requirements on transverse impact parameter
    const xAOD::TrackParticle* mutrk = muon.primaryTrackParticle();
    float d0       = mutrk->d0();
    float sigma_d0 = sqrt(mutrk->definingParametersCovMatrix()(0,0));
    float sd0      = 0;
    if (sigma_d0 != 0) sd0 = d0/sigma_d0;

    cut_sd0.setFailedIf (m_sd0cut>0 && std::fabs(sd0) > m_sd0cut);

    // isolation, if m_isolation != nullptr
    cut_isolation.setFailedIf (m_isolation && !m_isolation->passSelection(muon));

    return StatusCode::SUCCESS;
  }



  StatusCode makeTopMuonTool (DefinitionArgs& args,
                              xAOD::Muon::Quality quality,
                              xAOD::Muon::Author author,
                              IsolationBase* isolation,
                              double z0cut,
                              double sd0cut)
  {
    // Retrieval tool
    std::unique_ptr<IAnaTool> retrieveTool
      (new AnaToolRetrieve (args.prefix() + "_retrieve", "Muons"));
    args.add (std::move (retrieveTool));

    // Pre Correction tool
    std::unique_ptr<IAnaTool> precorrectTool
      (new TopMuonToolPreCorrect (args.prefix() + "_precorrect"));
    args.add (std::move (precorrectTool));

    // Correction tool
    std::unique_ptr<IAnaTool> correctTool
      (new MuonToolCorrect (args.prefix() + "_correct"));
    args.add (std::move (correctTool));

    // Selection tool
    std::unique_ptr<TopMuonToolSelect> selectTool
      (new TopMuonToolSelect (args.prefix() + "_select"));
    // To-Do: declare these as properties as in TopElectronTool.cxx
    selectTool->m_quality = quality;
    selectTool->m_author = author;
    selectTool->m_isolation.reset (isolation);
    selectTool->m_z0cut = z0cut;
    selectTool->m_sd0cut = sd0cut;
    args.add (std::move (selectTool));

    if (args.configuration()->isData() == false)
    {
      // Weight tool
      std::unique_ptr<IAnaTool> weightTool
	(new MuonToolWeight (args.prefix() + "_weight"));
      args.add (std::move (weightTool));
    }

    return StatusCode::SUCCESS;
  }

  // Note: Tight quality doesn't exist yet, use Medium for now
  // Note: for some reason the top default doesn't require author to be xAOD::Muon::MuidCo yet
  // Note: For some reason the top default doesn't apply isolation on muon yet
  //   -- Update on 04/24/2015: they are now using StandardIsolation() (in trunk)

  // Macros expand to wrapper functions for making the tools

  QUICK_ANA_MUON_DEFINITION_MAKER ("top_MMtight", makeTopMuonTool (args, xAOD::Muon::Medium, xAOD::Muon::unknown, new StandardIsolation("Muon")) )
  QUICK_ANA_MUON_DEFINITION_MAKER ("top_MMloose", makeTopMuonTool (args, xAOD::Muon::Medium, xAOD::Muon::unknown))
  // Note: sd0 not validated in rel19, to avoid cutting away too much signal, either
  //  drop it or use a looser cut such as 5
  QUICK_ANA_MUON_DEFINITION_MAKER ("SSbjets_MMtight", makeTopMuonTool (args, xAOD::Muon::Medium, xAOD::Muon::unknown, new ApproxPTVarCone(0.05, -1.), 2))
  QUICK_ANA_MUON_DEFINITION_MAKER ("SSbjets_MMloose", makeTopMuonTool (args, xAOD::Muon::Medium, xAOD::Muon::unknown, nullptr, 2))

  // Run 1 definitions -- for reference
  QUICK_ANA_MUON_DEFINITION_MAKER ("run1-top_MMtight", makeTopMuonTool (args, xAOD::Muon::Tight, xAOD::Muon::MuidCo, new ApproxPTVarCone(0.05, -1.), 2))
  QUICK_ANA_MUON_DEFINITION_MAKER ("run1-top_MMloose", makeTopMuonTool (args, xAOD::Muon::Tight, xAOD::Muon::MuidCo, nullptr, 2))
  QUICK_ANA_MUON_DEFINITION_MAKER ("run1-SSbjets_MMtight", makeTopMuonTool (args, xAOD::Muon::Tight, xAOD::Muon::MuidCo, new ApproxPTVarCone(0.05, -1.), 2, 3))
  QUICK_ANA_MUON_DEFINITION_MAKER ("run1-SSbjets_MMloose", makeTopMuonTool (args, xAOD::Muon::Tight, xAOD::Muon::MuidCo, nullptr, 2, 3))
}
