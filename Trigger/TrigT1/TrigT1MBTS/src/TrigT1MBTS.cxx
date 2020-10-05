/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigT1MBTS.h"
#include "TrigT1Interfaces/MbtsCTP.h"
#include "TrigConfL1Data/ThresholdConfig.h"
#include "TrigT1Interfaces/TrigT1StoreGateKeys.h"
#include "boost/lexical_cast.hpp"

namespace LVL1 {

  //---------------------------------------------------------------------

  TrigT1MBTS::TrigT1MBTS(const std::string& name,
			 ISvcLocator* pSvcLocator): AthAlgorithm(name, pSvcLocator), 
						    m_configSvc("TrigConf::LVL1ConfigSvc/LVL1ConfigSvc", name),
						    m_tileTBID(0),
						    m_tileTTL1ContainerName("TileTTL1MBTS"),
						    m_tileTTL1MBTS(0),
						    m_thresholds_a(),
						    m_thresholds_short_a(),
						    m_thresholds_c(),
						    m_thresholds_short_c(),
						    m_cablestart_a(0),
						    m_cablestart_c(0),
						    m_cablestarts_a(),
						    m_cablestarts_c(),
						    m_thresholdNumber(),
						    m_thresholdNumber12(),
						    m_tZeroBin(3),
						    m_CFD_fraction(0.9),
						    m_badDataFound(false),
						    m_ThrVecSize12(false)
                                                    //m_singleCounterInputs(false)
  {

    // Defining the mapping of scintillator counters to threshold.
    unsigned int counter = 0;
    std::vector<unsigned int> phi(0);
    for (int i=0;i<2;i++) {
      phi.clear();
      for (int j=0;j<8;j++) {
	if(i==1 && j%2!=0) phi.push_back(1000);
	else{
	  phi.push_back(counter);
	  counter++;
	}
      }
      m_thresholdNumber12.push_back(phi);
    }
    
    counter = 0;
    for(int i=0;i<2;i++){
      phi.clear();
      for(int j=0;j<8;j++){
        phi.push_back(counter);
        counter++;
      }   
      m_thresholdNumber.push_back(phi);
    } 

    declareProperty("LVL1ConfigSvc", m_configSvc, "LVL1 Config Service");
    declareProperty("TileTTL1ContainerName", m_tileTTL1ContainerName = "TileTTL1MBTS");
    declareProperty("TzeroBin", m_tZeroBin = 3);
  }

  //---------------------------------------------------------------------

