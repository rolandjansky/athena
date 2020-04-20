/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
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
  if(warn_binning!="") ATH_MSG_WARNING(warn_binning+"Did you remember to adjust the white bin filling? The algorithm fills in white eta-phi bins in EMB1, EMEC2 and HEC coverage plots: this expects 256 phi bins. If you change the number of bins, you should change the white bin filling as well. Tip: look at the fill() call where deltaPhi is not zero");

  /** Retrieve ID helpers */
  ATH_CHECK(  detStore()->retrieve( m_caloIdMgr ) );
  m_LArEM_IDHelper   = m_caloIdMgr->getEM_ID();
  m_LArHEC_IDHelper  = m_caloIdMgr->getHEC_ID();
  m_LArFCAL_IDHelper = m_caloIdMgr->getFCAL_ID();
   
  ATH_CHECK( detStore()->retrieve(m_LArOnlineIDHelper, "LArOnlineID") );
  ATH_CHECK( m_BCKey.initialize() );
  ATH_CHECK( m_BFKey.initialize() );
  /** retrieve bad channel tool */
  ATH_CHECK( m_badChannelMask.retrieve() );

  /** Initialize cabling key */
  ATH_CHECK(m_cablingKey.initialize());
   

  /** calo noise key */
  ATH_CHECK( m_noiseCDOKey.initialize() );

  /** raw channel key */
  ATH_CHECK( m_rawChannelsKey.initialize() ); 

  /** tool maps (arrays of histograms) */
  m_CaloNoiseGroupArrEM = Monitored::buildToolMap<int>(m_tools,m_CaloNoiseToolGroupName+"EM",m_Nsample);
  m_CaloNoiseGroupArrHEC = Monitored::buildToolMap<int>(m_tools,m_CaloNoiseToolGroupName+"HEC",m_Nsample);
  m_CaloNoiseGroupArrFCAL = Monitored::buildToolMap<int>(m_tools,m_CaloNoiseToolGroupName+"FCAL",m_Nsample);
  m_CoverageHWToolArrayBarrel = Monitored::buildToolMap<int>(m_tools,m_CoverageHWGroupName,m_CoverageBarrelPartitions);
  m_CoverageHWToolArrayEndcap = Monitored::buildToolMap<int>(m_tools,m_CoverageHWGroupName,m_CoverageEndcapPartitions);

  m_BadChannelToolArrayBarrel = Monitored::buildToolMap<int>(m_tools,m_BadChannelsGroupName,m_Sides);
  m_BadChannelToolArrayEndcap = Monitored::buildToolMap<int>(m_tools,m_BadChannelsGroupName,m_Sides);

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
  //Note for when we'll have the proper histogram class: the feedthrough-slot coverage plot must be filled with the latest value, the eta-phi coverage plot must be filled with the maximum value
  auto mon_coverage = Monitored::Collection("mon_coverage",the_coverageMap,[](const LArChanHelp ch){return ch.getChStatus();});
  auto mon_Channels = Monitored::Collection("mon_Channels",the_coverageMap,[](const LArChanHelp ch){return ch.getChNumber();});
  auto mon_Eta = Monitored::Collection("mon_Eta",the_coverageMap,[](const LArChanHelp ch){return ch.getChEta();});
  double deltaPhi=0;
  auto mon_Phi = Monitored::Collection("mon_Phi",the_coverageMap,[&deltaPhi](const LArChanHelp ch){return ch.getChPhi()+deltaPhi;});

  /** lambda functions and arrays for manipulating the_coverageMap when filling the white bins, defined here since they will be called twice */ 
  auto etaRangeEMB1_lambda = [](const LArChanHelp ch){return !(std::abs(ch.getChEta())<1.4);};
  const int n_binSteps = 3;
  int binStepsEM_A[n_binSteps] = {-1,+1,+2};
  int binStepsEM_C[n_binSteps] = {-2,-1,+1};
  auto etaRangeEMEC2andHEC_lambda = [](const LArChanHelp ch){return !(std::abs(ch.getChEta())>2.5);};
  int binStepsHEC = -1;

  /** Coverage map 
   * each line is a FEB, each column a sampling (needed for eta-phi plots): coverageMapHWEMBA[ft*(Nslot)+slot-1][sampling]=channelStatus. 
   * NOTE the -1 with the slot, needed because slots counts from 1 and vectors want 0. 
   * also: GlobalVariables::slotEMBA=[1,14]-->Nslot=14 (index=slot-1 from 0 to 13), while feedthroughEMBS=[0,31]-->Nfeedthrough=32. 
   */
  std::vector<std::vector<std::vector<LArChanHelp> > > coverageMapEMBA(m_NftEMB*m_NslotEMB,std::vector<std::vector<LArChanHelp> >(m_Nsample,std::vector<LArChanHelp>(0)));
  std::vector<std::vector<std::vector<LArChanHelp> > > coverageMapEMBC(m_NftEMB*m_NslotEMB,std::vector<std::vector<LArChanHelp> >(m_Nsample,std::vector<LArChanHelp>(0)));
  std::vector<std::vector<std::vector<LArChanHelp> > > coverageMapEMECA(m_NftEMEC*m_NslotEMEC,std::vector<std::vector<LArChanHelp> >(m_Nsample,std::vector<LArChanHelp>(0)));
  std::vector<std::vector<std::vector<LArChanHelp> > > coverageMapEMECC(m_NftEMEC*m_NslotEMEC,std::vector<std::vector<LArChanHelp> >(m_Nsample,std::vector<LArChanHelp>(0)));
  std::vector<std::vector<std::vector<LArChanHelp> > > coverageMapHECA(m_NftHEC*m_NslotHEC,std::vector<std::vector<LArChanHelp> >(m_Nsample,std::vector<LArChanHelp>(0)));
  std::vector<std::vector<std::vector<LArChanHelp> > > coverageMapHECC(m_NftHEC*m_NslotHEC,std::vector<std::vector<LArChanHelp> >(m_Nsample,std::vector<LArChanHelp>(0)));
  std::vector<std::vector<std::vector<LArChanHelp> > > coverageMapFCALA(m_NftFCAL*m_NslotFCAL,std::vector<std::vector<LArChanHelp> >(m_Nsample,std::vector<LArChanHelp>(0)));
  std::vector<std::vector<std::vector<LArChanHelp> > > coverageMapFCALC(m_NftFCAL*m_NslotFCAL,std::vector<std::vector<LArChanHelp> >(m_Nsample,std::vector<LArChanHelp>(0)));


  /** retrieve det. description manager */
  const CaloDetDescrManager* ddman = nullptr;
  ATH_CHECK( detStore()->retrieve (ddman, "CaloMgr") );

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


      /*THIS WILL BE MODIFIED ONCE WE HAVE FILL SOLUTION FOR 'SETBINCONTENT' PROBLEM, FROM CENTRAL MONITORING*/

      /** Fill Bad Channels histograms  */
      flag = DBflag(id);
      if (flag!=0) {//only fill bad channels
	std::string the_side= (etaChan >= 0 ? "A" : "C");
	if(m_LArOnlineIDHelper->isEMBchannel(id)){
	  mon_FtSlot=ft*m_NftEMB+slot;
	  fill(m_tools[m_BadChannelToolArrayBarrel.at(the_side)],mon_FtSlot,single_channel,flag);
	}else{
	  mon_FtSlot=ft*m_NftEMEC+slot;
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

      if(m_badChannelMask->cellShouldBeMasked(id)) cellContent=2;
      else if(energyChan != 0) cellContent=3;
      }


    /** Fill Coverage maps */
    /** A-Side */
    if(etaChan >= 0){
      /** EM Barrel */
      if(m_LArOnlineIDHelper->isEMBchannel(id) ) {
	int sampling = m_LArEM_IDHelper->sampling(offlineID);
	coverageMapEMBA[ft*m_NslotEMB+slot-1][sampling].push_back(LArChanHelp(single_channel,cellContent,etaChan,phiChan));
      }
      /** EM Endcap */
      if(m_LArOnlineIDHelper->isEMECchannel(id)) {
	int sampling = m_LArEM_IDHelper->sampling(offlineID);
	coverageMapEMECA[ft*m_NslotEMEC+slot-1][sampling].push_back(LArChanHelp(single_channel,cellContent,etaChan,phiChan));
      }
      /** HEC */
      if (m_LArOnlineIDHelper->isHECchannel(id)) {
	int sampling = m_LArHEC_IDHelper->sampling(offlineID);
	coverageMapHECA[ft*m_NslotHEC+slot-1][sampling].push_back(LArChanHelp(single_channel,cellContent,etaChan,phiChan));
      }
      /** FCAL */
      if (m_LArOnlineIDHelper->isFCALchannel(id)) {
	int sampling = m_LArFCAL_IDHelper->module(offlineID); 
	double etaFCal = m_LArFCAL_IDHelper->eta(offlineID);
	double phiFCal = m_LArFCAL_IDHelper->phi(offlineID);
	coverageMapFCALA[ft*m_NslotFCAL+slot-1][sampling].push_back(LArChanHelp(single_channel,cellContent,etaFCal,phiFCal));
      }
      /** C-Side */
    }else{
      /** EM Barrel */
      if(m_LArOnlineIDHelper->isEMBchannel(id) ) {
	int sampling = m_LArEM_IDHelper->sampling(offlineID);
	coverageMapEMBC[ft*m_NslotEMB+slot-1][sampling].push_back(LArChanHelp(single_channel,cellContent,etaChan,phiChan));
      }
      /** EM Endcap */
      if(m_LArOnlineIDHelper->isEMECchannel(id)) {
	int sampling = m_LArEM_IDHelper->sampling(offlineID);
	coverageMapEMECC[ft*m_NslotEMEC+slot-1][sampling].push_back(LArChanHelp(single_channel,cellContent,etaChan,phiChan));
      }
      /** HEC */
      if (m_LArOnlineIDHelper->isHECchannel(id)) {
	int sampling = m_LArHEC_IDHelper->sampling(offlineID);
	coverageMapHECC[ft*m_NslotHEC+slot-1][sampling].push_back(LArChanHelp(single_channel,cellContent,etaChan,phiChan));
      }
      /** FCAL */
      if (m_LArOnlineIDHelper->isFCALchannel(id)) {
	int sampling = m_LArFCAL_IDHelper->module(offlineID); 
	double etaFCal = m_LArFCAL_IDHelper->eta(offlineID);
	double phiFCal = m_LArFCAL_IDHelper->phi(offlineID);
	coverageMapFCALC[ft*m_NslotFCAL+slot-1][sampling].push_back(LArChanHelp(single_channel,cellContent,etaFCal,phiFCal));
      }
    }
  }// end Raw Channels Loop
  

  /** now correct for missing febs and pass the coverage maps to the fill() method. */

  SG::ReadCondHandle<LArBadFebCont> bf{m_BFKey,ctx};
  const LArBadFebCont* mfCont{*bf};
  ATH_MSG_DEBUG( "now check missing FEBS");
  if(!mfCont) { 
    ATH_MSG_WARNING( "Do not have Missing FEBs container !!" );
    /** simply fill in what you have */
      //EMB
      for(int i_feb=0;i_feb<m_NftEMB*m_NslotEMB;i_feb++) {
	mon_FtSlot=i_feb;
	for(int i_sam=0;i_sam<m_Nsample;i_sam++) {
	  the_coverageMap.clear();
	  the_coverageMap=coverageMapEMBA[i_feb][i_sam];
	  fill(m_tools[m_CoverageHWToolArrayBarrel.at("EMBA")],mon_Channels,mon_FtSlot,mon_coverage);
	  deltaPhi=0;
	  fill(m_CoverageHWGroupName+"EMBA"+std::to_string(i_sam),mon_Eta,mon_Phi,mon_coverage);
	  //fill in white bins
	  if(i_sam==1) {
	    the_coverageMap.erase(std::remove_if(the_coverageMap.begin(),the_coverageMap.end(),etaRangeEMB1_lambda),the_coverageMap.end()); //remove eta region which doesn't need additional fill
	    for(int i_step=0;i_step<n_binSteps;i_step++) {
	      deltaPhi=(2*TMath::Pi()/m_NphiBinsEMB1)*binStepsEM_A[i_step];
	      fill(m_CoverageHWGroupName+"EMBA"+std::to_string(i_sam),mon_Eta,mon_Phi,mon_coverage);
	    }
	  } //done filling the white bins
	  the_coverageMap.clear();
	  the_coverageMap=coverageMapEMBC[i_feb][i_sam];
	  fill(m_tools[m_CoverageHWToolArrayBarrel.at("EMBC")],mon_Channels,mon_FtSlot,mon_coverage);
	  deltaPhi=0;
	  fill(m_CoverageHWGroupName+"EMBC"+std::to_string(i_sam),mon_Eta,mon_Phi,mon_coverage);
	  //fill in white bins
	  if(i_sam==1) {
	    the_coverageMap.erase(std::remove_if(the_coverageMap.begin(),the_coverageMap.end(),etaRangeEMB1_lambda),the_coverageMap.end()); //remove eta region which doesn't need additional fill
	    for(int i_step=0;i_step<n_binSteps;i_step++) {
	      deltaPhi=(2*TMath::Pi()/m_NphiBinsEMB1)*binStepsEM_C[i_step];
	      fill(m_CoverageHWGroupName+"EMBC"+std::to_string(i_sam),mon_Eta,mon_Phi,mon_coverage);
	    }
	  } //done filling the white bins
	}
      }
      //EMEC
      for(int i_feb=0;i_feb<m_NftEMEC*m_NslotEMEC;i_feb++) {
	mon_FtSlot=i_feb;
	for(int i_sam=0;i_sam<m_Nsample;i_sam++) {
	  the_coverageMap.clear();
	  the_coverageMap=coverageMapEMECA[i_feb][i_sam];
	  fill(m_tools[m_CoverageHWToolArrayEndcap.at("EMECA")],mon_Channels,mon_FtSlot,mon_coverage);
	  deltaPhi=0;
	  fill(m_CoverageHWGroupName+"EMECA"+std::to_string(i_sam),mon_Eta,mon_Phi,mon_coverage);
	  //fill in white bins
	  if(i_sam==2) {
	    the_coverageMap.erase(std::remove_if(the_coverageMap.begin(),the_coverageMap.end(),etaRangeEMEC2andHEC_lambda),the_coverageMap.end()); //remove eta region which doesn't need additional fill
	    for(int i_step=0;i_step<n_binSteps;i_step++) {
	      deltaPhi=(2*TMath::Pi()/m_NphiBinsEMEC2)*binStepsEM_A[i_step];
	      fill(m_CoverageHWGroupName+"EMECA"+std::to_string(i_sam),mon_Eta,mon_Phi,mon_coverage);
	    }
	  } //done filling the white bins
	  the_coverageMap.clear();
	  the_coverageMap=coverageMapEMECC[i_feb][i_sam];
	  fill(m_tools[m_CoverageHWToolArrayEndcap.at("EMECC")],mon_Channels,mon_FtSlot,mon_coverage);
	  deltaPhi=0;
	  fill(m_CoverageHWGroupName+"EMECC"+std::to_string(i_sam),mon_Eta,mon_Phi,mon_coverage);
	  //fill in white bins
	  if(i_sam==2) {
	    the_coverageMap.erase(std::remove_if(the_coverageMap.begin(),the_coverageMap.end(),etaRangeEMEC2andHEC_lambda),the_coverageMap.end()); //remove eta region which doesn't need additional fill
	    for(int i_step=0;i_step<n_binSteps;i_step++) {
	      deltaPhi=(2*TMath::Pi()/m_NphiBinsEMEC2)*binStepsEM_C[i_step];
	      fill(m_CoverageHWGroupName+"EMECC"+std::to_string(i_sam),mon_Eta,mon_Phi,mon_coverage);
	    }
	  } //done filling the white bins
	}
      }
      //HEC
      for(int i_feb=0;i_feb<m_NftHEC*m_NslotHEC;i_feb++) {
	mon_FtSlot=i_feb;
	for(int i_sam=0;i_sam<m_Nsample;i_sam++) {
	  the_coverageMap.clear();
	  the_coverageMap=coverageMapHECA[i_feb][i_sam];
	  fill(m_tools[m_CoverageHWToolArrayEndcap.at("HECA")],mon_Channels,mon_FtSlot,mon_coverage);
	  deltaPhi=0;
	  fill(m_CoverageHWGroupName+"HECA"+std::to_string(i_sam),mon_Eta,mon_Phi,mon_coverage);
	  //fill in white bins
	  the_coverageMap.erase(std::remove_if(the_coverageMap.begin(),the_coverageMap.end(),etaRangeEMEC2andHEC_lambda),the_coverageMap.end()); //remove eta region which doesn't need additional fill
	  deltaPhi=(2*TMath::Pi()/m_NphiBinsHEC[i_sam])*binStepsHEC;
	  fill(m_CoverageHWGroupName+"HECA"+std::to_string(i_sam),mon_Eta,mon_Phi,mon_coverage);
	  //done filling the white bins

	  the_coverageMap.clear();
	  the_coverageMap=coverageMapHECC[i_feb][i_sam];
	  fill(m_tools[m_CoverageHWToolArrayEndcap.at("HECC")],mon_Channels,mon_FtSlot,mon_coverage);
	  deltaPhi=0;
	  fill(m_CoverageHWGroupName+"HECC"+std::to_string(i_sam),mon_Eta,mon_Phi,mon_coverage);
//fill in white bins
	  the_coverageMap.erase(std::remove_if(the_coverageMap.begin(),the_coverageMap.end(),etaRangeEMEC2andHEC_lambda),the_coverageMap.end()); //remove eta region which doesn't need additional fill
	  deltaPhi=(2*TMath::Pi()/m_NphiBinsHEC[i_sam])*binStepsHEC;
	  fill(m_CoverageHWGroupName+"HECC"+std::to_string(i_sam),mon_Eta,mon_Phi,mon_coverage);
	  //done filling the white bins
	}
      }
      //FCAL
      for(int i_feb=0;i_feb<m_NftFCAL*m_NslotFCAL;i_feb++) {
	mon_FtSlot=i_feb;
	for(int i_sam=1;i_sam<m_Nsample;i_sam++) { //starting from 1 in fcal
	  the_coverageMap=coverageMapFCALA[i_feb][i_sam];
	  fill(m_tools[m_CoverageHWToolArrayEndcap.at("FCalA")],mon_Channels,mon_FtSlot,mon_coverage);
	  deltaPhi=0;
	  fill(m_CoverageHWGroupName+"FCalA"+std::to_string(i_sam),mon_Eta,mon_Phi,mon_coverage);
	  the_coverageMap=coverageMapFCALC[i_feb][i_sam];
	  fill(m_tools[m_CoverageHWToolArrayEndcap.at("FCalC")],mon_Channels,mon_FtSlot,mon_coverage);
	  deltaPhi=0;
	  fill(m_CoverageHWGroupName+"FCalC"+std::to_string(i_sam),mon_Eta,mon_Phi,mon_coverage);
	}
      }
      return StatusCode::SUCCESS;
}

  /** since you have the missing FEB information, apply the correction */
  for (std::vector<HWIdentifier>::const_iterator allFeb = m_LArOnlineIDHelper->feb_begin();allFeb != m_LArOnlineIDHelper->feb_end(); ++allFeb) {
    HWIdentifier febid = HWIdentifier(*allFeb);

    /**
     * Known missing FEB: set content to 1
     * Known missing FEB but cells actually readout: set content to 4
     * Known FEB with error: set content to 0 (CaloCells not produced)
     */

    int barrel_ec = m_LArOnlineIDHelper->barrel_ec(febid);
    int pos_neg   = m_LArOnlineIDHelper->pos_neg(febid);
    int ft        = m_LArOnlineIDHelper->feedthrough(febid);
    int slot      = m_LArOnlineIDHelper->slot(febid);   
    const LArBadFeb febStatus = mfCont->status(febid);

    //Barrel
    if (barrel_ec==0) {
      int i_feb=ft*m_NslotEMB+slot-1;
      mon_FtSlot=i_feb;

      // EMBA
      if (pos_neg==1){
	//check for replacement
	int replace=-100;
	bool isMapEmpty=true;
	for(uint i_sam=0;i_sam<coverageMapEMBA[i_feb].size();i_sam++) {
	  if(coverageMapEMBA[i_feb][i_sam].size()>0) {
	      ATH_MSG_DEBUG( "Feedthrough=" << ft << ", Slot=" << slot << ". Channel checked for replacement: " << (coverageMapEMBA[i_feb][i_sam][1]).getChNumber() );
	    replace = CheckReplacement((coverageMapEMBA[i_feb][i_sam][1]).getChStatus(),febStatus);
	    isMapEmpty=false;
	    break;
	  }
	}
	if(isMapEmpty) continue;
	
	/** replace content if you have to */
        bool doReplace=(replace>-100);
	for(uint i_sam=0;i_sam<coverageMapEMBA[i_feb].size();i_sam++) {
	  if(doReplace) { 
	    for (auto chanElement : coverageMapEMBA[i_feb][i_sam]) chanElement.setChanStatus(replace); //no need to check if connected, since channels not connected are not saved in the map in the first place
	  }
	  
	  /** now that everything is set, fill */
	  the_coverageMap.clear();
	  the_coverageMap=coverageMapEMBA[i_feb][i_sam];
	  ATH_MSG_DEBUG( "fill EMBA: i_feb=" << i_feb << " i_sam=" << i_sam << " size="<< the_coverageMap.size());
	  fill(m_tools[m_CoverageHWToolArrayBarrel.at("EMBA")],mon_Channels,mon_FtSlot,mon_coverage);
	  deltaPhi=0;
	  fill(m_CoverageHWGroupName+"EMBA"+std::to_string(i_sam),mon_Eta,mon_Phi,mon_coverage);
	  //fill in white bins
	  if(i_sam==1) {
	    the_coverageMap.erase(std::remove_if(the_coverageMap.begin(),the_coverageMap.end(),etaRangeEMB1_lambda),the_coverageMap.end()); //remove eta region which doesn't need additional fill
	    for(int i_step=0;i_step<n_binSteps;i_step++) {
	      deltaPhi=(2*TMath::Pi()/m_NphiBinsEMB1)*binStepsEM_A[i_step];
	      fill(m_CoverageHWGroupName+"EMBA"+std::to_string(i_sam),mon_Eta,mon_Phi,mon_coverage);
	    }
	  } //done filling the white bins
	}
      }
      
      // EMBC
      if (pos_neg==0){
	
	//check for replacement
	int replace=-100;
	bool isMapEmpty=true;
	for(uint i_sam=0;i_sam<coverageMapEMBC[i_feb].size();i_sam++) {
	  if(coverageMapEMBC[i_feb][i_sam].size()>0) {
	    replace = CheckReplacement((coverageMapEMBC[i_feb][i_sam][1]).getChStatus(),febStatus);
	    isMapEmpty=false;
	    break;
	  }
	}
	if(isMapEmpty) continue;

	/** replace content if you have to */
	bool doReplace=(replace>-100);
	for(uint i_sam=0;i_sam<coverageMapEMBC[i_feb].size();i_sam++) {
	  if(doReplace) {
	    for (auto chanElement : coverageMapEMBC[i_feb][i_sam]) chanElement.setChanStatus(replace); //no need to check if connected, since channels not connected are not saved in the map in the first place
	  }
	  
	  /** now that everything is set, fill */
	  the_coverageMap.clear();
	  the_coverageMap=coverageMapEMBC[i_feb][i_sam];
	  fill(m_tools[m_CoverageHWToolArrayBarrel.at("EMBC")],mon_Channels,mon_FtSlot,mon_coverage);
	  deltaPhi=0;
	  fill(m_CoverageHWGroupName+"EMBC"+std::to_string(i_sam),mon_Eta,mon_Phi,mon_coverage);
	  //fill in white bins
	  if(i_sam==1) {
	    the_coverageMap.erase(std::remove_if(the_coverageMap.begin(),the_coverageMap.end(),etaRangeEMB1_lambda),the_coverageMap.end()); //remove eta region which doesn't need additional fill
	    for(int i_step=0;i_step<n_binSteps;i_step++) {
	      deltaPhi=(2*TMath::Pi()/m_NphiBinsEMB1)*binStepsEM_C[i_step];
	      fill(m_CoverageHWGroupName+"EMBC"+std::to_string(i_sam),mon_Eta,mon_Phi,mon_coverage);
	    }
	  } //done filling the white bins
	}
      }
    } 
    //Endcap
    if (barrel_ec==1) {
      // First find out if it's EMEC, HEC or FCAL !
      HWIdentifier test_chid = m_LArOnlineIDHelper->channel_Id(febid,1);

      //EMEC
      if(m_LArOnlineIDHelper->isEMECchannel(test_chid)) {
	int i_feb=ft*m_NslotEMEC+slot-1;
	mon_FtSlot=i_feb;

	// EMECA
	if (pos_neg==1){
	  
	  //check for replacement
	  int replace=-100;
	  bool isMapEmpty=true;
	  for(uint i_sam=0;i_sam<coverageMapEMECA[i_feb].size();i_sam++) {
	    if(coverageMapEMECA[i_feb][i_sam].size()>0) {
	      ATH_MSG_DEBUG( "Feedthrough=" << ft << ", Slot=" << slot << ". Channel checked for replacement: " << (coverageMapEMECA[i_feb][i_sam][1]).getChNumber() );
	      replace = CheckReplacement((coverageMapEMECA[i_feb][i_sam][1]).getChStatus(),febStatus);
	      isMapEmpty=false;
	      break;
	    }
	  }
	  if(isMapEmpty) continue;

	  /** replace content if you have to */
	  bool doReplace=(replace>-100);
	  for(uint i_sam=0;i_sam<coverageMapEMECA[i_feb].size();i_sam++) {
	    if(doReplace) {
	      for (auto chanElement : coverageMapEMECA[i_feb][i_sam]) chanElement.setChanStatus(replace); //no need to check if connected, since channels not connected are not saved in the map in the first place
	    }
	    
	    /** now that everything is set, fill */
	    the_coverageMap.clear();
	    the_coverageMap=coverageMapEMECA[i_feb][i_sam];
	    fill(m_tools[m_CoverageHWToolArrayEndcap.at("EMECA")],mon_Channels,mon_FtSlot,mon_coverage);
	    deltaPhi=0;
	    fill(m_CoverageHWGroupName+"EMECA"+std::to_string(i_sam),mon_Eta,mon_Phi,mon_coverage);
	    //fill in white bins
	    if(i_sam==2) {
	      the_coverageMap.erase(std::remove_if(the_coverageMap.begin(),the_coverageMap.end(),etaRangeEMEC2andHEC_lambda),the_coverageMap.end()); //remove eta region which doesn't need additional fill
	      for(int i_step=0;i_step<n_binSteps;i_step++) {
		deltaPhi=(2*TMath::Pi()/m_NphiBinsEMEC2)*binStepsEM_A[i_step];
		fill(m_CoverageHWGroupName+"EMECA"+std::to_string(i_sam),mon_Eta,mon_Phi,mon_coverage);
	      }
	    } //done filling the white bins
	  }
	}

	// EMECC
	if (pos_neg==0){
	  
	  //check for replacement
	  int replace=-100;
	  bool isMapEmpty=true;
	  for(uint i_sam=0;i_sam<coverageMapEMECC[i_feb].size();i_sam++) {
	    if(coverageMapEMECC[i_feb][i_sam].size()>0) {
	      ATH_MSG_DEBUG( "Feedthrough=" << ft << ", Slot=" << slot << ". Channel checked for replacement: " << (coverageMapEMECC[i_feb][i_sam][1]).getChNumber() );
	      replace = CheckReplacement((coverageMapEMECC[i_feb][i_sam][1]).getChStatus(),febStatus);
	      isMapEmpty=false;
	      break;
	    }
	  }
	  if(isMapEmpty) continue;

	  /** replace content if you have to */
	  bool doReplace=(replace>-100);
	  for(uint i_sam=0;i_sam<coverageMapEMECC[i_feb].size();i_sam++) {
	    if(doReplace) {
	      for (auto chanElement : coverageMapEMECC[i_feb][i_sam]) chanElement.setChanStatus(replace); //no need to check if connected, since channels not connected are not saved in the map in the first place
	    }
	    
	    //now that everything is set, fill
	    the_coverageMap.clear();
	    the_coverageMap=coverageMapEMECC[i_feb][i_sam];
	    fill(m_tools[m_CoverageHWToolArrayEndcap.at("EMECC")],mon_Channels,mon_FtSlot,mon_coverage);
	    deltaPhi=0;
	    fill(m_CoverageHWGroupName+"EMECC"+std::to_string(i_sam),mon_Eta,mon_Phi,mon_coverage);
	    //fill in white bins
	    if(i_sam==2) {
	      the_coverageMap.erase(std::remove_if(the_coverageMap.begin(),the_coverageMap.end(),etaRangeEMEC2andHEC_lambda),the_coverageMap.end()); //remove eta region which doesn't need additional fill
	      for(int i_step=0;i_step<n_binSteps;i_step++) {
		deltaPhi=(2*TMath::Pi()/m_NphiBinsEMEC2)*binStepsEM_C[i_step];
		fill(m_CoverageHWGroupName+"EMECC"+std::to_string(i_sam),mon_Eta,mon_Phi,mon_coverage);
	      }
	    } //done filling the white bins
	  }
	}
      }

      //HEC
      if(m_LArOnlineIDHelper->isHECchannel(test_chid)){
	int i_feb=ft*m_NslotHEC+slot-1;
	mon_FtSlot=i_feb;

	// HECA
	if (pos_neg==1){
	  
	  //check for replacement
	  int replace=-100;
	  bool isMapEmpty=true;
	  for(uint i_sam=0;i_sam<coverageMapHECA[i_feb].size();i_sam++) {
	    if(coverageMapHECA[i_feb][i_sam].size()>0) {
	      ATH_MSG_DEBUG( "Feedthrough=" << ft << ", Slot=" << slot << ". Channel checked for replacement: " << (coverageMapHECA[i_feb][i_sam][1]).getChNumber() );
	      replace = CheckReplacement((coverageMapHECA[i_feb][i_sam][1]).getChStatus(),febStatus);
	      isMapEmpty=false;
	      break;
	    }
	  }
	  if(isMapEmpty) continue;

	  /** replace content if you have to */
	  bool doReplace=(replace>-100);
	  for(uint i_sam=0;i_sam<coverageMapHECA[i_feb].size();i_sam++) {
	    if(doReplace) {
	      for (auto chanElement : coverageMapHECA[i_feb][i_sam]) chanElement.setChanStatus(replace); //no need to check if connected, since channels not connected are not saved in the map in the first place
	    }
	    
	    //now that everything is set, fill
	    the_coverageMap.clear();
	    the_coverageMap=coverageMapHECA[i_feb][i_sam];
	    fill(m_tools[m_CoverageHWToolArrayEndcap.at("HECA")],mon_Channels,mon_FtSlot,mon_coverage);
	    deltaPhi=0;
	    fill(m_CoverageHWGroupName+"HECA"+std::to_string(i_sam),mon_Eta,mon_Phi,mon_coverage);
	    //fill in white bins
	    the_coverageMap.erase(std::remove_if(the_coverageMap.begin(),the_coverageMap.end(),etaRangeEMEC2andHEC_lambda),the_coverageMap.end()); //remove eta region which doesn't need additional fill
	    deltaPhi=(2*TMath::Pi()/m_NphiBinsHEC[i_sam])*binStepsHEC;
	    fill(m_CoverageHWGroupName+"HECA"+std::to_string(i_sam),mon_Eta,mon_Phi,mon_coverage);
	    //done filling the white bins
	  }
	}

	// HECC
	if (pos_neg==0){
	  
	  //check for replacement
	  int replace=-100;
	  bool isMapEmpty=true;
	  for(uint i_sam=0;i_sam<coverageMapHECC[i_feb].size();i_sam++) {
	    if(coverageMapHECC[i_feb][i_sam].size()>0) {
	      ATH_MSG_DEBUG( "Feedthrough=" << ft << ", Slot=" << slot << ". Channel checked for replacement: " << (coverageMapHECC[i_feb][i_sam][1]).getChNumber() );
	      replace = CheckReplacement((coverageMapHECC[i_feb][i_sam][1]).getChStatus(),febStatus);
	      isMapEmpty=false;
	      break;
	    }
	  }
	  if(isMapEmpty) continue;

	  /** replace content if you have to */
	  bool doReplace=(replace>-100);
	  for(uint i_sam=0;i_sam<coverageMapHECC[i_feb].size();i_sam++) {
	    if(doReplace) {
	      for (auto chanElement : coverageMapHECC[i_feb][i_sam]) chanElement.setChanStatus(replace); //no need to check if connected, since channels not connected are not saved in the map in the first place
	    }
	    
	    //now that everything is set, fill
	    the_coverageMap.clear();
	    the_coverageMap=coverageMapHECC[i_feb][i_sam];
	    fill(m_tools[m_CoverageHWToolArrayEndcap.at("HECC")],mon_Channels,mon_FtSlot,mon_coverage);
	    deltaPhi=0;
	    fill(m_CoverageHWGroupName+"HECC"+std::to_string(i_sam),mon_Eta,mon_Phi,mon_coverage);
	    //fill in white bins
	    the_coverageMap.erase(std::remove_if(the_coverageMap.begin(),the_coverageMap.end(),etaRangeEMEC2andHEC_lambda),the_coverageMap.end()); //remove eta region which doesn't need additional fill
	    deltaPhi=(2*TMath::Pi()/m_NphiBinsHEC[i_sam])*binStepsHEC;
	    fill(m_CoverageHWGroupName+"HECC"+std::to_string(i_sam),mon_Eta,mon_Phi,mon_coverage);
	    //done filling the white bins
	  }
	}
      }

      //FCAL
      if(m_LArOnlineIDHelper->isFCALchannel(test_chid)){ 
	int i_feb=ft*m_NslotFCAL+slot-1;
	mon_FtSlot=i_feb;

	// FCALA
	if (pos_neg==1){
	  
	  //check for replacement
	  int replace=-100;
	  bool isMapEmpty=true;
	  for(uint i_sam=0;i_sam<coverageMapFCALA[i_feb].size();i_sam++) {
	    if(coverageMapFCALA[i_feb][i_sam].size()>0) {
	      ATH_MSG_DEBUG( "Feedthrough=" << ft << ", Slot=" << slot << ". Channel checked for replacement: " << (coverageMapFCALA[i_feb][i_sam][1]).getChNumber() );
	      replace = CheckReplacement((coverageMapFCALA[i_feb][i_sam][1]).getChStatus(),febStatus);
	      isMapEmpty=false;
	      break;
	    }
	  }
	  if(isMapEmpty) continue;

	  /** replace content if you have to */
	  bool doReplace=(replace>-100);
	  for(uint i_sam=1;i_sam<coverageMapFCALA[i_feb].size();i_sam++) { //starting from 1 in FCal
	    if(doReplace) {
	      for (auto chanElement : coverageMapFCALA[i_feb][i_sam]) chanElement.setChanStatus(replace); //no need to check if connected, since channels not connected are not saved in the map in the first place
	    }
	    
	    //now that everything is set, fill
	    the_coverageMap.clear();
	    the_coverageMap=coverageMapFCALA[i_feb][i_sam];
	    fill(m_tools[m_CoverageHWToolArrayEndcap.at("FCalA")],mon_Channels,mon_FtSlot,mon_coverage);
	    deltaPhi=0;
	    fill(m_CoverageHWGroupName+"FCalA"+std::to_string(i_sam),mon_Eta,mon_Phi,mon_coverage);
	  }
	}

	// FCALC
	if (pos_neg==0){
	  
	  //check for replacement
	  int replace=-100;
	  bool isMapEmpty=true;
	  for(uint i_sam=0;i_sam<coverageMapFCALC[i_feb].size();i_sam++) {
	    if(coverageMapFCALC[i_feb][i_sam].size()>0) {
	      ATH_MSG_DEBUG( "Feedthrough=" << ft << ", Slot=" << slot << ". Channel checked for replacement: " << (coverageMapFCALC[i_feb][i_sam][1]).getChNumber() );
	      replace = CheckReplacement((coverageMapFCALC[i_feb][i_sam][1]).getChStatus(),febStatus);
	      isMapEmpty=false;
	      break;
	    }
	  }
	  if(isMapEmpty) continue;

	  /** replace content if you have to */
	  bool doReplace=(replace>-100);
	  //set content
	  for(uint i_sam=1;i_sam<coverageMapFCALC[i_feb].size();i_sam++) { //starting from 1 in fcal
	    if(doReplace) {
	      for (auto chanElement : coverageMapFCALC[i_feb][i_sam]) chanElement.setChanStatus(replace); //no need to check if connected, since channels not connected are not saved in the map in the first place
	    }
	    
	    //now that everything is set, fill
	    the_coverageMap.clear();
	    the_coverageMap=coverageMapFCALC[i_feb][i_sam];
	    fill(m_tools[m_CoverageHWToolArrayEndcap.at("FCalC")],mon_Channels,mon_FtSlot,mon_coverage);
	    deltaPhi=0;
	    fill(m_CoverageHWGroupName+"FCalC"+std::to_string(i_sam),mon_Eta,mon_Phi,mon_coverage);
	  }
	}
      }
    }
  }// end of FEB loop

  return StatusCode::SUCCESS; 
}


/*---------------------------------------------------------*/
int LArCoverageAlg::CheckReplacement(int content,LArBadFeb febStatus) const
{
  /** check if this FEB needs replacement */
  int replace=-100;
  if (febStatus.deadAll() || febStatus.deadReadout()) {
    if(content==0 || content==1) replace=1;
    else replace=4;
  }
  if(febStatus.inError()) {
    replace=1;
  }
  return replace;
}

/*---------------------------------------------------------*/
int LArCoverageAlg::DBflag(HWIdentifier onID) const
{
  SG::ReadCondHandle<LArBadChannelCont> bch{m_BCKey};
  const LArBadChannelCont* bcCont{*bch};
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



