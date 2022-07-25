/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
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
#include "InDetConditionsSummaryService/IDetectorElementStatusTool.h"
#include "InDetReadoutGeometry/SiDetectorElementCollection.h"
#include "InDetReadoutGeometry/SiDetectorElementStatus.h"
#include "StoreGate/ReadCondHandleKey.h"

//Gaudi Includes
#include "GaudiKernel/ToolHandle.h"

//STL includes
#include <string>
#include <vector>

//forward declarations
class ISCT_ConditionsTool;

class SCT_ID;


/**
 * @class SCT_ConditionsSummaryTool
 * Interface class for tool providing summary of status of an SCT detector element
**/
class SCT_ConditionsSummaryTool: public extends<AthAlgTool, IDetectorElementStatusTool, IInDetConditionsTool> {

public:
  SCT_ConditionsSummaryTool(const std::string& type, const std::string& name, const IInterface* parent); //!< Tool constructor
  virtual ~SCT_ConditionsSummaryTool() = default;
  //@name Gaudi STool Implementation
  //@{
  virtual StatusCode initialize() override;          //!< Tool init
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
  virtual std::unique_ptr<InDet::SiDetectorElementStatus>
  getDetectorElementStatus(const EventContext& ctx,
                           SG::WriteCondHandle<InDet::SiDetectorElementStatus>* whandle) const override;

  virtual bool hasBSError(const IdentifierHash& elementHash) const override;
  virtual bool hasBSError(const IdentifierHash& elementHash, Identifier elementId) const override;
  virtual bool hasBSError(const IdentifierHash& elementHash, const EventContext& ctx) const override;
  virtual bool hasBSError(const IdentifierHash& elementHash, Identifier elementId, const EventContext& ctx) const override;
  virtual bool hasBSError(const IdentifierHash& elementHash, const EventContext& ctx, Identifier elementId) const override;
  virtual uint64_t getBSErrorWord(const IdentifierHash& moduleHash, const EventContext& ctx) const  override;
  virtual uint64_t getBSErrorWord(const IdentifierHash& moduleHash, const int index, const EventContext& ctx) const  override;
  //@}
private:
  std::unique_ptr<InDet::SiDetectorElementStatus>
  createDetectorElementStatus(const EventContext& ctx,
                              SG::WriteCondHandle<InDet::SiDetectorElementStatus>* whandle) const;

  ToolHandleArray<ISCT_ConditionsTool> m_toolHandles
     {this, "ConditionsTools", {},""};
  SG::ReadCondHandleKey<InDetDD::SiDetectorElementCollection> m_SCTDetEleCollKey
     {this, "SCTDetEleCollKey", "SCT_DetectorElementCollection", "Key of SiDetectorElementCollection for SCT"};
  SG::ReadHandleKey<InDet::SiDetectorElementStatus> m_SCTDetElStatusEventKey
     {this, "SCTDetElStatusEventDataBaseKey", "", "Optional event data key of an input SiDetectorElementStatus on which the newly created object will be based."};
  SG::ReadCondHandleKey<InDet::SiDetectorElementStatus> m_SCTDetElStatusCondKey
     {this, "SCTDetElStatusCondDataBaseKey", "" , "Optional conditions data key of an input SiDetectorElementStatus on which the newly created object will be based."};
  const SCT_ID* m_id_sct{nullptr}; //!< Handle to SCT ID helper


  bool m_noReports{true};
};

#endif // SCT_ConditionsSummaryTool_h
