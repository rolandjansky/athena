/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// C/C++
#include <cmath>
#include <iostream>
#include <sstream>

// Local
#include "TrigCostBase/Factory.h"
#include "TrigCostBase/EventSvc.h"
#include "TrigCostBase/LumiABC.h"
#include "TrigCostBase/UtilCore.h"
#include "TrigCostBase/UtilHack.h"
#include "TrigCostBase/UtilTrig.h"

// Local
#include "TrigCostRate/MakeRates.h"

REGISTER_ANP_OBJECT(AlgEvent,MakeRates)

using namespace std;

//-----------------------------------------------------------------------------
Anp::MakeRates::MakeRates()
  :fDebug(false),
   fIsData(false),
   fIsSubAlg(false),
   fDoBandwidth(false),
   fDoGroups(true),
   fDoOverlap(false),
   fDoUnique(false),
   fIsConfig(false),
   fTotal(0.0),
   fScale(0.0),
   fCountPass(0),
   fCountFailL1(0),
   fCountFailL2(0),
   fCountFailEF(0),
   fDecisionBitLV1(TrigMonL1Item::kL1AfterVeto),
   fDecisionBitHLT(TrigMonChain::kPassedRaw),
   fBand(new RateBand())
{
}

//-----------------------------------------------------------------------------
Anp::MakeRates::~MakeRates()
{
}

//-----------------------------------------------------------------------------
void Anp::MakeRates::Config(const Registry &reg)
{
  //
  // Read configfuration and setup ntuple and other services
  //
  fReg = reg;

  reg.Get("MakeRates", "Debug",        fDebug);
  reg.Get("MakeRates", "IsData",       fIsData);
  reg.Get("MakeRates::IsSubAlg",       fIsSubAlg);
  reg.Get("MakeRates::DoBandwidth",    fDoBandwidth);
  reg.Get("MakeRates::DoGroups",       fDoGroups);  
  reg.Get("MakeRates::DoOverlap",      fDoOverlap);
  reg.Get("MakeRates::DoUnique",       fDoUnique);
  reg.Get("MakeRates::LumiAlg",        fLumiNameAlg);
  reg.Get("MakeRates::Ignore",         fIgnore);

  //
  // Determine bits to be used for rate computation
  //
  std::string rateKey;        // Rate computation type: weighted, raw count, etc
  std::string decisionKeyLV1; // Decision key used for LV1 rates
  std::string decisionKeyHLT; // Decision key used for HLT rates

  reg.Get("RateKey", rateKey);
 
  if(reg.Get("MakeRates", "DecisionKeyLV1", decisionKeyLV1)) {
    fDecisionBitLV1 = Anp::AsTriggerBitLV1(decisionKeyLV1);
  }
  if(reg.Get("MakeRates", "DecisionKeyHLT", decisionKeyHLT)) {
    fDecisionBitHLT = Anp::AsTriggerBitHLT(decisionKeyHLT);
  }

  log() << "Config" << endl
        << "   RateKey:          " << rateKey << endl
	<< "   LV1 decision bit: " << Anp::AsString(fDecisionBitLV1) << endl
	<< "   HLT decision bit: " << Anp::AsString(fDecisionBitHLT) << endl; 

  if(!fIsSubAlg) {
    if(fDebug) log() << "Config - is top level algorithm" << endl;

    //
    // Book tree branches before Init()
    //
    fEntry = NtupleSvc::Instance("event").Branch<TrigMonEvent>("data");
    
    //
    // Add self to global list - used by HtmlRates
    //
    Factory<RateABC>::Instance().Hold(AlgEvent::GetName(), Handle<RateABC>(this));
    
    //
    // Register trigger configuration algorithm
    //
    Anp::RegisterCallback(reg, "MakeRates::ConfAlg", fConf, this);

    //
    // Create scaling algorithm for BG fill pattern
    //
    vector<string> scale_algs;
    reg.Get("MakeRates::ScaleAlg", scale_algs);
    
    for(unsigned i = 0; i < scale_algs.size(); ++i) {
      const string &name = scale_algs.at(i);
      Handle<ScaleABC> alg = Factory<ScaleABC>::Instance().Get(name);

      if(alg.valid()) {
	log() << "Config - retrieved scaling algorithm: " << name << endl;
	fScaleVec.push_back(alg);
      }
      else {
	log() << "Config - missing scaling algorithm: " << name << endl;
      }
    }
  }
  else {
    if(fDebug) log() << "Config - is sub-algorithm" << endl;
  }

  fNode  .ConfigReg(fReg);
  fNodeMS.ConfigReg(fReg);
}

//-----------------------------------------------------------------------------
void Anp::MakeRates::Signal(const Registry &, const string &signal)
{
  //
  // Handle callback from configuration algorithm 
  //
  if(signal == "TRIG_CONFIG" || signal == "TRIG_CONFIG_DB") {
    if(!fConf.valid()) { 
      log() << "Signal - invalid configuration algorithm" << endl;
      return;
    }

    if(!fIsConfig) {
      Set(fConf->GetConfig()); 
    }
    else {
      Run(fConf->GetConfig());
    }
  }
}

//-----------------------------------------------------------------------------
bool Anp::MakeRates::Init()
{  
  //
  // Read configuration from init file and release pointer
  //
  return true;
}

//-----------------------------------------------------------------------------
void Anp::MakeRates::Exec()
{
  //
  // Read new event entry
  //
  if(fEntry.valid() && 
     fEntry.GetState() == Ntuple::kREAD &&
     fEntry.GetEvent() == Ntuple::kPASS) {
    Run(fEntry.GetRef());
  }
}

