/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigT1ZDC.h"
#include "TrigT1Interfaces/ZdcCTP.h"
#include "TrigT1Interfaces/TrigT1StoreGateKeys.h"
#include "TrigConfL1Data/ThresholdConfig.h"

// Needed for truth based trigger simulation.
#include "GeneratorObjects/McEventCollection.h"
#include "GaudiKernel/IPartPropSvc.h"
#include "GaudiKernel/ITHistSvc.h"
#include "AtlasHepMC/GenParticle.h"

#include <bitset>

namespace LVL1 {

  TrigT1ZDC::TrigT1ZDC(const std::string& name,
		       ISvcLocator* pSvcLocator): AthAlgorithm(name, pSvcLocator), 
						  m_configSvc("TrigConf::LVL1ConfigSvc/LVL1ConfigSvc", name),
						  m_zdcContainerName("ZDC"),
						  m_threshold_c(-1),
						  m_threshold_a(-1),
						  m_cablestart_a(-1),
						  m_cablestart_c(-1),	
						  m_cablestart_ac(-1),	
						  m_passedA(0),
						  m_passedC(0),
						  m_passedAC(0)
  {
    declareProperty("LVL1ConfigSvc", m_configSvc, "LVL1 Config Service");
    declareProperty("ZdcContainerName", m_zdcContainerName = "ZDC");

    declareProperty("McEventCollectionKey",  m_mcEventCollectionKey = "TruthEvent");
    
    declareProperty("ZDCIncludeLHCf", m_zdcIncludeLHCf = true);
  }

  StatusCode TrigT1ZDC::initialize() {
     if(msgLvl(MSG::INFO)) msg(MSG::INFO) << "Initialising" << endmsg;

    StatusCode sc;

     sc = Algorithm::initialize();
    if (sc.isFailure()) {
       if(msgLvl(MSG::ERROR)) msg(MSG::ERROR) << "Couldn't initialize Algorithm base class." << endmsg;
       return sc;
    }


    // Connect to the Detector Store to retrieve ZDC identifier helper.
    sc = detStore().retrieve();
    if (sc.isFailure()) {
      if(msgLvl(MSG::ERROR)) msg(MSG::ERROR) << "Couldn't connect to " << detStore().typeAndName()
					     << endmsg;
      return sc;
    } 
    else if(msgLvl(MSG::DEBUG)) {
      msg(MSG::DEBUG) << "Connected to " << detStore().typeAndName()
		      << endmsg;
    }
    
    // Connect to the LVL1ConfigSvc to retrieve threshold settings.
    sc = m_configSvc.retrieve();
    if (sc.isFailure()) {
      if(msgLvl(MSG::ERROR)) msg(MSG::ERROR) << "Couldn't connect to " << m_configSvc.typeAndName() 
					      << endmsg;
      return sc;
    } 
    else if(msgLvl(MSG::DEBUG)) {
      msg(MSG::DEBUG) << "Connected to " << m_configSvc.typeAndName() 
		      << endmsg;
    }

    // Connect to StoreGate service to retrieve input ZDC
    // trigger simulation container.
    sc = evtStore().retrieve();
    if (sc.isFailure()) {
      if(msgLvl(MSG::ERROR)) msg(MSG::ERROR) << "Couldn't connect to " << evtStore().typeAndName() 
					     << endmsg;
      return sc;
    } 
    else if(msgLvl(MSG::DEBUG)) {
      msg(MSG::DEBUG) << "Connected to " <<  evtStore().typeAndName() 
		      << endmsg;
    }
    
    // Get level 1 ZDC threshold settings from the level 1
    // configuration service.  The thresholds are in GeV and are not a
    // ptcut.  The ptcut() methods just returns the value in the xml file.
    std::vector<TrigConf::TriggerThreshold*> thresholds = m_configSvc->thresholdConfig()->getZdcThresholdVector();
    std::vector<TrigConf::TriggerThreshold*>::iterator th_itr = thresholds.begin();
    std::vector<TrigConf::TriggerThreshold*>::iterator th_itr_end = thresholds.end();
    for(;th_itr!=th_itr_end;th_itr++) {

      // Set all three triggers to the same threshold
      // (The level 1 configuration service provides three thresholds
      // but it is likely that the hardware implementation will only
      // allow 1 threshold setting.)
      if((*th_itr)->name() == "ZDC_A") {
	m_threshold_a = (*th_itr)->triggerThresholdValue(0, 0)->ptcut(); // threshold in GeV
	m_cablestart_a = (*th_itr)->cableStart();
	if(msgLvl(MSG::INFO)) msg(MSG::INFO) << "m_threshold_a=" << m_threshold_a << " m_cablestart_a=" << m_cablestart_a << endmsg;
      }
      else if((*th_itr)->name() == "ZDC_C") {
	m_threshold_c = (*th_itr)->triggerThresholdValue(0, 0)->ptcut(); // threshold in GeV
	m_cablestart_c = (*th_itr)->cableStart();
	if(msgLvl(MSG::INFO)) msg(MSG::INFO) << "m_threshold_c=" << m_threshold_c << " m_cablestart_c=" << m_cablestart_c << endmsg;
      }
      else if((*th_itr)->name() == "ZDC_AND") { //Legacy
        m_cablestart_ac = (*th_itr)->cableStart();
      	if(msgLvl(MSG::INFO)) msg(MSG::INFO) << "(legacy ZDC coincidence) m_cablestart_ac=" << m_cablestart_ac << endmsg;
      }
    }

   if(msgLvl(MSG::INFO) &&  m_zdcIncludeLHCf) msg(MSG::INFO) << "LHCf Included, NO photons taken by ZDC" << endmsg;
   if(msgLvl(MSG::INFO) && !m_zdcIncludeLHCf) msg(MSG::INFO) << "LHCf Not Included, photons taken by ZDC" << endmsg;
    
    //Zero internal ZDC counters
    m_passedA = 0; m_passedC = 0; m_passedAC = 0;

    if(m_threshold_a == -1) {
      if(msgLvl(MSG::WARNING)) msg(MSG::WARNING) << "ZDC_A Lvl 1 threshold not set.  Trigger will be disabled." << endmsg;
      m_threshold_a = 10000000;
    }
    if(m_threshold_c == -1) {
      if(msgLvl(MSG::WARNING)) msg(MSG::WARNING) << "ZDC_C Lvl 1 threshold not set.  Trigger will be disabled." << endmsg;
      m_threshold_c = 10000000;
    }

    if(m_threshold_a != m_threshold_c) {
      if(msgLvl(MSG::WARNING)) msg(MSG::WARNING) << "ZDC_A and ZDC_C Lvl 1 thresholds are not equal.  Are you sure you want to do that?" << endmsg;
    }

    // The ZDC simulation is currently not ready for use.  Use truth
    // information rather than simulated ZDC RAW signals.

    // get the Particle Properties Service
    IPartPropSvc* partPropSvc = 0;
    sc =  service("PartPropSvc", partPropSvc, true);
    if (sc.isFailure()) {
      msg(MSG::FATAL) << " Could not initialize Particle Properties Service" << endmsg;
      return sc;
    }

    return StatusCode::SUCCESS;
  }

