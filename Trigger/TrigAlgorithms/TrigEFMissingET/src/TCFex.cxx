/*
 * Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
 */

#include "TCFex.h"
#include "TrigInterfaces/HLTCheck.h"
#include "TrigEFMissingET/METComponent.h"
#include "TrigEFMissingET/SignedKinematics.h"
#include "xAODCaloEvent/CaloClusterContainer.h"
#include <numeric>

namespace {
  std::vector<std::pair<float, float>> bounds{
    {0, 1.5}, {-1.5, 0}, {1.5, 5}, {-5, -1.5}};
} //> end anonymous namespace

namespace HLT { namespace MET {
  TCFex::TCFex(const std::string& name, ISvcLocator* pSvcLocator) :
    FexBase(name, pSvcLocator)
  {
    declareProperty("UseUncalibrated", m_saveUncalibrated=false,
        "Use the uncalibrated (EM level) topoclusters");
  }

  TCFex::~TCFex() {}

  HLT::ErrorCode TCFex::hltInitialize()
  {
    m_clusterState = m_saveUncalibrated ? 
                     xAOD::CaloCluster::UNCALIBRATED :
                     xAOD::CaloCluster::CALIBRATED;
    return initializeBase({"TCB1", "TCB2", "TCE1", "TCE2"});
  }

  HLT::ErrorCode TCFex::hltFinalize()
  {
    return HLT::OK;
  }

  HLT::ErrorCode TCFex::fillMET(
      xAOD::TrigMissingET& met,
      const std::vector<std::vector<HLT::TriggerElement*>>& tes_in)
  {
    const xAOD::CaloClusterContainer* clusters = nullptr;
    if (tes_in.size() != 1 || tes_in.at(0).size() != 1) {
      ATH_MSG_ERROR("Unexpected number of input trigger elements");
      return HLT::NAV_ERROR;
    }
    HLT_CHECK( getFeature(tes_in.at(0).at(0), clusters) );

    // The summed components
    std::vector<METComponent> sums(bounds.size() );
    for (const xAOD::CaloCluster* iclus : *clusters) {
      SignedKinematics kin = SignedKinematics::fromEnergyEtaPhiM(
          iclus->e(m_clusterState),
          iclus->eta(m_clusterState),
          iclus->phi(m_clusterState),
          m_saveUncalibrated ? iclus->rawM() : iclus->calM() );
      // Put it in the right component
      for (std::size_t ii = 0; ii < bounds.size(); ++ii)
        if (bounds.at(ii).first < kin.eta() &&
            kin.eta() <= bounds.at(ii).second)
          sums.at(ii) += kin;
    }

    // Now save the MET
    std::accumulate(sums.begin(), sums.end(), METComponent{}).fillMET(met);
    // Save the components
    for (std::size_t ii = 0; ii < sums.size(); ++ii)
      sums.at(ii).fillMETComponent(ii, met);
    return HLT::OK;
  }
} } //> end namespace HLT::MET
