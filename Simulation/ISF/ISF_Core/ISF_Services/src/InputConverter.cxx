/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// InputConverter.cxx, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

// class header include
#include "InputConverter.h"

// framework
#include "GaudiKernel/IPartPropSvc.h"
#include "GaudiKernel/PhysicalConstants.h"

// ISF_HepMC include
#include "ISF_Event/TruthBinding.h"
#include "ISF_HepMC_Interfaces/IGenParticleFilter.h"
// barcode event and interfaces for bc creation
#include "BarcodeEvent/Barcode.h"
#include "BarcodeInterfaces/IBarcodeSvc.h"
// ISF_Event include
#include "ISF_Event/ISFParticle.h"
#include "ISF_Event/ISFParticleContainer.h"
#include "ISF_Event/ParticleUserInformation.h"
// HepMC includes
#include "HepMC/GenParticle.h"
#include "HepMC/GenEvent.h"
// McEventCollection
#include "GeneratorObjects/McEventCollection.h"
// CLHEP includes
#include "CLHEP/Geometry/Point3D.h"
#include "CLHEP/Geometry/Vector3D.h"
// HepPDT
#include "HepPDT/ParticleID.hh"
#include "HepPDT/DecayData.hh"
#include "HepPDT/ParticleDataTable.hh"


/** Constructor **/
ISF::InputConverter::InputConverter(const std::string& name, ISvcLocator* svc)
    : base_class(name,svc)
    , m_particlePropSvc("PartPropSvc",name)
    , m_particleDataTable(nullptr)
    , m_useGeneratedParticleMass(false)
    , m_genParticleFilters(this)
{
  // particle mass from particle data table?
  declareProperty("UseGeneratedParticleMass",
                  m_useGeneratedParticleMass,
                  "Use particle mass assigned to GenParticle.");
  // particle filters
  declareProperty("GenParticleFilters",
                  m_genParticleFilters,
                  "Tools for filtering out GenParticles.");
  // the particle property service
  declareProperty("ParticlePropertyService",
                  m_particlePropSvc,
                  "ParticlePropertyService to retrieve the PDT.");
}


/** Destructor **/
ISF::InputConverter::~InputConverter()
{
}


// Athena algtool's Hooks
StatusCode
ISF::InputConverter::initialize()
{
  ATH_MSG_VERBOSE("initialize() begin");

  // setup PDT if requested (to get particle masses later on)
  if (!m_useGeneratedParticleMass) {
    ATH_CHECK( m_particlePropSvc.retrieve() );
    m_particleDataTable = m_particlePropSvc->PDT();
    if (!m_particleDataTable) {
      ATH_MSG_FATAL( "Could not get ParticleDataTable from " << m_particlePropSvc << ". Abort" );
      return StatusCode::FAILURE;
    }
  }

  if (!m_genParticleFilters.empty()){
    ATH_CHECK(m_genParticleFilters.retrieve());
  }

  ATH_MSG_VERBOSE("initialize() successful");
  return StatusCode::SUCCESS;
}


/** Athena algtool Hook */
StatusCode
ISF::InputConverter::finalize()
{
  ATH_MSG_DEBUG("Finalizing ...");
  return StatusCode::SUCCESS;
}


/** Convert selected particles from the given McEventCollection into ISFParticles
    and push them into the given ISFParticleContainer */
StatusCode
ISF::InputConverter::convert(const McEventCollection& inputGenEvents,
                             ISF::ISFParticleContainer& simParticles,
                             bool isPileup) const
{
  for ( const auto& eventPtr : inputGenEvents ) {
    // skip empty events
    if (eventPtr == nullptr) continue;

    // @FIXME: set the bunch-crossing identifier for pileup dynamically
    // rather than a constant '1' (e.g. could use GenEvent index for that?)
    int bcid = isPileup ? 1 : 0;

    ATH_MSG_DEBUG("Starting conversion of GenEvent with"
                  " signal_process_id=" << eventPtr->signal_process_id() <<
                  " and event_number=" << eventPtr->event_number() );

    // new collection containing all gen particles that passed filters
    bool legacyOrdering = true; // FIXME: this is only to keep the same order of particles
                                //        as the prior 'StackFiller' implementation
                                // TODO:  remove this functionality from the
                                //        getSelectedParticles function once
                                //        happy with the results
    const auto passedGenParticles = getSelectedParticles(*eventPtr, legacyOrdering);

    for ( auto& genPartPtr : passedGenParticles ) {
      ATH_MSG_VERBOSE("Picking up following GenParticle for conversion to ISFParticle: " <<  *genPartPtr);
      auto simParticlePtr = convertParticle(genPartPtr, bcid);
      if (!simParticlePtr) {
        ATH_MSG_ERROR("Error while trying to convert input generator particles. Aborting.");
        return StatusCode::FAILURE;
      }
      // add to collection that will be returned
      simParticles.push_back(simParticlePtr);

    } // loop over passed gen particles

  } // loop over gen events

  ATH_MSG_DEBUG( "Created initial simulation particle collection with size " << simParticles.size() );

  return StatusCode::SUCCESS;
}