//-----------------------------------------------------------------------------
void Anp::MakeRates::Run(const TrigMonEvent &event)
{
  //
  // Process one event and collect triger decision bits
  //
  if(!fIsConfig) return;

  if(fDebug) log() << "Run - starting new event" << endl;

  fEventData.event_number = ++fCountPass;
  fEventData.run_number   = event.getRun();
  fEventData.lumi_block   = event.getLumi();
  fEventData.event_weight = EventSvc::Instance().GetWeight();

  fEventData.event_scale_lumi   = 1.0 ;
  fEventData.event_scale_rd0    = 1.0 ;
  fEventData.event_scale_rd1    = 1.0 ;
  fEventData.event_scale_pileup = 1.0 ;

  for(unsigned i = 0; i < fScaleVec.size(); ++i) {
    Scale::ScaleBGInfo scale_factors = fScaleVec.at(i)->GetScaleBCID();
    fEventData.event_scale_lumi *= scale_factors.scale_lumi;
    fEventData.event_scale_rd0  *= scale_factors.scale_rd0;
    fEventData.event_scale_rd1  *= scale_factors.scale_rd1;

    fEventData.event_scale_pileup  *= fScaleVec.at(i)->GetScalePileup();

    if(fDebug) {
      log() << "Run - BG scale factors (lumi, rd0, rd1): " 
	    << scale_factors.scale_lumi   << ", " 
	    << scale_factors.scale_rd0    << ", " 
	    << scale_factors.scale_rd1    << endl;
      log() << "Run - pileup scale factor: " 
	    << fScaleVec.at(i)->GetScalePileup() << endl;
    }
  }
  fEventBits.bitsL1.reset();
  fEventBits.bitsL2.reset();
  fEventBits.bitsEF.reset();
  fActiveChains.clear();

  const std::vector<uint16_t> &event_lv1 = event.getL1Item();
  const std::vector<uint32_t> &event_hlt = event.getChain();

  if(fDebug) {
    log() << "Run - processing new event" << endl
	  << "   LV1 triggers: " << event_lv1.size() << endl
	  << "   HLT triggers: " << event_hlt.size() << endl;
  }

  //
  // Process LV1
  //
  for(unsigned i = 0; i < event_lv1.size(); ++i) {
    const TrigMonL1Item item(event_lv1.at(i));

    if(item.isPassed(fDecisionBitLV1)) {
      AddActiveChain(1, item.getCounter());
    }
  }
  for(unsigned i = 0; i < event_hlt.size(); ++i) {
    const TrigMonChain chain(event_hlt.at(i));
    
    if(chain.isPassed(TrigMonChain::kL1BeforePrescale) && chain.getLevel() == 1) {
      AddActiveChain(1, chain.getCounter());
    }
  }

  //
  // Process HLT
  //
  for(unsigned i = 0; i < event_hlt.size(); ++i) {
    const TrigMonChain chain(event_hlt.at(i));
    if(chain.isPassed(fDecisionBitHLT)) {
      AddActiveChain(chain.getLevel(), chain.getCounter());
    }
  }

  //
  // Add trigger bits for simulated triggers
  //
  if(!fSimulVec.empty()) {
    for(SimulVec::const_iterator it = fSimulVec.begin(); it != fSimulVec.end(); ++it) {
      Handle<SimlABC> sim = *it;
      if(!sim.valid()) continue;
      
      const std::vector<TrigMonChain> &svec = sim->GetBits();
      for(unsigned int i = 0; i < svec.size(); ++i) {
	const TrigMonChain &chain = svec.at(i);
	if(chain.isPassed(fDecisionBitHLT)) {
	  AddActiveChain(chain.getLevel(), chain.getCounter());
	}
      }
    }
  }

  //
  // Set decisions for new rates code
  //
  fNode  .SetTrigger(fEventBits);
  fNodeMS.SetTrigger(fEventBits);

  //
  // Count passed triggers
  //
  std::sort(fActiveChains.begin(), fActiveChains.end());
  std::vector<RateChain *>::iterator it = std::unique(fActiveChains.begin(), fActiveChains.end());
  fActiveChains.erase(it, fActiveChains.end());
  
  for(unsigned i = 0; i < fActiveChains.size(); ++i) {
    RateChain *rate = fActiveChains.at(i);
    if(rate) {
      rate -> PassEvent(fEventData, fEventBits);
    }
    else {
      log() << "Run - NULL pointer for active chain" << endl;
    }
  }

  //
  // Count failed events
  //
  fCountFailL1 += int(fEventBits.bitsL1.count() == 0);
  fCountFailL2 += int(fEventBits.bitsL2.count() == 0);
  fCountFailEF += int(fEventBits.bitsEF.count() == 0);

  //
  // Special case: compute rates for chains with PassThrough
  //
  for(unsigned i = 0; i < fAlwaysChains.size(); ++i) {
    fAlwaysChains.at(i)->PassEvent(fEventData, fEventBits);
  }

  //
  // Compute group decisions
  //
  for(unsigned i = 0; i < fAllGroup.size(); ++i) {
    fAllGroup.at(i)->PassEvent(fEventData, fEventBits);
  }

}

