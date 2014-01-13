/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// C/C++
#include <cmath>
#include <iostream>
#include <sstream>
#include <string>

// Local
#include "TrigCostBase/EventSvc.h"
#include "TrigCostBase/Factory.h"
#include "TrigCostBase/UtilCore.h"
#include "TrigCostBase/UtilTrig.h"

// Local
#include "TrigCostRate/ScaleBG.h"

REGISTER_ANP_OBJECT(AlgEvent,ScaleBG)

using namespace std;

//-----------------------------------------------------------------------------
void Anp::ScaleBG::HLTData::PrintTriggers() const
{
  //
  // Reset triggers
  //
  cout << "      HLT filter: " << filter_name 
       << ", HLT PS=" << hlt_ps 
       << ", random rate fraction=" << l1_rndweight
       << ", HLT weight=" << hlt_weight
       << ", total weight=" << tot_weight
       << ", counter=" << hlt_counter << endl;

  if(!ef_unbiastrig.empty()){
    cout << "      Unbias EF trigger=" << ef_unbiastrig
       << ", total PS=" << hlt_eb_unbias_ps
       << ", total weight=" << hlt_eb_unbias_weight << endl;
  }

  if(hlt_same_seeds.size() > 0){
    cout << "      Same seeded EF triggers=" << endl;;
    for(unsigned i = 0; i < hlt_same_seeds.size(); ++i) {
      const HLTData &same_seeds = hlt_same_seeds.at(i);
      cout << "            " << same_seeds.filter_name
      << ", HLT PS=" << same_seeds.hlt_ps
      << ", total weight=" << same_seeds.hlt_weight << endl;
      cout << "               L1 list: ";
      for(unsigned i = 0; i < same_seeds.lv1_weight.size(); ++i) {
	const LV1Data &data = same_seeds.lv1_weight.at(i);
	cout << data.lv1_name << ", ";
      }
      cout << endl;
    }
  }

  cout << "      Run range=" << runnumber_low << " to " << runnumber_high << endl;

  unsigned wid = 0;

  for(unsigned i = 0; i < lv1_filter.size(); ++i) {
    const LV1Data &data = lv1_filter.at(i);
    wid = std::max<unsigned>(wid, data.lv1_name.size());
  }
  for(unsigned i = 0; i < lv1_weight.size(); ++i) {
    const LV1Data &data = lv1_weight.at(i);
    wid = std::max<unsigned>(wid, data.lv1_name.size());
  }

  for(unsigned i = 0; i < lv1_filter.size(); ++i) {
    const LV1Data &data = lv1_filter.at(i);
    cout << "      FILTER: " << setw(wid) << data.lv1_name << ", PS=" << data.lv1_ps << " CTP id =" << data.ctp_id << endl;
  }
  for(unsigned i = 0; i < lv1_weight.size(); ++i) {
    const LV1Data &data = lv1_weight.at(i);
    cout << "      WEIGHT: " << setw(wid) << data.lv1_name << ", PS=" << data.lv1_ps << endl;
  }

  for(unsigned i = 0; i < lv1_ebtrigs.size(); ++i) {
    const LV1Data &data = lv1_ebtrigs.at(i);
    cout << "      EB Triggers: " << setw(wid) << data.lv1_name << ", HLT chain " << data.hlt_chain_name << " CTP id =" << data.ctp_id << endl;
  }
}

//-----------------------------------------------------------------------------
void Anp::ScaleBG::HLTData::ResetTriggers(bool fromXML)
{
  //
  // Reset triggers
  //
  hlt_ps     = 0.0;
  hlt_weight = 0.0;

  // if reading from config, just clear the values
  if(fromXML){
    for(unsigned i = 0; i < lv1_filter.size(); ++i) {
      LV1Data &data = lv1_filter.at(i);
      data.lv1_ps     = 0.0;
      data.lv1_weight = 0.0;
      data.hlt_chain_ps = 0.0;
      data.hlt_chain_weight = 0.0;
    }
    for(unsigned i = 0; i < lv1_weight.size(); ++i) {
      LV1Data &data = lv1_weight.at(i);
      data.lv1_ps     = 0.0;
      data.lv1_weight = 0.0;
      data.hlt_chain_ps = 0.0;
      data.hlt_chain_weight = 0.0;
    }
    for(unsigned i = 0; i < lv1_ebtrigs.size(); ++i) {
      LV1Data &data = lv1_ebtrigs.at(i);
      data.lv1_ps     = 0.0;
      data.lv1_weight = 0.0;
      data.hlt_chain_ps = 0.0;
      data.hlt_chain_weight = 0.0;
    }
    // if reading from DB, clear all the weight names
  } else {
    lv1_filter.clear();
    lv1_weight.clear();
    lv1_ebtrigs.clear();
  }
}

