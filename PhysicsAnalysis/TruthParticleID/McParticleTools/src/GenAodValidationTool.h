///////////////////////// -*- C++ -*- /////////////////////////////

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// GenAodValidationTool.h 
// Header file for class GenAodValidationTool
// Author: S.Binet<binet@cern.ch>
/////////////////////////////////////////////////////////////////// 
#ifndef MCPARTICLETOOLS_GENAODVALIDATIONTOOL_H 
#define MCPARTICLETOOLS_GENAODVALIDATIONTOOL_H 

// STL includes
#include <string>
#include <fstream>

// FrameWork includes
#include "GaudiKernel/ToolHandle.h"

// McParticleUtils includes
#include "McParticleUtils/McVtxFilter.h"

// McParticleTools includes
#include "TruthParticleValidationBaseTool.h"

// Forward declaration
class IIOHepMcTool;
class McEventCollection;
#include "AtlasHepMC/GenParticle_fwd.h"
#include "AtlasHepMC/GenVertex_fwd.h"
#include "AtlasHepMC/GenEvent_fwd.h"

class GenAodValidationTool : public TruthParticleValidationBaseTool
{ 

  /////////////////////////////////////////////////////////////////// 
  // Public methods: 
  /////////////////////////////////////////////////////////////////// 
 public: 

  // Copy constructor: 

  /// Constructor with parameters: 
  GenAodValidationTool( const std::string& type,
			const std::string& name, 
			const IInterface* parent );

  /// Destructor: 
  virtual ~GenAodValidationTool(); 

  /////////////////////////////////////////////////////////////////// 
  // Const methods: 
  ///////////////////////////////////////////////////////////////////

  /////////////////////////////////////////////////////////////////// 
  // Non-const methods: 
  /////////////////////////////////////////////////////////////////// 

  /////////////////////////////////////////////////////////////////// 
  // Protected methods: 
  /////////////////////////////////////////////////////////////////// 
 protected: 

  /** Default constructor: 
   */
  GenAodValidationTool();

  // to be implemented (if needed) by derived classes
  virtual StatusCode  initializeTool();
  virtual StatusCode  finalizeTool();

  /** Main method to perform the validation.
   *  Just a wrapper around the more API-detailed method.
   */
  virtual StatusCode  executeTool();

  /** Main method to perform the validation:
   *   - check that each vertex is identical to the original one
   *   - check that missing particles are not there for a good reason
   */
  virtual StatusCode  executeTool( const McEventCollection* refMcEvents,
				   const McEventCollection* checkMcEvents );

  /** Main method to perform the validation (directly onto the @c GenEvents):
   *   - check that each vertex is identical to the original one
   *   - check that missing particles are not there for a good reason
   */
  virtual StatusCode  executeTool( const HepMC::GenEvent* refMcEvents,
  				   const HepMC::GenEvent* checkMcEvents );

  /** @brief Check that 2 given vertices are the same:
   *   - check same number of branches
   *   - check same particles (4-mom + id + status)
   */
  bool compareVtx( const HepMC::GenVertex* vtx1,
		   const HepMC::GenVertex* vtx2 ) const;

  /** @brief Check that 2 given particles are the same:
   *    - 4-mom 
   *    - id 
   *    - status
   *    - flow
   *    - polarization
   */
  bool compareParts( const HepMC::GenParticle* p1,
		     const HepMC::GenParticle* p2 ) const;

  /** Request pointers to the HepMcWriterTools to be able to write 
   *  out HepMC::GenEvent for further comparison (off-framework)
   */
  StatusCode setupHepMcWriterTools();
  
  /////////////////////////////////////////////////////////////////// 
  // Protected data: 
  /////////////////////////////////////////////////////////////////// 
 protected: 

  /** Location of the reference McEventCollection
   */
  StringProperty m_refMcEventsName;

  /** Location of the 'to-be-validated' McEventCollection
   */
  StringProperty m_checkMcEventsName;

  /** Name of the output text file which will contain the hard-scattering
   *  vertices. Default = "hepmc.hard.vtx.txt"
   */
  StringProperty m_hardVtxOutFileName;

  /** Pointer to the output text file containing hard-scattering vertices
   */
  std::ofstream * m_outFile;

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
  
  /** shorthand for lazy people (good coders are lazy people, aren't they ?)
   */
  typedef ToolHandle<IIOHepMcTool> HepMcTool_t;

  /** Tool to write the reference HepMC::GenEvent into a dedicated file
   */
  HepMcTool_t m_refMcEventWriter;

  /** Tool to write the 'to-be-checked' HepMC::GenEvent into a dedicated file
   */
  HepMcTool_t m_checkMcEventWriter;

}; 

/// I/O operators
//////////////////////

/////////////////////////////////////////////////////////////////// 
/// Inline methods: 
/////////////////////////////////////////////////////////////////// 

#endif //> MCPARTICLETOOLS_GENAODVALIDATIONTOOL_H
