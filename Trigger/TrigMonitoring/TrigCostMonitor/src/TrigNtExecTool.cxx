/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// Framework
#include "GaudiKernel/MsgStream.h"

// Trigger
#include "TrigInterfaces/AlgoConfig.h"
#include "TrigNavigation/Navigation.h"
#include "TrigNavigation/TriggerElement.h"
#include "TrigSteering/TrigSteer.h"
#include "TrigSteeringEvent/TrigRoiDescriptorCollection.h"
#include "TrigSteeringEvent/TrigOperationalInfo.h"
#include "TrigSteeringEvent/TrigOperationalInfoCollection.h"

// Local
#include "TrigCostMonitor/TrigNtExecTool.h"

using namespace std;

//---------------------------------------------------------------------------------------
Trig::TrigNtExecTool::TrigNtExecTool(const std::string &name,
				     const std::string &type,
				     const IInterface  *parent)
  :AlgTool(name, type, parent),
   m_parentAlg(0),
   m_log(0),
   m_storeGate("StoreGateSvc", name),
   m_timeAlg(0.0),
   m_timeSeq(0.0),
   m_config(0)
{
  declareInterface<Trig::ITrigNtTool>(this);
  declareInterface<Trig::TrigNtExecTool>(this);

  declareProperty("printOPI",     m_printOPI     = false);
  declareProperty("printSEQ",     m_printSEQ     = false);
  declareProperty("saveSeqTimer", m_saveSeqTimer = false);
}

//---------------------------------------------------------------------------------------
StatusCode Trig::TrigNtExecTool::initialize()
{
  // Get message service and print out properties
  m_log = new MsgStream(msgSvc(), name());
  
  log() << MSG::DEBUG << "initialize()" << endreq;

  if(m_storeGate.retrieve().isFailure()) {
    log() << MSG::ERROR << "Could not retrieve StoreGateSvc!" << endreq;
    return StatusCode::FAILURE;
  }

  return StatusCode::SUCCESS;
}

//---------------------------------------------------------------------------------------
StatusCode Trig::TrigNtExecTool::finalize()
{
  //
  // Clean up
  //  
  log() << MSG::DEBUG << "finalize()" << endreq;
  
  delete m_log; m_log = 0;
  m_config = 0;

  return StatusCode::SUCCESS;
}

//---------------------------------------------------------------------------------------
void Trig::TrigNtExecTool::SetSteer(const HLT::TrigSteer *ptr)
{
  if(!ptr) {
    log() << MSG::WARNING << "Null HLT::TrigSteer pointer" << endreq;
    return;
  }

  m_parentAlg = ptr;
  
  if(m_parentAlg->getAlgoConfig()->getHLTLevel() == HLT::L2) { 
    m_keyExten = "HLT_TrigOperationalInfoCollection_OPI_extended_L2";
    m_fallBackKey = "HLT_OPI_extended_L2"; // Provide some backward compatability
  }
  if(m_parentAlg->getAlgoConfig()->getHLTLevel() == HLT::EF) { 
    m_keyExten = "HLT_TrigOperationalInfoCollection_OPI_extended_EF";
    m_fallBackKey = "HLT_OPI_extended_EF"; // Provide some backward compatability
  }
  if(m_parentAlg->getAlgoConfig()->getHLTLevel() == HLT::HLT) { 
    m_keyExten = "HLT_TrigOperationalInfoCollection_OPI_extended_HLT";
    m_fallBackKey = "HLT_OPI_extended_HLT"; // Provide some backward compatability
  }

  // Was HLT_OPI_extended_EF etc.
  // Now
  //flags: (  valid, UNLOCKED,  reset) --- data: 0x41d9d6e0 --- key: HLT_TrigOperationalInfoCollection
  //flags: (  valid, UNLOCKED,  reset) --- data: 0x44aa5480 --- key: HLT_TrigOperationalInfoCollection_EXPRESS_OPI_HLT
  //flags: (  valid, UNLOCKED,  reset) --- data: 0x4534b4a0 --- key: HLT_TrigOperationalInfoCollection_OPI_HLT
  //flags: (  valid, UNLOCKED,  reset) --- data: 0x4343b920 --- key: HLT_TrigOperationalInfoCollection_OPI_extended_HLT
}

