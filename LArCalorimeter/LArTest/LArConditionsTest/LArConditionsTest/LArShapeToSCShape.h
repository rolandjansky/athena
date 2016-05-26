/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef LARCONDITIONSTEST_LARSHAPETOSCSHAPE_H
#define LARCONDITIONSTEST_LARSHAPETOSCSHAPE_H 1

// STL includes
#include <string>

#include "AthenaBaseComps/AthAlgorithm.h"
#include "CaloDetDescr/ICaloSuperCellIDTool.h"
#include "LArCabling/LArSuperCellCablingTool.h"

#include "GaudiKernel/ToolHandle.h"

class LArShapeToSCShape
  : public ::AthAlgorithm
{ 

  /////////////////////////////////////////////////////////////////// 
  // Public methods: 
  /////////////////////////////////////////////////////////////////// 
 public: 

  // Copy constructor: 

  /// Constructor with parameters: 
  LArShapeToSCShape( const std::string& name, ISvcLocator* pSvcLocator );

  /// Destructor: 
  virtual ~LArShapeToSCShape(); 

  // Assignment operator: 
  //LArShapeToSCShape &operator=(const LArShapeToSCShape &alg); 

  // Athena algorithm's Hooks
  StatusCode  initialize();
  StatusCode  execute();
  StatusCode  finalize();

 private: 

  /// Default constructor: 
  LArShapeToSCShape();


  ToolHandle<ICaloSuperCellIDTool> m_scidTool;
  ToolHandle<LArSuperCellCablingTool> m_scCablingTool;

  

}; 

#endif //> !LARCONDITIONSTEST_LARSHAPETOSCSHAPE_H
