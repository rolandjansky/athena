/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// C/C++
#include <cmath>
#include <iostream>
#include <sstream>
#include <fstream>
// Local
#include "TrigCostBase/Factory.h"

// Locla
#include "TrigCostBase/UtilTrig.h"
#include "TrigCostRate/SimlComb.h"

REGISTER_ANP_OBJECT(AlgEvent,SimlComb)

using namespace std;
using namespace Trig;

//-----------------------------------------------------------------------------
// Code for helper class
//

//-----------------------------------------------------------------------------
Anp::CombTrig::CombTrig()
  :fLevel(0),
   fIEvent(0),
   fPass(false)
{
}

//-----------------------------------------------------------------------------
void Anp::CombTrig::Clear()
{
  //
  // Clear all data
  //
  fLevel = 0;
  fIEvent = 0;
  fPass = 0;

  fConfgVec.clear();
  fLower.release();
}

//-----------------------------------------------------------------------------
bool Anp::CombTrig::Config(const std::vector<TrigConfChain> &ivec)
{
  //
  // Input chains for combined trigger
  //
  if(ivec.empty()) {
    cerr << "CombTrig::Config - empty input vector" << endl;
    return false;
  }

  for(unsigned int i = 0; i < ivec.size(); ++i) {
    // Copy chain and reset PT and PS
    TrigConfChain chain = ivec[i];
    chain.setPrescale(1);
    chain.setPassThrough(0);
	
    // Save chain
    fConfgVec.push_back(chain);

    // Check and save trigger level
    
    fLevel = chain.getLevelId();
    if(fLevel != chain.getLevelId()) {
		cout << "Warning: Logic error for level" << endl;
        cerr << "CombTrig::Config - logic error for level" << endl;
        Clear();
        return false;
    }
  }
  return true;
}

//-----------------------------------------------------------------------------
bool Anp::CombTrig::SetTEs(const std::vector<TrigConfChain> &cvec){


  resetTEcount();
  resetChcount();

  for(unsigned i= 0; i < cvec.size(); ++i){
    std::set<unsigned> tevec;
    unsigned countDouble = 0;
    cout << "SetTEs(...): " << endl;

    TrigConfChain conf = cvec[i];
    cout << "starting setconfg on: " << conf.getName() << ", number of Signatures: "  << conf.getSignature().size() << endl;
    for(unsigned j = 0; j < conf.getSignature().size(); ++j){
      cout << "added: " << conf.getSignature()[j].getLabel() << " with " << conf.getSignature()[j].getOutputTEs().size() << " TEs: " << endl;
       
      for(unsigned k = 0; k <  conf.getSignature()[j].getOutputTEs().size(); ++k){
        if(conf.getSignature()[j].getOutputTEs().size() > 1 && fLevel != 1) {
          cout << "Warning: Combined input chains are not allowed! " << endl;
//          return true;	
        }
        cout << conf.getSignature()[j].getOutputTEs()[k] << endl;
        if(!tevec.insert(conf.getSignature()[j].getOutputTEs()[k]).second) countDouble++;
      }
    }
    cout <<  "tevec size: " << tevec.size() << endl;
    std::set<unsigned>::iterator it = tevec.find(3205490841U);
    if(it != tevec.end()) tevec.erase(it);
    it = tevec.find(1342371167U);
    if(it != tevec.end()) tevec.erase(it);
    if(fLevel == 1 && tevec.size()+countDouble > 1) {
      cout << "Warning: Combined input chains are not allowed! " << endl;
      cerr << " Combined input chains are not allowed! " << endl;
//      return false;
    }
    std::vector<unsigned> vec(tevec.begin(),tevec.end()); //cast to vector;
    SimulChain dummy(conf.getCounter(),vec);
    dummy.xeteChain = false;
    if(conf.getName().find("xe") !=  string::npos || conf.getName().find("te") !=  string::npos){
      dummy.xeteChain = true;
      std::cout << " xeteChain: " << dummy.xeteChain << std::endl;
    }
    fRequiredTEs.push_back(dummy);

    if(conf.getLevelId() != fLevel) {
      cerr << "CombTrig::SetConfg - level mismatch" << endl;
	  cout << "Warning: CombTrig::SetConfg - level mismatch" << endl;

      return false;
    }
  }
  return true;

}
//-----------------------------------------------------------------------------

