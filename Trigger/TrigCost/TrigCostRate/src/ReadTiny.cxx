/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// C/C++
#include <string>
#include <iostream>

// ROOT
#include "TFile.h"
#include "TTree.h"
#include "TString.h"

// Local
#include "TrigCostBase/Factory.h"
#include "TrigCostBase/EventSvc.h"
#include "TrigCostBase/LumiABC.h"
#include "TrigCostBase/UtilCore.h"
#include "TrigCostBase/UtilHack.h"
#include "TrigCostBase/UtilTrig.h"
#include "TrigMonitoringEvent/TrigMonEvent.h"

// Local
#include "TrigCostRate/ReadTiny.h"

REGISTER_ANP_OBJECT(AlgEvent,ReadTiny)

using namespace std;

//---------------------------------------------------------------------------------------
Anp::ReadTiny::VertexData::VertexData():
  dVtx_x   (-1),
  dVtx_y   (-1),
  dVtx_z   (-1),
  dVtx_chi2(-1),
  dSumPt   (-1),
  dVtx_ndof(-1),
  dNTrack  (-1)
{
}

//---------------------------------------------------------------------------------------
Anp::ReadTiny::EventData::EventData():
  dRunNumber   (-1),
  dLumiblock   (-1),
  dEventNumber (-1),
  dBunchId     (-1),
  dTimestamp   (-1),
  dTimestamp_ns(-1),
  dNVtx        (-1)
{
}

//---------------------------------------------------------------------------------------
Anp::ReadTiny::RunData::RunData():
  isConfig       (false),
  dHLTPrescaleKey(-1),
  dLV1PrescaleKey(-1),
  dMasterKey     (-1)
{
}

//---------------------------------------------------------------------------------------
Anp::ReadTiny::ReadTiny()
  :fDebug(false),
   fTreeNameEvent ("eb_event"),
   fTreeNameConfig("eb_config"),
   fRunNumber      (-1),
   fLumiblock      (-1),
   fEventNumber    (-1),
   fBunchId        (-1),
   fTimestamp      (-1),
   fTimestamp_ns   (-1),
   fNVtx           (-1),   
   fVtx_x          (new std::vector<Float_t>),
   fVtx_y          (new std::vector<Float_t>),
   fVtx_z          (new std::vector<Float_t>),
   fVtx_chi2       (new std::vector<Float_t>),
   fVtx_ndof       (new std::vector<Int_t>),
   fEventLV1       (new std::vector<uint16_t>),
   fEventHLT       (new std::vector<uint32_t>),
   fHLTPrescaleKey (-1),
   fLV1PrescaleKey (-1),
   fMasterKey      (-1),    
   fTrigNames      (new std::vector<std::string>),
   fTrigIds        (new std::vector<uint32_t>),
   fNFailedUpdate(0),
   fNFailedBCIDUpdate(0),
   fNNonoverlappingVtx(-1),
   fNFailedMu(0),
   fCounter(0),
   fT0(time(NULL))
{
}

Anp::ReadTiny::~ReadTiny(){
   if(fVtx_x    ) delete fVtx_x     ;
   if(fVtx_y    ) delete fVtx_y     ;
   if(fVtx_z    ) delete fVtx_z     ;
   if(fVtx_chi2 ) delete fVtx_chi2  ;
   if(fVtx_ndof ) delete fVtx_ndof  ;
   if(fEventLV1 ) delete fEventLV1  ;
   if(fEventHLT ) delete fEventHLT  ;
   if(fTrigNames) delete fTrigNames ;
   if(fTrigIds  ) delete fTrigIds   ;
}

