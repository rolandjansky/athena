/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// TrackThroughDetectorBoundaries.cxx, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

#include "TrackThroughDetectorBoundaries.h"
#include "GaudiKernel/IIncidentSvc.h"

// EDM
#include "TrkTrack/Track.h"
#include "TrkTrack/TrackStateOnSurface.h"
#include "Particle/TrackParticle.h"

// Geometry
#include "TrkGeometry/TrackingVolume.h"
#include "TrkGeometry/TrackingGeometry.h"

// Tool and Service handles
#include "TrkDetDescrInterfaces/ITrackingVolumesSvc.h"
#include "TrkExInterfaces/IExtrapolator.h"
// #include "TrkFitterInterfaces/ITrackFitter.h"
#include "RecoToolInterfaces/IExtrapolateToCaloTool.h"


// constructor
Rec::TrackThroughDetectorBoundaries::TrackThroughDetectorBoundaries(const std::string& t, const std::string& name, const IInterface* p) :
  AthAlgTool(t,name,p),
  m_extrapolator("Trk::Extrapolator/AtlasExtrapolator"),
  m_trackToCalo("ExtrapolateToCaloTool/ExtrapolateToCaloTool"),
  m_incidentSvc("IncidentSvc",name),
  m_idEnclosingVolume(0),
  m_msEntranceVolume(0),
  m_haveMuonGeometry(true)
  //  m_fitter                (""),   // possibly add protected way of refitting when parameters absent

{
   declareInterface<ITrackThroughDetectorBoundaries>(this);
   declareProperty("Extrapolator", m_extrapolator);
   declareProperty("ExtrapolateToCaloTool", m_trackToCalo);
   declareProperty("DetFlagsMuonOn",m_haveMuonGeometry);

}

// destructor
Rec::TrackThroughDetectorBoundaries::~TrackThroughDetectorBoundaries()
{

}

//================ Initialisation =================================================
StatusCode Rec::TrackThroughDetectorBoundaries::initialize()
{
  StatusCode sc = AthAlgTool::initialize();

  // need to know which TrackingVolume we are in: indet/calo/spectrometer
  /*  if (m_trackingVolumesSvc.retrieve().isFailure()) {
    ATH_MSG_ERROR( "Failed to retrieve Svc " << m_trackingVolumesSvc );
    return StatusCode::FAILURE;
  } else {
    ATH_MSG_DEBUG( "Retrieved Svc " << m_trackingVolumesSvc );
    m_indetVolume           = 
      new Trk::Volume(m_trackingVolumesSvc->volume(ITrackingVolumesSvc::CalorimeterEntryLayer));
      }*/

  if(m_extrapolator.retrieve().isFailure()){
    ATH_MSG_ERROR ("Failed to retrieve IExtrapolator!");
    return StatusCode::FAILURE;
  }else{
    ATH_MSG_DEBUG ("Retrieved tool " << m_extrapolator);
  }

  // have to make calo layers an option for Calo-OFF jobs with segment tagging ON
  if (!m_trackToCalo.empty()) {
    if (m_trackToCalo.retrieve().isFailure()){
      ATH_MSG_ERROR("Failed to retrieve IExtrapolateToCaloTool!");
      return StatusCode::FAILURE;
    } else {
      ATH_MSG_DEBUG ("Retrieved tool " << m_trackToCalo );
    }
  }

  ATH_MSG_DEBUG (" initialize() successful in "<<name());
  return sc;
}

//================ Finalisation =================================================
StatusCode Rec::TrackThroughDetectorBoundaries::finalize()
{
  ATH_MSG_DEBUG ("finalize() successful in " << name());
  return StatusCode::SUCCESS;
}

//================ entry/exit pars =================================================
std::pair<const Trk::TrackParameters*, const Trk::TrackParameters*>
Rec::TrackThroughDetectorBoundaries::getIndetEntryAndExitParameters(const Trk::Track& track) const
{
  if (m_idEnclosingVolume == 0 ) {
    // argh, its "InnerDetector" in reco and "InDetEnclosingVolume" in fatras
    m_idEnclosingVolume = m_extrapolator->trackingGeometry()->trackingVolume("InDet::Containers::InnerDetector");
    if (m_idEnclosingVolume==0) {
      ATH_MSG_WARNING ("Could not get ID enclosing volume from tracking geometry.");
      return std::make_pair<const Trk::TrackParameters*, const Trk::TrackParameters*> (0, 0);
    }
  }
  const Trk::TrackParameters* firstPar = NULL;
  const Trk::TrackParameters* lastPar = NULL;
  DataVector<const Trk::TrackStateOnSurface>::const_iterator itTSoS 
    = track.trackStateOnSurfaces()->begin();
  for (   ; itTSoS!=track.trackStateOnSurfaces()->end(); ++itTSoS) {
    if ( (*itTSoS)->measurementOnTrack()==0 && 
	 !(*itTSoS)->type(Trk::TrackStateOnSurface::Perigee)) continue;
    const Trk::TrackParameters* parametersOnSurface = (*itTSoS)->trackParameters();
    if ( (parametersOnSurface!= 0) &&
	 (m_idEnclosingVolume->inside(parametersOnSurface->position())) ) {
      if (firstPar==NULL) firstPar = parametersOnSurface;
      lastPar = parametersOnSurface;
    }
  }
  if ( (firstPar == 0) || (lastPar == 0) ) {
    ATH_MSG_WARNING ("Track structure not understood, or no ID track. First par "<<
		     (firstPar ? "found" : "not found") << ", last par " <<
		     (lastPar  ? "found" : "not found") ) ;
  } else {
    ATH_MSG_DEBUG("Found parameters! First par: " << firstPar->position() <<
		  "  last Par: " << lastPar->position());
  }
  return std::make_pair(firstPar, lastPar);
}

