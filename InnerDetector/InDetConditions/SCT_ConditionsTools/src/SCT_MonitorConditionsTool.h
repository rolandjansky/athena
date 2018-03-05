/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

#ifndef SCT_MONITORCONDITIONSTOOL_SCT_MONITORCONDITIONSTOOL_H
#define SCT_MONITORCONDITIONSTOOL_SCT_MONITORCONDITIONSTOOL_H

//STL
#include <map>
#include <set>
#include <list>
#include <mutex>

// Gaudi includes
#include "GaudiKernel/EventContext.h"
#include "GaudiKernel/ContextSpecificPtr.h"

// Athena includes
#include "AthenaBaseComps/AthAlgTool.h"
#include "SCT_ConditionsTools/ISCT_MonitorConditionsTool.h"
#include "SCT_ConditionsData/SCT_MonitorConditionsCondData.h"

// Read Handle Key
#include "StoreGate/ReadHandleKey.h"
#include "StoreGate/ReadCondHandleKey.h"

//forward declarations
class SCT_ID;

/**
 ** Class for keeping track of errors caught by the monitoring
 **/

class SCT_MonitorConditionsTool: public extends<AthAlgTool, ISCT_MonitorConditionsTool> {

public:
  // Constructor
  SCT_MonitorConditionsTool(const std::string& type, const std::string& name, const IInterface* parent);
  // Destructor
  virtual ~SCT_MonitorConditionsTool() = default;

  // overloading functions
  virtual StatusCode initialize() override;
  virtual StatusCode finalize() override;

  ///Can the service report about the given component? (chip, module...)
  virtual bool canReportAbout(InDetConditions::Hierarchy h) override;

  ///Is the detector element good?
  virtual bool isGood(const Identifier& elementId, InDetConditions::Hierarchy h=InDetConditions::DEFAULT) override;

  ///is it good?, using wafer hash
  virtual bool isGood(const IdentifierHash& hashId) override;

  /// List of bad strip Identifiers
  virtual void badStrips(std::set<Identifier>& strips) override;
  
  /// List of bad strip Identifiers for a given module
  virtual void badStrips(const Identifier& moduleId, std::set<Identifier>& strips) override;

  /// String of bad strip numbers for a given module
  virtual std::string badStripsAsString(const Identifier& moduleId) override;

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

  IntegerProperty              m_nhits_noisychip;
  IntegerProperty              m_nhits_noisywafer;
  IntegerProperty              m_nhits_noisymodule;
  const SCT_ID*                m_pHelper;

  // Mutex to protect the contents.
  mutable std::mutex m_mutex;
  // Cache to store events for slots
  mutable std::vector<EventContext::ContextEvt_t> m_cache;
  // Pointer of SCT_MonitorConditionsCondData
  mutable Gaudi::Hive::ContextSpecificPtr<const SCT_MonitorConditionsCondData> m_condData;

  SG::ReadCondHandleKey<SCT_MonitorConditionsCondData> m_condKey;
  const SCT_MonitorConditionsCondData* getCondData(const EventContext& ctx) const;
};

#endif // SCT_MONITORCONDITIONSTOOL_SCT_MONITORCONDITIONSTOOL_H
