/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

// System includes
#include <typeinfo>

// Framework includes
#include "AthContainers/ConstDataVector.h"

// Local includes
#include "AssociationUtils/MuPFJetOverlapTool.h"
#include "AssociationUtils/DeltaRMatcher.h"

namespace
{
  /// Unit conversion constants
  const double GeV = 1e3;
}

namespace ORUtils
{

  //---------------------------------------------------------------------------
  // Constructor
  //---------------------------------------------------------------------------
  MuPFJetOverlapTool::MuPFJetOverlapTool(const std::string& name)
    : BaseOverlapTool(name)
  {
    declareProperty("BJetLabel", m_bJetLabel = "",
                    "Input b-jet flag. Disabled by default.");
    declareProperty("NumJetTrk", m_numJetTrk = 4,
                    "Min number of jet tracks to keep jet and remove muon");
    declareProperty("JetNumTrackDecoration", m_jetNumTrkDec = "",
                    "User-defined decoration for jet numTrack");
    declareProperty("JetSumTrackPTDecoration", m_jetSumTrkPtDec = "",
                    "User-defined decoration for jet sumTrackPT");
    declareProperty("ConeDR", m_coneDR = 0.4,
                    "Cone for removing jets");
    declareProperty("MuPFJet_lowNtrk_x1", m_muPFJet_lowNtrk_x1 = 0.7,"");
    declareProperty("MuPFJet_lowNtrk_x2", m_muPFJet_lowNtrk_x2 = 0.85,"");
    declareProperty("MuPFJet_lowNtrk_y0", m_muPFJet_lowNtrk_y0 = 15.*GeV,"");
    declareProperty("MuPFJet_lowNtrk_y1", m_muPFJet_lowNtrk_y1 = 15.*GeV,"");
    declareProperty("MuPFJet_lowNtrk_y2", m_muPFJet_lowNtrk_y2 = 30.*GeV,"");
    declareProperty("MuPFJet_highNtrk_x1", m_muPFJet_highNtrk_x1 = 0.6,"");
    declareProperty("MuPFJet_highNtrk_x2", m_muPFJet_highNtrk_x2 = 0.9,"");
    declareProperty("MuPFJet_highNtrk_y0", m_muPFJet_highNtrk_y0 = 5.*GeV,"");
    declareProperty("MuPFJet_highNtrk_y1", m_muPFJet_highNtrk_y1 = 5.*GeV,"");
    declareProperty("MuPFJet_highNtrk_y2", m_muPFJet_highNtrk_y2 = 30.*GeV,"");

    declareProperty("UseRapidity", m_useRapidity = true,
                    "Calculate delta-R using rapidity");
  }

  //---------------------------------------------------------------------------
  // Initialize
  //---------------------------------------------------------------------------
  StatusCode MuPFJetOverlapTool::initializeDerived()
  {

    ATH_MSG_DEBUG("Configuring mu-pflow-jet cone size " << m_coneDR);
    m_dRMatchCone = std::make_unique<DeltaRMatcher>(m_coneDR, m_useRapidity);

    // Initialize the b-jet helper
    if(!m_bJetLabel.empty()) {
      ATH_MSG_DEBUG("Configuring btag-aware mu-pflow-jet OR with btag label: " << m_bJetLabel);
      m_bJetHelper = std::make_unique<BJetHelper>(m_bJetLabel);
    }

    // Additional config printouts
    ATH_MSG_DEBUG("PFlow jet removal which are identified as muons config: NumJetTrk " << m_numJetTrk <<
                  " MuPFJet_lowNtrk_x1 " << m_muPFJet_lowNtrk_x1 <<
                  " MuPFJet_lowNtrk_x2 " << m_muPFJet_lowNtrk_x2 <<
                  " MuPFJet_lowNtrk_y0 " << m_muPFJet_lowNtrk_y0 <<
                  " MuPFJet_lowNtrk_y1 " << m_muPFJet_lowNtrk_y1 <<
                  " MuPFJet_lowNtrk_y2 " << m_muPFJet_lowNtrk_y2 <<
                  " MuPFJet_highNtrk_x1 " << m_muPFJet_highNtrk_x1 <<
                  " MuPFJet_highNtrk_x2 " << m_muPFJet_highNtrk_x2 <<
                  " MuPFJet_highNtrk_y0 " << m_muPFJet_highNtrk_y0 <<
                  " MuPFJet_highNtrk_y1 " << m_muPFJet_highNtrk_y1 <<
                  " MuPFJet_highNtrk_y2 " << m_muPFJet_highNtrk_y2);
    if(!m_jetNumTrkDec.empty()) {
      ATH_MSG_DEBUG("Using user-defined JetNumTrackDecoration " << m_jetNumTrkDec);
    }
    if(!m_jetSumTrkPtDec.empty()) {
      ATH_MSG_DEBUG("Using user-defined JetSumTrackPTDecoration " << m_jetSumTrkPtDec);
    }

    return StatusCode::SUCCESS;
  }

