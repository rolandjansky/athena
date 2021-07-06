// Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration


#include "TrigT1MBTS.h"
#include "TrigT1Interfaces/MbtsCTP.h"
#include "TrigConfL1Data/ThresholdConfig.h"
#include "TrigT1Interfaces/TrigT1StoreGateKeys.h"
#include "TrigConfData/L1Menu.h"



LVL1::TrigT1MBTS::TrigT1MBTS(const std::string& name, ISvcLocator* pSvcLocator)
   : AthAlgorithm(name, pSvcLocator),
   m_thresholdNumber{{0,1,2,3,4,5,6,7}, {8,9,10,11,12,13,14,15}},
   m_thresholdNumber12{{0,1,2,3,4,5,6,7}, {8,1000,9,1000,10,1000,11,1000}} // in Run 2 the outer MBTS were reduced to 4
{} 

StatusCode
LVL1::TrigT1MBTS::initialize()
{
   ATH_MSG_INFO("Initialising TrigT1MBTS, name = " << name());
   ATH_MSG_INFO("UseNewConfig set to " <<  (m_useNewConfig ? "True" : "False"));

   ATH_CHECK(AthAlgorithm::initialize());

   m_thresholds_a.clear();
   m_thresholds_c.clear();
   m_thresholds_short_a.clear();
   m_thresholds_short_c.clear();
   m_thresholds_a.resize(16,0);// set the threshold to zero here -
   // override again further down
   m_thresholds_c.resize(16,0);// set the threshold to zero here -
   // override again further down
   m_thresholds_short_a.resize(12,0);// set the threshold to zero here -
   // override again further down
   m_thresholds_short_c.resize(12,0);// set the threshold to zero here -
   // override again further down
   m_cablestarts_a.clear();
   m_cablestarts_c.clear();
   m_cablestarts_a.resize(16,0);// default is bit 0
   m_cablestarts_c.resize(16,0);// default is bit 0

   // Connect to the LVL1ConfigSvc to retrieve threshold settings.
   ATH_CHECK(m_configSvc.retrieve());

   // Retrieve TileTBID helper from det store
   // (The MBTS was added to the Test Beam (TB) list.)
   ATH_CHECK(detStore()->retrieve(m_tileTBID));

   const TrigConf::L1Menu * l1menu = nullptr;
   if(m_useNewConfig) {
      // Run 3 menu 
      ATH_CHECK( detStore()->retrieve(l1menu) );
   }

   if(m_useNewConfig) {
      // Run 3 menu 
      // MBTS
      for (std::shared_ptr<TrigConf::L1Threshold> thr : l1menu->thresholds("MBTS")) {
         if(thr->name() != "MBTS_A" && thr->name() != "MBTS_C") {
            continue;
         }
         std::string connName = l1menu->connectorNameFromThreshold(thr->name());
         unsigned int startbit = l1menu->connector(connName).triggerLine(thr->name()).startbit();
         m_ThrVecSize12 = true; // TODO: check for (thresholds[0]->thresholdValueVector().size() == 12);
         std::vector<float> hwThrValues; // TODO need to add configuration access in TrigConfData/Threshold.h
         if(thr->name() == "MBTS_A") {
            m_cablestart_a = startbit;
            if(m_ThrVecSize12) {
               m_thresholds_short_a = hwThrValues;
            } else {
               m_thresholds_a = hwThrValues;
            }
         } else {
            m_cablestart_c = startbit;
            if(m_ThrVecSize12) {
               m_thresholds_short_c = hwThrValues;
            } else {
               m_thresholds_c = hwThrValues;
            }
         }
      }
      
      // MBTSSI
      for (std::shared_ptr<TrigConf::L1Threshold> thr : l1menu->thresholds("MBTSSI")) {
         //m_singleCounterInputs = true;
         std::string thrname = thr->name();
         // figure out module number from threshold name
         size_t module = std::stoi(thrname.substr(6));
         float  hwValue = 0; // TODO implement access
         std::string connName = l1menu->connectorNameFromThreshold(thr->name());
         unsigned int startbit = l1menu->connector(connName).triggerLine(thr->name()).startbit();
         ATH_MSG_INFO("Read " << thrname << " with voltage " << hwValue << "mV at bit " << startbit << " on " << connName);
         // Get the discriminator threshold settings (single inputs) for the C side.
         bool isCSide = thrname.find("MBTS_C")==0;
         if(isCSide) {
            if(module >= m_thresholds_c.size()) {
               ATH_MSG_WARNING("Module number " << module << " on side C out of range");
            } else {
               m_thresholds_c[module] = hwValue;
               m_cablestarts_c[module] = startbit;
            }
         } else if(thrname.find("MBTS_A")==0 && thrname.size()>6) {
            // Get the discriminator threshold settings (single inputs) for the A side.
            // figure out module number from threshold name
            if(module >= m_thresholds_a.size()) {
               ATH_MSG_WARNING("Module number " << module << " on side A out of range");
            } else {
               m_thresholds_a[module] = hwValue;
               m_cablestarts_a[module] = startbit;
            }
         }
      }

   } else {
      // Get level 1 MBTS threshold settings from the level 1
      // configuration service for the discriminators on the
      // multiplicity inputs.  All discriminators are assumed to have
      // the same setting.  The thresholds are in mV and are not a
      // ptcut.  The ptcut() methods just returns the value in the xml
      // file.
      const std::vector<TrigConf::TriggerThreshold*> & thresholds = m_configSvc->thresholdConfig()->getMbtsThresholdVector();
      m_ThrVecSize12 = (thresholds[0]->thresholdValueVector().size() == 12);
      ATH_MSG_INFO("Size of thresholdValueVector: " << thresholds[0]->thresholdValueVector().size());
      for(const TrigConf::TriggerThreshold* thr : thresholds) {

         // Get the discriminator threshold settings (multiplicity input) for the C side.
         if( thr->name() == "MBTS_C" ) {
            const std::vector<TrigConf::TriggerThresholdValue*>& thrValues = thr->thresholdValueVector();

            for(size_t ii = 0; ii<thrValues.size(); ++ii) {
               float hwThresholdValue = thrValues[ii]->ptcut();

               if(m_ThrVecSize12) {
                  m_thresholds_short_c[ii] = hwThresholdValue;
               } else {
                  m_thresholds_c[ii] = hwThresholdValue;
               }
               ATH_MSG_INFO( "Multiplicity input side C, counter " << ii << ", read threshold in mV of " << hwThresholdValue);
            }
            m_cablestart_c = thr->cableStart();
            ATH_MSG_DEBUG("CableStart: " << m_cablestart_c << ", CableEnd: " << thr->cableEnd());

         } else if( thr->name() == "MBTS_A" ) {

            // Get the discriminator threshold settings (multiplicity input) for the A side.
            const std::vector<TrigConf::TriggerThresholdValue*>& thrValues = thr->thresholdValueVector();

            for(size_t ii = 0; ii<thrValues.size();++ii) {
               float hwThresholdValue = thrValues[ii]->ptcut();
               if(m_ThrVecSize12) {
                  m_thresholds_short_a[ii] = hwThresholdValue;
               } else {
                  m_thresholds_a[ii] = hwThresholdValue;
               }
               ATH_MSG_INFO("Multiplicity input side A, counter " << ii << ", read threshold in mV of " << hwThresholdValue);
            }
            m_cablestart_a = thr->cableStart();
            ATH_MSG_DEBUG("CableStart: " << m_cablestart_a << ", CableEnd: " << thr->cableEnd());
         }
      }

      // Get level 1 MBTS threshold settings from the level 1
      // configuration service for the discriminators on the single
      // inputs.  There are 32 physical discriminator inputs these can
      // only be set to one value each.  These are the same 32
      // discriminators used for the multiplicity inputs.  Each one the
      // thresholds are in mV and are not a ptcut.  The ptcut() methods
      // just returns the value in the xml file.
      ATH_MSG_DEBUG( "Size of MbtssiThresholdValueVector: " << m_configSvc->thresholdConfig()->getMbtssiThresholdVector().size());
      for(const TrigConf::TriggerThreshold* thr : m_configSvc->thresholdConfig()->getMbtssiThresholdVector()) {
         //m_singleCounterInputs = true;
         std::string thrname = thr->name();

         // Get the discriminator threshold settings (single inputs) for the C side.
         if(thrname.find("MBTS_C")==0 && thrname.size()>6) {
            // figure out module number from threshold name
            ATH_MSG_INFO("Single input threshold name " << thrname);
            thrname.replace(thrname.find("MBTS_C"),6,"");
            size_t module = std::stoi(thrname);
            ATH_MSG_INFO(" converts to Module number " << module);
            if(module >= m_thresholds_c.size()) {
               ATH_MSG_WARNING("Module number " << module << " on side C out of range");
            } else {
               m_thresholds_c[module] = thr->triggerThresholdValue(0, 0)->ptcut();
               m_cablestarts_c[module] = thr->cableStart();
               ATH_MSG_INFO(", read threshold in mV of " << m_thresholds_c[module]);
               ATH_MSG_DEBUG("CableStart: " << m_cablestarts_c[module] << ", CableEnd: " << thr->cableEnd());
            }
         } else if(thrname.find("MBTS_A")==0 && thrname.size()>6) {
            // Get the discriminator threshold settings (single inputs) for the A side.
            // figure out module number from threshold name
            ATH_MSG_INFO("Single input threshold name " << thrname);
            thrname.replace(thrname.find("MBTS_A"),6,"");
            size_t module = std::stoi(thrname);
               ATH_MSG_DEBUG(", moduel number " << module);
            if(module >= m_thresholds_a.size()) {
               ATH_MSG_WARNING("Module number " << module << " on side A out of range");
            } else {
               m_thresholds_a[module] = thr->triggerThresholdValue(0, 0)->ptcut();
               m_cablestarts_a[module] = thr->cableStart();
               ATH_MSG_INFO(", read threshold in mV of " << m_thresholds_a[module]);
               ATH_MSG_DEBUG("CableStart: " << m_cablestarts_a[module] << ", CableEnd: " << thr->cableEnd());
            }
         }
      }
   }

   // MBTS_A, MBTS_C or MBTS_A, MBTS_C, MBTS_0, MBTS_1,...,MBTS_15 are used.
   // Therefore thess messages are just INFO rather than warning. 
   if(m_thresholds_a.size() != 16) {
      ATH_MSG_INFO("MBTS_A Lvl 1 single input thresholds not set. Triggers will be disabled.");
      m_thresholds_a.clear();
      for(size_t i=0;i<16;i++) {
         m_thresholds_a.push_back(10000000);
      }
   }
   if(m_thresholds_c.size() != 16) {
      ATH_MSG_INFO("MBTS_C Lvl 1 single input thresholds not set. Triggers will be disabled.");
      m_thresholds_c.clear();
      for(size_t i=0;i<16;i++) {
         m_thresholds_c.push_back(10000000);
      }
   }

   // Print out the state of this algorithm
   if(msgLvl(MSG::INFO)) {
      msg(MSG::INFO) << "=================== Settings ==================" << endmsg;
      msg(MSG::INFO) << "TileTTL1ContainerName = " << m_tileTTL1ContainerName << endmsg;
      msg(MSG::INFO) << "Sample t0 bin index = " << m_tZeroBin << endmsg;
      msg(MSG::INFO) << "CFD fraction constant = " << m_CFD_fraction << endmsg;
      if(m_ThrVecSize12){
         msg(MSG::INFO) << "C side thresholds for single inputs = {";
         for(size_t i=0;i<12;i++) {
            msg(MSG::INFO) << m_thresholds_short_c[i];
            if(i<11) msg(MSG::INFO) << ",";
         }
         msg(MSG::INFO) << "}" << endmsg;
         msg(MSG::INFO) << "A side thresholds for single inputs = {";
         for(size_t i=0;i<12;i++) {
            msg(MSG::INFO) << m_thresholds_short_a[i];
            if(i<11) msg(MSG::INFO) << ",";
         }
         msg(MSG::INFO) << "}" << endmsg;
         if(msgLvl(MSG::INFO)) {
            msg(MSG::DEBUG) << "thresholdNumber12: ";
            for(size_t j=0;j<2;j++) {
               for(size_t i=0;i<8;i++) {
                  msg(MSG::DEBUG) << m_thresholdNumber12[j][i];
                  if(i<7) msg(MSG::DEBUG) << ",";
               }
               msg(MSG::DEBUG) << endmsg;
            }
         }
      } else {
         msg(MSG::INFO) << "C side thresholds for single inputs = {";
         for(size_t i=0;i<16;i++) {
            msg(MSG::INFO) << m_thresholds_c[i];
            if(i<15) msg(MSG::INFO) << ",";
         }
         msg(MSG::INFO) << "}" << endmsg;
         msg(MSG::INFO) << "A side thresholds for single inputs = {";
         for(size_t i=0;i<16;i++) {
            msg(MSG::INFO) << m_thresholds_a[i];
            if(i<15) msg(MSG::INFO) << ",";
         }
         msg(MSG::INFO) << "}" << endmsg;
      }	
         
      msg(MSG::DEBUG) << "thresholdNumber: " << endmsg;
      for(size_t j=0;j<2;j++) {
         for(size_t i=0;i<8;i++) {
            msg(MSG::DEBUG) << m_thresholdNumber[j][i];
            if(i<7) msg(MSG::DEBUG) << ",";
         }
         msg(MSG::DEBUG) << endmsg;
      }
      msg(MSG::INFO) << "===============================================" << endmsg;
   }
   return StatusCode::SUCCESS;
}