bool Anp::CombTrig::SetConfg(const TrigConfChain &conf)
{
  //
  // Copy trigger configuration
  //

  
  fConfg = conf;
  return true;
}

//-----------------------------------------------------------------------------
bool Anp::CombTrig::SetLower(Anp::Handle<Anp::CombTrig> trig)
{
  //
  // Add handle for previous trigger chain
  //

  if(!trig.valid()) {
    cerr << "CombTrig::AddLower - invalid handle" << endl;
    return false;
  }
  
  if(trig->GetLevel()+1 != fLevel) {
    cerr << "CombTrig::AddLower - level mismatch" << endl;
    return false;
  }

  fLower = trig;
  return false;
}

//-----------------------------------------------------------------------------
bool Anp::CombTrig::Pass(const TrigMonEvent& event, unsigned int ievent,
			 TrigMonL1Item::Decision     decisionBitLV1, // Decision bit used for LV1 rates
			 TrigMonChain::Decision      decisionBitHLT  // Decision bit used for HLT rates
			 )
{

  //
  // Check if combined trigger has passed event
  //
  stringstream ss (stringstream::in | stringstream::out);
  bool EventPassedTE = true;
  std::vector<SimulChain> FoundTEChains; //Are the required TEs indeed found?
  bool fDebug = false;
  // prepare for new event
  fPass = false;
  fIEvent = ievent;
  
  // Check previous trigger
  if(fLower.valid() && !(fLower->Pass(event, fIEvent, decisionBitLV1, decisionBitHLT))) { 
    return false;
  }
  ss << "SimlComb::Pass - checking "<< GetConfg().getName() << " in event: " << fIEvent << endl;
  ss << "Number of configured chains: " << fConfgVec.size() << endl;

  // get lists of L1 and HLT chains that passed
  const std::vector<uint16_t> &event_lv1 = event.getL1Item();
  const std::vector<uint32_t> &event_hlt = event.getChain();
  const std::vector<TrigMonTE> &event_TE = event.getVec<TrigMonTE>();

  for(unsigned i = 0; i < event_hlt.size(); ++i){
	  TrigMonChain monchain(event_hlt[i]);
	  if(monchain.getCounter() == 234 && monchain.isPassed()) cout << "original combined l2_e12Tvh_loose1 trigger passed! In event: " << fIEvent << endl;
	  if(monchain.getCounter() == 719 && monchain.isPassed()) cout << "original combined l2_2e12Tvh_loose1 trigger passed! In event: " << fIEvent << endl;
	  if(monchain.getCounter() == 718 && monchain.isPassed()) cout << "original combined l2_tau29T_medium trigger passed! In event: " << fIEvent << endl;
      if(monchain.getCounter() == 357 && monchain.isPassed()) cout << "original combined EF_j240_a10tcem passed! In event: " << fIEvent << endl;

	  fDebug = true;
  }

  if(fDebug){
    ss<< "Pass - processing new chain" << endl
  	  << "   LV1 triggers: " << event_lv1.size() << endl
  	  << "   HLT triggers: " << event_hlt.size() << endl
	  << "   number of TEs: "<< event_TE.size() << std::endl;
  }


// Initialize the Found TEs: 
  FoundTEChains = fRequiredTEs;
  for(unsigned i = 0; i <fRequiredTEs.size(); ++i){
    ss << "checking xeteChain Req: " << fRequiredTEs[i].xeteChain << endl;
    ss << "checking xeteChain found: " << FoundTEChains[i].xeteChain << endl;
  }
  ss << "fRequiredTEs.size() << " << FoundTEChains.size() << endl;
  ss << "fRequiredTEs[0].TE_ids[0] << " << FoundTEChains[0].TE_ids[0] << endl;

  //
  // Find TEs for the requested signatures and store their roi if found;
  // The FoundChains are copied from the requiredChains. Now loop over those
  // FoundChains and see if all required TEs are found for the matching roi.  
  // If a second TE of the same ID is found but with another roi, check the other 
  // FoundChains and make another instance if there is no chain known that  
  // also matches the TE-id and add it to the vector of FoundTEChains with the new roi.
        
  bool foundSecond = false;
  for(unsigned i = 0; i < FoundTEChains.size(); ++i){
//    bool foundSecond = false;
    for(unsigned j = 0; j < event_TE.size(); ++j) {
	  foundSecond = false;
      int pos = FoundTEChains[i].isRequired(event_TE[j].getId());
      if(pos == 999) continue;
      if(fDebug) ss << "TE matched to required: " <<  event_TE[j].getId() << " Roi Id: " << (int) event_TE[j].getRoiId()[0] <<  endl;
      if(FoundTEChains[i].compareRoi(event_TE[j].getRoiId())) {
		 FoundTEChains[i].TE_found[pos] = true;
		 if(fDebug) ss << " RoI " << (int) event_TE[j].getRoiId()[0] << " matched and TE_found " << event_TE[j].getId() << " set to true in FoundTEChains: " << i << endl;
      }
      else{
		if(fDebug) ss << "checking other chains: " << endl;
        for(unsigned k = 0; k < FoundTEChains.size(); ++k){
	      if(k==i) continue;
          int pos2 = FoundTEChains[k].isRequired(event_TE[j].getId());
		  if(pos2 == 999) continue;
		  if(FoundTEChains[k].compareRoi(event_TE[j].getRoiId())) {
			FoundTEChains[k].TE_found[pos2] = true;
			foundSecond = true;
			if(fDebug) ss << " RoI " << (int) event_TE[j].getRoiId()[0] << " matched and TE_found " << event_TE[j].getId() << " set to true in another " << k << endl;
          }
        }
		if(fDebug) ss << " found second: " << foundSecond << endl;
        if(foundSecond) continue;
        else{
          vector<unsigned char> roivec = event_TE[j].getRoiId();
          SimulChain Dummy(FoundTEChains[i].ChainId,FoundTEChains[i].TE_ids,roivec);
		  int pos2 = Dummy.isRequired(event_TE[j].getId());
		  if(pos2 == 999) cout << "ERROR in SimulChain::Pass: new Simulchain is made, but TE is not required" << endl;
		  Dummy.TE_found[pos2] = true;
          Dummy.xeteChain = false;
          FoundTEChains.push_back(Dummy);
		  if(fDebug) ss << " New Chain made with RoI:  " << (int) event_TE[j].getRoiId()[0] << " and TE: " <<  event_TE[j].getId() << " set to true " << endl;
    	  foundSecond = false;
        }
      }
    }
  }

  std::map<unsigned,unsigned> ChainMultiplicity;
  std::map<unsigned,std::list<std::vector<unsigned char> > > roiToChainMap;

  for(unsigned j = 0; j < fRequiredTEs.size(); ++j){
    std::map<unsigned,unsigned>::iterator it = ChainMultiplicity.find(fRequiredTEs[j].ChainId);
    if(it == ChainMultiplicity.end()) ChainMultiplicity.insert(std::pair<unsigned,unsigned>(fRequiredTEs[j].ChainId,1));
    else it->second++;
    bool reqPassed = false;
    for(unsigned i = 0; i < FoundTEChains.size(); ++i){
      // 
      // Check whether this could be a xe/te chain and should be compared to the TrigMonChain in the Event. 
      //

      bool pass = false;
      if(!FoundTEChains[i].isPassed()){
        ss << " is it a xetechain?: "<< FoundTEChains[i].xeteChain << endl;
        if(!FoundTEChains[i].xeteChain) continue;
        for(unsigned k = 0; k < event_hlt.size(); ++k) {
          const TrigMonChain chain(event_hlt.at(k));
          if(chain.isPassed(decisionBitHLT)&& chain.getLevel() == fLevel) {
            if(chain.getCounter() == FoundTEChains[i].ChainId) pass = true;
          }
        }
      }
      else pass = true;
      if(!pass) continue;
      if(fRequiredTEs[j].ChainId == FoundTEChains[i].ChainId) reqPassed = true;
      else continue;
      for(unsigned k = 0; k < FoundTEChains[i].roi_id.size(); ++k){
        if(fDebug) ss << "Chain " << fRequiredTEs[j].ChainId << " passed with " << "roi nr: "<< k << " id: " << (int) FoundTEChains[i].roi_id[k] << endl;
      }

      std::map<unsigned,std::list<std::vector<unsigned char> > >::iterator iter = roiToChainMap.find(fRequiredTEs[j].ChainId);
      if(iter == roiToChainMap.end()){
	std::list<std::vector<unsigned char> > passedRoi;
        passedRoi.push_back(FoundTEChains[i].roi_id);
        roiToChainMap.insert(pair<unsigned,std::list<std::vector<unsigned char> > >(fRequiredTEs[j].ChainId,passedRoi) );
      }
      else iter->second.push_back(FoundTEChains[i].roi_id);
      
    }
    if(!reqPassed){
      EventPassedTE = false;
      if(fDebug) ss << "Chain " << fRequiredTEs[j].ChainId << " did not pass " << endl;
    }
  }
  if(EventPassedTE){
    std::list<unsigned char> buflist;
    for(std::map<unsigned,std::list<std::vector<unsigned char> > >::iterator it = roiToChainMap.begin(); it != roiToChainMap.end(); ++it){
      if(fDebug) ss << "before unique: " << it->second.size() << endl;
      for(std::list<std::vector<unsigned char> >::iterator iter = it->second.begin(); iter != it->second.end(); ++iter){
	if(fDebug) ss << "vec size: " << iter->size() << endl;
        for(unsigned j = 0; j < iter->size(); ++j){
          buflist.push_back(iter->at(j));
          if(fDebug) ss << "roi id: " << (int) iter->at(j) << " at " << j << " ";
	}
      }
      ss << endl;
      buflist.sort();
      buflist.unique();
      if(fDebug) ss << "after unique: " << buflist.size() << endl;

      std::map<unsigned,unsigned>::iterator iter = ChainMultiplicity.find(it->first);
      if(iter == ChainMultiplicity.end()){
        cerr << " Chain id not found in map! " << endl;
        EventPassedTE = false;
      }
      if(iter->second > buflist.size()){
        if(fDebug) ss << "Not enough unique Chains found of ID: " << iter->first << endl;
        EventPassedTE = false;
      }
    }
  }




//
//  Investigate whether all necessary TEs were found by checking whether the roi_ids_total vector was completely filled
//

// Number of passed and configured chains matches

  if(EventPassedTE){ 
    fTEcount++;
    if(fDebug) ss << "CombTrig: " << GetConfg().getName() << " Passed acc. to TEs: " << EventPassedTE << std::endl;
    string outstr = ss.str();
    if(fLevel > 1 && fDebug) cout << outstr;
    fPass = true;
  } 


  return fPass;
}