  //---------------------------------------------------------------------------
  // Identify overlaps
  //---------------------------------------------------------------------------
  StatusCode MuPFJetOverlapTool::
  findOverlaps(const xAOD::IParticleContainer& cont1,
               const xAOD::IParticleContainer& cont2) const
  {
    // Check the container types
    if(typeid(cont1) != typeid(xAOD::MuonContainer) &&
       typeid(cont1) != typeid(ConstDataVector<xAOD::MuonContainer>)) {
      ATH_MSG_ERROR("First container arg is not of type MuonContainer!");
      return StatusCode::FAILURE;
    }
    if(typeid(cont2) != typeid(xAOD::JetContainer) &&
       typeid(cont2) != typeid(ConstDataVector<xAOD::JetContainer>)) {
      ATH_MSG_ERROR("Second container arg is not of type JetContainer!");
      return StatusCode::FAILURE;
    }
    ATH_CHECK( findOverlaps(static_cast<const xAOD::MuonContainer&>(cont1),
                            static_cast<const xAOD::JetContainer&>(cont2)) );
    return StatusCode::SUCCESS;
  }

  //---------------------------------------------------------------------------
  // Identify overlaps
  //---------------------------------------------------------------------------
  StatusCode MuPFJetOverlapTool::
  findOverlaps(const xAOD::MuonContainer& muons,
               const xAOD::JetContainer& jets) const
  {
    ATH_MSG_DEBUG("Removing overlapping muons and jets");

    // Initialize output decorations if necessary
    m_decHelper->initializeDecorations(muons);
    m_decHelper->initializeDecorations(jets);

    // Retrieve the primary vertex for later reference
    size_t vtxIdx = 0;
    if(m_jetNumTrkDec.empty() && m_jetSumTrkPtDec.empty()) {
      auto vtx = getPrimVtx();
      ATH_CHECK(vtx != nullptr);
      vtxIdx = vtx->index();
    }

    // Remove suspicious jets that overlap with muons.
    for(const xAOD::Muon* muon : muons){
      if(!m_decHelper->isSurvivingObject(*muon)) continue;

      for(const xAOD::Jet* jet : jets){
        if(!m_decHelper->isSurvivingObject(*jet)) continue;

        // Don't reject user-defined b-tagged jets
        if(m_bJetHelper && m_bJetHelper->isBJet(*jet)) continue;

        // Get the number of tracks and the sumPT of those tracks
        int nTrk = getNumTracks(*jet, vtxIdx);
        float sumTrkPt = getSumTrackPt(*jet, vtxIdx);

        // Find muon ID track pT
        float mu_id_pt(0);
        typedef ElementLink<xAOD::TrackParticleContainer> TrackLink;
        if(muon->isAvailable<TrackLink>("inDetTrackParticleLink")){
          const TrackLink link = muon->auxdata<TrackLink>("inDetTrackParticleLink");
          if(link.isValid() && muon->auxdata<unsigned short>("muonType")!=xAOD::Muon::SiliconAssociatedForwardMuon){
            mu_id_pt=(*link)->pt();
          }
        }

        // Find muon topoetcone40
        static const SG::AuxElement::ConstAccessor< float > topoetcone40_acc("topoetcone40");
        float mu_topoetcone40 = topoetcone40_acc(*muon);

        bool removeJet=false;
        if(m_dRMatchCone->objectsMatch(*muon, *jet)){
          // determine if we are in the low or high nTrk
          if (nTrk < m_numJetTrk){
            if (mu_topoetcone40 < m_muPFJet_lowNtrk_y0) removeJet=true;
            if (mu_topoetcone40<m_muPFJet_lowNtrk_y0+(m_muPFJet_lowNtrk_y2-m_muPFJet_lowNtrk_y1)/(m_muPFJet_lowNtrk_x2-m_muPFJet_lowNtrk_x1)*(mu_id_pt/sumTrkPt-m_muPFJet_lowNtrk_x1)) removeJet=true;
            if (mu_id_pt/sumTrkPt>m_muPFJet_lowNtrk_x2) removeJet=true;
          }else{
            if (mu_topoetcone40 < m_muPFJet_highNtrk_y0) removeJet=true;
            if (mu_topoetcone40<m_muPFJet_highNtrk_y0+(m_muPFJet_highNtrk_y2-m_muPFJet_highNtrk_y1)/(m_muPFJet_highNtrk_x2-m_muPFJet_highNtrk_x1)*(mu_id_pt/sumTrkPt-m_muPFJet_highNtrk_x1)) removeJet=true;
            if (mu_id_pt/sumTrkPt>m_muPFJet_highNtrk_x2) removeJet=true;
          }
        }
        if (removeJet==true) ATH_CHECK( handleOverlap(jet, muon) );

      }
    }

    return StatusCode::SUCCESS;
  }

