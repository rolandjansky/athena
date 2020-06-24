/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

/**
 * @file SCT_ConditionsSummaryTool.h
 * @author shaun.roe@cern.ch
**/
#ifndef SCT_ConditionsSummaryTool_h
#define SCT_ConditionsSummaryTool_h

#include "AthenaBaseComps/AthAlgTool.h"

#include "InDetConditionsSummaryService/InDetHierarchy.h"
#include "InDetConditionsSummaryService/IInDetConditionsTool.h"

//Gaudi Includes
#include "GaudiKernel/ToolHandle.h"

//STL includes
#include <string>
#include <vector>

//forward declarations
class ISCT_ConditionsTool;

/**
 * @class SCT_ConditionsSummaryTool
 * Interface class for tool providing summary of status of an SCT detector element
**/
class SCT_ConditionsSummaryTool: public extends<AthAlgTool, IInDetConditionsTool> {

public:
  SCT_ConditionsSummaryTool(const std::string& type, const std::string& name, const IInterface* parent); //!< Tool constructor
  virtual ~SCT_ConditionsSummaryTool() = default;
  //@name Gaudi STool Implementation
  //@{
  virtual StatusCode initialize() override;          //!< Tool init
  virtual StatusCode finalize() override;            //!< Tool finalize
  //@}
  
  //@name reimplemented from IInDetConditionsTool
  //@{
  virtual bool isActive(const Identifier& elementId, const InDetConditions::Hierarchy h=InDetConditions::DEFAULT) const override;
  virtual bool isActive(const IdentifierHash& elementHash) const override;
  virtual bool isActive(const IdentifierHash& elementHash, const Identifier& elementId) const override;
  virtual double activeFraction(const IdentifierHash& elementHash, const Identifier& idStart, const Identifier& idEnd) const override;

  virtual bool isActive(const Identifier& elementId, const InDetConditions::Hierarchy h, const EventContext& ctx) const override;
  virtual bool isActive(const IdentifierHash& elementHash, const EventContext& ctx) const override;
  virtual bool isActive(const IdentifierHash& elementHash, const Identifier& elementId, const EventContext& ctx) const override;
  virtual double activeFraction(const IdentifierHash& elementHash, const Identifier& idStart, const Identifier& idEnd, const EventContext& ctx) const override;

  virtual bool isGood(const Identifier& elementId, const InDetConditions::Hierarchy h=InDetConditions::DEFAULT) const override;
  virtual bool isGood(const IdentifierHash& elementHash) const override;
  virtual bool isGood(const IdentifierHash& elementHash, const Identifier& elementId) const override;
  virtual double goodFraction(const IdentifierHash& elementHash, const Identifier& idStart, const Identifier& idEnd) const override;

  virtual bool isGood(const Identifier& elementId, const InDetConditions::Hierarchy h, const EventContext& ctx) const override;
  virtual bool isGood(const IdentifierHash& elementHash, const EventContext& ctx) const override;
  virtual bool isGood(const IdentifierHash& elementHash, const Identifier& elementId, const EventContext& ctx) const override;
  virtual double goodFraction(const IdentifierHash& elementHash, const Identifier& idStart, const Identifier& idEnd, const EventContext& ctx) const override;

  virtual bool hasBSError(const IdentifierHash& elementHash) const override;
  virtual bool hasBSError(const IdentifierHash& elementHash, Identifier elementId) const override;
  virtual bool hasBSError(const IdentifierHash& elementHash, const EventContext& ctx) const override;
  virtual bool hasBSError(const IdentifierHash& elementHash, Identifier elementId, const EventContext& ctx) const override;
  virtual uint64_t getBSErrorWord(const IdentifierHash& moduleHash, const EventContext& ctx) const  override;
  //@}
private:
  StringArrayProperty m_reportingTools; //!< list of tools to be used
  ToolHandleArray<ISCT_ConditionsTool> m_toolHandles;
  bool m_noReports{true};
};

#endif // SCT_ConditionsSummaryTool_h
