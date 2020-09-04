///////////////////////// -*- C++ -*- /////////////////////////////

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// DeltaROverlapTool.h 
// Header file for class DeltaROverlapTool
// Author: S.Binet<binet@cern.ch>
/////////////////////////////////////////////////////////////////// 
#ifndef ASSOCIATIONCOMPS_DELTAROVERLAPTOOL_H 
#define ASSOCIATIONCOMPS_DELTAROVERLAPTOOL_H 

// STL includes

// AssociationUtils includes
#include "DeltaROverlapFilter.h"

// AssociationComps includes
#include "OverlapBaseTool.h"

// Forward declaration

class DeltaROverlapTool : public OverlapBaseTool
{ 

  /////////////////////////////////////////////////////////////////// 
  // Public methods: 
  /////////////////////////////////////////////////////////////////// 
 public: 

  // Copy constructor: 

  /// Constructor with parameters: 
  DeltaROverlapTool( const std::string& type,
		     const std::string& name, 
		     const IInterface* parent );

  /// Destructor: 
  virtual ~DeltaROverlapTool(); 

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
  bool overlap( const INavigable4Momentum& obj1, 
		const INavigable4Momentum& obj2, 
		std::list<const INavigable4Momentum*>* overlappingObjects ) const;

  /////////////////////////////////////////////////////////////////// 
  // Non-const methods: 
  /////////////////////////////////////////////////////////////////// 

  /** Initialization of the tool (this is being indirected from base class
   *  @c AlgTool : @c OverlapBaseTool )
   */
  StatusCode initializeTool();

  /////////////////////////////////////////////////////////////////// 
  // Protected methods: 
  /////////////////////////////////////////////////////////////////// 
 protected: 

  /** Default constructor: 
   */
  DeltaROverlapTool();

  /** Call-back method to configure the "XyzDeltaR" properties (min/max)
   */
  void setupDeltaR( Gaudi::Details::PropertyBase& deltaR );

  /////////////////////////////////////////////////////////////////// 
  // Protected data: 
  /////////////////////////////////////////////////////////////////// 
 protected: 

  /** DeltaR-Overlap predicate
   */
  AssociationComps::DeltaROverlapFilter m_filter;

  /** Minimum delta-R for the overlap filter
   */
  DoubleProperty m_minDeltaR;

  /** Maximum delta-R for the overlap filter
   */
  DoubleProperty m_maxDeltaR;

}; 

// I/O operators
//////////////////////

/////////////////////////////////////////////////////////////////// 
// Inline methods: 
/////////////////////////////////////////////////////////////////// 

inline void DeltaROverlapTool::setupDeltaR( Gaudi::Details::PropertyBase& /*maxDeltaR*/ )
{
  m_filter = AssociationComps::DeltaROverlapFilter( m_minDeltaR.value(), m_maxDeltaR.value() );
  return;
}

#endif //> ASSOCIATIONCOMPS_DELTAROVERLAPTOOL_H
