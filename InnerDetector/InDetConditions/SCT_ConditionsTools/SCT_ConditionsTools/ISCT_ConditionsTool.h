/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

/**
 * @file ISCT_ConditionsTool.h
 * header file for baseclass of SCT conditions tools
 * @author Susumu.Oda@cern.ch
 **/

#ifndef ISCT_ConditionsTool_h
#define ISCT_ConditionsTool_h

//Athena includes
#include "Identifier/Identifier.h"
#include "Identifier/IdentifierHash.h"
#include "InDetConditionsSummaryService/InDetHierarchy.h"
#include "StoreGate/WriteCondHandle.h"

//Gaudi Includes
#include "GaudiKernel/IAlgTool.h"
#include "GaudiKernel/EventContext.h"
#include "GaudiKernel/EventIDRange.h"

namespace InDet {
   class SiDetectorElementStatus;
}

/**
 * @class ISCT_ConditionsTool
 * Base class for SCT conditions tools so they can be used in the summary tool
 **/

class ISCT_ConditionsTool: virtual public IAlgTool {
 public:
  virtual ~ISCT_ConditionsTool() = default;

  /// Creates the InterfaceID and interfaceID() method
  DeclareInterfaceID(ISCT_ConditionsTool, 1, 0);
  
  ///Return whether this service can report on the hierarchy level (e.g. module, chip...)
  virtual bool canReportAbout(InDetConditions::Hierarchy h) const =0;
  
  ///Summarise the result from the service as good/bad
  virtual bool isGood(const Identifier& elementId, InDetConditions::Hierarchy h=InDetConditions::DEFAULT) const =0;
  virtual bool isGood(const Identifier& elementId, const EventContext& ctx, InDetConditions::Hierarchy h=InDetConditions::DEFAULT) const =0;
  virtual void getDetectorElementStatus(const EventContext& ctx, InDet::SiDetectorElementStatus &element_status, 
                                        SG::WriteCondHandle<InDet::SiDetectorElementStatus>* whandle) const = 0;

  //@todo introduce hash identifier method
  virtual bool isGood(const IdentifierHash& hashId) const =0;
  virtual bool isGood(const IdentifierHash& hashId, const EventContext& ctx) const =0;
};

#endif // ISCT_ConditionsTool_h
