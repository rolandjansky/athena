/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


#include "TrigT1CaloSim/JEPCMMMaker.h"
#include "TrigT1Interfaces/TrigT1CaloDefs.h"

namespace LVL1 {


JEPCMMMaker::JEPCMMMaker(const std::string& name, ISvcLocator* pSvcLocator)
             : AthAlgorithm(name, pSvcLocator)
{
  declareProperty("JetElementLocation",
         m_jetElementLocation = TrigT1CaloDefs::JetElementLocation);
  declareProperty("JEMHitsLocation",
         m_jemHitsLocation    = TrigT1CaloDefs::JEMHitsLocation);
  declareProperty("JEMEtSumsLocation",
         m_jemEtSumsLocation  = TrigT1CaloDefs::JEMEtSumsLocation);
  declareProperty("CMMJetHitsLocation",
         m_cmmJetLocation     = TrigT1CaloDefs::CMMJetHitsLocation);
  declareProperty("CMMEtSumsLocation",
         m_cmmEnergyLocation  = TrigT1CaloDefs::CMMEtSumsLocation);
  declareProperty("JetRoILocation",
         m_jetRoiLocation     = TrigT1CaloDefs::JetROILocation);
  declareProperty("JEMRoILocation",
         m_jemRoiLocation     = TrigT1CaloDefs::JEMRoILocation);
  declareProperty("CMMRoILocation",
         m_cmmRoiLocation     = TrigT1CaloDefs::CMMRoILocation);
  declareProperty("JEPBSCollectionLocation",
         m_jepBsCollectionLocation = TrigT1CaloDefs::JEPBSCollectionLocation);
  declareProperty("JEPRoIBSCollectionLocation",
    m_jepRoiBsCollectionLocation = TrigT1CaloDefs::JEPRoIBSCollectionLocation);
  declareProperty("RunSimulation", m_runSimulation = true);
}

JEPCMMMaker::~JEPCMMMaker()
{
}

// Initialize

StatusCode JEPCMMMaker::initialize()
{
  ATH_MSG_WARNING("JEPCMMMaker is DEPRECATED in Run2");
  return StatusCode::SUCCESS;
}

// Execute

StatusCode JEPCMMMaker::execute()
{
  return StatusCode::SUCCESS;
}

// Finalize

StatusCode JEPCMMMaker::finalize()
{
  return StatusCode::SUCCESS;
}


} // end namespace
