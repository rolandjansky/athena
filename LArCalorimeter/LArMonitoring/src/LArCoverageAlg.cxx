/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

// ********************************************************************
//
// NAME:     LArCoverageAlg.cxx
// PACKAGE:  LArMonitoring
//
// AUTHOR:   Margherita Spalla, migrated from LArCoverage by Jessica Leveque
//
// Class for monitoring : Detector cells in the readout
//                        Masked Channels from database
//                        Availability of calibration constants 
//                        Bad calibration at FEB level
// ********************************************************************

#include "LArCoverageAlg.h"

#include "GaudiKernel/ISvcLocator.h"
#include "GaudiKernel/IToolSvc.h"

#include "Identifier/IdentifierHash.h"

#include "TMath.h"

#include <sstream>
#include <iomanip>
#include <fstream>
#include <string>
#include <vector>
#include <cstdlib>
#include <functional>
#include <map>
#include <utility>




using namespace std;

/*---------------------------------------------------------*/
LArCoverageAlg::LArCoverageAlg(const std::string& name, ISvcLocator* pSvcLocator )
  : AthMonitorAlgorithm(name,pSvcLocator)
{

  m_LArOnlineIDHelper	= nullptr;
  m_LArEM_IDHelper	= nullptr;
  m_LArFCAL_IDHelper	= nullptr;
  m_LArHEC_IDHelper	= nullptr;
  m_caloIdMgr		= nullptr;
 }

/*---------------------------------------------------------*/
LArCoverageAlg::~LArCoverageAlg()
{}

/*---------------------------------------------------------*/
StatusCode 
LArCoverageAlg::initialize()
{
  ATH_MSG_INFO( "Initialize LArCoverageAlg" );  

  /** check binning (for filling white bins in eta-phi coverage plots) */
  TString warn_binning="";
  if(m_NphiBinsEMB1!=256) warn_binning="NphiBinsEMB1!=256 ";
  if(m_NphiBinsEMEC2!=256) warn_binning="NphiBinsEMEC2!=256 ";
  if((int)(m_NphiBinsHEC.size())<m_Nsample) ATH_MSG_ERROR("NphiBinsHEC must contain " << m_Nsample << " elements");
  if(m_NphiBinsHEC[0]!=64) warn_binning="NphiBinsHEC[0]!=64 ";
  if(m_NphiBinsHEC[1]!=64) warn_binning="NphiBinsHEC[1]!=64 ";
  if(m_NphiBinsHEC[2]!=64) warn_binning="NphiBinsHEC[2]!=64 ";
  if(m_NphiBinsHEC[3]!=64) warn_binning="NphiBinsHEC[3]!=64 ";


  /** Retrieve ID helpers */
  ATH_CHECK(  detStore()->retrieve( m_caloIdMgr ) );
  m_LArEM_IDHelper   = m_caloIdMgr->getEM_ID();
  m_LArHEC_IDHelper  = m_caloIdMgr->getHEC_ID();
  m_LArFCAL_IDHelper = m_caloIdMgr->getFCAL_ID();
   
  ATH_CHECK( detStore()->retrieve(m_LArOnlineIDHelper, "LArOnlineID") );
  ATH_CHECK( m_BCKey.initialize() );
  ATH_CHECK( m_BFKey.initialize() );
  ATH_CHECK( m_caloMgrKey.initialize() );

  /** retrieve bad channel tool */
   ATH_CHECK(m_bcMask.buildBitMask(m_problemsToMask,msg()));

  /** Initialize cabling key */
  ATH_CHECK(m_cablingKey.initialize());
   

  /** calo noise key */
  ATH_CHECK( m_noiseCDOKey.initialize() );

  /** raw channel key */
  ATH_CHECK( m_rawChannelsKey.initialize() ); 

  std::vector<std::string> availableErrorCodesStrs;
  /** Translate codes to integers **/
  for (const auto& code : m_availableErrorCodes) {
    availableErrorCodesStrs.emplace_back(Form("%d", code));
    m_availableErrorCodesPairs.emplace_back(code, availableErrorCodesStrs.back());
  }

  /** tool maps (arrays of histograms) */
  m_CaloNoiseGroupArrEM = Monitored::buildToolMap<int>(m_tools,m_CaloNoiseToolGroupName+"EM",m_Nsample);
  m_CaloNoiseGroupArrHEC = Monitored::buildToolMap<int>(m_tools,m_CaloNoiseToolGroupName+"HEC",m_Nsample);
  m_CaloNoiseGroupArrFCAL = Monitored::buildToolMap<int>(m_tools,m_CaloNoiseToolGroupName+"FCal",m_Nsample);

  m_CoverageToolArrayEMBA = Monitored::buildToolMap<int>(m_tools,m_CoverageHWGroupName+"EMBA",availableErrorCodesStrs);
  m_CoverageToolArrayEMECA = Monitored::buildToolMap<int>(m_tools,m_CoverageHWGroupName+"EMECA",availableErrorCodesStrs);
  m_CoverageToolArrayHECA = Monitored::buildToolMap<int>(m_tools,m_CoverageHWGroupName+"HECA",availableErrorCodesStrs);
  m_CoverageToolArrayFCalA = Monitored::buildToolMap<int>(m_tools,m_CoverageHWGroupName+"FCalA",availableErrorCodesStrs);
  m_CoverageToolArrayEMBC = Monitored::buildToolMap<int>(m_tools,m_CoverageHWGroupName+"EMBC",availableErrorCodesStrs);
  m_CoverageToolArrayEMECC = Monitored::buildToolMap<int>(m_tools,m_CoverageHWGroupName+"EMECC",availableErrorCodesStrs);
  m_CoverageToolArrayHECC = Monitored::buildToolMap<int>(m_tools,m_CoverageHWGroupName+"HECC",availableErrorCodesStrs);
  m_CoverageToolArrayFCalC = Monitored::buildToolMap<int>(m_tools,m_CoverageHWGroupName+"FCalC",availableErrorCodesStrs);

  m_BadChannelToolArrayBarrel = Monitored::buildToolMap<int>(m_tools,m_BadChannelsGroupName+"Barrel",m_Sides);
  m_BadChannelToolArrayEndcap = Monitored::buildToolMap<int>(m_tools,m_BadChannelsGroupName+"EndCap",m_Sides);

  /** End Initialize */
  return AthMonitorAlgorithm::initialize();
}



