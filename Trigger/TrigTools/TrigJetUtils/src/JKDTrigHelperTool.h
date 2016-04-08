///////////////////////// -*- C++ -*- /////////////////////////////

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// JKDTrigHelperTool.h 
// Header file for class JKDTrigHelperTool
/////////////////////////////////////////////////////////////////// 
#ifndef TRIGJETUTILS_JKDTRIGHELPERTOOL_H
#define TRIGJETUTILS_JKDTRIGHELPERTOOL_H 1

// STL includes
#include <string>

// FrameWork includes
#include "AthenaBaseComps/AthAlgTool.h"
#include "GaudiKernel/ServiceHandle.h"
#include "GaudiKernel/IIncidentListener.h"

// TrigJetUtils includes
#include "TrigJetUtils/IJKDTrigHelperTool.h"

// Forward declaration

class JKDTrigHelperTool
  : virtual public ::IJKDTrigHelperTool, virtual public IIncidentListener,
            public ::AthAlgTool
{ 

 public: 

  /// Constructor with parameters: 
  JKDTrigHelperTool( const std::string& type,
	     const std::string& name, 
	     const IInterface* parent );

  /// Destructor: 
  virtual ~JKDTrigHelperTool(); 

  // Athena algtool's Hooks
  virtual StatusCode  initialize();
  virtual StatusCode  finalize();
  /////////////////////////////////////////////////////////////////// 
   
  StatusCode setupJetKeyDescriptor(StoreGateSvc*, HLT::Navigation*);

  virtual void handle(const Incident&);

 private: 


  /// Default constructor: 
  JKDTrigHelperTool();

  bool m_is_setup;
  

}; 


#endif //> !TRIGJETUTILS_JKDTRIGHELPERTOOL_H
