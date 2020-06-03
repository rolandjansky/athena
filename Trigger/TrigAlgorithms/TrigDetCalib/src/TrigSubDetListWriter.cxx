/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigSteeringEvent/TrigRoiDescriptor.h"
#include "TrigSteeringEvent/PartialEventBuildingInfo.h"
#include "IRegionSelector/IRegSelSvc.h"

#include "TrigSubDetListWriter.h"
#include "GaudiKernel/IJobOptionsSvc.h"


//using namespace eformat;

TrigSubDetListWriter::TrigSubDetListWriter(const std::string& name, ISvcLocator* pSvcLocator)
  //: HLT::FexAlgo(name, pSvcLocator),
  : HLT::AllTEAlgo(name, pSvcLocator),
    m_regionSelector("RegSelSvc/RegSelSvc", "TrigSubDetListWriter"),
    m_detectors(0),
    m_sourceid(0),
    m_addL1Calo(false),
    m_useCachedResult(false),
    m_nRoIs(0),
    m_rejectAll(false)
{
  declareProperty("PEBInfoLabel", m_pebLabel = "dunno", "The label under which the PartialEventBuildingInfo object is stored");
  //declareProperty("Subdetectors", m_detSpec = "All");
  declareProperty("Subdetectors", m_detSpec); // <--- obsolote, use SubdetId instead
  declareProperty("SubdetId", m_detId);
  declareProperty("MaxRoIsPerEvent", m_maxRoIsPerEvent = -1);
  declareProperty("MonitoringHistoPath", m_outputpath = "/EXPERT/"+name+"/");
// the tile calibration needs an special ROB:
  declareProperty("extraROBs", m_extrarobs);
  declareProperty("addL1Calo", m_addL1Calo);
  declareProperty("rejectAll", m_rejectAll);
}