//---------------------------------------------------------------------------------------
bool Trig::TrigNtExecTool::Fill(TrigMonConfig *confg)
{
  //
  // Make local copy of new configuration
  //
  if(!confg) {
    log() << MSG::WARNING << "Null TrigMonConfig pointer" << endreq;
    return false;
  }

  if(outputLevel() <= MSG::DEBUG) {
    log() << MSG::DEBUG << "Filling the trig config into TrigNtExecTool NChains: " << confg->getVec<TrigConfChain>().size() << endreq;
  }

  // Copy data
  m_config = confg;
  
  // Clean, (re)fill and sort vectors
  m_TrigConfChain.clear();
  m_TrigConfSeq.clear();

  m_TrigConfChain.insert(m_TrigConfChain.end(),
			 m_config->begin<TrigConfChain>(), m_config->end<TrigConfChain>());
  
  m_TrigConfSeq.insert(m_TrigConfSeq.end(),
		       m_config->begin<TrigConfSeq>(), m_config->end<TrigConfSeq>());

  // Sort by name
  std::sort(m_TrigConfChain.begin(), m_TrigConfChain.end());
  std::sort(m_TrigConfSeq.begin(), m_TrigConfSeq.end());

  return true;
}

//---------------------------------------------------------------------------------------
bool Trig::TrigNtExecTool::Fill(TrigMonEvent &event)
{
  //
  // Process current event and collect extended operational info
  //

  m_timeAlg = 0.0; m_timeSeq = 0.0;
  unsigned ntoi = 0;

  if(m_storeGate->transientContains<TrigOperationalInfoCollection>(m_keyExten)) { // Check primary

    if(outputLevel() <= MSG::DEBUG) {
      log() << MSG::DEBUG << "Found TrigOperationalInfoCollection " << m_keyExten << endreq;
    }
    
    const TrigOperationalInfoCollection *toi_col = 0;
    if(m_storeGate->retrieve<TrigOperationalInfoCollection>(toi_col, m_keyExten).isSuccess() && toi_col) {
      ntoi = toi_col->size();
	  if(outputLevel() <= MSG::DEBUG) log() << MSG::DEBUG << "N Trig Op. Info " << ntoi << endreq;
      ReadOPI(event, *toi_col);
    }
  } else if(m_storeGate->transientContains<TrigOperationalInfoCollection>(m_fallBackKey)) { //Check fallback

    if(outputLevel() <= MSG::DEBUG) {
      log() << MSG::DEBUG << "Found TrigOperationalInfoCollection with FallBack key " << m_fallBackKey << endreq;
    }
    
    const TrigOperationalInfoCollection *toi_col = 0;
    if(m_storeGate->retrieve<TrigOperationalInfoCollection>(toi_col, m_fallBackKey).isSuccess() && toi_col) {
      ntoi = toi_col->size();
	  if(outputLevel() <= MSG::DEBUG) log() << MSG::DEBUG << "N Trig Op. Info BackupKey " << ntoi << endreq;
      ReadOPI(event, *toi_col);
    }

  } else {
    if(outputLevel() <= MSG::DEBUG) {
      log() << MSG::DEBUG << "Cannot Find TrigOperationalInfoCollection with key " << m_keyExten 
      << " or with key " << m_fallBackKey << endreq;
    }
  }
	  
  if(outputLevel() <= MSG::DEBUG) {
    log() << MSG::DEBUG 
	  << "Processed " << ntoi << " extended TrigOperationalInfo object(s)" << endreq
	  << "  T_alg = " << m_timeAlg << endreq
	  << "  T_seq = " << m_timeSeq << endreq;
  }

  return true;
}

