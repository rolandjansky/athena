///////////////////////// -*- C++ -*- /////////////////////////////

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// OverlapBaseTool.h 
// Header file for class OverlapBaseTool
// Author: S.Binet<binet@cern.ch>
/////////////////////////////////////////////////////////////////// 
#ifndef ASSOCIATIONCOMPS_OVERLAPBASETOOL_H 
#define ASSOCIATIONCOMPS_OVERLAPBASETOOL_H 

// STL includes
#include <string>

// HepMC / CLHEP includes

// FrameWork includes
#include "AthenaBaseComps/AthAlgTool.h"
#include "GaudiKernel/ToolHandle.h"

// AssociationKernel includes
#include "AssociationKernel/IOverlapTool.h"

// Forward declaration

class OverlapBaseTool : virtual public IOverlapTool,
                                public AthAlgTool
{ 

  /////////////////////////////////////////////////////////////////// 
  // Public methods: 
  /////////////////////////////////////////////////////////////////// 
 public: 

  // Copy constructor: 

  /// Constructor with parameters: 
  OverlapBaseTool( const std::string& type,
		   const std::string& name, 
		   const IInterface* parent );

  /// Destructor: 
  virtual ~OverlapBaseTool(); 

  // Athena algorithm's Hooks
  virtual StatusCode  initialize();

  /////////////////////////////////////////////////////////////////// 
  // Const methods: 
  ///////////////////////////////////////////////////////////////////

  /////////////////////////////////////////////////////////////////// 
  // Non-const methods: 
  /////////////////////////////////////////////////////////////////// 

  StatusCode append( IOverlapTool* tool );

  /** Delegate this specific part of initialization to concrete
   *  algtool
   */
  virtual StatusCode initializeTool();

  /////////////////////////////////////////////////////////////////// 
  // Protected method: 
  /////////////////////////////////////////////////////////////////// 
 protected: 

  /** Default constructor: 
   */
  OverlapBaseTool();

  /////////////////////////////////////////////////////////////////// 
  // Protected data: 
  /////////////////////////////////////////////////////////////////// 
 protected: 

  typedef ToolHandleArray<IOverlapTool> IOverlapTools_t;

  /** List of private IOverlapTools which will decide if 2 objects are
   *  overlaping and hence associated
   */
  IOverlapTools_t m_overlapTools;

}; 

// I/O operators
//////////////////////

/////////////////////////////////////////////////////////////////// 
// Inline methods: 
/////////////////////////////////////////////////////////////////// 

#endif //> ASSOCIATIONCOMPS_OVERLAPBASETOOL_H
