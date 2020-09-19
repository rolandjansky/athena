/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//////////////////////////////////////////////////////////////////////
// 
// Author   : 
//              Anna Henrichs <anna.henrichs@cern.ch>
//              Michele Pinamonti <michele.pinamonti@cern.ch>
// Creation : 
// Purpose  : main method to retrieve event-by-event weights 
//            from the Matrix Method
// Updated  : 
//           2016-07-14 : TTP: replace _isRun1 by _period
//           2016-01-29 : FD: set _isRun1 = false 
//                        set default weighter
//                        remove pt/dR variable, never used
//           2016-02-02 : FD update SetupWeighterDefault
//           2016-02-12 : SetupWeighterDefault : default do not use dPhi
//////////////////////////////////////////////////////////////////////

#include "TopFakes/FakesWeights.h"
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

// CONSTRUCTOR
FakesWeights::FakesWeights() : _mmUtil(0) {
  _channel = EJETS;
  _dataPath = "data";
  _dataPathIsSet = false;
  _isInitialized = false;
  _debug = false;
  
  _period = MMEffSet::Period::Year2015;
  _trig1_el = "";
  _trig2_el = "";
  _trig3_el = "";
  _trig1_mu = "";
  _trig2_mu = "";
  _trig3_mu = "";
  _ps1_el = 1;
  _ps2_el = 1;
  _ps3_el = 1;
  _ps1_mu = 1;
  _ps2_mu = 1;
  _ps3_mu = 1;
  _ptTres_el = 0;
  _ptTres_mu = 0;

  for (int i = 0; i < 4; ++i)
      _EffSet[i] = 0;
}

// DESTRUCTOR
FakesWeights::~FakesWeights(){
  delete _mmUtil;

  for (int i = 0; i < 4; ++i)
      delete _EffSet[i];
}

// ===============================================
void FakesWeights::SetDataPath(std::string path) {
  // method to set the path were efficiencies are 
  _dataPath = path;
  if(_isInitialized) for(int i=0;i<4;i++) _EffSet[i]->SetDataPath(path);
  _dataPathIsSet = true;
}

// ===============================================
void FakesWeights::SetDebug(bool debug){
  _debug = debug;
}

// ===============================================
void FakesWeights::SetPeriod(int period){
  _period = period;
}

// ===============================================
void FakesWeights::SetTriggerNamesEl(std::string trig1,std::string trig2,std::string trig3){
  _trig1_el = trig1;
  _trig2_el = trig2;
  _trig3_el = trig3;
}

// ===============================================
void FakesWeights::SetTriggerNamesMu(std::string trig1,std::string trig2,std::string trig3){
  _trig1_mu = trig1;
  _trig2_mu = trig2;
  _trig3_mu = trig3;
}

// ===============================================
void FakesWeights::SetTriggerPrescalesEl(float ps1,float ps2,float ps3){
  _ps1_el = ps1;
  _ps2_el = ps2;
  _ps3_el = ps3;
}

// ===============================================
void FakesWeights::SetTriggerPrescalesMu(float ps1,float ps2,float ps3){
  _ps1_mu = ps1;
  _ps2_mu = ps2;
  _ps3_mu = ps3;
}

// ===============================================
void FakesWeights::SetTriggerThresholdEl(float thres){
  _ptTres_el = thres;
}

// ===============================================
void FakesWeights::SetTriggerThresholdMu(float thres){
  _ptTres_mu = thres;
}