HLT::ErrorCode TrigSubDetListWriter::hltInitialize()
{
  ServiceHandle<IJobOptionsSvc> p_jobOptionsSvc("JobOptionsSvc", name());
  if ((p_jobOptionsSvc.retrieve()).isFailure()) {
    msg() << MSG::WARNING << "Could not find JobOptionsSvc to set DataFlow properties" << endmsg;
  } else {
    // get the list of enabled ROBs from OKS
    const Property* pprob=Gaudi::Utils::getProperty( p_jobOptionsSvc->getProperties("DataFlowConfig"), "DF_Enabled_ROB_IDs");
    if (pprob != 0) {
      if (m_enabledROBs.assign(*pprob)) {
        //robOKSconfigFound = true;
        msg() << MSG::DEBUG << " ---> Read from DataFlow configuration: " << m_enabledROBs.value().size()
              << " enabled ROB IDs." << endmsg;
      } else {
        msg() << MSG::WARNING << "Could not set Property '" << pprob->name() << "' from DataFlow." << endmsg;
      }
    }
    // get the list of enabled Sub Detectors from OKS
    const Property* ppsd=Gaudi::Utils::getProperty( p_jobOptionsSvc->getProperties("DataFlowConfig"), "DF_Enabled_SubDet_IDs");
    if (ppsd != 0) {
      if (m_enabledSubDetectors.assign(*ppsd)) {
        //subDetOKSconfigFound  = true;
        msg() << MSG::DEBUG << " ---> Read from DataFlow configuration: " << m_enabledSubDetectors.value().size()
              << " enabled Sub Detector IDs." << endmsg;
      } else {
        msg() << MSG::WARNING << "Could not set Property '" << ppsd->name() << "' from DataFlow." << endmsg;
      }
    } 
  }

  // The RegionSelector is being retrieved here
  if( (m_regionSelector.retrieve()).isFailure() ) {
    msg() << MSG::FATAL << "Unable to retrieve RegionSelector Service" << endmsg;
    return HLT::TOOL_FAILURE;
  }

  bool do_Pixel = false,do_SCT = false, do_TRT = false,
   do_TTEM = false, do_TTHEC = false, do_Tile = false,
   do_FCalEM = false,do_FCalHad = false, 
   do_MDT = false, do_RPC = false, do_TGC = false;

  for(std::vector<std::string>::iterator detit = m_detId.begin(); detit != m_detId.end(); ++detit){
     //if (m_detSpec.find("HLTResult") != std::string::npos) {
     if (*detit=="RPC") { 
        m_sourceid.push_back(eformat::MUON_RPC_BARREL_A_SIDE);
        m_sourceid.push_back(eformat::MUON_RPC_BARREL_C_SIDE);
     } else if (*detit=="HLTResult") {
        m_sourceid.push_back(eformat::TDAQ_HLT);
        m_sourceid.push_back(eformat::TDAQ_CTP);
     } else if (*detit=="TDAQ_HLT") {
        m_sourceid.push_back(eformat::TDAQ_HLT);
     } else if (*detit=="L2EFResult") {
        m_sourceid.push_back(eformat::TDAQ_LVL2);
        m_sourceid.push_back(eformat::TDAQ_EVENT_FILTER);
        m_sourceid.push_back(eformat::TDAQ_CTP);
     } else if (*detit=="TDAQ_LVL2") {
        m_sourceid.push_back(eformat::TDAQ_LVL2);
     } else if (*detit=="TDAQ_EVENT_FILTER") {
        m_sourceid.push_back(eformat::TDAQ_EVENT_FILTER);
     } else if (*detit=="TDAQ_CTP") {
        m_sourceid.push_back(eformat::TDAQ_CTP);
     } else if (*detit=="TDAQ_SFI") {
        m_sourceid.push_back(eformat::TDAQ_SFI);
     } else if (*detit=="TDAQ_SFO") {
        m_sourceid.push_back(eformat::TDAQ_SFO);
     } else if (*detit=="TDAQ_CALO" || *detit=="TDAQ_L1CALO") {
        m_sourceid.push_back(eformat::TDAQ_CALO_PREPROC);
        m_sourceid.push_back(eformat::TDAQ_CALO_CLUSTER_PROC_DAQ);
        m_sourceid.push_back(eformat::TDAQ_CALO_CLUSTER_PROC_ROI);
        m_sourceid.push_back(eformat::TDAQ_CALO_JET_PROC_DAQ);
        m_sourceid.push_back(eformat::TDAQ_CALO_JET_PROC_ROI);
     } else if (*detit=="TDAQ_MUON") {     
         m_sourceid.push_back(eformat::TDAQ_MUON_CTP_INTERFACE);
     } else if (*detit=="TDAQ_BEAM_CRATE") {     
         m_sourceid.push_back(eformat::TDAQ_BEAM_CRATE);
     } else if (*detit=="TDAQ_L2SV") {     
         m_sourceid.push_back(eformat::TDAQ_L2SV);
     } else if (*detit=="InnerDetector") {
        m_sourceid.push_back(eformat::PIXEL_IBL);
        m_sourceid.push_back(eformat::PIXEL_BARREL);
        m_sourceid.push_back(eformat::PIXEL_DISK);
        m_sourceid.push_back(eformat::PIXEL_B_LAYER);
        m_sourceid.push_back(eformat::SCT_BARREL_A_SIDE);
        m_sourceid.push_back(eformat::SCT_BARREL_C_SIDE);
        m_sourceid.push_back(eformat::SCT_ENDCAP_A_SIDE);
        m_sourceid.push_back(eformat::SCT_ENDCAP_C_SIDE);
        m_sourceid.push_back(eformat::TRT_ANCILLARY_CRATE);
        m_sourceid.push_back(eformat::TRT_BARREL_A_SIDE);
        m_sourceid.push_back(eformat::TRT_BARREL_C_SIDE);
        m_sourceid.push_back(eformat::TRT_ENDCAP_A_SIDE);
        m_sourceid.push_back(eformat::TRT_ENDCAP_C_SIDE);
     } else if (*detit=="SiOnly") {
        m_sourceid.push_back(eformat::PIXEL_IBL);
        m_sourceid.push_back(eformat::PIXEL_BARREL);
        m_sourceid.push_back(eformat::PIXEL_DISK);
        m_sourceid.push_back(eformat::PIXEL_B_LAYER);
        m_sourceid.push_back(eformat::SCT_BARREL_A_SIDE);
        m_sourceid.push_back(eformat::SCT_BARREL_C_SIDE);
        m_sourceid.push_back(eformat::SCT_ENDCAP_A_SIDE);
        m_sourceid.push_back(eformat::SCT_ENDCAP_C_SIDE);
     } else if (*detit=="DBM") {
        m_sourceid.push_back(eformat::PIXEL_DBM);
		 } else if (*detit=="IBL") {
        m_sourceid.push_back(eformat::PIXEL_IBL);
		 } else if (*detit=="Pixel") {
        m_sourceid.push_back(eformat::PIXEL_IBL);
        m_sourceid.push_back(eformat::PIXEL_BARREL);
        m_sourceid.push_back(eformat::PIXEL_DISK);
        m_sourceid.push_back(eformat::PIXEL_B_LAYER);
     } else if (*detit=="Muons") {
        m_sourceid.push_back(eformat::MUON_ANCILLARY_CRATE);
        m_sourceid.push_back(eformat::MUON_MDT_BARREL_A_SIDE);
        m_sourceid.push_back(eformat::MUON_MDT_BARREL_C_SIDE);
        m_sourceid.push_back(eformat::MUON_MDT_ENDCAP_A_SIDE);
        m_sourceid.push_back(eformat::MUON_MDT_ENDCAP_C_SIDE);
        m_sourceid.push_back(eformat::MUON_RPC_BARREL_A_SIDE);
        m_sourceid.push_back(eformat::MUON_RPC_BARREL_C_SIDE);
        m_sourceid.push_back(eformat::MUON_TGC_ENDCAP_A_SIDE);
        m_sourceid.push_back(eformat::MUON_TGC_ENDCAP_C_SIDE);
        m_sourceid.push_back(eformat::MUON_CSC_ENDCAP_A_SIDE);
        m_sourceid.push_back(eformat::MUON_CSC_ENDCAP_C_SIDE);
        m_sourceid.push_back(eformat::TDAQ_MUON_CTP_INTERFACE);
     } else if (*detit=="CSC") {
       m_sourceid.push_back(eformat::MUON_CSC_ENDCAP_A_SIDE);
       m_sourceid.push_back(eformat::MUON_CSC_ENDCAP_C_SIDE);
     } else if (*detit=="Tile") {
        m_sourceid.push_back(eformat::TILECAL_LASER_CRATE);
        m_sourceid.push_back(eformat::TILECAL_BARREL_A_SIDE);
        m_sourceid.push_back(eformat::TILECAL_BARREL_C_SIDE);
        m_sourceid.push_back(eformat::TILECAL_EXT_A_SIDE);
        m_sourceid.push_back(eformat::TILECAL_EXT_C_SIDE);
     } else if (*detit=="LAr") {
        m_sourceid.push_back(eformat::LAR_EM_BARREL_A_SIDE);
        m_sourceid.push_back(eformat::LAR_EM_BARREL_C_SIDE);
        m_sourceid.push_back(eformat::LAR_EM_ENDCAP_A_SIDE);
        m_sourceid.push_back(eformat::LAR_EM_ENDCAP_C_SIDE);
        m_sourceid.push_back(eformat::LAR_FCAL_A_SIDE);
        m_sourceid.push_back(eformat::LAR_FCAL_C_SIDE);
        m_sourceid.push_back(eformat::LAR_HAD_ENDCAP_A_SIDE);
        m_sourceid.push_back(eformat::LAR_HAD_ENDCAP_C_SIDE);
     } else if (*detit=="FCal") {
        m_sourceid.push_back(eformat::LAR_FCAL_A_SIDE);
        m_sourceid.push_back(eformat::LAR_FCAL_C_SIDE);     
     } else if (*detit=="FORWARD_BCM") {     
         m_sourceid.push_back(eformat::FORWARD_BCM);
     } else if (*detit=="FORWARD_LUCID") {     
         m_sourceid.push_back(eformat::FORWARD_LUCID);
     } else if (*detit=="FORWARD_ZDC") {     
         m_sourceid.push_back(eformat::FORWARD_ZDC);
     } else if (*detit=="FORWARD_ALPHA") {     
         m_sourceid.push_back(eformat::FORWARD_ALPHA);
     } else if (*detit=="FORWARD_AFP") {     
         m_sourceid.push_back(eformat::FORWARD_AFP);
     } else {
         ATH_MSG_ERROR("Unknown detector ID for partial event building:" << *detit << ". Please update TrigSubSetListWriter.");
     }

  }

  if (m_detSpec.find("ID") != std::string::npos) {
    do_Pixel = true;
    do_SCT = true;
    do_TRT = true;
  }

  if (m_detSpec.find("SiOnly") != std::string::npos) {
    do_Pixel = true;
    do_SCT = true;
  }

  if (m_detSpec.find("LAr") != std::string::npos) {
    do_TTEM = true;
    do_TTHEC = true;
    do_FCalEM = true;
    do_FCalHad = true;
  }

  if (m_detSpec.find("Muons") != std::string::npos) {
    do_MDT = true;
    do_RPC = true;
    do_TGC = true;
  }

  if (m_detSpec.find("EM") != std::string::npos) {
    do_TTEM = true;
    do_FCalEM = true;
  }

  if (m_detSpec.find("Had") != std::string::npos) {
    do_TTHEC = true;
    do_Tile = true;
    do_FCalHad = true;
  }

  if (m_detSpec.find("Tile") != std::string::npos) {
    do_Tile = true;
  }

  if (m_detSpec.find("All") != std::string::npos) {
    do_Pixel = true;
    do_SCT = true;
    do_TRT = true;
    do_TTEM = true;
    do_TTHEC = true;
    do_FCalEM = true;
    do_FCalHad = true;
    do_Tile = true;
    do_MDT = true;
    do_RPC = true;
    do_TGC = true;
  }

  m_detectors.clear();
  if (m_detSpec.find("Pixel")   != std::string::npos) do_Pixel = true;
  if (m_detSpec.find("SCT")     != std::string::npos) do_SCT = true;
  if (m_detSpec.find("TRT")     != std::string::npos) do_TRT = true;
  if (m_detSpec.find("TTEM")    != std::string::npos) do_TTEM = true;
  if (m_detSpec.find("TTHEC")   != std::string::npos) do_TTHEC = true;
  if (m_detSpec.find("Tile")    != std::string::npos) do_Tile = true;
  if (m_detSpec.find("FCalEM")  != std::string::npos) do_FCalEM = true;
  if (m_detSpec.find("FCalHad") != std::string::npos) do_FCalHad = true;
  if (m_detSpec.find("MDT")     != std::string::npos) do_MDT = true;
  if (m_detSpec.find("RPC")     != std::string::npos) do_RPC = true;
  if (m_detSpec.find("TGC")     != std::string::npos) do_TGC = true;

  if (do_Pixel)   m_detectors.push_back(PIXEL);
  if (do_SCT)     m_detectors.push_back(SCT);
  if (do_TRT)     m_detectors.push_back(TRT);
  if (do_TTEM)    m_detectors.push_back(TTEM);
  if (do_TTHEC)   m_detectors.push_back(TTHEC);
  if (do_Tile)    m_detectors.push_back(TILE);
  if (do_FCalEM)  m_detectors.push_back(FCALEM);
  if (do_FCalHad) m_detectors.push_back(FCALHAD);
  if (do_MDT)     m_detectors.push_back(MDT);
  if (do_RPC)     m_detectors.push_back(RPC);
  if (do_TGC)     m_detectors.push_back(TGC);

  msg() << MSG::INFO << "Selected "<< m_detectors.size() << " subdetectors = ";
  for (std::vector<DETID>::const_iterator det = m_detectors.begin(); 
       det != m_detectors.end(); det++) msg() << *det << " ";
  msg() << endmsg;

  if ( service("THistSvc", m_thistSvc).isFailure() ) {
      msg() << MSG::ERROR << "Unable to retrieve pointer to THistSvc" << endmsg;
      return HLT::BAD_JOB_SETUP;
  }

  return HLT::OK;
}

