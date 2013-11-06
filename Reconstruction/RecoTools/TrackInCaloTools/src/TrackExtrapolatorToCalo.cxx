/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/IToolSvc.h"

#include "ITrackInCaloTools/ITrackInCaloTools.h" // for CaloLayer
#include "TrackInCaloTools/TrackExtrapolatorToCalo.h"
#include "ITrackInCaloTools/ITrackDirectionTool.h"

#include "TrkTrack/Track.h"
#include "Particle/TrackParticle.h"
#include "muonEvent/Muon.h"

#include "CaloDetDescr/CaloDepthTool.h"
#include "CaloTrackingGeometry/ICaloSurfaceBuilder.h"

#include "CaloUtils/CaloCellDetPos.h"
#include "CaloDetDescr/CaloDetDescrManager.h"
#include "CaloDetDescr/CaloDetDescriptor.h"
#include "TileDetDescr/TileDetDescrManager.h"
#include "TileDetDescr/TileDetDescriptor.h"

/////////////////////////////////////////////////////////
TrackExtrapolatorToCalo::TrackExtrapolatorToCalo (const std::string&type, const std::string&name, const IInterface*parent)
  :AthAlgTool(type, name, parent),
  m_extrapolator("Trk::Extrapolator/AtlasExtrapolator"),
  m_particleHypothesisString("muon")
{
  declareInterface<ITrackExtrapolatorToCalo>(this);
  declareProperty ("ExtrapolatorHandle", m_extrapolator );
  declareProperty ("Cosmics", m_cosmics=false);
  declareProperty ("ParticleHypothesis", m_particleHypothesisString);

  setParticleHypothesis(m_particleHypothesisString);
  m_propDirection = Trk::alongMomentum;
  m_currentTrackParameters = 0;
  m_currentTrkTrack = 0;
  m_currentI4Mom = 0;
  m_caloCellDetPos = new CaloCellDetPos();
}
// ------------------------------------------------------------------ 
TrackExtrapolatorToCalo::~TrackExtrapolatorToCalo() 
{
  delete m_caloCellDetPos;
}
// ------------------------------------------------------------------ 
StatusCode TrackExtrapolatorToCalo::initialize()
{
  StatusCode sc = StatusCode::SUCCESS;
  ATH_MSG_DEBUG( "Initializing..." );
  
  //Get the extrapolator
  if(m_extrapolator.retrieve().isFailure()){
    ATH_MSG_FATAL( "Could not find Tool " << m_extrapolator.typeAndName() << ". Exiting." );
    return StatusCode::FAILURE;
  }
  else
    ATH_MSG_INFO( "Successfully created tool " << m_extrapolator.typeAndName() ); 
  
  // Get pointer to StoreGateSvc and cache:
  if (service("StoreGateSvc", m_storeGate) == StatusCode::FAILURE)
  {
    ATH_MSG_WARNING( "Unable to retrieve pointer to StoreGateSvc. Listener may not work" );
  }
  
  //Get Tool services
  IToolSvc* toolsvc;
  if (service("ToolSvc",toolsvc) == StatusCode::FAILURE)
  {
    ATH_MSG_FATAL( "Could not get tool sevices " );
    return StatusCode::FAILURE;
  }
  
  // Retrieve CaloSurfaceBuilder
  sc = toolsvc->retrieveTool("CaloSurfaceBuilder",m_surface,this); // *this* means the tool is private
  if(sc.isFailure()){ 
    ATH_MSG_FATAL( "Could not find Tool CaloSurfaceBuilder" );
    return StatusCode::FAILURE;
  } 
  
  // Get Calo Depth
  m_calodepth = m_surface->getCaloDepth();
  if (!m_calodepth){
    ATH_MSG_WARNING( "Cannot get CaloDepthTool" );
    m_calodepth=0;
    return StatusCode::FAILURE;
  }
  if (m_cosmics) {
    sc = toolsvc->retrieveTool("TrackDirectionTool", m_trackDirectionTool);
    if(sc.isFailure()){
      ATH_MSG_FATAL( "Cannot get TrackDirectionTool");
      return StatusCode::FAILURE;
    }
  }
  else
    m_trackDirectionTool = 0;
  
  m_calo_dd = CaloDetDescrManager::instance();
 
  setParticleHypothesis(m_particleHypothesisString); 
  getCaloSamples();
  initializeListener( this, name());
  return StatusCode::SUCCESS;
}
// ------------------------------------------------------------------ 
StatusCode TrackExtrapolatorToCalo::finalize() 
{
  return StatusCode::SUCCESS;
}
//--------------------------------------------------------------------