//-----------------------------------------------------------------------------
void Anp::MakeRates::Set(const TrigMonConfig &config)
{
  //
  // Process configuration data for first time
  //
  if(fIsConfig) {
    log() << "Set - error: this MakeRates object is already configured!" << endl;
    return;
  }
  else {
    fIsConfig = true;
  }

  //
  // Read in list of chains 
  //
  std::vector<TrigConfChain> config_chains, multi_seeds, ignore_chains;
  ReadConfig(config, config_chains, multi_seeds, ignore_chains);
  

  cout 	<< "--------------------------------------------------------------------------" << endl;
  log() << "Set - these multi-seeded chains will be ignored for group rates" << endl;
  
  for(unsigned i = 0; i < multi_seeds.size(); ++i) {
    cout << "   " << multi_seeds.at(i).getName() << endl;
  }

  cout 	<< "--------------------------------------------------------------------------" << endl;
  
  //
  // Configure tree maker for new rates code
  //
  fNode  .ConfigTrigger(config_chains);
  fNodeMS.ConfigTrigger(multi_seeds);

  //
  // Make RateChain objects and collect data for RateGroup 
  //
  map<string, ChainVec> groups, levels, streams, ogroups;

  for(unsigned int i = 0; i < config_chains.size(); ++i) {
    const TrigConfChain &chain = config_chains.at(i);
    //
    // Create and configure RateChain using new tree-based rates code
    //
    string seed_opt = GetLv1RandomSeed(config, chain, config_chains);
    Handle<RateChain> rate = MakeChain(chain, config_chains, seed_opt);
    fChain.push_back(rate);

    //
    // Make lookup tables
    //
    if     (chain.getLevelId() == 1) Anp::ExpandVector(fLookupL1, rate, !rate->IsPT()); 
    else if(chain.getLevelId() == 2) Anp::ExpandVector(fLookupL2, rate, !rate->IsPT()); 
    else if(chain.getLevelId() == 3) Anp::ExpandVector(fLookupEF, rate, !rate->IsPT());

    if(rate->IsPT()) {
      fAlwaysChains.push_back(rate.get());
      
      Handle<RateChain> ratePT = MakeChain(chain, config_chains, "pt_only");
      fChainPT.push_back(ratePT);
      fAlwaysChains.push_back(ratePT.get());
    }
    
    // Collect chains for groups and streams defined in menu
    if(fDoGroups) {
      for(unsigned int j = 0; j < chain.getGroup().size(); ++j) {
	if(seed_opt != ""){
	  groups[chain.getLevel()+"_"+chain.getGroup().at(j)+"_"+seed_opt].push_back(rate);
	} else {
	  groups[chain.getLevel()+"_"+chain.getGroup().at(j)].push_back(rate);
	}
      }
      
      for(unsigned int j = 0; j < chain.getStream().size(); ++j) {
	const string &stream = chain.getStream().at(j);

	if(chain.getLevelId() == 2 && stream == "express") {
	  // Ignore express stream at L2
	  continue;
	}

	if(seed_opt != ""){
	  streams[chain.getLevel()+"_"+stream+"_"+seed_opt].push_back(rate);
	} else {
	  streams[chain.getLevel()+"_"+stream].push_back(rate);
	}
      }

      if(rate->GetLevel() == 3 && !rate->IsMultiSeed()) {
   
        const string& l1name = rate->GetInputL1().getName();
        if(std::find(config_chains.begin(), config_chains.end(), l1name) != config_chains.end()) {
	  ogroups[l1name].push_back(rate);
        }
      }
    }
    
    // Collect chains for entire level
    if(seed_opt != ""){
      levels[chain.getLevel()+"_"+seed_opt].push_back(rate);
    } else {
      levels[chain.getLevel()].push_back(rate);
    }
  }

  if (fDebug) {
    cout << "Lookup L1 Size = " << fLookupL1.size() << endl;
    for (unsigned counter =0; counter < fLookupL1.size(); counter++) {
      RateChain *chain = fLookupL1.at(counter);
      if (chain) {
	cout << counter << " " << chain->GetName() << endl;
      }
    }
    cout << "Lookup L2 Size = " << fLookupL2.size() << endl;
    for (unsigned counter =0; counter < fLookupL2.size(); counter++) {
      RateChain *chain = fLookupL2.at(counter);
      if (chain) {
	cout << counter << " " << chain->GetName() << endl;
      }
    }
    cout << "Lookup EF Size = " << fLookupEF.size() << endl;
    for (unsigned counter =0; counter < fLookupEF.size(); counter++) {
      RateChain *chain = fLookupEF.at(counter);
      if (chain) {
	cout << counter << " " << chain->GetName() << endl;
      }
    }
  }


  
  //
  // Make groups, streams and levels
  //
  for(map<string, ChainVec>::iterator git = groups.begin(); git != groups.end(); ++git) {
    Handle<RateGroup> rate = MakeGroup(git->first, "GROUP", git->second);
    fGroup.push_back(rate);
  }
  for(map<string, ChainVec>::iterator sit = streams.begin(); sit != streams.end(); ++sit) {
    Handle<RateGroup> rate;
    
    if(sit->first.find("express") != string::npos) {
      rate = MakeExpressStream(sit->first, sit->second, config_chains);
    }
    else {
      rate = MakeGroup(sit->first, "STREAM", sit->second);
    }

    fStream.push_back(rate);
    
    string lname = "";
    if(rate->GetLevel() == 2) lname = "L2";
    else if(rate->GetLevel() == 3) lname = "EF";
    else continue;
    
    for(unsigned i = 0; i < fChain.size(); ++i) {
      if( fChain[i]->GetLevel() != rate->GetLevel() ) {
        continue;
      }
      const string & phys_stream = GetPhysicsStream(fChain[i]);
      if( sit->first == lname+"_"+phys_stream) {
        fChain[i]->SetStreamGroup(rate);
      }
    }

  }
  for(map<string, ChainVec>::iterator lit = levels.begin(); lit != levels.end(); ++lit) {
    Handle<RateGroup> rate = MakeGroup(lit->first, "LEVEL", lit->second);
    fLevel.push_back(rate);
  }
  for(map<string, ChainVec>::iterator ogr = ogroups.begin(); ogr != ogroups.end(); ++ogr) {
    Handle<RateGroup> rate = MakeGroup("EF_SEEDED_"+ogr->first, "GROUP", ogr->second);
    vector<Handle<RateChain> >::iterator cit = std::find(fChain.begin(), fChain.end(), ogr->first);
    if(cit != fChain.end()) {
      (*cit)->SetOutputGroup(rate);
    }
  }
  
  //
  // Configure multi-seeded and unseeded chains
  //
  for(unsigned i = 0; i < multi_seeds.size(); ++i) {
    const TrigConfChain &chain = multi_seeds.at(i);
    MakeMultiSeed(chain, multi_seeds, config_chains);
  }

  //
  // Prepare all groups: make union groups for overlap, read XML, etc
  //
  PrepareGroups();

  //
  // Set bandwidth calculator
  //
  if(fDoBandwidth) {
    fBand->ConfigReg(fReg);
    fBand->SetRateAlg(*this);
    map<string, ChainVec> bgroups = fBand->GetRateGroups();
    for(map<string, ChainVec>::const_iterator git = bgroups.begin(); git != bgroups.end(); ++git) {
      fBand->AddConfiguredGroup(MakeGroup(git->first, "GROUP", git->second));
    }
  }

  log() << "Set - summary of configuration objects: " << endl 
	<< "   # of chains: input      "  << config.size<TrigConfChain>() << endl
	<< "   # of chains: config     "  << config_chains.size()         << endl
	<< "   # of chains: rate       "  << fChain.size()                << endl
	<< "   # of chains: always     "  << fAlwaysChains.size()         << endl
	<< "   # of chains: multi-seed "  << multi_seeds.size()           << endl    
	<< "   # of groups: rate       "  << fGroup.size()                << endl
	<< "   # of groups: level      "  << fLevel.size()                << endl
	<< "   # of groups: stream     "  << fStream.size()               << endl
	<< "   # of groups: compliment "  << fCompl.size()                << endl
	<< "   # of groups: cumulative "  << fCumul.size()                << endl
	<< "   # of groups: union      "  << fUnion.size()                << endl
	<< "   # of groups: total      "  << fAllGroup.size()             << endl;
}

