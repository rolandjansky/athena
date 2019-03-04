/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef SCT_MONITORCONDITIONSTOOL_SCT_MONITORCONDITIONSTOOL_H
#define SCT_MONITORCONDITIONSTOOL_SCT_MONITORCONDITIONSTOOL_H

// Athena includes
#include "AthenaBaseComps/AthAlgTool.h"
#include "SCT_ConditionsTools/ISCT_MonitorConditionsTool.h"
#include "SCT_ConditionsData/SCT_MonitorCondData.h"

// Read Handle Key
#include "StoreGate/ReadHandleKey.h"
#include "StoreGate/ReadCondHandleKey.h"

// Gaudi includes
#include "GaudiKernel/EventContext.h"

//STL
#include <set>

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
  virtual bool canReportAbout(InDetConditions::Hierarchy h) const override;

  ///Is the detector element good?
  virtual bool isGood(const Identifier& elementId, InDetConditions::Hierarchy h=InDetConditions::DEFAULT) const override;
  virtual bool isGood(const Identifier& elementId, const EventContext& ctx, InDetConditions::Hierarchy h=InDetConditions::DEFAULT) const override;

  ///is it good?, using wafer hash
  virtual bool isGood(const IdentifierHash& hashId) const override;
  virtual bool isGood(const IdentifierHash& hashId, const EventContext& ctx) const override;

  /// List of bad strip Identifiers
  virtual void badStrips(std::set<Identifier>& strips) const override;
  virtual void badStrips(std::set<Identifier>& strips, const EventContext& ctx) const override;
  
  /// List of bad strip Identifiers for a given module
  virtual void badStrips(const Identifier& moduleId, std::set<Identifier>& strips) const override;
  virtual void badStrips(const Identifier& moduleId, std::set<Identifier>& strips, const EventContext& ctx) const override;

  /// String of bad strip numbers for a given module
  virtual std::string badStripsAsString(const Identifier& moduleId) const override;
  virtual std::string badStripsAsString(const Identifier& moduleId, const EventContext& ctx) const override;

private:
  // ------------------------------------------------------------------------------------
  // local stuff 
  // ------------------------------------------------------------------------------------
  std::string getList(const Identifier& imodule, const EventContext& ctx) const;

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

  void expandRange(const std::string& rangeStr, std::set<int>& rangeList) const;
  
  void expandList(const std::string& defectStr, std::set<int>& defectList) const;

  // small helper function
  unsigned int computeIstrip4moncond(const Identifier& elementId) const;

  static const std::string s_separator;

  IntegerProperty              m_nhits_noisychip{this, "Nnoisychip", 64};
  IntegerProperty              m_nhits_noisywafer{this, "Nnoisywafer", 384};
  IntegerProperty              m_nhits_noisymodule{this, "Nnoisycmodule", 768};
  const SCT_ID*                m_pHelper;

  SG::ReadCondHandleKey<SCT_MonitorCondData> m_condKey{this, "CondKey", "SCT_MonitorCondData", "SCT noisy strips"};
  const SCT_MonitorCondData* getCondData(const EventContext& ctx) const;
};

#endif // SCT_MONITORCONDITIONSTOOL_SCT_MONITORCONDITIONSTOOL_H
