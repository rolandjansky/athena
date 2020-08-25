/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


// STL includes
#include <cmath>
#include <stdexcept>
#include <sstream>
#include <fstream>

// FrameWork includes
#include "GaudiKernel/IPartPropSvc.h"

// CLHEP/HepMC includes
#include "TruthHelper/IsGenStable.h"
#include "TruthHelper/IsGenInteracting.h"
#include "AtlasHepMC/GenEvent.h"
#include "AtlasHepMC/GenParticle.h"
#include "AtlasHepMC/GenVertex.h"
#include "GeneratorObjects/McEventCollection.h"
#include "CLHEP/Units/SystemOfUnits.h"
#include "CLHEP/Vector/LorentzVector.h"

// McParticle includes
#include "McParticleEvent/TruthEtIsolationsContainer.h"
#include "McParticleUtils/McUtils.h"

// McParticleTools includes
#include "TruthIsolationTool.h"

using namespace TruthHelper;
using CLHEP::GeV;

using CLHEP::HepLorentzVector;
namespace {
  inline 
  HepLorentzVector svToLv( const HepMC::FourVector& v )
  { return HepLorentzVector( v.x(), v.y(), v.z(), v.t() ); }
}

typedef std::list<const HepMC::GenParticle*> GenParticles_t;

TruthIsolationTool::TruthIsolationTool( const std::string& type, 
					const std::string& name, 
					const IInterface* parent ) : 
  AthAlgTool ( type, name,   parent ),
  m_pdt      ( 0 )
{
  //
  // Property declaration
  // 

  declareProperty( "ptGammaMin", 
		   m_ptGamMin = 0.5*GeV,
		   "Minimum transverse energy of gammas to be taken into "
		   "account into the isolation computation." );

  declareProperty( "TruthEtIsolationsPrefix",
		   m_prefix = "TruthEtIsol",
		   "Prefix for the TruthEtIsolations container. This is the "
		   "string which will be prepended to the key of the "
		   "McEventCollection to build the (StoreGate) output "
		   "location for the TruthEtIsolations.\nie: \"GEN_EVENT\" "
		   "--> \"<prefix>_GEN_EVENT\"" );
  m_prefix.declareUpdateHandler
    ( &TruthIsolationTool::setupTruthEtIsolationsPrefix,
      this );

  declareProperty( "McEventsOutput",
		   m_mcEventsOutputName = "GEN_AOD",
		   "Name of the McEventCollection we should attach "
		   "isolations to" );
  m_mcEventsOutputName.declareUpdateHandler
    ( &TruthIsolationTool::setupMcEventsOutput,
      this );

  declareInterface<ITruthIsolationTool>(this);
}

TruthIsolationTool::~TruthIsolationTool()
{ 
}

StatusCode TruthIsolationTool::initialize() 
{
  // retrieve StoreGate
  if ( !evtStore().retrieve().isSuccess() ) {
    ATH_MSG_ERROR("Could not get a handle on StoreGateSvc !!");
    return StatusCode::FAILURE;
  }

  ATH_MSG_INFO(" McEventsOutput: [" << m_mcEventsOutputName.value() << "]");

  // Get the Particle Properties Service
  ServiceHandle<IPartPropSvc> partPropSvc("PartPropSvc", name());
  if ( !partPropSvc.retrieve().isSuccess() ) {
    ATH_MSG_ERROR(" Could not initialize Particle Properties Service");
    return StatusCode::FAILURE;
  }      

  m_pdt = partPropSvc->PDT();
  if ( 0 == m_pdt ) {
    ATH_MSG_ERROR("Could not retrieve HepPDT::ParticleDataTable from "\
		  "ParticleProperties Service !!");
    return StatusCode::FAILURE;
  }

  return StatusCode::SUCCESS;
}

const std::string&
TruthIsolationTool::etIsolationsName( const std::string& mcEvtName ) const
{
  static const std::string s_emptyString = "";
  const EtIsolMap_t::const_iterator i = m_etIsolMap.find(mcEvtName);
  if ( i != m_etIsolMap.end() ) {
    return i->second;
  }

  return s_emptyString;
}