HLT::ErrorCode TrigSubDetListWriter::hltBeginRun()
{
  TrigRoiDescriptor roi(true); /// create a fullscan RoI 

  m_ttemrange.clear();
  m_tthecrange.clear();
  m_fcalrange.clear();
  m_fcalemrange.clear();
  m_tilerange.clear();
  m_pixrange.clear();
  m_sctrange.clear();
  m_trtrange.clear();
  m_regionSelector->DetROBIDListUint(TTEM, roi, m_ttemrange);
  m_regionSelector->DetROBIDListUint(TTHEC, roi, m_tthecrange);
  m_regionSelector->DetROBIDListUint(FCALHAD, roi, m_fcalrange);
  m_regionSelector->DetROBIDListUint(FCALEM, roi, m_fcalemrange);
  m_fcalrange.insert(m_fcalrange.end(),m_fcalemrange.begin(),m_fcalemrange.end());
  // Tile uses a different phi convention
  m_regionSelector->DetROBIDListUint(TILE, roi, m_tilerange);
  m_regionSelector->DetROBIDListUint(PIXEL, roi, m_pixrange);
  m_regionSelector->DetROBIDListUint(SCT, roi, m_sctrange);
  m_regionSelector->DetROBIDListUint(TRT, roi, m_trtrange);

  for(std::vector<uint32_t>::iterator it = m_fcalrange.begin();
        it != m_fcalrange.end(); it++) {
      if (!isRobEnabled( *it ) ) it = m_fcalrange.erase(it)-1;
  }

  for(std::vector<uint32_t>::iterator it = m_ttemrange.begin();
        it != m_ttemrange.end(); it++) {
      if (!isRobEnabled( *it ) ) it = m_ttemrange.erase(it)-1;
  }

  for(std::vector<uint32_t>::iterator it = m_tthecrange.begin();
        it != m_tthecrange.end(); it++) {
      if (!isRobEnabled( *it ) ) it = m_tthecrange.erase(it)-1;
  }

  for(std::vector<uint32_t>::iterator it = m_tilerange.begin();
        it != m_tilerange.end(); it++) {
      if (!isRobEnabled( *it ) ) it = m_tilerange.erase(it)-1;
  }

  for(std::vector<uint32_t>::iterator it = m_sctrange.begin();
        it != m_sctrange.end(); it++) {
      if (!isRobEnabled( *it ) ) it = m_sctrange.erase(it)-1;
  }

  for(std::vector<uint32_t>::iterator it = m_trtrange.begin();
        it != m_trtrange.end(); it++) {
      if (!isRobEnabled( *it ) ) it = m_trtrange.erase(it)-1;
  }

  for(std::vector<uint32_t>::iterator it = m_pixrange.begin();
        it != m_pixrange.end(); it++) {
      if (!isRobEnabled( *it ) ) it = m_pixrange.erase(it)-1;
  }

// Prepare for Histograms
  int bin=0;
  for(std::vector<uint32_t>::const_iterator it=m_ttemrange.begin();it!=m_ttemrange.end();++it){
    m_robidmap[*it] = bin;
    bin++;
  }
  bin=0;
  for(std::vector<uint32_t>::const_iterator it=m_tthecrange.begin();it!=m_tthecrange.end();++it){
    m_robidmap[*it] = bin;
    bin++;
  }
  bin=0;
  for(std::vector<uint32_t>::const_iterator it=m_fcalrange.begin();it!=m_fcalrange.end();++it){
    m_robidmap[*it] = bin;
    bin++;
  }
  bin=0;
  for(std::vector<uint32_t>::const_iterator it=m_tilerange.begin();it!=m_tilerange.end();++it){
    m_robidmap[*it] = bin;
    bin++;
  }
  bin=0;
  for(std::vector<uint32_t>::const_iterator it=m_sctrange.begin();it!=m_sctrange.end();++it){
    m_robidmap[*it] = bin;
    bin++;
  }
  bin=0;
  for(std::vector<uint32_t>::const_iterator it=m_trtrange.begin();it!=m_trtrange.end();++it){
    m_robidmap[*it] = bin;
    bin++;
  }
  bin=0; 
  for(std::vector<uint32_t>::const_iterator it=m_pixrange.begin();it!=m_pixrange.end();++it){
    m_robidmap[*it] = bin;
    bin++;
  }
  
    /// Now setup all the histograms!
  if ( this->bookMonitoringHistograms().isFailure() ) {
    msg() << MSG::WARNING << "Histogram booking error. Issuing HLT::BAD_JOB_SETUP" << endmsg;
    return HLT::BAD_JOB_SETUP;
  }
  
/*  std::stringstream ss;
  std::string label;
  for(unsigned int j=0; j<m_ttemrange.size(); j++){
    ss.clear();
    ss << std::hex << "0x" << m_ttemrange[j] << std::dec;
    ss >> label;
    m_httemrob->GetXaxis()->SetBinLabel(j+1,label.c_str());
  }
  
  for(unsigned int j=0; j<m_tthecrange.size(); j++){
    ss.clear();
    ss << std::hex << "0x" << m_tthecrange[j] << std::dec;
    ss >> label;
    m_htthecrob->GetXaxis()->SetBinLabel(j+1,label.c_str());
  }

  for(unsigned int j=0; j<m_fcalrange.size(); j++){
    ss.clear();
    ss << std::hex << "0x" << m_fcalrange[j] << std::dec;
    ss >> label;
    m_hfcalrob->GetXaxis()->SetBinLabel(j+1,label.c_str());
  }
  
  for(unsigned int j=0; j<m_tilerange.size(); j++){
    ss.clear();
    ss << std::hex << "0x" << m_tilerange[j] << std::dec;
    ss >> label;
    m_htilerob->GetXaxis()->SetBinLabel(j+1,label.c_str());
  }

  for(unsigned int j=0; j<m_trtrange.size(); j++){
    ss.clear();
    ss << std::hex << "0x" << m_trtrange[j] << std::dec;
    ss >> label;
    m_htrtrob->GetXaxis()->SetBinLabel(j+1,label.c_str());
  }

  for(unsigned int j=0; j<m_sctrange.size(); j++){
    ss.clear();
    ss << std::hex << "0x" << m_sctrange[j] << std::dec;
    ss >> label;
    m_hsctrob->GetXaxis()->SetBinLabel(j+1,label.c_str());
  }

  for(unsigned int j=0; j<m_pixrange.size(); j++){
    ss.clear();
    ss << std::hex << "0x" << m_pixrange[j] << std::dec;
    ss >> label;
    m_hpixrob->GetXaxis()->SetBinLabel(j+1,label.c_str());
  }

  for(unsigned int j=0; j<m_ttemrange.size(); j++){
    ss.clear();
    ss << std::hex << "0x" << m_ttemrange[j] << std::dec;
    ss >> label;
    m_httemrob->GetXaxis()->SetBinLabel(j+1,label.c_str());
  }*/

  return HLT::OK;

}

