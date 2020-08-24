/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef COOLCORALCLIENT
#define COOLCORALCLIENT 

// GENERIC C++/C headers
#include <string>
#include <vector>


// COOL API include files (CoolKernel)
#include "CoolKernel/IDatabaseSvc.h"
#include "ConfDBif.h"

#include "CxxUtils/checker_macros.h"

namespace coral {
  class ISessionProxy;
}

//using namespace cool;

class ATLAS_NOT_THREAD_SAFE COOLCORALClient { // Thread unsafe coral::AttributeList is used.

protected:
  std::string m_connString;
  coral::ISessionProxy *m_session;

  /// Copy of the connection string.
  cool::DatabaseId m_dbId;
  
  /// Pointer to the opened database.
  cool::IDatabasePtr m_db;

  
public:
  COOLCORALClient(const std::string& id1, bool verbose);
  COOLCORALClient( const std::string& id1, const std::string& id2, cool::IDatabaseSvc &dbSvc, bool verbose);
  COOLCORALClient(const std::string& id1);
  COOLCORALClient( const std::string& id1, const std::string& id2, cool::IDatabaseSvc &dbSvc);
  ~COOLCORALClient();
  
  void printTables();
  void printTableContent(const std::string& tableName);
  void createTables(const std::string& part_trt);
  void createHistTables(bool);
  void dropTables(const std::string& part_trt);
  void fillTables(const std::string& part_trt);
  void createConnect(const std::string& part_trt);
  int GetTTCdummy(int ttc_id);
  TTCobj_t *GetTTC(int ttc_id);
  RODobj_t *GetROD(int rod_id);
  void dropFolders(const std::string&);
  void createFolders(const std::string&);
  void printFoldersContent(const std::string&);
  void fillFolderTables(const std::string&, const std::string&);
  TTCobj_t* GetTTCOOL(int);
  void createAuxTables();
  void UpdateKey();
  void fillTTC(const TTC_t&);
  void fillROD(const ROD_t&);
  void fillTTCGR(const TTCGroup_t&);
  void fillROC(const DTMROC_t&);
  void changeROC(const DTMROC_t&, const std::string&, double );
  void fillRODV(const std::vector<ROD_t>&);
  void fillROCV(const std::vector<DTMROC_t>&);
  void fillTTCGRV(const std::vector<TTCGroup_t>&);
  void fillTTCV(const std::vector<TTC_t>& ttcv);
  void fillBarrel(const Barrel_t&);
  void createMap();
  void fillMap();
  void fillMapV(const std::vector<Mapping_t>&);
  int GetIDS(int);
  //
  int GetOneTTC(int, const std::string& , TTC_t &);
  int GetOneTTC(int, long long, const std::string& , TTC_t &);
  int GetOneTTCGR(int, const std::string& , TTCGroup_t &);
  int GetOneTTCGR(int, long long, const std::string& , TTCGroup_t &);
  int GetOneROD(int, const std::string& , ROD_t &);
  int GetOneROD(int, long long, const std::string& , ROD_t &);
  int GetOneROC(int, const std::string& , DTMROC_t &);
  int GetOneROC(int, long long, const std::string& , DTMROC_t &);
  //
  int  GetFullIDS(int,const std::string&);
  void UpdateThr(std::vector<DTMROC*>*);
  void UpdateFine(std::vector<TTC_Line*>*);
  void UpdateGol(const GOL*,int);
  int  GetLastTTC();
  void GetLastValues(Detector_t &);
  void fillDetector(const Detector_t&);
  void fillHistDetector(const Detector_t&,const std::string&);
  //
  long long GetIoVKey(long long, long long, const std::string&);
  long long GetIoVROC(int,long long,const std::string&);
  long long GetIoVROC(int,long long,long long);
  void createIoVKeyTables();
  void GenIoVROCTable();
  void UpdateIoVROCTable(int,long long,const std::string&);
  int GetTags();
  
  Detector_t GetHistValues(const std::string&,long long);

  int fillLastDetector(const Detector_t&);
  void clearCurrentTables();
  void fillHistory(const Detector_t&,const std::string&);
  
  void CompHistValues(const std::string&,long long,const std::string&,long long, std::vector<Detector_t>&);
  int DiffHistValues(const std::vector<Detector_t>&);

  int GetRichTags();

  int DumpHistValues(const Detector_t&,const std::string&,long long);


  
  bool m_verbose;
};


#endif