StatusCode 
TruthIsolationTool::buildEtIsolations( const std::string& mcEvtName,
				       ITruthIsolationTool::ParticleSelect partSel )
{
  // retrieve collection
  const McEventCollection* mcEvts = 0;
  if ( !evtStore()->retrieve( mcEvts, mcEvtName ).isSuccess() ) {
    ATH_MSG_WARNING("Could not retrieve a McEventCollection at ["
		    << mcEvtName << "] !!" << endmsg
		    << "No Et-isolations will be computed !");
    return StatusCode::RECOVERABLE;
  }

  // the name of the output (filtered) McEventCollection we want to attach
  // the Et-isolations to
  const std::string& outMcEvtName = m_mcEventsOutputName.value();

  // create the container of TruthEtIsolations
  std::ostringstream truthEtIsolName;
  truthEtIsolName << m_prefix.value() 
		  << (partSel == ITruthIsolationTool::UseChargedOnly 
		      ? "Charged"
		      : "") 
		  << "_" << outMcEvtName;

  TruthEtIsolationsContainer * etIsolations = new TruthEtIsolationsContainer;
  if ( !evtStore()->record( etIsolations, 
			     truthEtIsolName.str() ).isSuccess() ) {
    delete etIsolations;
    etIsolations = 0;
    ATH_MSG_WARNING("Could not record a TruthEtIsolations container at ["
		    << truthEtIsolName.str() << "] !!");
    return StatusCode::RECOVERABLE;
  }
  if ( !evtStore()->setConst( etIsolations ).isSuccess() ) {
    ATH_MSG_WARNING("Could not setConst the TruthEtIsolations container at ["
		    << truthEtIsolName.str() << "] !!");
  }

  // update our registry of EtIsol StoreGate locations
  m_etIsolMap[outMcEvtName] = truthEtIsolName.str();

  bool allGood = true;
  for ( std::size_t iMc = 0, iMax = mcEvts->size(); iMc != iMax; ++iMc ) {
    TruthEtIsolations * etIsols = new TruthEtIsolations( outMcEvtName, iMc );
    etIsolations->push_back( etIsols );
    if ( !buildEtIsolations( mcEvtName, (*mcEvts)[iMc], iMc,
			     *etIsols, partSel ).isSuccess() ) {
      msg(MSG::WARNING)
	<< "Problem encountered while computing Et-isolations for idx=["
	<< iMc << "] of McEventCollection [" << mcEvtName << "] !!"
	<< endmsg;
      allGood = false;
    }
  }
  return allGood ? StatusCode::SUCCESS : StatusCode::RECOVERABLE;
}

StatusCode 
TruthIsolationTool::buildEtIsolations( const std::string& mcEvtName,
				       const HepMC::GenEvent* genEvt,
				       const std::size_t genIdx,
				       TruthEtIsolations& etIsols, 
				       ITruthIsolationTool::ParticleSelect partSel )
{
  if ( 0 == genEvt ) {
    msg(MSG::WARNING)
      << "Null pointer to GenEvent (idx = [" << genIdx << "] from "
      << "McEventCollection [" << mcEvtName << "]) !!"
      << endmsg;
    return StatusCode::RECOVERABLE;
  }

  static const IsGenStable isStable;
  static const IsGenInteracting isInteracting;

  // create a reduced list of particles
  GenParticles_t particles;
  for ( HepMC::GenEvent::particle_const_iterator 
	  i    = genEvt->particles_begin(),
	  iEnd = genEvt->particles_end();
	i != iEnd;
	++i ) {
    if ( isStable(*i) && isInteracting(*i) ) {
      particles.push_back( *i );
    }
  }

  for ( HepMC::GenEvent::particle_const_iterator
	  i    = genEvt->particles_begin(),
	  iEnd = genEvt->particles_end(); 
	i != iEnd;
	++i ) {
    const HepMC::FourVector hlv = (*i)->momentum();
    const int    ida = std::abs((*i)->pdg_id());
    const int    sta = (*i)->status();
    const double pt  = hlv.perp();

    // Compute isolation only for photon, electron, muon or tau. 
    // Not for documentation particle
    const bool doComputeIso = ( ( ida == 22 && pt > m_ptGamMin ) ||
                                ida == 11 || ida == 13 || ida == 15 ) &&
                                sta != 3 && isInteracting(*i);
    if ( doComputeIso ) {
      computeIso( particles, *i, etIsols, partSel );
    }
  }
  
  return StatusCode::SUCCESS;
}