//---------------------------------------------------------------------------------------
void Trig::TrigNtExecTool::ReadOPI(TrigMonEvent &event,
				   const TrigOperationalInfoCollection &ovec)
{
  //
  // Extract timing information for one CHAIN step (single TrigOperationalInfo object)
  //
  
  for(unsigned int itoi = 0; itoi < ovec.size(); ++itoi) {
    const TrigOperationalInfo *tinfo = ovec[itoi];
    if(!tinfo)
    {
      log() << MSG::WARNING << "Null TrigOperationalInfo pointer" << endreq;
      continue;
    }
    
    const std::pair<std::vector<std::string>, std::vector<float> > vp = tinfo -> infos();
    const std::vector<std::string> &svec = vp.first;
    const std::vector<float>       &fvec = vp.second;
    
    if(svec.size() != fvec.size())
    {
      log() << MSG::WARNING << "Mismatched vector sizes" << endreq;
      continue;
    }
    
    if(m_printOPI)
      log() << MSG::INFO << "Printing TOI with " << svec.size() << " entries" << endreq;
    
    //
    // define local variables for current TrigOperationalInfo object
    //
    vector<TrigConfChain>::const_iterator chainIter = m_TrigConfChain.end();
    vector<TrigConfSeq>::const_iterator myseqIter = m_TrigConfSeq.end();
    std::string curAlgName;
    unsigned int curAlgPos = 0;

    std::vector<ReadEntry> read_vec;
    
    for(unsigned int ikey = 0; ikey < svec.size(); ++ikey) {  
      //
      // key is parsed using ':' token: TYPE:NAME: and then other type specific items
      //
      const std::string &key = svec[ikey];
      const        float val = fvec[ikey];  
  
      if(m_printOPI)
        log() << MSG::INFO << ikey << ": TOI[" << key << "]=" << val << endreq;
            
      //
      // New entry, first extract entry type
      //  
      const int ksize = key.size();
      int isep1 = -1, isep2 = ksize;

      ReadEntry entry;
      if     (key.compare(0, 3, "ALG") == 0) { entry.m_opiState=kAlg; isep1=5; }
      else if(key.compare(0, 3, "CHA") == 0) { entry.m_opiState=kChn; isep1=6; }
      else if(key.compare(0, 3, "SEQ") == 0) { entry.m_opiState=kSeq; isep1=9; }
      else continue;
      
      if(isep1+1 >= ksize) {
        log() << MSG::WARNING << "Bad type format for: " << key << endreq;
        return;
      }

      for(int i = isep1; i < ksize; ++i) {
        // Ignore character not equal to ':'
        if(key[i] != ':') continue;
	
        // Also ignore two '::' characters in row
        if(i+1 < ksize && key[i+1] == ':') continue;
        if(i-1 > 0     && key[i-1] == ':') continue;

        isep2 = i;
        break;
      }

      const std::string myname(key, isep1, isep2-isep1);
      entry.m_seqIter = myseqIter;
      entry.m_value   = val;
      //entry.m_key     = key;

      if(entry.m_opiState == kChn) {
        //
        // Find chain configuration
        //
        if(chainIter != m_TrigConfChain.end()) {
          log() << MSG::WARNING << "More than 1 chain entry for key: " << key << endreq;
        }

        chainIter = std::lower_bound(m_TrigConfChain.begin(), m_TrigConfChain.end(), myname);
        if(chainIter == m_TrigConfChain.end() || chainIter->getName() != myname) {
          log() << MSG::WARNING << "No TrigConfChain for: " << key << "/" << myname << endreq;
          log() << MSG::WARNING << "Options Are:" << endreq;
          for (vector<TrigConfChain>::const_iterator _it = m_TrigConfChain.begin(); _it != m_TrigConfChain.end(); ++_it) {
            log() << MSG::WARNING << " --- " << _it->getName() << endreq;
          }
          return;
        }
        
        continue;
      }
      
      //
      // Parse remainder of string
      //
      std::vector<std::string> parsed_key;
      ParseKey(parsed_key, key, isep2);

      if(entry.m_opiState == kSeq) {
        //
        // Find sequence configuration
        //
        myseqIter = std::lower_bound(m_TrigConfSeq.begin(), m_TrigConfSeq.end(), myname);

        if(myseqIter == m_TrigConfSeq.end() || myseqIter->getName() != myname) {
          log() << MSG::WARNING << "No TrigConfSeq for: " << key << "/" << myname << endreq;
          continue;
        }

        entry.m_seqIter = myseqIter;

        // Determine sequence state
        if(parsed_key.empty()) {
          entry.m_seqState = kFirst;
        }
        else if(parsed_key.size() == 1) {
          if     (parsed_key[0] == "Start")       entry.m_seqState = kStart;
          else if(parsed_key[0] == "ElapsedTime") entry.m_seqState = kElapsedTime;
          else if(parsed_key[0] == "Previous")    entry.m_seqState = kPrevious;
          else {
            log() << MSG::WARNING << "Bad SEQ state: " << key << "/" << parsed_key[0] << endreq;
            continue;
          }
	      }
        else {
          log() << MSG::WARNING << "Bad sequence string: " << key << endreq;
          continue;
        }
      }
      else if(entry.m_opiState == kAlg) {
        //
        // Fill algorithm data
        //
	
        if(curAlgName != myname) {
          // New algorithm
          curAlgName.clear();
          vector<TrigConfAlg>::const_iterator algIter = myseqIter -> findName(myname);
          if(algIter == myseqIter -> algEnd()) {
            log() << MSG::WARNING << "Bad alg iterator " << endreq;
            return;
          }
    
          curAlgName = myname;
          curAlgPos = algIter -> getPosition();
        }

        entry.m_palg = curAlgPos;

        if(parsed_key.size() == 1) {
          // Process entry for number of algorithm calls
          if(parsed_key[0] == "Call") { 
            entry.m_algState = kCall;
            entry.m_call = static_cast<int>(val);
          }
          else {
            log() << MSG::WARNING << "Bad algorithm call: " << key << endreq;
            continue;
          }
        }
        else if(parsed_key.size() == 2) {
          // Process single algorithm call
          entry.m_call = std::atoi(parsed_key[0].c_str());

          if     (parsed_key[1] == "TotalTime")  entry.m_algState = kTotalTime;
          else if(parsed_key[1] == "Cached")     entry.m_algState = kCached;
          else if(parsed_key[1] == "StartSec")   entry.m_algState = kStartSec;
          else if(parsed_key[1] == "StartUSec")  entry.m_algState = kStartUSec;
          else if(parsed_key[1] == "StopSec")    entry.m_algState = kStopSec;
          else if(parsed_key[1] == "StopUSec")   entry.m_algState = kStopUSec;
          else if(parsed_key[1] == "NOutputTEs") continue;
          else {
            log() << MSG::WARNING << "Bad algorithm state: " << key << endreq;
            continue;
          }
        }
        else if(parsed_key.size() == 3) {
          // Process RoiId
          entry.m_call = std::atoi(parsed_key[0].c_str());

          if     (parsed_key[1] == "RoiId")   entry.m_algState = kRoiId;
          else if(parsed_key[1] == "RoiWord") continue;
          else {
            log() << MSG::WARNING << "Bad RoiId string: " << key << endreq;
            continue;
          }
        }
        else {
          log() << MSG::WARNING << "Bad algorithm string: " << key << endreq;
          continue;
        }
      }

      //
      // Fill data for one chain step
      //
      read_vec.push_back(entry);
    }
    
    if(chainIter != m_TrigConfChain.end()) {
      ReadSeq(event, *chainIter, read_vec, 0);
    }
    else {
      log() << MSG::WARNING << "Missing configiration for chain step" << endreq;
    }
  }
}

