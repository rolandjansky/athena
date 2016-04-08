/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**
 * @brief  Algorithm to correlate L1 items across neighbouring bunch crossings
 * @author Nils Ruthmann, Till Eifert
 *
 * $Id: $
 */

#include "TrigGenericAlgs/L1CorrelationAlgo.h"

#include "GaudiKernel/IJobOptionsSvc.h"
#include "GaudiKernel/Property.h"
#include "ByteStreamCnvSvcBase/IROBDataProviderSvc.h"

#include <math.h>
#include <sstream>
#include <algorithm>

#include <boost/foreach.hpp>
#include <boost/algorithm/string.hpp>
#include <boost/tokenizer.hpp>

#include "CTPfragment/CTPfragment.h"
#include "CTPfragment/CTPdataformat.h"

#include "TrigT1Result/RoIBResult.h"
//#include "eformat/eformat.h"
//#include "eformat/write/eformat.h"

#include "TrigT1Interfaces/RecMuonRoI.h"
#include "TrigConfL1Data/CTPConfig.h"
#include "TrigConfL1Data/Menu.h"
#include "TrigConfL1Data/TriggerItem.h"
using namespace std;

const short N_ROBID_BITS(24);  // 3 bytes for ROB ID

unsigned int getBitMaskValue( const unsigned int uintValue,
                            const unsigned int mask ) {
  unsigned int result;
  unsigned int maskcopy;
  maskcopy = mask;
  result = uintValue & mask;
  if ( mask != 0 ) {
    while ( ( maskcopy & 0x00000001 ) == 0 ) {
      maskcopy = maskcopy >> 1;
      result = result >> 1;
    }
  }
  return result;
}

L1CorrelationAlgo::L1CorrelationAlgo(const std::string& name, ISvcLocator* pSvcLocator)
  : HLT::AllTEAlgo(name, pSvcLocator),
    m_robDataProviderSvc("ROBDataProviderSvc", name),
    m_trigMuonRoITool( "TrigMuonRoITool" ),
    m_recRPCRoiSvc("LVL1RPC::RPCRecRoiSvc",""),
    m_recTGCRoiSvc("LVL1TGC::TGCRecRoiSvc",""),
    m_configSvc("TrigConf::TrigConfigSvc/TrigConfigSvc",name)
{

  declareProperty("noMuon",   m_nomuon = false, "no muon in the list of items to be correlated");
  declareProperty("m_l1itemlist",   m_l1itemlist = {"L1_EM22VHI","L1_J400","L1_MU20"}, "list of l1 items to be used");
  
  declareMonitoredVariable("EF_L1Corr_beforeafterflag", m_beforeafterflag, 0);
  declareMonitoredVariable("EF_L1Corr_l1a_type", m_l1a_type, 0);
  declareMonitoredVariable("EF_L1Corr_other_type", m_other_type, 0); 

  declareMonitoredStdContainer("EF_etaOutOfTimeMuon", m_etaOutOfTimeMuon, AutoClear);
  declareMonitoredStdContainer("EF_phiOutOfTimeMuon", m_phiOutOfTimeMuon, AutoClear);
  declareMonitoredStdContainer("EF_ptThrOutOfTimeMuon", m_ptThrOutOfTimeMuon, AutoClear);

}


