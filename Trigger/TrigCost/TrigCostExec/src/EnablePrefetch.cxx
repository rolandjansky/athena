/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// C/C++
#include <cmath>
#include <iostream>
#include <sstream>

// ROOT
#include "TH1.h"
#include "TH2.h"
#include "TCanvas.h"
#include "TImageDump.h"

// Local
#include "TrigCostBase/Factory.h"
#include "TrigCostBase/HistMan.h"
#include "TrigCostBase/UtilCore.h"

// Local
#include "TrigCostExec/RobService.h"
#include "TrigCostBase/UtilTrig.h"
#include "TrigCostExec/EnablePrefetch.h"

//Local
#include "TrigMonitoringEvent/TrigMonROBData.h"

REGISTER_ANP_OBJECT(AlgEvent,EnablePrefetch)

using namespace std;
using namespace Trig;

//-----------------------------------------------------------------------------
// Begin EnablePrefetch code
//
Anp::EnablePrefetch::EnablePrefetch()
  :fDir(0),
   fDirName("time"),
   fRootName(""),
   fDebug(false),
   fIsConfig(false),
   fLevel(0)
{
}

//-----------------------------------------------------------------------------
Anp::EnablePrefetch::~EnablePrefetch()
{
}

//-----------------------------------------------------------------------------
void Anp::EnablePrefetch::Config(const Registry &reg)
{
  fReg = reg;

  reg.Get("EnablePrefetch", "Debug",        fDebug);
  reg.Get("EnablePrefetch::DirName",        fDirName);
  reg.Get("EnablePrefetch::Level",          fLevel);
  reg.Get("EnablePrefetch::RootFile",       fRootName);
  reg.Get("EnablePrefetch::PrefetchType",   fType);

  cout << "EnablePrefetch::Config - Type: " << fType << endl;

  //                                                                                                                                                   
  // Initiliaze input data pointers                                                                                                                    
  //                                                                                                                                                   
  fEntry = NtupleSvc::Instance("event").Branch<TrigMonEvent>("data");

  //                                                                                                                                                   
  // Register with trigger configuration algorithm                                                                                                     
  //                                                                                                                                                   
  Anp::RegisterCallback(reg, "StudyTime::ConfAlg", fConf, this);

}

//-----------------------------------------------------------------------------
void Anp::EnablePrefetch::Signal(const Registry &, const string &signal)
{
  //
  // Handle callback or parent signal
  //
  if(signal != "TRIG_CONFIG" && signal != "TRIG_CONFIG_DB") return;

  if(fConf.valid()) { 
    if(!fIsConfig) {
      Set(fConf->GetConfig()); 
      fIsConfig = true;
    }
  }
  else {
    log() << "Signal - invalid configuration algorithm" << endl;   
  }
}

//-----------------------------------------------------------------------------
void Anp::EnablePrefetch::Save(TDirectory *dir)
{
  fDir = Anp::GetDir(dir, fDirName);
}

//-----------------------------------------------------------------------------
bool Anp::EnablePrefetch::Init()
{  
  //
  // Read configuration from init file and release pointer
  //

  return true;
}

//-----------------------------------------------------------------------------
void Anp::EnablePrefetch::Exec()
{
  //
  // Process data for one event
  // 
  if(fDebug) cout << "EnablePrefetch::Exec - Called" << endl;
  if(fEntry.valid() && fEntry.GetState() == Ntuple::kREAD) {
    Run(fEntry.GetRef());
  }

}

//-----------------------------------------------------------------------------
void Anp::EnablePrefetch::Done()
{
  //
  // Reset active chains, sequences and algorithms from last event
  //

}

//-----------------------------------------------------------------------------
void Anp::EnablePrefetch::Set(const TrigMonConfig &config)
{
  //
  // Copy configuration and prepare algorithm, chain and sequence objects
  //
  fConfig   = config;
}

