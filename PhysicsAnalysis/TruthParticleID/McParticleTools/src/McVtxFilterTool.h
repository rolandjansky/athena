///////////////////////// -*- C++ -*- /////////////////////////////

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// McVtxFilterTool.h 
// Header file for class McVtxFilterTool
// Author: S.Binet<binet@cern.ch>
// Modified: D. Schouten <dschoute@sfu.ca>
/////////////////////////////////////////////////////////////////// 
#ifndef MCPARTICLETOOLS_MCVTXFILTERTOOL_H 
#define MCPARTICLETOOLS_MCVTXFILTERTOOL_H 1

/** \class McVtxFilterTool is an AlgTool which filters an input 
 *  McEventCollection and outputs a new one from the decay patterns it has 
 *  been setup with.
 *  The decay patterns describe the decay vertices one is looking for and are 
 *  modeled via a simple std::string.
 *  One can specify multiple decay patterns and modify them at the jobOption 
 *  level via the property "DecayPatterns".
 *  One can also explicitly specify to record the signal process vertex of the 
 *  input McEventCollection into the new one, even if it does not satisfy a 
 *  single decay pattern in the list of decay vertices to look for.
 *
 *  Note that the real work of the filtering is performed by the McVtxFilter
 *  class which is located in the AnalysisUtils package.
 * 	 
 *  Added new properties: 	 
 *   - "FillTree" jobOption allows one to save the HepMC tree of outgoing 	 
 *  particles from decay vertices chosen by the "DecayPatterns" jobOption. 	 
 * 	 
 *   - "KeepParticles" specifies PDG codes of outgoing particles to keep from 	 
 *  decay vertices specified by "DecayPatterns". 	 
 * 	 
 *  ex. suppose have processes like: 	 
 *  u + ubar -> gluon(->jet(s)) + photon(->e+/e- + bremsstrahlung photons) 	 
 * 	 
 *  and want to filter out only the jet(s) from the gluon. Use these jobOptions
 *  \code 	 
 *  DecayPatterns   = [ "-2+2 -> 21 + 22" ] # select the process 	 
 *  ParticlesToKeep = [ 21 ]             # only keep outgoing gluon 	 
 *  FillTree        = True               # keep all decay products of the gluon
 *  \endcode 	 
 */

// STL includes
#include <string>
#include <vector>

// HepMC / CLHEP includes

// FrameWork includes
#include "Gaudi/Property.h"
#include "GaudiKernel/ServiceHandle.h"
#include "AthenaBaseComps/AthAlgTool.h"

// DataModel includes
#include "AthContainers/DataVector.h"

// McParticleUtils includes
#include "McParticleUtils/McVtxFilter.h"

// McParticleKernel includes
#include "McParticleKernel/IMcVtxFilterTool.h"

// Forward declaration
class McEventCollection;
#include "AtlasHepMC/GenVertex_fwd.h"