HLT::ErrorCode L1CorrelationAlgo::hltInitialize()
{
  ServiceHandle<IJobOptionsSvc> jobOptSvc("JobOptionsSvc", name());
  if (m_configSvc.retrieve().isFailure()) {
    ATH_MSG_ERROR("Cannot retrieve " << m_configSvc);
    return HLT::BAD_JOB_SETUP;
  }
  
  
  if (m_robDataProviderSvc.retrieve().isFailure()) {
    ATH_MSG_ERROR("Cannot retrieve " << m_robDataProviderSvc);
    return HLT::BAD_JOB_SETUP;
  }
  
  // Retrieve the MuonRoITool
  StatusCode sc = m_trigMuonRoITool.retrieve();
  if ( sc.isFailure() ) {
    ATH_MSG_ERROR( "Could not retrieve " << m_trigMuonRoITool );
    return HLT::ERROR;
  } else {
    ATH_MSG_INFO("Retrieved tool " << m_trigMuonRoITool);
  }

  sc = m_recRPCRoiSvc.retrieve();
  if ( sc.isFailure() ) {
    ATH_MSG_ERROR( "Couldn't connect to " << m_recRPCRoiSvc );
    return HLT::ERROR;
  } else {
   ATH_MSG_INFO( "Retrieved Service " << m_recRPCRoiSvc );
  }
  
  sc = m_recTGCRoiSvc.retrieve();
  if ( sc.isFailure() ) {
    ATH_MSG_ERROR( "Couldn't connect to " << m_recTGCRoiSvc );
    return HLT::ERROR;
  } else {
    ATH_MSG_INFO( "Retrieved Service " << m_recTGCRoiSvc);
  }
  

  //  ATH_MSG_INFO("Hi Nils this is in the L1Correlation algo was able to retrieve this svc thingy");


  // build the bitmask filtering the l1 items we want. for smk 2224 maybe start with these ones:

  /*
    ctpid="4" name="L1_EM10VH"
    ctpid="5" name="L1_EM13VH"
    ctpid="6" name="L1_EM15"
    ctpid="7" name="L1_EM15VH"
    ctpid="8" name="L1_EM18VH"
    ctpid="9" name="L1_EM20VH"
    ctpid="10" name="L1_EM20VHI"
    ctpid="11" name="L1_EM22VHI"
    
    ctpid="16" name="L1_MU10"
    ctpid="17" name="L1_MU15"
    ctpid="18" name="L1_MU20"

    ctpid="96" name="L1_J40"
    ctpid="97" name="L1_J50"
    ctpid="98" name="L1_J75"
    ctpid="99" name="L1_J85"
    ctpid="100" name="L1_J100"
    ctpid="101" name="L1_J120"
    ctpid="102" name="L1_J400"

    should be: 031 to 000 | (fill 0) 1110000111111110000
               127 to 096 |       (fill 0)       1111111

  */


   std::vector<int> ctpids_ele;
   std::vector<int> ctpids_mu;
   std::vector<int> ctpids_jets;
   
   std::vector<int> ctpids;

   const TrigConf::CTPConfig *ctp_confg = m_configSvc->ctpConfig();
   for(TrigConf::TriggerItem *item : ctp_confg->menu().items()) {
     //     std::cout<<"Hi Nils, this item here is in the L1 menu:"<<     item->name().c_str()     << " ID:"<<item-> ctpId() <<std::endl;
     for(unsigned int nl1=0; nl1< m_l1itemlist.size();nl1++ ){

       if(m_l1itemlist[nl1].compare(item->name()) == 0 ){
	 ATH_MSG(INFO) << "L1CorrAlgInit: Configured to use item:" <<item->name().c_str()<< " CTPID:"<<item-> ctpId() << endmsg;

	 ctpids.push_back( item-> ctpId() );
	 if( m_l1itemlist[nl1].find( "L1_EM" )  != std::string::npos ){
	   ctpids_ele.push_back( item-> ctpId() );
	   //	   std::cout<<"Hi Nils, pushing it back as EM item" <<std::endl;
	 }
	 if( m_l1itemlist[nl1].find( "L1_MU" )  != std::string::npos ){
	   ctpids_mu.push_back( item-> ctpId() );
	   //	   std::cout<<"Hi Nils, pushing it back as MU item" <<std::endl;
	 }
	 if( m_l1itemlist[nl1].find( "L1_J" )  != std::string::npos ){
	   ctpids_jets.push_back( item-> ctpId() );
	   //   	   std::cout<<"Hi Nils, pushing it back as J item" <<std::endl;
	 }
       }       
     }     
   }

   

   // done with the CTP ID retrieval 
   
   // the following is a hardcoded version for one specific L1 menu:
   
   // //ctpids.push_back(9);  ctpids.push_back(10);
   // ctpids.push_back(11); //ele
   // //ctpids_ele.push_back(9);  ctpids_ele.push_back(10);
   // ctpids_ele.push_back(11); //ele

   // if (!m_nomuon){
   //   ctpids.push_back(18); // l1mu20
   //   ctpids_mu.push_back(18); // l1mu20
   // }
   
   // //   ctpids.push_back(99);  ctpids.push_back(100);  //jets
   // ctpids.push_back(101);  ctpids.push_back(102); //jets
   // //ctpids_jets.push_back(99);  ctpids_jets.push_back(100);  //jets
   // ctpids_jets.push_back(101);  ctpids_jets.push_back(102); //jets
   

  // init with 0
   m_bitmasks.clear();
   m_bitmasks_ele.clear();
   m_bitmasks_mu.clear();
   m_bitmasks_jets.clear();

   for(int n=0; n<16 ; n++){
     uint32_t tmpword=0;
     m_bitmasks.push_back(tmpword);
     m_bitmasks_ele.push_back(tmpword);
     m_bitmasks_mu.push_back(tmpword);
     m_bitmasks_jets.push_back(tmpword);

   }
   
   // eventually the length of the words can change? discover from ctp format version instead ?
   for(unsigned int n=0; n<ctpids.size()  ; n++){
     // check in which word it belongs
     int cycle =  ctpids[n] / 32;
     int pos = ctpids[n] % 32;
     
     uint32_t currentmask = m_bitmasks[cycle];
     uint32_t tmpmask = 1;
     // shift this pos positions to left
     tmpmask = tmpmask << (pos);
     
     // OR this with the existing mask
     m_bitmasks[cycle] = tmpmask | currentmask;
   }

   for(unsigned int n=0; n<ctpids_ele.size()  ; n++){
     // check in which word it belongs
     int cycle =  ctpids_ele[n] / 32;
     int pos = ctpids_ele[n] % 32;
     
     uint32_t currentmask = m_bitmasks_ele[cycle];
     uint32_t tmpmask = 1;
     // shift this pos positions to left
     tmpmask = tmpmask << (pos);
     
     // OR this with the existing mask
     m_bitmasks_ele[cycle] = tmpmask | currentmask;
   }
   for(unsigned int n=0; n<ctpids_mu.size()  ; n++){
     // check in which word it belongs
     int cycle =  ctpids_mu[n] / 32;
     int pos = ctpids_mu[n] % 32;
     
     uint32_t currentmask = m_bitmasks_mu[cycle];
     uint32_t tmpmask = 1;
     // shift this pos positions to left
     tmpmask = tmpmask << (pos);
     
     // OR this with the existing mask
     m_bitmasks_mu[cycle] = tmpmask | currentmask;
   }  
   for(unsigned int n=0; n<ctpids_jets.size()  ; n++){
     // check in which word it belongs
     int cycle =  ctpids_jets[n] / 32;
     int pos = ctpids_jets[n] % 32;
     
     uint32_t currentmask = m_bitmasks_jets[cycle];
     uint32_t tmpmask = 1;
     // shift this pos positions to left
     tmpmask = tmpmask << (pos);
     
     // OR this with the existing mask
     m_bitmasks_jets[cycle] = tmpmask | currentmask;
   }  
   
   int nBITs = 16 * 32 - 1;
   /*Print out the bitmasks into the logfile:*/
   printf(" L1CorrelationAlgo Init: bitmasks used for BC correlation \n");
   std::vector<uint32_t>::reverse_iterator bm_it = m_bitmasks.rbegin();
   
   for(bm_it = m_bitmasks.rbegin(); bm_it != m_bitmasks.rend(); ++bm_it) {
     printf("D  |_      %03d to %03d | ", nBITs, nBITs-31);
     for (int32_t bit = 31; bit>=0;--bit,--nBITs) {
       ( ((*bm_it)>>bit) & 1 ) ? printf("1") : printf("0");
       if(bit==0) printf("\n");
     }
   }
   
   //  m_robRequest.declareUpdateHandler(&L1CorrelationAlgo::updateHandler, this);
   //  parseROBRequest();
   return HLT::OK;
}