// ===============================================
void FakesWeights::SetupWeighter(int channel, std::string aReal, std::string aFake, std::string aReal2, std::string aFake2) {
  // method to set up tools which calculate the weights

  std::cout << "--------------------------------------------------------" << std::endl;
  std::cout << "  FakesWeights::INFO: Setting-up Matrix Method Weighter." << std::endl;
  if(channel==EJETS)  std::cout << "    channel:  e+jets" << std::endl;
  if(channel==MUJETS) std::cout << "    channel:  mu+jets" << std::endl;
  if(channel==EE)     std::cout << "    channel:  ee" << std::endl;
  if(channel==MUMU)   std::cout << "    channel:  mumu" << std::endl;
  if(channel==EMU)    std::cout << "    channel:  emu" << std::endl;
  if(channel==EMU){
    std::cout << "    real eff(e): " << aReal << std::endl;
    std::cout << "    fake eff(e): " << aFake << std::endl;    
    std::cout << "    real eff(mu): " << aReal2 << std::endl;
    std::cout << "    fake eff(mu): " << aFake2 << std::endl;    
  }
  else{
    std::cout << "    real eff: " << aReal << std::endl;
    std::cout << "    fake eff: " << aFake << std::endl;
  }
  std::cout << "--------------------------------------------------------" << std::endl;
  _channel = channel;
  _mmUtil = new MatrixUtils();
  _EffSet[0] = new MMEffSet(channel==EJETS || channel==EE || channel==EMU, true,  _debug);   // isEle, isReal
  _EffSet[1] = new MMEffSet(channel==EJETS || channel==EE || channel==EMU, false, _debug);
  _EffSet[2] = new MMEffSet(channel==EE, true,  _debug);
  _EffSet[3] = new MMEffSet(channel==EE, false, _debug);
  for(int i=0;i<4;i++){
    _EffSet[i]->SetPeriod(_period);
  }
  if(_period!=MMEffSet::Period::Year2012){
    if(channel==EJETS || channel==EE || channel==EMU){
      _EffSet[0]->SetTriggerNames(_trig1_el,_trig2_el,_trig3_el);
      _EffSet[1]->SetTriggerNames(_trig1_el,_trig2_el,_trig3_el);
      _EffSet[0]->SetTriggerPrescales(_ps1_el,_ps2_el,_ps3_el);
      _EffSet[1]->SetTriggerPrescales(_ps1_el,_ps2_el,_ps3_el);
      _EffSet[0]->SetTriggerThreshold(_ptTres_el);
      _EffSet[1]->SetTriggerThreshold(_ptTres_el);
    }
    else if(channel==MUJETS || channel==MUMU){
      _EffSet[0]->SetTriggerNames(_trig1_mu,_trig2_mu,_trig3_mu);
      _EffSet[1]->SetTriggerNames(_trig1_mu,_trig2_mu,_trig3_mu);
      _EffSet[0]->SetTriggerPrescales(_ps1_mu,_ps2_mu,_ps3_mu);
      _EffSet[1]->SetTriggerPrescales(_ps1_mu,_ps2_mu,_ps3_mu);
      _EffSet[0]->SetTriggerThreshold(_ptTres_mu);
      _EffSet[1]->SetTriggerThreshold(_ptTres_mu);
    }
    if(channel==EE){
      _EffSet[2]->SetTriggerNames(_trig1_el,_trig2_el,_trig3_el);
      _EffSet[3]->SetTriggerNames(_trig1_el,_trig2_el,_trig3_el);
      _EffSet[2]->SetTriggerPrescales(_ps1_el,_ps2_el,_ps3_el);
      _EffSet[3]->SetTriggerPrescales(_ps1_el,_ps2_el,_ps3_el);
      _EffSet[2]->SetTriggerThreshold(_ptTres_el);
      _EffSet[3]->SetTriggerThreshold(_ptTres_el);
    }
    else if(channel==MUMU || channel==EMU){
      _EffSet[2]->SetTriggerNames(_trig1_mu,_trig2_mu,_trig3_mu);
      _EffSet[3]->SetTriggerNames(_trig1_mu,_trig2_mu,_trig3_mu);
      _EffSet[2]->SetTriggerPrescales(_ps1_mu,_ps2_mu,_ps3_mu);
      _EffSet[3]->SetTriggerPrescales(_ps1_mu,_ps2_mu,_ps3_mu);
      _EffSet[2]->SetTriggerThreshold(_ptTres_mu);
      _EffSet[3]->SetTriggerThreshold(_ptTres_mu);
    }
  }
  _isInitialized = true;
  // setup data path if already specified
  if(_dataPathIsSet){
    if(_debug) std::cout << " --> Setting-up data-path = " << _dataPath << std::endl;
    SetDataPath(_dataPath);
  }
  // setup firt lepton, for all the channels
  _EffSet[0]->SetupEff(aReal);
  _EffSet[1]->SetupEff(aFake);
  // setup the second lepton in the same way as the first one for ee and mumu, but following aReal2/aFake2 for emu
  if(channel==EE || channel==EMU || channel==MUMU){
    if(aReal2!="") _EffSet[2]->SetupEff(aReal2);
    else _EffSet[2]->SetupEff(aReal);
    if(aFake2!="") _EffSet[3]->SetupEff(aFake2);
    else _EffSet[3]->SetupEff(aFake);
  }
}

