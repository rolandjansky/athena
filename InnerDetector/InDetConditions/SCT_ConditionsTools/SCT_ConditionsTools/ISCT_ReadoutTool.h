/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

/**
 * @file ISCT_ReadoutTool.h
 * Interface file for the SCT_ReadoutTool class
 * @author Carl Gwilliam <gwilliam@mail.cern.ch>
 */

#ifndef SCT_ConditionTools_ISCT_ReadoutTool_h
#define SCT_ConditionTools_ISCT_ReadoutTool_h

//STL
#include <vector>

// Gaudi
#include "GaudiKernel/IAlgTool.h"

// Athena
#include "Identifier/Identifier.h"

// Forward declarations
class SCT_Chip;

/**
 * @class ISCT_ReadoutTool
 * Interface for SCT_ReadoutTool
 * Class to determine which chips are in the readout and spot misconfigurations
 */

class ISCT_ReadoutTool : virtual public IAlgTool {
 public:

  virtual ~ISCT_ReadoutTool() = default;
  
  /// Creates the InterfaceID and interfaceID() method
  DeclareInterfaceID(ISCT_ReadoutTool, 1, 0);
  
  /** Determine which chips are in the readout for a module of a particular type by Identifier*/
  virtual StatusCode determineReadout(const Identifier& moduleId, std::vector<SCT_Chip>& chips, bool link0ok, bool link1ok) const =0;
  /** Determine which chips are in the readout for a module of a particular type by truncated serial number*/
  virtual StatusCode determineReadout(const int truncatedSerialNumber, std::vector<SCT_Chip>& chips, bool link0ok, bool link1ok) const =0;
};

#endif // SCT_ConditionTools_ISCT_ReadoutTool_h