//-----------------------------------------------------------------------------
const Anp::CombTrig& Anp::CombTrig::GetLower() const
{
  if(fLevel == 1) {
    cerr << "CombTrig::GetLower - L1 chain has no lower chain" << endl;
    return *this;
  }
  if(!fLower.valid()) {
    cerr << "CombTrig::GetLower - lower chain is invalid" << endl;
    return *this;
  }
  
  return *fLower;
}

//-----------------------------------------------------------------------------
Anp::SimlComb::SimlComb()
  :fDebug(false),
   fIsConfig(false),
   fIEvent(0),
   fDecisionBitLV1(TrigMonL1Item::kL1AfterVeto),
   fDecisionBitHLT(TrigMonChain::kPassedRaw)
{
  for (int l = 0;l<4;l++) {
    fCounter[l]=0;
  }
}

//-----------------------------------------------------------------------------
Anp::SimlComb::~SimlComb()
{
}

//-----------------------------------------------------------------------------
void Anp::SimlComb::Config(const Registry &reg)
{
  //
  // Configure self
  //
  fReg = reg;
  fReg.Get("SimlComb", "Debug",   fDebug);
  //
  // Bits
  //
  std::string decisionKeyLV1; // Decision key used for LV1 rates
  std::string decisionKeyHLT; // Decision key used for HLT rates
  if(reg.Get("SimlComb", "DecisionKeyLV1", decisionKeyLV1)) {
    fDecisionBitLV1 = Anp::AsTriggerBitLV1(decisionKeyLV1);
  }
  if(reg.Get("SimlComb", "DecisionKeyHLT", decisionKeyHLT)) {
    fDecisionBitHLT = Anp::AsTriggerBitHLT(decisionKeyHLT);
  }

  log() << "Config" << endl
	<< "   LV1 decision bit: " << Anp::AsString(fDecisionBitLV1) << endl
	<< "   HLT decision bit: " << Anp::AsString(fDecisionBitHLT) << endl; 

  //
  // Initiliaze data tree
  //
  fEntry = NtupleSvc::Instance("event").Branch<TrigMonEvent>("data");

  //
  // Add self to global service
  //
  Anp::Factory<Anp::SimlABC>::Instance().Hold(GetName(), Handle<Anp::SimlABC>(this));

  //
  // Register with trigger configuration algorithm
  //
  Anp::RegisterCallback(reg, "SimlComb::ConfAlg", fConf, this);
}

