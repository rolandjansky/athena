///////////////////////// -*- C++ -*- /////////////////////////////

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// AssocBuilderTool.h 
// Header file for class AssocBuilderTool
// Author: S.Binet<binet@cern.ch>
/////////////////////////////////////////////////////////////////// 
#ifndef ASSOCIATIONCOMPS_ASSOCBUILDERTOOL_H 
#define ASSOCIATIONCOMPS_ASSOCBUILDERTOOL_H 

// STL includes

// FrameWork includes
#include "AthenaBaseComps/AthAlgTool.h"
#include "GaudiKernel/ToolHandle.h"

// AssociationKernel includes
#include "AssociationKernel/IAssocBuilderTool.h"
#include "AssociationKernel/IOverlapTool.h"

// Forward declaration

class AssocBuilderTool : virtual public IAssocBuilderTool,
                                 public AthAlgTool
{ 
  using IAssocBuilderTool::buildAssocs;

  /////////////////////////////////////////////////////////////////// 
  // Public methods: 
  /////////////////////////////////////////////////////////////////// 
 public: 

  // Copy constructor: 

  /// Constructor with parameters: 
  AssocBuilderTool( const std::string& type,
		    const std::string& name, 
		    const IInterface* parent );

  /// Destructor: 
  virtual ~AssocBuilderTool(); 

  // Athena algorithm's Hooks
  virtual StatusCode  initialize();

  /////////////////////////////////////////////////////////////////// 
  // Const methods: 
  ///////////////////////////////////////////////////////////////////

  StatusCode buildAssocs( const ElementLink<INavigable4MomentumCollection>& obj1,
			  const INavigable4MomentumCollection& objectsToAssociate,
			  INav4MomAssocs& mapOfAssociations ) const;

  /////////////////////////////////////////////////////////////////// 
  // Non-const methods: 
  /////////////////////////////////////////////////////////////////// 

 protected:
  
  /////////////////////////////////////////////////////////////////// 
  // Const methods: 
  ///////////////////////////////////////////////////////////////////

  /////////////////////////////////////////////////////////////////// 
  // Non-const methods: 
  /////////////////////////////////////////////////////////////////// 

  /////////////////////////////////////////////////////////////////// 
  // Protected data: 
  /////////////////////////////////////////////////////////////////// 
 protected: 

  /** Default constructor: 
   */
  AssocBuilderTool();

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

#endif //> ASSOCIATIONCOMPS_ASSOCBUILDERTOOL_H
