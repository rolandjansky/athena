/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

/**
 * @file SCT_ReadoutTool.h
 * Header file for the SCT_ReadoutTool class
 * @author Carl Gwilliam <gwilliam@mail.cern.ch>
 */

#ifndef SCT_ConditionTools_SCT_ReadoutTool_h
#define SCT_ConditionTools_SCT_ReadoutTool_h

// Athena
#include "AthenaBaseComps/AthAlgTool.h"
#include "SCT_ConditionsTools/ISCT_ReadoutTool.h"

#include "SCT_Cabling/ISCT_CablingTool.h"
#include "SCT_ConditionsData/SCT_Chip.h"

// Gaudi
#include "GaudiKernel/ToolHandle.h"

// Forward declarations
class SCT_ID;

/**
 * @class SCT_ReadoutTool
 * Class to represent the SCT module readout
 **/

class SCT_ReadoutTool : public extends<AthAlgTool, ISCT_ReadoutTool> {

 public:

  /** Usual framework methods for an AlgTool*/
  SCT_ReadoutTool(const std::string& type, const std::string& name, const IInterface* parent);
  virtual ~SCT_ReadoutTool() = default;
  StatusCode initialize() override;
  StatusCode finalize() override;
  
  /** Determine which chips are in the readout for a module of a particular type by Identifier*/
  virtual StatusCode determineReadout(const Identifier& moduleId, std::vector<SCT_Chip>& chips, bool link0ok, bool link1ok) const override;
  /** Determine which chips are in the readout for a module of a particular type by truncated serial number*/
  virtual StatusCode determineReadout(const int truncatedSerialNumber, std::vector<SCT_Chip>& chips, bool link0ok, bool link1ok) const override;

 private:

  /** Private data*/
  const SCT_ID*                       m_sctId;               //!< ID helper for SCT
  IMessageSvc* m_msgSvc;
  ToolHandle<ISCT_CablingTool> m_cablingTool{this, "SCT_CablingTool", "SCT_CablingTool", "Tool to retrieve SCT Cabling"};

};

#endif // SCT_ConditionTools_SCT_ReadoutTool_h