//---------------------------------------------------------------------------------------
void Trig::TrigNtExecTool::ReadSeq(TrigMonEvent &event,
				   const TrigConfChain &chain,
				   std::vector<ReadEntry> &read_vec,
				   int icurr)
{
  //
  // Process OPI entries from a single chain step
  //
  const int rsize = read_vec.size(); 
  if(!(icurr < rsize)) {
    if(outputLevel() <= MSG::DEBUG && m_printOPI)
       log() << MSG::DEBUG << "Reached end of recursion" << endreq;
    return;
  }

  // Current entru
  const ReadEntry &curr = read_vec[icurr];

  if(curr.m_opiState != kSeq) {
    log() << MSG::WARNING << "ReadSeq must start with kSeq: " << curr.m_key << endreq;
    return;
  }

  // Sequence did not execute any algorithms
  if(!(icurr+1 < rsize)) {
    TrigMonSeq myseq(chain, *curr.m_seqIter);
    myseq.addState(TrigMonSeq::kInitial);
    SaveSeq(event, myseq);
    return;
  }

  // Walk through vector and select one sequence
  const ReadEntry &next = read_vec[icurr+1];

  // New sequence entry
  TrigMonSeq myseq(chain, *curr.m_seqIter);

  // Check next entry
  if(curr.m_seqState == kFirst) {
    
    if(curr.m_value > 0.5) {
      // Add already executed bit 
      myseq.addState(TrigMonSeq::kAlreadyExecuted);

      // Save, make recursive call and return
      SaveSeq(event, myseq);
      ReadSeq(event, chain, read_vec, icurr+1);
      return;
    }

    if(next.m_seqState == kStart) {
      if(next.m_seqIter == curr.m_seqIter) {
        // Same sequence, advance position forward to kStart
        ++icurr;
        myseq.addState(TrigMonSeq::kInitial);
        myseq.addState(TrigMonSeq::kStart);
      }
      else {
        log() << MSG::WARNING << "Logic error for next entry" << endreq;
        return;
      }
    }
    else {
      // This sequence forced execution of previous sequence(s)
      myseq.addState(TrigMonSeq::kInitial);

      // Save, make recursive call and return
      SaveSeq(event, myseq);
      ReadSeq(event, chain, read_vec, icurr+1);
      return;
    }
  }
  else if(curr.m_seqState == kPrevious) {
    // Recursive sequence execution - advance forward
    ReadSeq(event, chain, read_vec, icurr+1);
    return;    
  }
  else if(curr.m_seqState == kStart) {
    // Debugging check: go back and find kFirst entry
    bool found_first = false;
    for(int iprev = icurr-1; iprev >= 0; --iprev) {
      const ReadEntry &prev = read_vec[iprev];

      if(prev.m_seqState == kFirst && prev.m_seqIter == curr.m_seqIter) {
        found_first = true;
        break;
      }
    }

    // Set state
    myseq.addState(TrigMonSeq::kStart);
    
    // This should be sequence which forced recursive previous call(s)
    if(!found_first) {
      log() << MSG::WARNING << "Logic error for recursive sequence" << endreq;
      return;
    }
  }

  // Check if this sequence was called recursively
  for(int iprev = icurr-1; iprev >= 0; --iprev) {
    const ReadEntry &prev = read_vec[iprev];

    if(prev.m_seqState == kPrevious && prev.m_seqIter == curr.m_seqIter) {
      myseq.addState(TrigMonSeq::kPrevious);
      break;
    }
  }

  // Loop through algorithm entries
  int istop = icurr+1;
  
  // Find last sequence entry
  for(; istop < rsize; ++istop) {
    const ReadEntry &stop = read_vec[istop];
    
    if(stop.m_seqState == kElapsedTime) {
      if(stop.m_seqIter != curr.m_seqIter) {
        log() << MSG::WARNING << "stop and curr have different TrigConfSeq iterators" << endreq;
      }

      if(m_saveSeqTimer) myseq.addTimer(stop.m_value);
      break;
    }
    
    if(stop.m_opiState != kAlg) {
      log() << MSG::WARNING << "Logic error for kAlg at key: " << stop.m_key << endreq;
      return;
    }
  }

  // Read all algorithms from single sequence
  ReadAlg(myseq, read_vec, icurr+1, istop);
  SaveSeq(event, myseq);

  // Recursive call
  ReadSeq(event, chain, read_vec, istop+1);
}