//-----------------------------------------------------------------------------
void Anp::SimlComb::Signal(const Registry &, const string &signal)
{
  //
  // Handle callback or parent signal
  //

  if(signal != "TRIG_CONFIG") return;

  if(fConf.valid()) { 
    Set(fConf->GetConfig()); 
 }
  else {
    log() << "Signal - invalid configuration algorithm" << endl;
  }
}

//-----------------------------------------------------------------------------
bool Anp::SimlComb::Init()
{
  return true;
}

//-----------------------------------------------------------------------------
void Anp::SimlComb::Exec()
{
  //
  // Read new event
  //
  ++fIEvent;
  fBits.clear();

  if(fEntry.valid() && fEntry.GetState() == Ntuple::kREAD) {
    Run(fEntry.GetRef());    
    if(fDebug) { 
      log()<< "Exec - reading from TrigData" << endl;
    }
  }
  else {
    if(fDebug) { 
      log() << "Exec - no valid data" << endl;
    }
  }
}

//-----------------------------------------------------------------------------
void Anp::SimlComb::Done()
{
}

//-----------------------------------------------------------------------------
void Anp::SimlComb::Set(const TrigMonConfig &config)
{
  // 
  // Make combined triggers
  //  

  std::vector<TrigConfSeq> Seqs = config.getVec<TrigConfSeq>();
  if(fIsConfig || !fTrig.empty()) {
    return;
  }
  else {
    fIsConfig = true;
  }

  //
  // Insert configured chains
  //
  fConfgCh.insert(fConfgCh.begin(), 
		  config.begin<TrigConfChain>(), config.end<TrigConfChain>());
  
  // Find last counter
  for (unsigned int l = 1; l<4; l++) {
    for(std::vector<TrigConfChain>::const_iterator it = fConfgCh.begin(); it != fConfgCh.end(); ++it) {
      if (l==it->getLevelId()) {
	fCounter[l] = std::max<unsigned int>(fCounter[l], it->getCounter());
      }
    }
  }

  // Combination of all trigger in two vectors

//
//  This part is no longer supported. It used to enable the configuration of combined chains from the python configuration file.
//

/*  
  int created = 0;
  vector<string> list1, list2;
  if(fReg.Get("SimlComb::First",  list1) && !list1.empty() &&
     fReg.Get("SimlComb::Second", list2) && !list2.empty()) {   
    if (fDebug) {
      log() << "Set - DEBUG - First List ";
      for(unsigned int i = 0; i < list1.size(); ++i) { 
	cout << list1[i] << " ";
      }
      cout << endl;
      log() << "Set - DEBUG - Second List ";
      for(unsigned int i = 0; i < list2.size(); ++i) { 
	cout << list2[i] << " ";
      }
      cout << endl;
    }


    for(unsigned int i = 0; i < list1.size(); ++i) {
      for(unsigned int j = 0; j < list2.size(); ++j) {
	vector<string> tlist;
	tlist.push_back(list1[i]);
	tlist.push_back(list2[j]);
	log() << "Set - making trigger " << list1[i]+" "+list2[j] <<  endl;
	Make(tlist);
 	created++;
     }
    }
  }
*/
  // ParseXML
  string xmlname;
  int created = 0;

  if(fReg.Get("SimlComb::SimlXML",  xmlname)) {
    std::cout << "ParseXML simlcomb file " << xmlname << std::endl; 
    vector<vector< vector<string> > > combtrg_list = ParseXML(xmlname);
        
		
	int first = 0;
    for(vector<vector< vector<string> > >::iterator combtrg = combtrg_list.begin(); combtrg!=combtrg_list.end(); combtrg++) {
		first++;
		int second = 0;
		cout << "first " << first << " size: " << combtrg->size() << endl;

		for(vector< vector<string> >::iterator comp = combtrg->begin(); comp != combtrg->end(); comp++){
			second++;
			cout << "second: " << second << "size: " << comp->size() << endl;
			for(vector<string>::iterator lev = comp->begin(); lev != comp->end(); lev++) cout << *lev << endl; 
		}
	    Make(*combtrg);
    	created++;
    }
  }


  log() << "Set - created " << created << " trigger(s)" << endl;
}