//-----------------------------------------------------------------------------
void Anp::MakeRates::Run(const TrigMonConfig &)
{
  //
  // Update configuration
  //
  if(!fIsConfig) {
    log() << "Run - error: object is not configured... use Set() instead." << endl;
    return;
  }
}

//-----------------------------------------------------------------------------
namespace Helper
{
  template<class T>
  std::string GetStr(const T &val)
  {
    std::stringstream sval;
    sval << val;    
    return sval.str();
  }
}

//-----------------------------------------------------------------------------
void Anp::MakeRates::Done()
{
  //
  // Count passed events for trigger groups
  //  
  if(fCountPass < 1) {
    log() << "Done - no events passed trigger(s)" << endl;
    return;
  }

  //
  // Get scale for convertion from fraction to rate
  //
  if(fIsData) {

    Handle<LumiABC> alg = Factory<LumiABC>::Instance().Get(fLumiNameAlg);
    double total_time = 0, scale_corr = 1.0;

    if(alg.valid()) { 
      log() << "Done - found luminosity algorithm: " << fLumiNameAlg << endl;

      total_time = alg->GetIntegratedTime();      
      scale_corr = alg->GetRateCorrection();

      if(total_time > 0.0) { 
	fScale = fCountPass*scale_corr/total_time;
	fTotal = fCountPass;
      }
      else {
	fScale = 1.0;
	fTotal = 1.0;
      }
      
      double myscale = 1.0;
      fReg.Get("MakeRates::Scale", myscale);
      fScale *= myscale;

      if(fScaleVec.size() > 0) {
	fRegIndex.Set("Prediction Lumi", Helper::GetStr(fScaleVec.at(0)->GetPredLumi()));

	Registry bg_reg;
	vector<int> bg_size = fScaleVec.at(0)->GetPredBunchGroupNum();
	for(int ibg=0; ibg<(int) bg_size.size(); ibg++){
	  std::stringstream sval_bg;
	  sval_bg << "Prediction BGRP" << ibg;
	  bg_reg.Set(sval_bg.str(),       Helper::GetStr(bg_size.at(ibg)));
	  fRegIndex.Set("BunchGroups", bg_reg);
	}
      }

      std::vector<std::string> datasets = EventSvc::Instance().GetDataset();
      fRegIndex.Set("Number of datasets", Helper::GetStr(datasets.size()));
      for(int ifile=0; ifile<(int) datasets.size(); ifile++) {
	std::stringstream sval_set;
	sval_set << "Dataset  " << ifile+1;
	fRegIndex.Set(sval_set.str(), Helper::GetStr(datasets.at(ifile)));
      }
    }
    else {
      log() << "Done - missing luminosity algorithm: " << fLumiNameAlg << endl;
    }

    fRegIndex.Set("Time period (s)",  Helper::GetStr(total_time));
    fRegIndex.Set("Number of events", Helper::GetStr(fCountPass));
    
    log() << "Done - parameters for rates computation:" << endl
	  << "   nevent  = " << fCountPass   << endl
	  << "   fail L1 = " << fCountFailL1 << endl
	  << "   fail L2 = " << fCountFailL2 << endl
	  << "   fail EF = " << fCountFailEF << endl
	  << "   total   = " << fTotal       << endl
	  << "   scale   = " << fScale       << endl
	  << "   dt      = " << total_time   << endl
	  << "   corr    = " << scale_corr   << endl;
  }
  else if(EventSvc::Instance().IsValid()) {
    double lumi = 1.0;
    fReg.Get("MakeRates::Lumi",    lumi);

    fTotal = 1.0;
    if(!fReg.Get("MakeRates::Scale", fScale)) {
      fScale = 1.0;
    }

    log() << "Done - parameters are set using EventSvc" << endl
	  << "   total = " << fTotal     << endl
	  << "   lumi  = " << lumi       << endl
	  << "   scale = " << fScale     << endl;
  }
  else {    
    //
    // Read and also save(!) configuration for rate calculation
    //
    double geff = 1.0, lumi = 0, xsec = 0, ncollis = 1.0, ndenom = 0.0;
    
    fReg.Get("MakeRates::NCollis", ncollis);
    fReg.Get("MakeRates::NDenom",  ndenom);
    fReg.Get("MakeRates::Geff",    geff);
    fReg.Get("MakeRates::Lumi",    lumi);
    fReg.Get("MakeRates::Xsec",    xsec);
    
    if(fReg.KeyExists("MakeRates::Total")) {
      fReg.Get("MakeRates::Total", fTotal);
    }
    else {
      fTotal = fCountPass;
    }

    if(!fReg.Get("MakeRates::Scale", fScale)) {
      fScale = geff*lumi*xsec;
    }
      
    if(ncollis > 0) fScale /= ncollis;
    if(ndenom  > 0) fScale *= fCountPass/ndenom;

    fRegIndex.Set("Cross section",            Helper::GetStr(xsec));
    fRegIndex.Set("Luminosity",               Helper::GetStr(lumi));
    fRegIndex.Set("Collisions/crossing",      Helper::GetStr(ncollis));
    fRegIndex.Set("Number of events",         Helper::GetStr(fTotal)); 
    fRegIndex.Set("Single Event Sensitivity", Helper::GetStr(lumi*xsec*geff/(fTotal*ncollis))); 

    log() << "Done - parameters for rates computation:" << endl
	  << "   nevent  = " << fCountPass   << endl
	  << "   fail L1 = " << fCountFailL1 << endl
	  << "   fail L2 = " << fCountFailL2 << endl
	  << "   fail EF = " << fCountFailEF << endl
	  << "   total   = " << fTotal       << endl
	  << "   denom   = " << ndenom       << endl
	  << "   geff    = " << geff         << endl
	  << "   lumi    = " << lumi         << endl
	  << "   xsec    = " << xsec         << endl
	  << "   scale   = " << fScale       << endl;
  }
  
  if(fScale > 0.0 && fTotal > 0.0) {
    //
    // Finalize all elements
    //
    Registry reg;
    reg.Set("RateChain::CommonScale", fScale);
    reg.Set("RateChain::TotalEvents", fTotal);
    
    reg.Set("RateGroup::CommonScale", fScale);
    reg.Set("RateGroup::TotalEvents", fTotal);
    
    for(unsigned i = 0; i < fAllChain.size(); ++i) {
      fAllChain.at(i)->UpdateReg(reg);
    }
    for(unsigned i = 0; i < fAllGroup.size(); ++i) {
      fAllGroup.at(i)->UpdateReg(reg);
    }
    
    //
    // Finalize bandwidth calculations
    //
    if(fDoBandwidth) {
      fBand->Finalize(reg);
    }
  }
  else {
    log() << "Done - failed to determine rate normalization" << endl;
  }
}