class McVtxFilterTool : virtual public IMcVtxFilterTool,
			        public AthAlgTool
{ 

  /////////////////////////////////////////////////////////////////// 
  // Public enums: 	 
  /////////////////////////////////////////////////////////////////// 	 
 public: 	 
  struct VtxType { 	 
    enum Flag { 	 
      Unknown = -1, 	 
      IsRootVertex = 0, 	 
      IsNotRootVertex 	 
    }; 	 
  }; 	 
  
  /////////////////////////////////////////////////////////////////// 	 
  // Public methods: 
  /////////////////////////////////////////////////////////////////// 
 public: 

  // Copy constructor: 

  /// Constructor with parameters: 
  McVtxFilterTool( const std::string& type,
		   const std::string& name, 
		   const IInterface* parent );

  /// Destructor: 
  virtual ~McVtxFilterTool(); 

  // Assignment operator: 
  //McVtxFilterTool &operator=(const McVtxFilterTool &alg); 

  // Athena algorithm's Hooks
  virtual StatusCode  initialize();
  virtual StatusCode  execute();

  /////////////////////////////////////////////////////////////////// 
  // Const methods: 
  ///////////////////////////////////////////////////////////////////

  /** Display the decay pattern of each McVtxFilter this Tool is embedding
   */
  void displayOptions() const;

  /** Display the statistics for each McVtxFilter this Tool is embedding
   */
  void stats() const;

  /** Return true if one of the McVtxFilters accept this GenVertex
   */
  virtual bool isAccepted( const HepMC::GenVertex* vtx ) const;

  /////////////////////////////////////////////////////////////////// 
  // Non-const methods: 
  /////////////////////////////////////////////////////////////////// 

  /** Filter the McEventCollection according to the list of McVtxFilter
   *  embedded in this AlgTool, and fill the output McEventCollection
   */
  virtual void filterMcEventCollection( const McEventCollection* mcCollection,
					McEventCollection* filterColl );

  /////////////////////////////////////////////////////////////////// 
  // Protected methods: 
  /////////////////////////////////////////////////////////////////// 
 protected: 

  /// Default constructor: 
  McVtxFilterTool();

  /** Add the vertex to the event : nothing very special, just to factorize code
   *  Input GenVertex is const because we add a <b>copy</b> of it to the GenEvent.
   *  The last parameter is used here to add (or not) the whole decay tree at a given
   *  vertex. (Default: just add the vertex, not the whole tree of children)
   */
  void addVertex( const HepMC::GenVertex* vtx, HepMC::GenEvent * evt,
		  const VtxType::Flag vtxType = VtxType::IsNotRootVertex ) const;

  /** Helper function: return true if we are at a root vertex (i.e., a vertex
   *  that satisfies one of the chosen decay modes) and if the PDG of the particle
   *  is in the m_particles list (or the m_particle list is empty).
   */
  bool keepParticle( const VtxType::Flag vtxType, const HepMC::GenParticle* p ) const;

  /** @brief Callback method to ensure consistency of filters.
   *  Method to ensure the synchronisation between the filters
   *  and the decay patterns they are supposed to match.
   *  This is to cope with the case where this tool is called by some
   *  parent AlgTool/Algorithm which wants to override the DecayPatterns
   *  property given by the JobOptionSvc.
   *  We ensure the consistency between those 2 entities by defining a callback
   *  function (@setupFilters) to perform the job.
   */
  void setupFilters( Gaudi::Details::PropertyBase& decayPatterns );

  /////////////////////////////////////////////////////////////////// 
  // Protected data: 
  /////////////////////////////////////////////////////////////////// 
 protected: 

  // Containers
  
  /** Location of the McEventCollection to be filtered
   */
  StringProperty m_mcEventsName;

  /** Output McEventCollection location (filtered from the McEventCollection)
   */
  StringProperty m_mcEventsOutputName;

  /** Patterns of the decays to look for (ex: "23 -> -5 + 5" )
   */
  StringArrayProperty m_decayPatterns;

  /** Booleans to setup each of the McVtxFilter::matchSign
   */
  BooleanArrayProperty m_matchSigns;

  /** Booleans to setup each of the McVtxFilter::matchBranch
   */
  BooleanArrayProperty m_matchBranches;

  /** Filters
   */
  DataVector<McVtxFilter> m_filters;

  /** Stores the total number of filtered vertices for each McVtxFilter
   */
  std::vector<double> m_counter;

  /** Switch to include or not the signal_process_vertex 
   *  into the McEventCollection (default = true)
   */
  BooleanProperty m_doSignalProcessVtx;

  /** Switch to fill the entire tree from a decay vertex
   */
  BooleanProperty m_fillTree;

  /** List of outgoing particles from the decay vertex to include
   */
  LongArrayProperty m_particles;
}; 

/// I/O operators
//////////////////////

/////////////////////////////////////////////////////////////////// 
/// Inline methods: 
/////////////////////////////////////////////////////////////////// 

#endif //> MCPARTICLETOOLS_MCVTXFILTERTOOL_H