/*---------------------------------------------------------*/
StatusCode 
LArCoverageAlg::fillHistograms( const EventContext& ctx ) const
{
  ATH_MSG_DEBUG( "in fillHists()" );

  if(ctx.evt()+1 > m_nevents ) return StatusCode::SUCCESS; //ctx.evt() counts from 0

  auto noise = Monitored::Scalar<float>("noise",0.);
  auto etaChan = Monitored::Scalar<float>("etaChan",1e3);
  auto flag = Monitored::Scalar<int>("flag",0);
  auto single_channel = Monitored::Scalar<int>("single_channel",-1);
  auto lb1 = Monitored::Scalar<float>("lb1",0);
  auto lb1_x = Monitored::Scalar<float>("lb1_x",0);

  /** monitoring of coverage maps */
  auto mon_FtSlot = Monitored::Scalar<int>("mon_FtSlot",-1);
  std::vector<LArChanHelp> the_coverageMap(0);
  auto ref_the_coverageMap = std::ref(the_coverageMap);
  //Note for when we'll have the proper histogram class: the feedthrough-slot coverage plot must be filled with the latest value, the eta-phi coverage plot must be filled with the maximum value
  auto mon_ChanFtSlot = Monitored::Collection("mon_ChanFtSlot",ref_the_coverageMap,[](const LArChanHelp& ch){return ch.getChFtSlot();});
  auto mon_Channels = Monitored::Collection("mon_Channels",ref_the_coverageMap,[](const LArChanHelp& ch){return ch.getChNumber();});
  auto mon_Eta = Monitored::Collection("mon_Eta",ref_the_coverageMap,[](const LArChanHelp& ch){return ch.getChEta();});
  auto mon_Phi = Monitored::Collection("mon_Phi",ref_the_coverageMap,[](const LArChanHelp& ch){return ch.getChPhi();});

  //cutmasks for filling the proper partition
  auto mon_isSampling0 = Monitored::Collection("isSampl0",ref_the_coverageMap,[](const LArChanHelp& ch){return (ch.getChSampling()==0);});
  auto mon_isSampling1 = Monitored::Collection("isSampl1",ref_the_coverageMap,[](const LArChanHelp& ch){return (ch.getChSampling()==1);});
  auto mon_isSampling2 = Monitored::Collection("isSampl2",ref_the_coverageMap,[](const LArChanHelp& ch){return (ch.getChSampling()==2);});
  auto mon_isSampling3 = Monitored::Collection("isSampl3",ref_the_coverageMap,[](const LArChanHelp& ch){return (ch.getChSampling()==3);});

  /** Coverage map 
   * each line is a FEB, each column a sampling (needed for eta-phi plots): coverageMapHWEMBA[ft*(Nslot)+slot-1][sampling]=channelStatus. 
   * NOTE the -1 with the slot, needed because slots counts from 1 and vectors want 0. 
   * also: GlobalVariables::slotEMBA=[1,14]-->Nslot=14 (index=slot-1 from 0 to 13), while feedthroughEMBS=[0,31]-->Nfeedthrough=32. 
   */
  std::map<int,std::map<std::string,std::vector<LArChanHelp> > > coverageMap;
  for(auto code : m_availableErrorCodes) {
    for(auto part : m_CoverageBarrelPartitions) coverageMap[code][part] = std::vector<LArChanHelp>(0);
    for(auto part : m_CoverageEndcapPartitions) coverageMap[code][part] = std::vector<LArChanHelp>(0);
  }

  /** known problematic FEB array, used to avoid retrieving FEB information for each channel*/
  std::vector<long> knownDeadFEBs(0);
  std::vector<long> knownErrorFEBs(0);

  /** retrieve det. description manager */
  SG::ReadCondHandle<CaloDetDescrManager> caloMgrHandle{m_caloMgrKey,ctx};
  ATH_CHECK(caloMgrHandle.isValid());
  const CaloDetDescrManager* ddman = *caloMgrHandle;

  /** retrieve cabling (copied from LArCalibUtils/src/LArAutoCorrExtrapolate.cxx) */
  SG::ReadCondHandle<LArOnOffIdMapping> cablingHdl{m_cablingKey,ctx};
  const LArOnOffIdMapping* larCabling{*cablingHdl};
  if(!larCabling){
    ATH_MSG_ERROR("Do not have mapping object " << m_cablingKey.key() );
    return StatusCode::FAILURE;
  }

  /** Retrieve Raw Channels Container */
  SG::ReadHandle<LArRawChannelContainer> pRawChannelsContainer(m_rawChannelsKey, ctx);
  if(! pRawChannelsContainer.isValid() ) {
    ATH_MSG_WARNING( "Can\'t retrieve LArRawChannelContainer with key " << m_rawChannelsKey );
    return StatusCode::SUCCESS;
  }

  /** Retrieve CaloNoise */
  SG::ReadCondHandle<CaloNoise> noiseHdl{m_noiseCDOKey, ctx};
  const CaloNoise* noiseCDO=*noiseHdl;

  if(ctx.evt() == 0){ //first event
    lb1 = (float)GetEventInfo(ctx)->lumiBlock();
    fill(m_CaloNoiseToolGroupName,lb1,lb1_x);
  }


  /** Retrieve BadChannels */
  SG::ReadCondHandle<LArBadChannelCont> bch{m_BCKey,ctx};
  const LArBadChannelCont* bcCont{*bch};  


  ATH_MSG_DEBUG( "collect known faulty FEBs" );
  SG::ReadCondHandle<LArBadFebCont> bf{m_BFKey,ctx};
  const LArBadFebCont* mfCont{*bf};
  if(!mfCont) ATH_MSG_WARNING( "Do not have Missing FEBs container !!" );
  else { 
    for (std::vector<HWIdentifier>::const_iterator allFeb = m_LArOnlineIDHelper->feb_begin();allFeb != m_LArOnlineIDHelper->feb_end(); ++allFeb) {
      HWIdentifier febid = HWIdentifier(*allFeb);
      const LArBadFeb febStatus = mfCont->status(febid);
      if (febStatus.deadAll() || febStatus.deadReadout()) knownDeadFEBs.push_back(febid.get_compact());
      if(febStatus.inError()) knownErrorFEBs.push_back(febid.get_compact());
    }
  }


  ATH_MSG_DEBUG( "now loop on channels" );
  /** Loop over LArRawChannels */
  for (const LArRawChannel& pRawChannel : *pRawChannelsContainer) {
    int provenanceChan  = pRawChannel.provenance();
    float energyChan  = pRawChannel.energy();
    HWIdentifier id  = pRawChannel.hardwareID();
    Identifier offlineID = larCabling->cnvToIdentifier(id);
    
    /** Skip disconnected channels */
    if(!larCabling->isOnlineConnected(id)) continue;
    
    /** Get ft/slot info */
    HWIdentifier febID = m_LArOnlineIDHelper->feb_Id(id);
    HWIdentifier feedthroughID = m_LArOnlineIDHelper->feedthrough_Id(id);
    single_channel = m_LArOnlineIDHelper->channel(id);
    int slot = m_LArOnlineIDHelper->slot(febID);
    int ft = m_LArOnlineIDHelper->feedthrough(feedthroughID);
    
    /** Get Physical Coordinates */
    float phiChan = 0.;
    const CaloDetDescrElement* caloDetElement = ddman->get_element(offlineID);
    if(caloDetElement == 0 ){
      ATH_MSG_ERROR( "Cannot retrieve (eta,phi) coordinates for raw channels" );
      continue; 
    }else{
      etaChan = caloDetElement->eta_raw();
      phiChan = caloDetElement->phi_raw();
    }
    
    /** Fix phi range in HEC */
    if (m_LArOnlineIDHelper->isHECchannel(id)) phiChan = CaloPhiRange::fix(phiChan);
    
    /** Retrieve expected noise */
    noise = noiseCDO->getNoise(offlineID,m_highestGain[caloDetElement->getSubCalo()]); 
    
    if(ctx.evt() == 0){ //first event
      /** Plot the average expected noise vs eta for reference */
      std::string cnGroup_toFill="";
      if(m_LArOnlineIDHelper->isEMBchannel(id) || m_LArOnlineIDHelper->isEMECchannel(id)){
	int sampling = m_LArEM_IDHelper->sampling(offlineID);
	if(sampling>=0 && sampling<m_Nsample) fill(m_tools[m_CaloNoiseGroupArrEM.at(sampling)],etaChan,noise);
	else ATH_MSG_WARNING( "LAr IDhelper returned unexpected sampling: " << sampling << ". Group EM could not be filled.");
      }
      if(m_LArOnlineIDHelper->isHECchannel(id)){
	int sampling = m_LArHEC_IDHelper->sampling(offlineID);
	if(sampling>=0 && sampling<m_Nsample) fill(m_tools[m_CaloNoiseGroupArrHEC.at(sampling)],etaChan,noise);
	else ATH_MSG_WARNING( "LAr IDhelper returned unexpected sampling: " << sampling << ". Group HEC could not be filled.");
      }
      if(m_LArOnlineIDHelper->isFCALchannel(id)){
	int sampling = m_LArFCAL_IDHelper->module(offlineID);
	if(sampling>=0 && sampling<m_Nsample) fill(m_tools[m_CaloNoiseGroupArrFCAL.at(sampling)],etaChan,noise);
	else ATH_MSG_WARNING( "LAr IDhelper returned unexpected sampling: " << sampling << ". Group FCAL could not be filled.");
      }


      /** Fill Bad Channels histograms  */
      flag = DBflag(id,bcCont);
      if (flag!=0) {//only fill bad channels
	std::string the_side= (etaChan >= 0 ? "A" : "C");
	if(m_LArOnlineIDHelper->isEMBchannel(id)){
	  mon_FtSlot=ft*m_NslotEMB+slot;
	  fill(m_tools[m_BadChannelToolArrayBarrel.at(the_side)],mon_FtSlot,single_channel,flag);
	}else{
	  mon_FtSlot=ft*m_NslotEMEC+slot;
	  fill(m_tools[m_BadChannelToolArrayEndcap.at(the_side)],mon_FtSlot,single_channel,flag);
	}
      }
    } //end of 'if(ctx.evt() == 0)'

    
    //
    // Compute cells status
    //

    int cellContent = 0;    

    /** Cell is connected and in the Readout
     * Select raw channels properly reconstructed, with all calib constants available
     * provenance&0x00ff == 0x00a5 : raw channels from OFC iteration, all calib constants found in DB
     * provenance&0x1000 == 0x1000 : raw channels from DSP. If no constant loaded in DSP, energy==0
     */
    if ( (provenanceChan&0x00ff) == 0x00a5 || (provenanceChan&0x1000) == 0x1000 ){

      if(m_bcMask.cellShouldBeMasked(bcCont,id)) cellContent=2;
      else if(energyChan != 0) cellContent=3;
    }

    /** Now check if it's known issue 
     * Known missing FEB: set content to 1
     * Known missing FEB but cells actually readout: set content to 4
     * Known FEB with error: set content to 0 (CaloCells not produced)
     */
    if(knownDeadFEBs.size()>0 && std::find(knownDeadFEBs.begin(), knownDeadFEBs.end(), febID.get_compact())!=knownDeadFEBs.end()) {
      if(cellContent==0) cellContent=1;
      else cellContent=4;
    }
    if(knownErrorFEBs.size()>0 && std::find(knownErrorFEBs.begin(), knownErrorFEBs.end(), febID.get_compact())!=knownErrorFEBs.end())cellContent=1;

    /** Fill Coverage maps */
    const auto cellStatusCodeItr = std::find(m_availableErrorCodes.begin(),
                                             m_availableErrorCodes.end(),
                                             cellContent);
    if(cellStatusCodeItr!=m_availableErrorCodes.end()) {
      std::string part;
      int sampling=-1;
      int i_ftslot=-1;
      double etaFCal=0.;
      double phiFCal=0.;
      //set the variables for partition, sampling etc.

      /** EM Barrel */
      if(m_LArOnlineIDHelper->isEMBchannel(id) ) {
	part="EMB";
	sampling = m_LArEM_IDHelper->sampling(offlineID);
	i_ftslot=ft*m_NslotEMB+slot-1;
      }
      /** EM Endcap */
      if(m_LArOnlineIDHelper->isEMECchannel(id)) {
	part="EMEC";
	sampling = m_LArEM_IDHelper->sampling(offlineID);
	i_ftslot=ft*m_NslotEMEC+slot-1;
      }
      /** HEC */
      if (m_LArOnlineIDHelper->isHECchannel(id)) {
	part="HEC";
	sampling = m_LArHEC_IDHelper->sampling(offlineID);
	i_ftslot=ft*m_NslotHEC+slot-1;
      }
      /** FCAL */
      if (m_LArOnlineIDHelper->isFCALchannel(id)) {
	part="FCal";
	sampling = m_LArFCAL_IDHelper->module(offlineID); 
	i_ftslot=ft*m_NslotFCAL+slot-1;
	etaFCal = m_LArFCAL_IDHelper->eta(offlineID);
	phiFCal = m_LArFCAL_IDHelper->phi(offlineID);
      }

      //set A-C side
      if(etaChan >= 0) part+="A";
      else part+="C";
      
      if(part.find("FCal") != std::string::npos) coverageMap[cellContent][part].push_back(LArChanHelp(single_channel,i_ftslot,sampling,etaFCal,phiFCal));
      else coverageMap[cellContent][part].push_back(LArChanHelp(single_channel,i_ftslot,sampling,etaChan,phiChan));
    }//end of 'if cellContent in availableErrors'
  }// end Raw Channels Loop


  /** now fill the plots */  

  ATH_MSG_DEBUG( "now fill coverage plots");

  for (const auto& chanStatusCodePair : m_availableErrorCodesPairs) {
    //EMBA
    ref_the_coverageMap=coverageMap[chanStatusCodePair.first]["EMBA"];
    if(ref_the_coverageMap.get().size()!=0) fill(m_tools[m_CoverageToolArrayEMBA.at(chanStatusCodePair.second)],mon_Channels,mon_ChanFtSlot,mon_Eta,mon_Phi,mon_isSampling0,mon_isSampling1,mon_isSampling2,mon_isSampling3);
    //EMBC
    ref_the_coverageMap=coverageMap[chanStatusCodePair.first]["EMBC"];
    if(ref_the_coverageMap.get().size()!=0) fill(m_tools[m_CoverageToolArrayEMBC.at(chanStatusCodePair.second)],mon_Channels,mon_ChanFtSlot,mon_Eta,mon_Phi,mon_isSampling0,mon_isSampling1,mon_isSampling2,mon_isSampling3);
  
    //EMECA
    ref_the_coverageMap=coverageMap[chanStatusCodePair.first]["EMECA"];
    if(ref_the_coverageMap.get().size()!=0) fill(m_tools[m_CoverageToolArrayEMECA.at(chanStatusCodePair.second)],mon_Channels,mon_ChanFtSlot,mon_Eta,mon_Phi,mon_isSampling0,mon_isSampling1,mon_isSampling2,mon_isSampling3);

    //EMECC
    ref_the_coverageMap=coverageMap[chanStatusCodePair.first]["EMECC"];
    if(ref_the_coverageMap.get().size()!=0) fill(m_tools[m_CoverageToolArrayEMECC.at(chanStatusCodePair.second)],mon_Channels,mon_ChanFtSlot,mon_Eta,mon_Phi,mon_isSampling0,mon_isSampling1,mon_isSampling2,mon_isSampling3);

    //HECA
    ref_the_coverageMap=coverageMap[chanStatusCodePair.first]["HECA"];
    if(ref_the_coverageMap.get().size()!=0) fill(m_tools[m_CoverageToolArrayHECA.at(chanStatusCodePair.second)],mon_Channels,mon_ChanFtSlot,mon_Eta,mon_Phi,mon_isSampling0,mon_isSampling1,mon_isSampling2,mon_isSampling3);

    //HECC
    ref_the_coverageMap=coverageMap[chanStatusCodePair.first]["HECC"];
    if(ref_the_coverageMap.get().size()!=0) fill(m_tools[m_CoverageToolArrayHECC.at(chanStatusCodePair.second)],mon_Channels,mon_ChanFtSlot,mon_Eta,mon_Phi,mon_isSampling0,mon_isSampling1,mon_isSampling2,mon_isSampling3);
    
    //FCalA
    ref_the_coverageMap=coverageMap[chanStatusCodePair.first]["FCalA"];
    if(ref_the_coverageMap.get().size()!=0) fill(m_tools[m_CoverageToolArrayFCalA.at(chanStatusCodePair.second)],mon_Channels,mon_ChanFtSlot,mon_Eta,mon_Phi,mon_isSampling0,mon_isSampling1,mon_isSampling2,mon_isSampling3);

    //FCalC
    ref_the_coverageMap=coverageMap[chanStatusCodePair.first]["FCalC"];
    if(ref_the_coverageMap.get().size()!=0) fill(m_tools[m_CoverageToolArrayFCalC.at(chanStatusCodePair.second)],mon_Channels,mon_ChanFtSlot,mon_Eta,mon_Phi,mon_isSampling0,mon_isSampling1,mon_isSampling2,mon_isSampling3);

  }

  return StatusCode::SUCCESS; 
}




/*---------------------------------------------------------*/
int LArCoverageAlg::DBflag(HWIdentifier onID, const LArBadChannelCont* bcCont) const
{
  if(!bcCont) {
    ATH_MSG_WARNING( "Do not have Bad chan container " << m_BCKey.key() );
    return -1;
  }

  LArBadChannel bc = bcCont->status(onID);

  int flag = 0;
  if(bc.deadCalib()) flag = 1;
  if(bc.lowNoiseHG()||bc.lowNoiseMG()||bc.lowNoiseLG()) flag = 2;
  if(bc.distorted()) flag = 3;
  if(bc.unstable()) flag = 4;
  if(bc.sporadicBurstNoise())  flag = 5; 
  if(bc.highNoiseHG()|| bc.highNoiseMG() || bc.highNoiseLG())  flag = 6; 
  if(bc.deadReadout()||bc.deadPhys()||bc.almostDead()||bc.shortProblem()) flag = 7;

  return flag;
  }



