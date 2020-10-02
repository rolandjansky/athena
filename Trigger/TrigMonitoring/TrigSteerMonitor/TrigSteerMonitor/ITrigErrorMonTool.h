/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/
#ifndef TRIGSTEERMONITOR_ITRIGERRORMONTOOL_H
#define TRIGSTEERMONITOR_ITRIGERRORMONTOOL_H

#include "GaudiKernel/IAlgTool.h"
#include "GaudiKernel/EventContext.h"

/**
 * @class ITrigErrorMonTool
 * @brief Interface of a tool which retrieves and monitors all non-success status codes returned by algorithms
 **/
class ITrigErrorMonTool : virtual public IAlgTool {
public: 
  DeclareInterfaceID(ITrigErrorMonTool, 1, 0);

  /// Produce a subset of IAlgExecStateSvc::algExecStates with only non-success StatusCodes and fill relevant histograms
  virtual std::unordered_map<std::string_view, StatusCode> algExecErrors(const EventContext& eventContext) const = 0;
}; 

#endif // TRIGSTEERMONITOR_ITRIGERRORMONTOOL_H
