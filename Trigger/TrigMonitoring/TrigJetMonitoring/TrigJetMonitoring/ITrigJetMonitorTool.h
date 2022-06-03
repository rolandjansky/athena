// -*- C++ -*-

/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGJETMONITORING_ITRIGJETMONITORTOOL_H
#define TRIGJETMONITORING_ITRIGJETMONITORTOOL_H `

#include "GaudiKernel/IAlgTool.h"
#include "GaudiKernel/EventContext.h"

#include <vector>
#include <string>


enum class MatchToEnum {hlt, offline};

class JetData;
class JetMatchData;

class ITrigJetMonitorTool : virtual public IAlgTool {
public:
  // static const InterfaceID&  interfaceID();
  DeclareInterfaceID(ITrigJetMonitorTool, 1, 0);
  virtual StatusCode getData(const EventContext& ctx,
			     std::vector<JetData>& jetData) const = 0;
  
  virtual StatusCode getMatchData(const EventContext& ctx,
				  MatchToEnum, 
				  std::vector<JetMatchData>& ) const = 0;

};
#endif