//---------------------------------------------------------------------------------------
void Trig::TrigNtExecTool::SaveSeq(TrigMonEvent &event, const TrigMonSeq &myseq)
{
  //
  // Save sequence with event
  //
  event.add<TrigMonSeq>(myseq);

  m_timeAlg += myseq.getAlgTimer();
  m_timeSeq += myseq.getSeqTimer();

  if(m_printSEQ) {
    vector<TrigConfSeq>::const_iterator iseq = std::find(m_TrigConfSeq.begin(),
							 m_TrigConfSeq.end(), myseq);
    if(iseq != m_TrigConfSeq.end()) {
      myseq.print(*iseq, std::cout);
    }
    else {	
      log() << MSG::WARNING << "No TrigConfSeq for SEQ at index=" << myseq.getSeqIndex() << endreq;
    }
  }
}

//---------------------------------------------------------------------------------------
void Trig::TrigNtExecTool::ReadAlg(TrigMonSeq &myseq,
				   std::vector<ReadEntry> &read_vec,
				   const int icurr, const int istop)
{
  //
  // Collect algorithm entries
  //
  const int rsize = read_vec.size();
  if(!(istop < rsize) || !(icurr < istop)) {
    if(m_printOPI)
      log() << MSG::DEBUG << "No algorithms in range: " << icurr << ", " << istop << endreq;
    return;
  }
  if(istop < 0 || icurr < 0) {
    log() << MSG::WARNING << "Negative index" << endreq;
    return;
  }

  // Indexes for all algorithms in this interval
  std::map<int, int> alg_map;

  for(int i = icurr; i < istop; ++i) {
    const ReadEntry &entry = read_vec[i];

    if(entry.m_opiState != kAlg) {
      log() << MSG::WARNING << "Range contains non kAlg entry: " << entry.m_key << endreq;
      return;
    }

    if(entry.m_palg < 0) {
      log() << MSG::WARNING << "Negative position" << endreq;
      return;
    }
    if(entry.m_algState == kCall && entry.m_call < 0) {
      log() << MSG::WARNING << "Bad alg state: " << entry.m_key << ":" << entry.m_value << endreq;
      return;
    }

    if(entry.m_algState == kCall) {
      alg_map[entry.m_palg] = entry.m_call;
    }
  }

  // Run over algorithms 
  for(map<int, int>::iterator it = alg_map.begin(); it != alg_map.end(); ++it) {
    for(int i = 0; i <= it->second; ++i) {
      myseq.addAlg(MakeAlg(static_cast<unsigned int>(it->first), read_vec, icurr, istop, i));
    }
  }
}

