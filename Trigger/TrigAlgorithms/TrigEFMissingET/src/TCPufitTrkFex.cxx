/*
 * Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
 */

#include "TCPufitTrkFex.h"
#include "TrigEFMissingET/METComponent.h"
#include "TrigEFMissingET/PufitGrid.h"
#include "TrigEFMissingET/PufitUtils.h"
#include "TrigInterfaces/HLTCheck.h"
#include "xAODJet/JetContainer.h"
#include "xAODTracking/TrackParticleContainer.h"
#include "xAODTracking/VertexContainer.h"
#include "AthContainers/ConstDataVector.h"

#include <algorithm>


namespace HLT { namespace MET {
  TCPufitTrkFex::TCPufitTrkFex(const std::string& name, ISvcLocator* pSvcLocator) :
    FexBase(name, pSvcLocator),
    m_trackSelTool("InDet::InDetTrackSelectionTool/TrackSelectionTool", this)
  {
    declareProperty("MinJetPtRpT", m_minJetPtRpT = 20 * Gaudi::Units::GeV,
        "Minimum pT [MeV] to require for all central jets");
    declareProperty("MaxJetPtRpT", m_maxJetPtRpT = 60 * Gaudi::Units::GeV,
        "Maximum pT [MeV] for central jets when applying RpT.");
    declareProperty("ForwardPtCut", m_forwardPtCut = 50 * Gaudi::Units::GeV,
        "pT cut [MeV] to use for forward jets");
    declareProperty("RpTCut", m_RpTCut = 0.1, "RpT cut to apply to central jets");
    declareProperty("TrackPtCut", m_trackPtCut = 0.0,
        "pT cut [MeV] for tracks entering the RpT calculation");
    declareProperty("TrackSelectionTool", m_trackSelTool,
        "The tool to use to select tracks");
    declareProperty("MaxEta", m_maxEta=5, "The eta range to use");
    declareProperty("NEtaBins", m_nEtaBins=14, "The number of eta bins");
    declareProperty("NPhiBins", m_nPhiBins=8, "The number of phi bins");
    declareProperty("TrimFraction", m_trimFraction=0.9,
        "The fraction of bins to use when calculating the mean and variance");
    declareProperty("CaloNoise", m_caloNoise=50,
        "The noise term in the calorimeter noise estimate [MeV]");
    declareProperty("CaloStochastic", m_caloStoch=15.81,
        "The stochastic term in the calorimeter noise estimate [MeV^1/2]");
    declareProperty("ConstraintImportance", m_constraintImportance=1,
        "The relative importance of the two constraints");
  }

  TCPufitTrkFex::~TCPufitTrkFex() {}

  HLT::ErrorCode TCPufitTrkFex::hltInitialize()
  {
    if (!m_trackSelTool.retrieve() ) {
      ATH_MSG_ERROR("Failed to retrieve track selection tool!");
      return HLT::ERROR;
    }

    std::vector<std::string> componentNames{"Uncorrected"};
    return initializeBase(componentNames);
  }

  HLT::ErrorCode TCPufitTrkFex::hltFinalize()
  {
    return HLT::OK;
  }

