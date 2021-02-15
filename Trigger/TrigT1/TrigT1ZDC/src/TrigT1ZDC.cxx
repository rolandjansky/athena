/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigT1ZDC.h"
#include "TrigT1Interfaces/ZdcCTP.h"
#include "TrigT1Interfaces/TrigT1StoreGateKeys.h"
#include "TrigConfL1Data/ThresholdConfig.h"
#include "TrigConfData/L1Menu.h"

// Needed for truth based trigger simulation.
#include "GeneratorObjects/McEventCollection.h"
#include "GaudiKernel/IPartPropSvc.h"
#include "GaudiKernel/ITHistSvc.h"
#include "AtlasHepMC/GenParticle.h"

#include <bitset>


LVL1::TrigT1ZDC::TrigT1ZDC(const std::string& name, ISvcLocator* pSvcLocator)
   : AthAlgorithm(name, pSvcLocator)
{}

StatusCode
LVL1::TrigT1ZDC::initialize()
{
   ATH_MSG_INFO("Initialising");

   StatusCode sc;
   ATH_CHECK(Algorithm::initialize());
   
   // Connect to the LVL1ConfigSvc to retrieve threshold settings.
   ATH_CHECK(m_configSvc.retrieve());

   if(m_useNewConfig) {
      // Run 3 menu 
      const TrigConf::L1Menu * l1menu = nullptr;
      ATH_CHECK( detStore()->retrieve(l1menu) );
      for (std::shared_ptr<TrigConf::L1Threshold> thr : l1menu->thresholds("ZDC")) {
         std::string connName = l1menu->connectorNameFromThreshold(thr->name());
         unsigned int startbit = l1menu->connector(connName).triggerLine(thr->name()).startbit();
         // Set all three triggers to the same threshold
         // (The level 1 configuration service provides three thresholds
         // but it is likely that the hardware implementation will only
         // allow 1 threshold setting.)
         if(thr->name() == "ZDC_A") {
            m_threshold_a = thr->thrValue(); // threshold in GeV
            m_cablestart_a = startbit;
            ATH_MSG_INFO("m_threshold_a=" << m_threshold_a << " m_cablestart_a=" << startbit);
         } else if(thr->name() == "ZDC_C") {
            m_threshold_c = thr->thrValue(); // threshold in GeV
            m_cablestart_c = startbit;
            ATH_MSG_INFO("m_threshold_c=" << m_threshold_c << " m_cablestart_c=" << startbit);
         } else if(thr->name() == "ZDC_AND") { //Legacy
            m_cablestart_ac = startbit;
            ATH_MSG_INFO("(legacy ZDC coincidence) m_cablestart_ac=" << startbit);
         }
      }
   
   } else {
      // Get level 1 ZDC threshold settings from the level 1
      // configuration service.  The thresholds are in GeV and are not a
      // ptcut.  The ptcut() methods just returns the value in the xml file.
      for(const TrigConf::TriggerThreshold* thr : m_configSvc->thresholdConfig()->getZdcThresholdVector()) {
         // Set all three triggers to the same threshold
         // (The level 1 configuration service provides three thresholds
         // but it is likely that the hardware implementation will only
         // allow 1 threshold setting.)
         if(thr->name() == "ZDC_A") {
            m_threshold_a = thr->triggerThresholdValue(0, 0)->ptcut(); // threshold in GeV
            m_cablestart_a = thr->cableStart();
            ATH_MSG_INFO("m_threshold_a=" << m_threshold_a << " m_cablestart_a=" << m_cablestart_a);
         } else if(thr->name() == "ZDC_C") {
            m_threshold_c = thr->triggerThresholdValue(0, 0)->ptcut(); // threshold in GeV
            m_cablestart_c = thr->cableStart();
            ATH_MSG_INFO("m_threshold_c=" << m_threshold_c << " m_cablestart_c=" << m_cablestart_c);
         } else if(thr->name() == "ZDC_AND") { //Legacy
            m_cablestart_ac = thr->cableStart();
            ATH_MSG_INFO("(legacy ZDC coincidence) m_cablestart_ac=" << m_cablestart_ac);
         }
      }
   }
   if(m_zdcIncludeLHCf) {
      ATH_MSG_INFO("LHCf Included, NO photons taken by ZDC");
   } else {
      ATH_MSG_INFO("LHCf Not Included, photons taken by ZDC");
   }
   
   //Zero internal ZDC counters
   m_passedA = 0;
   m_passedC = 0;
   m_passedAC = 0;

   if(m_threshold_a == -1) {
      ATH_MSG_WARNING( "ZDC_A Lvl 1 threshold not set.  Trigger will be disabled." );
      m_threshold_a = 10000000;
   }
   if(m_threshold_c == -1) {
      ATH_MSG_WARNING( "ZDC_C Lvl 1 threshold not set.  Trigger will be disabled." );
      m_threshold_c = 10000000;
   }

   if(m_threshold_a != m_threshold_c) {
      ATH_MSG_WARNING( "ZDC_A and ZDC_C Lvl 1 thresholds are not equal.  Are you sure you want to do that?" );
   }

   // The ZDC simulation is currently not ready for use.  Use truth
   // information rather than simulated ZDC RAW signals.

   // get the Particle Properties Service
   IPartPropSvc* partPropSvc = 0;
   ATH_CHECK(service("PartPropSvc", partPropSvc, true));

   return StatusCode::SUCCESS;
}

