/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


/**
* @file SCT_CalibHvSvc.cxx
* Implementation file for the SCT_CalibHvSvc class
* @author Shaun Roe
**/

#include "SCT_CalibHvSvc.h"
#include "SCT_CalibUtilities.h"
#include "SCT_CalibNumbers.h"
#include "SCT_Cabling/ISCT_CablingSvc.h"  //template parameter


#include "EventInfo/EventInfo.h"
#include "EventInfo/EventID.h"

using namespace std;
using namespace SCT_CalibAlgs;

//const static int n_chipsPerSide(6);
//const static int n_stripsPerChip(128);
const static string detectorNames[] = { "negativeEndcap", "barrel", "positiveEndcap" };


const static string detectorPaths[]={"SCTEC/", "SCTB/","SCTEA/"};
namespace{
  void
  initQueue(std::queue<int> & q, const int numvals, const int setval){
    for (int i(0);i<numvals;++i) q.push(setval);
  }

}


SCT_CalibHvSvc::SCT_CalibHvSvc(const std::string &name, ISvcLocator * svc):
  AthService(name,svc),
  m_detStore("DetectorStore", name),
  m_evtStore("StoreGateSvc", name),
  m_DCSConditionsSvc( "SCT_DCSConditionsSvc", name ),
  m_cablingSvc( "SCT_CablingSvc", name ),
  m_pSCTHelper(0),
  m_sct_waferHash(0),  
  m_sct_numHitsInWafer(0),
  m_lumiBlock(0),
  m_maxq(100),
  m_phvtripPrevTime(0),
  m_phvtripFirstTime(0),
  m_absolutetriplimit(0),
  m_relativetriplimit(0),
  m_tq{0},
  m_evt(0),
  m_outputLowHits(false),m_lowHitCut(100)
{
}


StatusCode 
SCT_CalibHvSvc::initialize(){
  //if (not retrievedService(m_DCSConditionsSvc, "DCSConditionsSvc")) return StatusCode::FAILURE;
  
  return StatusCode::SUCCESS;

}

StatusCode 
SCT_CalibHvSvc::finalize(){
  return StatusCode::SUCCESS;
}

StatusCode 
SCT_CalibHvSvc::queryInterface(const InterfaceID & riid, void** ppvInterface ){
  if ( ISCT_CalibHistoSvc::interfaceID().versionMatch(riid) ) {
    *ppvInterface = dynamic_cast<ISCT_CalibHistoSvc*>(this);
  } else {
    return AthService::queryInterface(riid, ppvInterface);
  }
  addRef();
  return StatusCode::SUCCESS;
}


