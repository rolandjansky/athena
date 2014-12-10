/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**    @file SCTRatioNoiseMonTool.cxx
 *
 *    @author Eleanor Davies based on code from Steve McMahon, HelenHayward
 *  
 */
#include "SCT_Monitoring/SCTRatioNoiseMonTool.h"
#include "deletePointers.h"
#include "SCT_NameFormatter.h"
#include "boost/lexical_cast.hpp"
#include <cmath>

#include "GaudiKernel/StatusCode.h"
#include "GaudiKernel/IToolSvc.h"

#include "TH1F.h"
#include "TH2F.h"
#include "TProfile2D.h"
#include "TF1.h"
#include "DataModel/DataVector.h"
#include "Identifier/Identifier.h"
#include "InDetIdentifier/SCT_ID.h"
#include "InDetReadoutGeometry/SCT_DetectorManager.h"
#include "InDetRawData/SCT3_RawData.h"
#include "InDetPrepRawData/SCT_ClusterContainer.h"
#include "AthenaKernel/errorcheck.h"
#include "SCT_Monitoring/SCT_ClusterStruct.h"
#include "GaudiKernel/StatusCode.h"
#include "GaudiKernel/ITHistSvc.h"
#include "GaudiKernel/PropertyMgr.h"

//conditions stuff
#include "InDetConditionsSummaryService/InDetHierarchy.h"
#include "SCT_ConditionsServices/ISCT_ByteStreamErrorsSvc.h"
#include "TMath.h"
#include "TH1F.h"
#include "TH2I.h"
#include "LWHists/TH1F_LW.h"
#include "LWHists/TH2F_LW.h"
#include "TProfile2D.h"
#include "InDetRawData/SCT3_RawData.h"
#include "InDetRawData/InDetRawDataContainer.h"
#include "InDetRawData/InDetRawDataCLASS_DEF.h"
#include "DataModel/DataVector.h"
#include "Identifier/Identifier.h"
#include "InDetIdentifier/SCT_ID.h"
#include "InDetReadoutGeometry/SCT_DetectorManager.h"
#include "InDetConditionsSummaryService/IInDetConditionsSvc.h"
#include "EventInfo/EventID.h"
#include "EventInfo/EventInfo.h"

//
using namespace std;
using namespace SCT_Monitoring;

namespace { //use anonymous namespace to restrict scope to this file, equivalent to 'static' keyword here

  enum Pattern{ IXX=0, XIX, XXI };

  bool timeBinInPattern(const int tbin, const Pattern xxx){
    switch (xxx) {
    case IXX:
      return (tbin>3);
      break;
    case XIX:
      return (tbin==2||tbin==3||tbin==6||tbin==7);
      break;
    case XXI:
      return (tbin==1||tbin==3||tbin==5||tbin==7);
      break;
    default:
      return false;
      break;
    }
  }
  const std::string names[]={"Endcap C","Barrel","Endcap A"};
  const std::string abbreviations[]={"ECC", "Bar","ECA"};

} //end of anonymous namespace

//====================================================================================================
/** Constructor, calls base class constructor with parameters
 *
 *  several properties are "declared" here, allowing selection
 *  of the filepath for histograms, the first and second plane
 *  numbers to be used, and the timebin.
 */
//====================================================================================================
SCTRatioNoiseMonTool::SCTRatioNoiseMonTool(const string & type,
					   const string & name,           
					   const IInterface* parent) : SCTMotherTrigMonTool(type, name, parent),
								       m_pSCTHelper(0),
								       m_sctmgr(0),
								       m_pSummarySvc("SCT_ConditionsSummarySvc", name),
								       m_checkBadModules(true),
								       m_ignore_RDO_cut_online(true)
{
  declareProperty("histoPathBase", m_stream = "/stat");
  declareProperty("conditionsService", m_pSummarySvc);
  declareProperty("checkBadModules", m_checkBadModules);
  declareProperty("CheckRate",m_checkrate=1000);
  declareProperty("CheckRecent",m_checkrecent=20);
  declareProperty("NOTrigger",m_NOTrigger="L1_RD0_EMPTY");
  declareProperty("IgnoreRDOCutOnline",m_ignore_RDO_cut_online);
  m_numberOfEvents=0;

  //General Histograms
  m_side=0;
  m_NZ1=0;
  m_N11=0;
  m_N21=0;
  m_NOb=0;
  m_NO=0;
  m_NOEC=0;
  m_NOEC_Outer=0;
  m_NOEC_ShortMiddle=0;
  m_NOEC_Inner=0;
  m_NOEC_Middle=0; 
  m_NZ1_vs_modnum=0;
  m_N11_vs_modnum=0;
  m_current_lb=0;
  m_last_reset_lb=0;
  
  eventID = 0;


  for (int i = 0; i <n_mod[GENERAL_INDEX]  ; i++) {
    nLayer[i]=0;
    nEta[i]=0;
    nPhi[i]=0;
    nOne[i]=0;
    nZero[i]=0;
    nBoth[i]=0;
    nLink0[i]=0;
    nLink1[i]=0;
    nNonGoodModule[i]=0;
    
    goodModules[i] = true;
  } 
  for (int i = 0; i < n_layers[BARREL_INDEX] ; i++){
    nNoSidesBarrelModule[i]=0;
    nOneSideBarrelModule[i]=0;
    nTwoSideBarrelModule[i]=0;
    nNonGoodModulesBarrelModule[i]=0;
    m_NZ1BAR[i]=0;
    m_N11BAR[i]=0;
    m_N21BAR[i]=0;

    m_NZ1BAR_vsLB[i]=0;
    m_N11BAR_vsLB[i]=0;
    m_NOb_layer_vsLB[i]=0;

    m_NOEV_Barrel[i]=0;
    m_NOb_layer[i]=0;
    m_numberHitsinBarrel[i]=0;
    n1Barrel[i]=0;
    d1Barrel[i]=0;
  }

  for (int i = 0; i < n_layers[ENDCAP_C_INDEX] ; i++){
    m_NOECC_disk[i]=0;
    m_NOECA_disk[i]=0;
    nNoSidesEndcapCModule[i]=0;
    nOneSideEndcapCModule[i]=0;
    nTwoSideEndcapCModule[i]=0;
    nNonGoodModulesEndcapCModule[i]=0;
    nNoSidesEndcapAModule[i]=0;
    nOneSideEndcapAModule[i]=0;
    nTwoSideEndcapAModule[i]=0;
    nNonGoodModulesEndcapAModule[i]=0;
    m_NOEV_EndcapC[i]=0;
    m_NOEV_EndcapA[i]=0;
    m_NZ1ECC[i]=0;
    m_N11ECC[i]=0;
    m_N21ECC[i]=0;
    m_NZ1ECA[i]=0;
    m_N11ECA[i]=0;
    m_N21ECA[i]=0;

    m_NOECC_disk_vsLB[i]=0;
    m_NZ1ECC_vsLB[i]=0;
    m_N11ECC_vsLB[i]=0;

    m_NOECA_disk_vsLB[i]=0;
    m_NZ1ECA_vsLB[i]=0;
    m_N11ECA_vsLB[i]=0;

    n1EndcapA[i]=0;
    n1EndcapC[i]=0;
    d1EndcapA[i]=0;
    d1EndcapC[i]=0;
  }
  noSidesHit=false;
  oneSideHit=false;
  twoSidesHit=false;
  set_timebin = -1;
  tbin=-1;
  nOneSideModule=0;
  nTwoSideModule=0;
  nNoFiredModules=0;
  ratio=-1;
  d1=0;
  n1=0;

}