//-----------------------------------------------------------------------------
void Anp::ScaleBG::HLTData::GetLVL1Seeds(const TrigMonConfig &config, bool fromXML, std::ostream &os)
{  

  if(fromXML) return;

  // 1) find EF trigger
  const vector<TrigConfChain>::const_iterator it_ef = config.findChain(filter_name);
  if(it_ef == config.end<TrigConfChain>()) {
    os << "GetLVL1Seeds - failed to find EF filter trigger: " << filter_name << endl;
    return;
  }

  // 2) find L2 trigger
  const vector<TrigConfChain>::const_iterator it_l2 = config.findChain(it_ef->getLowerName());
  if(it_l2 == config.end<TrigConfChain>()) {
    os << "GetLVL1Seeds - failed to find L2 filter trigger: " << it_ef->getLowerName() << endl;
  }


  // return) find L1 trigger - for single seeded filters only
  const vector<TrigConfChain>::const_iterator it_l1 = config.findChain(it_l2->getLowerName());
  if(it_l1 != config.end<TrigConfChain>()) {

    // if the chain has EB items listed in the L2_hypo, skip this
    if(it_l2->getEBHypo().size() != 0) return;

    LV1Data lv1;
    lv1.lv1_name = it_l2->getLowerName();

    if(!lv1.lv1_name.empty()) {
      lv1_weight.push_back(lv1);
    }
  }
  // 4) Find L1 trigger - for multi seeded triggers
  else {

    std::vector<string> lchains;
    Anp::StringTok(lchains, it_l2->getLowerName(), ", ");
    
    for(unsigned j = 0; j < lchains.size(); ++j) {
      LV1Data lv1;
      lv1.lv1_name = lchains.at(j);

      if(!lv1.lv1_name.empty()) {
	lv1_weight.push_back(lv1);
      }
    }
  }
  return;

}
//-----------------------------------------------------------------------------
void Anp::ScaleBG::HLTData::GetHypoSeeds(const TrigMonConfig &config, bool fromXML, std::ostream &os)
{  

  // find the LVL1 seeds for any EB chain (if not already filled via XML file)
  if(fromXML) return;

  // 1) find EF trigger
  const vector<TrigConfChain>::const_iterator it_ef = config.findChain(filter_name);
  if(it_ef == config.end<TrigConfChain>()) {
    os << "GetHypoSeeds - failed to find EF filter trigger: " << filter_name << endl;
    return;
  }
  
  // 2) find L2 trigger
  const vector<TrigConfChain>::const_iterator it_l2 = config.findChain(it_ef->getLowerName());
  if(it_l2 == config.end<TrigConfChain>()) {
    os << "GetHypoSeeds - failed to find L2 filter trigger: " << it_ef->getLowerName() << endl;
  }

  // parse the L1 names for this hypo 
  for(unsigned int il1=0; il1<it_l2->getEBHypo().size(); il1++){
    std::string hypo_list = it_l2->getEBHypo().at(il1);
    
    // Remove extra characters
    size_t pos = 0;
    while((pos = hypo_list.find("'", pos)) != std::string::npos) {
      hypo_list.replace(pos, 1, "");
      pos += 1;
    }
    hypo_list.replace(hypo_list.find("["), 1, "");
    hypo_list.replace(hypo_list.find("]"), 1, "");

    std::vector<string> lchains;
    Anp::StringTok(lchains, hypo_list, ", ");
    
    for(unsigned j = 0; j < lchains.size(); ++j) {
      LV1Data lv1;
      lv1.lv1_name = lchains.at(j);

      if(!lv1.lv1_name.empty()) {
	lv1_weight.push_back(lv1);
      }
    }
  }
  
}

//-----------------------------------------------------------------------------
void Anp::ScaleBG::HLTData::UpdateHLT(const TrigMonConfig &config, std::ostream &os)
{
  //
  // Update PS for HLT and LV1 triggers
  //

  // 1) find EF trigger
  const vector<TrigConfChain>::const_iterator it_ef = config.findChain(filter_name);
  if(it_ef == config.end<TrigConfChain>()) {
    os << "UpdateHLT - failed to find EF filter trigger: " << filter_name << endl;
    return;
  }

  // 2) find L2 trigger
  const vector<TrigConfChain>::const_iterator it_l2 = config.findChain(it_ef->getLowerName());
  if(it_l2 == config.end<TrigConfChain>()) {
    os << "UpdateHLT - failed to find L2 filter trigger: " << it_ef->getLowerName() << endl;
    return;
  }
  
  // 3) find L1 trigger - for single seeded filters only
  const vector<TrigConfChain>::const_iterator it_l1 = config.findChain(it_l2->getLowerName());
  l1_eb_weight = 1.0;
  if(it_l1 != config.end<TrigConfChain>()) {
    //
    // Compute total PS using LV1 and HLT triggers
    //
    if(!(it_l1->getPS() < 1.0) && !(it_l2->getPS() < 1.0) && !(it_ef->getPS() < 1.0)) {
      l1_eb_weight  = 1.0/it_l1->getPS();
    }
  } 

  if(!(it_l2->getPS() < 1.0) && !(it_ef->getPS() < 1.0)) {
    hlt_ps     = it_l2->getPS()*it_ef->getPS();
    hlt_weight = 1.0/hlt_ps;
    tot_weight = hlt_weight*l1_eb_weight;
  }

  hlt_counter = it_ef->getCounter();

  UpdateLV1(config, lv1_filter);
  UpdateLV1(config, lv1_weight);
  UpdateLV1(config, lv1_ebtrigs);

  UpdateEBPS(config, lv1_ebtrigs);

  // get the HLT information for the unbiased EB chain
  if(!ef_unbiastrig.empty()) {
    // 1) find EF trigger
    const vector<TrigConfChain>::const_iterator it_ef = config.findChain(ef_unbiastrig);
    if(it_ef == config.end<TrigConfChain>()) {
      os << "UpdateHLT - failed to find EF filter trigger: " << ef_unbiastrig << endl;
      return;
    }

    // 2) find L2 trigger
    const vector<TrigConfChain>::const_iterator it_l2 = config.findChain(it_ef->getLowerName());
    if(it_l2 == config.end<TrigConfChain>()) {
      os << "UpdateHLT - failed to find L2 filter trigger: " << it_ef->getLowerName() << endl;
      return;
    }
  
    // 3) find L1 trigger - for single seeded filters only
    const vector<TrigConfChain>::const_iterator it_l1 = config.findChain(it_l2->getLowerName());
    if(it_l1 != config.end<TrigConfChain>()) {
      //
      // Compute total PS using LV1 and HLT triggers
      //
      if(!(it_l1->getPS() < 1.0) && !(it_l2->getPS() < 1.0) && !(it_ef->getPS() < 1.0)) {
	hlt_eb_unbias_ps     = it_l1->getPS()*it_l2->getPS()*it_ef->getPS();
	hlt_eb_unbias_weight = l1_rndweight/hlt_eb_unbias_ps;
	l1_eb_unbias_counter  = it_l1->getCounter();
	hlt_eb_unbias_counter  = it_ef->getCounter();
      }
    }
    else {
      //
      // Compute total PS using HLT triggers only
      //
      if(!(it_l2->getPS() < 1.0) && !(it_ef->getPS() < 1.0)) {
	hlt_eb_unbias_ps     = it_l2->getPS()*it_ef->getPS();
	hlt_eb_unbias_weight = l1_rndweight/hlt_eb_unbias_ps;
	l1_eb_unbias_counter  = 0;
	hlt_eb_unbias_counter  = 0;
      }
    }
  }
}

