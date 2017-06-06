/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//////////////////////////////////////////////////////////////////////
// 
// Author   : Michele Pinamonti <michele.pinamonti@cern.ch>
// Creation : 
// Purpose  : base class to set efficiencies for the matrix method
// Updated  : 
//           2016-01-29 : FD: set _isRun1 = false and set muon triggers for Run2
//////////////////////////////////////////////////////////////////////

#include "TopFakes/MMEffSet.h"
#include "TopFakes/MMEfficiency.h"
#include "TopFakes/MatrixUtils.h"

#include "TH1F.h"
#include "TH2F.h"
#include "TFile.h"
#include "TString.h"
#include "TObjString.h"
#include <cmath>
#include <iostream>

using namespace std;

MMEffSet::MMEffSet(bool isEle, bool isReal, bool debug) {
  _Estimate = "";
  _dataPath = "data";
  _isEle = isEle;
  _isReal = isReal;
  _effIsRead = false;
  _useCorr = false;
  _numCond = 0;
  _splitBitMask = 0;
  _debug = debug;
  _period = Period::Year2015;
  _ptTres = 0;
  _trig1 = "";
  _trig2 = "";
  _trig3 = "";
  _trigger = -100;

  for (int i = 0; i < 10; ++i) {
    _Eff[i] = 0;
    _EffCorr[i] = 0;
  }
}

MMEffSet::~MMEffSet(){
  for (int i = 0; i < 10; ++i) {
    delete _Eff[i];
    delete _EffCorr[i];
  }
}

void MMEffSet::SetupEff(std::string aEff){
  _Estimate = aEff;
  ParseOptions(_Estimate,&_optMap);  
  if(_optMap["Conditions"]=="x"){ // if x is there as condition, wait before initializing
    if(_debug) std::cout << "Custom conditions. Not setting up efficiencies yet." << std::endl;
    return;
  }
  else if(_optMap["Conditions"].find("NB/NS/TR")!=std::string::npos) AddNumBtagAndNumJetsShortAndTriggerSplit();
  else if(_optMap["Conditions"].find("NB/NJ/TR")!=std::string::npos) AddNumBtagAndNumJetsAndTriggerSplit();
  else if(_optMap["Conditions"].find("NB/NS")!=std::string::npos) AddNumBtagAndNumJetsShortSplit();
  else if(_optMap["Conditions"].find("NB/NJ")!=std::string::npos) AddNumBtagAndNumJetsSplit();
  else if(_optMap["Conditions"].find("NB/TR")!=std::string::npos) AddNumBtagAndTriggerSplit();
  else if(_optMap["Conditions"].find("NS/TR")!=std::string::npos) AddNumJetsShortAndTriggerSplit();
  else if(_optMap["Conditions"].find("NJ/TR")!=std::string::npos) AddNumJetsAndTriggerSplit();
  else if(_optMap["Conditions"].find("NB")!=std::string::npos) AddNumBtagSplit();
  else if(_optMap["Conditions"].find("NS")!=std::string::npos) AddNumJetsShortSplit();
  else if(_optMap["Conditions"].find("NJ")!=std::string::npos) AddNumJetsSplit();
  else if(_optMap["Conditions"].find("TR")!=std::string::npos) AddTriggerSplit();
  else{
    _numCond = 1;
    _condVec.push_back(_optMap["Conditions"]);
  }
  if(_optMap["Conditions"].find("corr")!=std::string::npos) AddCorrection();
  ReadEfficiencies();
}

void MMEffSet::SetDataPath(std::string path){
  _dataPath = path;
}

void MMEffSet::ParseOptions(std::string s,std::map<std::string,std::string> *m){
   TString ss(s);
   TObjArray *optArray = ss.Tokenize(":");
   (*m)["Name"] = ((TObjString*)optArray->At(0))->GetString();
   (*m)["Conditions"] = ((TObjString*)optArray->At(1))->GetString();
   for(int i=0;i<(int)optArray->GetEntries()-2;i++){
     (*m)[Form("Histo%d",i)] = ((TObjString*)optArray->At(i+2))->GetString();
   }
   delete optArray;
}