//---------------------------------------------------------------------------------------
void Anp::ReadTiny::Config(const Registry &reg)
{
  //
  //
  //
  log() << "Configuring ReadTiny..." << endl;
  
  reg.Get("ReadTiny::Debug"         , fDebug          );
  reg.Get("ReadTiny::TreeNameEvent" , fTreeNameEvent  );
  reg.Get("ReadTiny::TreeNameConfig", fTreeNameConfig );
  reg.Get("ReadTiny::eosXMLPath"    , fEosXmlPath     );
  reg.Get("ReadTiny::MuXMLPath"     , fMuXmlPath      );
  reg.Get("ReadTiny::BCIDXMLPath"   , fBCIDXmlPath    );

  //
  // Register trigger configuration algorithm
  //
  Anp::RegisterCallback(reg, "ReadTiny::ConfAlg", fConf, this);

  //
  // Register ntuple branches
  //
  fNtupleEntry = NtupleSvc::Instance("event").Branch<TrigMonEvent>("data");
  fOnlineEntry = NtupleSvc::Instance("online").Branch<TrigMonEvent>("online");
}

//---------------------------------------------------------------------------------------
bool Anp::ReadTiny::Init()
{
  //
  // Run init...
  //
  log() << "Initializing ReadTiny" << endl;

  //
  // Read file list per run
  //
  ReadXML();
  
  return true;
}

//-----------------------------------------------------------------------------
void Anp::ReadTiny::Signal(const Registry &, const string &signal)
{
  //
  // Handle callback from configuration algorithm 
  //
  if(signal == "TRIG_CONFIG_DB") {

    if(!fConf.valid()) { 
      log() << "Signal - invalid configuration algorithm" << endl;
      return;
    }
    else {
      fConfigs[fConf->GetConfig().getRun()] = fConf->GetConfig();
    }
  }
}

//---------------------------------------------------------------------------------------
void Anp::ReadTiny::Exec()
{
  //
  // Read new event entry
  //
  fOnlineEntry.SetState(Ntuple::kNONE);
  fOnlineEntry.SetEvent(Ntuple::kNONE);
  fCounter++ ;
  
  if(fNtupleEntry.valid() && 
     fNtupleEntry.GetState() == Ntuple::kREAD &&
     fNtupleEntry.GetEvent() == Ntuple::kPASS) {
    
    if(fDebug) log() << "Exec - ntuple is valid" << endl ;
    if(UpdateEvent(fNtupleEntry.GetRef())) {
      //
      // Set status for clients
      //
      if(fDebug) log() << "Exec - Updating event"  << endl ;
      fOnlineEntry.SetState(Ntuple::kREAD);
      fOnlineEntry.SetEvent(Ntuple::kPASS);
    }
    else {
      if(fNFailedUpdate==0){
        log() << "Exec - failed to update event. (This message will not repeat)" << endl;
      }
      fNFailedUpdate++ ;
    }
  }
}

//---------------------------------------------------------------------------------------
void Anp::ReadTiny::Done()
{
  //
  // Clean up internal state
  //
  log() << "Failed to update event "            << fNFailedUpdate     << " times" << endl ;
  log() << "Failed to update BCID information " << fNFailedBCIDUpdate << " times" << endl ;
  log() << "Failed to find mu information     " << fNFailedMu         << " times" << endl ;
  log() << "Finalizing ReadTiny" << std::endl;
}

//---------------------------------------------------------------------------------------
bool Anp::ReadTiny::UpdateEvent(TrigMonEvent &event)
{
  //
  // Check if this run number is known
  //
  const unsigned run = event.getRun();
  
  if(fDebug) log() << "UpdateEvent - Looking for run " << run << endl ;
  
  RunMap::iterator rit = fRuns.find(run);
  if(rit == fRuns.end()) {
    //
    // Read all files for this run
    //
    RunData data;
    ReadFiles(run, data);
    
    //
    // Output run number and event counter to signify change of run
    //
    if(fDebug) log() << "UpdateEvent - New run: " << event.getRun() << "   Entry = " << fCounter << endl;

    //
    // Insert new run data
    //
    //  std::bad_alloc occurs here
    rit = fRuns.insert(RunMap::value_type(run, data)).first;

    //
    // Check config consistency
    //
    const ConfigMap::const_iterator cit = fConfigs.find(run);
    if(cit != fConfigs.end()) {
      //
      // Check consistency between trigger configuration from DB and EB ntuple
      //
      if(!CheckConfig(cit->second, rit->second)){
        return false;
      }
    }
    else {
      log() << "UpdateEvent - missing online configuration for run: " << run << endl;
    }
  }

  const RunData &data = rit->second;
  
  //
  // Fill event data
  //
  const EventKey key(event.getSec(), event.getNanoSec());
  
  const EventMap::const_iterator kit = data.events.find(key);
  if(kit != data.events.end()) {
    //
    // Fill vertex information
    //
    UpdateVertex(event, kit->second);
    
    //
    // Fill online decisions
    //
    if(fOnlineEntry.valid()) {
      UpdateOnline(fOnlineEntry.ref(), kit->second);
    }
  }
  else {
    if(fDebug) log() << "UpdateEvent - Can't find event in data." << endl ;
    return false;
  }
  
  return true;
}