StatusCode TrigSubDetListWriter::bookMonitoringHistograms()
{
  /// Say hello
  if (msgLvl()<=MSG::DEBUG) msg() << MSG::DEBUG << "Now trying to register standard monitoring histograms" << endmsg;
  /*m_httemrob = bookAndRegisterTH1I("RobIdTTEM", m_outputpath,m_ttemrange.size() ,0,m_ttemrange.size() );
  m_htthecrob = bookAndRegisterTH1I("RobIdTTHEC", m_outputpath,m_tthecrange.size() ,0,m_tthecrange.size() );
  m_hfcalrob = bookAndRegisterTH1I("RobIdFcal", m_outputpath,m_fcalrange.size() ,0,m_fcalrange.size() );
  m_htilerob = bookAndRegisterTH1I("RobIdTile", m_outputpath,m_tilerange.size() ,0,m_tilerange.size() );
  m_htrtrob = bookAndRegisterTH1I("RobIdTRT", m_outputpath,m_trtrange.size() ,0,m_trtrange.size() );
  m_hsctrob = bookAndRegisterTH1I("RobIdSCT", m_outputpath,m_sctrange.size() ,0,m_sctrange.size() );
  m_hpixrob = bookAndRegisterTH1I("RobIdPixel", m_outputpath,m_pixrange.size() ,0,m_pixrange.size() );*/

  return StatusCode::SUCCESS;
}