void
TruthIsolationTool::computeIso( const GenParticles_t& particles, 
				const HepMC::GenParticle* part,
				TruthEtIsolations& etIsolations, 
				ITruthIsolationTool::ParticleSelect partSel  )
{
  static const IsGenInteracting isInteracting;
  const HepLorentzVector hlv = ::svToLv(part->momentum());
  const int ida = std::abs(part->pdg_id());

  McAod::EtIsolations etIsol = { {0.*GeV, 0.*GeV, 0.*GeV, 0.*GeV,
				  0.*GeV, 0.*GeV, 0.*GeV, 0.*GeV} };
  McAod::EtIsolations pxi = etIsol;
  McAod::EtIsolations pyi = etIsol;

  for ( GenParticles_t::const_iterator
	  i    = particles.begin(),
	  iEnd = particles.end(); 
	i != iEnd; 
	++i ) {
    if ( (*i)->barcode() == part->barcode() ) {
      continue;
    }
    if( partSel == ITruthIsolationTool::UseChargedOnly ) {
      double particleCharge = McUtils::chargeFromPdgId((*i)->pdg_id(),
						       m_pdt);
      if( std::abs(particleCharge)<1.e-2 )
	continue;
    }
    const HepLorentzVector itrHlv = ::svToLv((*i)->momentum());
    const double r = hlv.deltaR(itrHlv);
    for ( std::size_t iCone = 0; 
	  iCone != TruthParticleParameters::NbrOfCones; 
	  ++iCone ) {
      if ( r < TruthParticleParameters::coneCut(static_cast<TruthParticleParameters::ConeSize>(iCone) ) ) {
	pxi[iCone] += itrHlv.px();
	pyi[iCone] += itrHlv.py();
      }
    }
  }

  //
  // Correction for tau (as was done in the old tool for the time being)
  double pxv = 0.*GeV;
  double pyv = 0.*GeV;
  auto decVtx = part->end_vertex();
  if (ida == 15 && decVtx) {
    HepMC::GenVertex::particle_iterator child  = decVtx->particles_begin(HepMC::children);
    HepMC::GenVertex::particle_iterator childE = decVtx->particles_end(HepMC::children);
    for (; child != childE; ++child) {
      if ( isInteracting(*child) ) {
	if( partSel == ITruthIsolationTool::UseChargedOnly ) {
	  double particleCharge = McUtils::chargeFromPdgId((*child)->pdg_id(),
							   m_pdt);
	  if( std::abs(particleCharge)<1.e-2 )
	    continue;
	}
 	const HepMC::FourVector childHlv = (*child)->momentum();
	pxv += childHlv.px();
	pyv += childHlv.py();
      }
    }
  }

  for ( std::size_t i = 0; 
        i != static_cast<std::size_t>(TruthParticleParameters::NbrOfCones);
        ++i ) {
    pxi[i] -= pxv;
    pyi[i] -= pyv;
    etIsol[i] = std::sqrt(pxi[i]*pxi[i]+pyi[i]*pyi[i]);
  }

  etIsolations.setEtIsol( part, etIsol );
  return;
}

void 
TruthIsolationTool::setupTruthEtIsolationsPrefix( Gaudi::Details::PropertyBase& /*truthEtIsolationsPrefix*/ )
{
  // no-op for now
}

void 
TruthIsolationTool::setupMcEventsOutput( Gaudi::Details::PropertyBase& /*mcEventsOutputName*/ )
{
  // no-op for now
}

StatusCode 
TruthIsolationTool::registerAlias( const std::string& originalMcEvtColl,
				   const std::string& aliasMcEvtColl )
{
  m_etIsolMap[aliasMcEvtColl] = m_etIsolMap[originalMcEvtColl];
  return StatusCode::SUCCESS;
}
