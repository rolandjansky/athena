/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/*
 ** authors:
 **   Michele Pinamonti <michele.pinamonti@cern.ch>
*/

#ifndef MMEFFSET_h
#define MMEFFSET_h

#include <string>
#include <vector>
#include <map>

class TFile;
class TH1F;
class TH2F;
class TH3F;
class MMEfficiency;
class MatrixUtils;


//----
class MMLepton{
public:
  float pt;
  float eta;
  float cleta;
  float dR;    // min dR(jet-lepton)
  float dRpt;  // pT/dR (lepton-closest jet)
  float dPhi;  // dPhi(MET-lepton)
  float d0sig; // d0 significance
  int trigger; // trigger matching the lepton
};

class MMEvent{
public:
  float jetpt;  // pT of the leading jet
  float sumet;  // met_sumet
  float hthad;  // sum of pT's of jets
  float met;    // met_et in the event
  int njets;    // number of jets
  int ntag;     // number of b-tagged jets (MV1 70%)
};
//----

  

class MMEffSet{

public:

  enum{
    TRIGGER_MASK   =1,
    NJETSSHORT_MASK=2,
    NJETS_MASK     =4,
    NBTAG_MASK     =8
  };

  // "period"
  enum Period {
    Year2012, // 0
    Year2015,
    Year2016
  };

  MMEffSet(bool isEle,bool isReal,bool debug=false);
  ~MMEffSet();
  
  void SetDataPath(std::string path="data");

  void SetupEff(std::string aEff);

  void AddCustomSplit(std::string config); // config = "condition1:condition2:condition3_..." -> value = 0,1,2,...
  void AddNumBtagSplit();
  void AddNumJetsSplit();
  void AddNumJetsShortSplit();
  void AddTriggerSplit();
  void AddNumBtagAndTriggerSplit();
  void AddNumBtagAndNumJetsSplit();
  void AddNumBtagAndNumJetsShortSplit();
  void AddNumJetsAndTriggerSplit();
  void AddNumJetsShortAndTriggerSplit();
  void AddNumBtagAndNumJetsAndTriggerSplit();
  void AddNumBtagAndNumJetsShortAndTriggerSplit();
  void ReadEfficiencies(); // to call after having set up and added splittings
  
  void AddCorrection();
  
  void SetLepton(float v0,float v1=0,float v2=0,float v3=0,float v4=0,float v5=0,float v6=0,float v7=0,float v8=0,float v9=0);
  
  void SetCustom(int value);
  void SetNumBtag(int nbtag);
  void SetNumJets(int njets);
  void SetNumJetsShort(int njets);
  void SetTrigger(int trigger,float pt); // pT in GeV
  
  void SetNumBtagAndNumJets(int ntag,int njets);
  void SetNumBtagAndNumJetsShort(int ntag,int njets);
  void SetNumBtagAndTrigger(int nbtag,int trigger,float pt);
  void SetNumJetsAndTrigger(int njets,int trigger,float pt);
  void SetNumJetsShortAndTrigger(int njets,int trigger,float pt);
  void SetNumBtagAndNumJetsAndTrigger(int nbtag,int njets,int trigger,float pt);
  void SetNumBtagAndNumJetsShortAndTrigger(int nbtag,int njets,int trigger,float pt);
  
  void SetPtThres(int thres);  // set the pT-threshold for trigger splitting (in GeV); set it to -1 to ignore it
  void SetTriggerNames(std::string trig1,std::string trig2,std::string trig3);

  int TriggerValue(int trigger,float pt);

  int NumJetsValue(int njets);
  int NumJetsShortValue(int njets);
  int NumBtagValue(int nbtag);
  
  void SetLepton(MMEvent event,MMLepton lepton);
  float GetEff(bool isAverage=false);
  
  void SetPeriod(int period);
  void SetTriggerPrescales(float ps1,float ps2,float ps3);
  void SetTriggerThreshold(float thres);
  
  float GetPrescale();
  
private:
  void ReadEfficiency(MMEfficiency *eff,std::string aEff,std::string condition="",bool isCorrection=false);
  void ParseOptions(std::string s,std::map<std::string,std::string> *m);
  
  MMEvent _event;
  MMLepton _lepton;

  std::string _Estimate;
  
  std::string _dataPath;

  std::map<std::string,std::string> _optMap;

  int _numCond;
  
  int _splitIdx;
  
  int _splitBitMask;    //     nbtag-njet-njetshort-trigger (eg. 1000, 1010, 0011..)
  
  float _ptTres;
  
  std::vector<std::string> _condVec;

  // separate class for efficiencies
  MMEfficiency *_Eff[10];
  MMEfficiency *_EffCorr[10];

  bool _useCorr;
  
  bool _effIsRead;
  
  //int _estimate;
  //int _syst;
  
  bool _isEle;
  bool _isReal;
  bool _debug;
  
  int _period;
  std::string _trig1;
  std::string _trig2;
  std::string _trig3;
  float _ps1;
  float _ps2;
  float _ps3;  
  int _trigger;

  // Internal methods / functions
  int CountInString(std::string s,char c);
  std::vector<std::string> Tokenize(std::string s, std::string c);
  std::vector<std::string> Combine(std::vector<std::string> v1,std::vector<std::string> v2);
};

#endif

 
