/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// InputConverter.cxx, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

// class header include
#include "InputConverter.h"

#include <memory>

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
#include "ISF_Event/ISFParticleVector.h"
#include "ISF_Event/ParticleUserInformation.h"
// MCTruth includes
#include "MCTruth/PrimaryParticleInformation.h"
#include "MCTruth/AtlasG4EventUserInfo.h"
// McEventCollection
#include "GeneratorObjects/McEventCollection.h"
// Geant4 includes
#include "G4PrimaryParticle.hh"
#include "G4Event.hh"
#include "G4Geantino.hh"
#include "G4ChargedGeantino.hh"
#include "G4ParticleTable.hh"
#include "G4LorentzVector.hh"
#include "G4TransportationManager.hh"
// HepMC includes
#include "AtlasHepMC/GenParticle.h"
#include "AtlasHepMC/GenEvent.h"
#include "AtlasHepMC/GenVertex.h"
// CLHEP includes
#include "CLHEP/Geometry/Point3D.h"
#include "CLHEP/Geometry/Vector3D.h"
#include "CLHEP/Units/SystemOfUnits.h"
// HepPDT
#include "HepPDT/ParticleID.hh"
#include "HepPDT/DecayData.hh"
#include "HepPDT/ParticleDataTable.hh"


/** Constructor **/
ISF::InputConverter::InputConverter(const std::string& name, ISvcLocator* svc)
    : base_class(name, svc)
    , m_particlePropSvc("PartPropSvc",name)
    , m_particleDataTable(nullptr)
    , m_useGeneratedParticleMass(false)
    , m_genParticleFilters(this)
    , m_quasiStableParticlesIncluded(false)
    , m_barcodeSvc("", name)
    , m_barcodeGenerationIncrement(Barcode::fUndefinedBarcode)
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
  // the barcode service (used to compute Vertex Barcodes)
  declareProperty("BarcodeSvc",                 m_barcodeSvc           );
  declareProperty("QuasiStableParticlesIncluded", m_quasiStableParticlesIncluded);
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

  // retrieve BarcodeSvc
  ATH_CHECK(m_barcodeSvc.retrieve());
  m_barcodeGenerationIncrement = m_barcodeSvc->particleGenerationIncrement();
  if (m_barcodeGenerationIncrement == Barcode::fUndefinedBarcode) {
    ATH_MSG_FATAL( "'Barcode::fUndefinedBarcode' returned as 'BarcodeGenerationIncrement' by BarcodeService. Abort." );
    return StatusCode::FAILURE;
  }

  // setup PDT if requested (to get particle masses later on)
  if (!m_useGeneratedParticleMass) {
    ATH_CHECK(m_particlePropSvc.retrieve());
    m_particleDataTable = m_particlePropSvc->PDT();
    if (!m_particleDataTable) {
      ATH_MSG_FATAL( "Could not get ParticleDataTable from " << m_particlePropSvc << ". Abort" );
      return StatusCode::FAILURE;
    }
  }

  if (!m_genParticleFilters.empty()) {
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
                             EBC_EVCOLL kindOfCollection) const
{
  for ( const auto& eventPtr : inputGenEvents ) {
    // skip empty events
    if (eventPtr == nullptr) { continue; }

    ATH_MSG_DEBUG("Starting conversion of GenEvent with"
                  " signal_process_id=" << HepMC::signal_process_id(eventPtr) <<
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
      auto simParticlePtr = convertParticle(genPartPtr, kindOfCollection);
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

StatusCode ISF::InputConverter::convertHepMCToG4Event(McEventCollection& inputGenEvents,
                                                      G4Event*& outputG4Event,
                                                      EBC_EVCOLL kindOfCollection) const
{
  ISF::ISFParticleContainer simParticleList{}; // particles for ISF simulation
  ATH_CHECK(this->convert(inputGenEvents, simParticleList, kindOfCollection));
  //Convert from ISFParticleContainer to ConstISFParticleVector
  ISF::ConstISFParticleVector simParticleVector{std::make_move_iterator(std::begin(simParticleList)),
                                                std::make_move_iterator(std::end(simParticleList))};
  outputG4Event = this->ISF_to_G4Event(simParticleVector, inputGenEvents.back());
  return StatusCode::SUCCESS;
}


/** get all generator particles which pass filters */
std::vector<HepMC::GenParticlePtr>
ISF::InputConverter::getSelectedParticles(const HepMC::GenEvent& evnt, bool legacyOrdering) const {
  auto allGenPartBegin = evnt.particles_begin();
  auto allGenPartEnd = evnt.particles_end();

  // reserve destination container with maximum size, i.e. number of particles in input event
  std::vector<HepMC::GenParticlePtr> passedGenParticles{};
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
                   [this](HepMC::GenParticlePtr p){return this->passesFilters(*p);});
    }
  }
  else {
    std::copy_if(allGenPartBegin,
                 allGenPartEnd,
                 std::back_inserter(passedGenParticles),
                 [this](HepMC::GenParticlePtr p){return this->passesFilters(*p);});
  }

  passedGenParticles.shrink_to_fit();

  return passedGenParticles;
}


