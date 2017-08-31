/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**
 * @file ISCT_ReadoutTool.h
 * Interface file for the SCT_ReadoutTool class
 * @author Carl Gwilliam <gwilliam@mail.cern.ch>
 */

#ifndef SCT_ConditionServices_ISCT_ReadoutTool_h
#define SCT_ConditionServices_ISCT_ReadoutTool_h

//STL
#include <vector>

// Gaudi
#include "GaudiKernel/IAlgTool.h"

// Forward declarations
class Identifier;
class SCT_Chip;

static const InterfaceID IID_ISCT_ReadoutTool{"InDet::ISCT_ReadoutTool", 1, 0};

/**
 * @class ISCT_ReadoutTool
 * Interface for SCT_ReadoutTool
 * Class to determine which chips are in the readout and spot misconfigurations
 */

class ISCT_ReadoutTool : virtual public IAlgTool {
 public:

  virtual ~ISCT_ReadoutTool() {};
  
  static const InterfaceID& interfaceID() { return IID_ISCT_ReadoutTool; };
  
  /** Determine which chips are in the readout for a module of a particular type by Identifier*/
  virtual StatusCode determineReadout(const Identifier& moduleId, std::vector<SCT_Chip*>& chips, bool link0ok, bool link1ok)=0;
  /** Determine which chips are in the readout for a module of a particular type by truncated serial number*/
  virtual StatusCode determineReadout(const int truncatedSerialNumber, std::vector<SCT_Chip*>& chips, bool link0ok, bool link1ok)=0;
};

#endif // SCT_ConditionServices_ISCT_ReadoutTool_h
