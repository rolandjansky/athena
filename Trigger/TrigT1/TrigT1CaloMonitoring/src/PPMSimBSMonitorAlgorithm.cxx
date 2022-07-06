/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

#include "PPMSimBSMonitorAlgorithm.h"
#include "SGTools/StlVectorClids.h"
#include "TrigT1CaloToolInterfaces/IL1TriggerTowerToolRun3.h"

PPMSimBSMonitorAlgorithm::PPMSimBSMonitorAlgorithm( const std::string& name, ISvcLocator* pSvcLocator )
  : AthMonitorAlgorithm(name,pSvcLocator)
{
}

StatusCode PPMSimBSMonitorAlgorithm::initialize() {

  ATH_MSG_DEBUG("PPMSimBSMonitorAlgorithm::initialize");
  ATH_MSG_DEBUG("Package Name "<< m_packageName);
  ATH_MSG_DEBUG("m_xAODTriggerTowerContainerName "<< m_xAODTriggerTowerContainerName); 

  // we initialise all the containers that we need
  ATH_CHECK( AthMonitorAlgorithm::initialize() );
  ATH_CHECK( m_xAODTriggerTowerContainerName.initialize() );
 
  
  ATH_CHECK( m_ttTool.retrieve());
  ATH_CHECK( m_runParametersContainer.initialize() );
  
  ATH_CHECK(m_errorLocation.initialize());
 
  return StatusCode::SUCCESS;
}

