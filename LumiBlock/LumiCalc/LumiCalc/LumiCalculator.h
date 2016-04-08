/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef LUMICALC_LUMICALCULATOR_H
#define LUMICALC_LUMICALCULATOR_H


#include "LumiCalc/CoolQuery.h"
#include "LumiCalc/LumiBlockRangeContainerConverter.h"
#include <vector>
#include "TH1F.h"
#include "TTree.h"

namespace Root {
  class TGoodRunsList;
}

//class TTree;
// MB 20100115: turn off for now, RootGraphics lib crashes on some non-cern sites.
//class TCanvas;

class LumiCalculator{

 public:
  LumiCalculator();
  ~LumiCalculator();

  void UseMC(bool mc = true); // No longer supported
  void UseLArNoiseDB(bool lar, const std::string& lardb); // Calculate LAr defect fraction 
  void UseBeamspot(bool bs, const std::string& bstag); // Calculate online beamspot validity fraction
  void UseOnlineLumi(bool online);
  void Verbose(bool verbose = true);
  void UseLumiTag(const std::string& tag);// i.e. COOL Folder tag
  void UseLumiMethod(const std::string& method);// i.e. COOL Folder channel
  void UseLumiChannel(int chan);
  void UseLiveTrigger(bool live, std::string& livetrigger);
  void IntegrateLumi(const xAOD::LumiBlockRangeContainer * iovc, const std::string& triggerchain);
  void SetCollName(const std::string& lbcollname);
  void setTree(TTree * tree = 0);

  TTree *getTree();

  inline void MakePlots(bool plots) { m_makePlots = plots; }
  inline void MakeCollList(bool collList) { m_makecollList = collList; }
  inline void ScaleL1TrigRate( const float& rate ) { m_effxsec=1./rate; }
  inline void SetMinL1TrigRate( const float& mintrigrate ) { m_mintrigrate=mintrigrate; }

  // Print terse summary of results
  void printSummary(std::ostream& os);

 private:

  void DoHistogramAdmin(const uint32_t& runnbr, const TString& trigName, const float& effxsec);
  void SetHistogramStyle(TH1F* hist, const char* title=0, const char* xaxis=0, const char* yaxis=0);
  void RebinHistograms(const int& nbins, const double& start, const double& end);
  void MakePlots(const std::string& triggerchain);

  void LoadChannelIds();

  //TCanvas* GetNiceCanvas(const char* name="tcan", const char* title="tcan");

  // A TTree to bookkeep the calculation
  TTree * m_LumiTree;
  bool m_recordTTree;

  // State of LumiCalculator: True - OK, False - something went bad..
  bool m_State;


  std::string lumi_database;
  std::string trig_database;
  std::string lar_database;
  std::string bs_database;

  std::string m_trigger;
  std::string m_livetrigger;
  std::string m_lumioff;
  std::string m_lumionl;
  std::string m_data_db;
  std::string m_lumitag;
  std::string m_lumimethod;
  std::string m_laroff;
  std::string m_lartag;
  std::string m_bsonl;
  std::string m_bstag;
  int m_lumichannel;
  std::vector<uint32_t> lbstart;
  std::vector<uint32_t> lbend;

  std::string m_parofflumiestfolder;
  std::string m_paronllumiestfolder;
  std::string m_parlumiestfolder;
  std::string m_parlvl1menufolder;
  std::string m_parhltmenufolder;
  std::string m_parhltprescalesfolder;
  std::string m_parlumilvl1folder;
  std::string m_parlumihltfolder;
  std::string m_parlvl1prescalesfolder;
  std::string m_parlvl1lblbfolder;
  std::string m_parlareventvetofolder;
  std::string m_paronlbeamspotfolder;

  Root::TMsgLogger m_logger;
  std::string m_lbcollname;
  bool m_uselivetrigger;
  bool m_verbose;
  


  // Internal variables

  double m_lbstarttime;  // In seconds since start of epoch
  double m_lbendtime;