//-----------------------------------------------------------------------------
void Anp::ScaleBG::HLTData::UpdateLV1(const TrigMonConfig &config, std::vector<LV1Data> &lv1_vec) const
{
  //
  // Update PS for LV1 triggers
  //

  for(unsigned j = 0; j < lv1_vec.size(); ++j) {
    LV1Data &lv1 = lv1_vec.at(j);
    
    //
    // Find L1 trigger configuration
    //
    const vector<TrigConfChain>::const_iterator iter = config.findChain(lv1.lv1_name);
    if(iter == config.end<TrigConfChain>()) {
      cout << "ScaleBG::ERROR: lv1 chain not found! " << lv1.lv1_name << endl;
      throw new std::string("ScaleBG::ERROR: lv1 chain not found!");
    }
    
    lv1.ctp_id = iter->getCounter();

    if(!(iter->getPS() < 1.0)) {
      lv1.lv1_ps     = iter->getPS();
      lv1.lv1_weight = 1.0/lv1.lv1_ps;
    }
    
    //
    // Fill L1 threshold information
    //
    const std::vector<TrigConfSig> &sig_list = iter->getSignature();
    
    for(unsigned int isig = 0; isig < sig_list.size(); ++isig) {
      const std::vector<uint32_t> &tes = sig_list.at(isig).getOutputTEs();
      
      for(unsigned ite = 0; ite < tes.size(); ++ite) {
	const vector<TrigConfSeq>::const_iterator iterSeq = config.findSeq(tes.at(ite));
	if(iterSeq == config.end<TrigConfSeq>()) {
	  cout << "ERROR: sequence not found! " << tes.at(ite) << endl;
	  throw new std::string("ScaleBG::ERROR: sequence not found!");
	}
	
	lv1.lv1_thr.insert(iterSeq->getName());
      }
    }
  }
}

void Anp::ScaleBG::HLTData::UpdateEBPS(const TrigMonConfig &config, std::vector<LV1Data> &lv1_vec) const
{
  //
  // Update PS for EB triggers
  //

  for(unsigned j = 0; j < lv1_vec.size(); ++j) {
    LV1Data &lv1 = lv1_vec.at(j);

    // get the HLT information for the EB chain
    if(!lv1.hlt_chain_name.empty()) {
      // 1) find EF trigger
      const vector<TrigConfChain>::const_iterator it_ef = config.findChain(lv1.hlt_chain_name);
      if(it_ef == config.end<TrigConfChain>()) {
	cout << "UpdateHLT - failed to find EF filter trigger: " << lv1.hlt_chain_name << endl;
	return;
      }

      // 2) find L2 trigger
      const vector<TrigConfChain>::const_iterator it_l2 = config.findChain(it_ef->getLowerName());
      if(it_l2 == config.end<TrigConfChain>()) {
	cout << "UpdateHLT - failed to find L2 filter trigger: " << it_ef->getLowerName() << endl;
	return;
      }
  
      //
      // Compute total PS using HLT triggers only
      //
      if(!(it_l2->getPS() < 1.0) && !(it_ef->getPS() < 1.0)) {
	lv1.hlt_chain_ps     = it_l2->getPS()*it_ef->getPS();
	lv1.hlt_chain_weight = 1.0/lv1.hlt_chain_ps;
      }
    }
  }
}

//-----------------------------------------------------------------------------
std::vector<unsigned> Anp::ScaleBG::UpdateFilters(const TrigMonConfig &config, std::ostream &os)
{

  std::vector<unsigned> filter_counters(0);
  //
  // Get HLT counters for the filter list
  //
  for(unsigned i = 0; i < fFilters.size(); ++i) {
    std::string filter_name = fFilters.at(i);

    // 1) find EF trigger
    const vector<TrigConfChain>::const_iterator it_ef = config.findChain(filter_name);
    if(it_ef == config.end<TrigConfChain>()) {
      os << "UpdateHLT - failed to find EF filter trigger: " << filter_name << endl;
      return filter_counters;
    }
    filter_counters.push_back(it_ef->getCounter());
    os << "Filter " << filter_name << " -- Counter: " << it_ef->getCounter() << endl;
  }
  
  return filter_counters;
}
//-----------------------------------------------------------------------------
Anp::ScaleBG::ScaleBG()
  :fDebug(false),
   fIsConfig(false),
   fIsCAFReprocess(false),
   fReadConfigXML(false),
   fPredRun(0),
   fPredLB(0),
   fCountPass(0),
   fCountFail(0),
   fCountSignal(0),
   fCurrLB(0),
   fDataLiveFrac(1.0),
   fDataLumi(0.0),
   fPredLumi(0.0),
   fDecisionBitHLT(TrigMonChain::kPassedRaw)
{
}

//-----------------------------------------------------------------------------
Anp::ScaleBG::~ScaleBG()
{
}

//-----------------------------------------------------------------------------
void Anp::ScaleBG::Config(const Registry &reg)
{
  //
  // Configure self and register input data
  //
  string pred_lumi_alg, data_lumi_alg;

  reg.Get("ScaleBG", "Debug",     fDebug);
  reg.Get("ScaleBG", "XMLPath",   fXMLPath);
  //  fDebug = true;

  //
  // Read new bunch group info once
  //  
  ParseXML(fXMLPath);

  //
  // Read configuration - can override values set in XML
  //
  reg.Get("ScaleBG::DataLiveFrac", fDataLiveFrac);
  reg.Get("ScaleBG::DataLumi",     fDataLumi);
  reg.Get("ScaleBG::PredLumi",     fPredLumi);

  reg.Get("ScaleBG::DataLumiAlg",  data_lumi_alg);
  reg.Get("ScaleBG::PredLumiAlg",  pred_lumi_alg);
  reg.Get("ScaleBG::PredRun",      fPredRun);
  reg.Get("ScaleBG::PredLB",       fPredLB);
  reg.Get("ScaleBG::CAFReprocess", fIsCAFReprocess);
  reg.Get("ScaleBG::ReadXML_EBConfig", fReadConfigXML);

  // Read PS file, if specified, fill BG information
  string ps_path;
  if(reg.Get("ScaleBG::ReadXML_PS", ps_path)) {
    BGMap bunch_map = Trig::BunchXMLparser(ps_path, fDebug);
   if(!bunch_map.empty()){
     fBGPred = bunch_map;
   }
  }


  //
  // Register input data
  //
  fEntry = NtupleSvc::Instance("event").Branch<TrigMonEvent>("data");
  fOnlineEntry = NtupleSvc::Instance("online").Branch<TrigMonEvent>("online");

  //
  // Register with trigger configuration algorithm
  //
  Anp::RegisterCallback(reg, "ScaleBG::DataConfAlg", fDataConfAlg, this);
  Anp::RegisterCallback(reg, "ScaleBG::PredConfAlg", fPredConfAlg, this);

  //
  // Register self with factory
  //
  Factory<ScaleABC>::Instance().Hold(AlgEvent::GetName(), Handle<ScaleABC>(this));

  //
  // Retrieve luminosity algorithm(s)
  //
  if(!data_lumi_alg.empty()) {
    fDataLumiAlg = Factory<LumiABC>::Instance().Get(data_lumi_alg);
    
    if(fDataLumiAlg.valid()) {
      log() << "Config - got lumi algorithm for input data: " << data_lumi_alg << endl;
    }
  }
  if(!pred_lumi_alg.empty()) {
    fPredLumiAlg = Factory<LumiABC>::Instance().Get(pred_lumi_alg);
 
    if(fPredLumiAlg.valid()) {
      log() << "Config - got lumi algorithm for prediction: " << pred_lumi_alg << endl;
    }
  }

}

