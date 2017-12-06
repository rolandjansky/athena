/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef SCT_MONITORCONDITIONSSVC_SCT_MONITORCONDITIONSSVC_H
#define SCT_MONITORCONDITIONSSVC_SCT_MONITORCONDITIONSSVC_H

//STL
#include <string>
#include <map>
#include <set>
#include <list>
#include <mutex>

// Gaudi includes
#include "GaudiKernel/ServiceHandle.h"
#include "GaudiKernel/StatusCode.h"
#include "GaudiKernel/EventContext.h"
#include "GaudiKernel/ContextSpecificPtr.h"

// Athena includes
#include "AthenaBaseComps/AthService.h"
#include "Identifier/Identifier.h"
#include "StoreGate/StoreGateSvc.h"

// local includes
#include "InDetConditionsSummaryService/InDetHierarchy.h"
#include "SCT_ConditionsServices/ISCT_ConditionsSvc.h"
#include "SCT_ConditionsServices/ISCT_MonitorConditionsSvc.h"

#include "SCT_ConditionsData/SCT_MonitorConditionsCondData.h"

// Read Handle Key
#include "StoreGate/ReadHandleKey.h"
#include "StoreGate/ReadCondHandleKey.h"

//forward declarations
class ISvcLocator;
class IdentifierHash;
class SCT_ID;

/**
 ** Class for keeping track of errors caught by the monitoring
 **/

class SCT_MonitorConditionsSvc: virtual public ISCT_MonitorConditionsSvc, virtual public AthService {

public:
  // Constructor
  SCT_MonitorConditionsSvc(const std::string& name, ISvcLocator* svc);
  // Destructor
  virtual ~SCT_MonitorConditionsSvc();

  // overloading functions
  virtual StatusCode initialize();
  virtual StatusCode finalize();
  virtual StatusCode queryInterface(const InterfaceID& riid, void** ppvInterface);

  ///Can the service report about the given component? (chip, module...)
  virtual bool canReportAbout(InDetConditions::Hierarchy h);

  ///Is the detector element good?
  virtual bool isGood(const Identifier& elementId, InDetConditions::Hierarchy h=InDetConditions::DEFAULT);

  ///is it good?, using wafer hash
  virtual bool isGood(const IdentifierHash& hashId);

  ///Manually get the data in the structure before proceding
  virtual StatusCode fillData();

  ///Are the data available?
  virtual bool filled() const;

  //I'm going to fill this from job options, so the callback version of fillData is not needed.
  virtual StatusCode fillData(int& /*i*/, std::list<std::string>& /*l*/) { //comment out unused parameters to prevent compiler warning
    return StatusCode::FAILURE;
  }

  ///Can the data be filled during the initialize phase?
  virtual bool canFillDuringInitialize() { return true; }

  /// List of bad strip Identifiers
  virtual void badStrips(std::set<Identifier>& strips);
  
  /// List of bad strip Identifiers for a given module
  virtual void badStrips(const Identifier& moduleId, std::set<Identifier>& strips);

  /// String of bad strip numbers for a given module
  virtual std::string badStripsAsString(const Identifier& moduleId);

private:
  // ------------------------------------------------------------------------------------
  // local stuff 
  // ------------------------------------------------------------------------------------
  std::string getList(const Identifier& imodule) const;

  bool stripIsNoisy(const int strip, const std::string& defectList) const;

  bool chipIsNoisy(int strip, const std::string& defectList) const;

  bool waferIsNoisy(const int strip, const std::string& defectList) const;

  bool moduleIsNoisy(const std::string& defectList) const ;

  bool inRange(const int theNumber, const std::string& stringRange) const;

  bool inList(const int theNumber, const std::string& theList) const;

  int  nBlock(const int theNumber, const std::string& stringRange) const;

  int  nDefect(const int theNumber, const std::string& theList) const;

  int  nBlock(const std::string& stringRange) const;

  int  nDefect(const std::string& theList) const;

  bool inRange(const int x, const int min, const int max) const;

  void expandRange(const std::string& rangeStr, std::set<int>& rangeList);
  
  void expandList(const std::string& defectStr, std::set<int>& defectList);

  // small helper function
  unsigned int computeIstrip4moncond(const Identifier& elementId) const;

  static std::string s_separator;
  static std::string s_defectFolderName;

  mutable std::map<const int, const std::string>  m_defectListMap;
  ServiceHandle<StoreGateSvc>  m_detStore;
  IntegerProperty              m_nhits_noisychip;
  IntegerProperty              m_nhits_noisywafer;
  IntegerProperty              m_nhits_noisymodule;
  bool                         m_filled;
  const SCT_ID*                m_pHelper;
  mutable std::string          m_currentDefectList;

  // Mutex to protect the contents.
  mutable std::mutex m_mutex;
  // Cache to store events for slots
  mutable std::vector<EventContext::ContextEvt_t> m_cache;
  // Pointer of SCT_MonitorConditionsCondData
  mutable Gaudi::Hive::ContextSpecificPtr<const SCT_MonitorConditionsCondData> m_condData;

  SG::ReadCondHandleKey<SCT_MonitorConditionsCondData> m_condKey;
  const SCT_MonitorConditionsCondData* getCondData(const EventContext& ctx) const;
};

#endif // SCT_MonitorConditinosSvc.h
