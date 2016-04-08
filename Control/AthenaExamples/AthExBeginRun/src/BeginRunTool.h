// -*- C++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef ATHEXBEGINRUN_BEGINRUNTOOL_H
#define ATHEXBEGINRUN_BEGINRUNTOOL_H 1

#include "GaudiKernel/IIncidentListener.h"
#include "AthenaBaseComps/AthAlgTool.h"

#include <string>


/**@class BeginRunTool
 * @brief Simple AlgTool to demonstrate incident handling
 *
 * This AlgTool will register itself to the "BeginRun" incident and
 * print a message whenever a new run starts.
 *
 * @author Frank Winklmeier
 */
class BeginRunTool : public AthAlgTool,
                     virtual public IIncidentListener {
public:
  BeginRunTool( const std::string&, const std::string&, const IInterface* );  
  virtual StatusCode initialize();
  
  /// Implementation of IIncidentListener::handle
  virtual void handle(const Incident&);
};

#endif