//-----------------------------------------------------------------------------
bool Anp::MakeRates::Ignore(const TrigConfChain &chain) const
{
  //
  // Check input chain against list of ignored chain/keys
  //
  if(std::find(fIgnore.begin(), fIgnore.end(), chain.getName()) != fIgnore.end()) {
    return true;
  }

  for(unsigned int i = 0; i < fIgnore.size(); ++i) {
    if(chain.getName().find(fIgnore[i]) != string::npos) {
      return true;
    }
  }
  
  return false;
}

//-----------------------------------------------------------------------------
Anp::Handle<Anp::RateGroup> Anp::MakeRates::MakeGroup(const std::string &name, 
						      const std::string &type,
						      std::vector<Handle<RateChain> > chains) 
{
  //
  // Remove duplicates
  //
  const unsigned isize = chains.size();
  
  std::sort(chains.begin(), chains.end());
  std::vector<Handle<RateChain> >::iterator it = std::unique(chains.begin(), chains.end());
  chains.erase(it, chains.end());
  
  if(chains.size() != isize) {
    log() << "MakeGroup - removed " << isize-chains.size()<< " duplicate chains for: " << name << endl;
  }

  //
  // Create and configure simple group
  //
  vector<TrigConfChain> conf_chains;
  
  for(unsigned i = 0; i < chains.size(); ++i) {
    conf_chains.push_back(chains.at(i)->GetChain());
  }

  Anp::Registry reg(fReg);
  reg.Set("RateGroup::Name", name);
  reg.Set("RateGroup::Type", type);
  reg.Set("ElemName",        name);

  if(name.find("_rd0") != string::npos){
    reg.Set("MakeRates::IsRD0Seeded", "yes");
  } else {
    reg.Set("MakeRates::IsRD0Seeded", "no");
  } 

  if(name.find("_rd1") != string::npos){
    reg.Set("MakeRates::IsRD1Seeded", "yes");
  } else {
    reg.Set("MakeRates::IsRD1Seeded", "no");
  }

  Handle<RateGroup> group(new RateGroup());
  group->ConfigReg(reg);
  
  RateNode *node = fNode.CreateNode(conf_chains, reg); 
  group->ConfigGroup(node, chains);

  //
  // Store group handle for ownership and pointer for computation
  //
  fOwnGroup.push_back(group);
  fAllGroup.push_back(group.get());

  return group;
}

//-----------------------------------------------------------------------------
Anp::Handle<Anp::RateChain> Anp::MakeRates::MakeChain(const TrigConfChain &chain,
						      const vector<TrigConfChain> &config_chains,
						      const std::string &opt)						      
{
  //
  // Create new RateChain object
  //
  Registry reg;
  reg.Set("ElemName",  chain.getName());
  
  bool is_express = false;
  if(opt.find("express") != string::npos) {
    reg.Set("DoExpress", "yes");
    is_express = true;
  }
  
  if(opt.find("pt_only") != string::npos) {
    reg.Set("DoPTOnly", "yes");
  }
  
  if(opt.find("rd0") != string::npos){
    fReg.Set("MakeRates::IsRD0Seeded", "yes");
  } 
  else {
    fReg.Set("MakeRates::IsRD0Seeded", "no");
  }
  if(opt.find("rd1") != string::npos){
    fReg.Set("MakeRates::IsRD1Seeded", "yes");
  }
  else {
    fReg.Set("MakeRates::IsRD1Seeded", "no");
  }

  Handle<RateChain> rate(new Anp::RateChain());
  rate->ConfigReg(fReg);

  //
  // Create express stream node
  //
  RateNode *node = fNode.CreateNode(std::vector<TrigConfChain>(1, chain), reg);
  rate->ConfigChain(node, chain, config_chains);
  
  if(is_express && chain.getLevelId() == 3) {
    if(rate->IsPT()) {
      //
      // Express stream chain has PT: add it to always list
      //
      fAlwaysChains.push_back(rate.get()); 
    }
    else {
      //
      // HACK: add express stream chains to LookupES because they are duplicates
      //       of already existing chains (same counters).
      //
      Anp::ExpandVector(fLookupES, rate, true);
    }
  }
  
  //
  // Store pointer for ownership and reseting
  //
  fAllChain.push_back(rate);

  return rate;
}

//-----------------------------------------------------------------------------
Anp::Handle<Anp::RateGroup> Anp::MakeRates::MakeExpressStream(const std::string &name, 
							      vector<Handle<RateChain> > chains,
							      const vector<TrigConfChain> &config_chains)
{
  //
  // Remove duplicates
  //
  const unsigned isize = chains.size();
  
  std::sort(chains.begin(), chains.end());
  std::vector<Handle<RateChain> >::iterator it = std::unique(chains.begin(), chains.end());
  chains.erase(it, chains.end());
  
  if(chains.size() != isize) {
    log() << "MakeExpressStream - removed " << isize-chains.size()<< " duplicate chains for: " << name << endl;
  }

  std::vector<Handle<RateChain> > new_chains;
  vector<TrigConfChain> conf_chains;

  for(unsigned i = 0; i < chains.size(); ++i) {
    //
    // Make new RateChain objects with express stream PS applied
    //    
    const TrigConfChain &chain = chains.at(i)->GetChain();

    Handle<RateChain> rate = MakeChain(chain, config_chains, "express");

    new_chains.push_back(rate);
    conf_chains.push_back(chain);
  }

  //
  // Create and configure simple group
  //
  Anp::Registry reg(fReg);
  reg.Set("RateGroup::Name", name);
  reg.Set("RateGroup::Type", "STREAM");
  reg.Set("ElemName",        name);
  reg.Set("DoExpress",       "yes");

  Handle<RateGroup> group(new RateGroup());
  group->ConfigReg(reg);
  
  RateNode *node = fNode.CreateNode(conf_chains, reg); 
  group->ConfigGroup(node, new_chains);

  //
  // Store group handle for ownership and pointer for computation
  //
  fOwnGroup.push_back(group);
  fAllGroup.push_back(group.get());

  return group;
}