  StatusCode TrigT1ZDC::execute() {
    if(msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << "execute()" << endmsg;
    
    StatusCode sc;

    // The ZDC simulation is currently not in working order.
    // Therefore just use long lived neutrals within the ZDC
    // acceptance.

    double energyECC = 0, energyECA = 0;
    int particlesProcessed = 0;

    if (evtStore()->contains<McEventCollection>(m_mcEventCollectionKey)) {  //Do not run if no truth has been found, all triggers zero.
      const McEventCollection *mcEventCollection = 0;
      sc = evtStore()->retrieve(mcEventCollection, m_mcEventCollectionKey);
      if( sc.isFailure()  || !mcEventCollection ) {
        if(msgLvl(MSG::ERROR)) msg(MSG::ERROR) << "Error retrieving " << m_mcEventCollectionKey << endmsg;
        return sc;
      }
    
      // Loop over MC GenParticles contained in McEventCollection
      McEventCollection::const_iterator mcCollection_itr = mcEventCollection->begin();
      McEventCollection::const_iterator mcCollection_itr_end = mcEventCollection->end();
      HepMC::GenEvent::particle_const_iterator hepmc_part_itr;
      HepMC::GenEvent::vertex_const_iterator hepmc_vtx_itr;
      HepMC::GenVertex *end_vtx;

      // Loop over the McEventCollections
      for (; mcCollection_itr != mcCollection_itr_end; mcCollection_itr++) {
      
        // Loop over the particles and build up the particle-index map.  
        for (hepmc_part_itr = (*mcCollection_itr)->particles_begin(); 
	     hepmc_part_itr != (*mcCollection_itr)->particles_end();
	     hepmc_part_itr++) {

	  //Stop after 500 particles (avoid crash on long truth logs)
	  if(particlesProcessed > 500) break;
	  particlesProcessed++;
	
	  // Exclude Geant particles
	  if((*hepmc_part_itr)->barcode() >= 200000) continue;
	
	  // Require stable particles, to avoid the top of the generator record.
	  if((*hepmc_part_itr)->status() != 1) continue;
    
	  // Require neutral hadronic particles which are able to penetrate LHCf and the BRAN.
	  // This consists of neutrons along with long lived neutral mesons & baryons with
	  // sufficient Lorentz boost to travel the 140m to the TAN where the ZDC is located.
	  if     (abs((*hepmc_part_itr)->pdg_id()) == 2112) {} // Neutron
	  else if(    (*hepmc_part_itr)->pdg_id()  == 22   && !m_zdcIncludeLHCf) {} // Photon (Only without LHCf)
	  else if(    (*hepmc_part_itr)->pdg_id()  == 130  && fabs((*hepmc_part_itr)->momentum().pz()) > 4500) {} // Boosed K_L0
	  else if(abs((*hepmc_part_itr)->pdg_id()) == 3122 && fabs((*hepmc_part_itr)->momentum().pz()) > 2e6) {} // Boosted Lambda_0
	  else if(abs((*hepmc_part_itr)->pdg_id()) == 3322 && fabs((*hepmc_part_itr)->momentum().pz()) > 2.1e6) {} // Boosted Xi_0
	  else continue;
	
	  // Check if the particle points at the ZDC.
	  if(fabs((*hepmc_part_itr)->momentum().pseudoRapidity()) < 8.3) continue;
	
	  // If the particle has an end vertex check that it reaches the TAN.
	  end_vtx = (*hepmc_part_itr)->end_vertex();
	  if(end_vtx) {
	    if(fabs(end_vtx->position().z())/1000.0 < 140.) continue;
	  }
	
	  // Increment the energy deposited in this side of the ZDC
	  if((*hepmc_part_itr)->momentum().pz() < 0) {
	    energyECC += (*hepmc_part_itr)->momentum().e();
	  }
	  else {
	    energyECA += (*hepmc_part_itr)->momentum().e();
	  }
        }
      }
    }
    
    // Pack the word
    unsigned int cableWord = 0;
    
    if(m_cablestart_a >= 0 && (energyECA/1000.0) > m_threshold_a) {
      cableWord += 1 << m_cablestart_a; //Default: bit 17 (decimal 131072)
      m_passedA++;
    }
    
    if(m_cablestart_c >= 0 && (energyECC/1000.0) > m_threshold_c) {
      cableWord += 1 << m_cablestart_c; //Default: bit 18 (decimal 262144)
      m_passedC++;
    }
    
    if(m_cablestart_ac >= 0 && (energyECC/1000.0) > m_threshold_c && (energyECA/1000.0) > m_threshold_a) {
      cableWord += 1 << m_cablestart_ac; //Default: bit 19 (decimal 524288)
      m_passedAC++;
    }
    
    // Retrieve Lvl1 ZDC container
    /*containerName = m_zdcContainerName;
      m_zdcContainer = 0;
      sc=m_storeGate->retrieve( m_zdcContainer, containerName);
      if( sc.isFailure()  || !m_zdcContainer ) {
      if(!m_badDataFound) {
      msg(MSG::WARNING) << containerName << " not found" << endmsg;
      msg(MSG::WARNING) << "The ZDC trigger will be disabled for the rest of the run." << endmsg;
      }
      else {
      m_badDataFound = true;
      }
      return StatusCode::SUCCESS;
      }
      else {
      msg(MSG::DEBUG) << containerName << " Container Successfully Retrieved" 
      << endmsg;
      }*/
    
    // Logic handling trigger simulation goes here...
    
    if(msgLvl(MSG::DEBUG)) {
      msg(MSG::DEBUG) << "ZDC Energy    : C side = " << energyECC/1000. << "(GeV),  A side = " << energyECA/1000. << "(GeV)" << endmsg;
      msg(MSG::DEBUG) << "ZDC Cable Word: " << std::bitset<std::numeric_limits<unsigned int>::digits>(cableWord) << endmsg;
    }

    // Record the CTP trigger word in StoreGate.
    ZdcCTP *zdcCTP = new ZdcCTP(cableWord);
    if(msgLvl(MSG::DEBUG)) {
      zdcCTP->dump();
    }
    
    
    sc=evtStore()->record(zdcCTP, LVL1::DEFAULT_ZdcCTPLocation, false);
    if(sc.isFailure()) {
      if(msgLvl(MSG::ERROR)) msg(MSG::ERROR) << "Failed to register " << LVL1::DEFAULT_ZdcCTPLocation << endmsg;
      return sc;
    } 
    else if(msgLvl(MSG::DEBUG)) {
      msg(MSG::DEBUG) << LVL1::DEFAULT_ZdcCTPLocation << " registered successfully "
		      << endmsg;
    }
    
    return StatusCode::SUCCESS;
  }
  
  StatusCode TrigT1ZDC::finalize() {
    msg(MSG::INFO) << "ZDC Finalize:  m_passedA:" << m_passedA << ", m_passedC:" << m_passedC << ", m_passedAC:" << m_passedAC << endmsg;
    return StatusCode::SUCCESS;
  }
}