//================ extrapolation through ATLAS ====================================
const Rec::ParticleExtrapolationVector* 
Rec::TrackThroughDetectorBoundaries::getParametersAcrossAtlas
( const Trk::Track& track,
  const Trk::ParticleHypothesis particleHypothesis,
  const Trk::PropDirection direction) const
{

  if (m_msEntranceVolume == 0 && m_haveMuonGeometry) {
    m_msEntranceVolume = 
      m_extrapolator->trackingGeometry()->trackingVolume("MuonSpectrometerEntrance");
    if (m_msEntranceVolume == 0) {
      ATH_MSG_WARNING ("could not retrieve MSEntrance TrackingVolume!");
      return NULL;
    }
  }

  DataVector< const Trk::TrackParameters >* caloParameterVector = NULL;
  const Trk::TrackParameters* idCaloParams = 0;

  if (!m_trackToCalo.empty()) {
    //Why is TrackToCalo returning a const vector? And why is it a DataVector? 
    caloParameterVector = const_cast<DataVector< const Trk::TrackParameters >* > (m_trackToCalo->getParametersInCalo(track,particleHypothesis,direction));
    if ( caloParameterVector==NULL || 
	 (*caloParameterVector)[IExtrapolateToCaloTool::Tile]==NULL) {
      ATH_MSG_DEBUG ("Could not reach tile calorimeter with id track candidate, giving up");
      delete caloParameterVector;
      return NULL;
    }
  }

  Rec::ParticleExtrapolationVector* parameterVector = 
    new Rec::ParticleExtrapolationVector();
  std::pair<const Trk::TrackParameters*, const Trk::TrackParameters*> firstAndLast =
    this->getIndetEntryAndExitParameters(track);
  if (firstAndLast.first != 0) {
    Rec::IdentifiedTrackParameters firstPar = 
      std::make_pair(firstAndLast.first->clone(),RecIntersect::InDetFirstHit);
    parameterVector->push_back(firstPar);
    ATH_MSG_VERBOSE ("Added first parameter.");
  }
  if (firstAndLast.second != 0) {
    Rec::IdentifiedTrackParameters lastPar = 
      std::make_pair(firstAndLast.second->clone(),RecIntersect::InDetLastHit);
    parameterVector->push_back(lastPar);
    ATH_MSG_VERBOSE ("Added last parameter.");
    idCaloParams = firstAndLast.second;
  }

  if (!m_trackToCalo.empty()) {
    const Trk::TrackParameters* presamplerPars = 0;
    caloParameterVector->swapElement( IExtrapolateToCaloTool::PreSampler, 0, presamplerPars );
    Rec::IdentifiedTrackParameters par0 = 
      std::make_pair(presamplerPars,RecIntersect::PreSampler);
    parameterVector->push_back(par0);
  
    const Trk::TrackParameters* stripsPars = 0;
    caloParameterVector->swapElement( IExtrapolateToCaloTool::Strips, NULL, stripsPars );
    Rec::IdentifiedTrackParameters par1 = 
      std::make_pair(stripsPars,RecIntersect::Strips);
    parameterVector->push_back(par1);

    const Trk::TrackParameters* middlePars = 0;
    caloParameterVector->swapElement( IExtrapolateToCaloTool::Middle, NULL, middlePars );
    Rec::IdentifiedTrackParameters par2 = 
      std::make_pair(middlePars,RecIntersect::Middle);
    parameterVector->push_back(par2);

    const Trk::TrackParameters* backPars = 0;
    caloParameterVector->swapElement( IExtrapolateToCaloTool::Back, NULL, backPars );
    Rec::IdentifiedTrackParameters par3 = 
      std::make_pair(backPars,RecIntersect::Back);
    parameterVector->push_back(par3);

    const Trk::TrackParameters* tilePars = 0;
    caloParameterVector->swapElement( IExtrapolateToCaloTool::Tile, NULL, tilePars );
    Rec::IdentifiedTrackParameters par4 = 
      std::make_pair(tilePars,RecIntersect::Tile);
    parameterVector->push_back(par4);

    idCaloParams = dynamic_cast<const Trk::TrackParameters*>(tilePars);
  }
  delete caloParameterVector; caloParameterVector=0; 


  const Trk::TrackParameters* msEntrancePars =
    (m_haveMuonGeometry && idCaloParams) ? 
    m_extrapolator->extrapolateToVolume(*idCaloParams,
					*m_msEntranceVolume,
					direction,
					particleHypothesis) :
    NULL;


  if (msEntrancePars!=NULL) {
    ATH_MSG_DEBUG("Successfully extrapolated muon candidate to MS entrance");
    Rec::IdentifiedTrackParameters par5 = 
      std::make_pair(msEntrancePars,RecIntersect::MSEntrance);
    parameterVector->push_back(par5);
  }

  ATH_MSG_DEBUG ("Got vector of parameters at layers/boundaries of size " << 
		 parameterVector->size());
  return parameterVector;
}

void Rec::TrackThroughDetectorBoundaries::handle(const Incident& inc) {

  if (inc.type() != "EndEvent") return;

  m_idEnclosingVolume = NULL;
  m_msEntranceVolume  = NULL;

}  

