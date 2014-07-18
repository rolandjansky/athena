/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


#include <numeric>

#include "GaudiKernel/MsgStream.h"
#include "StoreGate/StoreGateSvc.h"

#include "TrigT1CaloEvent/CMMEtSums.h"
#include "TrigT1CaloEvent/CMMJetHits.h"
#include "TrigT1CaloEvent/CMMRoI.h"
#include "TrigT1CaloEvent/JEMEtSums.h"
#include "TrigT1CaloEvent/JEMHits.h"
#include "TrigT1CaloEvent/JEMRoI.h"
#include "TrigT1CaloEvent/JEPBSCollection.h"
#include "TrigT1CaloEvent/JEPRoIBSCollection.h"
#include "TrigT1CaloEvent/JetElement.h"
#include "TrigT1CaloEvent/JetROI.h"
#include "TrigT1CaloEvent/JetROI_ClassDEF.h"
#include "TrigT1Interfaces/TrigT1CaloDefs.h"

#include "TrigT1CaloSim/JEPCMMMaker.h"

namespace LVL1 {


JEPCMMMaker::JEPCMMMaker(const std::string& name, ISvcLocator* pSvcLocator)
             : Algorithm(name, pSvcLocator),
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
  MsgStream log( msgSvc(), name() );

  StatusCode sc = m_storeGate.retrieve();
  if (sc.isFailure()) {
    log << MSG::ERROR << "Couldn't connect to " << m_storeGate.typeAndName()
        << endreq;
    return sc;
  } else {
    log << MSG::DEBUG << "Connected to " << m_storeGate.typeAndName()
        << endreq;
  }

  if (m_runSimulation) {

    // Retrieve EtSums tool

    sc = m_jepEtSumsTool.retrieve();
    if ( sc.isFailure() ) {
      log << MSG::ERROR << "Couldn't retrieve JEPEtSumsTool" << endreq;
      return sc;
    }

    // Retrieve Hits tool

    sc = m_jepHitsTool.retrieve();
    if ( sc.isFailure() ) {
      log << MSG::ERROR << "Couldn't retrieve JEPHitsTool" << endreq;
      return sc;
    }
  }

  return StatusCode::SUCCESS;
}

// Execute

