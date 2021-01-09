///////////////////////// -*- C++ -*- /////////////////////////////

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// VtxBasedFilterTool.h 
// Header file for class VtxBasedFilterTool
// Author: S.Binet<binet@cern.ch>
/////////////////////////////////////////////////////////////////// 
#ifndef MCPARTICLETOOLS_VTXBASEDFILTERTOOL_H 
#define MCPARTICLETOOLS_VTXBASEDFILTERTOOL_H 

// STL includes
#include <string>
#include <map>


// McParticleUtils includes
#include "McParticleUtils/McVtxFilter.h"

// McParticleTools includes
#include "TruthParticleFilterBaseTool.h"

// Forward declaration
/** @brief class to filter a @c McEventCollection based only on vertices
 *  grounds (hard-scatter + whatever the @c McVtxFilterTool has been
 *  configured to keep via its @c DecayPatterns property)
 */
class VtxBasedFilterTool : public TruthParticleFilterBaseTool
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
  VtxBasedFilterTool( const std::string& type,
		      const std::string& name, 
		      const IInterface* parent );

  /// Destructor: 
  virtual ~VtxBasedFilterTool(); 


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
  VtxBasedFilterTool();

  /////////////////////////////////////////////////////////////////// 
  // Const methods: 
  ///////////////////////////////////////////////////////////////////

  /** This method will check the validity of the input HepMC::GenEvent
   *  and build a filtered one from the strategy implemented by this
   *  concrete tool.
   */
  StatusCode buildGenEvent( const HepMC::GenEvent* in, HepMC::GenEvent* out );

  /** Check if a given vertex is satisfying any selection criterion
   */
  bool isAccepted( const HepMC::GenVertex* vtx ) const;

  /** Helper method to copy a given vertex and add it to a GenEvent
   */
  StatusCode addVertex( const HepMC::GenVertex* srcVtx, 
			HepMC::GenEvent* evt ) const;

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

  /////////////////////////////////////////////////////////////////// 
  // Protected data: 
  /////////////////////////////////////////////////////////////////// 
 protected: 

  /** Property to setup the maximum vertex barcode to look for hard-scattering
   *  vertices.
   *  Default is 20.
   *  Note that you don't want to increase this number by a too big a number
   *  otherwise the downstream selection of the vertex could pick-up showering
   *  particles.
   */
  IntegerProperty m_maxHardScatteringVtxBarcode;

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
  
}; 


#endif //> MCPARTICLETOOLS_VTXBASEDFILTERTOOL_H