//-----------------------------------------------------------------------------
void Anp::ScaleBG::Signal(const Registry &reg, const string &signal)
{
  //
  // Handle callback for trigger configuration
  //
  if(signal != "TRIG_CONFIG_DB") return;

  string config_source;
  reg.Get("TRIG_CONFIG_SOURCE", config_source);

  if(++fCountSignal < 10) {
    log() << "Signal - got configuration algorithm: " << config_source << endl;
  }

  if(fDataConfAlg.valid() && fDataConfAlg->GetName() == config_source) {
    //
    // Read online trigger configuration used to record input data
    //
  
    fFiltersCounters = UpdateFilters(fDataConfAlg->GetConfig(), std::cout);

    for(unsigned i = 0; i < fHLT.size(); ++i) {
      //
      // Set PS for LV1 and HLT triggers for EB filters/weights
      //
      HLTData &data = fHLT.at(i);
      
      // get L1 config from Hypo if needed
      data.ResetTriggers(fReadConfigXML);
      data.GetLVL1Seeds(fDataConfAlg->GetConfig(), fReadConfigXML, std::cout); 
      data.GetHypoSeeds(fDataConfAlg->GetConfig(), fReadConfigXML, std::cout);
      data.UpdateHLT(fDataConfAlg->GetConfig(), std::cout);
      
      // update HLT information for any same seeded EB triggers
      for(unsigned i = 0; i < data.hlt_same_seeds.size(); ++i) {
	HLTData &same_seeds = data.hlt_same_seeds.at(i);
	same_seeds.ResetTriggers(fReadConfigXML);
	same_seeds.GetHypoSeeds(fDataConfAlg->GetConfig(), fReadConfigXML, std::cout);
	same_seeds.UpdateHLT(fDataConfAlg->GetConfig(), std::cout);
      }

      if(fCountSignal < 10) {
	data.PrintTriggers();
      }
    }
    
    //
    // Save BG configuration for input data
    //
    fBGData = fDataConfAlg->GetBG();     
    fIsConfig = true;
  }
  
  if(fPredConfAlg.valid() && fPredConfAlg->GetName() == config_source) {
    //
    // Read bunch group configuration for prediction run
    //
    fBGPred = fPredConfAlg->GetBG();
  }

  fPredBGSize.clear();

  BGMap::const_iterator bit_pred = fBGPred.begin();
  for(; bit_pred != fBGPred.end(); ++bit_pred) {
    const Trig::BunchGroup &bg_pred = bit_pred->second;
    fPredBGSize.push_back(bg_pred.size);

    //    bg_pred.PrintBunchGroup();
  }



  return;
}

//-----------------------------------------------------------------------------
bool Anp::ScaleBG::Init()
{
  //  
  // Read luminosity and bunch group config for predicted run number
  //  - use run number specidfied on command line: --pred-run=fPredRun
  //
  if(fPredLumiAlg.valid() && fPredRun > 0 && fPredLB > 0) {

    Lumi::BlockInfo info;

    if(!fPredLumiAlg->FillBlockInfo(fPredRun, fPredLB, info)) {
      log() << "Init - lumi block does not exist: " << fPredLB << endl;
    }
    else {
      //
      // Rescale luminosity cm^-2 s^-1 units
      //
      fPredLumi = 1.0e30*info.lumiInstan;
    }

    log() << "Init - target for prediction: " << endl
	  << "   run: " << fPredRun << endl
	  << "   lb:  " << fPredLB  << endl;
  }
  
  log() << "Init - initial luminosity parameters:" << endl
	<< "   pred. lumi: " << fPredLumi     << endl
	<< "   input lumi: " << fDataLumi     << endl
	<< "   livetime f: " << fDataLiveFrac << endl;

  return true;
}

//-----------------------------------------------------------------------------
void Anp::ScaleBG::Exec()
{
  //
  // Process event: compute weight - used by clients
  //
  if(fEntry.valid() && 
     fEntry.GetState() == Ntuple::kREAD &&
     fEntry.GetEvent() == Ntuple::kPASS) {    
    
    if(Run(fEntry.GetRef())) {
      ++fCountPass;
    }
    else {
      fEntry.SetEvent(Ntuple::kFAIL);
      ++fCountFail;
    }
  }
}

//-----------------------------------------------------------------------------
void Anp::ScaleBG::Done()
{
  log() << "ScaleBG - print summary: " << endl
	<< "   Count passed: " << fCountPass            << endl
	<< "   Count failed: " << fCountFail            << endl
	<< "   Count total:  " << fCountPass+fCountFail << endl
	<< "   Count signal: " << fCountSignal          << endl;
}