void MMEffSet::AddCustomSplit(std::string config){
  std::vector<std::string> condVec = Tokenize(config,":");
  if(_numCond>0) _condVec = Combine(_condVec,condVec);
  else _condVec = condVec;
  _numCond = _condVec.size();
}

void MMEffSet::ReadEfficiencies(){
  if(_effIsRead) return;
  // real
  for(int i=0;i<(int)_condVec.size();i++){
    _Eff[i] = new MMEfficiency();
    ReadEfficiency(_Eff[i],_Estimate,_condVec[i]);
    if(_useCorr){
      _EffCorr[i] = new MMEfficiency();
      ReadEfficiency(_EffCorr[i],_Estimate,_condVec[i],true);
    }
  }
  _effIsRead = true;
}

void MMEffSet::AddNumBtagSplit(){
  AddCustomSplit("0b:ge1b");
  _splitBitMask |= (NBTAG_MASK);
}
void MMEffSet::AddNumJetsSplit(){
  AddCustomSplit("1j:2or3j:ge4j");
  _splitBitMask |= (NJETS_MASK);
}
void MMEffSet::AddNumJetsShortSplit(){
  AddCustomSplit("1j:ge2j");
  _splitBitMask |= (NJETSSHORT_MASK);
}
void MMEffSet::AddTriggerSplit(){
  // for Run1
  if(_period == Period::Year2012){
    if(_isEle){
      AddCustomSplit("e24vhi:e60:e24vh");
      _ptTres = 62;
    }
    else{
      AddCustomSplit("mu24i:mu36:mu24");
      _ptTres = 37;
    }
  }
  else if (_period == Period::Year2015) { // for Run2
    //if(_isEle){ // no trigger splitting for electrons
    //}
    if(!_isEle) { // for muons
      AddCustomSplit("mu20_iloose_L1MU15:mu50:mu20_L1MU15");
      _ptTres = 51;
    }
    //AddCustomSplit(Form("%s:%s:%s",_trig1.c_str(),_trig2.c_str(),_trig3.c_str()));
  }
  else if (_period == Period::Year2016) { // for Run2
    if(_isEle){ // no trigger splitting for electrons
      AddCustomSplit("e24_lhtight_nod0_ivarloose:e60_lhmedium_nod0:e24_lhmedium_L1EM20VH");
      _ptTres = 62;
    }
    if(!_isEle) { // for muons
      AddCustomSplit("mu24_ivarmedium:mu50:mu24");
      _ptTres = 51;
    }
    //AddCustomSplit(Form("%s:%s:%s",_trig1.c_str(),_trig2.c_str(),_trig3.c_str()));
  }
  _splitBitMask |= (TRIGGER_MASK);
}

// ... and combined
void MMEffSet::AddNumBtagAndTriggerSplit(){
  AddNumBtagSplit();
  AddTriggerSplit();
}
void MMEffSet::AddNumBtagAndNumJetsSplit(){
  AddNumBtagSplit();
  AddNumJetsSplit();
}
void MMEffSet::AddNumBtagAndNumJetsShortSplit(){
  AddNumBtagSplit();
  AddNumJetsShortSplit();
}
void MMEffSet::AddNumJetsAndTriggerSplit(){
  AddNumJetsSplit();
  AddTriggerSplit();
}
void MMEffSet::AddNumJetsShortAndTriggerSplit(){
  AddNumJetsShortSplit();
  AddTriggerSplit();
}
void MMEffSet::AddNumBtagAndNumJetsAndTriggerSplit(){
  AddNumBtagSplit();
  AddNumJetsSplit();
  AddTriggerSplit();
}
void MMEffSet::AddNumBtagAndNumJetsShortAndTriggerSplit(){
  AddNumBtagSplit();
  AddNumJetsShortSplit();
  AddTriggerSplit();
}

