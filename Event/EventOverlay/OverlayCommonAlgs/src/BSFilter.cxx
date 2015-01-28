/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include <algorithm>
#include <iostream>
#include "GaudiKernel/Property.h"
#include "StoreGate/StoreGateSvc.h"
#include "StoreGate/DataHandle.h"
#include "OverlayCommonAlgs/BSFilter.h"
#include "TrigT1Result/CTP_RDO.h"
#include "TrigT1Result/CTP_RIO.h"
#include "TrigT1Result/CTP_Decoder.h"
#include "EventInfo/EventInfo.h"
#include "EventInfo/EventID.h"
#include "EventInfo/TriggerInfo.h"

BSFilter::BSFilter( const std::string& name,ISvcLocator* pSvcLocator ) : 
    AthAlgorithm (name, pSvcLocator), mLog( msgSvc(),name ), m_storeGate(0)

{
  declareProperty("TriggerBit", m_trigbit=63);
  declareProperty("filterfile", m_filterfile="");
  declareProperty("EventIdFile", m_EventIdFile="");
}

BSFilter::~BSFilter(){}

StatusCode BSFilter::initialize()
{
  All=0; pass=0;
  StatusCode  sc;
  ATH_MSG_INFO( "Initializing BSFilter" );

  sc = service("StoreGateSvc", m_storeGate);
  if (sc.isFailure()) 
    {
      ATH_MSG_ERROR("Unable to retrieve pointer to StoreGateSvc");
      return sc;
    }
  
  if ( !evtStore().retrieve().isSuccess() ) 
    {
      ATH_MSG_ERROR ("Unable to retrieve evtStore");
      return StatusCode::FAILURE;
    }

  ////////////////////////////
  if (m_filterfile!=""){
    FILE *vfile = fopen(m_filterfile.c_str(),"r");
    if (vfile){
    msg(MSG::INFO)<<"Opened filter file: "<<m_filterfile<<endreq;
    int vrun=0, vevent=0, vtrig=0, vnvtx=0; double vdt=0.0; int ne=0;
    while (true){
      int r = fscanf(vfile, "%i %i %i %i %lf\n", &vrun, &vevent, &vtrig, &vnvtx, &vdt);
      if (r>0){
	msg(MSG::DEBUG) << "Read "<<r<<" filter values: "<<vrun<<"/"<<vevent<<" "<<vtrig<<","<<vnvtx<<","<<vdt<<endreq;
	if (filtermap[vrun][vevent].magic==777){
	  msg(MSG::WARNING)<<"Already filter info for run/event "<<vrun<<"/"<<vevent<<", magic="<<filtermap[vrun][vevent].magic<<endreq;
	}
	filtermap[vrun][vevent].trig=vtrig;
	filtermap[vrun][vevent].nvtx=vnvtx;
	filtermap[vrun][vevent].dt=vdt;
	filtermap[vrun][vevent].magic=777;
	++ne;
      }
      else{
	msg(MSG::INFO) << "Got "<<r<<" from fscanf, stopping"<<endreq;
	break;
      }
    }
    fclose(vfile);
    msg(MSG::INFO)<<"Read "<<ne<<" filter entries from file."<<endreq;
    }
    else{
      msg(MSG::ERROR)<<"Could not open filter file: "<<m_filterfile<<endreq;
    }
  }

  //////////////////////////
  efile=NULL;
  if (m_EventIdFile!=""){
    efile = fopen(m_EventIdFile.c_str(),"w");
    if (efile){
      msg(MSG::INFO)<<"Opended EventIdFile: "<<m_EventIdFile<<endreq;
    }
  }

  return StatusCode::SUCCESS;
}

StatusCode BSFilter::finalize()
{
  ATH_MSG_INFO("BS Filter PASSED "<<pass<<" FROM "<< All);
  return StatusCode::SUCCESS;
}