  StatusCode TrigT1MBTS::initialize() {
    int i;
    int j;

    if (msgLvl(MSG::INFO)) {
      msg(MSG::INFO) << "Initialising TrigT1MBTS, name = " << name() << endmsg;
    }

    if (AthAlgorithm::initialize().isFailure()) {
      if (msgLvl(MSG::ERROR)) msg(MSG::ERROR) << "Couldn't initialize Algorithm base class." << endmsg;
      return StatusCode::FAILURE;
    }

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
    StatusCode sc = m_configSvc.retrieve();
    if(sc.isFailure()) {
      if (msgLvl(MSG::ERROR)) msg(MSG::ERROR) << "Couldn't connect to " 
					      << m_configSvc.typeAndName() 
					      << endmsg;
      return StatusCode::FAILURE;
    } 
    else if(msgLvl(MSG::INFO)) {
      msg(MSG::INFO) << "Connected to " << m_configSvc.typeAndName() 
		      << endmsg;
    }
 
    // Retrieve TileTBID helper from det store
    // (The MBTS was added to the Test Beam (TB) list.)
    sc = detStore()->retrieve(m_tileTBID);
    if (sc.isFailure()) {
      if (msgLvl(MSG::ERROR)) msg(MSG::ERROR) << "Unable to retrieve TileTBID helper from DetectorStore" << endmsg;
      return sc;
    }

    std::vector<TrigConf::TriggerThreshold*> thresholds;
    std::vector<TrigConf::TriggerThreshold*>::iterator th_itr;
    std::vector<TrigConf::TriggerThreshold*>::iterator th_itr_end;
    
    // Get level 1 MBTS threshold settings from the level 1
    // configuration service for the discriminators on the
    // multiplicity inputs.  All discriminators are assumed to have
    // the same setting.  The thresholds are in mV and are not a
    // ptcut.  The ptcut() methods just returns the value in the xml
    // file.
    thresholds = m_configSvc->thresholdConfig()->getMbtsThresholdVector();
    th_itr = thresholds.begin();
    th_itr_end = thresholds.end();

    if((*th_itr)->thresholdValueVector().size() == 12) m_ThrVecSize12 = true;
    if(msgLvl(MSG::INFO)) msg(MSG::INFO) << "Size of thresholdValueVector: " << (*th_itr)->thresholdValueVector().size() << endmsg;

    for(;th_itr!=th_itr_end;th_itr++) {

      // Get the discriminator threshold settings (multiplicity input) for the C side.
      if((*th_itr)->name() == "MBTS_C") {
	 const std::vector<TrigConf::TriggerThresholdValue*>& thrValues = (*th_itr)->thresholdValueVector();

	 for(size_t ii = 0; ii<thrValues.size();++ii) {
	    float hwThresholdValue = thrValues[ii]->ptcut();

	    if(m_ThrVecSize12) m_thresholds_short_c[ii] = hwThresholdValue;
	    else m_thresholds_c[ii] = hwThresholdValue;

	    if (msgLvl(MSG::INFO)) msg(MSG::INFO) << "Multiplicity input side C, counter " << ii 
						  << ", read threshold in mV of " << hwThresholdValue << endmsg;
	 }
	 m_cablestart_c = (*th_itr)->cableStart();
	 if(msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << "CableStart: " << m_cablestart_c;
	 if(msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << ", CableEnd: " << (*th_itr)->cableEnd() << endmsg;
      }

      // Get the discriminator threshold settings (multiplicity input) for the A side.
      else if((*th_itr)->name() == "MBTS_A") {
	 const std::vector<TrigConf::TriggerThresholdValue*>& thrValues = (*th_itr)->thresholdValueVector();

	 for(size_t ii = 0; ii<thrValues.size();++ii) {
	    float hwThresholdValue = thrValues[ii]->ptcut();

	    if(m_ThrVecSize12) m_thresholds_short_a[ii] = hwThresholdValue;
	    else m_thresholds_a[ii] = hwThresholdValue;

	    if (msgLvl(MSG::INFO)) msg(MSG::INFO) << "Multiplicity input side A, counter " << ii 
	 					 << ", read threshold in mV of " << hwThresholdValue << endmsg;
	 }
	 m_cablestart_a = (*th_itr)->cableStart();
	 if(msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << "CableStart: " << m_cablestart_a;
	 if(msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << ", CableEnd: " << (*th_itr)->cableEnd() << endmsg;
      }
      
    }

    // Get level 1 MBTS threshold settings from the level 1
    // configuration service for the discriminators on the single
    // inputs.  There are 32 physical discriminator inputs these can
    // only be set to one value each.  These are the same 32
    // discriminators used for the multiplicity inputs.  Each one the
    // thresholds are in mV and are not a ptcut.  The ptcut() methods
    // just returns the value in the xml file.
    thresholds = m_configSvc->thresholdConfig()->getMbtssiThresholdVector();
    th_itr = thresholds.begin();
    th_itr_end = thresholds.end();
    std::string thrname;
    if(msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << "Size of MbtssiThresholdValueVector: " << thresholds.size() << endmsg;
    for(;th_itr!=th_itr_end;th_itr++) {
      //m_singleCounterInputs = true;
      thrname = (*th_itr)->name();

      // Get the discriminator threshold settings (single inputs) for the C side.
      if(thrname.find("MBTS_C")==0 && thrname.size()>6) {
	// figure out module number from threshold name
	if(msgLvl(MSG::INFO)) msg(MSG::INFO) << "Single input threshold name " << thrname;
	thrname.replace(thrname.find("MBTS_C"),6,"");
	size_t module = boost::lexical_cast<size_t,std::string>(thrname);
        if(msgLvl(MSG::INFO)) msg(MSG::INFO) << " converts to Moduel number " << module;
	if(module >= m_thresholds_c.size()) {
	   if(msgLvl(MSG::WARNING)) msg(MSG::WARNING) << endmsg;
	   if(msgLvl(MSG::WARNING)) msg(MSG::WARNING) << "Module number " << module << " on side C out of range" << endmsg;
	} else {
	   m_thresholds_c[module] = (*th_itr)->triggerThresholdValue(0, 0)->ptcut();
	   m_cablestarts_c[module] = (*th_itr)->cableStart();
	   if(msgLvl(MSG::INFO)) msg(MSG::INFO) << ", read threshold in mV of " << m_thresholds_c[module] << endmsg;
	   if(msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << "CableStart: " << m_cablestarts_c[module];
	   if(msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << ", CableEnd: " << (*th_itr)->cableEnd() << endmsg;
	}
      }

      // Get the discriminator threshold settings (single inputs) for the A side.
      else if(thrname.find("MBTS_A")==0 && thrname.size()>6) {
	// figure out module number from threshold name
	if(msgLvl(MSG::INFO)) msg(MSG::INFO) << "Single input threshold name " << thrname;
	thrname.replace(thrname.find("MBTS_A"),6,"");
	size_t module = boost::lexical_cast<size_t,std::string>(thrname);
        if(msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << ", moduel number " << module;
	if(module >= m_thresholds_a.size()) {
	   if(msgLvl(MSG::WARNING)) msg(MSG::WARNING) << endmsg;
	   if(msgLvl(MSG::WARNING)) msg(MSG::WARNING) << "Module number " << module << " on side A out of range" << endmsg;
	} else {
	   m_thresholds_a[module] = (*th_itr)->triggerThresholdValue(0, 0)->ptcut();
	   m_cablestarts_a[module] = (*th_itr)->cableStart();
	   if(msgLvl(MSG::INFO)) msg(MSG::INFO) << ", read threshold in mV of " << m_thresholds_a[module] << endmsg;
	   if(msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << "CableStart: " << m_cablestarts_a[module];
	   if(msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << ", CableEnd: " << (*th_itr)->cableEnd() << endmsg;
	}
      }
    }


    // MBTS_A, MBTS_C or MBTS_A, MBTS_C, MBTS_0, MBTS_1,...,MBTS_15 are used.
    // Therefore thess messages are just INFO rather than warning. 
    if(m_thresholds_a.size() != 16) {
      if(msgLvl(MSG::INFO)) msg(MSG::INFO) << "MBTS_A Lvl 1 single input thresholds not set.  Triggers will be disabled." << endmsg;
      m_thresholds_a.clear();
      for(i=0;i<16;i++) m_thresholds_a.push_back(10000000);
    }
    if(m_thresholds_c.size() != 16) {
      if(msgLvl(MSG::INFO)) msg(MSG::INFO) << "MBTS_C Lvl 1 single input thresholds not set.  Triggers will be disabled." << endmsg;
      m_thresholds_c.clear();
      for(i=0;i<16;i++) m_thresholds_c.push_back(10000000);
    }

    // Print out the state of this algorithm
    if(msgLvl(MSG::INFO)) {
      msg(MSG::INFO) << "=================== Settings ==================" << endmsg;
      msg(MSG::INFO) << "TileTTL1ContainerName = " << m_tileTTL1ContainerName << endmsg;
      msg(MSG::INFO) << "Sample t0 bin index = " << m_tZeroBin << endmsg;
      msg(MSG::INFO) << "CFD fraction constant = " << m_CFD_fraction << endmsg;
      if(m_ThrVecSize12){
        msg(MSG::INFO) << "C side thresholds for single inputs = {";
        for(i=0;i<12;i++) {
          msg(MSG::INFO) << m_thresholds_short_c[i];
          if(i<11) msg(MSG::INFO) << ",";
        }
        msg(MSG::INFO) << "}" << endmsg;
        msg(MSG::INFO) << "A side thresholds for single inputs = {";
        for(i=0;i<12;i++) {
          msg(MSG::INFO) << m_thresholds_short_a[i];
          if(i<11) msg(MSG::INFO) << ",";
        }
        msg(MSG::INFO) << "}" << endmsg;

        msg(MSG::DEBUG) << "thresholdNumber12: " << endmsg;
        for(j=0;j<2;j++) {
          for(i=0;i<8;i++) {
            msg(MSG::DEBUG) << m_thresholdNumber12[j][i];
            if(i<7) msg(MSG::DEBUG) << ",";
          }
          msg(MSG::DEBUG) << endmsg;
        }
      }

      else{
        msg(MSG::INFO) << "C side thresholds for single inputs = {";
        for(i=0;i<16;i++) {
          msg(MSG::INFO) << m_thresholds_c[i];
          if(i<15) msg(MSG::INFO) << ",";
        }
        msg(MSG::INFO) << "}" << endmsg;
        msg(MSG::INFO) << "A side thresholds for single inputs = {";
        for(i=0;i<16;i++) {
          msg(MSG::INFO) << m_thresholds_a[i];
          if(i<15) msg(MSG::INFO) << ",";
        }
        msg(MSG::INFO) << "}" << endmsg;
      }	
         
      msg(MSG::DEBUG) << "thresholdNumber: " << endmsg;
      for(j=0;j<2;j++) {
        for(i=0;i<8;i++) {
          msg(MSG::DEBUG) << m_thresholdNumber[j][i];
          if(i<7) msg(MSG::DEBUG) << ",";
        }
        msg(MSG::DEBUG) << endmsg;
      }
      msg(MSG::INFO) << "===============================================" << endmsg;
    }


    return StatusCode::SUCCESS;
  }

  //---------------------------------------------------------------------

  StatusCode TrigT1MBTS::execute() {
    if(msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << "execute()" << endmsg;
    
    StatusCode sc;
    std::string containerName;
    
    // Retrieve Lvl1 MBTS container
    containerName = m_tileTTL1ContainerName;
    m_tileTTL1MBTS = 0;
    sc = evtStore()->retrieve(m_tileTTL1MBTS, containerName);
    if( sc.isFailure()  || !m_tileTTL1MBTS ) {
      if(msgLvl(MSG::WARNING)) {
        msg(MSG::WARNING) << containerName << " not found" << endmsg;
        msg(MSG::WARNING) << "This event will be skipped." << endmsg;
      }
      return StatusCode::SUCCESS;
    }
    else if(msgLvl(MSG::DEBUG)) {
      msg(MSG::DEBUG) << containerName << " Container Successfully Retrieved" 
		      << endmsg;
    }

    // Check for previous bug: there should be 24 or less depending on
    // the configuration of the Tile algorithm that creates the
    // TileTTL1MBTS container.
    if(m_tileTTL1MBTS->size() > 32) {
      if(!m_badDataFound) {
	if(msgLvl(MSG::WARNING)) {
	  msg(MSG::WARNING) << "BAD DATA!!!  m_tileTTL1MBTS->size() = " << m_tileTTL1MBTS->size() << endmsg;
	  msg(MSG::WARNING) << "There should be no more than 24 TileTTL1MBTS elements in one event."
			    << endmsg;
	  msg(MSG::WARNING) << "This event will be skipped.  Any further bad data will be skipped."
			    << endmsg;
	}
	
	m_badDataFound = true;
      }
      return StatusCode::SUCCESS;
    }

    if(msgLvl(MSG::DEBUG)) {
      msg(MSG::DEBUG) << "m_tileTTL1MBTS->size() = "<< m_tileTTL1MBTS->size() <<", elements: " << endmsg;
      msg(MSG::DEBUG) << (std::string)(*m_tileTTL1MBTS) << endmsg;
    }

    unsigned int triggersEBA = 0; // Number of triggers in EBA
    unsigned int triggersEBC = 0; // Number of triggers in EBC
    unsigned int single_triggers_A = 0;
    unsigned int single_triggers_C = 0;
    
    // Loop over all Lvl 1 MBTS trigger paddles
    TileTTL1Container::const_iterator ttl1_mbts_itr = m_tileTTL1MBTS->begin();
    TileTTL1Container::const_iterator ttl1_mbts_itr_end = m_tileTTL1MBTS->end();
    for (; ttl1_mbts_itr != ttl1_mbts_itr_end; ttl1_mbts_itr++) {
      
      // Find out which MBTS paddle this is.
      Identifier id = (*ttl1_mbts_itr)->identify();
      if (!m_tileTBID->is_tiletb(id)) {
	if(msgLvl(MSG::ERROR)) msg(MSG::ERROR) << "This is not an MBTS identifier!" << endmsg;
	return StatusCode::FAILURE;
      }

      int phi = m_tileTBID->phi(id);
      if(phi < 0 || phi > 7) {
	if(msgLvl(MSG::ERROR)) msg(MSG::ERROR) << "Phi value " << phi << " is out of range!" << endmsg;
	return StatusCode::FAILURE;
      }

      int channel = m_tileTBID->channel(id);
      if(channel < 0 || channel > 1) {
	if(msgLvl(MSG::ERROR)) msg(MSG::ERROR) << "Channel value " << channel << " is out of range!" << endmsg;
	return StatusCode::FAILURE;
      }


      int detSide = m_tileTBID->type(id);
      if(detSide != -1 && detSide != 1) {
	if(msgLvl(MSG::ERROR)) msg(MSG::ERROR) << "Side value " << detSide << " is out of range!" << endmsg;
	return StatusCode::FAILURE;
      }

      // Retrieve the MBTS signal samples.
      std::vector<double> samples = (*ttl1_mbts_itr)->samples();
      unsigned int numSamples = samples.size();

      if(m_tZeroBin >= numSamples) {
	if(msgLvl(MSG::ERROR)) msg(MSG::ERROR) << "t0 bin index " << m_tZeroBin << " is greater than the number of bins " << numSamples << endmsg;
	return StatusCode::FAILURE;
      }

      if(msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << "Sample [" << m_tZeroBin << "]=" << samples[m_tZeroBin] << endmsg;
      /* 
      // Only the even counters are used for RunII
      if(channel == 1 && phi > 7 && phi%2 != 0) {
	if(msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << "Out counter " << phi << " is not used for RunII! This counter will be skipped!" << endmsg;
	continue;
      }
      */
      // Single input triggers.
      unsigned int thresholdIndex = m_thresholdNumber[channel][phi];
      unsigned int thresholdIndex12 = m_thresholdNumber12[channel][phi];
      if(thresholdIndex > (unsigned int)m_thresholds_c.size() || thresholdIndex > (unsigned int)m_thresholds_a.size()) {
	if(msgLvl(MSG::ERROR)) msg(MSG::ERROR) << "Threshold index \"" << thresholdIndex << "\" for single triggers is out of range." << endmsg;
	return StatusCode::FAILURE;
      }

      if(thresholdIndex12 == 1000){
	if(msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << "this is a needless counter for run2, the sample will be skipped!" << endmsg;
	continue;
      }
      
      // Emulate CFD samples[m_tZeroBin]*m_CFD_fraction
      float ThrValue_a = 0;
      float ThrValue_c = 0;

      if(m_ThrVecSize12){
	ThrValue_a = m_thresholds_short_a[thresholdIndex12]; 
	ThrValue_c = m_thresholds_short_c[thresholdIndex12]; 
      }
      else{
	ThrValue_a = m_thresholds_a[thresholdIndex]; 
	ThrValue_c = m_thresholds_c[thresholdIndex]; 
      }

      if((samples[m_tZeroBin]*m_CFD_fraction > ThrValue_c && detSide == -1) ||
	 (samples[m_tZeroBin]*m_CFD_fraction > ThrValue_a && detSide == 1)) {
	
	// Add the trigger bit to the correct trigger word
	if(detSide == -1) { // EBC
	  single_triggers_C += (1<<m_cablestarts_c[thresholdIndex]);
	  triggersEBC++; // Increment the number of EBC triggers
	}
	else if (detSide == 1) { // EBA
	  single_triggers_A += (1<<m_cablestarts_a[thresholdIndex]);
	  triggersEBA++; // Increment the number of EBA triggers.
	}

	if(msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << "Single input triggered on sample " << m_tZeroBin << " of " << numSamples << " bins." << endmsg;
      }
    }

    // Cropping to 3 bits
    if (triggersEBA>7) triggersEBA=7;
    if (triggersEBC>7) triggersEBC=7;

    if(msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << "Multis: "<< triggersEBA <<" and "<< triggersEBC <<endmsg;

    unsigned int cableWordA = single_triggers_A+(triggersEBA<<m_cablestart_a);
    unsigned int cableWordC = single_triggers_C+(triggersEBC<<m_cablestart_c);

    // Record the CTP trigger word in StoreGate.
    MbtsCTP *mbtsACTP = new MbtsCTP(cableWordA);
    MbtsCTP *mbtsCCTP = new MbtsCTP(cableWordC);

    // Methods used in CTPsimulation are added for testing
    ATH_MSG_DEBUG( " (in CTPSimulation) mbtsA cable word 0 is: 0x" << std::hex << std::setw( 8 ) << std::setfill( '0' ) << mbtsACTP->cableWord0()  );
    ATH_MSG_DEBUG( " (in CTPSimulation) Mult of mbtsA is: "<< static_cast<int>( (mbtsACTP->cableWord0() >> m_cablestart_a) & static_cast<unsigned int>( pow( 2, 3 ) - 1 ) )  );
    ATH_MSG_DEBUG( " mbtsC cable " << mbtsCCTP->print()  );
    ATH_MSG_DEBUG( " (in CTPSimulation) mbtsC cable word 0 is: 0x" << std::hex << std::setw( 8 ) << std::setfill( '0' ) << mbtsCCTP->cableWord0()  );
    ATH_MSG_DEBUG( " (in CTPSimulation) Mult of mbtsC is: "<< static_cast<int>( (mbtsCCTP->cableWord0() >> m_cablestart_c) & static_cast<unsigned int>( pow( 2, 3 ) - 1 ) )  );

    std::string containerNameA = DEFAULT_MbtsACTPLocation;
    std::string containerNameC = DEFAULT_MbtsCCTPLocation;

    sc = evtStore()->record(mbtsACTP, containerNameA, false);
    if(sc.isFailure()) {
      if(msgLvl(MSG::ERROR)) msg(MSG::ERROR) << "Failed to register " << containerNameA << endmsg;
      return StatusCode::FAILURE;
    } 
    else {
      ATH_MSG_DEBUG( containerNameA << " registered successfully "  );
    }
    sc = evtStore()->record(mbtsCCTP, containerNameC, false);
    if(sc.isFailure()) {
      if(msgLvl(MSG::ERROR)) msg(MSG::ERROR) << "Failed to register " << containerNameC << endmsg;
      return StatusCode::FAILURE;
    } 
    else {
      ATH_MSG_DEBUG( containerNameC << " registered successfully "  );
    }
    
    return StatusCode::SUCCESS;
  }

  //---------------------------------------------------------------------

  StatusCode TrigT1MBTS::finalize() {
    return StatusCode::SUCCESS;
  }
}