void MMEffSet::ReadEfficiency(MMEfficiency *eff,std::string aEff,std::string condition,bool isCorrection){
  string effFileName = Form("%s/%s_%s_%s.root",
                         _dataPath.c_str(),
                         isCorrection ? (_isReal ? "RCorr" : "FCorr") : (_isReal ? "R" : "F"),
                         _isEle ? "el" : "mu",
                         _optMap["Name"].c_str()
                        );
  if(_debug){
    if(_debug) std::cout << "------------------------------" << std::endl;
    if(_isReal) std::cout << " --> Opening file for Real Eff: ";
    else        std::cout << " --> Opening file for Fake Eff: ";
    std::cout << effFileName << std::endl;
  }
  TFile* effFile = TFile::Open(effFileName.c_str());
  
  std::map<std::string,std::string> optMap;
  ParseOptions(aEff,&optMap);
  
  string histoName;
  int ndim = 1;

  bool status;
  
  // take average from histo
  histoName = "av";
  if(condition=="") histoName = (string)(isCorrection ? "corr" : "eff")+"_" + histoName + "_" + optMap["Conditions"];
  else              histoName = (string)(isCorrection ? "corr" : "eff")+"_" + histoName + "_" + condition;
  if(_debug) std::cout << " --> Looking for histogram \"" << histoName << "\" inside file " << effFile->GetName() << std::endl;
  eff->SetAverageHist(effFile,histoName);
  
  for(int i=0;;i++){
    histoName = optMap[Form("Histo%d",i)];
    if(histoName == "") break;
    ndim = 1 + CountInString(histoName,'_');
    if(condition=="") histoName = (string)(isCorrection ? "corr" : "eff")+"_" + histoName + "_" + optMap["Conditions"];
    else              histoName = (string)(isCorrection ? "corr" : "eff")+"_" + histoName + "_" + condition;
    if(_debug){
      std::cout << " --> Looking for histogram \"" << histoName << "\" inside file " << effFile->GetName();
      std::cout << " " << (ndim==1 ?" (1D)" : (ndim==2 ? " (2D)" : " (3D)"));
      std::cout << "..... ";
    }
    status = eff->AddHisto(effFile,histoName,ndim);
    if(_debug){
      if(status) std::cout << "OK" << std::endl;
      else       std::cout << "NO" << std::endl << "  MMEfficiency::INFO: Dummy Hstogram added." << std::endl;
    }
  }

  delete effFile;
}

void MMEffSet::AddCorrection(){
  _useCorr = true;
}

void MMEffSet::SetCustom(int value){
  _splitIdx = value;
}

void MMEffSet::SetNumBtag(int nbtag){
  int value = -100;
  value = NumBtagValue(nbtag);
  SetCustom(value);
}

void MMEffSet::SetNumJets(int njets){
  int value = -100;
  value = NumJetsValue(njets);
  SetCustom(value);
}

void MMEffSet::SetNumJetsShort(int njets){
  int value = -100;
  value = NumJetsShortValue(njets);
  SetCustom(value);
}

void MMEffSet::SetTrigger(int trigger,float pt){
  int value = -100;
  value = TriggerValue(trigger, pt);
  SetCustom(value);
}

void MMEffSet::SetNumBtagAndNumJets(int nbtag,int njets){
  int value = -100;
  value = NumJetsValue(njets);
  value += 3*NumBtagValue(nbtag);
  SetCustom(value);
}

void MMEffSet::SetNumBtagAndNumJetsShort(int nbtag,int njets){
  int value = -100;
  value = NumJetsShortValue(njets);
  value += 2*NumBtagValue(nbtag);
  SetCustom(value);
}

void MMEffSet::SetNumBtagAndTrigger(int nbtag,int trigger,float pt){
  int value = -100;
  value = TriggerValue(trigger, pt);
  value += 3*NumBtagValue(nbtag);
  SetCustom(value);
}

void MMEffSet::SetNumJetsAndTrigger(int njets,int trigger,float pt){
  int value = -100;
  value = TriggerValue(trigger, pt);
  value += 3*NumJetsValue(njets);
  SetCustom(value);
}

void MMEffSet::SetNumJetsShortAndTrigger(int njets,int trigger,float pt){
  int value = -100;
  value = TriggerValue(trigger, pt);
  value += 3*NumJetsShortValue(njets);
  SetCustom(value);
}

void MMEffSet::SetNumBtagAndNumJetsAndTrigger(int nbtag,int njets,int trigger,float pt){
  int value = -100;
  value = TriggerValue(trigger, pt);
  value += 3*NumJetsValue(njets);
  value += 9*NumBtagValue(nbtag);
  SetCustom(value);
}