HLT::ErrorCode TrigSubDetListWriter::hltFinalize()
{
  return HLT::OK;
}


HLT::ErrorCode TrigSubDetListWriter::hltExecute(std::vector<std::vector<HLT::TriggerElement*> >& /*te_in*/,
                                                unsigned int te_out)
{

  if (m_useCachedResult) {
    if (msgLvl() <= MSG::DEBUG) {
      msg() << MSG::DEBUG << "Executing this Partial event " << name() << " in cached mode" << endmsg;
    }
  }

  m_nRoIs++;


/*    HLT::TEVec allTEs;
    for (std::vector<HLT::TEVec>::const_iterator it = te_in.begin();
	 it != te_in.end(); ++it) {

      for (HLT::TEVec::const_iterator inner_it = (*it).begin();
	   inner_it != (*it).end(); ++inner_it) {

	allTEs.push_back(*inner_it);
//        msg() << MSG::DEBUG << "Executing this TrigSubDetListWriter " << name() << " for types "
//	      << inner_it->getId();
      }
    }*/


  if (msgLvl(MSG::DEBUG)) {
      msg() << MSG::DEBUG << "Executing this TrigSubDetListWriter " << name() << endmsg;
      if (m_maxRoIsPerEvent > -1) msg() << " RoI " << m_nRoIs << "/" << m_maxRoIsPerEvent;
      msg() << endmsg;
  }
  
  if (m_maxRoIsPerEvent > -1 && m_nRoIs > m_maxRoIsPerEvent) {
    ATH_MSG_DEBUG("RoI limit ("  << m_maxRoIsPerEvent 
                  << ") reached for this event : will not process this RoI");
    return HLT::OK;
  }
  
  
// Get RoI descriptor
  TrigRoiDescriptor* roIDescriptor = 0;
  
  HLT::ErrorCode status = HLT::BAD_ALGO_CONFIG;
  
  // from dummyAlgoForCalibration
  // create new partial EB directive
  PartialEventBuildingInfo* pebInfo = config()->getPEBI();
  if(!pebInfo){
    msg() << MSG::DEBUG << "*** Not Executing this TrigSubDetListWriter " << name() << ", not a calib chain" << endmsg;
    return HLT::OK;
  }
  /*PartialEventBuildingInfo* pebInfo = 0;
  if (store()->transientContains<PartialEventBuildingInfo>(m_pebLabel))
    store()->retrieve(pebInfo, m_pebLabel).ignore();
  else {
    pebInfo = new PartialEventBuildingInfo();
    store()->record(pebInfo, m_pebLabel, true).ignore();
  }*/

  // now add ROBs
  status = fillPEBInfo(*pebInfo);
  if(status!=HLT::OK){
    msg() << MSG::DEBUG << "No ROB info for partial event building" << endmsg;
    return status;
  }
  HLT::TriggerElement* te = addRoI(te_out,roIDescriptor);
  if(m_rejectAll)  // Flag used for commissioning
     te->setActiveState(false);
  else
     te->setActiveState(true);
  return status;

}


