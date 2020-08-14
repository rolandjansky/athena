/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "RootTruthParticleCnvTool.h"
#include "GeneratorObjects/McEventCollection.h"
#include "McParticleEvent/TruthParticle.h"
#include "McParticleEvent/TruthParticleContainer.h"
#include "boost/array.hpp"
#include "TError.h"
#include "TDatabasePDG.h"
#include "TParticlePDG.h"


// GeneratorObjects includes
#include "GeneratorObjects/HepMcParticleLink.h"

namespace {

/** 3*charge for basic pdgId codes -- used to parse unknown id's
    Fix from Frank for the charge of the MC Truth Particle */
static const boost::array<int, 100> qcharge = {
  {+0, -1, +2, -1, +2, -1, +2, -1, +2, +0,  // 0-9
   +0, -3, +0, -3, +0, -3, +0, -3, +0, +0,  // 10-19
   +0, +0, +0, +3, +0, +0, +0, +0, +0, +0,  // 20-29
   +0, +0, +0, +3, +0, +0, +0, +3, +0, +0,  // 30-39
   +0, +0, +0, +0, +0, +0, +0, +0, +0, +0,
   +0, +0, +0, +0, +0, +0, +0, +0, +0, +0,
   +0, +0, +0, +0, +0, +0, +0, +0, +0, +0,
   +0, +0, +0, +0, +0, +0, +0, +0, +0, +0,
   +0, +0, +0, +0, +0, +0, +0, +0, +0, +0,
   +0, +0, +0, +0, +0, +0, +0, +0, +0, +0}
};

}

////////////////////////////////////////////////////////////////////////////////
//
//         Implementation of the ITruthParticleCnvTool function(s)
//

StatusCode RootTruthParticleCnvTool::execute() const { abort(); }
StatusCode RootTruthParticleCnvTool::execute(const EventContext&) const { abort(); }

StatusCode
RootTruthParticleCnvTool::convert(const McEventCollection *mcCollection,
                                  const unsigned int genEventIndex,
                                  TruthParticleContainer * container,
                                  const ITruthParticleVisitor* visitor )
  const 
{
  container->clear();

  if ( 0 == mcCollection ) {
    ::Warning ("RootTruthParticleCnvTool",
               "Null pointer to McEventCollection !");
    return StatusCode::RECOVERABLE;
  }

  if ( mcCollection->size() <= genEventIndex ) {
    ::Warning ("RootTruthParticleCnvTool",
               "McEventCollection size: %ui; Requested element nbr : %ui !!",
               // we could use %zd instead of using %ui and casting size_t to
               // unsigned int, but %zd is not ANSI...
               (unsigned int)mcCollection->size(), 
               genEventIndex);
    return StatusCode::RECOVERABLE;
  }

  const HepMC::GenEvent * evt = (*mcCollection)[genEventIndex];
  container->setGenEvent( mcCollection, genEventIndex );

  // reserve enough space for the container so we don't have to relocate it
  container->reserve( evt->particles_size() );

  /// Create a map to enhance access between GenParticles and TruthParticles
  TruthParticleContainer::Map_t bcToMcPart;

  const HepMC::GenEvent::particle_const_iterator itrEnd = evt->particles_end();
  for ( HepMC::GenEvent::particle_const_iterator itrPart=evt->particles_begin();
	itrPart != itrEnd;
	++itrPart ) {
    const HepMC::GenParticle * hepMcPart = (*itrPart);

    TruthParticle * mcPart = new TruthParticle( hepMcPart, container );
    container->push_back( mcPart );

    if ( visitor ) {
      visitor->visit( mcPart );
    }

    mcPart->setCharge( chargeFromPdgId( mcPart->pdgId() ) );
    mcPart->setGenEventIndex( genEventIndex);

    if ( hepMcPart != mcPart->genParticle() ) {
      ::Error ("RootTruthParticleCnvTool",
               "TruthParticle is not wrapping the GenParticle : %d !!",
               hepMcPart->barcode());
    }
    HepMcParticleLink mcLink( hepMcPart->barcode(), genEventIndex, EBC_MAINEVCOLL, HepMcParticleLink::IS_POSITION );
    bcToMcPart[ mcLink.compress() ] = mcPart;

  }//> end loop over particles

  // at this point the whole GenEvent has been proxied.
  // so we can setup its VectorMap
  container->setParticles( bcToMcPart );

#if 0
  // connect the TruthParticleContainer to the container of TruthEtIsolations
  // if it exists and if we are asked for
  if ( m_doEtIsolation.value() ) {
    const std::string& etIsolName 
      = m_isolationTool->etIsolationsName( container->genEventName() );
    if ( etIsolName.empty() ) {
      m_msg << MSG::WARNING
	    << "Could not retrieve the name of the TruthEtIsolations container"
	    << endmsg;
      return StatusCode::RECOVERABLE;
    }

    const TruthEtIsolationsContainer* etIsols = 0;
    if ( !m_storeGate->retrieve( etIsols, etIsolName ).isSuccess() ) {
      m_msg << MSG::WARNING
	    << "Could not retrieve the TruthEtIsolations container at ["
	    << etIsolName << "] !!"
	    << endmsg;
      return StatusCode::RECOVERABLE;
    }

    // index of HepMC::GenEvent within the McEventCollection is the same
    // than the one of the TruthEtIsolations within the TruthEtIsolationsCont.
    container->setEtIsolations( etIsols, genEventIndex );
  }
#endif

  return StatusCode::SUCCESS;
}