// =======================================================
float FakesWeights::GetFakesWeightLJets(bool tight){
  // retrieve MM weight for semileptonic events

  float f = GetFakeEff(); // fake efficiency of lepton
  float r = GetRealEff(); // real efficiency of lepton
  if(f<0 || r<0) return 0.;  // added for events which are neglected by trigger splitting

  double nf, nr;
  // 
  float weight = _mmUtil->lepton_weight(!tight, r, f, (double&)nf, (double&)nr);
  // protection against too large weights : this should not happen
  // and is due in general to histo bins with large statistical fluctuations
  if( weight > 5 || weight < -5 ){
    r = _EffSet[0]->GetEff(true);
    f = _EffSet[1]->GetEff(true);
    weight = _mmUtil->lepton_weight(!tight, r, f, (double&)nf, (double&)nr);
  }
  
  // scale by prescales! FIXME
  if(_EffSet[0]->GetPrescale()!=1) weight *= _EffSet[0]->GetPrescale();

  return weight;
}

// =======================================================
float FakesWeights::GetFakesWeightDilep(bool tight1,bool tight2){
  // retrieve MM weight for dilepton events
  
  float f1 = GetFakeEff();    // fake efficiency of 1st lepton
  float r1 = GetRealEff();    // real efficiency of 1st lepton
  float f2 = GetFakeEff2();   // fake efficiency of 2nd lepton
  float r2 = GetRealEff2();   // real efficiency of 2nd lepton
  if(f1<0 || f2<0 || r1<0 || r2<0) return 0.;  // added for events which are neglected by trigger splitting

  double nrf, nfr, nff;
  // fill the 4*4 matrix with efficiencies
  _mmUtil->fill_matrix(r1,f1,r2,f2);
  // retrieve the weights
  return _mmUtil->dilepton_weight(!tight1, r1, f1, !tight2, r2, f2, (double&)nrf, (double&)nfr, (double&)nff);
}

// =======================================================
float FakesWeights::GetFakesWeightDilepLL(bool tight1,bool tight2){
  // retrieve MM weight for dilepton events (loose-loose)
  return _mmUtil->dilepton_LL_weight(!tight1,!tight2);
}

// =======================================================
float FakesWeights::GetFakesWeightDilepLT(bool tight1,bool tight2){
  // retrieve MM weight for dilepton events (loose-tight)
  return _mmUtil->dilepton_LT_weight(!tight1,!tight2);
}

// =======================================================
float FakesWeights::GetFakesWeightDilepTL(bool tight1,bool tight2){
  // retrieve MM weight for dilepton events (tight-loose)
  return _mmUtil->dilepton_TL_weight(!tight1,!tight2);
}

// =======================================================
float FakesWeights::GetFakesWeightDilepTT(bool tight1,bool tight2){
  // retrieve MM weight for dilepton events (tight-tight)
  return _mmUtil->dilepton_TT_weight(!tight1,!tight2);
}

// =======================================================
float FakesWeights::GetRealEff(){
  // method to get the real efficiency 
  float R = _EffSet[0]->GetEff();
  return R;
}

// =======================================================
float FakesWeights::GetFakeEff(){
  // method to get the fake efficiency 
  float F = _EffSet[1]->GetEff();
  return F;
}

// =======================================================
float FakesWeights::GetRealEff2(){
  // method to get the real efficiency (of 2nd lepton)
  float R = _EffSet[2]->GetEff();
  return R;
}

// =======================================================
float FakesWeights::GetFakeEff2(){
  // method to get the real efficiency (of 2nd lepton)
  float F = _EffSet[3]->GetEff();
  return F;
}

// =======================================================
void FakesWeights::SetLepton(MMEvent event,MMLepton lepton){
  // set the MMEvent and MMLepton objects for l+jet case
  _EffSet[0]->SetLepton(event,lepton);
  _EffSet[1]->SetLepton(event,lepton);
}

