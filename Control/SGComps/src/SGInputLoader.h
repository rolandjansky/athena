///////////////////////// -*- C++ -*- /////////////////////////////

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// SGInputLoader.h 
// Header file for class SGInputLoader
/////////////////////////////////////////////////////////////////// 
#ifndef SGCOMPS_SGINPUTLOADER_H
#define SGCOMPS_SGINPUTLOADER_H 1

// STL includes
#include <string>

// FrameWork includes
#include "GaudiKernel/DataObjID.h"
#include "AthenaBaseComps/AthAlgorithm.h"
#include "GaudiKernel/DataObjID.h"


class SGInputLoader
  : public ::AthAlgorithm
{ 

  /////////////////////////////////////////////////////////////////// 
  // Public methods: 
  /////////////////////////////////////////////////////////////////// 
 public: 

  // Copy constructor: 

  /// Constructor with parameters: 
  SGInputLoader( const std::string& name, ISvcLocator* pSvcLocator );

  /// Destructor: 
  virtual ~SGInputLoader(); 

  // Assignment operator: 
  //SGInputLoader &operator=(const SGInputLoader &alg); 

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
  SGInputLoader();

  void loader(Property&);

  /// Containers
  DataObjIDColl m_load;

  bool m_dump;
  
  

}; 


#endif //> !SGCOMPS_SGINPUTLOADER_H