StatusCode JEPCMMMaker::execute()
{
  MsgStream log( msgSvc(), name() );

  StatusCode sc;

  if (m_runSimulation) {

    // CMM-Jet

    sc = makeCmmJet();
    if (sc.isFailure()) {
      log << MSG::ERROR << "Unable to make CMM-Jet" << endreq;
    }

    // CMM-Energy

    sc = makeCmmEnergy();
    if (sc.isFailure()) {
      log << MSG::ERROR << "Unable to make CMM-Energy" << endreq;
    }

    // JEM RoIs

    sc = makeJemRoi();
    if (sc.isFailure()) {
      log << MSG::ERROR << "Unable to make JEM RoIs" << endreq;
    }

    // CMM RoIs

    sc = makeCmmRoi();
    if (sc.isFailure()) {
      log << MSG::ERROR << "Unable to make CMM RoIs" << endreq;
    }
  }

  // JEP bytestream collection

  sc = makeJepBsCollection();
  if (sc.isFailure()) {
    log << MSG::ERROR << "Unable to make JEP bytestream collection" << endreq;
  }

  // JEP RoI bytestream collection

  sc = makeJepRoiBsCollection();
  if (sc.isFailure()) {
    log << MSG::ERROR << "Unable to make JEP RoI bytestream collection"
        << endreq;
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
  MsgStream log( msgSvc(), name() );

  // Find jet hits

  const JetHitsCollection* hitCollection = 0;
  StatusCode sc;
  if (m_storeGate->contains<JetHitsCollection>(m_jemHitsLocation)) {
    sc = m_storeGate->retrieve(hitCollection, m_jemHitsLocation);
  } else sc = StatusCode::FAILURE;
  if (sc.isFailure() || !hitCollection) {
    log << MSG::DEBUG << "No Jet Hits container found" << endreq;
    hitCollection = 0;
  }

  // Create CMM-Jet container

  CmmJetCollection* cmm = new CmmJetCollection;
  if (hitCollection) m_jepHitsTool->formCMMJetHits(hitCollection, cmm);

  // Save in StoreGate

  sc = m_storeGate->overwrite(cmm, m_cmmJetLocation,true,false,false);
  if (sc.isFailure()) {
    log << MSG::ERROR << "Error recording CMM-Jet container in TDS " << endreq;
    return sc;
  }

  return StatusCode::SUCCESS;
}

// Make CMM-Energy

StatusCode JEPCMMMaker::makeCmmEnergy()
{
  MsgStream log( msgSvc(), name() );

  // Find energy sums

  const EnergySumsCollection* sumsCollection = 0;
  StatusCode sc;
  if (m_storeGate->contains<EnergySumsCollection>(m_jemEtSumsLocation)) {
    sc = m_storeGate->retrieve(sumsCollection, m_jemEtSumsLocation);
  } else sc = StatusCode::FAILURE;
  if (sc.isFailure() || !sumsCollection) {
    log << MSG::DEBUG << "No Energy Sums container found" << endreq;
    sumsCollection = 0;
  }

  // Create CMM-Energy container

  CmmEnergyCollection* cmm = new CmmEnergyCollection;
  if (sumsCollection) m_jepEtSumsTool->formCMMEtSums(sumsCollection, cmm);

  // Save in StoreGate

  sc = m_storeGate->overwrite(cmm, m_cmmEnergyLocation,true,false,false);
  if (sc.isFailure()) {
    log << MSG::ERROR << "Error recording CMM-Energy container in TDS "
        << endreq;
    return sc;
  }

  return StatusCode::SUCCESS;
}

// Make JEM RoI container

StatusCode JEPCMMMaker::makeJemRoi()
{
  MsgStream log( msgSvc(), name() );

  // Create container

  JemRoiCollection* roiCollection = new JemRoiCollection;

  // Find Jet RoIs

  const JetRoiCollection* jrCollection = 0;
  StatusCode sc;
  if (m_storeGate->contains<JetRoiCollection>(m_jetRoiLocation)) {
    sc = m_storeGate->retrieve(jrCollection, m_jetRoiLocation);
  } else sc = StatusCode::FAILURE;
  if (sc.isFailure() || !jrCollection) {
    log << MSG::DEBUG << "No Jet RoIs found" << endreq;
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
    log << MSG::ERROR << "Error recording JEMRoI container in TDS" << endreq;
    return sc;
  }

  return StatusCode::SUCCESS;
}

// Make CMM RoI container

StatusCode JEPCMMMaker::makeCmmRoi()
{
  MsgStream log( msgSvc(), name() );

  // Find Jet-Et RoI in CMM hits

  unsigned int jetEt = 0;
  StatusCode sc;
  const CmmJetCollection* cmmJet = 0;
  if (m_storeGate->contains<CmmJetCollection>(m_cmmJetLocation)) {
    sc = m_storeGate->retrieve(cmmJet, m_cmmJetLocation);
  } else sc = StatusCode::FAILURE;
  if (sc.isFailure() || !cmmJet) {
    log << MSG::DEBUG << "No CMM Hits found" << endreq;
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
    log << MSG::DEBUG << "No CMM Energy Sums found" << endreq;
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
    log << MSG::ERROR << "Error recording CMMRoI container in TDS" << endreq;
    return sc;
  }

  return StatusCode::SUCCESS;
}

// Make JEP bytestream container

StatusCode JEPCMMMaker::makeJepBsCollection()
{
  MsgStream log( msgSvc(), name() );

  // Find jet elements

  const JetElementCollection* jeCollection = 0;
  StatusCode sc;
  if (m_storeGate->contains<JetElementCollection>(m_jetElementLocation)) { 
    sc = m_storeGate->retrieve(jeCollection, m_jetElementLocation);
  } else sc = StatusCode::FAILURE;
  if (sc.isFailure() || !jeCollection) {
    log << MSG::DEBUG << "No Jet Elements found" << endreq;
    jeCollection = 0;
  }

  // Find jet hits

  const JetHitsCollection* hitCollection = 0;
  if (m_storeGate->contains<JetHitsCollection>(m_jemHitsLocation)) {
    sc = m_storeGate->retrieve(hitCollection, m_jemHitsLocation);
  } else sc = StatusCode::FAILURE;
  if (sc.isFailure() || !hitCollection) {
    log << MSG::DEBUG << "No Jet Hits found" << endreq;
    hitCollection = 0;
  }

  // Find Energy Sums

  const EnergySumsCollection* etCollection = 0;
  if (m_storeGate->contains<EnergySumsCollection>(m_jemEtSumsLocation)) {
    sc = m_storeGate->retrieve(etCollection, m_jemEtSumsLocation);
  } else sc = StatusCode::FAILURE;
  if (sc.isFailure() || !etCollection) {
    log << MSG::DEBUG << "No Energy Sums found" << endreq;
    etCollection = 0;
  }

  // Find CMM hits

  const CmmJetCollection* cmmHitCollection = 0;
  if (m_storeGate->contains<CmmJetCollection>(m_cmmJetLocation)) {
    sc = m_storeGate->retrieve(cmmHitCollection, m_cmmJetLocation);
  } else sc = StatusCode::FAILURE;
  if (sc.isFailure() || !cmmHitCollection) {
    log << MSG::DEBUG << "No CMM Hits found" << endreq;
    cmmHitCollection = 0;
  }

  // Find CMM Energy Sums

  const CmmEnergyCollection* cmmEtCollection = 0;
  if (m_storeGate->contains<CmmEnergyCollection>(m_cmmEnergyLocation)) {
    sc = m_storeGate->retrieve(cmmEtCollection, m_cmmEnergyLocation);
  } else sc = StatusCode::FAILURE;
  if (sc.isFailure() || !cmmEtCollection) {
    log << MSG::DEBUG << "No CMM Energy Sums found" << endreq;
    cmmEtCollection = 0;
  }

  // Create JEP container

  JEPBSCollection* jep = new JEPBSCollection(jeCollection, hitCollection,
                                             etCollection, cmmHitCollection,
				                           cmmEtCollection);
  sc = m_storeGate->overwrite(jep, m_jepBsCollectionLocation,true,false,false);
  if (sc != StatusCode::SUCCESS) {
    log << MSG::ERROR << "Error recording JEP container in TDS " << endreq;
    return sc;
  }

  return StatusCode::SUCCESS;
}

// Make JEP RoI bytestream container

StatusCode JEPCMMMaker::makeJepRoiBsCollection()
{
  MsgStream log( msgSvc(), name() );

  // Find JEMRoIs

  const JemRoiCollection* jeCollection = 0;
  StatusCode sc;
  if (m_storeGate->contains<JemRoiCollection>(m_jemRoiLocation)) {
    sc = m_storeGate->retrieve(jeCollection, m_jemRoiLocation);
  } else sc = StatusCode::FAILURE;
  if (sc.isFailure() || !jeCollection) {
    log << MSG::DEBUG << "No JEMRoIs found" << endreq;
    jeCollection = 0;
  }

  // Find CMMRoI

  const CMMRoI* cr = 0;
  if (m_storeGate->contains<CMMRoI>(m_cmmRoiLocation)) {
    sc = m_storeGate->retrieve(cr, m_cmmRoiLocation);
  } else sc = StatusCode::FAILURE;
  if (sc.isFailure() || !cr ) {
    log << MSG::DEBUG << "No CMMRoI found" << endreq;
    cr = 0;
  }

  // Find CMM hits

  const CmmJetCollection* cmmHitCollection = 0;
  if (m_storeGate->contains<CmmJetCollection>(m_cmmJetLocation)) {
    sc = m_storeGate->retrieve(cmmHitCollection, m_cmmJetLocation);
  } else sc = StatusCode::FAILURE;
  if (sc.isFailure() || !cmmHitCollection) {
    log << MSG::DEBUG << "No CMM Hits found" << endreq;
    cmmHitCollection = 0;
  }

  // Find CMM Energy Sums

  const CmmEnergyCollection* cmmEtCollection = 0;
  if (m_storeGate->contains<CmmEnergyCollection>(m_cmmEnergyLocation)) {
    sc = m_storeGate->retrieve(cmmEtCollection, m_cmmEnergyLocation);
  } else sc = StatusCode::FAILURE;
  if (sc.isFailure() || !cmmEtCollection) {
    log << MSG::DEBUG << "No CMM Energy Sums found" << endreq;
    cmmEtCollection = 0;
  }

  // Create JEP RoI container

  JEPRoIBSCollection* jep = new JEPRoIBSCollection(jeCollection, cr,
                                        cmmHitCollection, cmmEtCollection);
  sc = m_storeGate->overwrite(jep, m_jepRoiBsCollectionLocation,true,false,false);
  if (sc != StatusCode::SUCCESS) {
    log << MSG::ERROR << "Error recording JEP RoI container in TDS" << endreq;
    return sc;
  }

  return StatusCode::SUCCESS;
}

} // end namespace
