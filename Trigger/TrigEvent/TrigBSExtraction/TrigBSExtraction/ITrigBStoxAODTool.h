/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGBSEXTRACTION_ITRIGBSTOXAODTOOL_H
#define TRIGBSEXTRACTION_ITRIGBSTOXAODTOOL_H
#include "GaudiKernel/IAlgTool.h"
#include "GaudiKernel/ToolHandle.h"
#include "TrigNavigation/Navigation.h"

/**
 * @brief Interface of Tool used by TrigBSExtraction to convert to xAOD
 */

static const InterfaceID IID_ITrigBStoxAODTool("ITrigBStoxAODTool", 1 , 0);

class ITrigBStoxAODTool : public virtual IAlgTool {
public:

  virtual StatusCode convert() = 0; 
  virtual StatusCode rewireNavigation(HLT::Navigation*) = 0;
  static const InterfaceID& interfaceID() { return IID_ITrigBStoxAODTool; }
};

#endif // TRIGBSEXTRACTION_ITRIGBSTOXAODTOOL_H