  float m_totalDelL;
  float m_totalL;
  float m_totalLRun;
  float m_totaltime;
  float m_instLumi;
  float m_AvEvtsPerBX;
  cool::UInt32 m_Valid;
  float m_delLumi;
  float m_intLumi;
  float m_deltaT;
  float m_TotaldeltaT;
  float m_livefrac;
  float m_livetime;
  unsigned int m_l1acc;
  unsigned int m_livetime_l1acc;
  bool m_l1accof;
  unsigned int m_l2acc;
  unsigned int m_l3acc;
  unsigned int m_totall1acc;
  unsigned int m_livtrig_totall1acc;
  unsigned int m_totall1befpresc;
  unsigned int m_totall2acc;
  unsigned int m_totall3acc;
  float m_l1prescale;
  float m_l2prescale;
  float m_l3prescale;
  ULong64_t m_afterprescale;
  ULong64_t m_livetime_beforeprescale;
  ULong64_t m_livetime_afterprescale;
  bool m_afterprescaleof;
  ULong64_t m_beforeprescale;
  bool m_beforeprescaleof;
  uint32_t m_runnbr;
  uint32_t m_lbstart;
  uint32_t m_lbstop;
  uint32_t m_lbstart_prev;
  uint32_t m_lbstop_prev;
  uint32_t m_runnbr_prev;
  unsigned int m_totalgoodblock;
  unsigned int m_totalbadblock;
  uint32_t m_clumiblocknbr;
  uint32_t m_clumiblocknbrend;
  unsigned int m_triglevel;
  float m_totalPrescaleWLiveTime;
  float mt_totalPrescaleWLiveTime;
  float m_totalPrescale;
  float mt_totalPrescale;
  float m_lumiWOPrescale;
  float mt_lumiWOPrescale;
  float m_lumiLAr;
  float mt_lumiLAr;
  float mt_totalDelL;
  float mt_totalL;
  float mt_totalLRun;
  float mt_totaltime;
  float mt_deltaT;
  unsigned int mt_l1acc;
  unsigned int mt_l2acc;
  unsigned int mt_l3acc;
  unsigned int mt_totalgoodblock;
  unsigned int mt_totalbadblock;
  unsigned int mt_totall1befpresc;    
  float m_lartime;
  float m_larfrac;
  float m_bsvalid;
  
  std::string m_triggerchain;
  std::vector<std::string> m_triggerlowerchains;
  std::string m_L1triggerchain;
  std::string m_lbcollectionname;

  float m_effxsec;
  float m_l1rate;
  float m_l2rate;
  float m_l3rate;
  float m_l1ratediveffxsec;
  float m_total_l1ratediveffxsec;
  float m_total_l1ratediveffxsecRun;
  float m_l1ratediveffxsec_recorded;
  float m_total_l1ratediveffxsec_recorded;
  float m_total_l1ratediveffxsecRun_recorded;
  float m_mintrigrate;

  Root::TGoodRunsList* m_collsgrl;

  TH1F* m_ntrigplb;
  TH1F* m_trigrateplb;
  TH1F* m_lumiplb;
  TH1F* m_lumitrigrateplb;
  TH1F* m_intlumi;
  TH1F* m_intlumitrigrate;
  TH1F* m_lumitrigrateplb_recorded;
  TH1F* m_intlumitrigrate_recorded;

  TH1F* m_intlumiruns;
  TH1F* m_intlumitrigrateruns;
  TH1F* m_intlumitrigrateruns_recorded;

  TH1F* m_avgintperbx;

  std::vector<TH1F*> m_ntrigplbVec;
  std::vector<TH1F*> m_trigrateplbVec;
  std::vector<TH1F*> m_lumiplbVec;
  std::vector<TH1F*> m_lumitrigrateplbVec;
  std::vector<TH1F*> m_intlumiVec;
  std::vector<TH1F*> m_intlumitrigrateVec;
  std::vector<TH1F*> m_lumitrigrateplb_recordedVec;
  std::vector<TH1F*> m_intlumitrigrate_recordedVec;

  bool m_makePlots;
  bool m_makecollList;

  cool::ChannelId Lumiid;
  cool::ChannelId L3id;
  cool::ChannelId L2id;
  cool::ChannelId L1id;
  cool::ChannelId LiveL1id;

  bool L1Valid;
  bool L2Valid;
  bool L3Valid;
  bool LiveValid;

  bool m_onlinelumi;
  bool m_uselar;
  bool m_usebs;

  unsigned int m_minrun;
  unsigned int m_maxrun;
};


#endif //> LUMICALC_LUMICALCULATOR_H