HLT::ErrorCode TrigSubDetListWriter::fillPEBInfo(PartialEventBuildingInfo& pebInfo)
{

  std::vector<uint32_t> detectorROBs;
  //std::vector<uint32_t> allROBs, detectorROBs;

//  double etaMin = -5.;
//  double etaMax = 5.;
//  double phiMax = M_PI;
//  double phiMin = -M_PI;
  //double phiMin = -2*M_PI;
  int size = 0;

  // reserve space of ROB vector, limits to be determined
  //allROBs.reserve(120);
  // ROB ID 0x520010, need special ROB for tile calibration
  //allROBs.insert(allROBs.end(),m_extrarobs.begin(),m_extrarobs.end());

  // including L1 Calo ROBs
  if(m_addL1Calo)
     this->fillL1CaloRobs(m_extrarobs);
     
  pebInfo.addROBs(m_extrarobs);

  size += m_extrarobs.size();
  size += m_sourceid.size();

  if(m_sourceid.size()!=0)
      pebInfo.addSubDetector(m_sourceid);

  for (std::vector<DETID>::const_iterator detector = m_detectors.begin();
       detector != m_detectors.end(); detector++)
  {
    detectorROBs.clear();
    // if((*detector)==TILE){
    //    phiMax = 2*M_PI;
    //    phiMin = 0.;
    // }else{
    //    phiMax = M_PI;
    //    phiMin = -M_PI;
    // }
    //m_regionSelector->DetROBIDListUint(*detector, etaMin, etaMax, phiMin, phiMax, detectorROBs);
    //m_regionSelector->DetROBIDListUint(*detector, detectorROBs);
    //allROBs.insert(allROBs.end(), detectorROBs.begin(), detectorROBs.end());
    //msg() << MSG::DEBUG << "ROBs for detector " << *detector << " : ";
    //for (unsigned int i = 0; i < detectorROBs.size(); i++) msg() << "0x" << std::hex << detectorROBs[i] << std::dec << " ";
    //msg() << endmsg;
    switch (*detector) {
      case PIXEL:
        //for(unsigned int i=0; i<detectorROBs.size(); i++) m_hpixrob->Fill(m_robidmap[detectorROBs[i]]);
        size += m_pixrange.size();
        pebInfo.addROBs(m_pixrange);
        break;
      case SCT:
        //for(unsigned int i=0; i<detectorROBs.size(); i++) m_hsctrob->Fill(m_robidmap[detectorROBs[i]]);
        size += m_sctrange.size();
        pebInfo.addROBs(m_sctrange);
        break;
      case TRT:
        //for(unsigned int i=0; i<detectorROBs.size(); i++) m_htrtrob->Fill(m_robidmap[detectorROBs[i]]);
        size += m_trtrange.size();
        pebInfo.addROBs(m_trtrange);
        break;
      case LAR:
      case TTEM:
        //for(unsigned int i=0; i<detectorROBs.size(); i++) m_httemrob->Fill(m_robidmap[detectorROBs[i]]);
        size += m_ttemrange.size();
        pebInfo.addROBs(m_ttemrange);
        break;
      case TTHEC:
        //for(unsigned int i=0; i<detectorROBs.size(); i++) m_htthecrob->Fill(m_robidmap[detectorROBs[i]]);
        size += m_tthecrange.size();
        pebInfo.addROBs(m_tthecrange);
        break;
      case TILE:
        //for(unsigned int i=0; i<detectorROBs.size(); i++) m_htilerob->Fill(m_robidmap[detectorROBs[i]]);
        size += m_tilerange.size();
        pebInfo.addROBs(m_tilerange);
        break;
      case FCALEM:
      case FCALHAD:
        //for(unsigned int i=0; i<detectorROBs.size(); i++) m_hfcalrob->Fill(m_robidmap[detectorROBs[i]]);
        size += m_fcalrange.size();
        pebInfo.addROBs(m_fcalrange);
        break;
      case MDT:
      case RPC:
      case TGC:
      case CSC:
        ATH_MSG_DEBUG("Muon detectors not yet implemented");
        break;
      default:
        msg() << MSG::WARNING << "unknown detector type requested " << endmsg;
     }

  }

  //for (std::vector<uint32_t>::const_iterator rob = allROBs.begin(); rob != allROBs.end(); rob++)
  //  pebInfo.add(*rob);

/*  std::cout << "in partial eb after " << std::endl;
  std::vector<uint32_t> rob2;
  pebInfo.get(rob2);
  for(int j=0;j<rob2.size();j++){
    std::cout << "passing this rob = " << (rob2)[j] << std::endl;
  }*/

  m_useCachedResult = true;

  if(size==0) return HLT::MISSING_FEATURE;

  return HLT::OK;
}
    

