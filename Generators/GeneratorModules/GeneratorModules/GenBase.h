/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef GENERATORMODULES_GENBASE_H
#define GENERATORMODULES_GENBASE_H

#include "AthenaBaseComps/AthAlgorithm.h"
#include "GaudiKernel/ServiceHandle.h"
#include "GaudiKernel/IPartPropSvc.h"
#include "GaudiKernel/IIncidentSvc.h"
#include "AthenaKernel/IAtRndmGenSvc.h"
#include "AthenaKernel/errorcheck.h"
#include "StoreGate/DataHandle.h"
#include "GeneratorObjects/McEventCollection.h"

#include "AtlasHepMC/GenEvent.h"
#include "AtlasHepMC/GenVertex.h"

#include "HepPDT/ParticleData.hh"
#include "HepPDT/ParticleDataTable.hh"

#include <string>
#include <vector>
#include <map>


/// @class GenBase
/// @brief Base class for common behaviour of MC truth algorithms.
///
/// This class is the base class used to specify the behavior of all
/// MC truth algorithms and is meant to capture the common behavior
/// of these modules. GenBase inherits the AthAlgorithm interface.
/// Event generator interfaces should inherit from the derived GenModule
/// interface, which adds more generation-specific functionality.
///
/// The common behavior currently consists of:
///   - Standard access to the event collection and its first member
///   - Providing access to the HepPDT ParticleProperties Table
///   - Including standard HepMC and Athena framework headers and CMT uses
///
/// The following virtual methods should be overloaded in the child class:
///
/// @author A. Buckley: Creating GenBase from GenModule, Nov 2013.
///
class GenBase : public AthAlgorithm {
public:

  /// @name Construction/destruction
  //@{

  /// Constructor
  GenBase(const std::string& name, ISvcLocator* pSvcLocator);

  /// Virtual destructor
  virtual ~GenBase() { }

  //@}


  /// @name Event loop algorithm methods
  //@{
  virtual StatusCode initialize();
  virtual StatusCode execute() { return StatusCode::SUCCESS; }
  virtual StatusCode finalize() { return StatusCode::SUCCESS; }
  //@}


  /// @name Event collection accessors (const and non-const)
  //@{

  /// Access the current signal event (first in the McEventCollection)
  ///
  /// @note This function will make a new McEventCollection
  /// if there is not already a valid one _and_ MakeMcEvent=True.
  HepMC::GenEvent* event() {
    if (events()->empty())
      ATH_MSG_ERROR("McEventCollection is empty during first event access");
    return *(events()->begin());
  }

  /// Access the current signal event (const)
  const HepMC::GenEvent* event_const() const {
    if (events_const()->empty())
      ATH_MSG_ERROR("Const McEventCollection is empty during first event access");
    return *(events_const()->begin());
  }

  /// @brief Access the current event's McEventCollection
  ///
  /// @note This function will make a new McEventCollection
  /// if there is not already a valid one _and_ MakeMcEvent=True.
  McEventCollection* events();

  /// Access the current event's McEventCollection (const)
  SG::ReadHandle<McEventCollection> events_const() const {
    SG::ReadHandle<McEventCollection> ret (m_mcevents_const);
    if (!ret.isValid())
      ATH_MSG_ERROR("No McEventCollection found in StoreGate with key " << m_mcEventKey);
    return ret;
  }

  //@}


  /// @name Particle data accessors
  //@{

  /// Access the particle property service
  const ServiceHandle<IPartPropSvc> partPropSvc() const {
    return m_ppSvc;
  }

  /// Get a particle data table
  const HepPDT::ParticleDataTable& particleTable() const {
    return *(m_ppSvc->PDT());
  }

  /// Shorter alias to get a particle data table
  const HepPDT::ParticleDataTable& pdt() const { return particleTable(); }

  /// Access an element in the particle data table
  const HepPDT::ParticleData* particleData(int pid) const {
    return pdt().particle(HepPDT::ParticleID(std::abs(pid)));
  }
  //@}


protected:

  /// @name Properties
  //@{
  /// StoreGate key for the MC event collection (defaults to GEN_EVENT)
  std::string m_mcEventKey;
  /// Flag to determine if a new MC event collection should be made if it doesn't exist
  bool m_mkMcEvent;
  //@}


  /// @name Utility event-mangling functions
  /// @todo Replace with HepMC units when available
  //@{
  /// Scale event energies/momenta by x 1000
  void GeVToMeV(HepMC::GenEvent* evt);
  /// Scale event energies/momenta by x 1/1000
  void MeVToGeV(HepMC::GenEvent* evt);
  /// Scale event lengths by x 10
  void cmTomm(HepMC::GenEvent* evt);
  /// Scale event lengths by x 1/10
  void mmTocm(HepMC::GenEvent* evt);
  //@}


private:

  /// Handle on the particle property service
  ServiceHandle<IPartPropSvc> m_ppSvc;

  /// Const handle to the MC event collection
  SG::ReadHandle<McEventCollection> m_mcevents_const;

};


#endif
