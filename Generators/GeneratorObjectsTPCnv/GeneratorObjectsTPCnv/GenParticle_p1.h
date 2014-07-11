///////////////////////// -*- C++ -*- /////////////////////////////

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// GenParticle_p1.h 
// Header file for class GenParticle_p1
// Author: S.Binet<binet@cern.ch>
/////////////////////////////////////////////////////////////////// 
#ifndef GENERATOROBJECTSTPCNV_GENPARTICLE_P1_H 
#define GENERATOROBJECTSTPCNV_GENPARTICLE_P1_H 

// STL includes
#include <vector>
#include <utility> //> for std::pair

// Forward declaration
class GenEventCnv_p1;
class McEventCollectionCnv_p2;

class GenParticle_p1
{ 
  // Make GenEventCnv_p1 our friend
  friend class GenEventCnv_p1;
  // Make the AthenaPoolCnv class our friend
  friend class McEventCollectionCnv_p2;

  /////////////////////////////////////////////////////////////////// 
  // Public methods: 
  /////////////////////////////////////////////////////////////////// 
 public: 

  /** Default constructor: 
   */
  GenParticle_p1();

  /// Constructor with (almost all) parameters (can't construct right
  /// away the flow vector)
  GenParticle_p1( const double px,  const double py, const double pz,
		  const double ene, const int pdgId,
		  const int status,
		  const std::size_t flowSize,
		  const double thetaPolarization,
		  const double phiPolarization,
		  const int prodVtx,
		  const int endVtx,
		  const int barcode );

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

  /** x-component of the 4-momentum of this particle
   */
  double m_px;

  /** y-component of the 4-momentum of this particle
   */
  double m_py;

  /** z-component of the 4-momentum of this particle
   */
  double m_pz;

  /** e-component of the 4-momentum of this particle
   */
  double m_ene;

  /** identity of this particle, according to the Particle Data Group notation
   */
  int m_pdgId;

  /** Status of this particle, as defined for HEPEVT
   */
  int m_status;

  /** Flow for this particle
   */
  std::vector< std::pair<int, int> > m_flow;

  /** polarization
   */
  //std::pair<double, double> m_polarization;

  /** theta polarization
   */
  double m_thetaPolarization;

  /** phi polarization
   */
  double m_phiPolarization;

  /** Barcode of the production vertex of this particle.
   *  0 means no production vertex
   */
  int m_prodVtx;

  /** Barcode of the decay vertex of this particle.
   *  0 means no production vertex
   */
  int m_endVtx;

  /** barcode of this particles (uniquely identifying this particle within
   *  a given GenEvent)
   */
  int m_barcode;
}; 

/////////////////////////////////////////////////////////////////// 
/// Inline methods: 
/////////////////////////////////////////////////////////////////// 

inline GenParticle_p1::GenParticle_p1():
  m_px           ( 0 ),
  m_py           ( 0 ),
  m_pz           ( 0 ),
  m_ene          ( 0 ),
  m_pdgId        ( 0 ),
  m_status       ( 0 ),
  m_flow         ( 0 ),
  m_thetaPolarization ( 0. ),
  m_phiPolarization   ( 0. ),
  m_prodVtx      ( 0 ),
  m_endVtx       ( 0 ),
  m_barcode      ( 0 )
{}

inline GenParticle_p1::GenParticle_p1( const double px,
				       const double py,
				       const double pz,
				       const double ene,
				       const int pdgId,
				       const int status,
				       const std::size_t flowSize,
				       const double thetaPolarization,
				       const double phiPolarization,
				       const int prodVtx,
				       const int endVtx,
				       const int barcode ):
  m_px           ( px ),
  m_py           ( py ),
  m_pz           ( pz ),
  m_ene          ( ene ),
  m_pdgId        ( pdgId ),
  m_status       ( status ),
  m_flow         ( flowSize ),
  m_thetaPolarization ( thetaPolarization ),
  m_phiPolarization   ( phiPolarization   ),
  m_prodVtx      ( prodVtx ),
  m_endVtx       ( endVtx  ),
  m_barcode      ( barcode )
{}

#endif //> GENERATOROBJECTSTPCNV_GENPARTICLE_P1_H