//---------------------------------------------------------------------------------------
bool Anp::ReadTiny::UpdateVertex(TrigMonEvent &event, const EventData &data) const
{
  //
  // Fill vertex info from data
  // Must remove dummy vertex
  //
  
  uint32_t key = -1 ;
  float nVtx        = (data.dNVtx==0) ? 0 : data.dNVtx-1.0 ;
  float mu          = data.dMu ;  
  float runNumber   = data.dRunNumber ;
  float eventNumber = data.dEventNumber ;
  float lumiblock   = data.dLumiblock ;
  float bunchId     = data.dBunchId ;
  float bcidStatus  = data.dBCIDStatus ;
                
  key = 251 ; event.addVar( key, nVtx        ) ;
  key = 260 ; event.addVar( key, mu          ) ;
  key = 261 ; event.addVar( key, runNumber   ) ;
  key = 262 ; event.addVar( key, eventNumber ) ;
  key = 263 ; event.addVar( key, lumiblock   ) ;
  key = 264 ; event.addVar( key, bunchId     ) ;
  key = 265 ; event.addVar( key, bcidStatus  ) ;
  
  return true;
}

//---------------------------------------------------------------------------------------
bool Anp::ReadTiny::UpdateOnline(TrigMonEvent &event, const EventData &data) const
{
  //
  // Fill online decisions
  //
  event.getL1Item() = data.dEventLV1;
  event.getChain () = data.dEventHLT;

  return true;
}

//---------------------------------------------------------------------------------------
bool Anp::ReadTiny::ReadFiles(unsigned run, RunData &data)
{
  log() << "Starting ReadFiles" << endl ;
  //
  // Read files for given run
  //

  const FileMap::const_iterator fit = fFileMap.find(run);
  if(fit == fFileMap.end()) {
    log() << "ReadFiles - missing files for run: " << run << endl;
    return false;
  }
  const vector<string> &files = fit->second;
  
  const NFileMap::const_iterator nit = fNFileMap.find(run);
  if(nit == fNFileMap.end()) {
    log() << "ReadFiles - missing NFiles for run: " << run << endl;
    return false;
  }
  unsigned nFiles = nit->second ;
  log() << "ReadFiles - nFiles = " << nFiles << endl ;
  
  TChain chainEvent (fTreeNameEvent .c_str()) ;
  TChain chainConfig(fTreeNameConfig.c_str()) ;

  for(unsigned i=0 ; i<files.size(); ++i){
    for(unsigned j=1 ; j<=nFiles ; j++){
      //
      // Open ntuple file
      //
      TString filename = Form("/afs/cern.ch/work/r/rsv/private/user.rsv.data11_2p76TeV.00178211.physics_MinBias.recon.RAW.r4238.130125014944/user.rsv.000091._%05d.test_withoutMuonReco.root",j) ;
      log() << "ReadFiles - filename:" << filename << endl;
      chainEvent .Add(filename) ;
      chainConfig.Add(filename) ;
      if(fDebug){
        log() << "ReadFiles - Adding file " << filename << endl ;
        log() << "ReadFiles - chainEvent  contains " << chainEvent .GetEntries() << " entries" << endl ;
        log() << "ReadFiles - chainConfig contains " << chainConfig.GetEntries() << " entries" << endl << endl ;
      }
    }
  }
  
  ReadChains(chainEvent, chainConfig, data);
  log() << "Leaving ReadFiles" << endl ;
  return true;
}