// =======================================================
void FakesWeights::SetLeptons(MMEvent event,MMLepton lepton1,MMLepton lepton2){
  // set the MMEvent and MMLepton objects for dilepton case

  // hack by Finn & Xiaowen to treat the trigger bias in the correct way
  //   a. if any lepton in the event matches the high pt trigger  
  if( (lepton1.trigger&2)>0 || (lepton2.trigger&2)>0 ){
    //   -- > apply pt dependent nonbiased efficiencies ( e.g. e24vh if pt<61 and e60 if pt>61 ) 
    //   ~~ > hack: switch the biased trigger off ;) 
    if( (lepton1.trigger&1)>0 ) {
      lepton1.trigger -= 1;
      if( (lepton1.trigger&4)==0 ) lepton1.trigger += (1 << 2);
    }
    if( (lepton2.trigger&1)>0 ){
      lepton2.trigger -= 1;
      if( (lepton2.trigger&4)==0 ) lepton2.trigger += (1 << 2);
    }
  }
  //   b. if no lepton in the event matches this trigger 
  else{
    //   -- > apply the low pt biased trigger efficiency ( e24vhi ) to one lepton that matches this trigger and pt dependent unbiased efficiencies to the others ( e.g. e24vh if pt<61 and e60 if pt>61 )
    //   -- > if more than one lepton matches e24vhi than apply that efficiency to the highest pt one and the unbiased e24vh to the other(s). 
    //   ~~ > hack: if both leptons are matched to the biased trigger, switch off the trigger for the lowest pt one
    if( (lepton1.trigger&1)>0 && (lepton2.trigger&1)>0 ){
      if( lepton1.pt > lepton2.pt ) lepton2.trigger -= 1;
      if( lepton2.pt > lepton1.pt ) lepton1.trigger -= 1;
    }
  }
  
  _EffSet[0]->SetLepton(event,lepton1);
  _EffSet[1]->SetLepton(event,lepton1);
  _EffSet[2]->SetLepton(event,lepton2);
  _EffSet[3]->SetLepton(event,lepton2);
}

// =======================================================
MatrixUtils* FakesWeights::GetMatrixUtils(){
  // retrieve the matrix
  return _mmUtil;
}