//-----------------------------------------------------------------------------
bool Anp::ScaleBG::Run(const TrigMonEvent &event)
{
  //
  // Compute event weight
  //
  if(!fIsConfig) { 
    log() << "Run - error! Missing configuration information" << endl;
    return false;
  }

  //
  // Calculate scale factor - default is 0, only use events which pass triggers on filter list
  //
  fScaleBCID.scale_lumi = 0.0;
  fScaleBCID.scale_rd0 = 0.0;
  fScaleBCID.scale_rd1 = 0.0;

  //
  // Read data for current lumi block: luminosity, etc
  //
  ReadCurrentData(event);

  //
  // Check that luminosity values are sensible
  //
  if(!(fDataLumi > 0.0) && !(fPredLumi > 0.0)) {
    log() << "Run - invalid luminosity parameters" << endl;
    return false;
  }

  //
  // Check that event passes required triggers and compute filter weight
  //
  double weight_filter = 0.0, weight_filter_rd0 = -1.0, weight_filter_rd1 = -1.0;

  bool filter_pass = false;
  if(fReadConfigXML){
    filter_pass = FilterTriggers(event, weight_filter, weight_filter_rd0, weight_filter_rd1);
  } else {
    filter_pass = FilterTriggersOnline(event, weight_filter, weight_filter_rd0, weight_filter_rd1);
  }

  if(!filter_pass) {
    if(fDebug) log() << "Run - failed filters" << endl;
    return false;
  }
  
  if(fDebug) {
    log() << "Run - passed filters" << endl;
  }
  
  if(!(weight_filter > 0.0)) {
    if(fDebug) {
      log() << "Run - logic error! Event passed trigger but has zero weight" << endl;
    }
    return true;
  }

  //
  // Determine the bunch group for this BCID
  //
  BGMap::const_iterator bit_data = fBGData.begin();

  for(; bit_data != fBGData.end(); ++bit_data) {
    const Trig::BunchGroup &bg = bit_data->second;

    if(bg.bcids.count(event.getBunchId()) && bg.keynum != 0) {
      break;
    }
  }
  
  if(bit_data == fBGData.end()) {
    log() << "Run - uknown BCID: " << event.getBunchId() << endl;
    return false;
  }

  if(fDebug) {
    log() << "Run - bcid=" << event.getBunchId() << " bunch group=" << bit_data->second.key << endl;
  }
  
  //
  // Compute event weight using:
  //   - triggers which passed this event
  //   - bunch group for this event
  //

  //
  // Event fired on FILLED bunch group: 
  //   - scale by lumi and filter eff
  //
  fScaleBCID.scale_lumi = fPredLumi/(fDataLumi*weight_filter);

  //
  // Event fired in non-FILLED bunch group
  //   - scale by bunch group size and filter eff
  //
  const BGMap::const_iterator bit_pred = fBGPred.find(bit_data->first);
  if(bit_pred == fBGPred.end()) {
    //
    // No data for size of predicted bunch group - use same BG size as before
    //
    fScaleBCID.scale_rd0 = 1.0/weight_filter;
    fScaleBCID.scale_rd1 = 1.0/weight_filter;
  }
  else {
    //
    // Scale event as ratio of bunch group sizes
    //
    const Trig::BunchGroup &bg_data = bit_data->second;
    const Trig::BunchGroup &bg_pred = bit_pred->second;
    if(bg_data.size > 0.0) {
      // for RD0 seeded triggers, the weight must used an unbias random stream
      if(weight_filter_rd0 > 0.0){
	fScaleBCID.scale_rd0 =double(bg_pred.size)/double(bg_data.size*weight_filter_rd0);
      } else{
	fScaleBCID.scale_rd0 = 0.0;
      }

      if(weight_filter_rd1 > 0.0){
	fScaleBCID.scale_rd1 =double(bg_pred.size)/double(bg_data.size*weight_filter_rd1);
      } else{
	fScaleBCID.scale_rd1 = 0.0;
      }
      
      // for non-lumi bunches also scale by bunch group
      if(bit_data->first != "BGRP1") {
	fScaleBCID.scale_lumi =double(bg_pred.size)/double(bg_data.size*weight_filter);
      }
    }
    else {
      //
      // Data bunch group has no bunches - logic error
      //
      log() << "Run - bunch group is empty: " << bg_data.key << endl;
      return false;	
    }
  }

  //
  // Scale by overall livetime fraction
  //
  if(fDataLiveFrac > 0.0) {
    fScaleBCID.scale_lumi /= fDataLiveFrac;
    fScaleBCID.scale_rd0 /= fDataLiveFrac;
    fScaleBCID.scale_rd1 /= fDataLiveFrac;
  }

  if(fDebug) {
    log() << "Run - scaleBCID (lumi, rd0, rd1)=" 
	  << fScaleBCID.scale_lumi << ", "
	  << fScaleBCID.scale_rd0 << ", " 
	  << fScaleBCID.scale_rd1 << endl
	  << "   weight_filter = " << weight_filter << endl
	  << "   PredLumi      = " << fPredLumi     << endl
	  << "   DataLumi      = " << fDataLumi     << endl;
  }

  return true;
}

//-----------------------------------------------------------------------------
bool Anp::ScaleBG::FilterTriggers(const TrigMonEvent &event, double &weight, double &weight_rd0, double &weight_rd1)
{
  //
  // 1) check that filter triggers passed
  // 2) compute event (filter) weight using LV1 and HLT prescales
  //
  unsigned count_triggers = 0;

  //
  // Set L1 bits
  //
  std::bitset<256> lv1_tbp;
  std::bitset<256> lv1_tap;

  lv1_tbp.reset();
  lv1_tap.reset();

  for(unsigned i = 0; i < event.getL1Item().size(); ++i) {
    const TrigMonL1Item item(event.getL1Item().at(i));
    
    lv1_tbp.set(item.getCtpId(), item.isPassed(TrigMonL1Item::kL1BeforePrescale));
    lv1_tap.set(item.getCtpId(), item.isPassed(TrigMonL1Item::kL1AfterPrescale));
  }

  //
  // Count LV1 triggers and collect total LV1 weight for this HLT trigger
  //

  for(unsigned h = 0; h < fHLT.size(); ++h) {
    const HLTData &data = fHLT.at(h);

    // check that the run number is in range
    // runumber == 0 means used for all runs
    if(data.runnumber_low != 0 && data.runnumber_high != 0 && 
       (event.getRun() < data.runnumber_low || event.getRun() > data.runnumber_high)) { 
      continue;
    }

    double weight_lv1 = 0.0;
    bool pass_lv1 = false;

    if(!fIsCAFReprocess){

      //
      // Check that L1 filters passed
      //    
      for(unsigned l = 0; l < data.lv1_filter.size(); ++l) {
	const LV1Data &lv1 = data.lv1_filter.at(l);
	if(lv1_tap.test(lv1.ctp_id)) {
	  pass_lv1 = true;
	  break;
	}
      }

      //
      // Require that at least one trigger on LV1 filter list passed event
      //
      if(!pass_lv1) continue;

    }


    //
    // Compute combined L1 weight
    //
    if(data.lv1_ebtrigs.size() == 0) {
      for(unsigned l = 0; l < data.lv1_weight.size(); ++l) {
	const LV1Data &lv1 = data.lv1_weight.at(l);
	if(lv1_tbp.test(lv1.ctp_id)){
	  weight_lv1 = weight_lv1 + lv1.lv1_weight*(1-weight_lv1);
	}
      }

      //
      // Scale up by HLT weight and compute total weight
      //
      weight_lv1 *= data.hlt_weight;
      weight = weight + weight_lv1*(1.0-weight);
      if(weight_rd0 < 0.0) weight_rd0 = weight;
      // Multiply by randomweight because for non-eb items, RD1 trigger can only come in via the RD0 stream 
      // and the overlap is 1/data.l1_rndweight
      if(weight_rd1 < 0.0) weight_rd1 = weight*data.l1_rndweight;
    }
    // compute the HLT weight for the EB runs where there are two random streams
    else {

      // loop over the EB threshold and calculate total hlt weight
      double weight_hlt = 0.0;
      vector<string> found_eb_chains(0);

      for(unsigned l = 0; l < data.lv1_ebtrigs.size(); ++l) {
	const LV1Data &lv1 = data.lv1_ebtrigs.at(l);
	if(lv1_tbp.test(lv1.ctp_id)){
	  if(find(found_eb_chains.begin(), found_eb_chains.end(), lv1.hlt_chain_name) == found_eb_chains.end()){
	    found_eb_chains.push_back(lv1.hlt_chain_name);
	    weight_hlt = weight_hlt + lv1.hlt_chain_weight*(1.0-weight_hlt);
	  }
	}
      }

      // If there is an EB trigger in the list that fires, the event weight is the 
      // combination of the EF trigger and the ef_unbiastrig (in otherwords the EF_EB chain and the MBTS EF chain)
      // If there is NO EB trigger, then the event weight is the unbias MBTS chain weight

      double weight_eb = data.hlt_eb_unbias_weight;
      for(unsigned l = 0; l < data.lv1_ebtrigs.size(); ++l) {
	const LV1Data &lv1 = data.lv1_ebtrigs.at(l);
	if(lv1_tbp.test(lv1.ctp_id)){
	  weight_eb = weight_eb + (data.l1_rndweight*data.l1_eb_weight*weight_hlt)*(1-weight_eb);
	  break;
	}
      } // end of EB L1 loop
      weight = weight + weight_eb*(1.0-weight);

      // The RD0 stream is biased (since it seeds an EB HLT trigger), therefore a weight must be calculated specially for it
      // For RD1 seeded triggers, the weight should be 1/PS_RD1, therefore need to remove the rndweight
      // select events where RD1 has fired
      weight_rd0 = 0.0;
      weight_rd1 = 0.0;
      if(lv1_tap.test(data.l1_eb_unbias_counter)) {
	weight_rd0 = data.hlt_eb_unbias_weight;
	weight_rd1 = data.hlt_eb_unbias_weight/data.l1_rndweight;
      }
      
    }

    ++count_triggers;
  }

  return count_triggers > 0;
}