/** Fill the vectors m_caloSamples and m_etaMax with the list of CaloSamples per layer and the maximum eta of each.
    Called in initialization **/
void TrackExtrapolatorToCalo::getCaloSamples()
{
  m_caloSamples.resize(NLAYERS);
  m_etaMax.resize(NLAYERS);
  
  m_caloSamples[ps].push_back(CaloCell_ID::PreSamplerB);
  m_caloSamples[ps].push_back(CaloCell_ID::PreSamplerE);
  m_caloSamples[em1].push_back(CaloCell_ID::EMB1);
  m_caloSamples[em1].push_back(CaloCell_ID::EME1);
  m_caloSamples[em2].push_back(CaloCell_ID::EMB2);
  m_caloSamples[em2].push_back(CaloCell_ID::EME2);
  m_caloSamples[em3].push_back(CaloCell_ID::EMB3);
  m_caloSamples[em3].push_back(CaloCell_ID::EME3);

  m_caloSamples[tile1].push_back(CaloCell_ID::TileBar0);
  m_caloSamples[tile1].push_back(CaloCell_ID::TileExt0);
  m_caloSamples[tile2].push_back(CaloCell_ID::TileBar1);
  m_caloSamples[tile2].push_back(CaloCell_ID::TileGap1);
  m_caloSamples[tile2].push_back(CaloCell_ID::TileExt1);
  m_caloSamples[tile3].push_back(CaloCell_ID::TileBar2);
  m_caloSamples[tile3].push_back(CaloCell_ID::TileGap2);
  m_caloSamples[tile3].push_back(CaloCell_ID::TileExt2);

  m_caloSamples[hec0].push_back(CaloCell_ID::HEC0);
  m_caloSamples[hec1].push_back(CaloCell_ID::HEC1);
  m_caloSamples[hec2].push_back(CaloCell_ID::HEC2);
  m_caloSamples[hec3].push_back(CaloCell_ID::HEC3);

  
  for (unsigned int i = 0; i < m_caloSamples.size(); i++)
  {
    m_etaMax[i].resize( m_caloSamples[i].size() );
    bool isTile = (i >= tile1 && i <= tile3);
    for (unsigned int j = 0; j < m_caloSamples[i].size(); j++)
    {
      m_etaMax[i][j] = etamax(m_caloSamples[i][j], isTile );
      ATH_MSG_DEBUG( "Sample: " << m_caloSamples[i][j] << " etaMax: " << m_etaMax[i][j] );
    }
  }
}
//-------------------------------------------------------------------- 
double TrackExtrapolatorToCalo::etamax(const CaloCell_ID::CaloSample sample, const bool isTile) 
{
  double  etamax = 0; 
  std::vector <CaloDetDescriptor*>::const_iterator  first; 
  std::vector <CaloDetDescriptor*>::const_iterator  last; 
  if(isTile){
    first = m_calo_dd->tile_descriptors_begin();
    last = m_calo_dd->tile_descriptors_end();
  }
  else{
    first = m_calo_dd->calo_descriptors_begin();
    last = m_calo_dd->calo_descriptors_end(); 
  }
  for (;first != last; first++) {
    CaloDetDescriptor* descr = *first;
    if (descr) {
      if ( descr->getSampling(0) == sample){ 
        if( etamax  < descr->calo_eta_max() )  etamax = descr->calo_eta_max();
      }
    }
  }
  return etamax;
} 
// ------------------------------------------------------------------
double TrackExtrapolatorToCalo::etamin(const CaloCell_ID::CaloSample sample, const bool isTile) {
  
  double  etamin = 99999.; 
  std::vector <CaloDetDescriptor*>::const_iterator  first; 
  std::vector <CaloDetDescriptor*>::const_iterator  last; 
  if(isTile){
    first = m_calo_dd->tile_descriptors_begin();
    last = m_calo_dd->tile_descriptors_end();
  }
  else{
    first = m_calo_dd->calo_descriptors_begin();
    last = m_calo_dd->calo_descriptors_end(); 
  }
  for (;first != last; first++) {
    CaloDetDescriptor* descr = *first;
    if (descr) {
      if ( descr->getSampling(0) == sample){ 
        if( etamin  > descr->calo_eta_min() )  etamin = descr->calo_eta_min();
      }
    }
  }
  return etamin;
}
//--------------------------------------------------------------------
/** Clear vectors (extrapolations, samples used, path length) and pointer to current track. Called by listener when event finishes **/ 
void TrackExtrapolatorToCalo::m_clear()
{
  ATH_MSG_DEBUG( "Calling m_clear" );
  // Delete extrapolations to entrance and exit
  std::vector<const Trk::TrackParameters*>::iterator it = m_extrapolationsToEntrance.begin();
  for ( ; it != m_extrapolationsToEntrance.end(); it++)
    if (*it)
      delete (*it);
  for (unsigned int i=0; i < m_extrapolationsToExit.size(); i++)
    if ( (i == ps || i == em3 || i == tile3 || i == hec3) && m_extrapolationsToExit[i] ) // the others have been deleted
      delete m_extrapolationsToExit[i];
  
  m_currentTrackParameters = 0;
  m_currentTrkTrack = 0;
  m_currentI4Mom = 0;
  m_extrapolationsToEntrance.clear();
  m_extrapolationsToExit.clear();
  m_sampleUsed.clear();
  m_pathLength.clear();
  
}
//--------------------------------------------------------------------
/** Check if given track is the one stored in m_currentTrackParameters. If not, store it **/
bool TrackExtrapolatorToCalo::newTrack(const Trk::TrackParameters* track)
{
  if (!m_listener)
  {
    ATH_MSG_DEBUG( "Incident Service not active: calling m_clear" );
    m_clear();
  }
  if (!track)
    ATH_MSG_WARNING( "Null pointer to trackParameters! " );

  bool new_track = (!m_currentTrackParameters ||  track != m_currentTrackParameters);
  if ( new_track )
  {
    m_clear();
    m_currentTrackParameters = track;
  }
  ATH_MSG_DEBUG( "New track: " << new_track );
  return new_track;
}
// ------------------------------------------------------------------
/** Create a surface for the given sample, extrapolate the track and return the Trk::TrackParameters*
Surface usually corresponds to the layer entrance, unless <offset> is given or <back_of_sample> is set **/
const Trk::TrackParameters* TrackExtrapolatorToCalo::extrapolateToSample(CaloCell_ID::CaloSample sample,
  const Trk::TrackParameters* param0, 
  double offset, 
  bool back_of_sample)
{
  const Trk::TrackParameters* param1 = 0;
  
  if (sample!=CaloCell_ID::Unknown)
  {
    double etaAtCalo = param0->eta();
    // Use the position in the calorimeter frame to construct the surface
    // ignore it for the first extrapolation (normally to the PS)
    // N.B.: phi is not used for building the surface
    if (param0->position().mag() > 800)
    {
        double phiCalo, eta = param0->position().eta();
        m_caloCellDetPos->getDetPosition(sample, eta, 0., etaAtCalo, phiCalo);
    }
  
    Trk::Surface* surfaceToExtrapolate = 0;
    if (back_of_sample) // create a surface at the back of the sample
      surfaceToExtrapolate = m_surface->CreateLastSurface(sample,offset,etaAtCalo);
    else // create a surface at the entrance of the sample
      surfaceToExtrapolate = m_surface->CreateUserSurface(sample,offset,etaAtCalo);
  
    if (surfaceToExtrapolate)
    {
      bool checkBoundaries = (m_propDirection == 1); // only for alongMomentum
     // Extrapolate
//       param1 = m_extrapolator->extrapolate(*param0, *surfaceToExtrapolate, Trk::anyDirection, checkBoundaries,particleHypothesis);
      param1 = m_extrapolator->extrapolate(*param0, *surfaceToExtrapolate, m_propDirection, checkBoundaries,m_particleHypothesis);
      
      //Delete the surface once has been used
      delete surfaceToExtrapolate;
    }
    else
    {
      ATH_MSG_DEBUG( "Cannot create surface on sample " 
        << sample 
        << ".Returning null pointer TrackParameters" );
    }
  }
  //if (param1 && deltaR(param0, param1) > 0.2)
  //{
  //  ATH_MSG_WARNING( "Extrapolation to sample " << sample << " far from previous one."
  //     "DeltaR = " << deltaR(param0, param1) << " Ignoring and returning input value" );
    //param1 = param0;
  //}
  
  //
  return param1;
}
// ------------------------------------------------------------------
/** Extrapolate the track to the entrance of each layer, save the results in m_extrapolation results and
    return a vector with one <const TrackParameter*> per layer (entry is null if extrapolation failed) **/
