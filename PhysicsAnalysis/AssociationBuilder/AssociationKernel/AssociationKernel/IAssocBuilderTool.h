/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/////////////////////////////////////////////////////////////////// 
// IAssocBuilderTool.h 
// Header file for class IAssocBuilderTool
// Author: S.Binet<binet@cern.ch>
/////////////////////////////////////////////////////////////////// 
#ifndef ASSOCIATIONKERNEL_IASSOCBUILDERTOOL_H 
#define ASSOCIATIONKERNEL_IASSOCBUILDERTOOL_H 

/** @class IAssocBuilderTool is the interface towards AlgTools which
 *  aim at creating association between @c INavigable4Momentum objects.
 */

// STL includes

// FrameWork includes
#include "GaudiKernel/IAlgTool.h"
#include "GaudiKernel/IProperty.h"

// DataModel includes
#include "AthLinks/ElementLink.h"

// EventKernel includes
#include "EventKernel/INavigable4Momentum.h"

// NavFourMom includes
#include "NavFourMom/INavigable4MomentumCollection.h"
#include "NavFourMom/INav4MomAssocs.h"

// Forward declaration

static const InterfaceID IID_IAssocBuilderTool("IAssocBuilderTool", 1, 0);

class IAssocBuilderTool : virtual public IAlgTool,
                          virtual public IProperty
{ 

  /////////////////////////////////////////////////////////////////// 
  // Public methods: 
  /////////////////////////////////////////////////////////////////// 
 public: 

  /** Destructor: 
   */
  virtual ~IAssocBuilderTool();

  /////////////////////////////////////////////////////////////////// 
  // Const methods: 
  ///////////////////////////////////////////////////////////////////

  StatusCode buildAssocs( const ElementLink<INavigable4MomentumCollection>& obj1,
                          const std::vector<const INavigable4MomentumCollection*>& objectsToAssociate,
                          INav4MomAssocs& mapOfAssociations ) const;

  StatusCode buildAssocs( const ElementLink<INavigable4MomentumCollection>& obj1,
                          const std::vector<const INavigable4MomentumCollection*>::const_iterator&  firstObjectToAssociate,
                          const std::vector<const INavigable4MomentumCollection*>::const_iterator&   lastObjectToAssociate,
                          INav4MomAssocs& mapOfAssociations ) const;

  virtual StatusCode buildAssocs( const ElementLink<INavigable4MomentumCollection>& obj1,
                                  const INavigable4MomentumCollection& objectsToAssociate,
                                  INav4MomAssocs& mapOfAssociations ) const = 0;

  /////////////////////////////////////////////////////////////////// 
  // Non-const methods: 
  /////////////////////////////////////////////////////////////////// 
  static const InterfaceID& interfaceID();

  /////////////////////////////////////////////////////////////////// 
  // Protected data: 
  /////////////////////////////////////////////////////////////////// 
 protected: 

}; 

/// I/O operators
//////////////////////

/////////////////////////////////////////////////////////////////// 
/// Inline methods: 
/////////////////////////////////////////////////////////////////// 
inline const InterfaceID& IAssocBuilderTool::interfaceID() 
{ 
   return IID_IAssocBuilderTool; 
}

#endif //> ASSOCIATIONKERNEL_IASSOCBUILDERTOOL_H
