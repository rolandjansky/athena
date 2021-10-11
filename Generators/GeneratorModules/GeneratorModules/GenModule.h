/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef GENERATORMODULES_GENMODULE_H
#define GENERATORMODULES_GENMODULE_H

#include "GeneratorModules/GenBase.h"
#include "CLHEP/Random/RandomEngine.h"
#include "CLHEP/Random/RandPoisson.h"
#include <memory>
#ifdef HEPMC3
#include "AtlasHepMC/GenEvent.h"
#endif


/// @brief Base class for common behaviour of generator interfaces.
///
/// This class is the base class used to specify the behavior of all
/// event generator interface packages. It inherits the GenBase (and hence
/// AthAlgorithm) interfaces.
///
/// The common behavior currently consists of:
///   - Providing access to the ATLAS random number generator service
///   - Creating the McEvent and if necessary the McEventCollection
///
/// The following virtual methods should be overloaded in the child class:
///
///    StatusCode genInitialize()
///    StatusCode callGenerator()
///    StatusCode genFinalize()
///    StatusCode fillEvt(GeneratorEvent* evt)
///
/// @author M. Shapiro: Initial code, March 2000
/// @author A. Buckley: Update to AthAlgorithm etc., November 2009
///                     General interface/implementation improvements, Jan/Feb 2010
///                     Removing stripPartons and multiple event mechanisms and
///                       rebasing on GenBase, Nov 2013
///
class GenModule : public GenBase {
public:

  /// @name Construction/destruction
  //@{
  /// Constructor
  GenModule(const std::string& name, ISvcLocator* pSvcLocator);
  /// Virtual destructor
  virtual ~GenModule() { }
  //@}


  /// @name Event loop algorithm methods: not to be overloaded
  //@{
  StatusCode initialize();
  StatusCode execute();
  StatusCode finalize() { return genFinalize(); }
  //@}


  /// @name Gen-specific event loop methods: to be overloaded
  //@{
  /// For initializing the generator, if required
  virtual StatusCode genInitialize() { return StatusCode::SUCCESS; }
  /// For initialization of user code, if required. Called after genInitialize
  virtual StatusCode genuserInitialize() { return StatusCode::SUCCESS; }
  /// For calling the generator on each iteration of the event loop
  virtual StatusCode callGenerator() { return StatusCode::SUCCESS; }
  /// For filling the HepMC event object
  virtual StatusCode fillEvt(HepMC::GenEvent* evt) = 0;
  /// For finalising the generator, if required
  virtual StatusCode genFinalize() { return StatusCode::SUCCESS; }
  //@}


protected:

  /// @name Features for derived classes to use internally
  //@{
  /// Get the ATLAS random number service
  IAtRndmGenSvc& atRndmGenSvc() { return *m_rndmGenSvc; }
  /// Get the random number generator engine
  CLHEP::HepRandomEngine& randomEngine();
  //@}

  /// Seed for random number engine
  int m_randomSeed;

  /// Flag for normal vs. afterburner generators
  int m_isAfterburner;

#ifdef HEPMC3
  /// The run info for HepMC3
  std::shared_ptr<HepMC3::GenRunInfo> m_runinfo;  
#endif

private:

  /// Data members
  //@{
  /// Random number generator objects
  std::unique_ptr<CLHEP::HepRandomEngine> m_pRandomEngine;
  // Random number service
  ServiceHandle<IAtRndmGenSvc> m_rndmGenSvc;
  /// Handle on the incident service
  ServiceHandle<IIncidentSvc> m_incidentSvc;
  //@}

};


#endif