void L1CorrelationAlgo::updateHandler(Property&)
{
  //  parseROBRequest();
}



/*
Generel idea is correlate l1 items across BCs. The items do not necessarily need to be the same. 
m_bitmasks encode the items to be considered
if any of the items in these masks fired in two successive bc write out the event

loop over bc
 - AND every word with the corresponding bitmask
 - if not 0 save this

if two successive non-0 say OK 
*/
HLT::ErrorCode L1CorrelationAlgo::hltExecute(std::vector<HLT::TEVec>& fake_seed, unsigned int output)
{


  bool debugoutput=false;
  
  std::vector<const OFFLINE_FRAGMENTS_NAMESPACE::ROBFragment*> robFragments;
  ROBList roblist;

  // magic number!
  roblist.push_back(0x770000); 
  m_robDataProviderSvc->addROBData(roblist);
  m_robDataProviderSvc->getROBData(roblist, robFragments);
  if (msgLvl(MSG::DEBUG)) {
    ostringstream os;
    os << hex;
    BOOST_FOREACH(uint32_t rob, roblist) os << " 0x" << rob;
    ATH_MSG_DEBUG(roblist.size() << "/" << robFragments.size()
		 << " ROBs requested/retrieved:" << os.str());
  }
  if (robFragments.size()<1){
    ATH_MSG_INFO("Could not retrieve ROB!");
    return HLT::OK;
  }
  const eformat::ROBFragment<const uint32_t*>* rbf = robFragments[0];

  // 
  uint32_t ctpFormatVersion = CTPfragment::ctpFormatVersion(robFragments[0]);

  
  int m_nitems(0);
  if (ctpFormatVersion < 4) {
    m_nitems = (CTPdataformat::TBPwords_v0)*32;
  } else {
    m_nitems = (CTPdataformat::TBPwords_v4)*32;
  }


  std::vector<uint32_t>::reverse_iterator tbp_it;

  // this is the l1a accept position
  uint32_t l1a = CTPfragment::lvl1AcceptBunch(rbf);

  // start impl
  std::vector<uint32_t> tbp;
  //  int nBITs=m_nitems-1;
  bool firedbc[3];
  firedbc[0]=0;
  firedbc[1]=0;
  firedbc[2]=0;

  bool firedbc_ele[3];
  firedbc_ele[0]=0;
  firedbc_ele[1]=0;
  firedbc_ele[2]=0;
  bool firedbc_mu[3];
  firedbc_mu[0]=0;
  firedbc_mu[1]=0;
  firedbc_mu[2]=0;
  bool firedbc_jet[3];
  firedbc_jet[0]=0;
  firedbc_jet[1]=0;
  firedbc_jet[2]=0;  


  unsigned int ibc=0;
  for(unsigned int bc=l1a-1; bc<l1a+2; bc++) {
    
    tbp = CTPfragment::triggerDecisionBeforePrescales(rbf,bc);
    unsigned ntbpwords(tbp.size());
    for(unsigned iw=0; iw<ntbpwords; iw++  ) {
      uint32_t tmpand = tbp[iw] & m_bitmasks[iw];
      uint32_t tmpand_ele = tbp[iw] & m_bitmasks_ele[iw];
      uint32_t tmpand_mu = tbp[iw] & m_bitmasks_mu[iw];
      uint32_t tmpand_jet = tbp[iw] & m_bitmasks_jets[iw];

      firedbc[ibc] = firedbc[ibc] || (tmpand > 0);
      firedbc_ele[ibc] = firedbc_ele[ibc] || (tmpand_ele > 0);
      firedbc_mu[ibc] = firedbc_mu[ibc] || (tmpand_mu > 0);
      firedbc_jet[ibc] = firedbc_jet[ibc] || (tmpand_jet > 0);

    }
    ibc++;
  }

  //  if (1){

  if ((firedbc[0] && firedbc[1]) || (firedbc[1] && firedbc[2])){
    beforeExecMonitors().ignore();
  
    if(debugoutput){
      ATH_MSG_WARNING("just to print the eventnr");
      printf(" ==> Hi Nils FIRED:  %u - %u - %u\n", firedbc[0],firedbc[1],firedbc[2]);
    }
    
    m_l1a_type = 0;

    if(firedbc_ele[1])  m_l1a_type = 1;
    if(firedbc_mu[1])  m_l1a_type = 2;
    if(firedbc_jet[1])  m_l1a_type = 3;
    if(firedbc_ele[1] && firedbc_mu[1])  m_l1a_type = 4;
    if(firedbc_ele[1] && firedbc_jet[1])  m_l1a_type = 5;
    if(firedbc_mu[1] && firedbc_jet[1])  m_l1a_type = 6;
    if(firedbc_ele[1] && firedbc_mu[1] && firedbc_jet[1])  m_l1a_type = 7;

      
    if((firedbc[0] && firedbc[1])){
      m_beforeafterflag = -1;
 
      if(firedbc_ele[0])  m_other_type = 1;
      if(firedbc_mu[0])  m_other_type = 2;
      if(firedbc_jet[0])  m_other_type = 3;
      if(firedbc_ele[0] && firedbc_mu[0])  m_other_type = 4;
      if(firedbc_ele[0] && firedbc_jet[0])  m_other_type = 5;
      if(firedbc_mu[0] && firedbc_jet[0])  m_other_type = 6;
      if(firedbc_ele[0] && firedbc_mu[0] && firedbc_jet[0])  m_other_type = 7;
      
    }
    else if((firedbc[1] && firedbc[2])){
      m_beforeafterflag = 1;

      if(firedbc_ele[2])  m_other_type = 1;
      if(firedbc_mu[2])  m_other_type = 2;
      if(firedbc_jet[2])  m_other_type = 3;
      if(firedbc_ele[2] && firedbc_mu[2])  m_other_type = 4;
      if(firedbc_ele[2] && firedbc_jet[2])  m_other_type = 5;
      if(firedbc_mu[2] && firedbc_jet[2])  m_other_type = 6;
      if(firedbc_ele[2] && firedbc_mu[2] && firedbc_jet[2])  m_other_type = 7;
    
    }
    if ((firedbc[0] && firedbc[1]) && (firedbc[1] && firedbc[2])){
      m_beforeafterflag = 2;
     
    }

    std::vector<HLT::TriggerElement*> empty_seed;
    HLT::TriggerElement* te = config()->getNavigation()->addNode(empty_seed, output);
    te->setActiveState(true);

      
    if(debugoutput){
      printf(" Full DEBUG output: \n");
    
  

      bool fired[1];
      for(unsigned int bc=l1a-1; bc<l1a+2; bc++) {
	int nBITs=m_nitems-1;
	tbp = CTPfragment::triggerDecisionBeforePrescales(rbf,bc);
	unsigned ntbpwords(tbp.size());
	for(int iw=ntbpwords-1; iw>=0; iw--  ) {
	  //	printf("will try to access iw %u \n", iw);
	  uint32_t tmpand = tbp[iw] & m_bitmasks[iw];
	  std::cout << std::setw(3) << std::dec;
	  for (int32_t bit = 31; bit>=0;--bit,--nBITs) {
	    fired[0]=(((tmpand)>>bit) & 1);
	    
	    if(fired[0]) {
	      if(bc!=l1a) std::cout << "  BC " << std::setw(2) << bc << " Item " << std::setw(3) << nBITs << " ";
	      else std::cout << "> BC " << std::setw(2) << bc << " Item " << std::setw(3) << nBITs << " ";
	      if(fired[0]) std::cout << "TBP "; else std::cout << "    ";
	      
	      std::cout << std::endl;
	    }
	  }
	}
      }// bc
    
      //    printf("second monitoring block \n");
    
      //    for(unsigned int bc=0; bc<nBC; bc++) {
      for(unsigned int bc=l1a-1; bc<l1a+2; bc++) {
	int nBITs=m_nitems-1;
	tbp = CTPfragment::triggerDecisionBeforePrescales(rbf,bc);
	tbp_it = tbp.rbegin();
      
	for(; tbp_it != tbp.rend(); ++tbp_it) {
	  std::cout << std::setw(3) << std::dec;
	  for (int32_t bit = 31; bit>=0;--bit,--nBITs) {
	    fired[0]=(((*tbp_it)>>bit) & 1);
	  
	    if(fired[0]) {
	      if(bc!=l1a) std::cout << "  BC " << std::setw(2) << bc << " Item " << std::setw(3) << nBITs << " ";
	      else std::cout << "> BC " << std::setw(2) << bc << " Item " << std::setw(3) << nBITs << " ";
	      if(fired[0]) std::cout << "TBP "; else std::cout << "    ";

	      std::cout << std::endl;
	    }
	  }
	}
      }// bc
    }


    if (!m_nomuon){

      /// now try to take a look into out of time muons:
      int nmuons = distance(m_trigMuonRoITool->begin_OutOfTimeRoIs(),m_trigMuonRoITool->end_OutOfTimeRoIs());
      if(debugoutput){
	std::cout <<  "=====================================================" << std::endl;
	std::cout <<  " RoIs out of time with event BCID:  Number of RoIs = "
		  << nmuons << std::endl;
	std::cout <<  "=====================================================" << std::endl;
      }
    
      std::vector< std::pair<ROIB::MuCTPIRoI,int> >::const_iterator it_begin_ot, it_end_ot;
    
      if(debugoutput)
	std::cout <<  "===> execute() TrigMuonRoITool Test Algorithm: get begin in time iterator" << std::endl;
    
      it_begin_ot = m_trigMuonRoITool->begin_OutOfTimeRoIs();
      if(debugoutput)
	std::cout <<  "===> execute() TrigMuonRoITool Test Algorithm: get end   in time iterator" << std::endl;
    
      it_end_ot   = m_trigMuonRoITool->end_OutOfTimeRoIs();
      if(debugoutput)
	std::cout <<  "===> execute() TrigMuonRoITool Test Algorithm: print out of time RoIs" << std::endl;
    
      for  (std::vector< std::pair<ROIB::MuCTPIRoI,int> >::const_iterator it = m_trigMuonRoITool->begin_OutOfTimeRoIs();
	    it != m_trigMuonRoITool->end_OutOfTimeRoIs(); ++it) {

	if (abs((*it).second) !=1) continue;
	if(debugoutput){
	  std::cout <<  " Difference(RoI(BCID) - Event(BCID)) = " << (*it).second << std::endl;
	  std::cout <<  " ------------------------------------- " << std::endl;
	  std::cout <<  "RoIB word               : 0x" << MSG::hex << ((*it).first).roIWord() << MSG::dec << std::endl;
	  std::cout <<  "Threshold               :  pt" << ((*it).first).pt() << std::endl;
	  std::cout <<  "Sector ID               :  " << ((*it).first).getSectorID() << std::endl;
	  std::cout <<  "Sector addr             :  0x" << MSG::hex << ((*it).first).getSectorAddress() << MSG::dec << std::endl;
	  std::cout <<  "Sector overflow         :  " << ((*it).first).getSectorOverflow() << std::endl;
	  std::cout <<  "RoI overflow            :  " << ((*it).first).getRoiOverflow() << std::endl;
	  std::cout <<  "RoI number              :  " << ((*it).first).getRoiNumber() << std::endl;
	  std::cout <<  "IsHighestPt             :  " << ((*it).first).getCandidateIsHighestPt() << std::endl;
	}

	unsigned int temp_sysID = getBitMaskValue(((*it).first).getSectorAddress(), LVL1::SysIDMask );
	unsigned int sysID = 0;                // Barrel
	if( temp_sysID & 0x2 ) sysID = 1;      // Endcap
	else if( temp_sysID & 0x1 ) sysID = 2; // Forward
          
	const LVL1::RecMuonRoiSvc* recMuonRoiSvc = 0;
	std::string region = "";
	if( sysID == 0 ) {
	  recMuonRoiSvc = &(*m_recRPCRoiSvc);
	  region = "Barrel region";
	} else if ( sysID == 1 ){
	  recMuonRoiSvc = &(*m_recTGCRoiSvc);
	  region = "Endcap region";
	} else {
	  recMuonRoiSvc = &(*m_recTGCRoiSvc);
	  region = "Forward region";
	}
     
  
	recMuonRoiSvc->reconstruct( ((*it).first).roIWord() );
	// create new trigger element for this out of time RoI
	float eta = recMuonRoiSvc->eta();
	float phi = recMuonRoiSvc->phi();
	if(debugoutput){
	  std::cout <<  "recoETA             :  " <<  eta << std::endl;
	  std::cout <<  "recoPHI             :  " <<  phi << std::endl;     
	  std::cout <<  "=================================================" << std::endl;
	}
	m_etaOutOfTimeMuon.push_back(eta);
	m_phiOutOfTimeMuon.push_back(phi);
	m_ptThrOutOfTimeMuon.push_back(((*it).first).pt());
     
      }
    }
    afterExecMonitors().ignore();
  
  }
  
  return HLT::OK;
}