void MMEffSet::SetNumBtagAndNumJetsShortAndTrigger(int nbtag,int njets,int trigger,float pt){
  int value = -100;
  value = TriggerValue(trigger, pt);
  value += 3*NumJetsShortValue(njets);
  value += 6*NumBtagValue(nbtag);
  SetCustom(value);
}

void MMEffSet::SetTriggerNames(std::string trig1,std::string trig2,std::string trig3){
  _trig1 = trig1;
  _trig2 = trig2;
  _trig3 = trig3;
}

void MMEffSet::SetPeriod(int period){
  _period = period;
}

void MMEffSet::SetTriggerPrescales(float ps1,float ps2,float ps3){
  _ps1 = ps1;
  _ps2 = ps2;
  _ps3 = ps3;
}

void MMEffSet::SetTriggerThreshold(float thres){
  _ptTres = thres;
}

int MMEffSet::TriggerValue(int trigger,float pt){
  _trigger = -100;
  if ((trigger & (1 << 0))>0 && (_ptTres<0 || pt <  _ptTres)) _trigger = 0; // if matched to the low-pT isolated trigger
  if ((trigger & (1 << 1))>0 && (_ptTres<0 || pt >= _ptTres)) _trigger = 1; // if matched to the high-pT trigger
  if ((trigger & (1 << 2))>0 && (_ptTres<0 || pt <  _ptTres)) _trigger = 2; // if matched to the low-pT non-iso trigger
  if ((trigger & 3)==0)                                       _trigger = 2; // if not matched to any trigger -> assign the low-pt non-iso eff
  return _trigger;
}

int MMEffSet::NumJetsValue(int njets){
  if(njets==1) return 0;
  if(njets<=3) return 1;
  else return 2;
}

int MMEffSet::NumJetsShortValue(int njets){
  if(njets==1) return 0;
  else return 1;
}

int MMEffSet::NumBtagValue(int nbtag){
  if(nbtag==0) return 0;
  else return 1;
}

void MMEffSet::SetLepton(float v0,float v1,float v2,float v3,float v4,float v5,float v6,float v7,float v8,float v9){
  for(int i=0;i<_numCond;i++) _Eff[i]->SetParVars(v0,v1,v2,v3,v4,v5,v6,v7,v8,v9);
  if(_useCorr)
    for(int i=0;i<_numCond;i++) _EffCorr[i]->SetParVars(v0,v1,v2,v3,v4,v5,v6,v7,v8,v9);
}

float MMEffSet::GetEff(bool isAverage){
  int idx = 0;
  if(_numCond>0) idx = _splitIdx;
  if(idx<0) return -1.; // added for events which are neglected by trigger splitting
  float E;
  if(isAverage){
    E = _Eff[idx]->GetAverageEfficiency();
    if(_useCorr) E *= _EffCorr[idx]->GetAverageEfficiency();
  }
  else{
    E = _Eff[idx]->GetEfficiency();
    if(_useCorr) E *= _EffCorr[idx]->GetEfficiency();
  }
  return E;
}

// to be called after 'AddTriggerSplit'
void MMEffSet::SetPtThres(int thres){
  _ptTres = thres;
}