StatusCode
LVL1::TrigT1MBTS::execute()
{
   // Retrieve the TileTTL1 container that contains the input MBTS information.
   const TileTTL1Container* tileTTL1MBTSContainer{nullptr};
   StatusCode sc = evtStore()->retrieve(tileTTL1MBTSContainer, m_tileTTL1ContainerName);
   if( sc.isFailure() || !tileTTL1MBTSContainer ) {
      ATH_MSG_WARNING(m_tileTTL1ContainerName << " not found. This event will be skipped.");
      return StatusCode::SUCCESS;
   }

   // Check for previous bug: there should be 24 or less depending on
   // the configuration of the Tile algorithm that creates the
   // TileTTL1MBTS container.
   if(tileTTL1MBTSContainer->size() > 32) {
      if(!m_badDataFound) {
         ATH_MSG_WARNING("BAD DATA!!!  tileTTL1MBTSContainer->size() = " << tileTTL1MBTSContainer->size());
         ATH_MSG_WARNING("There should be no more than 24 TileTTL1MBTS elements in one event.");
         ATH_MSG_WARNING("This event will be skipped.  Any further bad data will be skipped.");
         m_badDataFound = true;
      }
      return StatusCode::SUCCESS;
   }

   unsigned int triggersEBA = 0; // Number of triggers in EBA
   unsigned int triggersEBC = 0; // Number of triggers in EBC
   unsigned int single_triggers_A = 0;
   unsigned int single_triggers_C = 0;
   
   // Loop over all Lvl 1 MBTS trigger paddles
   for(const TileTTL1 * ttl1_mbts : *tileTTL1MBTSContainer) {
      
      // Find out which MBTS paddle this is.
      Identifier id = ttl1_mbts->identify();
      if (!m_tileTBID->is_tiletb(id)) {
         ATH_MSG_ERROR("This is not an MBTS identifier!");
         return StatusCode::FAILURE;
      }

      int phi = m_tileTBID->phi(id);
      if(phi < 0 || phi > 7) {
         ATH_MSG_ERROR("Phi value " << phi << " is out of range!");
         return StatusCode::FAILURE;
      }

      int channel = m_tileTBID->channel(id);
      if(channel < 0 || channel > 1) {
         ATH_MSG_ERROR("Channel value " << channel << " is out of range!");
         return StatusCode::FAILURE;
      }

      int detSide = m_tileTBID->type(id);
      if(detSide != -1 && detSide != 1) {
         ATH_MSG_ERROR("Side value " << detSide << " is out of range!");
         return StatusCode::FAILURE;
      }

      // Retrieve the MBTS signal samples.
      const std::vector<double> & samples = ttl1_mbts->samples();
      unsigned int numSamples = samples.size();
      if(m_tZeroBin >= numSamples) {
         ATH_MSG_ERROR("t0 bin index " << m_tZeroBin << " is greater than the number of bins " << numSamples);
         return StatusCode::FAILURE;
      }

      ATH_MSG_DEBUG("Sample [" << m_tZeroBin << "]=" << samples[m_tZeroBin]);
      /* 
      // Only the even counters are used for RunII
      if(channel == 1 && phi > 7 && phi%2 != 0) {
         ATH_MSG_DEBUG("Out counter " << phi << " is not used for RunII! This counter will be skipped!");
         continue;
      }
      */
      // Single input triggers.
      unsigned int thresholdIndex = m_thresholdNumber[channel][phi];
      unsigned int thresholdIndex12 = m_thresholdNumber12[channel][phi];
      if(thresholdIndex > (unsigned int)m_thresholds_c.size() || thresholdIndex > (unsigned int)m_thresholds_a.size()) {
         ATH_MSG_ERROR("Threshold index \"" << thresholdIndex << "\" for single triggers is out of range.");
         return StatusCode::FAILURE;
      }

      if(thresholdIndex12 == 1000){
         ATH_MSG_DEBUG("this is a needless counter for run2, the sample will be skipped!");
         continue;
      }
      
      // Emulate CFD samples[m_tZeroBin]*m_CFD_fraction
      float ThrValue_a = 0;
      float ThrValue_c = 0;
      if(m_ThrVecSize12) {
         ThrValue_a = m_thresholds_short_a[thresholdIndex12]; 
	      ThrValue_c = m_thresholds_short_c[thresholdIndex12]; 
      } else {
         ThrValue_a = m_thresholds_a[thresholdIndex]; 
         ThrValue_c = m_thresholds_c[thresholdIndex]; 
      }

      if((samples[m_tZeroBin]*m_CFD_fraction > ThrValue_c && detSide == -1) ||
         (samples[m_tZeroBin]*m_CFD_fraction > ThrValue_a && detSide == 1)) 
      {
         // Add the trigger bit to the correct trigger word
         if(detSide == -1)
         { // EBC
            single_triggers_C += (1<<m_cablestarts_c[thresholdIndex]);
            triggersEBC++; // Increment the number of EBC triggers
         }
         else if (detSide == 1)
         { // EBA
            single_triggers_A += (1<<m_cablestarts_a[thresholdIndex]);
            triggersEBA++; // Increment the number of EBA triggers.
         }

         ATH_MSG_DEBUG( "Single input triggered on sample " << m_tZeroBin << " of " << numSamples << " bins.");
      }
   }

   // Cropping to 3 bits
   if (triggersEBA>7) triggersEBA=7;
   if (triggersEBC>7) triggersEBC=7;

   ATH_MSG_DEBUG( "Multis: "<< triggersEBA <<" and "<< triggersEBC );

   unsigned int cableWordA = single_triggers_A + (triggersEBA<<m_cablestart_a);
   unsigned int cableWordC = single_triggers_C + (triggersEBC<<m_cablestart_c);

   // Record the CTP trigger word in StoreGate.
   MbtsCTP *mbtsACTP = new MbtsCTP(cableWordA);
   MbtsCTP *mbtsCCTP = new MbtsCTP(cableWordC);

   // Methods used in CTPsimulation are added for testing
   ATH_MSG_DEBUG( " (in CTPSimulation) mbtsA cable word 0 is: 0x" << std::hex << std::setw( 8 ) << std::setfill( '0' ) << mbtsACTP->cableWord0()  );
   ATH_MSG_DEBUG( " (in CTPSimulation) Mult of mbtsA is: "<< static_cast<int>( (mbtsACTP->cableWord0() >> m_cablestart_a) & static_cast<unsigned int>( pow( 2, 3 ) - 1 ) )  );
   ATH_MSG_DEBUG( " mbtsC cable " << mbtsCCTP->print()  );
   ATH_MSG_DEBUG( " (in CTPSimulation) mbtsC cable word 0 is: 0x" << std::hex << std::setw( 8 ) << std::setfill( '0' ) << mbtsCCTP->cableWord0()  );
   ATH_MSG_DEBUG( " (in CTPSimulation) Mult of mbtsC is: "<< static_cast<int>( (mbtsCCTP->cableWord0() >> m_cablestart_c) & static_cast<unsigned int>( pow( 2, 3 ) - 1 ) )  );

   ATH_CHECK(evtStore()->record(mbtsACTP, DEFAULT_MbtsACTPLocation, false));
   ATH_CHECK(evtStore()->record(mbtsCCTP, DEFAULT_MbtsCCTPLocation, false));
   
   return StatusCode::SUCCESS;
}