//-----------------------------------------------------------------------------
void Anp::EnablePrefetch::PrefetchImprovement(TrigMonEvent &Event, const std::string &name = "PrefetchImprovement"){

    vector<TrigMonSeq> &seqs = Event.getVec<TrigMonSeq>();
    TrigMonTimer *timer = new TrigMonTimer(0,0);

    //for(vector<TrigMonSeq>::iterator seq = seqs.begin(); seq != seqs.end(); ++seq) {
    //    const unsigned level = seq->getLevel();
    //    if(fLevel != 0 && level != fLevel) continue;
    //
    //    std::vector<TrigMonAlg>  &alg_entry = seq->getAlg();
    //    for(vector<TrigMonAlg>::iterator alg = alg_entry.begin(); alg != alg_entry.end(); ++alg){
    //      if(alg->stop().getSec() > timer->getSec() || (alg->stop().getSec() == timer->getSec() && alg->stop().getUSec() > timer->getUSec())   ){
    //        *timer = alg->stop();
    //      }
    //    }
    //}
    const unsigned index    = 5;
    const unsigned position = 0;
    const unsigned name_id  = 5; // need to hash this
    const TrigMonTimer *new_timer_start = new TrigMonTimer(timer->getSec(), timer->getUSec()+1);
    const TrigMonTimer *new_timer_stop = new TrigMonTimer(timer->getSec(), timer->getUSec()+5);

    const TrigConfAlg   *conf_alg = new TrigConfAlg(index,position,name,name,name_id,name_id); //(index,position,name,type_name,name_id, type_id)
    const TrigConfChain *conf_chn = new TrigConfChain(name,8000,2,"L2",name,0,2,1.0,0.0);
    TrigConfSeq         *conf_seq = new TrigConfSeq(name_id,index,name);
    TrigMonAlg          *mon_alg  = new TrigMonAlg(0,false);
    TrigMonSeq          *mon_seq  = new TrigMonSeq(*conf_chn, *conf_seq);

    // Filling the conf
    vector<TrigConfSeq>   &conf_seqs = fConfig.getVec<TrigConfSeq>();
    vector<TrigConfChain> &conf_chns = fConfig.getVec<TrigConfChain>();
    conf_seq->addAlg(*conf_alg);
    conf_seqs.push_back(*conf_seq);
    conf_chns.push_back(*conf_chn);

    // Filling mon_alg, mon_seq
    mon_alg->addTimer(*new_timer_start,*new_timer_stop);

    mon_seq->addAlg(*mon_alg);
    mon_seq->addState(TrigMonSeq::kInitial);
    mon_seq->addState(TrigMonSeq::kStart);

    seqs.insert(seqs.begin(),*mon_seq);

    vector<TrigMonROB> &mon_robs = Event.getVec<TrigMonROB>();
    TrigMonROB *NewROB = new TrigMonROB(2,mon_alg->start().getSec(),mon_alg->start().getUSec()+1,mon_alg->stop().getSec(),mon_alg->stop().getUSec()-1);

    for(MapPFetROS_ROB:: iterator it = fPreFetMap.begin(); it != fPreFetMap.end(); ++it){
        for(vector<TrigMonROBData>::iterator iter = it->second.begin(); iter != it->second.end(); ++iter){
            NewROB->addData(*iter);
        }
    }
    mon_robs.push_back(*NewROB);
}

//-----------------------------------------------------------------------------
void Anp::EnablePrefetch::Run(TrigMonEvent &event)
{
  //
  // Clear the prefetch map before the event
  //    -- this map holds the rob_ids that are prefetched
  //
  fPreFetMap.clear();

  //
  // Collect chain, sequence and algorithm timing entries
  //
  if(!fIsConfig) {
    log() << "Run - this algorithm is not configured" << endl;
    return;
  }

  //
  // If you want the old version of prefetching, then return and skip event
  //
  if(fType=="old"){
    if(fDebug) cout << "EnablePrefetch::Run - USing old prefetch...don't remove anything" << endl;
    return;
  }

  //
  // Filling the prefetch map
  //     -- fill map
  //     -- remove the prefetching chain
  //
  vector<TrigMonROB> &robs = event.getVec<TrigMonROB>();

  if(fDebug) cout << "EnablePrefetch::Run - number of robs - " << robs.size() << endl;

  for(vector<TrigMonROB>::iterator rob = robs.begin(); rob != robs.end(); ++rob){
    if(fDebug) cout << "Run - rob requestor id: " << rob->getRequestorId() << endl; 
    if(rob->getRequestorId() == 1498476240){
      if(fDebug) cout << "Run - found prefetch algorithm " << endl; 
      FillPFetMap(*rob);
      
      robs.erase(rob);
      --rob;
      continue;
    }
  }

  //
  // Turn the Prefetching off
  //
  if(fType=="no"){
    RemovePrefetching(event);
  }

  //
  // Generate the new prefetching procedure
  //
  if(fType=="new"){
    NewPrefetching(event);
  }
  PrefetchImprovement(event,"PrefetchImprovement");

}// end event