void MMEffSet::SetLepton(MMEvent event,MMLepton lepton){
  _event = event;
  _lepton = lepton;
  //
  int njets = _event.njets;
  int ntag = _event.ntag;
  float jetpt = _event.jetpt;
  float sumet = _event.sumet;
  //
  float pt = _lepton.pt;
  float eta = _lepton.eta;
  float dR = _lepton.dR;
  float dRpt = _lepton.dRpt;
  float dPhi = _lepton.dPhi;
  float d0sig = _lepton.d0sig;
  int trigger = _lepton.trigger;
  //
  // set the correct splitting
  if((_splitBitMask & NBTAG_MASK) && (_splitBitMask & NJETSSHORT_MASK) && (_splitBitMask & TRIGGER_MASK))
    SetNumBtagAndNumJetsShortAndTrigger(ntag,njets,trigger,pt);
  else if((_splitBitMask & NBTAG_MASK) && (_splitBitMask & NJETS_MASK) && (_splitBitMask & TRIGGER_MASK))
    SetNumBtagAndNumJetsAndTrigger(ntag,njets,trigger,pt);
  else if((_splitBitMask & NBTAG_MASK) && (_splitBitMask & TRIGGER_MASK))
    SetNumBtagAndTrigger(ntag,trigger,pt);
  else if((_splitBitMask & NJETSSHORT_MASK) && (_splitBitMask & TRIGGER_MASK))
    SetNumJetsShortAndTrigger(njets,trigger,pt);
  else if((_splitBitMask & NJETS_MASK) && (_splitBitMask & TRIGGER_MASK))
    SetNumJetsAndTrigger(njets,trigger,pt);
  else if((_splitBitMask & NBTAG_MASK) && (_splitBitMask & NJETSSHORT_MASK))
    SetNumBtagAndNumJetsShort(ntag,njets);
  else if((_splitBitMask & NBTAG_MASK) && (_splitBitMask & NJETS_MASK))
    SetNumBtagAndNumJets(ntag,njets);
  else if((_splitBitMask & NBTAG_MASK))
    SetNumBtag(ntag);
  else if((_splitBitMask & NJETSSHORT_MASK))
    SetNumJetsShort(njets);
  else if((_splitBitMask & NJETS_MASK))
    SetNumJets(njets);
  else if((_splitBitMask & TRIGGER_MASK))
    SetTrigger(trigger,pt);
  //
  // set the used variables
  float input[] = {0,0,0,0,0,0,0,0,0,0};
  int k = 0;
  if(_Estimate.find(":eta_dR")!=string::npos)    { input[k] = eta;   k++; input[k] = dR; k++; }
  else if(_Estimate.find(":eta")!=string::npos)  { input[k] = eta;   k++; }
  if(_Estimate.find(":pt_dR")!=string::npos)     { input[k] = pt;    k++; input[k] = dR; k++; }
  else if(_Estimate.find(":pt_jetpt")!=string::npos)     { input[k] = pt;    k++; input[k] = jetpt; k++; }
  else if(_Estimate.find(":pt")!=string::npos)   { input[k] = pt;    k++; }
  if(_Estimate.find(":jetpt_dR")!=string::npos)  { input[k] = jetpt; k++; input[k] = dR; k++; }
  else if(_Estimate.find(":jetpt")!=string::npos){ input[k] = jetpt; k++; }
  if(_Estimate.find(":sumet")!=string::npos)  { input[k] = sumet; k++; }
  if(_Estimate.find(":dR")!=string::npos && _Estimate.find(":dRpt")==string::npos)  { input[k] = dR; k++; }
  if(_Estimate.find(":dRpt")!=string::npos)   { input[k] = dRpt;  k++; }
  if(_Estimate.find(":dPhi")!=string::npos)   { input[k] = dPhi;  k++; }
  if(_Estimate.find(":d0sig")!=string::npos)   { input[k] = d0sig;  k++; }
  if(_Estimate.find(":nbjets")!=string::npos)   { input[k] = ntag;  k++; }

  SetLepton(input[0],input[1],input[2],input[3],input[4],input[5],input[6],input[7],input[8],input[9]);
}

float MMEffSet::GetPrescale(){
  if(_trigger==0) return _ps1;
  if(_trigger==1) return _ps2;
  if(_trigger==2) return _ps3;
  return 1.;
}


// ------------------------------

// Internal methods / functions:

int MMEffSet::CountInString(std::string s,char c){
  int n = 0;
  for(int i=0;i<(int)s.size();i++){
    if(s[i]==c) n++;
  }
  return n;
}

std::vector<std::string> MMEffSet::Tokenize(std::string s, std::string c){
  std::vector<std::string> v;
  v.clear();
  std::string new_s;
  while(true){
    new_s = s.substr(0,s.find(c));
    v.push_back(new_s);
    if(new_s == s) break;
    else s = s.substr(s.find(c)+1,s.size());
  }
  return v;
}

std::vector<std::string> MMEffSet::Combine(std::vector<std::string> v1,std::vector<std::string> v2){
  std::vector<std::string> v;
  v.clear();
  for(int i=0;i<(int)v1.size();i++){
    for(int j=0;j<(int)v2.size();j++){
      v.push_back(v1[i]+"_"+v2[j]);
    }
  }
  return v;
}