//-----------------------------------------------------------------------------
vector<vector<vector<string> > >Anp::SimlComb::ParseXML(const std::string &fname) {
  vector<vector<vector<string> > > combtrg_list;

  //
  // Read the brunch group info from XML file
  //
  if(fname.empty()) {
    log() << "ParseXML - empty input file name" << endl;
    return combtrg_list;
  }

  log() << "check ParseXML - simulated triggers from XML file:\n   " << fname << endl;

  Anp::Registry xml_reg = Anp::ParseXml(fname, fDebug);
  log() << "Printing xmlreg: " << endl;
   
  xml_reg.Print();

  while(xml_reg.KeyExists("combination")) {	  
    vector<vector<string> > combtrg;

    //
    // Read and then erase new "combination" key
    //

    Anp::Registry combination_reg;
    xml_reg.Get("combination", combination_reg);
    xml_reg.RemoveKey("combination");

    if(fDebug) {
      cout << " read new \"combination\" key" << endl;
    }

    while(combination_reg.KeyExists("component")) {
      vector<string> combtrglevel;
      //
      // Read and then erase new "component" key
      //
      Anp::Registry component;
      combination_reg.Get("component", component);
      combination_reg.RemoveKey("component");

      if(fDebug) {
		cout << "  read new \"component\" " << component << endl;
      }
      string EF,L2,L1;

      if(!component.KeyExists("EF")){
        log() << "ParseXML - No EF chain given for component" << endl;
		break;
      }
      component.Get("EF", EF);
      component.RemoveKey("EF");
      combtrglevel.push_back(EF);
      if(fDebug) cout << "Added EF: " << EF << " from xml file " << endl;

      if(component.KeyExists("L2")){        
        component.Get("L2", L2);
        component.RemoveKey("L2");
        combtrglevel.push_back(L2);
        if(fDebug) cout << "Added L2: " << L2 << " from xml file " << endl;
      }
      else combtrglevel.push_back("Empty");

      if(component.KeyExists("L1")){        
        component.Get("L1", L1);
        component.RemoveKey("L1");
        combtrglevel.push_back(L1);
        if(fDebug) cout << "Added L1: " << L1 << " from xml file " << endl;
      }
      else combtrglevel.push_back("Empty");

      combtrg.push_back(combtrglevel);
	  cout <<"ADDED Combtrglevel " << endl;

    }

    combtrg_list.push_back(combtrg);
  }
  return combtrg_list;
}