/** get all generator particles which pass filters */
std::vector<HepMC::GenParticle*>
ISF::InputConverter::getSelectedParticles(const HepMC::GenEvent& evnt, bool legacyOrdering) const {
  auto allGenPartBegin = evnt.particles_begin();
  auto allGenPartEnd = evnt.particles_end();

  // reserve destination container with maximum size, i.e. number of particles in input event
  std::vector<HepMC::GenParticle*> passedGenParticles{};
  size_t maxParticles = std::distance(allGenPartBegin, allGenPartEnd);
  passedGenParticles.reserve(maxParticles);

  if (legacyOrdering) {
    // FIXME: remove this block and the 'legacyOrdering' flag
    //        once we don't need the legacy order any longer
    auto vtxIt = evnt.vertices_begin();
    auto vtxItEnd = evnt.vertices_end();
    for ( ; vtxIt != vtxItEnd; ++vtxIt ) {
      const auto vtxPtr = *vtxIt;
      std::copy_if(vtxPtr->particles_begin(HepMC::children),
                   vtxPtr->particles_end(HepMC::children),
                   std::back_inserter(passedGenParticles),
                   [this](HepMC::GenParticle* p){return this->passesFilters(*p);});
    }

  } else {
    std::copy_if(allGenPartBegin,
                 allGenPartEnd,
                 std::back_inserter(passedGenParticles),
                 [this](HepMC::GenParticle* p){return this->passesFilters(*p);});
  }

  passedGenParticles.shrink_to_fit();

  return passedGenParticles;
}


/** get all generator particles which pass filters */
ISF::ISFParticle*
ISF::InputConverter::convertParticle(HepMC::GenParticle* genPartPtr, int bcid) const {

  if (!genPartPtr)
      return nullptr;

  auto& genPart = *genPartPtr;

  // -> particle origin (TODO: add proper GeoID, collision/cosmics)
  DetRegionSvcIDPair origin(AtlasDetDescr::fUndefinedAtlasRegion, ISF::fEventGeneratorSimID);

  // -> truth binding
  ISF::TruthBinding* tBinding = new ISF::TruthBinding(genPartPtr);

  // 4-vectors: position, momentum
  HepMC::GenVertex* pVertex = genPart.production_vertex();
  if (!pVertex) {
    ATH_MSG_ERROR("Unable to convert following generator particle due to missing "
                  << "production vertex: " << genPart);
    return nullptr;
  }
  const auto& pVertexPos(pVertex->point3d());
  const Amg::Vector3D pos(pVertexPos.x(), pVertexPos.y(), pVertexPos.z());
  const auto& pMomentum(genPart.momentum());
  const Amg::Vector3D mom(pMomentum.px(), pMomentum.py(), pMomentum.pz());

  // get the pdg_id, mass & time, barcode
  int pPdgId = genPart.pdg_id();
  double pMass = getParticleMass(genPart);
  double pTime = pVertex->position().t() / Gaudi::Units::c_light;
  double charge = HepPDT::ParticleID(pPdgId).charge();
  auto pBarcode = genPart.barcode();

  auto sParticle = new ISF::ISFParticle( std::move(pos),
                                         std::move(mom),
                                         pMass,
                                         charge,
                                         pPdgId,
                                         pTime,
                                         origin,
                                         bcid,
                                         pBarcode,
                                         tBinding );
  return sParticle;
}


/** get right GenParticle mass */
double
ISF::InputConverter::getParticleMass(const HepMC::GenParticle &part) const {
  // default value: generated particle mass
  double mass = part.generated_mass();
  ATH_MSG_VERBOSE("part.generated_mass, mass="<<mass);

  // 1. use PDT mass?
  if ( !m_useGeneratedParticleMass) {
    int absPDG = abs(part.pdg_id());
    HepPDT::ParticleData const *pData = (m_particleDataTable)
      ? m_particleDataTable->particle(absPDG)
      : nullptr ;
    if (pData) {
      mass = pData->mass();
      ATH_MSG_VERBOSE("using pData mass, mass="<<mass);
    }
    else
      ATH_MSG_WARNING( "Unable to find mass of particle with PDG ID '" << absPDG << "' in ParticleDataTable. Will set mass to generated_mass: " << mass);
  }
  return mass;
}


/** check if the given particle passes all filters */
bool
ISF::InputConverter::passesFilters(const HepMC::GenParticle& part) const {
  // TODO: implement this as a std::find_if with a lambda function
  for ( const auto& filter : m_genParticleFilters ) {
    // determine if the particle passes current filter
    bool passFilter = filter->pass(part);
    ATH_MSG_VERBOSE("GenParticleFilter '" << filter.typeAndName() << "' returned: "
                    << (passFilter ? "true, will keep particle."
                        : "false, will remove particle."));
    const auto& momentum = part.momentum();
    ATH_MSG_VERBOSE("Particle: ("
                    <<momentum.px()<<", "
                    <<momentum.py()<<", "
                    <<momentum.pz()<<"), pdgCode: "
                    <<part.pdg_id() );

    if (!passFilter) {
      return false;
    }
  }

  return true;
}