//====================================================================================================
//====================================================================================================
SCTRatioNoiseMonTool::~SCTRatioNoiseMonTool(){
  //nada
}

//====================================================================================================
//                       SCTRatioNoiseMonTool :: bookHistograms
//====================================================================================================
//StatusCode SCTRatioNoiseMonTool::bookHistograms( bool /*isNewEventsBlock*/, bool isNewLumiBlock, bool isNewRun )  // hidetoshi 14.01.21
StatusCode SCTRatioNoiseMonTool::bookHistogramsRecurrent()                                                          // hidetoshi 14.01.21
{
  m_path= "";
  //  if(isNewRun) m_numberOfEvents=0;                                                           // hidetoshi 14.01.21
  if(newRun) m_numberOfEvents=0;                                                                 // hidetoshi 14.01.21
  m_dataObjectName = "SCT_RDOs";
  if (msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << "initialize being called" << endreq;
  StatusCode sc = detStore()->retrieve(m_pSCTHelper,"SCT_ID");
  if (sc.isFailure()) {
    if (msgLvl(MSG::FATAL)) msg(MSG::FATAL)<< "SCT helper class not found !" << endreq;
    return StatusCode::FAILURE;
  }

  if(m_checkBadModules){
    msg(MSG::INFO) << "Clusterization has been asked to look at bad module info" << endreq;
    if (m_pSummarySvc.retrieve().isFailure()){
      msg(MSG:: ERROR) << "Conditions summary service not found!" << endreq;
      return StatusCode::FAILURE;}
  }

  //get the SCT detector manager
  
  sc = detStore()->retrieve(m_sctmgr,"SCT");
  if (sc.isFailure()) {
    if (msgLvl(MSG::FATAL)) msg(MSG::FATAL)<< "SCT detector manager not found !" << endreq;
    return StatusCode::FAILURE;
  }
  if (msgLvl(MSG::INFO)) msg(MSG::INFO)<< "SCT detector manager found: layout is \"" << m_sctmgr->getLayout() << "\"" << endreq;

  //Booking  Track related Histograms
  //  if(bookRatioNoiseHistos(isNewRun,isNewLumiBlock).isFailure() ) msg(MSG::WARNING) << "Error in bookRatioNoiseHistos()" << endreq;       // hidetoshi 14.01.21
  if(bookRatioNoiseHistos().isFailure() ) msg(MSG::WARNING) << "Error in bookRatioNoiseHistos()" << endreq;                                  // hidetoshi 14.01.22

  return StatusCode::SUCCESS;
}

//====================================================================================================
//                       SCTRatioNoiseMonTool :: bookHistograms
//====================================================================================================
//StatusCode SCTRatioNoiseMonTool::bookHistograms( bool /*isNewEventsBlock*/, bool isNewLumiBlock, bool isNewRun )  // hidetoshi 14.01.21
StatusCode SCTRatioNoiseMonTool::bookHistograms()                                                                   // hidetoshi 14.01.21
{
  m_path= "";
  //  if(isNewRun) m_numberOfEvents=0;                                                           // hidetoshi 14.01.21
  m_numberOfEvents=0;                                                                            // hidetoshi 14.01.21
  m_dataObjectName = "SCT_RDOs";
  if (msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << "initialize being called" << endreq;
  StatusCode sc = detStore()->retrieve(m_pSCTHelper,"SCT_ID");
  if (sc.isFailure()) {
    if (msgLvl(MSG::FATAL)) msg(MSG::FATAL)<< "SCT helper class not found !" << endreq;
    return StatusCode::FAILURE;
  }

  if(m_checkBadModules){
    msg(MSG::INFO) << "Clusterization has been asked to look at bad module info" << endreq;
    if (m_pSummarySvc.retrieve().isFailure()){
      msg(MSG:: ERROR) << "Conditions summary service not found!" << endreq;
      return StatusCode::FAILURE;}
  }

  //get the SCT detector manager
  
  sc = detStore()->retrieve(m_sctmgr,"SCT");
  if (sc.isFailure()) {
    if (msgLvl(MSG::FATAL)) msg(MSG::FATAL)<< "SCT detector manager not found !" << endreq;
    return StatusCode::FAILURE;
  }
  if (msgLvl(MSG::INFO)) msg(MSG::INFO)<< "SCT detector manager found: layout is \"" << m_sctmgr->getLayout() << "\"" << endreq;

  //Booking  Track related Histograms
  //  if(bookRatioNoiseHistos(isNewRun,isNewLumiBlock).isFailure() ) msg(MSG::WARNING) << "Error in bookRatioNoiseHistos()" << endreq;       // hidetoshi 14.01.21
  if(bookRatioNoiseHistos()) msg(MSG::WARNING) << "Error in bookRatioNoiseHistos()" << endreq;                                               // hidetoshi 14.01.22

  return StatusCode::SUCCESS;
}

//====================================================================================================
//                        SCTRatioNoiseMonTool :: fillHistograms
/// This is the real workhorse, called for each event. It retrieves the data each time
//====================================================================================================
StatusCode SCTRatioNoiseMonTool::fillHistograms(){
  if (msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) <<"enters fillHistograms"<< endreq;
  //msg(MSG::DEBUG) <<"enters fillHistograms"<< endreq;
  // lets get the raw hit container
  typedef SCT_RDORawData SCTRawDataType;
  const SCT_RDO_Container* p_rdocontainer; 
  if(evtStore()->retrieve(p_rdocontainer,m_dataObjectName).isFailure()) return StatusCode::FAILURE;
 
  Identifier SCT_Identifier;
  //Use new IDC
  SCT_RDO_Container::const_iterator col_it  = p_rdocontainer->begin();
  SCT_RDO_Container::const_iterator lastCol  = p_rdocontainer->end();
  
  //Declare Time Bin
  set_timebin = 4;
  tbin=-1;

  //Declaring Counting variables
  nOneSideModule=0;
  nTwoSideModule=0;
  nNoFiredModules=0;
  
  //Zero the module counters
  for (int i = 0; i < n_mod[GENERAL_INDEX] ; i++) {
    nLink0[i]=0;
    nLink1[i]=0;

    goodModules[i] = true;
  }  

  for (int i = 0; i < n_layers[BARREL_INDEX] ; i++){
    nNoSidesBarrelModule[i]=0;
    nOneSideBarrelModule[i]=0;
    nTwoSideBarrelModule[i]=0;   
  }

  for (int i = 0; i < n_layers[ENDCAP_C_INDEX] ; i++){
    nNoSidesEndcapCModule[i]=0;
    nOneSideEndcapCModule[i]=0;
    nTwoSideEndcapCModule[i]=0;
    nNoSidesEndcapAModule[i]=0;
    nOneSideEndcapAModule[i]=0;
    nTwoSideEndcapAModule[i]=0;
  }
  
  //EventID * eventID;
  const EventInfo* pEvent(0);
  CHECK (evtStore()->retrieve(pEvent));
  if (not pEvent) return ERROR ("Could not find event pointer"), StatusCode::FAILURE;
  //eventID = pEvent->event_ID();
 
  bool isSelectedTrigger = false;

  //  if(m_doTrigger){
    //    if  (AthenaMonManager::dataType() !=  AthenaMonManager::cosmics) {
    //if (SCTMotherTrigMonTool::isCalibrationNoise(m_NOTrigger)) {
	isSelectedTrigger = true;
	//}
      // }
	//}

  m_current_lb = pEvent->event_ID()->lumi_block();
  int count_SCT_RDO = 0;
  if( isSelectedTrigger == true || m_environment==AthenaMonManager::online){ // 21.11.2014 Fumiaki Ito
    for( ; col_it!= lastCol ; ++col_it){
      correct_TimeBin=false;
      const InDetRawDataCollection<SCTRawDataType>* SCT_Collection(*col_it);
      if (!SCT_Collection) continue;  // select only SCT RDOs
      const InDetRawDataCollection<SCT_RDORawData>* rd(*col_it);

      Identifier SCT_Identifier = SCT_Collection->identify();   
      int thisBec       = m_pSCTHelper->barrel_ec(SCT_Identifier);
      int thisLayerDisk = m_pSCTHelper->layer_disk(SCT_Identifier);
      int thisPhi       = m_pSCTHelper->phi_module(SCT_Identifier);
      int thisEta       = m_pSCTHelper->eta_module(SCT_Identifier);
      int thisSide      = m_pSCTHelper->side(SCT_Identifier);
      //int thisWaferHash = m_pSCTHelper->wafer_hash(SCT_Identifier);
       
      modNum = (rd->identifyHash())/2;

      bool goodModule = (m_checkBadModules and m_pSummarySvc) ? m_pSummarySvc->isGood(rd->identifyHash()) : true;

      goodModules[modNum] = goodModule;
      nLayer[modNum]= thisLayerDisk;
      nEta[modNum]= thisEta;  
      nPhi[modNum]= thisPhi;

      if(!goodModule){
	nNonGoodModule[modNum] = 1;
      }
      DataVector<SCTRawDataType>::const_iterator p_rdo_end= SCT_Collection->end();
      for(DataVector<SCTRawDataType>::const_iterator p_rdo=SCT_Collection->begin() ; p_rdo!=p_rdo_end; ++p_rdo){
	count_SCT_RDO++;
	SCT3_RawData* rdo3 = dynamic_cast<SCT3_RawData*>(*p_rdo);
	if (rdo3!=0) tbin = (rdo3)->getTimeBin();
	//  if (tbin == set_timebin && goodModule ){
	if (timeBinInPattern(tbin,XIX) && goodModule ){
  
	  if (thisBec==0){
	    int layer=thisLayerDisk;
	    m_numberHitsinBarrel[layer]->Fill(thisPhi,1.);
	  }         
	  if(thisSide==1){
	    //modNum = ((thisWaferHash-1)/2);
	    nLink1[modNum]+=1;
	    m_side->Fill(1);
	  } else {
	    //modNum=(thisWaferHash/2);
	    nLink0[modNum]+=1;
	    m_side->Fill(3);
	  }
	}
      }
    }
     
      // ignores the RDO cut online since the empty events are pre-filtered there
    if(count_SCT_RDO<1E6 || (m_ignore_RDO_cut_online && m_environment==AthenaMonManager::online)){
      m_num_RDO->Fill(count_SCT_RDO);    
      for(int j=0;j<n_mod[GENERAL_INDEX]; j++){
	noSidesHit=false;
	oneSideHit=false;
	twoSidesHit=false;
	if(nLink0[j] !=0 && nLink1[j] !=0) {
	  twoSidesHit=true;
	} else {
	  if(nLink0[j] !=0 || nLink1[j] !=0 ){
	    oneSideHit=true;
	  }
	}
	if(nLink0[j] ==0 && nLink1[j] ==0 && goodModules[j]/*no-sides-fix*/) {
	  noSidesHit=true;
	}
	if(isBarrel(j)){
	  if(noSidesHit){
	    nNoSidesBarrelModule[nLayer[j]]++; }
	  if(oneSideHit)
	    nOneSideBarrelModule[nLayer[j]]++; 
	  if(twoSidesHit)
	    nTwoSideBarrelModule[nLayer[j]]++;
	  if(nNonGoodModule[j]){
	    nNonGoodModulesBarrelModule[nLayer[j]]++;
	  }
	}
   
	if(isEndcapA(j)){
	  if(noSidesHit){
	    nNoSidesEndcapAModule[nLayer[j]]+=1;}
	  if(oneSideHit)
	    nOneSideEndcapAModule[nLayer[j]]+=1;
	  if(twoSidesHit)
	    nTwoSideEndcapAModule[nLayer[j]]+=1;
	  if(nNonGoodModule[j]){
	    nNonGoodModulesEndcapAModule[nLayer[j]]++;
	  }
	}
	if(isEndcapC(j)){
	  if(noSidesHit)
	    nNoSidesEndcapCModule[nLayer[j]]+=1;
	  if(oneSideHit)
	    nOneSideEndcapCModule[nLayer[j]]+=1;
	  if(twoSidesHit)
	    nTwoSideEndcapCModule[nLayer[j]]+=1;
	  if(nNonGoodModule[j]){
	    nNonGoodModulesEndcapCModule[nLayer[j]]++;
	  }
	}
       
	if(noSidesHit && !nNonGoodModule[j]){
	  m_NZ1_vs_modnum->Fill(j);
	  nNoFiredModules+=1;   
	  nZero[j]+=1;
	}   // end if(noSidesHit)
	//--------------------------------------
	if(oneSideHit){
	  m_N11_vs_modnum->Fill(j);
	  nOne[j]+=1;
	  nOneSideModule+=1;
	}   // end if(oneSideHit)
	//--------------------------------------
	if(twoSidesHit){
	  nBoth[j]+=1;
	  nTwoSideModule+=1;    
	}   // end if(twoSidesHit)
	//--------------------------------------
      }
       
       
      const int NumModBarrelLayer[N_BARRELS] = {384,480,576,672};
      const int NumModEndcapDisk[N_DISKS]= {92,132,132,132,132,132,92,92,52};

      double NO_ev = calculateNoiseOccupancyUsingRatioMethod(nOneSideModule, nNoFiredModules);
      
      m_NOEV->Fill(1E5*NO_ev);
       
      m_NOEV_RDO->Fill(1E5*NO_ev,count_SCT_RDO);
        
      m_NZ1->Fill(nNoFiredModules);
      m_N11->Fill(nOneSideModule);
      m_N21->Fill(nTwoSideModule);
       
      for(int l=0 ; l<n_layers[BARREL_INDEX]; l++){
	int nosidehit = NumModBarrelLayer[l]-nNonGoodModulesBarrelModule[l]-nOneSideBarrelModule[l]-nTwoSideBarrelModule[l];
	double NO_ev_barrel = calculateNoiseOccupancyUsingRatioMethod(nOneSideBarrelModule[l], nosidehit);
	m_NOEV_Barrel[l]->Fill(1E5*NO_ev_barrel);
	m_NZ1BAR[l]->Fill(nosidehit);
	m_N11BAR[l]->Fill(nOneSideBarrelModule[l]);
	m_N21BAR[l]->Fill(nTwoSideBarrelModule[l]);

	m_NZ1BAR_vsLB[l]->Fill(m_current_lb,nNoSidesBarrelModule[l]);
	m_N11BAR_vsLB[l]->Fill(m_current_lb,nOneSideBarrelModule[l]);
      }
         
      for(int m=0; m<n_layers[ENDCAP_C_INDEX]; m++){
	int nosidehit = NumModEndcapDisk[m]-nNonGoodModulesEndcapCModule[m]-nOneSideEndcapCModule[m]-nTwoSideEndcapCModule[m];
	double NO_ev_endcapc = calculateNoiseOccupancyUsingRatioMethod(nOneSideEndcapCModule[m], nosidehit);
	m_NOEV_EndcapC[m]->Fill(1E5*NO_ev_endcapc);
	m_NZ1ECC[m]->Fill(nosidehit);
	m_N11ECC[m]->Fill(nOneSideEndcapCModule[m]);
	m_N21ECC[m]->Fill(nTwoSideEndcapCModule[m]);

	m_NZ1ECC_vsLB[m]->Fill(m_current_lb,nNoSidesEndcapCModule[m]);
	m_N11ECC_vsLB[m]->Fill(m_current_lb,nOneSideEndcapCModule[m]);
      }       
       
      for(int q=0; q<n_layers[ENDCAP_A_INDEX]; q++){
	int nosidehit = NumModEndcapDisk[q]-nNonGoodModulesEndcapAModule[q]-nOneSideEndcapAModule[q]-nTwoSideEndcapAModule[q];
	double NO_ev_endcapa = calculateNoiseOccupancyUsingRatioMethod(nOneSideEndcapAModule[q], nosidehit);
	m_NOEV_EndcapA[q]->Fill(1E5*NO_ev_endcapa);
	m_NZ1ECA[q]->Fill(nosidehit);
	m_N11ECA[q]->Fill(nOneSideEndcapAModule[q]);
	m_N21ECA[q]->Fill(nTwoSideEndcapAModule[q]);

	m_NZ1ECA_vsLB[q]->Fill(m_current_lb,nNoSidesEndcapAModule[q]);
	m_N11ECA_vsLB[q]->Fill(m_current_lb,nOneSideEndcapAModule[q]);
      }
      
    }
    ++m_numberOfEvents;
  }// trigger    
  

  if(m_environment==AthenaMonManager::online){
    if ( (m_numberOfEvents % m_checkrate) == 0 ) {
      
      //       if (procHistograms(true,true,true).isFailure()){    // hidetoshi 14.01.21
      if (procHistograms().isFailure()) {                          // hidetoshi 14.01.21
	WARNING("Warning problem in procHistograms!");
      }
      //maiken: hm... why is this here... Why reset and fill histograms both for each 1000 events, and for each 20 lb's?
      if ( (m_current_lb % m_checkrecent == 0) && (m_current_lb>m_last_reset_lb) ){
	m_current_lb = pEvent->event_ID()->lumi_block();
	//   if (procHistograms(true,true,true).isFailure()) {  // hidetoshi 14.01.21
	if (procHistograms().isFailure()) {                     // hidetoshi 14.01.21
	  WARNING("Warning problem in procHistograms!");
	}
      }
      
      /*Now reset the counters used for the reset histograms*/
      
      for (int i = 0; i <n_mod[GENERAL_INDEX]  ; i++) {
	nOne[i]=0;
	nZero[i]=0;
      } 
    }
  }
  return StatusCode::SUCCESS;
  
}
  //====================================================================================================
//                             SCTRatioNoiseMonTool :: procHistograms
//====================================================================================================
//StatusCode  SCTRatioNoiseMonTool::procHistograms(bool /*isEndOfEventsBlock*/, bool /*isEndOfLumiBlock*/, bool isEndOfRun){  // hidetoshi 14.01.21
StatusCode  SCTRatioNoiseMonTool::procHistograms(){                                                                           // hidetoshi 14.01.21
  //  if(isEndOfRun){                                                                                                         // hidetoshi 14.01.21
  //  if(endOfRun){                                                                                                           // hidetoshi 14.01.21
    //Reset needed histograms for online
    if(m_environment==AthenaMonManager::online || endOfRun){
      m_NO->Reset();
      m_NOb->Reset();
      m_NOEC->Reset();
      m_NOEC_Outer->Reset();
      m_NOEC_ShortMiddle->Reset();
      m_NOEC_Inner->Reset();
      m_NOEC_Middle->Reset();
      for(unsigned int i=0;i<N_BARRELS;++i) m_NOb_layer[i]->Reset();
      for(unsigned int i=0;i<N_DISKS;++i) m_NOECC_disk[i]->Reset();
      for(unsigned int i=0;i<N_DISKS;++i) m_NOECA_disk[i]->Reset();
      for(unsigned int i=0;i<m_pnoiseoccupancymapHistoVectorBar.size();++i) m_pnoiseoccupancymapHistoVectorBar[i]->Reset();
      for(unsigned int i=0;i<m_pnoiseoccupancymapHistoVectorECA.size();++i) m_pnoiseoccupancymapHistoVectorECA[i]->Reset();
      for(unsigned int i=0;i<m_pnoiseoccupancymapHistoVectorECC.size();++i) m_pnoiseoccupancymapHistoVectorECC[i]->Reset();
    }

    //Construct the plots of interest
    for(int j=0; j<n_mod[GENERAL_INDEX]; j++){
      int layer = nLayer[j];

      if(m_environment==AthenaMonManager::online){
	//calculate and fill rationoise per lumiblock
	if(isBarrel(j)){
	  if(layer<4){
	    for (int bin=1; bin<NBINS_LBs + 1; bin++){
	      int num_zero = m_NZ1BAR_vsLB[layer]->GetBinContent(bin);
	      int num_one = m_N11BAR_vsLB[layer]->GetBinContent(bin);
	      if(num_zero != 0){
		float ratio_evt = 1E5*calculateNoiseOccupancyUsingRatioMethod(num_one,num_zero);
		m_NOb_layer_vsLB[layer]->SetBinContent(bin,ratio_evt);
	      }
	    }
	  }
	}
	if(isEndcap(j)){
	  if(isEndcapA(j)){
	    //calculate event-wise rationoise
	    for (int bin=1; bin<NBINS_LBs + 1; bin++){
	      int num_zero = m_NZ1ECA_vsLB[layer]->GetBinContent(bin);
	      int num_one = m_N11ECA_vsLB[layer]->GetBinContent(bin);
	      if(num_zero!=0){
		float ratio_evt = 1E5*calculateNoiseOccupancyUsingRatioMethod(num_one,num_zero);
		m_NOECA_disk_vsLB[layer]->SetBinContent(bin,ratio_evt);
	      }
	    }
	  }
	  if(isEndcapC(j)){
        
	    //calculate event-wise rationoise
	    for (int bin=1; bin<NBINS_LBs + 1; bin++){
	      int num_zero = m_NZ1ECC_vsLB[layer]->GetBinContent(bin);
	      int num_one = m_N11ECC_vsLB[layer]->GetBinContent(bin);
    
	      if(num_zero!=0){
		float ratio_evt = 1E5*calculateNoiseOccupancyUsingRatioMethod(num_one,num_zero);
		m_NOECC_disk_vsLB[layer]->SetBinContent(bin,ratio_evt);
	      }
	    }
	  }
	}
      }//end online requirement

      if(nZero[j]!=0){
	ratio=0;
	ratio = 1E5*calculateNoiseOccupancyUsingRatioMethod(nOne[j], nZero[j]);
	if(ratio!=0){
	  m_NO->Fill(ratio);
	  if(isBarrel(j)){
	    m_NOb->Fill(ratio);
	    // Produce Noise Occupancy plots for each layer
	    m_pnoiseoccupancymapHistoVectorBar[layer]->Fill(nEta[j],nPhi[j],ratio);
	    if(layer<4){
	      m_NOb_layer[layer]->Fill(ratio);
	    } 
	  } // end if Barrel    
	  if(isEndcap(j)){
	    if(isEndcapA(j)){
	      m_pnoiseoccupancymapHistoVectorECA[layer]->Fill(nEta[j],nPhi[j],ratio);
	      m_NOECA_disk[layer]->Fill(ratio);
	    }
	    if(isEndcapC(j)){
	      m_pnoiseoccupancymapHistoVectorECC[layer]->Fill(nEta[j],nPhi[j],ratio);
	      m_NOECC_disk[layer]->Fill(ratio);
	    }
	    m_NOEC->Fill(ratio);
	    switch (nEta[j]){
	    case 0:
	      m_NOEC_Outer->Fill(ratio);
	      break;
	    case 1:
	      switch (layer){
	      case 7:
		m_NOEC_ShortMiddle->Fill(ratio);
		break;
	      default:
		m_NOEC_Middle->Fill(ratio);
		break;
	      }
	      break;
	    case 2:
	      m_NOEC_Inner->Fill(ratio);
	      break;
	      break;
	    default:
	      break;
	    }
	  } 
	}
      }
    }
    m_NOb->SetBinContent(m_NOb->GetNbinsX(),m_NOb->GetBinContent(m_NOb->GetNbinsX() ) + m_NOb->GetBinContent(m_NOb->GetNbinsX() +1));
    m_NO->SetBinContent(m_NO->GetNbinsX(),m_NO->GetBinContent(m_NO->GetNbinsX() ) + m_NO->GetBinContent(m_NO->GetNbinsX() +1));
    m_NOEC->SetBinContent(m_NOEC->GetNbinsX(),m_NOEC->GetBinContent(m_NOEC->GetNbinsX() ) + m_NOEC->GetBinContent(m_NOEC->GetNbinsX() +1));

    //  }//isEndOfRun



  if (msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << "Total Rec Event Number: " << m_numberOfEvents<<endreq;
  //  msg(MSG::DEBUG) << "Total Rec Event Number: " << m_numberOfEvents<<endreq;
  return StatusCode::SUCCESS;
}

//====================================================================================================
//                           SCTRatioNoiseMonTool :: checkHists
//LF 13/01/2006
//Here we have the check of the histograms and we can also build
//efficiency, occupancy, rms histograms, etc from the basic ones

// LF: A little explanation is needed to understand the reason the following odd C++ coding:
// We registered the RMS histograms in bookHists and we really nead to register them before
// finalize to have  them available online on OH during the run.
// Unfortunaltely THistSvc doesn't (still) allow to unregister or update an histogram and
// ProjectionXY creates a new histogram by a 'new' call. Hence, we cannot change the pointer
//  in THistSvc memory and we cannot pass that pointer in input to ProjectionXY.
// Therefore the following lines pass the information of a tmp hist created by ProjectionXY
//  to the hist which pointer is in THistSvc memory
//====================================================================================================
StatusCode  SCTRatioNoiseMonTool::checkHists(bool /*fromFinalize*/){

  return StatusCode::SUCCESS;
}

//====================================================================================================
//                              SCTRatioNoiseMonTool :: bookRatioNoiseHistos
//====================================================================================================
//  StatusCode SCTRatioNoiseMonTool::bookRatioNoiseHistos(bool isNewRun){       // hidetoshi 14.01.22
StatusCode SCTRatioNoiseMonTool::bookRatioNoiseHistos(){       // hidetoshi 14.01.22
  using boost::lexical_cast;
  //  if(isNewRun){                                          // hidetoshi 14.01.22
  if(newRun){                                          // hidetoshi 14.01.22
    
    //     string stem=m_path+"/SCT/NO_Ratio/RatioNoise/";
    //     MonGroup RatioNoise(this,m_path+"SCT/NO_Ratio/RatioNoise",expert,run);
    // Priscilla: trying to make the dir structure similar to the rest of the mon Package
    
    
    string stem=m_path+"/SCT/GENERAL/RatioNoise/";
    //      MonGroup RatioNoise(this,m_path+"SCT/GENERAL/RatioNoise",expert,run);   // hidetoshi 14.01.22
    MonGroup RatioNoise(this,m_path+"SCT/GENERAL/RatioNoise",run,ATTRIB_UNMANAGED); // hidetoshi 14.01.22
    
    
    string hNumBarrel[N_BARRELS]={"0","1","2","3"};
    string hNumEndcap[N_DISKS]={"0","1","2","3","4","5","6","7","8"};
    
    //Book the General Histograms
    m_side=h1Factory("h_side", "Side of Module",RatioNoise, 0, 6, 6);
    m_side->GetXaxis()->SetBinLabel(2,"Outer");
    m_side->GetXaxis()->SetBinLabel(4,"Inner");
    m_num_RDO = h1Factory("h_num_RDO", "Number of RDO's", RatioNoise, 0, 6000,1000);
    m_num_RDO->GetXaxis()->SetTitle("Num of RDO's");
    m_num_RDO->GetYaxis()->SetTitle("Events");
    m_NOEV = h1Factory("h_NOEV", "Event Noise Occupancy", RatioNoise, 0, 5,500);
    m_NOEV->GetXaxis()->SetTitle("Event Noise Occupancy [10^{-5}]");
    m_NOEV->GetYaxis()->SetTitle("Events");
    m_NOEV_RDO = h2Factory("h_NOEV_RDO", "Event Noise Occupancy vs RDO", RatioNoise, 0, 5, 1000, 0, 500, 500);
    m_NOEV_RDO->GetXaxis()->SetTitle("Event Noise Occupancy [10^{-5}]");
    m_NOEV_RDO->GetYaxis()->SetTitle("Num of RDO's");
    m_NZ1 = h1Factory("h_NZ1", "Num of ZeroSide Hits in All Region", RatioNoise, 0, 6000, 1000);
    m_NZ1 ->SetTitle("Num of ZeroSide Hits in All Region; Num of ZeroSide Hits; Num of Entries");
    m_N11 = h1Factory("h_N11", "NoOneSide-1d", RatioNoise, 0, 6000, 1000);
    m_N11 ->SetTitle("Num of OneSide Hits in All Region; Num of OneSide Hits; Num of Entries");
    m_N21 = h1Factory("h_N21", "NoTwoSide-1d", RatioNoise, 0, 6000, 1000);
    m_N21 ->SetTitle("Num of TwoSide Hits in All Region; Num of TwoSide Hits; Num of Entries");
    m_NO = h1Factory("h_NO", "Noise Occupancy All",RatioNoise, 0, 100, 500);
    m_NO->SetTitle("Noise Occupancy for Barrel and Endcaps; Noise Occupancy [10^{-5}]; Num of Modules");
    m_NOEC = h1Factory("h_NOEC", "Noise Occupancy Endcap",RatioNoise, 0, 100, 500);
    m_NOEC->SetTitle("Noise Occupancy for Endcaps; Noise Occupancy [10^{-5}]; Num of Modules");
    m_NOb = h1Factory("h_NOb", "Noise Occupancy Barrel",RatioNoise, 0, 100, 500);
    m_NOb->SetTitle("Noise Occupancy for Barrel; Noise Occupancy [10^{-5}]; Num of Modules");
    m_NZ1_vs_modnum = h1Factory("h_NZ1_vs_modnum", "Num of ZeroSide hits vs module number", RatioNoise, 0,4088, 4088);
    m_NZ1_vs_modnum->SetTitle("ModuleNumber vs Num of ZeroSide Hits; Module Number; Num of ZeroSide Hits");
    m_N11_vs_modnum = h1Factory("h_N11_vs_modnum", "Num of OneSide hits vs module number", RatioNoise, 0,4088, 4088);
    m_N11_vs_modnum->SetTitle("ModuleNumber vs Num of OneSide Hits; Module Number; Num of OneSide Hits");      
    
    for(int l=0;l< n_layers[BARREL_INDEX];l++){
      //granularity set to one histogram/layer for now
      m_NOEV_Barrel[l] = h1Factory("h_NOEV_Barrel"+hNumBarrel[l], "Event Noise Occupancy Barrel Layer"+hNumBarrel[l], RatioNoise, 0, 5,500);
      m_NOEV_Barrel[l]->GetXaxis()->SetTitle("Event Noise Occupancy [10^{-5}]");
      m_NOEV_Barrel[l]->GetYaxis()->SetTitle("Events");
      m_numberHitsinBarrel[l] = h1Factory("h_numberHitsinBarrel"+hNumBarrel[l],"Number Hits in Barrel Layer"+hNumBarrel[l],RatioNoise, 0.,50.,50.);
      m_numberHitsinBarrel[l]->GetXaxis()->SetTitle("Index in the direction of #phi");
      m_numberHitsinBarrel[l]->GetYaxis()->SetTitle("Hits");
      m_NZ1BAR[l]=h1Factory("h_NZ1BAR"+hNumBarrel[l], "Num of ZeroSide Hits in Barrel Layer"+hNumBarrel[l], RatioNoise, 0, 6000, 1000);
      m_NZ1BAR[l]->GetXaxis()->SetTitle("Num of ZeroSide Hits");
      m_NZ1BAR[l]->GetYaxis()->SetTitle("Num of Entries");
      m_N11BAR[l]=h1Factory("h_N11BAR"+hNumBarrel[l], "Num of OneSide Hits in Barrel Layer"+hNumBarrel[l], RatioNoise, 0, 6000, 1000);
      m_N11BAR[l]->GetXaxis()->SetTitle("Num of OneSide Hits");
      m_N11BAR[l]->GetYaxis()->SetTitle("Num of Entries");
      m_N21BAR[l]=h1Factory("h_N21BAR"+hNumBarrel[l], "Num of TwoSide Hits in Barrel Layer"+hNumBarrel[l], RatioNoise, 0, 6000, 1000);
      m_N21BAR[l]->GetXaxis()->SetTitle("Num of TwoSide Hits");
      m_N21BAR[l]->GetYaxis()->SetTitle("Num of Entries");
      m_NOb_layer[l]=h1Factory("h_NOb_layer"+hNumBarrel[l], "Noise Occupancy Barrel Layer"+hNumBarrel[l],RatioNoise, 0, 100, 500);
      m_NOb_layer[l]->GetXaxis()->SetTitle("Noise Occupancy [10^{-5}]");
      m_NOb_layer[l]->GetYaxis()->SetTitle("Num of Modules");
      m_NOb_layer_vsLB[l]=h1Factory("h_NOb_layer"+hNumBarrel[l]+"_vsLB", "Noise Occupancy Barrel Layer"+hNumBarrel[l]+" vs LB",RatioNoise, 0, NBINS_LBs, NBINS_LBs);
      m_NOb_layer_vsLB[l]->GetXaxis()->SetTitle("LumiBlock");
      m_NOb_layer_vsLB[l]->GetYaxis()->SetTitle("Noise Occupancy [10^{-5}]");
      m_NZ1BAR_vsLB[l]=h1Factory("h_NZ1BAR"+hNumBarrel[l]+"_vsLB", "Num of ZeroSide Hits in Barrel Layer"+hNumBarrel[l]+" vs LB", RatioNoise, 0, NBINS_LBs, NBINS_LBs);
      m_NZ1BAR_vsLB[l]->GetXaxis()->SetTitle("LumiBlock");
      m_NZ1BAR_vsLB[l]->GetYaxis()->SetTitle("Num of ZeroSide Hits");
      m_N11BAR_vsLB[l]=h1Factory("h_N11BAR"+hNumBarrel[l]+"_vsLB", "Num of OneSide Hits in Barrel Layer"+hNumBarrel[l]+" vs LB", RatioNoise, 0, NBINS_LBs, NBINS_LBs);
      m_N11BAR_vsLB[l]->GetXaxis()->SetTitle("LumiBlock");
      m_N11BAR_vsLB[l]->GetYaxis()->SetTitle("Num of OneSide Hits");	
    } 
    
    for(int m=0; m<n_layers[ENDCAP_C_INDEX];m++){
      m_NOEV_EndcapC[m] = h1Factory("h_NOEV_EndcapC"+hNumEndcap[m], "Event Noise Occupancy EndcapC Disk"+hNumEndcap[m], RatioNoise, 0, 5,500);
      m_NOEV_EndcapC[m]->GetXaxis()->SetTitle("Event Noise Occupancy [10^{-5}]");
      m_NOEV_EndcapC[m]->GetYaxis()->SetTitle("Events");
      m_NZ1ECC[m]=h1Factory("h_NZ1ECC"+hNumEndcap[m], "Num of ZeroSide Hits in EndcapC Disk"+hNumEndcap[m], RatioNoise, 0, 6000, 1000);
      m_NZ1ECC[m]->GetXaxis()->SetTitle("Num of ZeroSide Hits");
      m_NZ1ECC[m]->GetYaxis()->SetTitle("Num of Entries");
      m_N11ECC[m]=h1Factory("h_N11ECC"+hNumEndcap[m], "Num of OneSide Hits in EndcapC Disk"+hNumEndcap[m], RatioNoise, 0, 6000, 1000);
      m_N11ECC[m]->GetXaxis()->SetTitle("Num of OneSide Hits");
      m_N11ECC[m]->GetYaxis()->SetTitle("Num of Entries");
      m_N21ECC[m]=h1Factory("h_N21ECC"+hNumEndcap[m], "Num of TwoSide Hits in EndcapC Disk"+hNumEndcap[m], RatioNoise, 0, 6000, 1000);
      m_N21ECC[m]->GetXaxis()->SetTitle("Num of TwoSide Hits");
      m_N21ECC[m]->GetYaxis()->SetTitle("Num of Entries");
      m_NOECC_disk[m]=h1Factory("h_NOECC_disk"+hNumEndcap[m], "Noise Occupancy EndcapC Disk"+hNumEndcap[m],RatioNoise, 0, 100, 500);
      m_NOECC_disk[m]->GetXaxis()->SetTitle("Noise Occupancy [10^{-5}]");	
      m_NOECC_disk[m]->GetYaxis()->SetTitle("Num of Modules");	
      
      m_NOECC_disk_vsLB[m] = h1Factory("h_NOECC_disk"+hNumEndcap[m]+"_vsLB", "Noise Occupancy EndcapC Disk"+hNumEndcap[m]+" vs LB",RatioNoise, 0, NBINS_LBs, NBINS_LBs);
      m_NOECC_disk_vsLB[m]->GetXaxis()->SetTitle("LumiBlock");
      m_NOECC_disk_vsLB[m]->GetYaxis()->SetTitle("Noise Occupancy [10^{-5}]");	
      m_NZ1ECC_vsLB[m]=h1Factory("h_NZ1ECC"+hNumEndcap[m]+"_vsLB", "Num of ZeroSide Hits in EndcapC Disk"+hNumEndcap[m]+" vs LB", RatioNoise, 0, NBINS_LBs, NBINS_LBs);
      m_NZ1ECC_vsLB[m]->GetXaxis()->SetTitle("LumiBlock");	
      m_NZ1ECC_vsLB[m]->GetYaxis()->SetTitle("Num of ZeroSide Hits");	
      m_N11ECC_vsLB[m] = h1Factory("h_N11ECC"+hNumEndcap[m]+"_vsLB", "Num of OneSide Hits in EndcapC Disk"+hNumEndcap[m]+" vs LB", RatioNoise, 0, NBINS_LBs, NBINS_LBs);
      m_N11ECC_vsLB[m]->GetXaxis()->SetTitle("LumiBlock");	
      m_N11ECC_vsLB[m]->GetYaxis()->SetTitle("Num of OneSide Hits");	
    }
    
    for(int p=0; p<n_layers[ENDCAP_A_INDEX];p++){
      m_NOEV_EndcapA[p] = h1Factory("h_NOEV_EndcapA"+hNumEndcap[p], "Event Noise Occupancy EndcapA Disk"+hNumEndcap[p], RatioNoise, 0, 5,500);
      m_NOEV_EndcapA[p]->GetXaxis()->SetTitle("Event Noise Occupancy [10^{-5}]");
      m_NOEV_EndcapA[p]->GetYaxis()->SetTitle("Events");
      m_NZ1ECA[p]=h1Factory("h_NZ1ECA"+hNumEndcap[p], "Num of ZeroSide Hits in EndcapA Disk"+hNumEndcap[p], RatioNoise, 0, 6000, 1000);
      m_NZ1ECA[p]->GetXaxis()->SetTitle("Num of ZeroSide Hits");
      m_NZ1ECA[p]->GetYaxis()->SetTitle("Num of Entries");
      m_N11ECA[p]=h1Factory("h_N11ECA"+hNumEndcap[p], "Num of OneSide Hits in EndcapA Disk"+hNumEndcap[p], RatioNoise, 0, 6000, 1000);
      m_N11ECA[p]->GetXaxis()->SetTitle("Num of OneSide Hits");
      m_N11ECA[p]->GetYaxis()->SetTitle("Num of Entries");
      m_N21ECA[p]=h1Factory("h_N21ECA"+hNumEndcap[p], "Num of TwoSide Hits in EndcapA Disk"+hNumEndcap[p], RatioNoise, 0, 6000, 1000);
      m_N21ECA[p]->GetXaxis()->SetTitle("Num of TwoSide Hits");
      m_N21ECA[p]->GetYaxis()->SetTitle("Num of Entries");
      m_NOECA_disk[p]=h1Factory("h_NOECA_disk"+hNumEndcap[p], "Noise Occupancy EndcapA Disk"+hNumEndcap[p],RatioNoise, 0, 100, 500);
      m_NOECA_disk[p]->GetXaxis()->SetTitle("Noise Occupancy [10^{-5}]");	
      m_NOECA_disk[p]->GetYaxis()->SetTitle("Num of Modules");	
      
      m_NOECA_disk_vsLB[p]=h1Factory("h_NOECA_disk"+hNumEndcap[p]+"_vsLB", "Noise Occupancy Barrel Disk"+hNumEndcap[p]+" vs LB",RatioNoise, 0, NBINS_LBs, NBINS_LBs);
      m_NOECA_disk_vsLB[p]->GetXaxis()->SetTitle("LumiBlock");		
      m_NOECA_disk_vsLB[p]->GetYaxis()->SetTitle("Noise Occupancy [10^{-5}]");
      m_NZ1ECA_vsLB[p]=h1Factory("h_NZ1ECA"+hNumEndcap[p]+"_vsLB", "Num of ZeroSide Hits in EndcapA Disk"+hNumEndcap[p]+" vs LB", RatioNoise, 0, NBINS_LBs, NBINS_LBs);
      m_NZ1ECA_vsLB[p]->GetXaxis()->SetTitle("LumiBlock");	
      m_NZ1ECA_vsLB[p]->GetYaxis()->SetTitle("Num of ZeroSide Hits");
      m_N11ECA_vsLB[p]=h1Factory("h_N11ECA"+hNumEndcap[p]+"_vsLB", "Num of OneSide Hits in EndcapA Disk"+hNumEndcap[p]+" vs LB", RatioNoise, 0, NBINS_LBs, NBINS_LBs);
      m_N11ECA_vsLB[p]->GetXaxis()->SetTitle("LumiBlock");	
      m_N11ECA_vsLB[p]->GetYaxis()->SetTitle("Num of OneSide Hits");
    }
    
    // Module Level Noise Occupancy
    
    // For each type of endcap module

    m_NOEC_Inner=h1Factory("h_NOEC_Inner", "Noise Occupancy Endcap Inner Modules", RatioNoise, 0, 100, 500);
    m_NOEC_Inner->SetTitle("Noise Occupancy for Endcap Inner; Noise Occupancy [10^{-5}] ; Num of Modules");	
    m_NOEC_Middle=h1Factory("h_NOEC_Middle", "Noise Occupancy Endcap Middle Modules", RatioNoise, 0, 100, 500);
    m_NOEC_Middle->SetTitle("Noise Occupancy for Endcap Middle; Noise Occupancy [10^{-5}] ; Num of Modules");	
    m_NOEC_ShortMiddle=h1Factory("h_NOEC_ShortMiddle", "Noise Occupancy Endcap Short Middle Modules",RatioNoise, 0, 100, 500);
    m_NOEC_ShortMiddle->SetTitle("Noise Occupancy for Endcap Short Middle; Noise Occupancy [10^{-5}] ; Num of Modules");	
    m_NOEC_Outer=h1Factory("h_NOEC_Outer", "Noise Occupancy Endcap Outer Modules",RatioNoise, 0, 100, 500);
    m_NOEC_Outer->SetTitle("Noise Occupancy for Endcap Outer; Noise Occupancy [10^{-5}] ; Num of Modules");	

    
    const string paths[]={"SCT/SCTEC/RatioNoise", "SCT/SCTB/RatioNoise", "SCT/SCTEA/RatioNoise"};
    const unsigned int limits[]={N_DISKS, N_BARRELS, N_DISKS};
    VecProf2_t * storageVectors[]={&m_pnoiseoccupancymapHistoVectorECC, &m_pnoiseoccupancymapHistoVectorBar, &m_pnoiseoccupancymapHistoVectorECA};
    const int bec[]={-2,0,2};
    
    
    for(int systemIndex=0; systemIndex<3; systemIndex++){
      (storageVectors[systemIndex])->clear();
      for (unsigned int i=0; i!=limits[systemIndex];++i) {
	//    MonGroup noiseOccMaps(this,m_path+paths[systemIndex],expert,run);      // hidetoshi 14.01.21
	MonGroup noiseOccMaps(this,m_path+paths[systemIndex],run,ATTRIB_UNMANAGED);  // hidetoshi 14.01.21
	const string streamhitmap ="noiseoccupancymap"+abbreviations[systemIndex]+"_"+hNumEndcap[i];
	std::string histotitle="SCT Noise Occupancy map for "+names[systemIndex]+": "+hNumEndcap[i];
	prof2Factory(streamhitmap,histotitle, bec[systemIndex], noiseOccMaps,*(storageVectors[systemIndex]));
      }   
    }      
  }  // hidetoshi 14.01.22
  return StatusCode::SUCCESS;
}



TProfile*
SCTRatioNoiseMonTool::pFactory(const std::string & name, const std::string & title, int nbinsx, double xlow, double xhigh, MonGroup & registry, int& iflag){

  Prof_t tmp = new TProfile(TString(name), TString(title),nbinsx,xlow,xhigh);
  bool success( registry.regHist(tmp).isSuccess());
  if (not success){
    if (msgLvl(MSG::WARNING)) msg(MSG::WARNING) << "Cannot book SCT histogram: " << name << endreq;
    iflag=0;
  }else{
    iflag=1;
  }

  return tmp;
}


SCTRatioNoiseMonTool::H1_t
SCTRatioNoiseMonTool::h1Factory(const std::string & name, const std::string & title, MonGroup & registry, const float lo, const float hi, const unsigned int nbins){
  H1_t tmp = TH1F_LW::create(TString(name),TString(title),nbins,lo,hi);
  bool success(registry.regHist(tmp).isSuccess());
  if (not success) msg(MSG::WARNING) << "Cannot book SCT histogram: " << name << endreq;
  return success?tmp:NULL;      
}

SCTRatioNoiseMonTool::H2_t
SCTRatioNoiseMonTool::h2Factory(const std::string & name, const std::string & title, MonGroup & registry, const float lo_x, const float hi_x, const unsigned int nbins_x, const float lo_y, const float hi_y, const unsigned int nbins_y){
  H2_t tmp = TH2F_LW::create(TString(name),TString(title),nbins_x,lo_x,hi_x, nbins_y, lo_y, hi_y);
  bool success(registry.regHist(tmp).isSuccess());
  if (not success) msg(MSG::WARNING) << "Cannot book SCT histogram: " << name << endreq;
  return success?tmp:NULL;      
}

SCTRatioNoiseMonTool::H1_t
SCTRatioNoiseMonTool::h1Factory(const std::string & name, const std::string & title, MonGroup & registry, VecH1_t & storageVector, const float lo, const float hi, const unsigned int nbins){
  H1_t tmp = TH1F_LW::create(TString(name),TString(title),nbins,lo,hi);
  bool success(registry.regHist(tmp).isSuccess());
  if (not success) msg(MSG::WARNING) << "Cannot book SCT histogram: " << name << endreq;
  storageVector.push_back(tmp);
  return success?tmp:NULL;      
}

SCTRatioNoiseMonTool::Prof2_t
SCTRatioNoiseMonTool::prof2Factory(const std::string & name, const std::string & title, const unsigned int& bec, MonGroup & registry, VecProf2_t & storageVector){
  int firstEta(FIRST_ETA_BIN), lastEta(LAST_ETA_BIN), firstPhi(FIRST_PHI_BIN), lastPhi(LAST_PHI_BIN), nEta(N_ETA_BINS), nPhi(N_PHI_BINS);
  if (bec != BARREL){
    firstEta=FIRST_ETA_BIN_EC;
    lastEta=LAST_ETA_BIN_EC;
    firstPhi=FIRST_PHI_BIN_EC;
    lastPhi=LAST_PHI_BIN_EC;
    nEta=N_ETA_BINS_EC;
    nPhi=N_PHI_BINS_EC;
  }
  Prof2_t tmp = new TProfile2D(TString(name), TString(title), nEta, firstEta-0.5, lastEta+0.5, nPhi, firstPhi-0.5, lastPhi+0.5);
  tmp->SetXTitle("Index in the direction of #eta");
  tmp->SetYTitle("Index in the direction of #phi");
  bool success( registry.regHist(tmp).isSuccess());
  if (not success)  msg(MSG::WARNING) << "Cannot book SCT histogram: " << name << endreq;
  storageVector.push_back(tmp);
  return success?tmp:NULL;
}


float SCTRatioNoiseMonTool::calculateNoiseOccupancyUsingRatioMethod(const float numberOneSide, const float numberZeroSide){

  float div, rat;

  div = numberOneSide/numberZeroSide;
  rat = (div/(div+2))/768;
  
  return rat;
}




bool SCTRatioNoiseMonTool::isEndcapC(const int moduleNumber){
  bool moduleinEndcapC = false;
  if(0<=moduleNumber    && moduleNumber<f_mod[BARREL_INDEX]){
    moduleinEndcapC = true;}
  return moduleinEndcapC;
}


bool SCTRatioNoiseMonTool::isBarrel(const int moduleNumber){
  bool moduleinBarrel = false;
  //if(moduleNumber>=f_mod[BARREL_INDEX] && moduleNumber<f_mod[ENDCAP_A_INDEX])
  if(f_mod[BARREL_INDEX]<=moduleNumber && moduleNumber<f_mod[ENDCAP_A_INDEX]){
    moduleinBarrel = true;}
  return moduleinBarrel;
}

bool SCTRatioNoiseMonTool::isEndcapA(const int moduleNumber){
  bool moduleinEndcapA = false;
  //  if(moduleNumber>=f_mod[ENDCAP_A_INDEX] && moduleNumber<n_mod[GENERAL_INDEX]){
  if(f_mod[ENDCAP_A_INDEX]<=moduleNumber && moduleNumber<n_mod[GENERAL_INDEX]){
    moduleinEndcapA = true;}
  return moduleinEndcapA;
}


bool SCTRatioNoiseMonTool::isEndcap(const int moduleNumber){
  bool moduleinEndcap = false;
  //  if(moduleNumber>=0 && moduleNumber<f_mod[BARREL_INDEX])
  if(0<=moduleNumber && moduleNumber<f_mod[BARREL_INDEX])
    moduleinEndcap = true;
  //  if(moduleNumber>=f_mod[ENDCAP_A_INDEX] && moduleNumber<n_mod[GENERAL_INDEX])
  if(f_mod[ENDCAP_A_INDEX]<=moduleNumber && moduleNumber<n_mod[GENERAL_INDEX])
    moduleinEndcap = true;
  return moduleinEndcap;
}