//---------------------------------------------------------------------------------------
TrigMonAlg Trig::TrigNtExecTool::MakeAlg(unsigned int alg_position,
					 std::vector<ReadEntry> &read_vec,
					 const int icurr, const int istop, const int icall)
{
  //
  // Collect timing information for single algorithm call
  //

  //
  // Declare local variables
  //
  float start_sec = 0.0, start_usec = 0.0;
  float stop_sec  = 0.0, stop_usec  = 0.0;
  float total_time = 0.0;
  bool cached = false;
  unsigned int icount_times = 0;

  vector<unsigned int> roi_vec;

  for(int i = icurr; i < istop; ++i) {
    const ReadEntry &entry = read_vec[i];
    if(entry.m_palg != int(alg_position)) {
      continue;
    }
    if(entry.m_call != icall) {
      continue;
    }
    if(entry.m_opiState != kAlg) {
      continue;
    }

    if(entry.m_algState == kTotalTime) { total_time = entry.m_value; }
    if(entry.m_algState == kCached)    { cached     = true; }
    if(entry.m_algState == kStartSec)  { start_sec  = entry.m_value; icount_times++; }
    if(entry.m_algState == kStartUSec) { start_usec = entry.m_value; icount_times++; }
    if(entry.m_algState == kStopSec)   { stop_sec   = entry.m_value; icount_times++; }
    if(entry.m_algState == kStopUSec)  { stop_usec  = entry.m_value; icount_times++; }

    // There can be many RoIs
    if(entry.m_algState == kRoiId) { 
      roi_vec.push_back(static_cast<unsigned int>(entry.m_value));
    }
  }

  // Make algorithm entry
  TrigMonAlg myalg(alg_position, cached);

  // Add RoiIds
  for(unsigned int i = 0; i < roi_vec.size(); ++i) {
    myalg.addRoiId(roi_vec[i]);
  }

  // Set start and stop times
  if(!cached) {        
    myalg.addTimer(TrigMonTimer(static_cast<int>(start_sec), static_cast<int>(start_usec)),
		   TrigMonTimer(static_cast<int>(stop_sec),  static_cast<int>(stop_usec)));
    
    if(icount_times != 4) {
      if(outputLevel() <= MSG::DEBUG && m_printOPI)
	log() << MSG::DEBUG << "Only found " << icount_times << " time entries" << endreq;
    }
  }
  
  // Sanity check
  if(cached && total_time > 0.0) {
    log() << MSG::WARNING << "Algorithm was cached but timer is positive" << endreq;
  }

  return myalg;
}

//-----------------------------------------------------------------------------
void Trig::TrigNtExecTool::ParseKey(std::vector<std::string> &svec,
				    const std::string& key, int ibeg)
{
  // 
  // Split a long string using ':' into a set of shorter strings
  //
  const int keysz = key.size();
  int ipos = ibeg;

  if(ibeg >= keysz) return;

  for(int i = ibeg; i < keysz; ++i) {
    // Ignore character not equal to ':'
    if(key[i] != ':') continue;
    
    // But also ignore two '::' characters 
    if(i+1 < keysz && key[i+1] == ':') continue;
    if(i-1 >     0 && key[i-1] == ':') continue;

    // Add new substring
    const int nstr = i-ipos;
    if(nstr > 0 && ipos < keysz) { 
      svec.push_back(key.substr(ipos, nstr));
    }
    
    ipos = i+1;
  }

  // Add last substring
  if(ipos < keysz) {
    svec.push_back(key.substr(ipos));
  }
}