//-----------------------------------------------------------------------------
bool Anp::ScaleBG::FilterTriggersOnline(const TrigMonEvent &event, double &weight, double &weight_rd0, double &weight_rd1)
{
  //
  // FilterTriggersOnline differs from FilterTrigger in that this routine uses the tiny ntuples
  // to read the configuration, whereas FilterTrigger uses the configuration read from XML
  //
  unsigned count_triggers = 0;
  bool set_rd_weights = true;

  //
  // Access the online trigger information from the tiny ntuple
  //
  if(!(fOnlineEntry.valid() && 
       fOnlineEntry.GetState() == Ntuple::kREAD &&
       fOnlineEntry.GetEvent() == Ntuple::kPASS)) { 
    return count_triggers;
  }

  // Determine what HLT items fires (only if filter is required)
  std::vector<unsigned> passing_hlt_chains;
  const TrigMonEvent &event_online = fOnlineEntry.ref();
  const std::vector<uint32_t> &online_hlt = event_online.getChain();
  for(unsigned i = 0; i < online_hlt.size(); ++i) {
    const TrigMonChain chain(online_hlt.at(i));
    if(chain.isPassed(fDecisionBitHLT)) {
      passing_hlt_chains.push_back(chain.getCounter());
    }
  }

  // Add in a required filter here
  bool pass_filter = false;
  for(unsigned f = 0; f < fFiltersCounters.size(); ++f) {
    if(std::find(passing_hlt_chains.begin(), passing_hlt_chains.end(), fFiltersCounters.at(f)) != passing_hlt_chains.end())
      pass_filter = true;
  }
  if(fFiltersCounters.size() > 0 && !pass_filter){
    return count_triggers;  
  }

  // Get L1 online information
  //
  std::bitset<256> lv1_tbp;
  std::bitset<256> lv1_tav;

  lv1_tbp.reset();
  lv1_tav.reset();

  for(unsigned i = 0; i < event_online.getL1Item().size(); ++i) {
    const TrigMonL1Item item(event_online.getL1Item().at(i));
    
    lv1_tbp.set(item.getCtpId(), item.isPassed(TrigMonL1Item::kL1BeforePrescale));
    lv1_tav.set(item.getCtpId(), item.isPassed(TrigMonL1Item::kL1AfterVeto));
  }

  //
  // Count LV1 triggers and collect total LV1 weight for this HLT trigger
  //
  for(unsigned h = 0; h < fHLT.size(); ++h) {
    const HLTData &data = fHLT.at(h);

    // check that the run number is in range
    // runumber == 0 means used for all runs
    if(data.runnumber_low != 0 && data.runnumber_high != 0 && 
       (event.getRun() < data.runnumber_low || event.getRun() > data.runnumber_high)) { 
      continue;
    }

    //
    // For all triggers except HLT triggers which have the same L1 seed as another HLT EB trigger (i.e. RD0_FILLED seeded chains)
    //
    if(data.hlt_same_seeds.size() == 0) {

      // Get all L1 weights from online decision
      double weight_lv1 = 0.0;
      for(unsigned l = 0; l < data.lv1_weight.size(); ++l) {
 	const LV1Data &lv1 = data.lv1_weight.at(l);
 	if(lv1_tbp.test(lv1.ctp_id)){
 	  weight_lv1 = weight_lv1 + lv1.lv1_weight*(1-weight_lv1);
 	}
      }

      //
      // Scale up by HLT weight and compute total weight
      //
      weight_lv1 *= data.hlt_weight;
      weight = weight + weight_lv1*(1.0-weight);
      if(set_rd_weights) {
	weight_rd0 = weight;
	// Multiply by randomweight because for non-eb items, RD1 trigger can only come in via the RD0 stream 
	// and the overlap is 1/data.l1_rndweight
	weight_rd1 = weight*data.l1_rndweight;
      }
    }

    // compute the HLT weight for the EB runs which have the same L1 seed (including eb_random)
    // eb_physics, high_eb_physics and eb_random must be treated as a group to account for correlations
    // these are called the 'eb chains' here
    // eb_physics_noL1PS can be calculated using the scaling above
    else {

      // determine if one of the EB chains fire (necessary to remove non-eb chain items)
      // this is not needed above because the weight is only calculated 
      // for items which pass before PS (otherwise the weight is zero)
      // In this case the eb_random weight is applied for all events where one of the eb chains passed

      // HLT weight for the EB trigger (if passed)
      double weight_hlt = 0.0;

      // main EB item
      vector<double> weight_eb_chains(0);
      // loop over all L1 items for the main EB trigger
      for(unsigned l = 0; l < data.lv1_weight.size(); ++l) {
	const LV1Data &lv1 = data.lv1_weight.at(l);
        if(lv1_tbp.test(lv1.ctp_id)){
	  weight_hlt = weight_hlt + data.hlt_weight*(1.0-weight_hlt);
	  weight_eb_chains.push_back(data.l1_rndweight*data.l1_eb_weight);
	  break;
	}
      }      
	
      // loop over the additional same seed EB items and calculate total hlt weight
      for(unsigned i = 0; i < data.hlt_same_seeds.size(); ++i) {
	const HLTData &same_seeds = data.hlt_same_seeds.at(i);
	for(unsigned l = 0; l < same_seeds.lv1_weight.size(); ++l) {
	  const LV1Data &lv1 = same_seeds.lv1_weight.at(l);
	  if(lv1_tbp.test(lv1.ctp_id)){
	    weight_hlt = weight_hlt + same_seeds.hlt_weight*(1.0-weight_hlt);
	    weight_eb_chains.push_back(data.l1_rndweight*same_seeds.l1_eb_weight);
	    break;
	  }
	}
      }
	
      // require that at least one passes
      if(weight_eb_chains.size() == 0 && !lv1_tbp.test(data.l1_eb_unbias_counter)) continue;

      // Now combine the L1 probability 
      double weight_eb = data.hlt_eb_unbias_weight;
      if(weight_eb_chains.size() > 0) {
	weight_eb = weight_eb + (weight_eb_chains.at(0)*weight_hlt)*(1.0-weight_eb);
      }

      weight = weight + weight_eb*(1.0-weight);

      // The RD0 stream is biased (since it seeds an EB HLT trigger), therefore a weight must be calculated specially for it
      // For RD1 seeded triggers, the weight should be 1/PS_RD1, therefore need to remove the rndweight
      // select events where RD1 has fired
      set_rd_weights = false; // so that weights do not get reset
      weight_rd0 = 0.0;
      weight_rd1 = 0.0;
      if(lv1_tav.test(data.l1_eb_unbias_counter)){
	weight_rd0 = data.hlt_eb_unbias_weight;
 	weight_rd1 = data.hlt_eb_unbias_weight/data.l1_rndweight;
      }
    }

    ++count_triggers;
  }

  return count_triggers > 0;
}