//-----------------------------------------------------------------------------
Anp::Handle<Anp::RateChain> Anp::MakeRates::MakeMultiSeed(const TrigConfChain &chain,
							  const vector<TrigConfChain> &multi_seeds,
							  const vector<TrigConfChain> &conf_chains,
							  const std::string &)
{
  //
  // Create new RateChain object
  //  
  Handle<RateChain> rate(new Anp::RateChain());
  rate->ConfigReg(fReg);

  //
  // Find input group
  //
  vector<TrigConfChain> inputs;
  
  if(chain.getLevelId() == 2) {
    //
    // Find inputs for multi-seeded L2 chain
    //
    inputs = Anp::GetInput(chain, conf_chains.begin(), conf_chains.end());
    if(inputs.size() < 2) {
      log() << "MakeMultiSeed - chain is not multi-seeded: input size: " << inputs.size() 
	    << ", "<< chain.getName() << ", seed: " << chain.getLowerName() << endl;
    }
  }
  else if(chain.getLevelId() == 3) {
    //
    // First look for inputs among regular L2 chains
    //
    inputs = Anp::GetInput(chain, conf_chains.begin(), conf_chains.end());

    if(inputs.size() == 0) {
      //
      // EF chain seeded by a single L2 multi-seeded chain
      //
      inputs = Anp::GetInput(chain, multi_seeds.begin(), multi_seeds.end());
      if(inputs.size() == 1) {
	inputs = Anp::GetInput(inputs.front(), conf_chains.begin(), conf_chains.end());

	if(inputs.size() < 2) {
	  log() << "MakeMultiSeed - L2 seed chain is not multi-seeded: " << chain.getName() << endl;
	}
      }
    }
  }

  //
  // Create rate tree
  //
  Registry reg;
  reg.Set("ElemName", chain.getName());

  RateNode *node   = fNode  .CreateNode(inputs, reg);
  RateNode *nodeMS = fNodeMS.CreateMultiSeedNode(chain, reg);

  rate->ConfigMultiSeed(node, nodeMS, chain, conf_chains); 
  
  //
  // Store pointer for ownership and reseting
  //
  fAllChain.push_back(rate);
  fMultiSeedChain.push_back(rate);
  fAlwaysChains.push_back(rate.get());

  return rate;
}

//-----------------------------------------------------------------------------
void Anp::MakeRates::ReadConfig(const TrigMonConfig &config,
				std::vector<TrigConfChain> &config_chains,
				std::vector<TrigConfChain> &multi_seeds,
				std::vector<TrigConfChain> &ignore_chains)
{
  //
  // Read trigger configuration from input TrigMonConfig and add simulated chains
  //

  //
  // Copy string variables for HTML and XML
  //
  const std::vector<std::string> &keys = config.getPairKey();
  const std::vector<std::string> &vals = config.getPairVal();
  
  if(keys.size() == vals.size()) {
    Registry reg;
    for(unsigned i = 0; i < keys.size(); ++i) {
      reg.Set(keys.at(i), vals.at(i));
    }
    
    fRegIndex.Set("MakeRates", reg);
  }

  //
  // Copy input chains and fill ignore list
  //
  for(unsigned i = 0; i < config.size<TrigConfChain>(); ++i) {
    const TrigConfChain &chain = config.at<TrigConfChain>(i);

    if(MakeRates::Ignore(chain)) {
      ignore_chains.push_back(chain);
    }
    else {        
      config_chains.push_back(chain);
    }
  }

  //
  // Now read configuration for simulated triggers
  //
  vector<string> svec;
  fReg.Get("MakeRates::SimulList", svec);

  for(unsigned int i = 0; i < svec.size(); ++i) {
    if(fDebug) {
      log() << "ReadConfig - trying simulation key: " << svec[i] << endl;
    }

    Handle<SimlABC> sim = Anp::Factory<SimlABC>::Instance().Get(svec[i]);
    if(!sim.valid()) { 
      if(fDebug) {
	log() << "ReadSimul - key does not exist: " << svec[i] << endl;
      }
      continue;
    }
    
    log() << "ReadConfig - adding " << sim -> GetConf().size()
	  << " simulated trigger(s) from: " << svec[i] << endl;
    
    //
    // Save simulation object and add new chains
    //
    fSimulVec.push_back(sim);

    //
    //const vector<TrigConfChain> &chains = sim -> GetConf();    
    //
    //for(unsigned i = 0; i < chains.size(); ++i) {
    //  const TrigConfChain &chain = chains.at(i);
    //  
    //  if(MakeRates::Ignore(chain)) {
    //	ignore_chains.push_back(chain);
    //  }
    //  else {        
    //	//config_chains.push_back(chain);
    //  }
    //}
  }

  if(fDebug) {
      log() << "ReadSimul - number of added simulations: " << fSimulVec.size() << endl;
  }

  if(fDebug) { 
    log() << "Exec - before pruning: config_chains.size(), ignore_chains.size() = " 
	  << config_chains.size() << ", " << ignore_chains.size() << endl;
  }

  //
  // Recursive remove for chains on ignored list for all three levels
  //
  Trig::Prune(ignore_chains, config_chains);
  
  //
  // Remove multi-seeded chains from config
  //
  vector<TrigConfChain>::iterator iter = config_chains.begin();
  while(iter != config_chains.end()) {
    if(iter->getLevelId() == 1) {
      ++iter;
      continue;
    }
    
    const string &seed = iter->getLowerName();
    if(seed.find(",") != string::npos || seed.empty()) {
      multi_seeds.push_back(*iter);
      iter = config_chains.erase(iter);
    }
    else {
      ++iter;
    }
  }

  //
  // Move to multi-seeded list single-seed EF chains seeded by multi-seeded L2 chains
  //
  iter = config_chains.begin();
  while(iter != config_chains.end()) {    
    const string &seed = iter->getLowerName();

    if(std::find(multi_seeds.begin(), multi_seeds.end(), seed) != multi_seeds.end()) {
      multi_seeds.push_back(*iter);
      iter = config_chains.erase(iter);
    }
    else {
      ++iter;
    }
  }  

  //
  // Remove chains with mis-configured input
  //
  vector<TrigConfChain> bad_config;

  iter = config_chains.begin();
  while(iter != config_chains.end()) {
    if(iter->getLevelId() == 1) {
      ++iter;
      continue;
    }
    
    const string &seed = iter->getLowerName();
    if(std::find(config_chains.begin(), config_chains.end(), seed) == config_chains.end()) {
      bad_config.push_back(*iter);
      iter = config_chains.erase(iter);
    }
    else {
      ++iter;
    }
  }

  iter = config_chains.begin();
  while(iter != config_chains.end()) {    
    const string &seed = iter->getLowerName();

    if(std::find(bad_config.begin(), bad_config.end(), seed) != bad_config.end()) {
      bad_config.push_back(*iter);
      iter = config_chains.erase(iter);
    }
    else {
      ++iter;
    }
  }  

  if(!bad_config.empty()) {
    cout << "**************************************************************************" << endl;
    log() << "ReadConfig - removed " << bad_config.size() << " mis-configured chain(s): " << endl;

    for(unsigned i = 0; i < bad_config.size(); ++i) {
      const TrigConfChain &c = bad_config.at(i);
      cout << "   " << c.getName() << " - lower chain: \"" << c.getLowerName() << "\"" << endl;
    }
  }

  std::sort(multi_seeds.begin(), multi_seeds.end());
  std::sort(config_chains.begin(), config_chains.end());
}

