/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#include <algorithm>
#include <iostream>

#include "StoreGate/StoreGateSvc.h"
#include "StoreGate/DataHandle.h"

#include "BSFilter.h"

#include "TrigSteeringEvent/Chain.h"
#include "TrigSteeringEvent/HLTResult.h"
#include "TrigConfHLTData/HLTChainList.h"
#include "TrigT1Result/CTP_RDO.h"
#include "TrigT1Result/CTP_Decoder.h"

using namespace HLT;

BSFilter::BSFilter( const std::string& name,ISvcLocator* pSvcLocator ) :
  AthAlgorithm (name, pSvcLocator),m_All(0),m_pass(0),m_EventCounter(0),
  m_trigbit(63),m_filterfile(""),m_efile(nullptr),m_EventIdFile(""),
  m_trigConf( "TrigConf::TrigConfigSvc/TrigConfigSvc", name )
{
  declareProperty("TriggerBit", m_trigbit);
  declareProperty("filterfile", m_filterfile);
  declareProperty("EventIdFile", m_EventIdFile);
}

BSFilter::~BSFilter(){}

StatusCode BSFilter::initialize()
{
  m_All=0; m_pass=0;
  ATH_MSG_INFO( "Initializing BSFilter" );
  CHECK( m_trigConf.retrieve() );

  ////////////////////////////
  if (m_filterfile!="" && m_trigbit<0){
    FILE *vfile = fopen(m_filterfile.c_str(),"r");
    if (vfile){
      msg(MSG::INFO)<<"Opened filter file: "<<m_filterfile<<endmsg;
      int vrun=0, vtrig=0, vnvtx=0; double vdt=0.0; int ne=0; uint64_t vevent=0;
      while (true){
        int r = fscanf(vfile, "%i %lu %i %i %lf\n", &vrun, &vevent, &vtrig, &vnvtx, &vdt);
        if (r>0){
          msg(MSG::DEBUG) << "Read "<<r<<" filter values: "<<vrun<<"/"<<vevent<<" "<<vtrig<<","<<vnvtx<<","<<vdt<<endmsg;
          if (m_filtermap[vrun][vevent].magic==777){
            msg(MSG::WARNING)<<"Already filter info for run/event "<<vrun<<"/"<<vevent<<", magic="<<m_filtermap[vrun][vevent].magic<<endmsg;
          }
          m_filtermap[vrun][vevent].trig=vtrig;
          m_filtermap[vrun][vevent].nvtx=vnvtx;
          m_filtermap[vrun][vevent].dt=vdt;
          m_filtermap[vrun][vevent].magic=777;
          ++ne;
        }
        else{
          msg(MSG::INFO) << "Got "<<r<<" from fscanf, stopping"<<endmsg;
          break;
        }
      }
      fclose(vfile);
      msg(MSG::INFO)<<"Read "<<ne<<" filter entries from file."<<endmsg;
    }
    else{
      msg(MSG::ERROR)<<"Could not open filter file: "<<m_filterfile<<endmsg;
    }
  }

  //////////////////////////
  m_efile=NULL;
  if (m_EventIdFile!=""){
    m_efile = fopen(m_EventIdFile.c_str(),"w");
    if (m_efile){
      msg(MSG::INFO)<<"Opended EventIdFile: "<<m_EventIdFile<<endmsg;
    }
    else{
      msg(MSG::ERROR)<<"Could not open EventIdFile output: "<<m_EventIdFile<<endmsg;
    }
  }

  return StatusCode::SUCCESS;
}

StatusCode BSFilter::finalize()
{
  ATH_MSG_INFO("BS Filter PASSED "<<m_pass<<" FROM "<< m_All);
  return StatusCode::SUCCESS;
}