//-----------------------------------------------------------------------------
void Anp::ScaleBG::ReadCurrentData(const TrigMonEvent &event)
{
  //
  // Read data for current run, lumi block
  //
  if(!fDataLumiAlg.valid()) {
    return;
  }

  if(fCurrLB == event.getLumi()) {
    return;
  }
  
  fCurrLB = event.getLumi();
  
  //
  // Retrieve average instantenois luminosity for current lumi block
  //
  Lumi::BlockInfo info;
  
  if(!fDataLumiAlg->FillBlockInfo(event.getRun(), fCurrLB, info)) {
    log() << "ReadCurrentData - missing lumi block: " << fCurrLB << endl;
    return;
  }

  //
  // Rescale luminosity cm^-2 s^-1 units
  //
  fDataLumi = 1.0e30*info.lumiInstan;

  if(fDebug) {
    log() << "ReadCurrentData - updated luminosity:" << endl
	  << "   lumi block: " << fCurrLB       << endl
	  << "   pred. lumi: " << fPredLumi     << endl
	  << "   input lumi: " << fDataLumi     << endl
	  << "   livetime f: " << fDataLiveFrac << endl;
  }
}

//----------------------------------------------------------------------------------
void Anp::ScaleBG::ParseXML(const std::string &fname)
{
  //
  // Read the brunch group info from XML file
  //
  if(fname.empty()) {
    log() << "ParseXML - empty input file name" << endl;
    return;
  }

  log() << "ParseXML - reading configuration from XML file:\n   " << fname << endl;
  
  Anp::Registry xml_reg = Anp::ParseXml(fname, fDebug);
    
  while(xml_reg.KeyExists("bunchgroup")) {
    //
    // Read and then erase new "bunch" key
    //
    Anp::Registry bunch_reg;
    xml_reg.Get("bunchgroup", bunch_reg);
    xml_reg.RemoveKey("bunchgroup");
    
    if(fDebug) {
      cout << "  read new \"bunchgroup\" key" << endl;
    }
    
    while(bunch_reg.KeyExists("bunchgrouptype")) {
      //
      // Read and then erase new "group" key
      //
      Anp::Registry grp_reg;
      bunch_reg.Get("bunchgrouptype", grp_reg);
      bunch_reg.RemoveKey("bunchgrouptype");

      Trig::BunchGroup data;
      
      if(grp_reg.Get("bunchgroup_key",    data.key)  && 
	 grp_reg.Get("bunchgroup_name",   data.name) && 
	 grp_reg.Get("bunchgroup_size",   data.size) &&
	 grp_reg.Get("bunchgroup_keynum", data.keynum)) {
	fBGPred[data.key] = data;
      }
      else {
	cout << "  failed to extract key \"bunchgrouptype\"" << endl;
      }
    }
  }

  //
  // Read from XML the input data and predicted lumi values
  //    
  while(xml_reg.KeyExists("lumivalues")) {
    //
    // Read and then erase new "lumi" key
    //
    Anp::Registry lumi_reg;
    xml_reg.Get("lumivalues", lumi_reg);
    xml_reg.RemoveKey("lumivalues");
    
    if(fDebug) {
      cout << "  read new \"lumivalues\" key" << endl;
    }
    
    //
    // Read the lumi values
    //
    lumi_reg.Get("livetime_frac",   fDataLiveFrac);
    lumi_reg.Get("lumivalues_data", fDataLumi);
    lumi_reg.Get("lumivalues_pred", fPredLumi);
  }
  
  //
  // Read from XML triggers used to for the tiny ntuple
  //  Alternatively, the EB configuration can be read from XML (see below)
  //
  while(xml_reg.KeyExists("eb")) {
    //
    // Read and then erase new "eb_hlt_items" key
    //
    Anp::Registry eb_reg;
    xml_reg.Get("eb", eb_reg);
    xml_reg.RemoveKey("eb");

    unsigned runnumber_low  = 0;
    unsigned runnumber_high = 0;
    
    eb_reg.Get("runnumber_low",  runnumber_low);
    eb_reg.Get("runnumber_high", runnumber_high);

    double rnd_rate = 1.0;
    if(eb_reg.Get("eb_rndrate", rnd_rate))
      rnd_rate = rnd_rate/40.0e6;

    // get the list of HLT filters
    while(eb_reg.KeyExists("eb_filter")) {
    
      string hlt_filter;
      eb_reg.Get("eb_filter", hlt_filter);
      eb_reg.RemoveKey("eb_filter");
	
      if(!hlt_filter.empty())    
	fFilters.push_back(hlt_filter);
    }

    while(eb_reg.KeyExists("eb_hlt_items")) {
      Anp::Registry eblist_reg;
      eb_reg.Get("eb_hlt_items", eblist_reg);
      eb_reg.RemoveKey("eb_hlt_items");

      string hlt_name;
      eblist_reg.Get("eb_hlt_name",  hlt_name);

      string ef_unbiastrig;
      eblist_reg.Get("eb_random_chain", ef_unbiastrig);

      HLTData hlt;
      hlt.filter_name = hlt_name;
      hlt.l1_rndweight = rnd_rate;
      hlt.runnumber_low = runnumber_low;
      hlt.runnumber_high = runnumber_high;
      hlt.ef_unbiastrig = ef_unbiastrig;
	
      std::vector<HLTData> hlt_ebtrigs_vec;
      while(eblist_reg.KeyExists("eb_same_seed")) {
	Anp::Registry ebss_reg;
	eblist_reg.Get("eb_same_seed", ebss_reg);
	eblist_reg.RemoveKey("eb_same_seed");

	string hlt_ebtrigs;
	ebss_reg.Get("eb_ss_name", hlt_ebtrigs);
	ebss_reg.RemoveKey("eb_ss_name");
	
	HLTData hlt_same_seed;
	hlt_same_seed.filter_name = hlt_ebtrigs;
	hlt_same_seed.l1_rndweight = rnd_rate;
	hlt_same_seed.runnumber_low = runnumber_low;
	hlt_same_seed.runnumber_high = runnumber_high;
	
	hlt.hlt_same_seeds.push_back(hlt_same_seed);
      }

      fHLT.push_back(hlt);
    }
  }

  //
  // Read from XML triggers used to filter the sample in use
  // This is used when readying the EB configuration from XML
  // Alternatively the EB configuration can be read via the tiny ntuple
  //    
  while(xml_reg.KeyExists("filters")) {
    //
    // Read and then erase new "filter" key
    //
    Anp::Registry filters_reg;
    xml_reg.Get("filters", filters_reg);
    xml_reg.RemoveKey("filters");
    
    while(filters_reg.KeyExists("filter")) {
      //
      // Read and then erase new "filter" key
      //
      Anp::Registry filter_reg;
      filters_reg.Get("filter", filter_reg);
      filters_reg.RemoveKey("filter");

      //
      // Read one HLT filter
      //
      HLTData hlt;

      if(!filter_reg.Get("filter_name", hlt.filter_name) ||
	 !filter_reg.Get("filter_type", hlt.filter_type)) {
	continue;
      }

      double rnd_rate = 1.0;
      if(filter_reg.Get("filter_rndrate", rnd_rate))
	hlt.l1_rndweight = rnd_rate/40.0e6;
      else hlt.l1_rndweight = 1.0;

      hlt.runnumber_low  = 0;
      hlt.runnumber_high = 0;

      filter_reg.Get("filter_runnumber_low",  hlt.runnumber_low);
      filter_reg.Get("filter_runnumber_high", hlt.runnumber_high);

      std::string ef_unbias;
      if(filter_reg.Get("ef_lowertrig", ef_unbias))
	hlt.ef_unbiastrig = ef_unbias;
      
      while(filter_reg.KeyExists("lv1_filter")) {
	//
	// Read and then erase new "filter" key
	//
	string lv1_filter;
	filter_reg.Get("lv1_filter", lv1_filter);
	filter_reg.RemoveKey("lv1_filter");
	
	LV1Data lv1;
	lv1.lv1_name = lv1_filter;

	if(!lv1.lv1_name.empty()) {
	  hlt.lv1_filter.push_back(lv1);
	}
      }
      while(filter_reg.KeyExists("lv1_weight")) {
	//
	// Read and then erase new "weight" key
	//
	string lv1_weight;
	filter_reg.Get("lv1_weight", lv1_weight);
	filter_reg.RemoveKey("lv1_weight");
	
	LV1Data lv1;
	lv1.lv1_name = lv1_weight;

	if(!lv1.lv1_name.empty()) {
	  hlt.lv1_weight.push_back(lv1);
	}
      }

      std::map < std::string, std::vector<LV1Data> > eb_chains;
      while(filter_reg.KeyExists("eb_hlt_chains")) {
	//
	// Read and then erase new "eb_hlt_chains" key
	//
	Anp::Registry eb_chain_reg;
	filter_reg.Get("eb_hlt_chains", eb_chain_reg);
	filter_reg.RemoveKey("eb_hlt_chains");

 	string eb_hlt_name;
	if(!eb_chain_reg.Get("eb_hlt_name", eb_hlt_name))
	  continue;

	std::vector<LV1Data> lv1_ebtrigs_vec;
	while(eb_chain_reg.KeyExists("lv1_ebtrigs")) {
	  //
	  // Read and then erase new "filter" key
	  //
	  string lv1_ebtrigs;
	  eb_chain_reg.Get("lv1_ebtrigs", lv1_ebtrigs);
	  eb_chain_reg.RemoveKey("lv1_ebtrigs");
	
	  LV1Data lv1;
	  lv1.lv1_name = lv1_ebtrigs;
	  lv1.hlt_chain_name = eb_hlt_name;

	  if(!lv1.lv1_name.empty()) {
	    hlt.lv1_ebtrigs.push_back(lv1);
	  }
	}
      }

      if(!hlt.lv1_filter.empty()) {
	fHLT.push_back(hlt);
      }
      else {
	log() << "ReadXML - failed to correctly parse HLT filter: " << hlt.filter_name << endl;
      }
    }
  }

  // print information about the HLT filters (if any)
  if(fFilters.size() > 0) {
    log() << "Requiring HLT Filters: "; 
    for(unsigned i = 0; i < fFilters.size(); ++i) {
      cout << fFilters.at(i) << ", ";
    }
    cout << endl;
  }

  log() << "ReadXML - read " << fHLT.size() << " EB HLT trigger(s) Config" << endl;
  for(unsigned i = 0; i < fHLT.size(); ++i) {
    const HLTData &hlt = fHLT.at(i);
    hlt.PrintTriggers();
  }
}
