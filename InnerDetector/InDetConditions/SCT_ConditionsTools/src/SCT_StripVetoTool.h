/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

/**
 * @file SCT_StripVetoTool.h
 * header file for tool allowing one to declare strips as bad
 * @author shaun.roe@cern.ch
**/

#ifndef SCT_StripVetoTool_h
#define SCT_StripVetoTool_h
//STL includes
#include <set>

//Gaudi includes
#include "AthenaBaseComps/AthAlgTool.h"

//Athena includes
#include "InDetConditionsSummaryService/InDetHierarchy.h"
#include "SCT_ConditionsTools/ISCT_ConditionsTool.h"

//forward declarations
class IdentifierHash;
class SCT_ID;

/**
 * @class SCT_StripVetoTool
 * Service allowing one to manually declare detector strips as 'bad' in the joboptions file
**/
class SCT_StripVetoTool: public extends<AthAlgTool, ISCT_ConditionsTool> {
public:
  //@name Tool methods
  //@{
  SCT_StripVetoTool(const std::string& type, const std::string& name, const IInterface* parent);
  virtual ~SCT_StripVetoTool() = default;
  virtual StatusCode initialize() override;
  virtual StatusCode finalize() override;
  //@}
  
  ///Can the service report about the given component? (chip, module...)
  virtual bool canReportAbout(InDetConditions::Hierarchy h) override;
  
  ///Is the detector element good?
  virtual bool isGood(const Identifier& elementId, InDetConditions::Hierarchy h=InDetConditions::SCT_STRIP) override;
  
  ///is it good?, using wafer hash
  virtual bool isGood(const IdentifierHash& hashId) override;

private:
  StatusCode fillData();

  StringArrayProperty m_badElements; //list of bad detector elements (= strips)
  std::set<Identifier> m_badIds;
  const SCT_ID* m_pHelper;
  
};

#endif // SCT_StripVetoTool_h