StatusCode PPMSimBSMonitorAlgorithm::fillHistograms( const EventContext& ctx ) const {

  ATH_MSG_DEBUG("PPMSimBSMonitorAlgorithm::fillHistograms");
  

  // Retrieve L1CaloRunParametersContainer
  SG::ReadCondHandle<L1CaloRunParametersContainer> runParameters( m_runParametersContainer, ctx);
  unsigned int readoutConfigID   = runParameters->runParameters(1)->readoutConfigID(); 
  ATH_MSG_DEBUG("RunParameters:: readoutConfigID " <<  readoutConfigID);
  
  // Retrieve Trigger Towers from SG
  SG::ReadHandle<xAOD::TriggerTowerContainer> triggerTowerTES(m_xAODTriggerTowerContainerName, ctx);
  if(!triggerTowerTES.isValid()){
    ATH_MSG_ERROR("No Trigger Tower container found in TES  "<< m_xAODTriggerTowerContainerName); 
    return StatusCode::FAILURE;
  }

  // Create a vector of trigger towers with quantities to be monitored
  std::vector<MonitorTT> vecMonTT;     // All towers

  const int nCrates = 8;
  ErrorVector crateError(nCrates);
  ErrorVector moduleError(nCrates);

  bool error_tt{false};

  // PPM sim vectors 
  std::vector<int> LutCp; 
  std::vector<int> LutJep; 
  std::vector<int> BcidR;
  std::vector<int> BcidD;

  // Loop over trigger tower container
  // Create the trigger tower objects and calculate scaled phi
  for (const xAOD::TriggerTower* tt : *triggerTowerTES) {
    ATH_CHECK( makePPMTower(tt, vecMonTT) );     
  }

  
  std::vector<std::string> mismatch_map= {"NonZeroMatches", "ZeroMatches", "DataNoSim", "SimNoData"};
  std::vector<std::string> lut_map= { "SimEqData", "SimNeData", "SimNoData", "DataNoSim"};
  std::vector<std::string> crate_map= { "cr0cr1", "cr2cr3", "cr4cr5", "cr6cr7"};

  // set maximum number of error events per lumiblock(per type) to avoid histograms with many x-bins
  const int maxErrorsPerLB=10;
  
  auto lb = GetEventInfo(ctx)->lumiBlock();
  ATH_MSG_DEBUG("Lumi Block" << lb); 
  const int eventNumber =  ctx.eventID().event_number();
  ATH_MSG_DEBUG("Event Number" << eventNumber); 

  
  
  // Loop over the trigger tower objects and fill the histograms 
 
    for (auto& myTower : vecMonTT) {
    ATH_MSG_DEBUG("PPMSimBSMonitorAlgorithm looping over TTs"); 

    const double eta =  (myTower.tower)->eta();
    const double phi =  (myTower.tower)->phi();
    const int datCp =   (myTower.tower)->cpET();
    const int datJep =  (myTower.tower)->lut_jep().empty() ? 0 :  (myTower.tower)->jepET();
    bool pedCorrOverflow = false;
    const std::size_t nPedCorr =  (myTower.tower)->correction().size();
    int simCp = 0;
    int simJep = 0;
    int simBcid = 0;
    uint8_t datBcid = 0;
    std::vector<uint8_t> datBcidVec = ( (myTower.tower)->bcidVec());
    if ( datBcidVec.size() > 0) {
      datBcid = datBcidVec[ (myTower.tower)->peak()];
    }
    
   

    //Retrieve RunParameters container from COOL database and check if run was taken with 80MHz readout. If yes, drop the 80MHz samples to emulate 40 MHz readout
    std::vector<uint16_t> digits40;

    if(readoutConfigID==5 or readoutConfigID==6){

      int nSlices = (myTower.tower)->adc().size();

      if((nSlices%4)==3){
	for (int i=0 ; i < (nSlices-1)/2 ; i++ ){
	  digits40.push_back((myTower.tower)->adc().at(2*i+1));
	}
      }
      else if((nSlices%4)==1){
	for (int i=0 ; i <= (nSlices-1)/2 ; i++ ){
	  digits40.push_back((myTower.tower)->adc().at(2*i));
	}
      }

    }else{
      digits40 = (myTower.tower)->adc();
    }

    const std::vector<uint_least16_t>& ADC = digits40;
    const int Slices = ADC.size();
    const int Peak = Slices/2.;

    //Check for over-/underflow of pedestalCorrection
    for(std::size_t i = 0; i < nPedCorr; ++i) {
    if((myTower.tower)->correction()[i]>=511 or (myTower.tower)->correction()[i]<=-512){
        pedCorrOverflow = true;
        break;
       }
    }


    // only run simulation for non-empty TTs
    if(datCp || datJep || *std::max_element(std::begin(ADC), std::end(ADC)) >= m_simulationADCCut) {
      BcidR.clear();
      BcidD.clear();
      m_ttTool->simulateChannel(*(myTower.tower), LutCp, LutJep, BcidR, BcidD);
      simBcid = BcidR[Peak];
      if (Slices < 7 || nPedCorr < 3) {
	simJep = LutJep[Peak];
	simCp = LutCp[Peak];
      }
    }
    

    
   // ---------------------Fill in error plots------------------------
    int mismatch = 0;
    
    std::string groupName = "group_Mismatch_peakf_"; 
    
    if (datCp || datJep || *std::max_element(std::begin(ADC), std::end(ADC)) >= m_simulationADCCut) {
      std::bitset<3> simBcidBits(simBcid);
      std::bitset<3> datBcidBits(datBcid);
      if ((Slices >= 7) && (nPedCorr >= 3)) { // compare simulation of peak finder to data (sim not possible in 5+1 readout mode)
       	if (simBcidBits[2] && datBcidBits[2]) { //non-zero match
	  ATH_CHECK( fillPPMEtaPhi(myTower, groupName + mismatch_map.at(0), "", 1) );
	} 
	else if (!simBcidBits[2] && !datBcidBits[2]) { // zero match
	  ATH_CHECK( fillPPMEtaPhi(myTower, groupName + mismatch_map.at(1), "", 1) );
	} 
	else if (simBcidBits[2] != datBcidBits[2]) { // mismatch
          mismatch = 1;
          if (!simBcidBits[2]) { //data no sim
	    ATH_CHECK( fillPPMEtaPhi(myTower, groupName + mismatch_map.at(2), "", 1) );
          }
	  else if (!datBcidBits[2]) { //sim no data
	    ATH_CHECK( fillPPMEtaPhi(myTower, groupName + mismatch_map.at(3), "", 1) );
          } 
          ATH_MSG_DEBUG(" PeakfinderBCID sim/data Mismatch eta/phi/sim/dat: " << eta << "/" << phi << "/" << simBcidBits[2] << "/" << datBcidBits[2]);
        } 
      }
    


      groupName = "group_Mismatch_satBcid_"; 

      if (ADC[0] < 1020 && ADC[1] < 1020) { // compare simulation of saturated bcid algorithm to data
        if (simBcidBits[1] && datBcidBits[1]) { // non-zero match
	  ATH_CHECK( fillPPMEtaPhi(myTower, groupName + mismatch_map.at(0), "", 1) );
	} 
	else if (!simBcidBits[1] && !datBcidBits[1]) { // zero match
	  ATH_CHECK( fillPPMEtaPhi(myTower, groupName + mismatch_map.at(1), "", 1) );
        } 
	else if (simBcidBits[1] != datBcidBits[1]) { // mismatch
          mismatch = 1;
          if (!simBcidBits[1]) { // data no sim
	    ATH_CHECK( fillPPMEtaPhi(myTower, groupName + mismatch_map.at(2), "", 1) );
	    mismatch=0;
          } 
	  else if (!datBcidBits[1]) { // sim no data
	    ATH_CHECK( fillPPMEtaPhi(myTower, groupName + mismatch_map.at(3), "", 1) );
	  }
          ATH_MSG_DEBUG(" SaturatedBCID sim/data Mismatch eta/phi/sim/dat: "
		        << eta << "/" << phi << "/" << simBcidBits[1] << "/" << datBcidBits[1]);  
	}
      }
    }

        
    // Compare LUT simulation to data
    // only fill histograms for non-empty towers (simulation or data)
    if (!simCp && !datCp && !simJep && !datJep) continue;

    
    groupName = "groupLUTCP_EM_"; 
    if ( (myTower.tower)->layer() == 0) { // For EM layer
      // For LUT-CP
      if (simCp && simCp == datCp) { // non-zero match
	ATH_CHECK( fillPPMEtaPhi(myTower, groupName + lut_map.at(0), "", 1) );
      } 
      else if (simCp != datCp && !pedCorrOverflow) {  // mis-match
        mismatch = 1;
        if (simCp && datCp) {       // non-zero mis-match
	  ATH_CHECK( fillPPMEtaPhi(myTower, groupName + lut_map.at(1), "", 1) );
        } 
	else if (!datCp) {        // no data
          if ((Slices >= 7) && (nPedCorr >= 3)) {
	    ATH_CHECK( fillPPMEtaPhi(myTower, groupName + lut_map.at(2), "", 1) );
          } 
	  else mismatch = 0;
        } 
	else {                    // no sim
	  ATH_CHECK( fillPPMEtaPhi(myTower, groupName + lut_map.at(3), "", 1) );
        }
	ATH_MSG_DEBUG(" EMTowerMismatch eta/phi/sim/dat: "
		      << eta << "/" << phi << "/" << simCp << "/" << datCp);
      }
      

      groupName = "groupLUTJEP_EM_"; 

      if (simJep && simJep == datJep) { // non-zero match
	ATH_CHECK( fillPPMEtaPhi(myTower, groupName + lut_map.at(0), "", 1) );
      } 
      else if (simJep != datJep && !pedCorrOverflow) {  // mis-match
        mismatch = 1;
        if (simJep && datJep) {       // non-zero mis-match
	  ATH_CHECK( fillPPMEtaPhi(myTower, groupName + lut_map.at(1), "", 1) );
        } 
	else if (!datJep) {        // no data
	  if ((Slices >= 7) && (nPedCorr >= 3)) {
	    ATH_CHECK( fillPPMEtaPhi(myTower, groupName + lut_map.at(2), "", 1) );
          } 
	  else mismatch = 0;
        } 
	else { 
	  // no sim
	  ATH_CHECK( fillPPMEtaPhi(myTower, groupName + lut_map.at(3), "", 1) );
        }
	ATH_MSG_DEBUG(" EMTowerMismatch eta/phi/sim/dat: "
		      << eta << "/" << phi << "/" << simJep << "/" << datJep);
      }
    }


    // For HAD layer
    groupName = "groupLUTCP_HAD_"; 
        
    if ((myTower.tower)->layer() == 1) { // For HAD layer
      // For LUT-CP
      if (simCp && simCp == datCp) { // non-zero match
	ATH_CHECK( fillPPMEtaPhi(myTower, groupName + lut_map.at(0), "", 1) );
      } 
      else if (simCp != datCp  && !pedCorrOverflow ) {  // mis-match
        mismatch = 1;
        if (simCp && datCp) {       // non-zero mis-match
	  ATH_CHECK( fillPPMEtaPhi(myTower, groupName + lut_map.at(1), "", 1) );
	} 
	else if (!datCp) {        // no data
	  if ((Slices >= 7) && (nPedCorr >= 3)) {
	    ATH_CHECK( fillPPMEtaPhi(myTower, groupName + lut_map.at(2), "", 1) );
          } 
	  else mismatch = 0;
        } 
	else {                    // no sim
	  ATH_CHECK( fillPPMEtaPhi(myTower, groupName + lut_map.at(3), "", 1) );
        }
	ATH_MSG_DEBUG(" HADTowerMismatch eta/phi/sim/dat: "
		      << eta << "/" << phi << "/" << simCp << "/" << datCp);
      }
            
      // For LUT-JEP
      groupName = "groupLUTJEP_HAD_"; 

      if (simJep && simJep == datJep) { // non-zero match
	ATH_CHECK( fillPPMEtaPhi(myTower, groupName + lut_map.at(0), "", 1) );
      } 
      else if (simJep != datJep  && !pedCorrOverflow ) {  // mis-match
        mismatch = 1;
        if (simJep && datJep) {       // non-zero mis-match
	  ATH_CHECK( fillPPMEtaPhi(myTower, groupName + lut_map.at(1), "", 1) );
	} 
	else if (!datJep) {        // no data
          if ((Slices >= 7) && (nPedCorr >= 3)) {
	    ATH_CHECK( fillPPMEtaPhi(myTower, groupName + lut_map.at(2), "", 1) );
          } 
	  else mismatch = 0;
        } 
	else {                    // no sim
	  ATH_CHECK( fillPPMEtaPhi(myTower, groupName + lut_map.at(3), "", 1) );

        }
	ATH_MSG_DEBUG(" HADTowerMismatch eta/phi/sim/dat: "
		      << eta << "/" << phi << "/" << simJep << "/" << datJep);
      }
    }
    


   
    groupName = "group_Error_";    

    
    // scope for mutable error event per lumi block tt counter
    std::lock_guard<std::mutex> lock(m_mutex);	
    if (!error_tt) {
      m_errorLB_tt_counter[lb]+=1;
      error_tt = true;
    }
    
    
    if (mismatch == 1) {
      const L1CaloCoolChannelId coolId((myTower.tower)->coolId());
      const int crate  = coolId.crate();
      const int module = coolId.module();
      crateError[crate] = 1;
      if (!((moduleError[crate] >> module) & 0x1)) {
	const int y = module + 16 * (crate % 2);
	auto y_2D = Monitored::Scalar<int>("y_2D", y);
	if (m_errorLB_tt_counter[lb]<=maxErrorsPerLB) {
	  auto  eventMonitor= Monitored::Scalar<std::string>("eventMonitor", std::to_string(eventNumber));
	  if     (crate == 0 || crate == 1) fill(groupName +  crate_map.at(0), eventMonitor, y_2D );
	  else if (crate == 2 || crate == 3) fill(groupName +  crate_map.at(1), eventMonitor, y_2D );
	  else if (crate == 4 || crate == 5) fill(groupName +  crate_map.at(2), eventMonitor, y_2D ); 
	  else if (crate == 6 || crate == 7) fill(groupName +  crate_map.at(3), eventMonitor, y_2D ); 
	  moduleError[crate] |= (1 << module);
	}
      }
    }
    
    

  } // End loop over tower objects 

  
    // Save error vector for global summary
  {
    auto save = std::make_unique<ErrorVector>(crateError);
    auto* result = SG::makeHandle(m_errorLocation, ctx).put(std::move(save));
    if (!result) {
      ATH_MSG_ERROR("Error recording PPMSimBS mismatch vector in TES");
      return StatusCode::FAILURE;
    }
  }


  return StatusCode::SUCCESS;
}


