///////////////////////// -*- C++ -*- /////////////////////////////

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// GenParticle_p6.h 
// Header file for class GenParticle_p6
// Author: S.Binet<binet@cern.ch>
/////////////////////////////////////////////////////////////////// 
#ifndef GENERATOROBJECTSTPCNV_GENPARTICLE_p6_H 
#define GENERATOROBJECTSTPCNV_GENPARTICLE_p6_H 

// STL includes
#include <vector>
#include <utility> //> for std::pair

// Forward declaration
class McEventCollectionCnv_p6;

class GenParticle_p6
{ 
  // Make the AthenaPoolCnv class our friend
  friend class McEventCollectionCnv_p6;

  /////////////////////////////////////////////////////////////////// 
  // Public methods: 
  /////////////////////////////////////////////////////////////////// 
 public: 

  /** Default constructor: 
   */
  GenParticle_p6();

  /// Constructor with (almost all) parameters (can't construct right
  /// away the flow vector)
  GenParticle_p6( const double px,   const double py, const double pz,
		  const double mass, const int pdgId,
		  const int status,
		  const std::size_t flowSize,
		  const double thetaPolarization,
		  const double phiPolarization,
		  const int prodVtx,
		  const int endVtx,
		  const int barcode,
		  const double generated_mass,
		  const short recoMethod );

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
  float m_px;

  /** y-component of the 4-momentum of this particle
   */
  float m_py;

  /** z-component of the 4-momentum of this particle
   */
  float m_pz;

  /** m-component of the 4-momentum of this particle
   */
  float m_m;

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
  float m_thetaPolarization;

  /** phi polarization
   */
  float m_phiPolarization;

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

  /**  mass of this particle when it was generated
   */
  float m_generated_mass;

  /// switch to know which method to chose to better recover the original
  /// HepLorentzVector.
  /// It is based on isTimelike(), isSpacelike(), isLightlike() and sign(ene)
  /// If m_recoMethod == 
  ///   0: use HepLorentzVector::setVectM( 3vect(px,py,pz), m )
  ///   1: use +(p**2 + m**2) to get the energy (where m**2 is signed)
  ///   2: use -(p**2 + m**2) to get the energy (where m**2 is signed)
  ///       (ex: gluon with negative energy)
  short m_recoMethod;
}; 

/////////////////////////////////////////////////////////////////// 
/// Inline methods: 
/////////////////////////////////////////////////////////////////// 

inline GenParticle_p6::GenParticle_p6():
  m_px           ( 0 ),
  m_py           ( 0 ),
  m_pz           ( 0 ),
  m_m            ( 0 ),
  m_pdgId        ( 0 ),
  m_status       ( 0 ),
  m_flow         ( 0 ),
  m_thetaPolarization ( 0. ),
  m_phiPolarization   ( 0. ),
  m_prodVtx      ( 0 ),
  m_endVtx       ( 0 ),
  m_barcode      ( 0 ),
  m_generated_mass( 0 ),
  m_recoMethod   ( 0 )
{}

inline GenParticle_p6::GenParticle_p6( const double px,
				       const double py,
				       const double pz,
				       const double m,
				       const int pdgId,
				       const int status,
				       const std::size_t flowSize,
				       const double thetaPolarization,
				       const double phiPolarization,
				       const int prodVtx,
				       const int endVtx,
				       const int barcode,  
				       const double generated_mass,
				       const short recoMethod ):
  m_px           ( px ),
  m_py           ( py ),
  m_pz           ( pz ),
  m_m            ( m  ),
  m_pdgId        ( pdgId ),
  m_status       ( status ),
  m_flow         ( flowSize ),
  m_thetaPolarization ( thetaPolarization ),
  m_phiPolarization   ( phiPolarization   ),
  m_prodVtx      ( prodVtx ),
  m_endVtx       ( endVtx  ),
  m_barcode      ( barcode ),
  m_generated_mass( static_cast<float>(generated_mass) ),
  m_recoMethod   ( recoMethod )
{}

#endif //> GENERATOROBJECTSTPCNV_GENPARTICLE_p6_H
