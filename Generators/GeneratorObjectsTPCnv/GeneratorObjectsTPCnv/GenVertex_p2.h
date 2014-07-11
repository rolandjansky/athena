///////////////////////// -*- C++ -*- /////////////////////////////

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// GenVertex_p2.h 
// Header file for class GenVertex_p2
// Author: S.Binet<binet@cern.ch>
/////////////////////////////////////////////////////////////////// 
#ifndef GENERATOROBJECTSTPCNV_GENVERTEX_P2_H 
#define GENERATOROBJECTSTPCNV_GENVERTEX_P2_H 

// STL includes
#include <vector>

// HepMC / CLHEP includes

// Gaudi includes

// Forward declaration
class McEventCollectionCnv_p2;

class GenVertex_p2
{ 

  // Make the AthenaPoolCnv class our friend
  friend class McEventCollectionCnv_p2;

  /////////////////////////////////////////////////////////////////// 
  // Public methods: 
  /////////////////////////////////////////////////////////////////// 
 public: 

  /** Default constructor: 
   */
  GenVertex_p2();

  /// Constructor with a fair number of parameters
  GenVertex_p2( const double x, const double y, const double z, const double t,
		const int id, 
		const std::vector<double>::const_iterator weightsBegin,
		const std::vector<double>::const_iterator weightsEnd,
		const int barcode );

  /////////////////////////////////////////////////////////////////// 
  // Protected data: 
  /////////////////////////////////////////////////////////////////// 
 protected: 
  
  /** x-coordinate of the vertex
   */
  double m_x;

  /** y-coordinate of the vertex
   */
  double m_y;

  /** z-coordinate of the vertex
   */
  double m_z;

  /** t-coordinate of the vertex
   */
  double m_t;

  /** collection of barcodes of in-going particles connected to this vertex 
   */
  std::vector<int> m_particlesIn;

  /** collection of barcodes of out-going particles connected to this vertex 
   */
  std::vector<int> m_particlesOut;

  /** Id of this vertex
   */
  int m_id;

  /** Weights for this vertex
   */
  std::vector<float> m_weights;

  /** barcode of this vertex (uniquely identifying a vertex within an event)
   */
  int m_barcode;
}; 

/////////////////////////////////////////////////////////////////// 
/// Inline methods: 
/////////////////////////////////////////////////////////////////// 
inline GenVertex_p2::GenVertex_p2():
  m_x           ( 0 ),
  m_y           ( 0 ),
  m_z           ( 0 ),
  m_t           ( 0 ),
  m_particlesIn (   ),
  m_particlesOut(   ),
  m_id          ( 0 ),
  m_weights     (   ),
  m_barcode     ( 0 )
{}

inline GenVertex_p2::GenVertex_p2( const double x, const double y, 
				   const double z, const double t,
				   const int id, 
				   const std::vector<double>::const_iterator weightsBegin,
				   const std::vector<double>::const_iterator weightsEnd,
				   const int barcode ):
  m_x           ( x ),
  m_y           ( y ),
  m_z           ( z ),
  m_t           ( t ),
  m_particlesIn (   ),
  m_particlesOut(   ),
  m_id          ( id ),
  m_weights     ( weightsBegin, weightsEnd ),
  m_barcode     ( barcode )
{}

#endif //> GENERATOROBJECTSTPCNV_GENVERTEX_P2_H
