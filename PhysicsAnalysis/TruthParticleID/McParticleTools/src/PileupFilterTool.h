////////////////////////////-*- C++ -*- /////////////////////////// 

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// PileupFilterTool.h 
// Header file for class PileupFilterTool
// Author: KeteviA.Assamagan<ketevi@bnl.gov>
/////////////////////////////////////////////////////////////////// 
#ifndef MCPARTICLETOOLS_PILEUPFILTERTOOL_H 
#define MCPARTICLETOOLS_PILEUPFILTERTOOL_H 1

// STL includes
#include <set>

// FrameWork includes

// McParticleTools includes
#include "TruthParticleFilterBaseTool.h"

// Forward declaration
namespace TruthHelper{
     class GenAccessIO;
}

#include "AtlasHepMC/GenEvent_fwd.h"
class IMcVtxFilterTool;

class PileupFilterTool : public TruthParticleFilterBaseTool
{ 

  /////////////////////////////////////////////////////////////////// 
  // Public methods: 
  /////////////////////////////////////////////////////////////////// 
 public: 

  // Copy constructor: 

  /// Constructor with parameters: 
  PileupFilterTool( const std::string& type,
		    const std::string& name, 
		    const IInterface* parent );

  /// Destructor: 
  virtual ~PileupFilterTool(); 

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
  PileupFilterTool();

  /** Retrieve the GenParticles from the GenEvent object (located into the
   *  McEventCollection container), filter them and store their barcode into 
   *  the list of barcodes of particles to keep
   */
  StatusCode selectSpclMcBarcodes();

  /** Build the out McEventCollection by removing all the GenParticles 
   *  whose barcode is not in the list of barcodes to keep.
   *  Remove also the dead vertices.
   */
  StatusCode shapeGenEvent( McEventCollection * outEvt );

  /** Reconnect the particles of the filtered McEventCollection.
   *  We need the original McEventCollection to know if 
   *  2 particles were connected
   */
  StatusCode reconnectParticles( const McEventCollection* inEvt,
				 McEventCollection* out );

  /** Rebuild the links between particles which were connected, eg by a long
   *  FSR chain
   */
  StatusCode rebuildLinks( const HepMC::GenEvent * mcEvent,
			   HepMC::GenEvent * outEvt,
			   HepMC::GenParticle * mcPart );

  /** to get tesIO **/
  StatusCode initializeTool();

  /////////////////////////////////////////////////////////////////// 
  // Protected data: 
  /////////////////////////////////////////////////////////////////// 
 protected: 

  /** List of particles (in fact their unique barcode) which have been
   *  labelled as "special"
   */
  std::set<int> m_barcodes;

  /** deltaR isolation energy cut for electrons, muons, taus and photons
   */
  DoubleProperty m_rIsol;

  /** Minimum threshold for transverse momentum of photons
   */
  DoubleProperty m_ptGamMin;

  /** Minimum threshold for transverse momentum for all particles.
   *  Warning: this cut is applied <b>before</b> Pt photon cut !
   */
  DoubleProperty m_ptMin;

  /** Eta acceptance cut applied on all stable particles
   */
  DoubleProperty m_etaRange;

  /** Radius acceptance cut on all stable particles
   */
  DoubleProperty m_rRange;
  
  /** Switch to include or not particles from detector simulation (Geant4)
   */
  BooleanProperty m_includeSimul;

  /** Switch to include or not parton showers
   */
  BooleanProperty m_includePartonShowers;

  /** Pointer to a StoreGateSvc helper (Mc particle retrieval with predicates)
   */
  TruthHelper::GenAccessIO* m_tesIO;

  /** Switch to remove particles which decay into themselves (t->tg) <b>but</b>
   *  only for generated particles, not the ones from the Geant4 interactions.
   */
  BooleanProperty m_removeDecayToSelf;

}; 

// I/O operators
//////////////////////

/////////////////////////////////////////////////////////////////// 
// Inline methods: 
/////////////////////////////////////////////////////////////////// 

#endif //> MCPARTICLETOOLS_PILEUPFILTERTOOL_H

