/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


#include <numeric>

#include "StoreGate/StoreGateSvc.h"

#include "TrigT1CaloEvent/CMMEtSums.h"
#include "TrigT1CaloEvent/CMMJetHits.h"
#include "TrigT1CaloEvent/CMMRoI.h"
#include "TrigT1CaloEvent/JEMEtSums.h"
#include "TrigT1CaloEvent/JEMHits.h"
#include "TrigT1CaloEvent/JEMRoI.h"
#include "TrigT1CaloEvent/JEPBSCollectionV1.h"
#include "TrigT1CaloEvent/JEPRoIBSCollectionV1.h"
#include "TrigT1CaloEvent/JetElement.h"
#include "TrigT1CaloEvent/JetROI.h"
#include "TrigT1CaloEvent/JetROI_ClassDEF.h"
#include "TrigT1Interfaces/TrigT1CaloDefs.h"

#include "TrigT1CaloSim/JEPCMMMaker.h"

namespace LVL1 {


JEPCMMMaker::JEPCMMMaker(const std::string& name, ISvcLocator* pSvcLocator)
             : AthAlgorithm(name, pSvcLocator),
	       m_storeGate("StoreGateSvc", name),
	       m_jepEtSumsTool("LVL1::L1JEPEtSumsTools/L1JEPEtSumsTools"),
	       m_jepHitsTool("LVL1::L1JEPHitsTools/L1JEPHitsTools")
{
  declareProperty("EventStore",m_storeGate,"StoreGate Service");
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
  

  StatusCode sc = m_storeGate.retrieve();
  if (sc.isFailure()) {
    ATH_MSG_ERROR( "Couldn't connect to " << m_storeGate.typeAndName()
        );
    return sc;
  } else {
    ATH_MSG_DEBUG( "Connected to " << m_storeGate.typeAndName()
        );
  }

  if (m_runSimulation) {

    // Retrieve EtSums tool

    sc = m_jepEtSumsTool.retrieve();
    if ( sc.isFailure() ) {
      ATH_MSG_ERROR( "Couldn't retrieve JEPEtSumsTool" );
      return sc;
    }

    // Retrieve Hits tool

    sc = m_jepHitsTool.retrieve();
    if ( sc.isFailure() ) {
      ATH_MSG_ERROR( "Couldn't retrieve JEPHitsTool" );
      return sc;
    }
  }

  return StatusCode::SUCCESS;
}

// Execute

StatusCode JEPCMMMaker::execute()
{
  

  StatusCode sc;

  if (m_runSimulation) {

    // CMM-Jet

    sc = makeCmmJet();
    if (sc.isFailure()) {
      ATH_MSG_ERROR( "Unable to make CMM-Jet" );
    }

    // CMM-Energy

    sc = makeCmmEnergy();
    if (sc.isFailure()) {
      ATH_MSG_ERROR( "Unable to make CMM-Energy" );
    }

    // JEM RoIs

    sc = makeJemRoi();
    if (sc.isFailure()) {
      ATH_MSG_ERROR( "Unable to make JEM RoIs" );
    }

    // CMM RoIs

    sc = makeCmmRoi();
    if (sc.isFailure()) {
      ATH_MSG_ERROR( "Unable to make CMM RoIs" );
    }
  }

  // JEP bytestream collection

  sc = makeJepBsCollection();
  if (sc.isFailure()) {
    ATH_MSG_ERROR( "Unable to make JEP bytestream collection" );
  }

  // JEP RoI bytestream collection

  sc = makeJepRoiBsCollection();
  if (sc.isFailure()) {
    ATH_MSG_ERROR( "Unable to make JEP RoI bytestream collection"
        );
  }

  return StatusCode::SUCCESS;
}

// Finalize

StatusCode JEPCMMMaker::finalize()
{

  return StatusCode::SUCCESS;
}

// Make CMM-Jet

StatusCode JEPCMMMaker::makeCmmJet()
{
  

  // Find jet hits

  const JetHitsCollection* hitCollection = 0;
  StatusCode sc;
  if (m_storeGate->contains<JetHitsCollection>(m_jemHitsLocation)) {
    sc = m_storeGate->retrieve(hitCollection, m_jemHitsLocation);
  } else sc = StatusCode::FAILURE;
  if (sc.isFailure() || !hitCollection) {
    ATH_MSG_DEBUG( "No Jet Hits container found" );
    hitCollection = 0;
  }

  // Create CMM-Jet container

  CmmJetCollection* cmm = new CmmJetCollection;
  if (hitCollection) m_jepHitsTool->formCMMJetHits(hitCollection, cmm);

  // Save in StoreGate

  sc = m_storeGate->overwrite(cmm, m_cmmJetLocation,true,false,false);
  if (sc.isFailure()) {
    ATH_MSG_ERROR( "Error recording CMM-Jet container in TDS " );
    return sc;
  }

  return StatusCode::SUCCESS;
}

// Make CMM-Energy

StatusCode JEPCMMMaker::makeCmmEnergy()
{
  

  // Find energy sums

  const EnergySumsCollection* sumsCollection = 0;
  StatusCode sc;
  if (m_storeGate->contains<EnergySumsCollection>(m_jemEtSumsLocation)) {
    sc = m_storeGate->retrieve(sumsCollection, m_jemEtSumsLocation);
  } else sc = StatusCode::FAILURE;
  if (sc.isFailure() || !sumsCollection) {
    ATH_MSG_DEBUG( "No Energy Sums container found" );
    sumsCollection = 0;
  }

  // Create CMM-Energy container

  CmmEnergyCollection* cmm = new CmmEnergyCollection;
  if (sumsCollection) m_jepEtSumsTool->formCMMEtSums(sumsCollection, cmm);

  // Save in StoreGate

  sc = m_storeGate->overwrite(cmm, m_cmmEnergyLocation,true,false,false);
  if (sc.isFailure()) {
    ATH_MSG_ERROR( "Error recording CMM-Energy container in TDS "
        );
    return sc;
  }

  return StatusCode::SUCCESS;
}

// Make JEM RoI container

StatusCode JEPCMMMaker::makeJemRoi()
{
  

  // Create container

  JemRoiCollection* roiCollection = new JemRoiCollection;

  // Find Jet RoIs

  const JetRoiCollection* jrCollection = 0;
  StatusCode sc;
  if (m_storeGate->contains<JetRoiCollection>(m_jetRoiLocation)) {
    sc = m_storeGate->retrieve(jrCollection, m_jetRoiLocation);
  } else sc = StatusCode::FAILURE;
  if (sc.isFailure() || !jrCollection) {
    ATH_MSG_DEBUG( "No Jet RoIs found" );
  }
  else {
    // Create JEMRoIs and add to container
    JetRoiCollection::const_iterator pos  = jrCollection->begin();
    JetRoiCollection::const_iterator pose = jrCollection->end();
    for (; pos != pose; ++pos) {
      roiCollection->push_back(new JEMRoI((*pos)->roiWord()));
    }
  }

  // Save container

  sc = m_storeGate->overwrite(roiCollection, m_jemRoiLocation,true,false,false);
  if (sc != StatusCode::SUCCESS) {
    ATH_MSG_ERROR( "Error recording JEMRoI container in TDS" );
    return sc;
  }

  return StatusCode::SUCCESS;
}

// Make CMM RoI container

StatusCode JEPCMMMaker::makeCmmRoi()
{
  

  // Find Jet-Et RoI in CMM hits

  unsigned int jetEt = 0;
  StatusCode sc;
  const CmmJetCollection* cmmJet = 0;
  if (m_storeGate->contains<CmmJetCollection>(m_cmmJetLocation)) {
    sc = m_storeGate->retrieve(cmmJet, m_cmmJetLocation);
  } else sc = StatusCode::FAILURE;
  if (sc.isFailure() || !cmmJet) {
    ATH_MSG_DEBUG( "No CMM Hits found" );
  } else {
    CmmJetCollection::const_iterator icmmj  = cmmJet->begin();
    CmmJetCollection::const_iterator icmmjE = cmmJet->end();
    for (; icmmj != icmmjE; ++icmmj) {
      if ((*icmmj)->dataID() == CMMJetHits::ET_MAP) {
        jetEt = (*icmmj)->Hits();
        break;
      }
    }
  }

  // Find Energy RoIs in CMM sums

  unsigned int ex = 0;
  unsigned int ey = 0;
  unsigned int et = 0;
  int exError = 0;
  int eyError = 0;
  int etError = 0;
  unsigned int sumEt = 0;
  unsigned int missingEt = 0;
  unsigned int missingEtSig = 0;
  const CmmEnergyCollection* cmmEnergy = 0;
  if (m_storeGate->contains<CmmEnergyCollection>(m_cmmEnergyLocation)) {
    sc = m_storeGate->retrieve(cmmEnergy, m_cmmEnergyLocation);
  } else sc = StatusCode::FAILURE;
  if (sc.isFailure() || !cmmEnergy) {
    ATH_MSG_DEBUG( "No CMM Energy Sums found" );
  } else {
    CmmEnergyCollection::const_iterator icmme  = cmmEnergy->begin();
    CmmEnergyCollection::const_iterator icmmeE = cmmEnergy->end();
    for (; icmme != icmmeE; ++icmme) {
      if ((*icmme)->dataID() == CMMEtSums::TOTAL) {
        ex = (*icmme)->Ex();
        ey = (*icmme)->Ey();
        et = (*icmme)->Et();
        exError = (*icmme)->ExError();
        eyError = (*icmme)->EyError();
        etError = (*icmme)->EtError();
      } else if ((*icmme)->dataID() == CMMEtSums::SUM_ET_MAP) {
        sumEt = (*icmme)->Et();
      } else if ((*icmme)->dataID() == CMMEtSums::MISSING_ET_MAP) {
        missingEt = (*icmme)->Et();
      } else if ((*icmme)->dataID() == CMMEtSums::MISSING_ET_SIG_MAP) {
        missingEtSig = (*icmme)->Et();
      }
    }
  }

  // Fill CMMRoI

  CMMRoI* roi = new CMMRoI(jetEt, sumEt, missingEt, missingEtSig, ex, ey, et,
                           0, 0, 0, 0, exError, eyError, etError);

  // Save container

  sc = m_storeGate->overwrite(roi, m_cmmRoiLocation,true,false,false);
  if (sc != StatusCode::SUCCESS) {
    ATH_MSG_ERROR( "Error recording CMMRoI container in TDS" );
    return sc;
  }

  return StatusCode::SUCCESS;
}

// Make JEP bytestream container

StatusCode JEPCMMMaker::makeJepBsCollection()
{
  

  // Find jet elements

  const JetElementCollection* jeCollection = 0;
  StatusCode sc;
  if (m_storeGate->contains<JetElementCollection>(m_jetElementLocation)) { 
    sc = m_storeGate->retrieve(jeCollection, m_jetElementLocation);
  } else sc = StatusCode::FAILURE;
  if (sc.isFailure() || !jeCollection) {
    ATH_MSG_DEBUG( "No Jet Elements found" );
    jeCollection = 0;
  }

  // Find jet hits

  const JetHitsCollection* hitCollection = 0;
  if (m_storeGate->contains<JetHitsCollection>(m_jemHitsLocation)) {
    sc = m_storeGate->retrieve(hitCollection, m_jemHitsLocation);
  } else sc = StatusCode::FAILURE;
  if (sc.isFailure() || !hitCollection) {
    ATH_MSG_DEBUG( "No Jet Hits found" );
    hitCollection = 0;
  }

  // Find Energy Sums

  const EnergySumsCollection* etCollection = 0;
  if (m_storeGate->contains<EnergySumsCollection>(m_jemEtSumsLocation)) {
    sc = m_storeGate->retrieve(etCollection, m_jemEtSumsLocation);
  } else sc = StatusCode::FAILURE;
  if (sc.isFailure() || !etCollection) {
    ATH_MSG_DEBUG( "No Energy Sums found" );
    etCollection = 0;
  }

  // Find CMM hits

  const CmmJetCollection* cmmHitCollection = 0;
  if (m_storeGate->contains<CmmJetCollection>(m_cmmJetLocation)) {
    sc = m_storeGate->retrieve(cmmHitCollection, m_cmmJetLocation);
  } else sc = StatusCode::FAILURE;
  if (sc.isFailure() || !cmmHitCollection) {
    ATH_MSG_DEBUG( "No CMM Hits found" );
    cmmHitCollection = 0;
  }

  // Find CMM Energy Sums

  const CmmEnergyCollection* cmmEtCollection = 0;
  if (m_storeGate->contains<CmmEnergyCollection>(m_cmmEnergyLocation)) {
    sc = m_storeGate->retrieve(cmmEtCollection, m_cmmEnergyLocation);
  } else sc = StatusCode::FAILURE;
  if (sc.isFailure() || !cmmEtCollection) {
    ATH_MSG_DEBUG( "No CMM Energy Sums found" );
    cmmEtCollection = 0;
  }

  // Create JEP container

  JEPBSCollectionV1* jep = new JEPBSCollectionV1(jeCollection, hitCollection,
                                             etCollection, cmmHitCollection,
				                           cmmEtCollection);
  sc = m_storeGate->overwrite(jep, m_jepBsCollectionLocation,true,false,false);
  if (sc != StatusCode::SUCCESS) {
    ATH_MSG_ERROR( "Error recording JEP container in TDS " );
    return sc;
  }

  return StatusCode::SUCCESS;
}

// Make JEP RoI bytestream container

StatusCode JEPCMMMaker::makeJepRoiBsCollection()
{
  

  // Find JEMRoIs

  const JemRoiCollection* jeCollection = 0;
  StatusCode sc;
  if (m_storeGate->contains<JemRoiCollection>(m_jemRoiLocation)) {
    sc = m_storeGate->retrieve(jeCollection, m_jemRoiLocation);
  } else sc = StatusCode::FAILURE;
  if (sc.isFailure() || !jeCollection) {
    ATH_MSG_DEBUG( "No JEMRoIs found" );
    jeCollection = 0;
  }

  // Find CMMRoI

  const CMMRoI* cr = 0;
  if (m_storeGate->contains<CMMRoI>(m_cmmRoiLocation)) {
    sc = m_storeGate->retrieve(cr, m_cmmRoiLocation);
  } else sc = StatusCode::FAILURE;
  if (sc.isFailure() || !cr ) {
    ATH_MSG_DEBUG( "No CMMRoI found" );
    cr = 0;
  }

  // Find CMM hits

  const CmmJetCollection* cmmHitCollection = 0;
  if (m_storeGate->contains<CmmJetCollection>(m_cmmJetLocation)) {
    sc = m_storeGate->retrieve(cmmHitCollection, m_cmmJetLocation);
  } else sc = StatusCode::FAILURE;
  if (sc.isFailure() || !cmmHitCollection) {
    ATH_MSG_DEBUG( "No CMM Hits found" );
    cmmHitCollection = 0;
  }

  // Find CMM Energy Sums

  const CmmEnergyCollection* cmmEtCollection = 0;
  if (m_storeGate->contains<CmmEnergyCollection>(m_cmmEnergyLocation)) {
    sc = m_storeGate->retrieve(cmmEtCollection, m_cmmEnergyLocation);
  } else sc = StatusCode::FAILURE;
  if (sc.isFailure() || !cmmEtCollection) {
    ATH_MSG_DEBUG( "No CMM Energy Sums found" );
    cmmEtCollection = 0;
  }

  // Create JEP RoI container

  JEPRoIBSCollectionV1* jep = new JEPRoIBSCollectionV1(jeCollection, cr,
                                        cmmHitCollection, cmmEtCollection);
  sc = m_storeGate->overwrite(jep, m_jepRoiBsCollectionLocation,true,false,false);
  if (sc != StatusCode::SUCCESS) {
    ATH_MSG_ERROR( "Error recording JEP RoI container in TDS" );
    return sc;
  }

  return StatusCode::SUCCESS;
}

} // end namespace