//---------------------------------------------------------------------------------------
void Anp::ReadTiny::ReadChains(TChain &chainEvent, TChain &chainConfig, RunData &data)
{
  //
  // Read data from chains
  //

  //
  // Initialize config chain
  //
  // Fix typo.  Oops.
  if(chainConfig.GetBranch("HLTPrescaleKey")) chainConfig.SetBranchAddress("HLTPrescaleKey", &fHLTPrescaleKey ) ;
  if(chainConfig.GetBranch("HTLPrescaleKey")) chainConfig.SetBranchAddress("HTLPrescaleKey", &fHLTPrescaleKey ) ;
  chainConfig.SetBranchAddress("LV1PrescaleKey", &fLV1PrescaleKey ) ;
  chainConfig.SetBranchAddress("masterKey"     , &fMasterKey      ) ;
  chainConfig.SetBranchAddress("trig_names"    , &fTrigNames      ) ;
  chainConfig.SetBranchAddress("trig_ids"      , &fTrigIds        ) ;

  if(chainConfig.GetEntries() > 0) {
    chainConfig.GetEntry(0);
    if(data.isConfig) {
    }
    else {
      data.isConfig        = true;
      data.dHLTPrescaleKey = fHLTPrescaleKey;
      data.dLV1PrescaleKey = fLV1PrescaleKey;
      data.dMasterKey      = fMasterKey;    
      data.dTrigNames      = *fTrigNames;
      data.dTrigIds        = *fTrigIds;
    }
  }
  
  //
  // Initialize event tree
  //
  chainEvent.SetBranchAddress( "runNumber"    , &fRunNumber    ) ;
  chainEvent.SetBranchAddress( "lumiBlock"    , &fLumiblock    ) ;
  chainEvent.SetBranchAddress( "eventNumber"  , &fEventNumber  ) ;
  chainEvent.SetBranchAddress( "bunchId"      , &fBunchId      ) ;
  chainEvent.SetBranchAddress( "timestamp"    , &fTimestamp    ) ;
  chainEvent.SetBranchAddress( "timestamp_ns" , &fTimestamp_ns ) ;
  chainEvent.SetBranchAddress( "nVtx"         , &fNVtx         ) ;
  
  chainEvent.SetBranchAddress( "vtx_x"     , &fVtx_x    ) ;
  chainEvent.SetBranchAddress( "vtx_y"     , &fVtx_y    ) ;
  chainEvent.SetBranchAddress( "vtx_z"     , &fVtx_z    ) ;
  chainEvent.SetBranchAddress( "vtx_chi2"  , &fVtx_chi2 ) ;
  chainEvent.SetBranchAddress( "vtx_ndof"  , &fVtx_ndof ) ;
  chainEvent.SetBranchAddress( "event_lv1" , &fEventLV1 ) ;
  chainEvent.SetBranchAddress( "event_hlt" , &fEventHLT ) ;
  
  int nEventsDone = 0 ;
  NEventKey nevents ;
  Int_t nEntries = chainEvent.GetEntries() ;
  if(fDebug) log() << "ReadChains - nEntries = " << nEntries << endl ;
  Int_t currentRunNumber = -1 ;
  for(int i = 0; i < nEntries ; ++i) {
    chainEvent.GetEntry(i);

    EventData d;
    d.dRunNumber    = fRunNumber;
    d.dLumiblock    = fLumiblock;
    d.dEventNumber  = fEventNumber;
    d.dBunchId      = fBunchId;
    d.dTimestamp    = fTimestamp;
    d.dTimestamp_ns = fTimestamp_ns;
    d.dNVtx         = fNVtx;
    d.dEventLV1     = *fEventLV1;
    d.dEventHLT     = *fEventHLT;
    
    if(i==0) currentRunNumber = fRunNumber ;
    
    if(i==0) fT0 = time(NULL) ;
    
    //if(i<10 && fDebug){
    if(i%100000==0 && fDebug){
        fT1 = time(NULL) ;
        log() << Form("ReadChains - %7d  %6d  %4d  %8d  %4d  %3d  Delta t = %8.4f", i, d.dRunNumber, d.dLumiblock, d.dEventNumber, d.dBunchId, d.dNVtx, difftime(fT1,fT0)) << std::endl ;
        fT0 = fT1 ;
    }
    
    // Get BCID information
    //const BCIDMap::const_iterator bcidit = fBCIDMap.find( fRunNumber );
    //if(bcidit == fBCIDMap.end()) {
    //  if(fNFailedBCIDUpdate==0){
    //    log() << "ReadChains - missing BCID information for run: " << fRunNumber << " (This message will not repeat)" << endl;
    //  }
    //  fNFailedBCIDUpdate++ ;
    //}
    //else{
    //  const BCIDData bcid = bcidit->second;
    //  data.dBCID = bcid ;
    //
    //  // Get BCID status from the run data
    //  d.dBCIDStatus = -1 ;
    //  std::vector<unsigned> firsts = data.dBCID.dFirstBunches ;
    //  std::vector<unsigned> others = data.dBCID.dOtherBunches ;
    //  
    //  for(unsigned i=0 ; i<firsts.size() ; i++){
    //    if(unsigned(d.dBunchId)==firsts.at(i)){
    //      d.dBCIDStatus = 1 ;
    //      break ;
    //    }
    //  }
    //  
    //  if(d.dBCIDStatus==-1){
    //    for(unsigned i=0 ; i<others.size() ; i++){
    //      if(unsigned(d.dBunchId)==others.at(i)){
    //        d.dBCIDStatus = 2 ;
    //        break ;
    //      }
    //    }
    //  }
    //  if(d.dBCIDStatus==-1){
    //    // Assume empty bunch crossing
    //    d.dBCIDStatus = 0 ;
    //  }
    //}

    //
    // Get mu from the MuMap
    //
    const MuMap::const_iterator mittmp = fMuMap.begin() ;
    
    const MuMap::const_iterator mit = fMuMap.find( d.dRunNumber );
    if(mit == fMuMap.end()) {
      if(fNFailedMu%10000==0){
        log() << "ReadChains - missing mu information for run: " << d.dRunNumber << " (This message will not repeat for every event)" << endl ;
      }
      fNFailedMu++ ;
    }
    const MuKey muKeys = mit->second;

    d.dMu = -1 ;
    const MuKey::const_iterator mmit = muKeys.find( d.dLumiblock );
    if(mmit != muKeys.end()) {
      d.dMu =  mmit->second ;
    }
    
    //
    // Vertex information
    //
    if(false){
    // Removed to save memory and prevent std::bad_alloc errors
      if(fVtx_x->size() == fVtx_y   ->size() && 
         fVtx_x->size() == fVtx_z   ->size() && 
         fVtx_x->size() == fVtx_chi2->size() && 
         fVtx_x->size() == fVtx_ndof->size()) {
      
      
        for(unsigned j = 0; j < fVtx_x->size(); ++j) {
          VertexData v;
    
          v.dVtx_x    = fVtx_x   ->at(j);
          v.dVtx_y    = fVtx_y   ->at(j);
          v.dVtx_z    = fVtx_z   ->at(j);
          v.dVtx_chi2 = fVtx_chi2->at(j);
          v.dVtx_ndof = fVtx_ndof->at(j);
          
          d.dVertex.push_back(v);
        }
      }
      else {
        log() << "ReadChains - inconsistent vertex data" << endl;
      }
    }

    const EventKey key(d.dTimestamp, d.dTimestamp_ns);
    
    if(!data.events.insert(EventMap::value_type(key, d)).second){
      log() << "ReadChains - non-unique timestamp: " << key.first << ", " << key.second << endl;
    }
    
    //
    // Get information about the lumiblocks
    //
    if(fRunNumber!=currentRunNumber || i==nEntries-1){
    
      NEventMap::iterator neventit = fNEventMap.find( currentRunNumber ) ;
      if(neventit == fNEventMap.end()){
        if(fDebug) log() << "ReadChains - Inserting lb information for run=" << currentRunNumber << endl ;
        neventit = fNEventMap.insert(NEventMap::value_type( fRunNumber , nevents)).first ;        
      }
      currentRunNumber = fRunNumber ;
    }
    
    nEventsDone++ ;
  }
}