// =======================================================
void FakesWeights::SetupWeighterDefault(int channel, std::string syst){
  //
  // default set up
  //

  // default efficiencies
  if     (syst=="")          _syst = NONE;
  // fake eff with removal of real lepton contamination from W/Z MC +30% - correlates electron and muon
  else if(syst=="MCup")      _syst = MCup;
  // fake eff with removal of real lepton contamination from W/Z MC -30% - correlates electron and muon
  else if(syst=="MCdown")    _syst = MCdown;
  // fake eff with removal of real lepton contamination from W/Z MC +30% - only for electron
  else if(syst=="MCupel")    _syst = MCupel;
  // fake eff with removal of real lepton contamination from W/Z MC -30% - only for electron
  else if(syst=="MCdownel")  _syst = MCdownel;
  // fake eff with removal of real lepton contamination from W/Z MC +30% - only for muon
  else if(syst=="MCupmu")    _syst = MCupmu;
  // fake eff with removal of real lepton contamination from W/Z MC -30% - only for muon
  else if(syst=="MCdownmu")  _syst = MCdownmu;
  // fake eff from alternative region - returns nominal for emu channel, otherwise equivalent to CRfakeel (CRfakemu) for e+jets and ee (mu+jets and mumu) channels
  else if(syst=="CRfake")    _syst = CRfake;
  // fake eff from alternative region (only for electrons, useful for dilepton)
  else if(syst=="CRfakeel")  _syst = CRfakeel;
  // fake eff from alternative region (only for muons, useful for dilepton)
  else if(syst=="CRfakemu")  _syst = CRfakemu;
  // real eff from alternative region - correlates electron and muon
  else if(syst=="CRreal")    _syst = CRreal;
  // real eff from alternative region - only for electron
  else if(syst=="CRrealel")  _syst = CRrealel;
  // real eff from alternative region - only for muon
  else if(syst=="CRrealmu")  _syst = CRrealmu;
  // efficiencies with alternative parametrisation (only for l+jet)
  else if(syst=="EffPar")    _syst = EffPar;
  else {
    std::cout << " FakesWeights::ERROR: Specified systematic NOT supported: " << syst << std::endl;
    return;
  }

  // E + JETS
  if(channel == FakesWeights::EJETS){
    if(_syst==NONE)                           SetupWeighter( FakesWeights::EJETS, "Default:NS:eta:pt:jetpt:dR", "Default:NB/NS:eta:pt:jetpt:dR");
    else if(_syst==MCup   || _syst==MCupel)   SetupWeighter( FakesWeights::EJETS, "Default:NS:eta:pt:jetpt:dR", "Default_MCup:NB/NS:eta:pt:jetpt:dR");
    else if(_syst==MCdown || _syst==MCdownel) SetupWeighter( FakesWeights::EJETS, "Default:NS:eta:pt:jetpt:dR", "Default_MCdown:NB/NS:eta:pt:jetpt:dR");
    else if(_syst==CRreal || _syst==CRrealel) SetupWeighter( FakesWeights::EJETS, "Default_CRreal:NB:eta:pt:jetpt:dR", "Default:NB/NS:eta:pt:jetpt:dR");
    else if(_syst==CRfake || _syst==CRfakeel) SetupWeighter( FakesWeights::EJETS, "Default:NS:eta:pt:jetpt:dR", "Default_CRfake:NB/NS:eta:pt:jetpt:dR");
    else if(_syst==EffPar)                    SetupWeighter( FakesWeights::EJETS, "Default:NS:eta:pt:jetpt:dR:dPhi", "Default:NB/NS:eta:pt:jetpt:dR:dPhi");
    else if(_syst==MCupmu || _syst==MCdownmu ||  _syst==CRrealmu || _syst==CRfakemu ) {
      std::cout << " FakesWeights::WARNING: Requested muon-specific systematic "<<syst<<" in e+jets channel - using nominal instead" << std::endl;
      SetupWeighterDefault(FakesWeights::EJETS);
    }    
  }

  // MU + JETS
  else if(channel == FakesWeights::MUJETS){
   if(_syst==NONE)                           SetupWeighter( FakesWeights::MUJETS, "Default:NS/TR:eta:pt:jetpt:dR", "Default:NB/NS/TR:eta:pt:jetpt:dR");
   else if(_syst==MCup   || _syst==MCupmu)   SetupWeighter( FakesWeights::MUJETS, "Default:NS/TR:eta:pt:jetpt:dR", "Default_MCup:NB/NS/TR:eta:pt:jetpt:dR");
   else if(_syst==MCdown || _syst==MCdownmu) SetupWeighter( FakesWeights::MUJETS, "Default:NS/TR:eta:pt:jetpt:dR", "Default_MCdown:NB/NS/TR:eta:pt:jetpt:dR");
   else if(_syst==CRreal || _syst==CRrealmu) SetupWeighter( FakesWeights::MUJETS, "Default_CRreal:NB/TR:eta:pt:jetpt:dR","Default:NB/NS/TR:eta:pt:jetpt:dR");
   else if(_syst==CRfake || _syst==CRfakemu) SetupWeighter( FakesWeights::MUJETS, "Default:NS/TR:eta:pt:jetpt:dR", "Default_CRfake:NB/NS/TR:eta:pt:jetpt:dR");
   else if(_syst==EffPar)                    SetupWeighter( FakesWeights::MUJETS, "Default:NS/TR:eta:pt:jetpt:dR:dPhi", "Default:NB/NS/TR:eta:pt:jetpt:dR:dPhi");
   else if(_syst==MCupel || _syst==MCdownel ||  _syst==CRrealel || _syst==CRfakeel ) {
     std::cout << " FakesWeights::WARNING: Requested electron-specific systematic "<<syst<<" in mu+jets channel - using nominal instead" << std::endl;
     SetupWeighterDefault(FakesWeights::MUJETS);
   }
  }

  // E E
  else if(channel == FakesWeights::EE){
    if(_syst==NONE)                            SetupWeighter( FakesWeights::EE, "Default:NS:eta:pt:jetpt:dR", "Default:NB/NS:eta:pt:jetpt:dR");
    else if (_syst==MCup   || _syst==MCupel)   SetupWeighter( FakesWeights::EE, "Default:NS:eta:pt:jetpt:dR", "Default_MCup:NB/NS:eta:pt:jetpt:dR");
    else if (_syst==MCdown || _syst==MCdownel) SetupWeighter( FakesWeights::EE, "Default:NS:eta:pt:jetpt:dR", "Default_MCdown:NB/NS:eta:pt:jetpt:dR");
    else if (_syst==CRreal || _syst==CRrealel) SetupWeighter( FakesWeights::EE, "Default_CRreal:NB/TR:eta:pt:jetpt:dR", "Default:NB/NS:eta:pt:jetpt:dR");
    else if (_syst==CRfake || _syst==CRfakeel) SetupWeighter( FakesWeights::EE, "Default:NS:eta:pt:jetpt:dR", "Default_CRfake:NB/NS:eta:pt:jetpt:dR");
    else if(_syst==EffPar) {
      std::cout << " FakesWeights::WARNING: Requested systematic "<<syst<<" is not relevant for dilepton channel - using nominal instead" << std::endl;
      SetupWeighterDefault(FakesWeights::EE);
    }
    else if(_syst==MCupmu || _syst==MCdownmu ||  _syst==CRrealmu || _syst==CRfakemu ) {
      std::cout << " FakesWeights::WARNING: Requested muon-specific systematic "<<syst<<" in ee channel - using nominal instead" << std::endl;
      SetupWeighterDefault(FakesWeights::EE);
    }
  }

  // MU MU
  else if(channel == FakesWeights::MUMU){
    if(_syst==NONE)                            SetupWeighter( FakesWeights::MUMU, "Default:NS/TR:eta:pt:jetpt:dR", "Default:NB/NS/TR:eta:pt:jetpt:dR");
    else if (_syst==MCup   || _syst==MCupmu)   SetupWeighter( FakesWeights::MUMU, "Default:NS/TR:eta:pt:jetpt:dR", "Default_MCup:NB/NS/TR:eta:pt:jetpt:dR");
    else if (_syst==MCdown || _syst==MCdownmu) SetupWeighter( FakesWeights::MUMU, "Default:NS/TR:eta:pt:jetpt:dR", "Default_MCdown:NB/NS/TR:eta:pt:jetpt:dR");
    else if (_syst==CRreal || _syst==CRrealmu) SetupWeighter( FakesWeights::MUMU, "Default_CRreal:NB/TR:eta:pt:jetpt:dR", "Default:NB/NS/TR:eta:pt:jetpt:dR");
    else if (_syst==CRfake || _syst==CRfakemu) SetupWeighter( FakesWeights::MUMU, "Default:NS/TR:eta:pt:jetpt:dR", "Default_CRfake:NB/NS/TR:eta:pt:jetpt:dR");
    else if(_syst==EffPar) {
      std::cout << " FakesWeights::WARNING: Requested systematic "<<syst<<" is not relevant for dilepton channel - using nominal instead" << std::endl;
      SetupWeighterDefault(FakesWeights::MUMU);
    }
    else if(_syst==MCupel || _syst==MCdownel ||  _syst==CRrealel || _syst==CRfakeel ) {
      std::cout << " FakesWeights::WARNING: Requested electron-specific systematic "<<syst<<" in mumu channel - using nominal instead" << std::endl;
      SetupWeighterDefault(FakesWeights::MUMU);
    }
  }

  // E MU
  else if(channel == FakesWeights::EMU){
    if(_syst==NONE) {
      SetupWeighter( FakesWeights::EMU,  
		     "Default:NS:eta:pt:jetpt:dR", "Default:NB/NS:eta:pt:jetpt:dR", 
		     "Default:NS/TR:eta:pt:jetpt:dR", "Default:NB/NS/TR:eta:pt:jetpt:dR");
    }
    else if (_syst==MCup) {
      std::cout << " FakesWeights::WARNING: Requested systematic MCup in emu channel will consider this variation totally correlated between the electron and the muon" << std::endl;
      std::cout << " FakesWeights::WARNING: Please use MCupel and MCupmu to get the systematic variation only for electron and muon, respectively" << std::endl;
      SetupWeighter( FakesWeights::EMU,  
		     "Default:NS:eta:pt:jetpt:dR", "Default_MCup:NB/NS:eta:pt:jetpt:dR", 
		     "Default:NS/TR:eta:pt:jetpt:dR", "Default_MCup:NB/NS/TR:eta:pt:jetpt:dR");
    }
    else if (_syst==MCdown) {
      std::cout << " FakesWeights::WARNING: Requested systematic MCdown in emu channel will consider this variation totally correlated between the electron and the muon" << std::endl;
      std::cout << " FakesWeights::WARNING: Please use MCdownel and MCdownmu to get the systematic variation only for electron and muon, respectively" << std::endl;
      SetupWeighter( FakesWeights::EMU,  
		     "Default:NS:eta:pt:jetpt:dR", "Default_MCdown:NB/NS:eta:pt:jetpt:dR", 
		     "Default:NS/TR:eta:pt:jetpt:dR", "Default_MCdown:NB/NS/TR:eta:pt:jetpt:dR");
    }
    else if (_syst==MCupel) {
      std::cout << " FakesWeights::WARNING: Requested systematic MCupel in emu channel will consider this variation only for the electron" << std::endl;
      std::cout << " FakesWeights::WARNING: Please use MCup instead to get this systematic variation for both electron and muon totally correlated" << std::endl;
      SetupWeighter( FakesWeights::EMU,  
		     "Default:NS:eta:pt:jetpt:dR", "Default_MCup:NB/NS:eta:pt:jetpt:dR", 
		     "Default:NS:eta:pt:jetpt:dR", "Default:NB/NS:eta:pt:jetpt:dR");
    }
    else if (_syst==MCdownel) {
      std::cout << " FakesWeights::WARNING: Requested systematic MCdownel in emu channel will consider this variation only for the electron" << std::endl;
      std::cout << " FakesWeights::WARNING: Please use MCdown instead to get this systematic variation for both electron and muon totally correlated" << std::endl;
      SetupWeighter( FakesWeights::EMU,  
		     "Default:NS:eta:pt:jetpt:dR", "Default_MCdown:NB/NS:eta:pt:jetpt:dR", 
		     "Default:NS/TR:eta:pt:jetpt:dR", "Default:NB/NS/TR:eta:pt:jetpt:dR");
    }
    else if (_syst==MCupmu) {
      std::cout << " FakesWeights::WARNING: Requested systematic MCupmu in emu channel will consider this variation only for the muon" << std::endl;
      std::cout << " FakesWeights::WARNING: Please use MCup instead to get this systematic variation for both electron and muon totally correlated" << std::endl;
      SetupWeighter( FakesWeights::EMU,  
		     "Default:NS:eta:pt:jetpt:dR", "Default:NB/NS:eta:pt:jetpt:dR", 
		     "Default:NS/TR:eta:pt:jetpt:dR", "Default_MCup:NB/NS/TR:eta:pt:jetpt:dR");
    }
    else if (_syst==MCdownmu) {
      std::cout << " FakesWeights::WARNING: Requested systematic MCdownmu in emu channel will consider this variation only for the muon" << std::endl;
      std::cout << " FakesWeights::WARNING: Please use MCdown instead to get this systematic variation for both electron and muon totally correlated" << std::endl;
      SetupWeighter( FakesWeights::EMU,  
		     "Default:NS:eta:pt:jetpt:dR", "Default:NB/NS:eta:pt:jetpt:dR", 
		     "Default:NS/TR:eta:pt:jetpt:dR", "Default_MCdown:NB/NS/TR:eta:pt:jetpt:dR");
    }
    else if (_syst==CRfake) {
      std::cout << " FakesWeights::WARNING: Requested systematic CRfake in emu channel is not possible - using nominal instead" << std::endl;
      std::cout << " FakesWeights::WARNING: Please use CRfakeel and CRfakemu to get the systematic variation only for electron and muon, respectively" << std::endl;
      SetupWeighterDefault(FakesWeights::EMU);
    }
    else if (_syst==CRfakeel) {
      SetupWeighter( FakesWeights::EMU,  
		     "Default:NS:eta:pt:jetpt:dR", "Default_CRfake:NB/NS:eta:pt:jetpt:dR", 
		     "Default:NS/TR:eta:pt:jetpt:dR", "Default:NB/NS/TR:eta:pt:jetpt:dR");
    }
    else if (_syst==CRfakemu) {
      SetupWeighter( FakesWeights::EMU,  
		     "Default:NS:eta:pt:jetpt:dR", "Default:NB/NS:eta:pt:jetpt:dR", 
		     "Default:NS/TR:eta:pt:jetpt:dR", "Default_CRfake:NB/NS/TR:eta:pt:jetpt:dR");
    }
    else if (_syst==CRreal) {
      std::cout << " FakesWeights::WARNING: Requested systematic CRreal in emu channel will consider this variation totally correlated between the electron and the muon" << std::endl;
      std::cout << " FakesWeights::WARNING: Please use CRrealel and CRrealmu to get the systematic variation only for electron and muon, respectively" << std::endl;
      SetupWeighter( FakesWeights::EMU,  
		     "Default_CRreal:NB/TR:eta:pt:jetpt:dR", "Default:NB/NS:eta:pt:jetpt:dR", 
		     "Default_CRreal:NB/TR:eta:pt:jetpt:dR","Default:NB/NS/TR:eta:pt:jetpt:dR");
    }
    else if (_syst==CRrealel) {
      std::cout << " FakesWeights::WARNING: Requested systematic CRrealel in emu channel will consider this variation only for the electron" << std::endl;
      std::cout << " FakesWeights::WARNING: Please use CRreal instead to get this systematic variation for both electron and muon totally correlated" << std::endl;
      SetupWeighter( FakesWeights::EMU,  
		     "Default_CRreal:NB/TR:eta:pt:jetpt:dR", "Default:NB/NS:eta:pt:jetpt:dR", 
		     "Default:NS/TR:eta:pt:jetpt:dR", "Default:NB/NS/TR:eta:pt:jetpt:dR");
    }
    else if (_syst==CRrealmu) {
      std::cout << " FakesWeights::WARNING: Requested systematic CRrealmu in emu channel will consider this variation only for the muon" << std::endl;
      std::cout << " FakesWeights::WARNING: Please use CRreal instead to get this systematic variation for both electron and muon totally correlated" << std::endl;
      SetupWeighter( FakesWeights::EMU,  
		     "Default:NS:eta:pt:jetpt:dR", "Default:NB/NS:eta:pt:jetpt:dR", 
		     "Default_CRreal:NB/TR:eta:pt:jetpt:dR","Default:NB/NS/TR:eta:pt:jetpt:dR");
    }
    else if(_syst==EffPar) {
      std::cout << " FakesWeights::WARNING: Requested systematic "<<syst<<" is not relevant for dilepton channel - using nominal instead" << std::endl;
      SetupWeighterDefault(FakesWeights::EMU);
    }
  }
  else  std::cout << "ERROR: Unsupported channel number: " << channel << std::endl;
}

