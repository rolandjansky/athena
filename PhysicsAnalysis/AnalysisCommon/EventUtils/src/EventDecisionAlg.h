
/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////// -*- C++ -*- /////////////////////////////
// EventDecisionAlg.h
// Header file for class EventDecisionAlg
// Author: Karsten Koeneke <karsten.koeneke@cern.ch>
///////////////////////////////////////////////////////////////////
#ifndef EVENTUTILS_EVENTDECISIONALG_H
#define EVENTUTILS_EVENTDECISIONALG_H 1

// STL includes
#include <string>

// FrameWork includes
#include "GaudiKernel/ServiceHandle.h"
#include "AthenaBaseComps/AthFilterAlgorithm.h"

// forward declarations
class IDecisionSvc;


class EventDecisionAlg
  : public ::AthFilterAlgorithm
{

  ///////////////////////////////////////////////////////////////////
  // Public methods:
  ///////////////////////////////////////////////////////////////////
 public:

  // Copy constructor:

  /// Constructor with parameters:
  EventDecisionAlg( const std::string& name, ISvcLocator* pSvcLocator );

  /// Destructor:
  virtual ~EventDecisionAlg();

  /// Athena algorithm's initalize hook
  virtual StatusCode  initialize();

  /// Athena algorithm's execute hook
  virtual StatusCode  execute();

  /// Athena algorithm's finalize hook
  virtual StatusCode  finalize();



  ///////////////////////////////////////////////////////////////////
  // Private data:
  ///////////////////////////////////////////////////////////////////
 private:
  // The handle to the IDecisionSvc;
  ServiceHandle<IDecisionSvc> m_decSvc;

  /// The names of all output streams to check
  StringArrayProperty m_streamNames;

};


#endif //> !EVENTUTILS_EVENTDECISIONALG_H
