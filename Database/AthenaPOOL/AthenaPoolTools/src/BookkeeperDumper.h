/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// BookkeeperDumper.h

#ifndef ATHENAPOOLTOOLS_BOOKKEEPERDUMPER_H
#define ATHENAPOOLTOOLS_BOOKKEEPERDUMPER_H
/**
 * @file BookkeeperDumper.h
 * @brief class definition for BookkeeperDumper
 */
 
/**
 * @class BookkeeperDumper
 * @author Jack Cranshaw (Jack.Cranshaw@cern.ch)
 * Created October 2017
 */
             
                                                                                
#include <string>
#include <vector>
#include "AthenaBaseComps/AthAlgorithm.h"
#include "GaudiKernel/ServiceHandle.h"
#include "GaudiKernel/IIncidentListener.h"

class IClassIDSvc;

class BookkeeperDumper : public AthAlgorithm, virtual public IIncidentListener {
public:
	  
  BookkeeperDumper(const std::string& name, ISvcLocator* pSvcLocator);
  virtual ~BookkeeperDumper();
  
  StatusCode initialize(); /// Algorithm interface. Cannot re-initialize with this
  StatusCode execute();    /// Algorithm interface.
  StatusCode finalize();   /// Algorithm interface.

  void handle(const Incident& inc);
  
private:
};
#endif
