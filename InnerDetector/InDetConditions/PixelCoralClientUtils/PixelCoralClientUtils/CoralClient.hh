/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef CORALCLIENT
#define CORALCLIENT 

// GENERIC C++/C headers
#include <time.h>
#include <sys/time.h>
#include <iostream>
#include <fstream>
#include <string>
#include <set>
#include <cstdio>

#include "PixelCoralClientUtils/IPixResultsDbListener.hh"
#include "PixelCoralClientUtils/AnalysisResultList_t.hh"

// same definitions are done by seal and omni

#ifdef HAVE_NAMESPACES
#  undef HAVE_NAMESPACES
#endif
#ifdef HAVE_BOOL
#  undef HAVE_BOOL
#endif
#ifdef HAVE_DYNAMIC_CAST
#  undef HAVE_DYNAMIC_CAST
#endif

// CORAL API
#include "CoralKernel/Context.h"
#include "CoralBase/Attribute.h"
#include "CoralBase/AttributeList.h"
#include "CoralBase/AttributeListException.h"
#include "RelationalAccess/IConnectionService.h"
#include "RelationalAccess/IConnectionServiceConfiguration.h"
#include "RelationalAccess/AccessMode.h"
#include "RelationalAccess/ISessionProxy.h"
#include "RelationalAccess/ISchema.h"
#include "RelationalAccess/ITransaction.h"

#include "RelationalAccess/ITable.h"
#include "RelationalAccess/ITableDescription.h"
#include "RelationalAccess/IColumn.h"
#include "RelationalAccess/ICursor.h"
#include "RelationalAccess/IQuery.h"
#include "RelationalAccess/TableDescription.h"
#include "RelationalAccess/ITableDataEditor.h"
#include "RelationalAccess/ITableSchemaEditor.h"
#include "RelationalAccess/IBulkOperation.h"
#include "RelationalAccess/SchemaException.h"

// COOL API: database service bootstrap
//#include "CoolApplication/Application.h"
//#include "CoolApplication/DatabaseSvcFactory.h"
#include <CoralBase/TimeStamp.h>


//using namespace cool;

namespace CAN {
  // Added typedef to remove dependency on idl (PixCalibDbCoral/Common.idl)
#ifdef __x86_64__
  typedef unsigned int SerialNumber_t;
#else
  typedef unsigned long SerialNumber_t;
#endif
  class AverageResult_t;
}

// added -- A.X.
namespace PixelCalib {
  class PixelCalibData;
}

class PixCoralClient {

private:
  std::string m_connString;
  coral::AccessMode m_accessMode;

  coral::IHandle<coral::IConnectionService> m_connectionService;
  coral::ISessionProxy *m_session;

  std::string m_pixeltable;
  bool m_verbose;

  void connect();
  static coral::IHandle<coral::IConnectionService> connectionService(bool verbose=false);

protected:

  void transactionStartReadOnly();
  void transactionStartUpdate();
  void transactionCommit();

public:
  // added table name -- A.X.
  PixCoralClient(std::string id1, bool verbose = false, coral::AccessMode access_mode = coral::Update, const char* tableName = "CALIB_ANAL");
  PixCoralClient(bool verbose = false, coral::AccessMode access_mode = coral::Update, const char* tableName = "CALIB_ANAL");
  ~PixCoralClient();

  void disconnect();
  
  void printTables(const char* option = 0); // added option -- A.X.
  void printTableDesc(std::string tableName);
  void printTableContent(std::string tableName);
  void createTables(const char* option = 0); // added option -- A.X.
  template <typename T> void createTable();
  template <typename T> int fillTable(long long fk, CAN::AnalysisResultList_t *results);
  // added -- A.X.
  int fillTablePixelCalibData(long long fk, const char* option);
  void dropTables();
  void fillTables(CAN::SerialNumber_t id, CAN::AnalysisResultList_t *results);
  // fill pcd table -- A.X.
  void fillTables(const char* option);

  //PVSS methods
  double get_value_from_PVSSarch(std::string,const coral::TimeStamp &,const coral::TimeStamp &);
  double get_values_from_PVSSarch(std::string,const coral::TimeStamp &,const coral::TimeStamp &);
  void get_alias_from_PVSSarch();

  void queryTable(CAN::SerialNumber_t anal_id, std::string varname="", std::string connName="");
  CAN::AnalysisResultList_t getAnalysisResultsFromDB(CAN::SerialNumber_t anal_id, std::string varname="", std::string connName="");
  CAN::AnalysisResultList_t getAnalysisResultsFromDB(CAN::SerialNumber_t anal_id, const std::vector<std::string> &connName, std::string varname="");

  /** Get analysis results for a list of connectivity objects.
   * @param analysis_id list of analysis serial numbers,
   * @param connName list of connectivity names for which the values should be retrieved
   * @param listener object which will be notified about new variables.
   */
  void getAnalysisResultsFromDB(std::vector<CAN::SerialNumber_t> analysis_id,
				const std::vector<std::string> &connName,
				IPixResultsDbListener &listener);

  /** Get analysis results for a list of connectivity objects.
   * @param analysis_id the analysis serial number,
   * @param connName list of connectivity names for which the values should be retrieved
   * @param listener object which will be notified about new variables.
   */
  void getAnalysisResultsFromDB(CAN::SerialNumber_t analysis_id, const std::vector<std::string> &connName, IPixResultsDbListener &listener) {
    std::vector<CAN::SerialNumber_t> temp;
    temp.push_back(analysis_id);
    getAnalysisResultsFromDB(temp,connName,listener);
  }

  // get PCD -- A.X.
  void getCalibrationDataFromDB(const char* option);

  void createAuxTables();
  long long updateKey();
  void verboseOutput(bool verbose) {m_verbose = verbose;};

  /** Get the chip number from the column and row
  */
  static int chip(int col, int row) {
    int FE = -1;
    if ((int)row/160 == 0)  FE = (int)col/18;
    if ((int)row/160 == 1)  FE = 8 + (int)(143-col)/18;
    return FE;
  }

  static std::string PixelMapToCLOB(const PixelMap_t & pixmap);
  static PixelMap_t CLOBtoPixelMap(const std::string & clob);
  

  // Convert local time t to GMT (UTC)
  std::time_t LocalToGMTTime (std::time_t t) {
    std::time_t rawtime = t ? t : time(0);
    struct tm result;
    tm * ptm;
    ptm = gmtime_r(&rawtime, &result);
    return mktime(ptm);
  }
  // Get current time in GMT (UTC)
  std::time_t GMTCurrentTime() { return LocalToGMTTime(0); } // Thread unsafe LocalToGMTTime method is used.
  // Convert GMT (UTC) time to local time
  std::time_t GMTtoLocalTime (std::time_t gmttime) { // Thread unsafe gmtime is used.
    struct tm result;
    return 2*gmttime - mktime (gmtime_r(&gmttime, &result));
  }
};


#endif