//-----------------------------------------------------------------------------
Anp::Handle<Anp::RateGroup> Anp::MakeRates::MakeCompliment(Handle<RateChain> chain)
{
  //
  // Make group which is compliment of input chain
  //
  
  // Collect compliment chains
  ChainVec comp_vec;  

  for(unsigned i = 0; i < fChain.size(); ++i) {
    Handle<RateChain> curr = fChain[i];
    
    if(chain->GetLevel() != curr->GetLevel() || chain->GetLevel() == 1) {
      continue;
    }
    if(chain->GetName() == curr->GetName()) {
      continue;
    }

   
    if(GetPhysicsStream(chain) == GetPhysicsStream(curr) ) {
      comp_vec.push_back(curr);
    }
  }

  //
  // Add pointer to compliment group for "this" chain
  //
  Handle<RateGroup> rate = MakeGroup(chain->GetName()+"_compl", "COMPL", comp_vec);
  chain->SetCompliment(rate);
  
  // Find total level group
  string level;
  if(chain->GetLevel() == 1) level = "L1"; 
  if(chain->GetLevel() == 2) level = "L2"; 
  if(chain->GetLevel() == 3) level = "EF"; 

  GroupVec::const_iterator lit = std::find(fLevel.begin(), fLevel.end(), level);
  if(lit != fLevel.end()) {
    chain->SetLevelGroup(*lit);    
  }

  return rate;
}

//-----------------------------------------------------------------------------
Anp::Handle<Anp::RateGroup> Anp::MakeRates::MakeCompliment(Handle<RateGroup> group)
{
  //
  // Make group which is complment of input chain
  //
  
  // Collect compliment chains
  ChainVec comp_vec;  

  for(unsigned i = 0; i < fChain.size(); ++i) {
    Handle<RateChain> curr = fChain.at(i);
    
    if(group->GetLevel() != curr->GetLevel()) {
      continue;
    }
    if(std::find(group->GetChains().begin(),
		 group->GetChains().end(), curr) != group->GetChains().end()) {
      continue;
    }

    comp_vec.push_back(curr);
  }

  //
  // Add pointer to compliment group for "this" group
  //
  Handle<RateGroup> rate = MakeGroup(group->GetName()+"_compl", "COMPL", comp_vec);
  group->SetCompliment(rate);

  // Find total level group
  string level;
  if(group->GetLevel() == 1) level = "L1"; 
  if(group->GetLevel() == 2) level = "L2"; 
  if(group->GetLevel() == 3) level = "EF"; 

//   if(group->GetName().find("_rd0") != string::npos) level += "_rd0";
//   if(group->GetName().find("_rd1") != string::npos) level += "_rd1";

  GroupVec::const_iterator lit = std::find(fLevel.begin(), fLevel.end(), level);
  if(lit != fLevel.end()) {
    group->SetLevelGroup(*lit);    
  }

  return rate;
}

//-----------------------------------------------------------------------------
Anp::Handle<Anp::RateGroup> Anp::MakeRates::MakeUnion(Handle<RateGroup> g1,
						      Handle<RateGroup> g2)
{
  //
  // Make group which is union of two groups
  //
  
  // Collect compliment chains
  ChainVec comp_vec;  

  comp_vec.insert(comp_vec.end(), g1->GetChains().begin(), g1->GetChains().end());
  comp_vec.insert(comp_vec.end(), g2->GetChains().begin(), g2->GetChains().end());

  // Keep unique chains
  std::sort(comp_vec.begin(), comp_vec.end());
  comp_vec.erase(std::unique(comp_vec.begin(), comp_vec.end()), comp_vec.end()); 
  
  Handle<RateGroup> rate = MakeGroup("UNION_"+g1->GetName()+"_AND_"+g2->GetName(), "UNION", comp_vec);
  rate->SetUnionGroup(g1, g2); 
  return rate;
}