  HLT::ErrorCode TCPufitTrkFex::fillMET(
      xAOD::TrigMissingET& met,
      const std::vector<std::vector<HLT::TriggerElement*>>& tes_in)
  {
    // Retrieve inputs
    const xAOD::CaloClusterContainer* clusters = nullptr;
    const xAOD::JetContainer* jets = nullptr;
    ConstDataVector<xAOD::TrackParticleContainer> tracks(SG::VIEW_ELEMENTS);
    const xAOD::VertexContainer* vertices = nullptr;
    if (tes_in.size() != 4 ||
        tes_in.at(0).size() != 1 ||
        tes_in.at(1).size() != 1 ||
        tes_in.at(3).size() != 1) {
      ATH_MSG_ERROR("Unexpected TE input!");
      return HLT::NAV_ERROR;
    }

    HLT_CHECK( getFeature(tes_in.at(0).at(0), clusters) );
    HLT_CHECK( getFeature(tes_in.at(1).at(0), jets) );
    for (const HLT::TriggerElement* te : tes_in.at(2) ) {
      const xAOD::TrackParticleContainer* tmpTracks = nullptr;
      HLT_CHECK( getFeature(te, tmpTracks) );
      tracks.insert(tracks.end(), tmpTracks->begin(), tmpTracks->end() );
    }
    HLT_CHECK( getFeature(tes_in.at(3).at(0), vertices) );

    // Find the right vertex
    const xAOD::Vertex* priVtx = nullptr;
    for (const xAOD::Vertex* vtx : *vertices) {
      if (vtx->vertexType() == xAOD::VxType::PriVtx) {
        priVtx = vtx;
        break;
      }
    }
    METComponent total;

    if (priVtx && jets->size() > 0) {
      // Use the jet radius as the min DR for tracks
      float radius = jets->at(0)->getSizeParameter();
      // Store the jets we class as hard scatter here
      std::vector<const xAOD::Jet*> hardScatterJets;
      // Store central jets here - we'll do a dR association to all central jets
      std::vector<const xAOD::Jet*> centralJets;
      for (const xAOD::Jet* ijet : *jets) {
        if (fabs(ijet->eta() ) < 2.4)
          // central
          centralJets.push_back(ijet);
        else if (ijet->pt() > m_forwardPtCut)
          // Accept high pT forward jet
          hardScatterJets.push_back(ijet);
        // Otherwise the jet is rejected
      }
      std::size_t nCentralJets = centralJets.size();
      if (nCentralJets > 0) {
        // For each jet, keep track of the sum over pt of PV tracks
        std::vector<float> sumTrkPtPV(nCentralJets, 0);
        for (const xAOD::TrackParticle* itrk : tracks) {
          // Skip any track not accepted by the tool or of too low pT
          if (itrk->pt() < m_trackPtCut || !m_trackSelTool->accept(*itrk, priVtx) )
            continue;
          // Find the closest jet
          std::vector<float> trackDRs(nCentralJets, 0);
          // Get the delta-rs
          std::transform(centralJets.begin(), centralJets.end(), trackDRs.begin(),
              [p4=itrk->p4()] (const xAOD::Jet* ijet)
              { return p4.DeltaR(ijet->p4() ); });
          // Now get the smallest element of this vector
          auto drItr = std::min_element(trackDRs.begin(), trackDRs.end() );
          if (*drItr > radius)
            // This track isn't close enough to any jet
            continue;
          // Increment the relevant counter
          sumTrkPtPV.at(std::distance(trackDRs.begin(), drItr) ) += itrk->pt();
        }
        // Now we can loop over the jets and calculate RpT
        for (std::size_t ii = 0; ii < nCentralJets; ++ii) {
          float RpT = sumTrkPtPV.at(ii)/centralJets.at(ii)->pt();
          if (RpT > m_RpTCut)
            hardScatterJets.push_back(centralJets.at(ii) );
        }
      } //> end if(nCentralJets > 0)

      std::size_t nHardScatter = hardScatterJets.size();
      if (nHardScatter > 0) {
        // We only calculate pufit if there's anything to correct
        // Get the indices of any pileup clusters.
        std::unordered_set<std::size_t> hardScatterIndices;
        for (const xAOD::Jet* ijet : hardScatterJets)
          for (const auto& link : ijet->constituentLinks() )
            hardScatterIndices.insert(link.index() );
        // Now prepare the pileup sum and the pufit grid
        PufitUtils::CovarianceSum pileupSum;
        PufitGrid grid(m_maxEta, m_nEtaBins, m_nPhiBins);
        // Loop over the clusters
        for (std::size_t ii = 0; ii < clusters->size(); ++ii) {
          // Extract the cluster's kinematics
          const xAOD::CaloCluster* iclus = clusters->at(ii);
          SignedKinematics kin = SignedKinematics::fromEnergyEtaPhi(
              iclus->e(xAOD::CaloCluster::CALIBRATED),
              iclus->eta(xAOD::CaloCluster::CALIBRATED),
              iclus->phi(xAOD::CaloCluster::CALIBRATED) );
          // Skip anything outside of the eta range
          if (abs(kin.eta() ) > m_maxEta)
            continue;
          if (hardScatterIndices.erase(ii) ) {
            // erase returns the number of elements removed so this allows us to
            // say if the element was found and also stop it participating in
            // future look-ups
            // Being here means that this is a hard-scatter cluster - add it
            // into the final sum
            total += kin;
          }
          else {
            // This is a pileup object
            // Calculate its resolution - NB is the resolution floor here too
            // high? It's the same one that's used for whole towers in the other
            // pufit algorithm...
            double sigma = m_caloNoise*m_caloNoise* +
              kin.absPt()*m_caloStoch*m_caloStoch;
            pileupSum.add(kin, sigma);
            // Also add it to the grid
            grid += kin;
          }
        }

        // Next we need to mask the grid. We mask any tower that overlaps a jet
        // that we've called hard scatter
        for (PufitGrid::Tower& tower : grid) {
          for (const xAOD::Jet* ijet : hardScatterJets) {
            // Get the dPhi/dEta between the jet and the centre of the tower
            float dPhi = fabs(tower.towerPhi() - ijet->phi() );
            float dEta = fabs(tower.towerEta() - ijet->eta() );
            if (
               // Check one edge
               (dPhi < grid.phiWidth()/2 && dEta < (radius + grid.etaWidth()/2) ) ||
               // Check the other
               (dEta < grid.etaWidth()/2 && dPhi < (radius + grid.phiWidth()/2) ) ||
               // Check the corner
               (pow(dEta - grid.etaWidth()/2, 2) + 
                pow(dPhi - grid.phiWidth()/2, 2) < radius*radius) ) {
              tower.mask();
              break;
            } //> end overlap check
          }
        } //> end loop over towers
        // Now we can calculate the mean and variance from the unmasked towers
        std::pair<double, double> mav = PufitUtils::unmaskedMeanAndVariance(
            grid);

        // Get the jet directions
        std::vector<SignedKinematics> jetKinematics(nHardScatter);
        std::transform(
            hardScatterJets.begin(), hardScatterJets.end(), jetKinematics.begin(),
            [] (const xAOD::Jet* ijet) { return *ijet; });
        // Get the jet areas
        std::vector<double> jetAreas(nHardScatter);
        std::transform(
            hardScatterJets.begin(), hardScatterJets.end(), jetAreas.begin(),
            [] (const xAOD::Jet* ijet)
            { return ijet->getAttribute<float>(xAOD::JetAttribute::AttributeID::ActiveArea); });
        // Get the tower areas
        double towerArea = grid.etaWidth()*grid.phiWidth();
        // Scale the means and variances by the area ratios
        std::vector<double> expected(nHardScatter);
        std::vector<double> variances(nHardScatter);
        std::transform(jetAreas.begin(), jetAreas.end(), expected.begin(),
            [towerArea, mean=mav.first] (double jetArea)
            { return mean * jetArea / towerArea; });
        std::transform(jetAreas.begin(), jetAreas.end(), variances.begin(),
            [towerArea, variance=mav.second] (double jetArea)
            { return variance * jetArea / towerArea; });

        // Get the corrections
        std::vector<SignedKinematics> corrections = PufitUtils::pufit(
            pileupSum.sum,
            pileupSum.covariance,
            expected,
            variances,
            jetKinematics,
            m_constraintImportance);

        // Before applying the corrections, save the uncorrected jet sum as a
        // component
        total.fillMETComponent(0, met);
        // Now add the pufit corrections
        for (const SignedKinematics& kin : corrections)
          total += kin;
      } //> end if(nHardScatter > 0)
    } //> end if(priVtx && jets->size() > 0)
    total.fillMET(met);
    return HLT::OK;
  }

} } //> end namespace HLT::MET