/** get all generator particles which pass filters */
ISF::ISFParticle*
ISF::InputConverter::convertParticle(HepMC::GenParticlePtr genPartPtr, EBC_EVCOLL kindOfCollection) const {
  if (!genPartPtr) { return nullptr; }
  auto& genPart = *genPartPtr;

  // @FIXME: set the bunch-crossing identifier for pile-up dynamically
  // rather than a constant '1' (e.g. could use GenEvent index for that?)
  const int bcid = (kindOfCollection==EBC_MAINEVCOLL) ? 0 : 1;

  HepMC::GenVertexPtr  pVertex = genPart.production_vertex();
  if (!pVertex) {
    ATH_MSG_ERROR("Unable to convert following generator particle due to missing "
                  << "production vertex: " << genPart);
    return nullptr;
  }


  const Amg::Vector3D pos(pVertex->position().x(), pVertex->position().y(), pVertex->position().z());
  const auto& pMomentum(genPart.momentum());
  const Amg::Vector3D mom(pMomentum.px(), pMomentum.py(), pMomentum.pz());
  const double pMass = this->getParticleMass(genPart);

  double e=pMomentum.e();
  if(e>1) { //only test for >1 MeV in momentum
    double px=pMomentum.px();
    double py=pMomentum.py();
    double pz=pMomentum.pz();
    double teste=sqrt(px*px + py*py + pz*pz + pMass*pMass);
    if(std::abs(e-teste)/e>0.01) {
      ATH_MSG_WARNING("Difference in energy for: " << genPart<<" Morg="<<pMomentum.m()<<" Mmod="<<pMass<<" Eorg="<<e<<" Emod="<<teste);
    }
  }  

  const int pPdgId = genPart.pdg_id();
  const double charge = HepPDT::ParticleID(pPdgId).charge();
  const double pTime = pVertex->position().t() / Gaudi::Units::c_light;
  /// particle origin (TODO: add proper GeoID, collision/cosmics)
  DetRegionSvcIDPair origin(AtlasDetDescr::fUndefinedAtlasRegion, ISF::fEventGeneratorSimID);
  const auto pBarcode = HepMC::barcode(genPart);
  auto tBinding = std::make_unique<ISF::TruthBinding>(genPartPtr);

  auto *parentEvent = genPart.parent_event();
  if(!parentEvent) {
    ATH_MSG_ERROR("Cannot convert a GenParticle without a parent GenEvent into an ISFParticle!!!");
    return nullptr;
  }
  auto hmpl = std::make_unique<HepMcParticleLink>(&genPart, parentEvent->event_number(), kindOfCollection);
  auto sParticle = std::make_unique<ISF::ISFParticle>( std::move(pos),
                                                       std::move(mom),
                                                       pMass,
                                                       charge,
                                                       pPdgId,
                                                       pTime,
                                                       origin,
                                                       bcid,
                                                       pBarcode,
                                                       tBinding.release(),
                                                       hmpl.release() );
  return sParticle.release();
}


/** get right GenParticle mass */
double
ISF::InputConverter::getParticleMass(const HepMC::GenParticle &part) const
{
  // default value: generated particle mass
  double mass = part.generated_mass();
  ATH_MSG_VERBOSE("part.generated_mass, mass="<<mass);

  // 1. use PDT mass?
  if ( !m_useGeneratedParticleMass ) {
    const int absPDG = std::abs(part.pdg_id());
    HepPDT::ParticleData const *pData = (m_particleDataTable)
      ? m_particleDataTable->particle(absPDG)
      : nullptr;
    if (pData) {
      mass = pData->mass();
      ATH_MSG_VERBOSE("using pData mass, mass="<<mass);
    }
    else {
      ATH_MSG_WARNING( "Unable to find mass of particle with PDG ID '" << absPDG << "' in ParticleDataTable. Will set mass to generated_mass: " << mass);
    }
  }
  return mass;
}