std::vector< const Trk::TrackParameters* > TrackExtrapolatorToCalo::getExtrapolationsToLayerEntrance(const Trk::TrackParameters* track)
{
  ATH_MSG_DEBUG( "In TrackExtrapolatorToCalo::getExtrapolationsToLayerEntrance" );
  if (!track)
  {
    ATH_MSG_WARNING( "Null pointer to TrackParameters!" 
       "Returning empty vector for extrapolations" );
    return std::vector< const Trk::TrackParameters* >();
  }
  
  if ( !newTrack(track) && m_extrapolationsToEntrance.size() != 0)
    return m_extrapolationsToEntrance;

  m_sampleUsed.resize(NLAYERS, CaloCell_ID::Unknown);
  const Trk::TrackParameters* currentParameters = track;
  const Trk::TrackParameters* lastParamUpToEM = track;
  
  for (int layer = ps; layer <= hec3; layer++)
  {
    // Extrapolate from last valid one
    // For tile and hec, start from last valid in the EM
    if (layer == tile1 || layer == hec0)
      currentParameters = lastParamUpToEM;
    // Choose what CaloSample to extrapolate first (barrel / gap / endcap)
    unsigned int jsample = 0;
    for ( ; jsample < m_caloSamples[layer].size() && m_etaMax[layer][jsample] <= fabs(currentParameters->eta()); jsample++) {};
    // Try to extrapolate to chosen sample. While it fails, go to next one
    const Trk::TrackParameters *param1 = 0;
    for (unsigned int isample = 0; isample < m_caloSamples[layer].size() && !param1; isample++, jsample++)
    {
      if ( jsample == m_caloSamples[layer].size() )
        jsample = 0;
      ATH_MSG_DEBUG( "Layer: " << layer << ". Trying to extrapolate to sample: " 
          << jsample << " " << m_caloSamples[layer][jsample] );
      param1 = extrapolateToSample(m_caloSamples[layer][jsample], currentParameters);
    }
    if (param1)
    {
      m_sampleUsed[layer] =  m_caloSamples[layer][--jsample];
      ATH_MSG_DEBUG( "Layer: " << layer << " In sample: " << m_caloSamples[layer][jsample]
        << "(" << m_sampleUsed[layer] << ")" << " extrapolation succeeded" );
      currentParameters = param1;
      if (layer <= em3)
        lastParamUpToEM = param1;
    }
    m_extrapolationsToEntrance.push_back(param1);
  }
  return m_extrapolationsToEntrance;
}
//-----------------------------------------------------------
/** Return the extrapolations to the exit of each layer **/
std::vector< const Trk::TrackParameters* > TrackExtrapolatorToCalo::getExtrapolationsToLayerExit(const Trk::TrackParameters* track)
{
  ATH_MSG_DEBUG( "In TrackExtrapolatorToCalo::getExtrapolationsToLayerExit" );

  if ( (!newTrack(track) && m_extrapolationsToExit.size() != 0) || !track)
    return m_extrapolationsToExit;

  getExtrapolationsToLayerEntrance(track);
  if (m_extrapolationsToEntrance.size() == 0)
    return std::vector< const Trk::TrackParameters* >();
  
  m_extrapolationsToExit.resize(NLAYERS, 0);
  m_pathLength.resize(NLAYERS, 0.);
  for (int layer = ps; layer <= hec3; layer++)
  {
    const Trk::TrackParameters* par = m_extrapolationsToEntrance[layer];
    if (par)
    {
      // For the ps or external layers, do another extrapolation
      // For the others, just take the following layer
      if (layer != ps && layer != em3 && layer != tile3 && layer != hec3)
        m_extrapolationsToExit[layer] = m_extrapolationsToEntrance[layer + 1];
      else
        if (layer != ps && layer != em3) // ps and em3 are not 'last samples', has to take the depth and set as offset
          m_extrapolationsToExit[layer] = extrapolateToSample(m_sampleUsed[layer], par, 0, true);
        else if (m_calodepth)
        {
          CaloCell_ID::CaloSample sample = m_sampleUsed[layer];
          double entrance = m_calodepth->entrance(sample, par->position().eta(), par->position().phi());
          // double middle = m_calodepth->middle(sample, par->position().eta(), par->position().phi());
          double exit = m_calodepth->exit(sample, par->position().eta(), par->position().phi());
          ATH_MSG_DEBUG( "Layer: " << layer << "Sample: " << sample << " Entrance: " << entrance << " Exit: " << exit );
          // Extrapolate to the back of sample -> (exit - entrance) will automatically take care of the sign of the offset
          m_extrapolationsToExit[layer] = extrapolateToSample(sample, par, exit - entrance);
        }
    }
    else
      ATH_MSG_DEBUG( "No extrapolation to entrance for layer " << layer );
    if (!m_extrapolationsToExit[layer])
      ATH_MSG_DEBUG( "Extrapolation to back of sample failed for layer " << layer );
    else
      m_pathLength[layer] = (m_extrapolationsToExit[layer]->position() - par->position() ).mag();
  }
  ATH_MSG_DEBUG( "getExtrapolationsToLayerExit done" );
  return m_extrapolationsToExit;
}
//-----------------------------------------------------------
/** Return a vector with the path length of the track on each layer.
For the layers where the extrapolation failed, path length = 0 **/
std::vector<double> TrackExtrapolatorToCalo::getPathLengths(const Trk::TrackParameters* track)
{
  ATH_MSG_DEBUG( "In TrackInCaloTools::getPathLenghts" );
  if ( newTrack(track) || m_pathLength.size() == 0)
    getExtrapolationsToLayerExit(track);
  return m_pathLength;
}
//-----------------------------------------------------------
/** Given the TrackParameters, return a vector< pair<double, double> > with the mid-point between the extrapolations
for the entrance and the exit of each layer. When the extrapolation failed for the exit of the layer, 
take the position in the entrance. When it fails for both, take the one for the previous layer (or the track direction) **/
std::vector< std::pair<double, double> > TrackExtrapolatorToCalo::getMidPointForExtrapolations(const Trk::TrackParameters* track)
{
  std::vector< std::pair<double, double> > positions;
  if (!track)
    return positions; 
  std::pair<double, double> position;
  std::pair<double, double> positionUpToEM = std::make_pair(track->eta(), track->parameters()[Trk::phi]);
  if ( newTrack(track) || m_extrapolationsToExit.size() == 0)
    if (getExtrapolationsToLayerExit(track).size() == 0)
  {
    ATH_MSG_DEBUG( "Could not get extrapolations to layer exit" );
    return positions;
  }
  for (int layer = ps; layer <= hec3; layer++)
  {
    if (layer == ps || layer == tile1 || layer == hec0)
      position = positionUpToEM;
    const Trk::TrackParameters *entrance = m_extrapolationsToEntrance[layer];
    const Trk::TrackParameters *exit = m_extrapolationsToExit[layer];
    if (entrance && exit)
      position = std::make_pair( (entrance->position().eta() + exit->position().eta())/2.,
        phiMean(entrance->position().phi(), exit->position().phi()) );
    else if (entrance)
      position = std::make_pair(entrance->position().eta(), entrance->position().phi() );
    if (layer <= em3)
      positionUpToEM = position;
    positions.push_back(position);
  }
  ATH_MSG_DEBUG( "Size of positions: " << positions.size() );
  return positions;
}
//-----------------------------------------------------------
/** Return the parametrized energy loss per layer (difference between the momentum at the entrance and exit) **/
std::vector<double> TrackExtrapolatorToCalo::getParametrizedEloss(const Trk::TrackParameters* track)
{
  std::vector<double> eloss(NLAYERS, 0.);
  if ( newTrack(track) || m_extrapolationsToExit.size() == 0)
    if (getExtrapolationsToLayerExit(track).size() == 0)
  {
    ATH_MSG_DEBUG( "Could not get extrapolations to layer exit" );
    return eloss;
  }
  for (int layer = ps; layer <= hec3; layer++)
  {
    const Trk::TrackParameters *entrance = m_extrapolationsToEntrance[layer];
    const Trk::TrackParameters *exit = m_extrapolationsToExit[layer];
    if (entrance && exit)
      eloss[layer] = entrance->charge()/entrance->parameters()[Trk::qOverP] - exit->charge()/exit->parameters()[Trk::qOverP];
  }
  return eloss;
}
//-----------------------------------------------------------
/** Return dR = sqrt(deta^2 + dphi^2) for the positions of TrackParameters* a and b **/
double TrackExtrapolatorToCalo::deltaR(const Trk::TrackParameters *a, const Trk::TrackParameters *b)
{
  double deta = a->position().eta() - b->position().eta();
  double dphi = CaloPhiRange::diff(a->position().phi(), b->position().phi());
  return sqrt( deta*deta + dphi*dphi );
}
//-----------------------------------------------------------
void TrackExtrapolatorToCalo::setPropagationDirection(Trk::PropDirection direction)
{
  ATH_MSG_DEBUG( "Setting propagation direction: " << direction );
  if (direction == m_propDirection)
    return;
  m_propDirection = direction;
  m_clear();
}
//------------------------------------------------------------------ 
/** Set the propagation direction (along or opposite to momentum) given a Trk::Track from a cosmic event **/
bool TrackExtrapolatorToCalo::setPropagationDirection(const Trk::Track* track)
{
  Trk::PropDirection direction;
  if (!m_trackDirectionTool->getPropagationDirection(track, direction) )
    return false;
  setPropagationDirection(direction);
  return true;
}
//------------------------------------------------------------------ 
/** Set the propagation direction (along or opposite to momentum) given a INavigable4Momentum from a cosmic event **/
bool TrackExtrapolatorToCalo::setPropagationDirection(const INavigable4Momentum* track)
{
  Trk::PropDirection direction;
  if (!m_trackDirectionTool->getPropagationDirection(track, direction) )
    return false;
  setPropagationDirection(direction);
  return true;
}
//-----------------------------------------------------------
const Trk::TrackParameters* TrackExtrapolatorToCalo::getTrackParameters(const Trk::Track *track)
{
  ATH_MSG_DEBUG( "In getTrackParameters(const Trk::Track)" );
  if (!track) {
    ATH_MSG_WARNING( "Null pointer to Trk::Track" );
    return 0;
  }
  if (m_listener && track == m_currentTrkTrack)
    return m_currentTrackParameters;
  
  // Set the propagation direction if it is a cosmic event
  if (m_cosmics && !setPropagationDirection(track))
    return 0;
  m_currentTrkTrack = track;
  return track->perigeeParameters();
}
//------------------------------------------------------------------ 
const Trk::TrackParameters* TrackExtrapolatorToCalo::getTrackParameters(const INavigable4Momentum *track)
{
  ATH_MSG_DEBUG( "In getTrackParameters(INavigable4Momentum)" );
  if (!track) {
    ATH_MSG_WARNING( "Null pointer to INavigable4Momentum" );
    return 0;
  }
  
  if (m_listener && track == m_currentI4Mom)
    return m_currentTrackParameters;
  
  const Analysis::Muon * muon = dynamic_cast<const Analysis::Muon*> (track);
  const Rec::TrackParticle * trackParticle = 0;
  if ( muon )
    trackParticle = muon->track();
  else
    trackParticle = dynamic_cast<const Rec::TrackParticle*>(track);
  
  if (!trackParticle)
  {
    ATH_MSG_DEBUG( "Could not get TrackParticle from INavigable4Momentum" );
    return 0;
  }
  
  // Set the propagation direction if it is a cosmic event
  if (m_cosmics && !setPropagationDirection(trackParticle))
    return 0;
  m_currentI4Mom = trackParticle;
  return &trackParticle->definingParameters();
}
//-----------------------------------------------------------
/** Define the particle hypothesis for the extrapolation **/
void TrackExtrapolatorToCalo::setParticleHypothesis(const std::string pHypothesis)
{
  // Convert string to lowercase
  std::string particleHypothesis = pHypothesis;
  for (unsigned int i = 0; i < particleHypothesis.length(); ++i)
    particleHypothesis[i] = std::tolower(pHypothesis[i]);
  
  if (particleHypothesis == "muon")
    m_particleHypothesis = Trk::muon;
  else if (particleHypothesis == "electron")
    m_particleHypothesis = Trk::electron;
  else if (particleHypothesis == "pion")
    m_particleHypothesis = Trk::pion;
  else
    ATH_MSG_WARNING( "Invalid particle: " << particleHypothesis << ", nothing changed" );
}
