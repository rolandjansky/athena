/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

// class header include
#include "EntryLayerToolMT.h"

// ISF includes
#include "ISF_Event/ISFParticle.h"

/** Constructor **/
ISF::EntryLayerToolMT::EntryLayerToolMT(const std::string& t, const std::string& n, const IInterface* p) :
  base_class(t,n,p),
  m_volumeName()
{
  // volumeNames for TrackRecords
  declareProperty( "CaloEntryVolumeString",
                   m_volumeName[ISF::fAtlasCaloEntry] = "IDET::IDET",
                   "VolumeName in TrackRecords in CaloEntryLayer");
  declareProperty( "MuonEntryVolumeString",
                   m_volumeName[ISF::fAtlasMuonEntry] = "CALO::CALO",
                   "VolumeName in TrackRecords in MuonEntryLayer");
  declareProperty( "MuonExitVolumeString",
                   m_volumeName[ISF::fAtlasMuonExit]  = "MUONQ02::MUONQ02",
                   "VolumeName in TrackRecords in MuonExitLayer");
}


/** Athena algtool's Hooks */
StatusCode  ISF::EntryLayerToolMT::initialize()
{
  ATH_MSG_INFO("initialize() ...");

  // retrieve the GeoIDSvc
  ATH_CHECK ( m_geoIDSvc.retrieve() );
  // store a quick-access pointer to the c++ class directly
  m_geoIDSvcQuick = &(*m_geoIDSvc);

  // retrieve the ParticleFilters
  ATH_CHECK ( m_particleFilterHandle.retrieve() );
  // store a quick-access pointer to the c++ classes directly
  m_numParticleFilters = m_particleFilterHandle.size();
  m_particleFilter = new ISF::IParticleFilter*[ m_numParticleFilters ];
  for ( size_t curFilter = 0; curFilter<m_numParticleFilters; curFilter++) {
    // convert ToolHandle to standard c++ class pointer
    m_particleFilter[curFilter] = &(*m_particleFilterHandle[curFilter]);
  }

  ATH_MSG_INFO("initialize() successful");
  return StatusCode::SUCCESS;
}


/** Check if given particle passes the EntryLayer filters */
bool ISF::EntryLayerToolMT::passesFilters( const ISFParticle& particle) {
  bool pass = true;
  for ( size_t curFilter=0; pass && (curFilter<m_numParticleFilters); curFilter++) {
    // check current filter
    pass = m_particleFilter[curFilter]->passFilter( particle);
  }

  return pass;
}


/** Identify the corresponding entry layer for the given particle (may return
    ISF::fUnsetEntryLayere if particle is not on an entry layer surface) */
ISF::EntryLayer ISF::EntryLayerToolMT::identifyEntryLayer( const ISFParticle& particle) {
  // the return value
  ISF::EntryLayer layerHit = ISF::fUnsetEntryLayer;

  const Amg::Vector3D &pos = particle.position();

  // check if particle on ID and/or Calo surface
  bool onIDSurface   = (m_geoIDSvcQuick->inside( pos, AtlasDetDescr::fAtlasID)   == ISF::fSurface);
  bool onCaloSurface = (m_geoIDSvcQuick->inside( pos, AtlasDetDescr::fAtlasCalo) == ISF::fSurface);

  // on CaloEntry layer ?
  if ( onIDSurface && onCaloSurface ) {
    layerHit = ISF::fAtlasCaloEntry;
  }

  // no surface hit yet -> test MS volume surface hit
  else {
    // check if particle on MS surface
    bool onMSSurface   = (m_geoIDSvcQuick->inside( pos, AtlasDetDescr::fAtlasMS) == ISF::fSurface);

    // on MuonEntry layer ?
    if (onCaloSurface && onMSSurface) {
      layerHit = ISF::fAtlasMuonEntry;
    }
    // on MuonExit layer ?
    else if (onMSSurface) {
      layerHit = ISF::fAtlasMuonExit;
    }
  }
  return layerHit;
}


/** Add the given particle to the corresponding Entry/Exit layer if applicable */
ISF::EntryLayer ISF::EntryLayerToolMT::registerParticle(const ISF::ISFParticle& particle, ISF::EntryLayer layerHit)
{
  // (1.) check whether the particle actually passes all the filters
  //      -> rather fast usually
  if ( !passesFilters(particle) ) {
    // return if not passed
    return ISF::fUnsetEntryLayer;
  }

  // (2.) check whether the particle lies on any entry surface
  //      -> this goes second because computation intensive routines
  //         are used for this
  if ( layerHit == ISF::fUnsetEntryLayer) {
    layerHit = identifyEntryLayer( particle);
  }

  // (3.) if particle is on a boundary surface
  //        -> add it to TrackRecordCollection
  if ( layerHit != ISF::fUnsetEntryLayer) {
    ATH_MSG_VERBOSE( "Particle >>" << particle << "<< hit boundary surface, "
                     "adding it to '" << (*m_collectionHolder.get())[layerHit]->Name() << "' TrackRecord collection");

    const Amg::Vector3D   &pos = particle.position();
    const Amg::Vector3D   &mom = particle.momentum();
    CLHEP::Hep3Vector      hepPos( pos.x(), pos.y(), pos.z() ); // not optimal, but required by TrackRecord
    CLHEP::Hep3Vector      hepMom( mom.x(), mom.y(), mom.z() ); // not optimal, but required by TrackRecord

    double mass            = particle.mass();
    double energy          = sqrt(mass*mass + mom.mag2());

    // Use barcode of generation zero particle from truth binding if possible (reproduces legacy AtlasG4 behaviour).
    // Use barcode assigend to ISFParticle only if no generation zero particle is present.
    auto                truthBinding = particle.getTruthBinding();
    auto generationZeroTruthParticle = truthBinding ? truthBinding->getGenerationZeroTruthParticle() : nullptr;
    Barcode::ParticleBarcode barcode = generationZeroTruthParticle ? HepMC::barcode(generationZeroTruthParticle)
                                                                   : particle.barcode();

    (*m_collectionHolder.get())[layerHit]->Emplace(particle.pdgCode(),
                                                  energy,
                                                  hepMom,
                                                  hepPos,
                                                  particle.timeStamp(),
                                                  barcode,
                                                  m_volumeName[layerHit] );
  }

  return layerHit;
}

/** Register the TrackRecordCollection pointer for a layer */
StatusCode ISF::EntryLayerToolMT::registerTrackRecordCollection(TrackRecordCollection* collection, EntryLayer layer)
{
  // Do we need to lock around this code???
  auto* collectionArray = m_collectionHolder.get();
  if (not collectionArray) {
    // need to create the array of TrackRecordCollections for this thread
    auto* temp = new std::array<TrackRecordCollection*, ISF::fNumAtlasEntryLayers>;
    m_collectionHolder.set(temp);
    collectionArray = m_collectionHolder.get();
  }
  (*collectionArray)[layer]=collection;
  return StatusCode::SUCCESS;
}