bool
SCT_CalibHvSvc::book(){
  bool result(true);
  if (msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << "Book HVTrips" <<endreq;
   //now initialize the queues (fill 100 spots with 0):
  queue<int> qtemp;
  initQueue(qtemp, m_maxq,0);
   // need to keep previous bins lbn 4 deep. 
  initQueue(m_prevLBN, 4, 0);
  vector< pair <int,int> > dummy;      
   //first set num events processed to 0
  //int n_elements=8176;  //info taken from SCTCalib.h: SCT specific number 
  m_phvtripProcessedEventsInt.insert(m_phvtripProcessedEventsInt.end(), n_elements, 1);
  m_phvtripRunningTotalInt.insert(m_phvtripRunningTotalInt.end(), n_elements, 0);
  m_phvtripHasItTripped.insert(m_phvtripHasItTripped.end(),n_elements, 0);
  m_phvtripQueue.insert(m_phvtripQueue.end(), n_elements, qtemp);
  m_summarytrips.insert(m_summarytrips.end(),n_elements, dummy);
  m_summarytripslb.insert(m_summarytripslb.end(), n_elements, dummy);
  m_phvtripHasItTripped_prev = m_phvtripHasItTripped;
  return result;
}

bool 
SCT_CalibHvSvc::fill(const bool fromData){
  if (fromData){
    return fillFromData(); 
  }
  bool result(true);
  int lumi_block(0);//fix me!;
  //int event_number;
  const int wafersize = m_sct_waferHash->size();
  int time_stamp= m_evt->event_ID()->time_stamp();
  int curr_time =  time_stamp;
  int dtime = curr_time - m_phvtripPrevTime;
  int totalHits = 0;
  if (curr_time<m_phvtripPrevTime){ 
    msg(MSG::ERROR) << "Events not sorted properly (time is going backwards!) " << endreq;
    return false;
  }
  // for first event
  if (m_phvtripFirstTime > curr_time){ m_phvtripFirstTime = curr_time; }
  bool newbin = false;
  // check if we have a new time bin, if we do pop off the back (oldest) part 
  // of the queue and enqueue 0 for every wafer
  int maxtbins(5);
  if (dtime > maxtbins) {
    newbin = true;
    if (msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << " new t bin " << dtime <<" since start "<< (curr_time-(m_phvtripFirstTime-1.01)) << endreq;
    m_phvtripHasItTripped_prev = m_phvtripHasItTripped;
    for (int iwaf = 0; iwaf!=n_elements; ++iwaf){
      m_phvtripQueue[iwaf].pop();
      m_phvtripQueue[iwaf].push(0);
      m_phvtripHasItTripped[iwaf]=0;
    }
  }
    //if (msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << "time stamp "<<time_stamp <<" event number "<< Event_number<<endreq;
  // Loop over all hits in the event 
  bool isgoodnow;
  for( int itrk=0; itrk!=wafersize; ++itrk){
    int waferhash = (*m_sct_waferHash)[itrk];
    Identifier waferId = m_pSCTHelper->wafer_id(waferhash);
    Identifier moduleId = m_pSCTHelper->module_id(waferId);
    //step one is to make sure this one isn't already know to be messed up:
    isgoodnow = m_DCSConditionsSvc->isGood(moduleId,InDetConditions::SCT_MODULE);
    if (msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << "checked is good "<< isgoodnow <<endreq;
    if (isgoodnow){
      int numhits = (*m_sct_numHitsInWafer)[itrk];
      totalHits += numhits;
    /**
    calculate the limit for an individual module, limit is m_relativetriplimit (default = 4) times the running average in that module. 
    limits are given in hits/second. need to convert numhits in bin to average (within bin) hits/sec.
    **/
    double limit =  m_relativetriplimit * (double)m_phvtripRunningTotalInt[waferhash] / ((double)curr_time-((double)m_phvtripFirstTime-0.01));

  if (msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << "waferhash "<<waferhash<<" itrk "<<itrk <<" run tot "<< m_phvtripRunningTotalInt[waferhash] 
    <<" num hits "<<numhits<<" evnts processed "<<  m_phvtripProcessedEventsInt[waferhash] <<" limit "<<limit
    <<" abs limit, rel limit "<<m_absolutetriplimit<<","<< m_relativetriplimit<<" max bins "<<maxtbins
    <<endreq;

  if ( ((numhits + m_phvtripQueue[waferhash].back() )/ (double)maxtbins ) > m_absolutetriplimit 
  and ((numhits + m_phvtripQueue[waferhash].back() )/ (double)maxtbins ) > limit) {
        //read back queue and see if the high hit rate is persistant for 3 previous bins (a bin is 5 seconds by default)
    for (int iq = 0; iq<m_maxq; ++iq){
      m_tq[iq]=m_phvtripQueue[waferhash].front();
      m_phvtripQueue[waferhash].pop();
    }
    if (m_tq[m_maxq-2]/(double)maxtbins > limit && m_tq[m_maxq-3]/(double)maxtbins > limit && m_tq[m_maxq-4]/(double)maxtbins > limit){
        // found a persistent trip/thing
      if (msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << "Found a potential trip in SCT wafer "<< waferhash<<endreq;
            //if (msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << "TRIP: "<< waferhash<<" "<<time_stamp <<" "<< Event_number <<" "<< (numhits + m_tq[m_maxq-1] ) <<endreq;
      if (msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << "TRIP more info: limit = "<< limit <<" abslimit = "<<  m_absolutetriplimit 
        <<" hits/secs now = "<< ((numhits + m_tq[m_maxq-1])/ maxtbins ) 
        <<" running total = "<< m_phvtripRunningTotalInt[waferhash]
        <<" running average =  "<<  m_phvtripRunningTotalInt[waferhash] / (curr_time-(m_phvtripFirstTime-1.01)) << endreq;
      m_phvtripHasItTripped[waferhash] =  (numhits + m_tq[m_maxq-1] );
    } // end trip found

        //put queue back the way we found it:
    for (int iq = 0; iq < m_maxq; ++iq) {
      m_phvtripQueue[waferhash].push(m_tq[iq]);
    }
  } // end potential trip id
      //incriment event counter
  m_phvtripProcessedEventsInt[waferhash] += 1;
      //recalcuate running total:
  m_phvtripRunningTotalInt[waferhash] += numhits;
  m_phvtripQueue[waferhash].back() += numhits;
} //end is good check   
else { 
  if (msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << "Was a bad module already: "<< waferhash <<endreq; 
}
} //end loop over tracks

// text output, summarizing the event
//if new bin summarzie the previous event. 
if (newbin) { 
  SCT_ID::const_id_iterator waferItr  = m_waferItrBegin;
  SCT_ID::const_id_iterator waferItrE = m_waferItrEnd;
  for( ; waferItr not_eq waferItrE; ++waferItr ) {
    Identifier       waferId   = *waferItr;
    IdentifierHash   waferHash = m_pSCTHelper->wafer_hash( waferId );
//    SCT_SerialNumber sn        = m_cablingSvc->getSerialNumberFromHash( waferHash );
    if (m_phvtripHasItTripped_prev[waferHash]>0) {
      pair<int,int> wp;
      wp.first = (m_phvtripPrevTime - 3*maxtbins);
      wp.second = (m_phvtripPrevTime + maxtbins);
      m_summarytrips[waferHash].push_back(wp);
      pair<int,int> lbn;
      lbn.first = m_prevLBN.front();
      lbn.second = m_prevLBN.back();
      m_summarytripslb[waferHash].push_back(lbn);
    }
  }
  m_phvtripPrevTime = curr_time; //reset prev and current time
  m_prevLBN.pop();
  m_prevLBN.push(lumi_block);
}

for (int iwaf(0); iwaf!=n_elements; ++iwaf){
  if (msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << "mod "<< iwaf <<" events with that waf " <<m_phvtripProcessedEventsInt[iwaf] << " numhits "<<m_phvtripRunningTotalInt[iwaf] <<endreq;
  if (msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << "did it trip "<<m_phvtripHasItTripped[iwaf] << endreq;
  if (m_phvtripHasItTripped[iwaf]) {
        //if (msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << "TRIP: "<< iwaf <<" "<<time_stamp <<" "<< Event_number <<" "<<  m_phvtripQueue[iwaf].back() <<endreq;
    if (msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << "TRIP more info: abslimit = "<<  m_absolutetriplimit 
      <<" hits/secs now = "<< ( m_phvtripQueue[iwaf].back() / maxtbins ) 
      <<" running total = "<< m_phvtripRunningTotalInt[iwaf]
      <<" running average =  "<<  m_phvtripRunningTotalInt[iwaf] / (curr_time-(m_phvtripFirstTime-1.01)) << endreq;
  }
}
if (m_outputLowHits && (totalHits < m_lowHitCut) ){
  //msg(MSG::WARNING) <<"Event: " << Event_number << " Total hits: " << totalHits << endreq;
}
return result;
}


bool 
SCT_CalibHvSvc::fillFromData(){
  bool result(true);


  return result;
}








