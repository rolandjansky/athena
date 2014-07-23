/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/***********************************************************************
ISUSYPlotTool: Interface for SUSYPlotTool tools. 

Modified from SusyPlot.
***********************************************************************/
#ifndef SUSYPLOT_ISUSYPLOTTOOL_H
#define SUSYPLOT_ISUSYPLOTTOOL_H

// Include files
#include "TrigDecisionTool/TrigDecisionTool.h"

#include "GaudiKernel/IAlgTool.h"


// Declaration of interface ID (interface id, major version, minor version)

static const InterfaceID IID_ISUSYPlotTool("ISUSYPlotTool", 1 , 0);

class ISUSYPlotTool : virtual public IAlgTool {

 public:
  /// Retrieve interface ID
  static const InterfaceID& interfaceID() { return IID_ISUSYPlotTool; }
 virtual StatusCode doIt(bool,bool,ToolHandle<Trig::TrigDecisionTool>)=0;
  virtual StatusCode takeAction()=0;

};

#endif // SUSYPLOT_ISUSYPLOTTOOL_H