//---------------------------------------------------------------------------------------
bool Anp::ReadTiny::ReadXML()
{
  //
  // Read ntuple filenames from XML file
  //

  if(fEosXmlPath==""){
    log() << "NTUP_ENHBIAS XML filename not initialized." << std::endl ;
    return false ;
  } 
  log() << "Using XML file " << fEosXmlPath << " for NTUP_ENHBIAS ntuples" << endl ;
  
  if(fBCIDXmlPath==""){
    log() << "BCID XML filename not initialized." << std::endl ;
    return false ;
  } 
  log() << "Using XML file " << fBCIDXmlPath << " for BCID scaling" << endl ;

  if(fMuXmlPath==""){
    log() << "Mu XML filename not initialized." << std::endl ;
    return false ;
  } 
  log() << "Using XML file " << fMuXmlPath << " for mu values" << endl ;
  
  Anp::Registry xml_reg = Anp::ParseXml(fEosXmlPath, fDebug) ;
  while(xml_reg.KeyExists("run")){
    Anp::Registry run_reg ;
    xml_reg.Get("run", run_reg) ;
    xml_reg.RemoveKey("run") ;
    
    int run = -1 ;
    run_reg.Get("value", run) ;
    
    std::vector<std::string> filenames ;
    unsigned nFiles = -1 ;

    while(run_reg.KeyExists("path")){
      std::string path = "" ;
      run_reg.Get("path", path) ;
      run_reg.RemoveKey("path") ;
      filenames.push_back(path) ;
      if(fDebug) log() << "Adding " << path << std::endl ;
    }
    while(run_reg.KeyExists("nMax")){
      run_reg.Get("nMax", nFiles) ;
      run_reg.RemoveKey("nMax") ;
    }

    FileMap::iterator  fit =  fFileMap.find(run) ;
    NFileMap::iterator nit = fNFileMap.find(run) ;
    if(fit == fFileMap.end()){
      fit = fFileMap.insert(FileMap::value_type(run, filenames)).first ;
    }
    else{
      log() << "ReadXML: Warning- attempting to load files which are already loaded. " << endl ;
    }
    if(nit == fNFileMap.end()){
      nit = fNFileMap.insert(NFileMap::value_type(run, nFiles)).first ;
    }
    else{
      log() << "ReadXML: Warning- attempting to load nFiles which are already loaded. " << endl ;
    }
    log() << "ReadXML: Loading files..." << endl ;
  }


  Anp::Registry muxml_reg = Anp::ParseXml(fMuXmlPath, fDebug) ;
  while(muxml_reg.KeyExists("run")){
    MuKey mus ;
    Anp::Registry run_reg ;
    muxml_reg.Get("run", run_reg) ;
    muxml_reg.RemoveKey("run") ;
    
    int run = -1 ;
    run_reg.Get("value", run) ;

    while(run_reg.KeyExists("lb")){
      Anp::Registry lb_reg ;
      run_reg.Get("lb", lb_reg) ;
      run_reg.RemoveKey("lb") ;
      
      Int_t   lb = -1 ;
      Float_t mu = -1 ;
      lb_reg.Get("value", lb) ;

      while(lb_reg.KeyExists("mu")){
        Anp::Registry mu_reg ;
        lb_reg.Get("mu", mu_reg) ;
        lb_reg.RemoveKey("mu") ;
        mu_reg.Get("value", mu) ;
      }
      
      MuKey::iterator mit = mus.find(lb) ;
      if(mit == mus.end()){
        mit = mus.insert(MuKey::value_type(lb, mu)).first ;
        //if(fDebug) log() << "Inserting info for run " << run << " lb " << lb << " mu " << mu << endl ;
      }
      else{
        log() << "ReadXML: Warning- attempting to store mu information which is already stored. " << endl ;
      }
    }
    
    MuMap::iterator mit = fMuMap.find(run) ;
    if(mit == fMuMap.end()){
      log() << "Inserting info for run " << run << endl ;
      mit = fMuMap.insert(MuMap::value_type(run, mus)).first ;
    }
    else{
      log() << "ReadXML: Warning- attempting to store lb and mu combinations which are already stored." << endl ;
    }
    log() << "ReadXML: Loading mu info..." << endl ;
  }
  
  Anp::Registry BCIDxml_reg = Anp::ParseXml(fBCIDXmlPath, fDebug) ;
  while(BCIDxml_reg.KeyExists("run")){
    BCIDData bcid ;
    std::vector<unsigned> firsts ;
    std::vector<unsigned> others ;
    Anp::Registry run_reg ;
    BCIDxml_reg.Get("run", run_reg) ;
    BCIDxml_reg.RemoveKey("run") ;
    
    int run = -1 ;
    run_reg.Get("run_number", run) ;
    run_reg.RemoveKey("run_number") ;

    while(run_reg.KeyExists("train")){
      Anp::Registry train_reg ;
      run_reg.Get("train", train_reg) ;
      run_reg.RemoveKey("train") ;
      
      float first = -1 ;
      int other = -1 ;
      train_reg.Get("firstBunch", first) ;
      train_reg.RemoveKey("firstBunch") ;
      if(first!=-1) firsts.push_back(first) ;
      first = -1 ; // Reset to default value
      
      while(train_reg.KeyExists("bunch")){
        Anp::Registry bunch_reg ;
        train_reg.Get("bunch", other) ;
        train_reg.RemoveKey("bunch") ;
        if(other!=-1) others.push_back(other) ;
        other = -1 ; // Reset to default value
      }
    }
    bcid.dFirstBunches = firsts ;
    bcid.dOtherBunches = others ;
    
    BCIDMap::iterator bcidit = fBCIDMap.find(run) ;
    if(bcidit == fBCIDMap.end()){
      log() << "Inserting BCID info for run " << run << endl ;
      bcidit = fBCIDMap.insert(BCIDMap::value_type(run, bcid)).first ;
    }
    else{
      log() << "ReadXML: Warning- attempting to store BCID configuration which is already stored." << endl ;
    }
    log() << "ReadXML: Loading BCID info..." << endl ;
  }
  
  log() << "ReadXML: Finished ReadXML" << endl ;
  return true ;
}

//---------------------------------------------------------------------------------------
bool Anp::ReadTiny::CheckConfig(const TrigMonConfig &config, const RunData &data) const
{
  //
  // Check consistency of online configuartion from DB and from EB ntuple
  // 
  if(data.dTrigNames.size() != data.dTrigIds.size()) {
    log() << "CheckConfig - inconsistent configuration data in EB ntuple" << endl;
    return false;
  }

  for(unsigned i = 0; i < data.dTrigNames.size(); ++i) {
    const string &name = data.dTrigNames.at(i);
    const unsigned id  = data.dTrigIds  .at(i);
    
    bool pass = false;

    for(unsigned j = 0; j < config.size<TrigConfChain>(); ++j) {
      const TrigConfChain &chain = config.at<TrigConfChain>(j);

      if(chain.getName() == name && chain.getEncodedId() == id) {
        //
        // Missing chain
        //
        pass = true;
      }
    }
    
    if(!pass) {
      log() << "CheckConfig - failed consistency check for: " << name << endl;
      return false;
    }
  }

  return true;
}