//-----------------------------------------------------------------------------
string Anp::EnablePrefetch::level() const
{  
  if     (fLevel == 0) return "AL";
  if     (fLevel == 2) return "L2";
  else if(fLevel == 3) return "EF";
  
  return "UN";
}

//-----------------------------------------------------------------------------
void Anp::EnablePrefetch::FillPFetMap(const TrigMonROB& pfetROB){

  //
  // Fill this map fPreFetMap
  //
  const vector<TrigMonROBData>& datas = pfetROB.getData();
  
  for(vector<TrigMonROBData>::const_iterator data = datas.begin(); data!=datas.end(); ++data){
  
    unsigned rob_id = data->getROBId();
    unsigned ros_id = RobService::Instance().GetRosId(rob_id);

    MapPFetROS_ROB::iterator ros = fPreFetMap.find(ros_id);

    if(ros == fPreFetMap.end()){
      vector<TrigMonROBData> rob_datas;
      
      ros = fPreFetMap.insert(MapPFetROS_ROB::value_type(ros_id, rob_datas)).first;
    }
	
	vector<TrigMonROBData> &robs = ros->second;
	
	bool found_rob=false;
	for(vector<TrigMonROBData>::iterator rob = robs.begin(); rob != robs.end(); ++rob){
		if(rob->getROBId()==rob_id) found_rob = true;
	}

	if(!found_rob) robs.push_back(*data);      
  }
}

//-----------------------------------------------------------------------------
const vector<TrigMonROBData> *Anp::EnablePrefetch::GetROBidsPFet(const unsigned ros_id) {

  //
  // Find a vector of ROB ids that are prefetched for the ROS
  //
  MapPFetROS_ROB::iterator ros_iter = fPreFetMap.find(ros_id);

  if(ros_iter != fPreFetMap.end()){
    
    return &(ros_iter->second);
  }

  vector<TrigMonROBData> *empty = new vector<TrigMonROBData>();
  
  return empty;
}

//-----------------------------------------------------------------------------
bool Anp::EnablePrefetch::CheckAndRemoveROBPFet(const unsigned rob_id) {

  //  
  // Find a vector of ROB ids that are prefetched for the ROS
  //  
  bool robFound = false;
  const vector<TrigMonROBData> *pFetRobVec = GetROBidsPFet(RobService::Instance().GetRosId(rob_id));

  for(unsigned i = 0; i < pFetRobVec->size(); ++i){
    if(pFetRobVec->at(i).getROBId() == rob_id){
      robFound=true;
      vector<TrigMonROBData> &pfetVec = fPreFetMap.find(RobService::Instance().GetRosId(rob_id))->second;
      pfetVec.erase(pfetVec.begin()+i);
      break;
    }
  }
  return robFound;
}

//-----------------------------------------------------------------------------
void Anp::EnablePrefetch::RemovePrefetching(TrigMonEvent &event){
  
   
  //
  // Fill algorithm objects for this sequence object
  //
  vector<TrigMonSeq> &seqs = event.getVec<TrigMonSeq>();
  
  for(vector<TrigMonSeq>::iterator seq = seqs.begin(); seq != seqs.end(); ++seq) {
    const unsigned level = seq->getLevel();

    if(fLevel != 0 && level != fLevel) continue;   
    
    std::vector<TrigMonAlg>  &alg_entry = seq->getAlg();    

    for(unsigned int j = 0; j < alg_entry.size(); ++j) {
      TrigMonAlg  &entry = alg_entry.at(j);
      
      const TrigMonTimer &alg_start = entry.start();
      const TrigMonTimer &alg_end   = entry.stop();

      if(fLevel==2){
	
    	vector<TrigMonROB> &robs = event.getVec<TrigMonROB>();
	
    	for(vector<TrigMonROB>::iterator rob = robs.begin(); rob != robs.end(); ++rob) {

     	  const TrigMonTimer &rob_start = rob->start();
    	  const TrigMonTimer &rob_end   = rob->stop();


    	  if(alg_start<rob_start && rob_end<alg_end){
     	    if(fDebug) cout << "EnablePrefetch::RemovePrefetching - found rob request for this algorithm call!!" << endl;
  
     	    //
	    // Looping over ROB_ids inside request
	    //
     	    for(unsigned int i = 0; i < rob->getData().size(); ++i) {

	      TrigMonROBData &robdata = rob->getData().at(i);
	      if(CheckAndRemoveROBPFet(robdata.getROBId())) robdata.setHistory(TrigMonROBData::kRETRIEVED);
	      

       	    }//loop over ROB data...these are ROB ids
	  }//end ask if rob belongs to this algorithm
    	}// end loop over ROB requests - contain multiple ROS and ROBs
      } // end if statement for L2 trigger rob work
    } // end loop of algorithms
  } //end loop over sequences
} // end function