StatusCode PPMSimBSMonitorAlgorithm::makePPMTower( const xAOD::TriggerTower* tt, 
                                                    std::vector<MonitorTT> &vecMonTT) const
{
  // Geometry
  const double phi = tt->phi();
  double phiMod = phi * m_phiScaleTT;
  
  // LUT JEP
  int jepET = 0;
  const std::vector<uint_least8_t> jepETvec = tt->lut_jep();
  if (jepETvec.size() > 0) jepET = tt->jepET();
  
 
  // Fill TT quantities
  MonitorTT monTT;
  monTT.tower = tt;
  monTT.phiScaled = phiMod;
  monTT.jepET = jepET;
  vecMonTT.push_back(monTT);
   
  return StatusCode::SUCCESS; 
}


StatusCode PPMSimBSMonitorAlgorithm::fillPPMEtaPhi( MonitorTT &monTT, 
                                           const std::string& groupName, 
                                           const std::string& weightName,
                                           double weight) const {
  
  // Number of bins filled in phi depends on eta due to electronics coverage
 
  // KW to do: fill in shrinkEtaBins part
  double phiMod = monTT.phiScaled;  // Integer binning for 2D plots 
  double etaMod = monTT.tower->eta();
  const double absEta = std::abs(etaMod);
  
  const std::vector<double> offset32 = {1.5, 0.5, -0.5, -1.5};                                                                             
  const std::vector<double> offset25 = {0.5, -0.5}; 
  std::vector<double> offset = {}; 
    
  if (absEta > 3.2) {
    // Fill four bins in phi
    phiMod = std::floor(phiMod/4)*4. + 2.;
    offset = offset32;
  } 
  else if (absEta > 2.5) {
    // Fill two bins in phi
    phiMod = std::floor(phiMod/2)*2. + 1.;
    offset = offset25;
  }     
  else {
    offset = {0.};
  }

  ATH_MSG_DEBUG("absEta: " << absEta << "offset.size(): " << offset.size());
 
  // Fill the histograms 
  for (auto phiOffset : offset)  {

    auto etaTT_2D = Monitored::Scalar<double>("etaTT_2D", etaMod);
    auto phiTT_2D = Monitored::Scalar<double>("phiTT_2D", phiMod + phiOffset);
    
    auto weight_2D = Monitored::Scalar<double>(weightName, weight); // Weight for filling 2D profile histograms; name must be included in python histogram definition
    ATH_MSG_DEBUG("groupName: weight_2D" << weight_2D); 
    
    fill(groupName, etaTT_2D, phiTT_2D, weight_2D);
    
  }      

  
  
 
    


  return StatusCode::SUCCESS;
} 

