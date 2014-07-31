/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/////////////////////////////////////////////////////////////
//
//	Temporary copy of TrigMonitoring/TrigHLTMonitoring/TrigHLTMonitoring/IHLTMonTool
//      class, until IHLTMonTool moved to Control/AthenaMonitoring
//
//	date: Thu Jan  8 10:57:24 CET 2009
//	
//
/////////////////////////////////////////////////////////////
#ifndef IEgammaMONTOOL_H
#define IEgammaMONTOOL_H

#include <string>
#include <vector>

#include "AthenaMonitoring/AthenaMonManager.h"
#include "AthenaMonitoring/ManagedMonitorToolBase.h"
#include "StoreGate/StoreGateSvc.h"
//for lumi
#include "LumiBlockComps/CoolQuery.h"
#include "GaudiKernel/Property.h"

// Forward declarations
class IInterface;
class TH1;
class TProfile;
class TProfile2D;
class StatusCode;
class TTree;

///Concrete monitoring tool derived from MonitorToolBase
class IEgammaMonTool : public ManagedMonitorToolBase {

  public:

  IEgammaMonTool(const std::string & type, const std::string & name, const IInterface* parent); 
  virtual ~IEgammaMonTool();

  StatusCode initialize();

  // Book & fill & check, should NOT be over-written: over-write the ones below
  //-REMI append _old
  virtual StatusCode bookHistograms_old(bool isNewEventsBlock, bool isNewLumiBlock, bool isNewRun);
  virtual StatusCode fillHistograms();
  //-REMI append _old
  virtual StatusCode procHistograms_old(bool isEndOfEventsBlock, bool isEndOfLumiBlock, bool isEndOfRun);

  // Should be over-written
  virtual StatusCode init() = 0; // called by initialize
  virtual StatusCode book(bool isNewEventsBlock, bool isNewLumiBlock, bool isNewRun) = 0; // called by bookHistograms
  virtual StatusCode fill() = 0; // called by fillHistograms
  virtual StatusCode proc(bool isEndOfEventsBlock, bool isEndOfLumiBlock, bool isEndOfRun) = 0; // called by procHistograms

  // Available to aid the child
  int getL1info();
  int getRunNr();
  int getEventNr();
  int getLumiBlockNr();
  int hasBadLar();
  double getLumi(std::vector<std::string> m_triggers, int LBstart, int LBend,std::vector<double>& lumivec);

  // Set current MonGroup
  void setCurrentMonGroup(const std::string &monGroup);

  // Accessor members
  void addMonGroup(MonGroup *m);
  void addHistogram(TH1 *h, const std::string &monGroup = "");
  void addHistogram(TH2 *h, const std::string &monGroup = "");
  void addHistogram(TGraph *h, const std::string &monGroup = "");
  void addProfile(TProfile *h, const std::string &monGroup = "");
  void addProfile2d(TProfile2D *h, const std::string &monGroup = "");


  void addTree(TTree *t, const std::string &monGroup = "");

  TH1 *hist(const std::string &histName, const std::string &monGroup = "");
  TH2 *hist2(const std::string &histName, const std::string &monGroup = "");
  TGraph *graph(const std::string &histName, const std::string &monGroup = "");
  TProfile *profile(const std::string &histName, const std::string &monGroup = "");
  TProfile2D *profile2d(const std::string &histName, const std::string &monGroup = "");

  TTree *tree(const std::string &treeName, const std::string &monGroup = "");

  protected:

  MsgStream *m_log;

  StoreGateSvc * m_storeGate;

  // Histograms
  ITHistSvc* m_histsvc;

  // Infra-structure members
  std::map<std::string, MonGroup *> m_group; // maintain Monitoring groups
  std::map<std::string, TH1 *> m_hist; // maintain histograms
  std::map<std::string, TH2 *> m_hist2; // maintain histograms
  std::map<std::string, TProfile *> m_profile; // maintain histograms
  std::map<std::string, TProfile2D *> m_profile2d; // maintain histograms
  std::map<std::string, TGraph *> m_graph; // maintain histograms
  std::map<std::string, TTree *> m_tree; // maintain trees

  // Book-keeping the current mon group
  std::string m_currentMonGroup;

  //whether to check the LAr EventInfo of not
  bool m_checkLar;

  //for the luminosity calculation
 
std::string lumi_database;
std::string trig_database;
std::string m_trigger;
std::string m_livetrigger;
std::string m_lumioff;
std::string m_lumionl;
std::string m_mc_db;
std::string m_data_db;
std::string m_lumitag;
std::string m_lumimethod;
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
std::string m_lbcollname;
bool m_mc;
bool m_uselivetrigger;
bool m_verbose;

// Internal variables

//std::vector<float> m_lumivec;
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

//Root::TGoodRunsList* m_collsgrl;
/*
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

std::vector<TH1F*> m_ntrigplbVec;
std::vector<TH1F*> m_trigrateplbVec;
std::vector<TH1F*> m_lumiplbVec;
std::vector<TH1F*> m_lumitrigrateplbVec;
std::vector<TH1F*> m_intlumiVec;
std::vector<TH1F*> m_intlumitrigrateVec;
std::vector<TH1F*> m_lumitrigrateplb_recordedVec;
std::vector<TH1F*> m_intlumitrigrate_recordedVec;
*/


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



};

#endif