//-----------------------------------------------------------------------------
void Anp::EnablePrefetch::NewPrefetching(TrigMonEvent &event){
  //
  // Fill algorithm objects for this sequence object
  //
  vector<TrigMonSeq> &seqs = event.getVec<TrigMonSeq>();
  int ii = 0;
  for(vector<TrigMonSeq>::iterator seq = seqs.begin(); seq != seqs.end(); ++seq) {

    const unsigned level = seq->getLevel();
    std::vector<TrigMonAlg>  &alg_entry = seq->getAlg();

    if(fLevel != 0 && level != fLevel) continue;

    for(unsigned int j = 0; j < alg_entry.size(); ++j) {

      TrigMonAlg         &entry     = alg_entry.at(j);
      const TrigMonTimer &alg_start = entry.start();
      const TrigMonTimer &alg_end   = entry.stop();
      
      if(fLevel==2){
	
	vector<TrigMonROB> &robs = event.getVec<TrigMonROB>();
        
	for(vector<TrigMonROB>::iterator rob = robs.begin(); rob != robs.end(); ++rob) {
 	  ii++;

	  const TrigMonTimer &rob_start = rob->start();
	  const TrigMonTimer &rob_end   = rob->stop();

	  if(alg_start<rob_start && rob_end<alg_end){

	    if(fDebug) cout << "EnablePrefetch::NewPrefetching - found rob request for this algorithm call!!" << endl;
	    
	    //
	    // looping over ROB_ids inside request
	    //
	    vector<TrigMonROBData> &datas = rob->getData();	

	    for(unsigned int i = 0; i < rob->getData().size(); ++i) {

	      TrigMonROBData &robdata = rob->getData().at(i);
	      vector<TrigMonROBData> pFetRobVec = *GetROBidsPFet(RobService::Instance().GetRosId(robdata.getROBId()));

	      if(fDebug) cout << "EnablePrefetch::NewPrefetching - ROB id=" << robdata.getROBId() << " size=" << robdata.getROBSize() << " requested from ros: " << RobService::Instance().GetRosId(robdata.getROBId()) << endl;

	      if( pFetRobVec.size() != 0){ 

		for(unsigned j = 0; j < pFetRobVec.size(); j++){
		  bool isInVec=false;
		  
		  for(unsigned k = 0; k < datas.size(); k++){
		    
		    if(pFetRobVec[j].getROBId() == datas[k].getROBId()) {
		      isInVec=true;
		      if(pFetRobVec[j].isRetrieved() && datas[k].isCached()){
			//
			// If prefetched robs is retrieved, then reset it for this algorithm
			//
			datas[k].setHistory(pFetRobVec[j].getHistory());
			pFetRobVec[j].setHistory(TrigMonROBData::kCACHED);
		      }
		      else pFetRobVec[j].setHistory(TrigMonROBData::kCACHED);
		      break;
		    }
		  }
		  if(!isInVec) datas.push_back(pFetRobVec[j]);
		}
		// done with this ROS, so delete it from the prefetched map
		MapPFetROS_ROB::iterator remrob = fPreFetMap.find(RobService::Instance().GetRosId(robdata.getROBId())); 
		if(remrob!=fPreFetMap.end()) fPreFetMap.erase(remrob);
	      }
	    }//loop over ROB data...these are ROB ids 
	  }//end ask if rob belongs to this algorithm
	}// end loop over ROB requests - contain multiple ROS and ROBs
      } // end if statement for L2 trigger rob work
    } // end loop of algorithms
  } //end loop over sequences
  if(fDebug) cout << endl << "PFetMap size at the end of event: " <<  fPreFetMap.size() << endl; 
} // end function