StatusCode BSFilter::execute()
{  
  All++;
  StatusCode sc = StatusCode::SUCCESS;
  ATH_MSG_DEBUG("BS Filter");


  // Get the EventInfo obj for run/event number
  const DataHandle<EventInfo> d;
  sc = m_storeGate->retrieve(d);
  if (sc.isFailure()) {
    ATH_MSG_ERROR(" Cant retrieve EventInfo ");
    return StatusCode::FAILURE;
  }
  uint32_t run     = d->event_ID()->run_number();
  uint32_t  event = d->event_ID()->event_number();
  //  uint64_t bc_time    = d->event_ID()->time_stamp() + ((uint64_t)(d->event_ID()->time_stamp_ns_offset()) << 32);
  uint32_t bc_time_sec    = d->event_ID()->time_stamp();
  //uint32_t bc_time_ns     = d->event_ID()->time_stamp_ns_offset();
  //uint32_t run_type   = 0;
  //uint32_t lvl1_id    = event; // FIXME... temp place for event number
  //uint32_t lvl1_type  = d->trigger_info()->level1TriggerType();
  //uint32_t global_id  = event;
  uint16_t lbn = d->event_ID()->lumi_block();
  //uint16_t bcid = d->event_ID()->bunch_crossing_id();
  
  ////////////////////////////////////////////////////////////
  if (m_trigbit>=0){
  const DataHandle< CTP_RDO > ctpRDO;
  const DataHandle< CTP_RIO > ctpRIO;
  
  sc = m_storeGate->retrieve( ctpRDO, "CTP_RDO" ); 
  if (sc.isFailure()) 
    {
      ATH_MSG_ERROR("Unable to retrieve CTP_RDO");
      return sc;
    }
  sc = m_storeGate->retrieve( ctpRIO, "CTP_RIO" ); 
  if (sc.isFailure()) 
    {
      ATH_MSG_ERROR("Unable to retrieve CTP_RIO");
      return sc;
    }
  
  CTP_Decoder ctp;
  ctp.setRDO(ctpRDO);
  uint16_t l1aPos = ctpRDO->getL1AcceptBunchPosition(); //ctpRIO->getDetectorEventType() >> 16;
  
  // list of all trigger items that fired in this event
  std::vector<unsigned int> triggersFired = ctp.getAllTriggers(l1aPos);
  
  // the item number you want to check, e.g. item3 (pt3, i.e. RPCs)
  unsigned int item_no = m_trigbit; 
  
  // this is true if the corresponding item fired
  //bool item_fired = ctp.checkTrigger(item_no,l1aPos); 
  
  // this is true if the corresponding item would have fired if it hadn't 
  // been vetoed... like that you can check, if for example in a run with 
  // the trigger mask set to the muon inputs (RPC/TGC), the TileCal NIM 
  // trigger was sent. Since the Tile trigger is masked out, it will only 
  // appear as 'before veto'
  bool item_fired_before_veto =  ctp.checkTriggerAfterPrescale(item_no,l1aPos); 
  ATH_MSG_INFO("Trig "<<item_no<<" fired before veto? "<<item_fired_before_veto);

  bool item_fired_after_veto =  ctp.checkTrigger(item_no,l1aPos); 
  //bool item_fired_after_veto = std::find(triggersFired.begin(), triggersFired.end(), item_no-1)!=triggersFired.end();
  ATH_MSG_INFO("Trig "<<item_no<<" fired after veto? "<<item_fired_after_veto);
  
  if(item_fired_after_veto)    {
    assert(item_fired_before_veto);//or else how could it pass after veto??
    ATH_MSG_DEBUG("Filter Passed");
    setFilterPassed(true);
    pass++;
    if (efile) fprintf(efile,"svcMgr.EvtIdModifierSvc.add_modifier(run_nbr=%d, evt_nbr=%d, time_stamp=%d, lbk_nbr=%d, nevts=1)\n",run,event,bc_time_sec,lbn);
  }
  else    {
    ATH_MSG_DEBUG("Filter Failed");
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
      if (filtermap[run][event].magic!=777){
	ATH_MSG_WARNING("Dont have info in filtermap for "<<run<<" "<<event);
      }
      else{
	if (filtermap[run][event].nvtx==1 && filtermap[run][event].trig==1 && fabs(filtermap[run][event].dt) <3.0){
	  passed=true;
	  ATH_MSG_INFO("Passing filter event "<<run<<" "<<event<<", trig run dt magic: "<<filtermap[run][event].trig<<" "<<filtermap[run][event].nvtx<<" "<<filtermap[run][event].dt<<" "<<filtermap[run][event].magic);
	}
	else{
	  ATH_MSG_INFO("Not passing filter event "<<run<<" "<<event<<", trig run dt magic: "<<filtermap[run][event].trig<<" "<<filtermap[run][event].nvtx<<" "<<filtermap[run][event].dt<<" "<<filtermap[run][event].magic);
	}
      }
    }
    //

    if(passed)    {
      ATH_MSG_DEBUG("Filter Passed");
      setFilterPassed(true);
      pass++;
      if (efile) fprintf(efile,"svcMgr.EvtIdModifierSvc.add_modifier(run_nbr=%d, evt_nbr=%d, time_stamp=%d, lbk_nbr=%d, nevts=1)\n",run,event,bc_time_sec,lbn);
    }
    else    {
      ATH_MSG_DEBUG("Filter Failed");
      setFilterPassed(false);
    }
  }
  return StatusCode::SUCCESS;
}
