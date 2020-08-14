///////////////////////// -*- C++ -*- /////////////////////////////

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// EtaPtFilterTool.h 
// Header file for class EtaPtFilterTool
// Author: S.Binet<binet@cern.ch>
/////////////////////////////////////////////////////////////////// 
#ifndef MCPARTICLETOOLS_ETAPTFILTERTOOL_H 
#define MCPARTICLETOOLS_ETAPTFILTERTOOL_H 1

// STL includes
#include <string>
#include <map>

// McParticleUtils includes
#include "McParticleUtils/McVtxFilter.h"

// McParticleTools includes
#include "TruthParticleFilterBaseTool.h"

// Forward declaration

class EtaPtFilterTool : public TruthParticleFilterBaseTool
{ 

  /////////////////////////////////////////////////////////////////// 
  // Public enum: 
  /////////////////////////////////////////////////////////////////// 
 public: 
  
  /** Enum which holds the definition of status codes as given by the
   *  standard HEPEVT (the so-called FORTRAN standard for event generator 
   *  output).
   */
 public: 	 
  struct HepEvt { 	 
    enum StatusCode { 	 
      Unknown = -1, 	 
      NullEntry = 0, 	 
      NotDecayed = 1,
      Decayed    = 2,
      DocLine    = 3  // -> this is Pythia specific !
    }; 	 
  }; 	 

  /////////////////////////////////////////////////////////////////// 
  // Public methods: 
  /////////////////////////////////////////////////////////////////// 
 public: 

  // Copy constructor: 

  /// Constructor with parameters: 
  EtaPtFilterTool( const std::string& type,
		   const std::string& name, 
		   const IInterface* parent );

  /// Destructor: 
  virtual ~EtaPtFilterTool(); 

  /////////////////////////////////////////////////////////////////// 
  // Const methods: 
  ///////////////////////////////////////////////////////////////////

  /////////////////////////////////////////////////////////////////// 
  // Non-const methods: 
  /////////////////////////////////////////////////////////////////// 

  /** This method will check the validity of the input McEventCollection 
   *  and build a filtered one from the strategy implemented by this
   *  concrete tool.
   */
  StatusCode buildMcAod( const McEventCollection* in, McEventCollection* out );

  /////////////////////////////////////////////////////////////////// 
  // Protected methods: 
  /////////////////////////////////////////////////////////////////// 
 protected: 

  /** Default constructor: 
   */
  EtaPtFilterTool();

  /////////////////////////////////////////////////////////////////// 
  // Const methods: 
  ///////////////////////////////////////////////////////////////////

  /** This method will check the validity of the input HepMC::GenEvent
   *  and build a filtered one from the strategy implemented by this
   *  concrete tool.
   */
  StatusCode buildGenEvent( const HepMC::GenEvent* in, HepMC::GenEvent* out );

  /** Check if a given particle is within the acceptance (pt+eta)
   */
  bool isAccepted( HepMC::ConstGenParticlePtr mcPart ) const;

  /** Check if a given vertex has at least one in-going
   *  or out-going particle within the acceptance (pt+eta)
   */
  bool isAccepted( const HepMC::GenVertex* vtx ) const;

  /** Check if a given vertex is the signal process vertex. */
  bool isSignalProcessVertex( const HepMC::GenVertex* vtx, 
			      const HepMC::GenEvent* evt );

  /** Helper method to copy a given vertex and add it to a GenEvent
   */
  StatusCode addVertex( const HepMC::GenVertex* srcVtx, HepMC::GenEvent* evt, 
			bool isSignalVertex = false ) const;

  /** @brief Helper method to check if this @c HepMC::GenVertex looks
   *  like the hard-scattering vertex.
   *  As this information is not stored into the @c HepMC::GenEvent
   *  we have to rely on some ad-hoc procedure. Moreover, it is a very
   *  generator-dependent.
   *  Here is the adopted "solution":
   *   - check this vertex belongs to the ~20 first vertices. This can
   *  be configured through @c m_maxHardScatteringVtxBarcode via the
   *  property "MaxHardScatteringVtxBarcode". Default is 20.
   *   - check that there are 2 incoming partons. This is done using
   *  a @c McVtxFilter predicate.
   */
  bool isFromHardScattering( const HepMC::GenVertex* vtx ) const;
  
  /////////////////////////////////////////////////////////////////// 
  // Non-const methods: 
  /////////////////////////////////////////////////////////////////// 

  /** Method to initialize the tool: we need to check the validity of 
   *  the parameters given for the inner and outer eta regions
   */
  StatusCode initializeTool();

  /** Callback function to setup the "InnerEtaRegionCuts" property
   */
  void setupInnerEtaRegionCuts( Gaudi::Details::PropertyBase& innerEtaRegionCuts );

  /** Callback function to setup the "OuterEtaRegionCuts" property
   */
  void setupOuterEtaRegionCuts( Gaudi::Details::PropertyBase& outerEtaRegionCuts );

  /////////////////////////////////////////////////////////////////// 
  // Protected data: 
  /////////////////////////////////////////////////////////////////// 
 protected: 

  /** vector of cuts parameters for the inner region delimited in |eta|
   *   0-th element is the minimum |eta| of this region
   *   1-st element is the maximum |eta| of this region
   *   2-nd element is the minimum pt for the stable particles to be accepted
   */
  DoubleArrayProperty m_innerEtaRegionCuts;

  /** vector of cuts parameters for the outer region delimited in |eta|
   *   0-th element is the minimum |eta| of this region
   *   1-st element is the maximum |eta| of this region
   *   2-nd element is the minimum pt for the stable particles to be accepted
   */
  DoubleArrayProperty m_outerEtaRegionCuts;

  /** Property to setup the maximum vertex barcode to look for hard-scattering
   *  vertices.
   *  Default is 20.
   *  Note that you don't want to increase this number by a too big a number
   *  otherwise the downstream selection of the vertex could pick-up showering
   *  particles.
   */
  IntegerProperty m_maxHardScatteringVtxBarcode;

  /** Switch to only include particles from generation and reject particles 
   *  from detector simulation (Geant4)
   */
  BooleanProperty m_onlyGenerator;

  /** Switch to keep *all* stable generator particles (IsGenStable) regardless 
   *  what eta or pt cuts are defined
   */
  BooleanProperty m_butKeepAllGeneratorStable;

  /** Switch to keep *all* generator particles which are documentaries
   *  (statuscode == 3)
   */
  BooleanProperty m_keepDocumentaries;

  /** Predicate to select pp->X vertices where p is a parton (q,g)
   *  This will select vertices:
   *    q+q' -> X
   *    q+g  -> X
   *    g+g  -> X
   */
  McVtxFilter m_ppFilter;

  /** Predicate to remove shower vertices: X -> 92 | 94
   */
  McVtxFilter m_showerFilter;
  
  /* For the isolation */
  std::map<int,double> m_isol_energies;

}; 

/// I/O operators
//////////////////////

/////////////////////////////////////////////////////////////////// 
/// Inline methods: 
/////////////////////////////////////////////////////////////////// 

#endif //> MCPARTICLETOOLS_ETAPTFILTERTOOL_H