//-----------------------------------------------------------------------------
void Anp::SimlComb::Run(const TrigMonEvent& event)
{
  const vector<TrigMonChain> chains = event.getChains();

  //
  // Compute decision bits
  //
  int i = 0;
  for(TrigVec::iterator it = fTrig.begin(); it != fTrig.end(); ++it) {
    i++;
    Handle<CombTrig> trig = *it;
    if(!trig.valid()) continue;
      if(trig->Pass(event, fIEvent,fDecisionBitLV1,fDecisionBitHLT)) {
      fBits.push_back(TrigMonChain(trig->GetEncodedId()));
      fBits.back().addDecision(TrigMonChain::kPassedRaw);
      if(trig->GetConfg().getName().find("L1") == string::npos) cout << "SimlComb::Run - passed "<< trig->GetConfg().getName()  << endl;
    }
  }
}

//-----------------------------------------------------------------------------
Anp::Handle<Anp::CombTrig> Anp::SimlComb::Make(vector<vector<string> >&clist)
{
  ofstream filestr ("EFStreams.txt");
  for(unsigned i = 0; i <fConfgCh.size(); ++i){
    filestr << "Chain: " << fConfgCh[i].getName() << endl;
    for(unsigned j = 0; j < fConfgCh[i].getStream().size(); ++j){
      filestr << "Stream Name: " << fConfgCh[i].getStream()[j] << endl;
    }
  }
  filestr.close();
      
  //
  // Create group of triggers
  //
  Anp::Handle<Anp::CombTrig> trig;

  // Need at least two chains in group
  if(clist.size() < 2) { 
    cout << "SimlComb::Make - bad configuration" << endl;
    //return trig;
  }

  // Find individual chains
  std::vector<TrigConfChain> cvec;
  vector<vector<string> > plist;
  vector<string> lowervec;
  vector<string> EFnamelist;
  if(fDebug) cout << "clist size: " << clist.size() <<endl;
  for(unsigned int i = 0; i < clist.size(); ++i) {
    // Look for chain configuration
    const string cname = clist[i][0];
	if(fDebug) cout << "clist i 0: " << cname << endl;
    EFnamelist.push_back(clist[i][0]);
    const vector<TrigConfChain>::const_iterator ichain = std::find(fConfgCh.begin(), fConfgCh.end(), cname);

    if(ichain == fConfgCh.end()) {
      cout << "SimlComb::Make - missing chain: \"" << cname << "\"" << endl;
      return trig;
    } else {
      cvec.push_back(*ichain);
	  if(fDebug) cout << " Added cvec string: " << ichain->getName() << endl; 
    }   

    // Add lower chain name for L2 or EF
	if(fDebug) cout << " ichain->getLevelId() " << ichain->getLevelId() << " Name: " <<ichain->getName() << endl;
    if(ichain->getLevelId() == 3) {
      if(clist[i][1] != "Empty") lowervec.push_back(clist[i][1]);
      else{ 
		  lowervec.push_back(ichain->getLowerName());
          if(fDebug) log() << " Added automated lower chain: " << ichain->getLowerName() << endl;
	  }
      if(clist[i][2] != "Empty") lowervec.push_back(clist[i][2]);
      else{
          lowervec.push_back(std::find(fConfgCh.begin(), fConfgCh.end(), ichain->getLowerName())->getLowerName());
          if(fDebug) log() << " Added automated lower chain: " << std::find(fConfgCh.begin(), fConfgCh.end(), ichain->getLowerName())->getLowerName() << endl;
      }
    }
    else if(ichain->getLevelId() == 2) {
      if(clist[i][1] != "Empty") lowervec.push_back(clist[i][1]);
      else{ 
		  lowervec.push_back(ichain->getLowerName());
		  if(fDebug) log() << " Added automated lower chain: " << ichain->getLowerName() << endl;
	  }
    }
    plist.push_back(lowervec);
	lowervec.clear();
  }
  if(fDebug){
	  cout << "plist: " << endl;
	  for(unsigned int i = 0; i < plist.size(); ++i) {
		  cout << i << endl;
		  for(unsigned int j = 0; j < plist[i].size(); ++j) cout << plist[i][j] << endl;
	  }
  }

  // Compute name
  string tname = Trig::CombinedName(EFnamelist);
  tname.append("_siml");
  if(tname == "_ siml"){ 
    cout << "SimlComb::Make - bad trigger name" << endl;
    return trig;
  }
  
  // Check aready existing triggers
  for(TrigVec::const_iterator it = fTrig.begin(); it != fTrig.end(); ++it) {
    if((*it) -> GetConfg().getName() == tname) {
      std::cout << "Trigger already found: " << (*it)->GetConfg().getName() <<endl;
      return *it;
    }
  }

  //
  // Make new trigger
  //
  trig = Anp::Handle<Anp::CombTrig>(new CombTrig());
  if(!trig->Config(cvec)) {
    cerr << "SimlComb::Make - failed to configure chain: " << tname << endl;    
    trig.release();
    return trig;
  }

  // Start new new configuration
  ++fCounter[trig->GetLevel()];

  // Add the required TEs to each chain.
  trig->SetTEs(cvec);
	cout << "trig->GetLevel" << trig->GetLevel() << endl;
  if(trig->GetLevel() > 1) {
    // Recursive call to make previous chain for L2 and EF
	cout <<"Making secondary time: " << endl;
    Anp::Handle<Anp::CombTrig> ptrig = SimlComb::Make(plist);
    if(!ptrig.valid()) {
      cerr << "SimlComb::Make - failed to make lower chain " << endl;    
      trig.release();
      return trig;
    }

    // Set previous (lower) trigger
    trig->SetLower(ptrig);

    const TrigConfChain &p = ptrig->GetConfg();
    TrigConfChain conf(tname, fCounter[trig->GetLevel()], 0, Trig::GetLevel(trig->GetLevel()), 
		       p.getChainName(), p.getCounter(), p.getId(), 1.0, 0.0);
    
    conf.addGroup("SIMUL");
    trig->SetConfg(conf);
    fSimulCh.push_back(conf);
    
    // add chain to the main (original) configuration
    if (!fConf.valid()) {
      log() << "Invalid configuration in SimlComb" << endl;
    }
    fConf->GetConfigModifiable("SimlComb").add(conf);

  }
  else if(trig->GetLevel() == 1) {
    // Special case for L1 chains
    TrigConfChain conf(tname, fCounter[trig->GetLevel()], 0, 1.0);
    conf.addGroup("SIMUL");
    trig->SetConfg(conf);
    fSimulCh.push_back(conf);

    // add chain to the main (original) configuration
    if (!fConf.valid()) {
      log() << "Invalid configuration in SimlComb" << endl;
    }
    fConf->GetConfigModifiable("SimlComb").add(conf);
  }

  // Save new trigger
  fTrig.push_back(trig);

  if(fDebug) {
    cout << "SimlComb - new trigger " << tname << ": ";
    for(unsigned int i = 0; i < cvec.size(); ++i){
	 cout << cvec[i].getName() << " ";
         TrigConfChain dummy = cvec[i];
         cout << "starting setconfg, number of Signatures: "  << dummy.getSignature().size() << endl;
         if(dummy.getSignature().size() > 0) cout << " signame: " << dummy.getSignature()[0].getLabel() << endl;
         for(unsigned k = 0; k <  dummy.getSignature().size(); ++k){
           TrigConfSig dummysig = dummy.getSignature()[k];
	   for(unsigned j = 0; j < dummysig.getOutputTEs().size(); ++j){
	     cout << "TEs: " << dummysig.getOutputTEs()[j] << endl;
           }
	}
    }
    cout << endl;
  }

  return trig;
}