StatusCode
LVL1::TrigT1ZDC::execute() {
   ATH_MSG_DEBUG("execute()" );
   
   StatusCode sc;

   // The ZDC simulation is currently not in working order.
   // Therefore just use long lived neutrals within the ZDC
   // acceptance.

   double energyECC{0}, energyECA{0};
   int particlesProcessed{0};

   if (evtStore()->contains<McEventCollection>(m_mcEventCollectionKey)) {  //Do not run if no truth has been found, all triggers zero.
      const McEventCollection *mcEventCollection = 0;
      sc = evtStore()->retrieve(mcEventCollection, m_mcEventCollectionKey);
      if( sc.isFailure()  || !mcEventCollection ) {
         ATH_MSG_ERROR( "Error retrieving " << m_mcEventCollectionKey );
         return sc;
      }
   
      // Loop over MC GenParticles contained in McEventCollection
      McEventCollection::const_iterator mcCollection_itr = mcEventCollection->begin();
      McEventCollection::const_iterator mcCollection_itr_end = mcEventCollection->end();

      // Loop over the McEventCollections
      for (; mcCollection_itr != mcCollection_itr_end; mcCollection_itr++) {
      
         // Loop over the particles and build up the particle-index map.  
         for (auto hepmc_part:  *(*mcCollection_itr)) {

            //Stop after 500 particles (avoid crash on long truth logs)
            if(particlesProcessed > 500) {
               break;
            }
            particlesProcessed++;
            
            // Exclude Geant particles
            if(HepMC::barcode(hepmc_part) >= 200000) {
               continue;
            }
            
            // Require stable particles, to avoid the top of the generator record.
            if(hepmc_part->status() != 1) {
               continue;
            }
            
            // Require neutral hadronic particles which are able to penetrate LHCf and the BRAN.
            // This consists of neutrons along with long lived neutral mesons & baryons with
            // sufficient Lorentz boost to travel the 140m to the TAN where the ZDC is located.
            if (std::abs(hepmc_part->pdg_id()) == 2112) {} // Neutron
            else if(hepmc_part->pdg_id()  == 22 && !m_zdcIncludeLHCf) {} // Photon (Only without LHCf)
            else if(hepmc_part->pdg_id()  == 130  && std::abs(hepmc_part->momentum().pz()) > 4500) {} // Boosed K_L0
            else if(std::abs(hepmc_part->pdg_id()) == 3122 && std::abs(hepmc_part->momentum().pz()) > 2e6) {} // Boosted Lambda_0
            else if(std::abs(hepmc_part->pdg_id()) == 3322 && std::abs(hepmc_part->momentum().pz()) > 2.1e6) {} // Boosted Xi_0
            else {
               continue;
            }
      
            // Check if the particle points at the ZDC.
            if(std::abs(hepmc_part->momentum().pseudoRapidity()) < 8.3) {
               continue;
            }

            // If the particle has an end vertex check that it reaches the TAN.
            auto end_vtx = hepmc_part->end_vertex();
            if(end_vtx) {
               if(std::abs(end_vtx->position().z())/1000.0 < 140.) continue;
            }
            
            // Increment the energy deposited in this side of the ZDC
            if(hepmc_part->momentum().pz() < 0) {
               energyECC += hepmc_part->momentum().e();
            } else {
               energyECA += hepmc_part->momentum().e();
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
   

   // Logic handling trigger simulation goes here...

   ATH_MSG_DEBUG("ZDC Energy    : C side = " << energyECC/1000. << "(GeV),  A side = " << energyECA/1000. << "(GeV)" );
   ATH_MSG_DEBUG("ZDC Cable Word: " << std::bitset<std::numeric_limits<unsigned int>::digits>(cableWord) );

   // Record the CTP trigger word in StoreGate.
   ZdcCTP *zdcCTP = new ZdcCTP(cableWord);
   if(msgLvl(MSG::DEBUG)) {
      zdcCTP->dump();
   }

   ATH_CHECK(evtStore()->record(zdcCTP, LVL1::DEFAULT_ZdcCTPLocation, false));
   return StatusCode::SUCCESS;
}

StatusCode 
LVL1::TrigT1ZDC::finalize()
{
   ATH_MSG_INFO( "ZDC Finalize:  m_passedA:" << m_passedA << ", m_passedC:" << m_passedC << ", m_passedAC:" << m_passedAC );
   return StatusCode::SUCCESS;
}