HLT::ErrorCode TrigSubDetListWriter::hltEndEvent()
{
  m_nRoIs = 0;
  m_useCachedResult=false;
  return HLT::OK;
}

TH1I* TrigSubDetListWriter::bookAndRegisterTH1I(const char* name, std::string outpath, int nBins, int minX, int maxX)
{
  TH1I* h = new TH1I(name,name,nBins,minX,maxX);
  if (m_thistSvc->regHist(outpath + h->GetName(), h).isFailure()) msg() << MSG::WARNING << "Can't book " << outpath + h->GetName() << endmsg;

  return h;
}

void TrigSubDetListWriter::fillL1CaloRobs(std::vector<uint32_t>& roblist)
{
  
   uint32_t l1caloarray [] = {
     0x710000, 0x710001, 0x710002, 0x710003, 0x710004, 0x710005, 0x710006, 0x710007, 0x710010, 0x710011, 0x710012, 0x710013, 0x710014, 0x710015, 0x710016, 0x710017, 0x710020, 0x710021, 0x710022, 0x710023, 0x710024, 0x710025, 0x710026, 0x710027, 0x710030, 0x710031, 0x710032, 0x710033, 0x710034, 0x710035, 0x710036, 0x710037, 0x720008, 0x720009, 0x72000a, 0x72000b, 0x720028, 0x720029, 0x72002a, 0x72002b, 0x730088, 0x730089, 0x73008a, 0x73008b, 0x74000c, 0x74000d, 0x74001c, 0x74001d, 0x74002c, 0x74002d, 0x74003c, 0x74003d, 0x75008c, 0x75008d };
   roblist.insert(roblist.end(),l1caloarray,l1caloarray+sizeof(l1caloarray)/sizeof(uint32_t));
}

//========================================================================= 
// check that a ROB ID is enabled for readout in OKS  ---> for "online" running
//=========================================================================
bool TrigSubDetListWriter::isRobEnabled(const uint32_t robid) {
  bool b_enabled = true;

  // check if given ROB is actually enabled for readout
  if (m_enabledROBs.value().size() != 0) {
    std::vector<uint32_t>::const_iterator rob_enabled_it =
      std::find(m_enabledROBs.value().begin(), m_enabledROBs.value().end(), robid);
    if(rob_enabled_it == m_enabledROBs.value().end()) {
      ATH_MSG_DEBUG("---> ROB Id : 0x" << MSG::hex << robid << MSG::dec
                    << " will not be retrieved, since it is not on the list of enabled ROBs.");
      b_enabled = false;
    }
  }

  return b_enabled;
}