/** check if the given particle passes all filters */
bool
ISF::InputConverter::passesFilters(const HepMC::GenParticle& part) const
{
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


//________________________________________________________________________
G4Event* ISF::InputConverter::ISF_to_G4Event(const ISF::ConstISFParticleVector& ispVector, HepMC::GenEvent *genEvent, bool useHepMC) const
{
  const int eventID(1);
  G4Event *g4evt = new G4Event(eventID);

  // retrieve world solid (volume)
  const G4VSolid *worldSolid = G4TransportationManager::GetTransportationManager()->GetNavigatorForTracking()->GetWorldVolume()->GetLogicalVolume()->GetSolid();

  int n_pp=0;
  for ( const ISF::ISFParticle *ispPtr: ispVector ) {
    const ISF::ISFParticle &isp = *ispPtr;
    if ( !isInsideG4WorldVolume(isp, worldSolid) ) {
        ATH_MSG_WARNING("Unable to convert ISFParticle to G4PrimaryParticle!");
        ATH_MSG_WARNING(" ISFParticle: " << isp );
        if(worldSolid) {
          ATH_MSG_WARNING(" is outside Geant4 world volume: ");
          worldSolid->DumpInfo();
          G4cout << std::flush;
        }
        else {
          ATH_MSG_WARNING(" is outside Geant4 world volume.");
	}
        continue;
    }
    this->addG4PrimaryVertex(g4evt,isp,useHepMC);
    n_pp++;
  }

  AtlasG4EventUserInfo *atlasG4EvtUserInfo=new AtlasG4EventUserInfo();
  atlasG4EvtUserInfo->SetNrOfPrimaryParticles(n_pp);
  atlasG4EvtUserInfo->SetNrOfPrimaryVertices(n_pp); // special case for ISF batches of particles
  atlasG4EvtUserInfo->SetHepMCEvent(genEvent);
  g4evt->SetUserInformation(atlasG4EvtUserInfo);

  return g4evt;
}

//________________________________________________________________________
const G4ParticleDefinition* ISF::InputConverter::getG4ParticleDefinition(int pdgcode) const
{
  /// Special cases for Geantinos
  if (pdgcode==998) {
    return G4ChargedGeantino::Definition();
  }
  if (pdgcode==999) {
    return G4Geantino::GeantinoDefinition();
  }
  /// Standard particles
  G4ParticleTable *ptable = G4ParticleTable::GetParticleTable();
  if(ptable) {
    return ptable->FindParticle(pdgcode);
  }
  ATH_MSG_ERROR("getG4ParticleDefinition - Failed to retrieve G4ParticleTable!");
  return nullptr;
}

//________________________________________________________________________
G4PrimaryParticle* ISF::InputConverter::getG4PrimaryParticle(const HepMC::GenParticle& genpart) const
{
  ATH_MSG_VERBOSE("Creating G4PrimaryParticle from GenParticle.");

  const G4ParticleDefinition *particleDefinition = this->getG4ParticleDefinition(genpart.pdg_id());

  if(particleDefinition==nullptr) {
    ATH_MSG_ERROR("ISF_to_G4Event particle conversion failed. ISF_Particle PDG code = " << genpart.pdg_id() <<
                  "\n This usually indicates a problem with the evgen step.\n" <<
                  "Please report this to the Generators group, mentioning the release and generator used for evgen and the PDG code above." );
    return nullptr;
  }

  // create new primaries and set them to the vertex
  //  G4double mass =  particleDefinition->GetPDGMass();
  auto &genpartMomentum = genpart.momentum();
  G4double px = genpartMomentum.x();
  G4double py = genpartMomentum.y();
  G4double pz = genpartMomentum.z();

  std::unique_ptr<G4PrimaryParticle> g4particle = std::make_unique<G4PrimaryParticle>(particleDefinition,px,py,pz);

  if (genpart.end_vertex()) {
    // Set the lifetime appropriately - this is slow but rigorous, and we
    //  don't want to end up with something like vertex time that we have
    //  to validate for every generator on earth...
    const auto& prodVtx = genpart.production_vertex()->position();
    const auto& endVtx = genpart.end_vertex()->position();
    const G4LorentzVector lv0 ( prodVtx.x(), prodVtx.y(), prodVtx.z(), prodVtx.t() );
    const G4LorentzVector lv1 ( endVtx.x(), endVtx.y(), endVtx.z(), endVtx.t() );
    g4particle->SetProperTime( (lv1-lv0).mag()/Gaudi::Units::c_light );

    if(m_quasiStableParticlesIncluded) {
      ATH_MSG_VERBOSE( "Detected primary particle with end vertex." );
      ATH_MSG_VERBOSE( "Will add the primary particle set on." );
      ATH_MSG_VERBOSE( "Primary Particle: " << genpart );
      ATH_MSG_VERBOSE( "Number of daughters of "<<genpart.barcode()<<": " << genpart.end_vertex()->particles_out_size() );
    }
    else {
      ATH_MSG_WARNING( "Detected primary particle with end vertex." );
      ATH_MSG_WARNING( "Will add the primary particle set on." );
      ATH_MSG_WARNING( "Primary Particle: " << genpart );
      ATH_MSG_WARNING( "Number of daughters of "<<genpart.barcode()<<": " << genpart.end_vertex()->particles_out_size() );
    }
    // Add all necessary daughter particles
    for ( auto daughterIter=genpart.end_vertex()->particles_out_const_begin();
          daughterIter!=genpart.end_vertex()->particles_out_const_end(); ++daughterIter ) {
      if(m_quasiStableParticlesIncluded) {
        ATH_MSG_VERBOSE ( "Attempting to add daughter particle of "<<HepMC::barcode(genpart)<<": " << **daughterIter );
      }
      else {
        ATH_MSG_WARNING ( "Attempting to add daughter particle of "<<HepMC::barcode(genpart)<<": " << **daughterIter );
      }
      G4PrimaryParticle *daughterG4Particle = this->getG4PrimaryParticle( **daughterIter );
      if(!daughterG4Particle) {
        ATH_MSG_ERROR("Bailing out of loop over daughters of particle with barcode: "<<HepMC::barcode(genpart) <<
                      " due to errors - will not return G4Particle.");
        return nullptr;
      }
      g4particle->SetDaughter( daughterG4Particle );
    }
  }

  // Set the user information for this primary to point to the HepMcParticleLink...
  PrimaryParticleInformation* ppi = new PrimaryParticleInformation(&genpart);
  ppi->SetParticle(&genpart);
  ppi->SetRegenerationNr(0);
  g4particle->SetUserInformation(ppi);
  ATH_MSG_VERBOSE("Making primary down the line with barcode " << ppi->GetParticleBarcode());

  return g4particle.release();
}


//________________________________________________________________________
G4PrimaryParticle* ISF::InputConverter::getG4PrimaryParticle(const ISF::ISFParticle& isp, bool useHepMC) const
{
  ATH_MSG_VERBOSE("Creating G4PrimaryParticle from ISFParticle.");

  const auto* truthBinding = isp.getTruthBinding();
  if (!truthBinding) {
      G4ExceptionDescription description;
      description << G4String("getG4PrimaryParticle: ") + "No ISF::TruthBinding associated with ISParticle (" << isp <<")";
      G4Exception("iGeant4::TransportTool", "NoISFTruthBinding", FatalException, description);
      return nullptr; //The G4Exception call above should abort the job, but Coverity does not seem to pick this up.
  }
  HepMC::GenParticlePtr        genpart = truthBinding->getTruthParticle();
  HepMC::GenParticlePtr primaryGenpart = truthBinding->getPrimaryTruthParticle();

  const G4ParticleDefinition *particleDefinition = this->getG4ParticleDefinition(isp.pdgCode());

  if(particleDefinition==nullptr) {
    ATH_MSG_ERROR("ISF_to_G4Event particle conversion failed. ISF_Particle PDG code = " << isp.pdgCode() <<
                  "\n This usually indicates a problem with the evgen step.\n" <<
                  "Please report this to the Generators group, mentioning the release and generator used for evgen and the PDG code above." );
    return nullptr;
  }

  // create new primaries and set them to the vertex
  //  G4double mass =  particleDefinition->GetPDGMass();
  G4double px(0.0);
  G4double py(0.0);
  G4double pz(0.0);
  if(useHepMC && genpart) {
    auto &genpartMomentum = genpart->momentum();
    px = genpartMomentum.x();
    py = genpartMomentum.y();
    pz = genpartMomentum.z();
  }
  else {
    auto &ispMomentum = isp.momentum();
    px = ispMomentum.x();
    py = ispMomentum.y();
    pz = ispMomentum.z();
  }

  std::unique_ptr<G4PrimaryParticle> g4particle = std::make_unique<G4PrimaryParticle>(particleDefinition,px,py,pz);
  // UserInformation
  std::unique_ptr<PrimaryParticleInformation> ppi = std::make_unique<PrimaryParticleInformation>(primaryGenpart,&isp);

  /// In the case that particles are being passed back to Geant4 then
  /// we may have particles which have already interacted, so we
  /// should set the regeneration number accordingly.
  Barcode::ParticleBarcode barcode = isp.barcode();
  const int regenerationNr = (barcode - barcode%m_barcodeGenerationIncrement)/m_barcodeGenerationIncrement;
  ppi->SetRegenerationNr(regenerationNr);

  if ( genpart ) {
    if (genpart->end_vertex()) {

      /// Set the lifetime appropriately - this is slow but rigorous,
      /// and we don't want to end up with something like vertex time
      /// that we have to validate for every generator on earth...
      const auto& prodVtx = genpart->production_vertex()->position();
      const auto& endVtx = genpart->end_vertex()->position();
      const G4LorentzVector lv0( prodVtx.x(), prodVtx.y(), prodVtx.z(), prodVtx.t() );
      const G4LorentzVector lv1( endVtx.x(), endVtx.y(), endVtx.z(), endVtx.t() );
      g4particle->SetProperTime( (lv1-lv0).mag()/Gaudi::Units::c_light );

      if(m_quasiStableParticlesIncluded) {
        ATH_MSG_VERBOSE( "Detected primary particle with end vertex." );
        ATH_MSG_VERBOSE( "Will add the primary particle set on." );
        ATH_MSG_VERBOSE( "ISF Particle: " << isp );
        ATH_MSG_VERBOSE( "Primary Particle: " << *genpart );
        ATH_MSG_VERBOSE( "Number of daughters of "<<genpart->barcode()<<": " << genpart->end_vertex()->particles_out_size() );
      }
      else {
        ATH_MSG_WARNING( "Detected primary particle with end vertex. This should only be the case if" );
        ATH_MSG_WARNING( "you are running with quasi-stable particle simulation enabled.  This is not" );
        ATH_MSG_WARNING( "yet validated - you'd better know what you're doing.  Will add the primary" );
        ATH_MSG_WARNING( "particle set on." );
        ATH_MSG_WARNING( "ISF Particle: " << isp );
        ATH_MSG_WARNING( "Primary Particle: " << *genpart );
        ATH_MSG_WARNING( "Number of daughters of "<<genpart->barcode()<<": " << genpart->end_vertex()->particles_out_size() );
      }
      // Add all necessary daughter particles
      for ( auto daughterIter=genpart->end_vertex()->particles_out_const_begin();
            daughterIter!=genpart->end_vertex()->particles_out_const_end(); ++daughterIter ) {
        if(m_quasiStableParticlesIncluded) {
          ATH_MSG_VERBOSE ( "Attempting to add daughter particle of "<<HepMC::barcode(genpart)<<": " << **daughterIter );
        }
        else {
          ATH_MSG_WARNING ( "Attempting to add daughter particle of "<<HepMC::barcode(genpart)<<": " << **daughterIter );
        }
        G4PrimaryParticle *daughterG4Particle = this->getG4PrimaryParticle( **daughterIter );
        if(!daughterG4Particle) {
          ATH_MSG_ERROR("Bailing out of loop over daughters of particle with barcode: "<<HepMC::barcode(genpart) <<
                        " due to errors - will not return G4Particle.");
          return nullptr;
        }
        g4particle->SetDaughter( daughterG4Particle );
      }
    } // particle had an end vertex
    
    double px,py,pz;
    const double pmass = g4particle->GetMass();
    CLHEP::Hep3Vector gpv = g4particle->GetMomentum();
    double g4px=g4particle->GetMomentum().x();
    double g4py=g4particle->GetMomentum().y();
    double g4pz=g4particle->GetMomentum().z();
    if(useHepMC) {
      //Code adapted from TruthHepMCEventConverter::TransformHepMCParticle
      px=g4px;
      py=g4py;
      pz=g4pz;
    } else {
      //Take mass from g4particle, put keep momentum as in genpart
      px=genpart->momentum().px();
      py=genpart->momentum().py();
      pz=genpart->momentum().pz();
      //Now a dirty hack to keep backward compatibility in the truth:
      //When running AtlasG4 or FullG4 between 21.0.41 and 21.0.111, the genpart 3-momentum and mass was reset to the values from the g4particle 
      //together with the mass of the g4particle after the 1st initialization of the g4particle from the genevent. This is done for a consistent mass
      //value in the truth record compared to the used g4 mass. Since g4particles don't store the 3-momentum directly, but rather a
      //unit direction vector, the mass and the kinetic energy, this reduces the numeric accuracy.
      //For backward compatibility, if all 3-momentum components agree to the g4particle momentum within 1 keV, we keep
      //this old method. This comparison is needed, since in ISF this code could be rerun after the ID or CALO simulation, where
      //real energy was lost in previous detectors and hence genpart should NOT be changed to some g4particle values!
      //TODO: find a way to implement this in a backward compatible way in ISF::InputConverter::convertParticle(HepMC::GenParticlePtr genPartPtr, int bcid)
      if(std::abs(px-g4px)<CLHEP::keV && std::abs(py-g4py)<CLHEP::keV && std::abs(pz-g4pz)<CLHEP::keV) {
        px=g4px;
        py=g4py;
        pz=g4pz;
      }
    }
    const double mag2=px*px + py*py + pz*pz;
    const double pe = std::sqrt(mag2 + pmass*pmass);  // this does only change for boosts, etc.
    
    double originalEnergy=genpart->momentum().e();
    if(originalEnergy>0.01) { //only test for >1 MeV in momentum
      if((originalEnergy-pe)/originalEnergy>0.01) {
        double genpx=genpart->momentum().px();
        double genpy=genpart->momentum().py();
        double genpz=genpart->momentum().pz();
        double genp=sqrt(genpx*genpx + genpy*genpy + genpz*genpz);
        ATH_MSG_WARNING("Truth change in energy for: " << genpart<<" Morg="<<genpart->momentum().m()<<" Mmod="<<pmass<<" Eorg="<<originalEnergy<<" Emod="<<pe<<" porg="<<genp<<" pmod="<<gpv.mag());
      }
    }  

    genpart->set_momentum(CLHEP::HepLorentzVector(px,py,pz,pe));
  } // Truth was detected

  ATH_MSG_VERBOSE("PrimaryParticleInformation:");
  ATH_MSG_VERBOSE("     GetParticleBarcode = " << ppi->GetParticleBarcode());
  ATH_MSG_VERBOSE("     GetRegenerationNr = " << ppi->GetRegenerationNr());
  ATH_MSG_VERBOSE("     GetHepMCParticle = " << ppi->GetHepMCParticle());
  ATH_MSG_VERBOSE("     GetISFParticle = " << ppi->GetISFParticle());
  g4particle->SetUserInformation(ppi.release());

  return g4particle.release();
}

//________________________________________________________________________
void ISF::InputConverter::addG4PrimaryVertex(G4Event* g4evt, const ISF::ISFParticle& isp, bool useHepMC) const
{
  /*
    see conversion from PrimaryParticleInformation to TrackInformation in
    http://acode-browser.usatlas.bnl.gov/lxr/source/atlas/Simulation/G4Atlas/G4AtlasAlg/src/AthenaStackingAction.cxx#0044

    need to check with
    http://acode-browser.usatlas.bnl.gov/lxr/source/atlas/Simulation/G4Atlas/G4AtlasAlg/src/TruthHepMCEventConverter.cxx#0151

    that we don't miss something
  */

  G4PrimaryParticle *g4particle = this->getG4PrimaryParticle( isp, useHepMC );
  if (!g4particle) {
    ATH_MSG_ERROR("Failed to create G4PrimaryParticle for ISParticle (" << isp <<")");
    return;
  }// Already printed a warning

  // create a new vertex
  G4PrimaryVertex *g4vertex = new G4PrimaryVertex(isp.position().x(),
                                                  isp.position().y(),
                                                  isp.position().z(),
                                                  isp.timeStamp());
  g4vertex->SetPrimary( g4particle );
  ATH_MSG_VERBOSE("Print G4PrimaryVertex: ");
  if (msgLevel(MSG::VERBOSE)) { g4vertex->Print(); }
  g4evt->AddPrimaryVertex( g4vertex );
  return;
}

//________________________________________________________________________
bool ISF::InputConverter::isInsideG4WorldVolume(const ISF::ISFParticle& isp, const G4VSolid* worldSolid) const
{

  const Amg::Vector3D &pos = isp.position();
  const G4ThreeVector g4Pos( pos.x(), pos.y(), pos.z() );
  EInside insideStatus = worldSolid->Inside( g4Pos );

  bool insideWorld = insideStatus != kOutside;
  return insideWorld;
}
