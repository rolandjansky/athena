// -*- C++ -*-

/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

/**
 * @file SCT_RODVetoTool.h
 * header file for tool allowing one to declare modules as bad
 * @author daiki.hayakawa@cern.ch
**/

#ifndef SCT_RODVetoTool_h
#define SCT_RODVetoTool_h

//Athena includes
#include "AthenaBaseComps/AthAlgTool.h"
#include "SCT_ConditionsTools/ISCT_ConditionsTool.h"

#include "Identifier/Identifier.h"
#include "InDetConditionsSummaryService/InDetHierarchy.h"
#include "SCT_ConditionsData/IdentifierSet.h"
#include "StoreGate/ReadHandleKey.h"

//STL includes
#include <string>

//forward declarations
class IdentifierHash;
class SCT_ID;
class SCT_RODVetoTool_test;

/**
 * @class SCT_RODVetoTool
 * Service allowing one to manually declare detector elements as 'bad' in the joboptions file
**/
class SCT_RODVetoTool: public extends<AthAlgTool, ISCT_ConditionsTool> {
  /** Allow the test class access to all methods */
  friend class SCT_RODVetoTool_test;

public:
  //@name Tool methods
  //@{
  SCT_RODVetoTool(const std::string& type, const std::string& name, const IInterface* parent);
  virtual ~SCT_RODVetoTool() = default;
  virtual StatusCode initialize() override;
  virtual StatusCode finalize() override;
  //@}
  
  ///Can the service report about the given component? (chip, module...)
  virtual bool canReportAbout(InDetConditions::Hierarchy h) const override;
  
  ///Is the detector element good?
  virtual bool isGood(const Identifier& elementId, InDetConditions::Hierarchy h=InDetConditions::DEFAULT) const override;
  virtual bool isGood(const Identifier& elementId, const EventContext& ctx, InDetConditions::Hierarchy h=InDetConditions::DEFAULT) const override;
  
  ///is it good?, using wafer hash
  virtual bool isGood(const IdentifierHash& hashId) const override;
  virtual bool isGood(const IdentifierHash& hashId, const EventContext& ctx) const override;

private:

  const IdentifierSet* getCondData(const EventContext& ctx) const;

  // The vector of bad rods should be kept in a threadsafe way so it can 
  // be called and read safely.
  SG::ReadCondHandleKey<IdentifierSet> m_badModuleIds{this, "BadModuleIds", "BadSCTModuleIds_RODVeto", "Read key for bad module identifiers"};

  const SCT_ID* m_pHelper{nullptr};
};

#endif // SCT_RODVetoTool_h
