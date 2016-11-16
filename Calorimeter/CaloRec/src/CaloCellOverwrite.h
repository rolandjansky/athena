//Dear emacs, this is -*- C++ -*- 

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


#ifndef CALOREC_CALOCELLOVERWRITE_H
#define CALOREC_CALOCELLOVERWRITE_H 1

// STL includes
#include <string>

// FrameWork includes
#include "AthenaBaseComps/AthAlgorithm.h"
#include "CaloIdentifier/CaloCellGroup.h"


class CaloCellOverwrite
  : public ::AthAlgorithm
{ 

  /////////////////////////////////////////////////////////////////// 
  // Public methods: 
  /////////////////////////////////////////////////////////////////// 
 public: 

  // Copy constructor: 

  /// Constructor with parameters: 
  CaloCellOverwrite( const std::string& name, ISvcLocator* pSvcLocator );

  /// Destructor: 
  virtual ~CaloCellOverwrite(); 

  // Athena algorithm's Hooks
  virtual StatusCode  initialize();
  virtual StatusCode  execute();
  virtual StatusCode  finalize();

 private: 

  /// Default constructor: 
  CaloCellOverwrite();

  std::vector<std::string> m_factorsPerGroupInput;
  std::string m_containerKey;
  CaloCellGroupList m_factorsPerGroup;
 

}; 

// I/O operators
//////////////////////

/////////////////////////////////////////////////////////////////// 
// Inline methods: 
/////////////////////////////////////////////////////////////////// 


#endif //> !CALORIMETER/CALOREC_CALOCELLOVERWRITE_H
