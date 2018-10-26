///////////////////////// -*- C++ -*- /////////////////////////////

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// TrackVertexAssoTestAlg.h 
// Header file for class TrackVertexAssoTestAlg
// Author: Rachid Mazini, Bo Liu
/////////////////////////////////////////////////////////////////// 
#ifndef TRACKVERTEXASSOCIATIONTOOL_TRACKVERTEXASSOTESTALG_H
#define TRACKVERTEXASSOCIATIONTOOL_TRACKVERTEXASSOTESTALG_H 1

// STL includes
#include <string>

// FrameWork includes
#include "AthenaBaseComps/AthAlgorithm.h"
#include "GaudiKernel/ToolHandle.h"

// Tools
#include "TrackVertexAssociationTool/ITrackVertexAssociationTool.h"

//xAOD 

class TrackVertexAssoTestAlg
  : public ::AthAlgorithm
{ 

  /////////////////////////////////////////////////////////////////// 
  // Public methods: 
  /////////////////////////////////////////////////////////////////// 
 public: 

  // Copy constructor: 

  /// Constructor with parameters: 
  TrackVertexAssoTestAlg( const std::string& name, ISvcLocator* pSvcLocator );

  /// Destructor: 
  virtual ~TrackVertexAssoTestAlg(); 

  // Assignment operator: 
  //TrackVertexAssoTestAlg &operator=(const TrackVertexAssoTestAlg &alg); 

  // Athena algorithm's Hooks
  virtual StatusCode  initialize();
  virtual StatusCode  execute();
  virtual StatusCode  finalize();

  /////////////////////////////////////////////////////////////////// 
  // Const methods: 
  ///////////////////////////////////////////////////////////////////

  /////////////////////////////////////////////////////////////////// 
  // Non-const methods: 
  /////////////////////////////////////////////////////////////////// 

  /////////////////////////////////////////////////////////////////// 
  // Private data: 
  /////////////////////////////////////////////////////////////////// 
 private: 

  /// Default constructor: 
  TrackVertexAssoTestAlg();

  ToolHandle<CP::ITrackVertexAssociationTool> m_TVATool;

  /// Containers
  
  std::string m_trkContname;
  std::string m_vertexContname;

}; 

// I/O operators
//////////////////////

/////////////////////////////////////////////////////////////////// 
// Inline methods: 
/////////////////////////////////////////////////////////////////// 


#endif //> !TRACKVERTEXASSOCIATIONTOOL_TRACKVERTEXASSOTESTALG_H