StatusCode BSFilter::execute()
{
  m_All++;
  ATH_MSG_INFO("BS Filter");


  // Get event information from EventContext
  const EventContext& ctx{Gaudi::Hive::currentContext()};
  uint32_t run   = ctx.eventID().run_number();
  uint64_t event = ctx.eventID().event_number();
  uint32_t time_stamp = ctx.eventID().time_stamp();
  uint32_t lbn = ctx.eventID().lumi_block();


  if (m_trigbit>=0){
    const DataHandle< CTP_RDO > ctpRDO;
    StatusCode sc = StatusCode::SUCCESS;
    sc = evtStore()->retrieve( ctpRDO, "CTP_RDO" );
    if (sc.isFailure())  {
      ATH_MSG_ERROR("Unable to retrieve CTP_RDO");
      return sc;
    }
    CTP_Decoder ctp;
    ctp.setRDO(ctpRDO);
    uint16_t l1aPos = ctpRDO->getL1AcceptBunchPosition(); //ctpRIO->getDetectorEventType() >> 16;
    std::vector<unsigned int> triggersFired = ctp.getAllTriggers(l1aPos); // list of all trigger items that fired in this event
    unsigned int item_no = m_trigbit; // the item number you want to check, e.g. item3 (pt3, i.e. RPCs)
    bool item_fired_after_veto =  ctp.checkTrigger(item_no,l1aPos);
    ATH_MSG_INFO("Trig "<<item_no<<" fired after veto? "<<item_fired_after_veto);

    
    /////////////////////////// HLT ////////////////////////////////////////////////
    //ATH_MSG_INFO("evtStore:\n" << evtStore()->dump());
    std::string keyResult = "HLTResult_HLT";
    const HLT::HLTResult *hlt_result = 0;
    if(evtStore()->retrieve<HLT::HLTResult>(hlt_result, keyResult).isFailure() || !hlt_result) {
      ATH_MSG_WARNING("Failed to retrieve HLTResult: "<<keyResult );
      return StatusCode::FAILURE;
    }
    ATH_MSG_INFO("Retrieved HLTResult '"<<keyResult<<"' containing " << hlt_result->getChainResult().size()-1 << " chain(s)" );
    const std::vector<uint32_t>& chainsData = hlt_result->getChainResult();
    assert(chainsData[0]==(chainsData.size()-1));// first entry is number of chains
    std::map<unsigned int, uint32_t> chains;  // map of <counter,chain>
    for (unsigned int i = 1; i < (chainsData.size()-1); ++i) {
      Chain myChain(chainsData[i]);
      //ATH_MSG_INFO(myChain);
      chains[myChain.getChainCounter()]=chainsData[i];
    }
    const TrigConf::HLTChainList* hltchains = m_trigConf->chainList();
    int passed_noalg=-1,passed_j40=-1,prescale_noalg=-1,prescale_j40=-1;
    if( hltchains ) {
      for (auto itr = hltchains->begin(); itr != hltchains->end(); ++itr ) {
	TrigConf::HLTChain* c=*itr;
	auto it=chains.find(c->chain_counter());
	if (it!=chains.end()){
	  const std::string& name =c->chain_name();
	  std::size_t found = name.find("L1ZB");
	  if (found!=std::string::npos){
	    Chain myChain(it->second);
	    ATH_MSG_INFO("chain :"<<name<<", level:"<<c->level()<<", counter:"<<c->chain_counter()<<", prescale:"<<c->prescale()<<", passed:"<<myChain.chainPassed());
	    if (name=="HLT_noalg_zb_L1ZB"){
	      ATH_MSG_INFO("HLT_noalg_zb_L1ZB passed? " << myChain.chainPassed());
	      passed_noalg=myChain.chainPassed();
	      prescale_noalg=c->prescale();
	    }
	    else if (name=="HLT_j40_L1ZB"){
	      ATH_MSG_INFO("HLT_j40_L1ZB passed? " << myChain.chainPassed());
	      passed_j40=myChain.chainPassed();
	      prescale_j40=c->prescale();
	    }
	  }
	}
      }
    }
    else{
      ATH_MSG_WARNING("No trig chains?!");
    }
    //////////////////////////////////////////////////////////////////////


    //If we have trigbit>0 and a filterfile, write the trigger info out to the filterfile
    if (m_filterfile!=""){
      static FILE* ffile=NULL;
      if (!ffile){
	ffile = fopen(m_filterfile.c_str(),"w");
	if (ffile){
	  msg(MSG::INFO)<<"Opended FilterFile output for Trigger info: "<<m_filterfile<<endmsg;
	}
	else {
	  msg(MSG::ERROR)<<"Could not open FilterFile output for Trigger info: "<<m_filterfile<<endmsg;
	}
      }
      if (ffile) fprintf(ffile,"run_nbr=%d, evt_nbr=%ld, time_stamp=%d, lbk_nbr=%d, noalg=%d, j40=%d, noalgps=%d, j40ps=%d\n",
			 run,event,time_stamp,lbn,passed_noalg,passed_j40,prescale_noalg,prescale_j40);
    }
    

    if(item_fired_after_veto && m_filterfile=="") {//don't bother to write out RAW if we're only running to make the trigs.txt file
      // assert(item_fired_before_veto); //or else how could it m_pass after veto??
      ATH_MSG_INFO("Filter Passed");
      setFilterPassed(true);
      m_pass++;
      if (m_efile) fprintf(m_efile,"svcMgr.EvtIdModifierSvc.add_modifier(run_nbr=%d, evt_nbr=%ld, time_stamp=%d, lbk_nbr=%d, nevts=1)\n",run,event,time_stamp,lbn); //"%ld" for evt_nbr since it's 64 bit, but need fix for https://its.cern.ch/jira/browse/ATEAM-286 first!
    }
    else    {
      ATH_MSG_INFO("Filter Failed");
      setFilterPassed(false);
    }

    
  }//m_trigbit>=0
  else{
    

    //Filter on TAG info?
    ATH_MSG_INFO("Will filter on TAG because m_trigbit="<<m_trigbit);
    bool passed = true;
    //Here is where you'd check for TAG info of some kind...
    if (m_filterfile!=""){
      passed=false;
      if (m_filtermap[run][event].magic!=777){
        ATH_MSG_WARNING("Dont have info in filtermap for "<<run<<" "<<event);
      }
      else{
        if (m_filtermap[run][event].nvtx==1 && m_filtermap[run][event].trig==1 && fabs(m_filtermap[run][event].dt) <3.0){
          passed=true;
          ATH_MSG_INFO("Passing filter event "<<run<<" "<<event<<", trig run dt magic: "<<m_filtermap[run][event].trig<<" "<<m_filtermap[run][event].nvtx<<" "<<m_filtermap[run][event].dt<<" "<<m_filtermap[run][event].magic);
        }
        else{
          ATH_MSG_INFO("Not passing filter event "<<run<<" "<<event<<", trig run dt magic: "<<m_filtermap[run][event].trig<<" "<<m_filtermap[run][event].nvtx<<" "<<m_filtermap[run][event].dt<<" "<<m_filtermap[run][event].magic);
        }
      }
    }
    else{
      ATH_MSG_INFO("No filterfile");
    }

    if(passed)    {
      ATH_MSG_INFO("Filter Passed");
      setFilterPassed(true);
      m_pass++;
      if (m_efile) fprintf(m_efile,"svcMgr.EvtIdModifierSvc.add_modifier(run_nbr=%d, evt_nbr=%ld, time_stamp=%d, lbk_nbr=%d, nevts=1)\n",run,event,time_stamp,lbn); //"%ld" for evt_n    br since it's 64 bit, but need fix for https://its.cern.ch/jira/browse/ATEAM-286 first!
    }
    else    {
      ATH_MSG_INFO("Filter Failed");
      setFilterPassed(false);
    }


  }
  return StatusCode::SUCCESS;
}