//-----------------------------------------------------------------------------
void Anp::MakeRates::PrepareGroups()
{
  //
  // Make compliments and set pointer for total level rate
  //
  if(fDoUnique) {
    for(unsigned int i = 0; i < fChain.size(); ++i) {      
      if(fChain[i]->GetLevel() != 1) {
        fCompl.push_back(MakeCompliment(fChain.at(i)));
      }
    }  
    for(unsigned int i = 0; i < fGroup.size(); ++i) {
      fCompl.push_back(MakeCompliment(fGroup.at(i)));
    }
    for(unsigned int i = 0; i < fStream.size(); ++i) {
      fCompl.push_back(MakeCompliment(fStream.at(i)));
    }
  }

  //
  // Make unions to compute group overlaps
  //
  if(fDoOverlap) {

    for(unsigned int i = 0; i < fGroup.size(); ++i) {
      Handle<RateGroup> group1 = fGroup.at(i);

      for(unsigned int j = i+1; j < fGroup.size(); ++j) {
	Handle<RateGroup> group2 = fGroup.at(j);

	if(group1->GetLevel() == group2->GetLevel()) {
	  fUnion.push_back(MakeUnion(group1, group2));
	}
      }
    }

    for(unsigned int i = 0; i < fStream.size(); ++i) {
      Handle<RateGroup> group1 = fStream.at(i);

      for(unsigned int j = i+1; j < fStream.size(); ++j) {
	Handle<RateGroup> group2 = fStream.at(j);

	if(group1->GetLevel() == group2->GetLevel()) {
	  fUnion.push_back(MakeUnion(group1, group2));
	}
      }
    }
  }
  
  //
  // Prepare groups for triggers not included in any other group
  //
  map<string, ChainVec> groups;

  for(unsigned i = 0; i < fChain.size(); ++i) {
    Handle<RateChain> chain = fChain.at(i);   
    if(chain->GetLevel() == 1) {
      continue;
    }

    bool pass = false;
    for(unsigned g = 0; g < fGroup.size(); ++g) {
      const std::vector<Handle<RateChain> > &chains = fGroup.at(g)->GetChains();
      pass = pass || std::find(chains.begin(), chains.end(), chain) != chains.end();
    }
    
    if(!pass) {
      groups[chain->GetChain().getLevel()+"_ChainsWithoutGroup"].push_back(chain);    
    }
  }
  
  //
  // Make groups for remaining chains
  //
  for(map<string, ChainVec>::iterator git = groups.begin(); git != groups.end(); ++git) {
    if(git->second.empty()) continue;
    
    if(std::find(fGroup.begin(), fGroup.end(), git->first) != fGroup.end()) {
      log() << "PrepareGroups - group already defined: " << git->first << endl;
      continue;
    }

    Handle<RateGroup> rate = MakeGroup(git->first, "GROUP", git->second);
    fGroup.push_back(rate);
  }
}
//-----------------------------------------------------------------------------
std::string Anp::MakeRates::GetPhysicsStream(const Handle<RateChain> chain) const {

  vector<string> streams = chain->GetChain().getStream();
  vector<string> phys_stream;
  for(unsigned int i = 0; i < streams.size(); ++i) {
    if( streams[i] != "express" ) {
      phys_stream.push_back(streams[i]);
    }
  }

  if(phys_stream.size() == 0) {
    if(fDebug) {
      cout << "MakeRates::GetPhysicsStream - No Stream Found for " << GetName() << endl;
    }
    return "";
  }
  if(phys_stream.size() > 1 && fDebug) {
    cout << "MakeRates::GetPhysicsStream - Multiple Physics streams found for " << GetName() << " : " << endl;
    for(unsigned i = 0; i < phys_stream.size(); ++i) {
      cout << phys_stream[i] << endl;
    }
  }

  return phys_stream[0];

}

//-----------------------------------------------------------------------------
std::string Anp::MakeRates::GetLv1RandomSeed(const TrigMonConfig &config, 
					     const TrigConfChain &chain, 
					     const std::vector<TrigConfChain> &config_chains) const {

  // determine if this chain is seeded by a random LVL1 seed or not

  string lvl1_seed = "";
  TrigConfChain chain_inputL1;
  //
  // Find LVL1 chain
  //
  if(chain.getLevelId() == 1) {
    chain_inputL1 = chain;
  }
  else if(chain.getLevelId() == 2) {

    const vector<TrigConfChain> inputL1 = Anp::GetInput(chain, config_chains.begin(), config_chains.end());
    
    if(inputL1.size() == 1) {
      chain_inputL1 = inputL1.at(0);
    }
    else {
      cout << "MakeRates::GetLv1RandomSeed - can not configure multi-seeded chains for: " << chain.getName() << endl;
      return "";
    }
  }
  else if(chain.getLevelId() == 3) {
    //
    // Read configurartion for input L2 chain
    //
    const vector<TrigConfChain> inputL2 = Anp::GetInput(chain, config_chains.begin(), config_chains.end());

    if(inputL2.size() != 1) {
      cout << "MakeRates::GetLv1RandomSeed - can not configure multi-seeded EF chains for: " << chain.getName() << endl;
      return "";
    }
    const vector<TrigConfChain> inputL1 = Anp::GetInput(inputL2.at(0), config_chains.begin(), config_chains.end());
      
    if(inputL1.size() == 1) {
      chain_inputL1 = inputL1.at(0);
    }
    else {
      cout << "MakeRates::GetLv1RandomSeed - can not configure multi-seeded chains for: " << chain.getName() << endl;
      return "";
    }
  }
    
  //
  // Fill L1 threshold information
  //
  const std::vector<TrigConfSig> &sig_list = chain_inputL1.getSignature();
    
  for(unsigned int isig = 0; isig < sig_list.size(); ++isig) {
    const std::vector<uint32_t> &tes = sig_list.at(isig).getOutputTEs();
    
    for(unsigned ite = 0; ite < tes.size(); ++ite) {
      const vector<TrigConfSeq>::const_iterator iterSeq = config.findSeq(tes.at(ite));
      if(iterSeq == config.end<TrigConfSeq>()) {
	continue;
      }
      string lv1_seed_name = iterSeq->getName();
      if( lv1_seed_name == "RNDM0" ) {
	lvl1_seed += "rd0";
      }
      if( lv1_seed_name == "RNDM1" ) {
	lvl1_seed += "rd1";
      }
    }
  }
  return lvl1_seed;

}
