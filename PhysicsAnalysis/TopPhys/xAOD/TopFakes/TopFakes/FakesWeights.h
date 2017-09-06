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
//           2016-01-29 : FD: set _isRun1 = false 
//                        set default weighter
//                        remove pt/dR variable, never used
//////////////////////////////////////////////////////////////////////

#ifndef FAKESWEIGHTS_h
#define FAKESWEIGHTS_h

#include <string>
#include <vector>
#include <map>

#include "TopFakes/MMEffSet.h"

class TFile;
class TH1F;
class TH2F;
class TH3F;
class MMEfficiency;
class MMEffSet;
class MatrixUtils;
class MMEvent;
class MMLepton;

class FakesWeights{

public:
  // CONSTRUCTOR
  FakesWeights();
  // DESTRUCTOR
  ~FakesWeights();

  // channels
  enum {
    EJETS, // 0
    MUJETS, // 1
    EE, // 2
    MUMU, // 3
    EMU // 4
  };
  
//   // estimates
//   enum{
//     PARIS, // 0
//     UPPSALA
//   };

  // systematics
  enum{
    NONE, // 0
    MCup,  // fake eff with removal of real lepton contamination from W/Z MC +30% - totally correlated between electron and muon
    MCdown, // fake eff with removal of real lepton contamination from W/Z MC -30% - totally correlated between electron and muon
    MCupel,  // fake eff with removal of real lepton contamination from W/Z MC +30% - only for electron
    MCdownel, // fake eff with removal of real lepton contamination from W/Z MC -30% - only for electron
    MCupmu,  // fake eff with removal of real lepton contamination from W/Z MC +30% - only for muon
    MCdownmu, // fake eff with removal of real lepton contamination from W/Z MC -30% - only for muon
    CRfake,  // fake eff from alternative region - returns nominal for emu channel, otherwise equivalent to CRfakeel (CRfakemu) for e+jets and ee (mu+jets and mumu) channels
    CRfakeel,   // fake eff from alternative region - only for electron
    CRfakemu,   // fake eff from alternative region - only for muon
    CRreal, // real eff from alternative region - totally correlated between electron and muon
    CRrealel, // real eff from alternative region - only for electron
    CRrealmu, // real eff from alternative region - only for muon
    EffPar //efficiencies with alternative parametrisation (only for l+jet)    
  };
  
  //void SetProtection(bool protection);
  
  void SetDataPath(std::string path);
  void SetDebug(bool debug);

  void SetPeriod(int period);
  void SetTriggerNamesEl(std::string trig1,std::string trig2,std::string trig3);
  void SetTriggerNamesMu(std::string trig1,std::string trig2,std::string trig3);
  void SetTriggerPrescalesEl(float ps1,float ps2,float ps3);
  void SetTriggerPrescalesMu(float ps1,float ps2,float ps3);
  void SetTriggerThresholdEl(float thres);
  void SetTriggerThresholdMu(float thres);

  // method to set up all tools (Weighters) 
  void SetupWeighter(int channel, std::string aReal, std::string aFake, std::string aReal2="", std::string aFake2="");

  // method to set the MMEvent and MMLepton objects for l+jets
  void SetLepton(MMEvent event, MMLepton lepton);
  // method to set the MMEvent and MMLepton objects for dilepton
  void SetLeptons(MMEvent event,MMLepton lep1, MMLepton lep2); 
  // method to get the weight for l+jets
  float GetFakesWeightLJets(bool tight);
  // method to get the weight for dilepton
  float GetFakesWeightDilep(bool tight1,bool tight2);

  // methods to retrieve dilepton weight for loose-loose (LL), loose-tight (LT)
  // tigh-loose (TL) and tight-tight (TT) events
  float GetFakesWeightDilepLL(bool tight1,bool tight2);
  float GetFakesWeightDilepLT(bool tight1,bool tight2);
  float GetFakesWeightDilepTL(bool tight1,bool tight2);
  float GetFakesWeightDilepTT(bool tight1,bool tight2);

  // method to set up the weighters by default 
  void SetupWeighterDefault(int channel, std::string name, std::string syst);
  // method to set up the weighters by default 
  void SetupWeighterDefault(int channel, std::string syst="");

  void SetLeptonDefault(float lep_pt, float lep_eta, float el_cl_eta, 
                        float dR_lj_min, float dPhi_lnu, 
                        float jet_pt0, int jet_n, int nJet_tagged, int trigger);
  float GetFakesWeightLJetsDefault(bool tight, float lep_pt, float lep_eta, 
				   float el_cl_eta, float dR_lj_min, 
				   float dPhi_lnu, float jet_pt0, 
				   int jet_n, int nJet_tagged, int trigger);

  // methods to get the real and fake efficiencies
  float GetRealEff();
  float GetFakeEff();
  float GetRealEff2();
  float GetFakeEff2();

  // method to retrieve the matrix
  MatrixUtils* GetMatrixUtils();
  
  // method to access the channel
  int Channel() const {return _channel;};
  
private:
  int _channel;
  int _syst;
  
  bool _dataPathIsSet;
  bool _isInitialized;
  //bool _protection;
  
  bool _debug;
  std::string _dataPath;
    
  MatrixUtils *_mmUtil;

  MMEffSet *_EffSet[4]; // real, fake, real2, fake2
  
  int _period;
  std::string _trig1_el;
  std::string _trig2_el;
  std::string _trig3_el;
  std::string _trig1_mu;
  std::string _trig2_mu;
  std::string _trig3_mu;
  float _ps1_el;
  float _ps2_el;
  float _ps3_el;
  float _ps1_mu;
  float _ps2_mu;
  float _ps3_mu;
  float _ptTres_el;
  float _ptTres_mu;
};

#endif