// =======================================================
// for backward compatibility
void FakesWeights::SetupWeighterDefault(int channel,std::string name,std::string syst){
  std::cout << " FakesWeights::INFO: Specifying " << name << " not needed anymore." << std::endl;
  SetupWeighterDefault(channel,syst);
}

// =======================================================
void FakesWeights::SetLeptonDefault(float lep_pt, float lep_eta, 
                                    float el_cl_eta, float dR_lj_min, 
                                    float dPhi_lnu, float jet_pt0, 
                                    int jet_n, int nJet_tagged, int trigger)
{
  //
  // default method to setup MMLepton and MMEvent objects
  //

  // turn eta's into abs value, just to be sure
  lep_eta = TMath::Abs(lep_eta);
  el_cl_eta = TMath::Abs(el_cl_eta);
  //
  MMLepton lepton;
  lepton.pt      = lep_pt;
  lepton.eta     = lep_eta;
  lepton.cleta   = el_cl_eta;
  lepton.dR      = dR_lj_min;
  lepton.dPhi    = dPhi_lnu; // NEW!!
  lepton.trigger = trigger;
  MMEvent event;
  event.njets = jet_n;
  event.ntag  = nJet_tagged;
  event.jetpt = jet_pt0;
  SetLepton(event,lepton);
}

// =======================================================
float FakesWeights::GetFakesWeightLJetsDefault(bool tight, float lep_pt, 
                                               float lep_eta, float el_cl_eta, 
                                               float dR_lj_min, 
                                               float dPhi_lnu, // NEW!!
                                               float jet_pt0, int jet_n, 
                                               int nJet_tagged, int trigger) 
{
  
  SetLeptonDefault(lep_pt,lep_eta,el_cl_eta,dR_lj_min,dPhi_lnu,
                   jet_pt0,jet_n,nJet_tagged,trigger);
  return GetFakesWeightLJets(tight);
}
