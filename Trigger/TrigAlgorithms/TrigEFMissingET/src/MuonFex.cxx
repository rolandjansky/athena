/*
 * Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
 */

#include "MuonFex.h"
#include "TrigInterfaces/HLTCheck.h"
#include "TrigEFMissingET/METComponent.h"
#include "TrigEFMissingET/SignedKinematics.h"
#include "xAODMuon/MuonContainer.h"
#include "AthContainers/ConstDataVector.h"

namespace HLT { namespace MET {
  MuonFex::MuonFex(const std::string& name, ISvcLocator* pSvcLocator) :
    FexBase(name, pSvcLocator)
  {}

  HLT::ErrorCode MuonFex::hltInitialize()
  {
    // I'm assuming that we don't need to split this into components at all.
    return initializeBase({});
  }

  HLT::ErrorCode MuonFex::hltFinalize()
  {
    return HLT::OK;
  }

  HLT::ErrorCode MuonFex::fillMET(
      xAOD::TrigMissingET& met,
      const std::vector<std::vector<HLT::TriggerElement*>>& tes_in)
  {
    // Check the input
    if (tes_in.size() != 1) {
      ATH_MSG_ERROR("Unexpected number of input trigger elements!");
      return HLT::NAV_ERROR;
    }
    ConstDataVector<xAOD::MuonContainer> muons(SG::VIEW_ELEMENTS);
    for (const HLT::TriggerElement* te : tes_in.at(0) ) {
      const xAOD::MuonContainer* tmp = nullptr;
      HLT_CHECK( getFeature(te, tmp) );
      muons.insert(muons.end(), tmp->begin(), tmp->end() );
    }
    return HLT::OK;
    METComponent sum;
    for (const xAOD::Muon* imu : muons)
      sum += *imu;
    sum.fillMET(met);
    return HLT::OK;
  }

} } //> end namespace HLT::MET
