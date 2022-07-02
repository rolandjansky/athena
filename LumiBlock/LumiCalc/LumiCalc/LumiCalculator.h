/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

#ifndef LUMICALC_LUMICALCULATOR_H
#define LUMICALC_LUMICALCULATOR_H

#include "CxxUtils/checker_macros.h"
#include "LumiCalc/CoolQuery.h"
#include "LumiCalc/LumiBlockRangeContainerConverter.h"
#include <vector>
#include "TH1F.h"
#include "TTree.h"

namespace Root {
  class TGoodRunsList;
}

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
  void IntegrateLumi ATLAS_NOT_THREAD_SAFE (const xAOD::LumiBlockRangeContainer * iovc, const std::string& triggerchain);
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

  void ParseL1Trigger(std::string lowerch, CoolQuery* cq_trigger);

  //TCanvas* GetNiceCanvas(const char* name="tcan", const char* title="tcan");

  // A TTree to bookkeep the calculation
  TTree * m_LumiTree;
  bool m_recordTTree;

  // State of LumiCalculator: True - OK, False - something went bad..
  bool m_State;


  std::string m_lumi_database;
  std::string m_trig_database;
  std::string m_lar_database;
  std::string m_bs_database;

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
  float m_t_totalPrescaleWLiveTime;
  float m_totalPrescale;
  float m_t_totalPrescale;
  float m_lumiWOPrescale;
  float m_t_lumiWOPrescale;
  float m_lumiLAr;
  float m_t_lumiLAr;
  float m_t_totalDelL;
  float m_t_totalL;
  float m_t_totalLRun;
  float m_t_totaltime;
  float m_t_deltaT;
  unsigned int m_t_l1acc;
  unsigned int m_t_l2acc;
  unsigned int m_t_l3acc;
  unsigned int m_t_totalgoodblock;
  unsigned int m_t_totalbadblock;
  unsigned int m_t_totall1befpresc;    
  float m_lartime;
  float m_larfrac;
  float m_bsvalid;
  
  std::string m_triggerchain;
  std::vector<std::string> m_triggerlowerchains;
  std::vector<std::string> m_L1triggerchains;  // Used for evaluating [L1_MU20,L1_MU21]
  std::vector<cool::ChannelId> m_L1idList;
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

  cool::ChannelId m_Lumiid;
  cool::ChannelId m_L3id;
  cool::ChannelId m_L2id;
  cool::ChannelId m_L1id;
  cool::ChannelId m_LiveL1id;

  bool m_L1Valid;
  bool m_L2Valid;
  bool m_L3Valid;
  bool m_LiveValid;

  bool m_onlinelumi;
  bool m_uselar;
  bool m_usebs;

  unsigned int m_minrun;
  unsigned int m_maxrun;
};


#endif //> LUMICALC_LUMICALCULATOR_H