  //---------------------------------------------------------------------------
  // Retrieve the primary vertex
  //---------------------------------------------------------------------------
  const xAOD::Vertex* MuPFJetOverlapTool::getPrimVtx() const
  {
    const char* contName = "PrimaryVertices";
    const xAOD::VertexContainer* vertices = nullptr;
    if(evtStore()->retrieve(vertices, contName).isSuccess()) {
      for(const xAOD::Vertex* vtx : *vertices) {
        if(vtx->vertexType() == xAOD::VxType::PriVtx)
          return vtx;
      }
    }
    else {
      ATH_MSG_WARNING("Failed to retrieve " << contName);
    }
    // No PV found. We cannot execute the OR recommendations.
    ATH_MSG_FATAL("No primary vertex in the PrimaryVertices container!");
    return nullptr;
  }

  //---------------------------------------------------------------------------
  // Get the number of tracks in a jet
  //---------------------------------------------------------------------------
  int MuPFJetOverlapTool::getNumTracks(const xAOD::Jet& jet, size_t vtxIdx) const
  {
    // Use the user decoration if configured
    if(!m_jetNumTrkDec.empty()) {
      static const SG::AuxElement::ConstAccessor< int > acc(m_jetNumTrkDec);
      return acc(jet);
    }
    static const SG::AuxElement::ConstAccessor< std::vector<int> > acc("NumTrkPt500");
    return acc(jet).at(vtxIdx);
  }

  //---------------------------------------------------------------------------
  // Get the sum track pt of a jet
  //---------------------------------------------------------------------------
  float MuPFJetOverlapTool::getSumTrackPt(const xAOD::Jet& jet, size_t vtxIdx) const
  {
    // Use the user decoration if configured
    if(!m_jetSumTrkPtDec.empty()) {
      static const SG::AuxElement::ConstAccessor< float > acc(m_jetSumTrkPtDec);
      return acc(jet);
    }
    static const SG::AuxElement::ConstAccessor< std::vector<float> > acc("SumPtTrkPt500");
    return acc(jet).at(vtxIdx);
  }

} // namespace ORUtils
