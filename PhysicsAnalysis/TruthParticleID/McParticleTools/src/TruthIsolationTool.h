///////////////////////// -*- C++ -*- /////////////////////////////

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MCPARTICLETOOLS_TRUTHISOLATIONTOOL_H 
#define MCPARTICLETOOLS_TRUTHISOLATIONTOOL_H 

// STL includes
#include <string>
#include <list>
#include <unordered_map>

// FrameWork includes
#include "GaudiKernel/ServiceHandle.h"
#include "AthenaBaseComps/AthAlgTool.h"

// McParticleKernel includes
#include "McParticleKernel/ITruthIsolationTool.h"

// McParticleEvent includes
#include "McParticleEvent/TruthParticleParamDefs.h"
#include "HepPDT/ParticleDataTable.hh"

// Forward declarations
class McEventCollection;
class TruthEtIsolations;
#include "AtlasHepMC/GenEvent_fwd.h"
#include "AtlasHepMC/GenParticle_fwd.h"

class TruthIsolationTool : virtual public ITruthIsolationTool,
                                   public AthAlgTool
{ 

  /////////////////////////////////////////////////////////////////// 
  // Public methods: 
  /////////////////////////////////////////////////////////////////// 
 public: 
  TruthIsolationTool( const std::string& type,
		      const std::string& name, 
		      const IInterface* parent );
  virtual ~TruthIsolationTool(); 

  // Athena algorithm's Hooks
  StatusCode initialize();

  /////////////////////////////////////////////////////////////////// 
  // Const methods: 
  ///////////////////////////////////////////////////////////////////

  /**
   * Return the name of the @c TruthEtIsolations container (ie: its 
   * @c StoreGate location) given the @c StoreGate location of a 
   * @c McEventCollection.
   * It returns an empty string if no @c TruthEtIsolations container has been
   * processed by the isolation tool.
   */
  const std::string&
  etIsolationsName( const std::string& mcEvtName ) const;

  /////////////////////////////////////////////////////////////////// 
  // Non-const methods: 
  /////////////////////////////////////////////////////////////////// 

  /** Computes the isolation energies for each of the @c HepMC::GenEvent 
   *  contained into the @c McEventCollection.
   *  One can switch b/w using all particles or only the charged ones.
   *  Default (from interface) is to use all.
   */
  StatusCode buildEtIsolations( const std::string& mcEvtName, 
				ITruthIsolationTool::ParticleSelect sel );

  /** Make an alias in the map of isolation energies
   */
  StatusCode registerAlias( const std::string& originalMcEvtColl,
			    const std::string& aliasMcEvtColl );

  /////////////////////////////////////////////////////////////////// 
  // Private methods: 
  /////////////////////////////////////////////////////////////////// 
 private: 
  
  /** Computes the isolation energies for a given @c HepMC::GenEvent 
   *  contained into the @c McEventCollection.
   */
  StatusCode buildEtIsolations( const std::string& mcEvtName,
				const HepMC::GenEvent* genEvt,
				const std::size_t genIdx,
				TruthEtIsolations& etIsols, 
				ITruthIsolationTool::ParticleSelect sel );

  /** @brief Computes and stores the list of transverse isolation energies
   *  for various cone sizes into the @c TruthEtIsolations container
   */
  void computeIso( const std::list<const HepMC::GenParticle*>& parts, 
		   const HepMC::GenParticle* p,
		   TruthEtIsolations& etIsolations, 
		   ITruthIsolationTool::ParticleSelect sel );

  /** @brief Callback method to ensure consistency of the TruthEtIsolations
   *  prefix key.
   */
  void setupTruthEtIsolationsPrefix( Gaudi::Details::PropertyBase& truthEtIsolationsPrefix );

  /** @brief Callback method to ensure consistency of output McEventCollection
   *  key.
   */
  void setupMcEventsOutput( Gaudi::Details::PropertyBase& mcEventsOutputName );

  /////////////////////////////////////////////////////////////////// 
  // Private data: 
  /////////////////////////////////////////////////////////////////// 
 private: 

  /** Particle Property service
   */
  const HepPDT::ParticleDataTable * m_pdt;

  /// Prefix for the @c TruthEtIsolations container. This is the string which
  /// will be prepended to the key of the @c McEventCollection to build the
  /// (@c StoreGate) output location for the @c TruthEtIsolations.
  /// ie: "GEN_EVENT" --> "<prefix>_GEN_EVENT"
  StringProperty m_prefix;

  /** Name of the @c McEventCollection we should attach isolations to
   */
  StringProperty m_mcEventsOutputName;

  /** Minimum transverse energy of gammas to be taken into account into
   *  the isolation computation.
   *  Default is ptMin = 0.5*GeV
   */
  DoubleProperty m_ptGamMin;

  typedef std::unordered_map<std::string, std::string> EtIsolMap_t;
  /// A map of @c McEventCollection @c StoreGate locations to the according
  /// @c StoreGate location of @c TruthEtIsolations
  EtIsolMap_t m_etIsolMap;

}; 

/////////////////////////////////////////////////////////////////// 
// Inline methods: 
/////////////////////////////////////////////////////////////////// 

#endif 
