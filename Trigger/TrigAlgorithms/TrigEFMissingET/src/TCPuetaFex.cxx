/*
 * Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
 */

#include "TCPuetaFex.h"
#include "TrigInterfaces/HLTCheck.h"
#include "TrigEFMissingET/METComponent.h"
#include "TrigEFMissingET/SignedKinematics.h"
#include "xAODCaloEvent/CaloClusterContainer.h"

namespace HLT { namespace MET {
  TCPuetaFex::TCPuetaFex(const std::string& name, ISvcLocator* pSvcLocator) :
    FexBase(name, pSvcLocator),
    m_clusState(xAOD::CaloCluster::CALIBRATED)
  {
    declareProperty("PileupNumRings", m_nRings,
        "Number of rings to divide the calorimeter into");
    declareProperty("PileupNumStdDev", m_pileupNSigma = 2.0 ,
        "remove energetic CaloClusters x Std Dev away from mean");
  }

  HLT::ErrorCode TCPuetaFex::hltInitialize()
  {
    return initializeBase({});
  }

  HLT::ErrorCode TCPuetaFex::hltFinalize()
  {
    return HLT::OK;
  }

  HLT::ErrorCode TCPuetaFex::fillMET(
      xAOD::TrigMissingET& met,
      const std::vector<std::vector<HLT::TriggerElement*>>& tes_in)
  {
    const xAOD::CaloClusterContainer* allClusters = nullptr;
    if (tes_in.size() != 1 || tes_in.at(0).size() != 1) {
      ATH_MSG_ERROR("Unexpected number of input trigger elements");
      return HLT::NAV_ERROR;
    }
    HLT_CHECK( getFeature(tes_in.at(0).at(0), allClusters) );

    // This is where we will hold the output
    METComponent sum;

    // Start by binning the clusters in eta rings
    float maxEta = 5;
    float etaWidth = 2*maxEta/m_nRings;
    std::vector<std::vector<const xAOD::CaloCluster*>> ringClusters(m_nRings);
    // Also build the necessary pieces to calculate the thresholds
    std::vector<float> ringEnergySum(m_nRings);
    std::vector<float> ringEnergySumSquared(m_nRings);
    for (const xAOD::CaloCluster* iclus : *allClusters) {
      // Skip any cluster outside of the eta range - put it straight into the
      // sum
      if (abs(iclus->eta(m_clusState) ) < maxEta) {
        sum += SignedKinematics::fromEnergyEtaPhiM(
            iclus->e(m_clusState),
            iclus->eta(m_clusState),
            iclus->phi(m_clusState),
            iclus->calM() );
        continue;
      }
      std::size_t ring = (iclus->eta(m_clusState) - maxEta) / etaWidth;
      ringClusters.at(ring).push_back(iclus);
      ringEnergySum.at(ring) += iclus->e(m_clusState);
      ringEnergySumSquared.at(ring) += iclus->e(m_clusState)*iclus->e(m_clusState);
    }
    // Now loop over the rings
    for (std::size_t iRing = 0; iRing < m_nRings; ++iRing) {
      std::vector<const xAOD::CaloCluster*>& clusters = ringClusters.at(iRing);
      // Get the threshold
      float mean = ringEnergySum.at(iRing)/clusters.size();
      float sigma = sqrt(abs(
            ringEnergySumSquared.at(iRing)/clusters.size()- mean*mean) );
      float threshold = mean + m_pileupNSigma * sigma;

      // Get the energy over threshold
      float EOverThreshold = 0;
      for (const xAOD::CaloCluster* iclus : clusters) {
        if (abs(iclus->e(m_clusState) ) < threshold)
          EOverThreshold += iclus->e(m_clusState);
      }
      // Now perform the correction
      // Get the ring parameters
      float ringEtaMin = -maxEta + iRing*etaWidth;
      float ringEtaMax = ringEtaMin + etaWidth;
      // Convert into a width in theta
      double dTheta = 2 * (std::atan(std::exp(-ringEtaMin) -
                           std::atan(std::exp(-ringEtaMax) ) ) );
      double ringSolidAngle = dTheta * 2 * TMath::Pi();
      for (const xAOD::CaloCluster* iclus : clusters) {
        SignedKinematics kin = SignedKinematics::fromEnergyEtaPhiM(
            iclus->e(m_clusState),
            iclus->eta(m_clusState),
            iclus->phi(m_clusState),
            iclus->calM() );
        sum += kin;
        // Get the cluster's moments to calculate its solid angle
        double secondR;
        double centerMag;
        iclus->retrieveMoment(xAOD::CaloCluster::SECOND_R, secondR);
        iclus->retrieveMoment(xAOD::CaloCluster::CENTER_MAG, centerMag);
        if (secondR == 0 || centerMag == 0)
          // Protected against divide by 0 errors. Don't correct these clusters
          continue;
        double clusSolidAngle = 4 * TMath::Pi() * secondR / (centerMag*centerMag);
        double correction = EOverThreshold * clusSolidAngle/ringSolidAngle;
        // Don't correct clusters below 0
        if (kin.absEnergy() < correction) {
          // The corrections are subtracted... this is probably wrong but it's
          // what was in the old algorithm
          sum.mpx -= correction*kin.cosPhi();
          sum.mpy -= correction*kin.sinPhi();
        }
      }
    } //> end loop over rings

    // Now save this into the output
    sum.fillMET(met);
    return HLT::OK;
  }
} } //> end namespace HLT::MET
