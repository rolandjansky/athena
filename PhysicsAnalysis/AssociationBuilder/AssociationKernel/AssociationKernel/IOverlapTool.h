/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/////////////////////////////////////////////////////////////////// 
// @file: AssociationKernel/IOverlapTool.h 
// Header file for class IOverlapTool
// @author: S.Binet<binet@cern.ch>
/////////////////////////////////////////////////////////////////// 
#ifndef ASSOCIATIONKERNEL_IOVERLAPTOOL_H 
#define ASSOCIATIONKERNEL_IOVERLAPTOOL_H 

/** @class IOverlapTool is the main interface to check if two 
 *  @c INavigable4Momentum objects are overlapping.
 *  Concrete classes have to implement the following method:
 *  <code>bool overlap(obj1, obj2, predicate, overlappingObjects)</code>
 *
 *  The philosophy is to put all the overlapping decision into the
 *  @c predicate object so that we don't require people to use @c AlgTools.
 *  However, we can more easily configure (ie: at the jobO level) the 
 *  overlap-checking if we wrap the @c predicate into an @c AlgTool.
 */

// STL includes
#include <list>
#include <functional>

// FrameWork includes
#include "GaudiKernel/IAlgTool.h"
#include "GaudiKernel/IProperty.h"

// EventKernel includes
#include "EventKernel/INavigable4Momentum.h"

// NavFourMom includes
#include "NavFourMom/INavigable4MomentumCollection.h"

// AnalysisAssociation includes
//#include "AnalysisAssociation/IAssocFilter.h"

// Forward declaration

static const InterfaceID IID_IOverlapTool("IOverlapTool", 1, 0);

class IOverlapTool : virtual public IAlgTool,
                     virtual public IProperty
{ 

  /////////////////////////////////////////////////////////////////// 
  // Private typedefs: 
  /////////////////////////////////////////////////////////////////// 
 protected: 
  typedef std::binary_function<const INavigable4Momentum&, // first argument
                               const INavigable4Momentum&, // second argument
                               bool                        // result type
                              > OverlapFilter_t;
  /////////////////////////////////////////////////////////////////// 
  // Public methods: 
  /////////////////////////////////////////////////////////////////// 
 public: 

  /** Destructor: 
   */
  virtual ~IOverlapTool();

  /////////////////////////////////////////////////////////////////// 
  // Const methods: 
  ///////////////////////////////////////////////////////////////////

  /**
   * @brief Overlap-checking method
   * @param obj1 The @c INavigable4Momentum for which we want its overlaps.
   * @param obj2 The @c INavigable4Momentum being tested for overlaps with @param obj1.
   * @param overlappingObjects The list of objects satisfying the @param predicate
   *                   criteria.
   */
  virtual bool overlap( const INavigable4Momentum& obj1, 
                        const INavigable4Momentum& obj2, 
                        std::list<const INavigable4Momentum*>* overlappingObjects = 0 ) const = 0;

  /**
   * @brief Overlap-checking method. Returns true if at least one overlap is
   *        detected
   * @param obj1 The @c INavigable4Momentum for which we want its overlaps.
   * @param objectsToCheck The @c INavigable4MomentumCollection being tested 
   *                       for overlaps with @param obj1.
   * @param overlappingObjects The list of objects satisfying the 
   *                           @param predicate criteria.
   */
  bool overlap( const INavigable4Momentum& obj1,
                const INavigable4MomentumCollection& objectsToCheck,
                std::list<const INavigable4Momentum*>* overlappingObjects = 0 ) const;

  /**
   * @brief Overlap-checking method. Returns true if at least one overlap is
   *        detected
   * @param obj1 The @c INavigable4Momentum for which we want its overlaps.
   * @param firstObjectToCheck The start @c iterator defining the range in
   *                           between overlaps are checked
   * @param lastObjectToCheck  The past-end @c iterator defining the range in
   *                           between overlaps are checked
   * @param overlappingObjects The list of objects satisfying the 
   *                           @param predicate criteria.
   */
  bool overlap( const INavigable4Momentum& obj1,
                const INavigable4MomentumCollection::const_iterator& firstObjectToCheck,
                const INavigable4MomentumCollection::const_iterator&  lastObjectToCheck,
                std::list<const INavigable4Momentum*>* overlappingObjects = 0 ) const;

  /////////////////////////////////////////////////////////////////// 
  // Non-const methods: 
  /////////////////////////////////////////////////////////////////// 

  static const InterfaceID& interfaceID();

  /** Append an @c IOverlapTool to this @c IOverlapTool
   */
  virtual StatusCode append( IOverlapTool* tool ) = 0;

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
inline const InterfaceID& IOverlapTool::interfaceID() 
{ 
   return IID_IOverlapTool; 
}


#endif //> ASSOCIATIONKERNEL_IOVERLAPTOOL_H