//
////////////////////////////////////////////////////////////////////////////////

double RootTruthParticleCnvTool::chargeFromPdgId (int pdgId) const
{
  const double third = 1./3;
  if (0 == pdgId)
    return -999;
  TParticlePDG* ap = TDatabasePDG::Instance()->GetParticle (pdgId);
  if ( ap ) {
    return ap->Charge()*third;
  } else {
    /** Set charge using PDG convention:
	id = nnnnijkl
	i == 0, j == 0:   see qcharge[100]
	i == 0:           meson, j kbar quarks    l = 2*spin+1
	i != 0:           baryon, i j k quarks    l = 2*spin+1
	Default is 0; */
    const int idmod = std::abs(pdgId) % 10000;
    const int q1 = (idmod/10) % 10;
    const int q2 = (idmod/100) % 10;
    const int q3 = (idmod/1000) % 10;
    double q = 0;

    if (abs(pdgId) >= 1000000000) {
      // Seems to be a nucleus: 100pppnnn0
      q = (abs(pdgId) / 10000) % 1000;
    }
    else if( idmod < 100 ) {
      q = qcharge[idmod]*third;
    }
    else if ( idmod < 1000 ) {
      q = (qcharge[q1]-qcharge[q2])*third;
      if ( qcharge[q2] == 2 ) {
	q *= -1.;
      }
    }
    else if( idmod < 10000 ) {
      q = (qcharge[q3]+qcharge[q2]+qcharge[q1])*third;
    }
    if (q == 0) q = 0; // Change -0 to 0.
    return (pdgId < 0) ? -q : q;
  }
}

////////////////////////////////////////////////////////////////////////////////
//
//              Implementation of the IInterface function(s)

StatusCode RootTruthParticleCnvTool::queryInterface( const InterfaceID&,
                                                     void** ) { abort(); }
unsigned long RootTruthParticleCnvTool::addRef() { abort(); }
unsigned long RootTruthParticleCnvTool::release() { abort(); }
unsigned long RootTruthParticleCnvTool::refCount() const { abort(); }

//
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
//
//               Implementation of the IProperty function(s)
//

StatusCode RootTruthParticleCnvTool::setProperty( const std::string& ) {
   abort(); }
StatusCode RootTruthParticleCnvTool::setProperty( const std::string&, const Gaudi::Details::PropertyBase& ) { abort(); }
StatusCode RootTruthParticleCnvTool::setPropertyRepr( const std::string&, const std::string& ){ abort(); }
StatusCode RootTruthParticleCnvTool::getProperty( Gaudi::Details::PropertyBase* ) const { abort(); }
const Gaudi::Details::PropertyBase&
RootTruthParticleCnvTool::getProperty( const std::string& ) const{ abort(); }
StatusCode RootTruthParticleCnvTool::getProperty( const std::string&,
                                                  std::string& ) const {
   abort(); }
const std::vector< Gaudi::Details::PropertyBase* >&
RootTruthParticleCnvTool::getProperties() const { abort(); }
bool RootTruthParticleCnvTool::hasProperty( const std::string& ) const {
   abort(); }

//
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
//
//               Implementation of the IAlgTool function(s)
//

const std::string& RootTruthParticleCnvTool::type() const { abort(); }
const IInterface* RootTruthParticleCnvTool::parent() const { abort(); }
StatusCode RootTruthParticleCnvTool::configure() { abort(); }
StatusCode RootTruthParticleCnvTool::initialize() { abort(); }
StatusCode RootTruthParticleCnvTool::sysInitialize() { abort(); }
StatusCode RootTruthParticleCnvTool::reinitialize() { abort(); }
StatusCode RootTruthParticleCnvTool::sysReinitialize() { abort(); }
StatusCode RootTruthParticleCnvTool::start() { abort(); }
StatusCode RootTruthParticleCnvTool::sysStart() { abort(); }
StatusCode RootTruthParticleCnvTool::restart() { abort(); }
StatusCode RootTruthParticleCnvTool::sysRestart() { abort(); }
StatusCode RootTruthParticleCnvTool::stop() { abort(); }
StatusCode RootTruthParticleCnvTool::sysStop() { abort(); }
StatusCode RootTruthParticleCnvTool::finalize() { abort(); }
StatusCode RootTruthParticleCnvTool::sysFinalize() { abort(); }
StatusCode RootTruthParticleCnvTool::terminate() { abort(); }
Gaudi::StateMachine::State RootTruthParticleCnvTool::FSMState() const {
   abort(); }

//
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
//
//              Implementation of the INamedInterface function(s)
//

const std::string& RootTruthParticleCnvTool::name() const{abort();}

//
////////////////////////////////////////////////////////////////////////////////
