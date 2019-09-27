/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/


// Local
#include "VertexFittingSvc.h"

// Athena
#include "xAODBTagging/SecVtxHelper.h"

// C/C++
#include <cmath>
#include <iostream>
#include <sstream>

using namespace std;

//=============================================================================
Prompt::VertexFittingSvc::VertexFittingSvc(const std::string &name, ISvcLocator *pSvcLocator):
  base_class                (name, pSvcLocator),
  m_vertexFitter            ("Trk::FastVertexFitter/FastVertexFitter", this),
  m_seedVertexFitter        ("Trk::FastVertexFitter/FastVertexFitter", this),
  m_countNumberOfFits       (0),
  m_countNumberOfFitsFailed (0),
  m_countNumberOfFitsInvalid(0)
{  
  declareProperty("vertexFitterTool",                  m_vertexFitter);
  declareProperty("seedVertexFitterTool",              m_seedVertexFitter);

  declareProperty("debug",                             m_debug           = false);
  declareProperty("doSeedVertexFit",                   m_doSeedVertexFit = false);

  declareProperty("DistToPriVtxName",                  m_distToPriVtxName);
  declareProperty("NormDistToPriVtxName",              m_normDistToPriVtxName);
  declareProperty("DistToRefittedPriVtxName",          m_distToRefittedPriVtxName);
  declareProperty("NormDistToRefittedPriVtxName",      m_normDistToRefittedPriVtxName);
  declareProperty("DistToRefittedRmLepPriVtxName",     m_distToRefittedRmLepPriVtxName);
  declareProperty("NormDistToRefittedRmLepPriVtxName", m_normDistToRefittedRmLepPriVtxName);
}

//=============================================================================
StatusCode Prompt::VertexFittingSvc::initialize()
{
  ATH_CHECK(m_vertexFitter.retrieve());

  if(m_doSeedVertexFit) {
    ATH_CHECK(m_seedVertexFitter.retrieve());    
  }
  
  m_distToPriVtx                   = std::make_unique<SG::AuxElement::Decorator<float> > (m_distToPriVtxName);
  m_normDistToPriVtx               = std::make_unique<SG::AuxElement::Decorator<float> > (m_normDistToPriVtxName);
  m_distToRefittedPriVtx           = std::make_unique<SG::AuxElement::Decorator<float> > (m_distToRefittedPriVtxName);
  m_normDistToRefittedPriVtx       = std::make_unique<SG::AuxElement::Decorator<float> > (m_normDistToRefittedPriVtxName);
  m_distToRefittedRmLepPriVtx      = std::make_unique<SG::AuxElement::Decorator<float> > (m_distToRefittedRmLepPriVtxName);
  m_normDistToRefittedRmLepPriVtx  = std::make_unique<SG::AuxElement::Decorator<float> > (m_normDistToRefittedRmLepPriVtxName);

  m_indexDec                       = std::make_unique<SG::AuxElement::Decorator<int> >   ("SecondaryVertexIndex");
  m_decoratorType                  = std::make_unique<SG::AuxElement::Decorator<int> >   ("SVType");

  m_timer.Reset();

  ATH_MSG_INFO("VertexFittingSvc::initialize - doSeedVertexFit = " << m_doSeedVertexFit);
  ATH_MSG_INFO("VertexFittingSvc::initialize - vertexFitter    = " << m_vertexFitter.name());

  return StatusCode::SUCCESS;
}

//=============================================================================
StatusCode Prompt::VertexFittingSvc::finalize()
{
  //
  // Delete pointers
  //
  m_timer.Stop();

  ATH_MSG_INFO("VertexFittingSvc::finalize - number of total   fits: " << m_countNumberOfFits);
  ATH_MSG_INFO("VertexFittingSvc::finalize - number of failed  fits: " << m_countNumberOfFitsFailed);
  ATH_MSG_INFO("VertexFittingSvc::finalize - number of invalid vtxs: " << m_countNumberOfFitsInvalid);
  ATH_MSG_INFO("VertexFittingSvc::finalize - fitting timer: " << PrintResetStopWatch(m_timer));

  return StatusCode::SUCCESS;
}

//=============================================================================
std::pair<xAOD::Vertex*, int>  Prompt::VertexFittingSvc::fitVertexWithPrimarySeed(const FittingInput &input,
										  const std::vector<const xAOD::TrackParticle* > &tracks,
										  VtxType vtxType)
{
  //
  // Make new secondary vertex
  //
  TimerScopeHelper timer(m_timer);
  m_countNumberOfFits++;

  std::pair<xAOD::Vertex*, int> out(0, 0);

  if(!input.priVtx) { 
    ATH_MSG_WARNING("fitVertexWithPrimarySeed -- missing primary vertex");      
    return out;
  }

  out.first = getSecondaryVertexWithSeed(tracks, input.inDetTracks, input.priVtx->position());

  if(!out.first) { 
    m_countNumberOfFitsFailed++;
    ATH_MSG_WARNING("fitVertexWithPrimarySeed -- failed to fit vertex");
    return out;
  }

  std::unique_ptr<xAOD::Vertex> ptr(out.first);

  if(!isValidVertex(out.first)) {
    m_countNumberOfFitsInvalid++;
    out.first = 0;

    ATH_MSG_WARNING("fitVertexWithPrimarySeed -- failed to get valid vertex");      
    return out;
  }

  out.second = m_secondaryVertexIndex++;
  
  (*m_indexDec)           (*out.first) = out.second;
  (*m_decoratorType)      (*out.first) = static_cast<int>(vtxType);

  decorateNewSecondaryVertex(input, out.first);

  //
  // Store smart pointer for memory management
  //
  m_storedVtxPtrs.push_back(std::move(ptr));

  return out;
}

//=============================================================================
std::pair<xAOD::Vertex*, int>  Prompt::VertexFittingSvc::fitVertexWithSeed(const FittingInput &input,
									   const std::vector<const xAOD::TrackParticle* > &tracks,
									   const Amg::Vector3D& seed,
									   VtxType vtxType)
{
  //
  // Make new secondary vertex
  //
  TimerScopeHelper timer(m_timer);
  m_countNumberOfFits++;

  std::pair<xAOD::Vertex*, int> out(0, 0); 

  out.first = getSecondaryVertexWithSeed(tracks, input.inDetTracks, seed);

  if(!out.first) { 
    m_countNumberOfFitsFailed++;

    ATH_MSG_WARNING("fitVertexWithSeed -- failed to fit vertex");      
    return out;
  }

  std::unique_ptr<xAOD::Vertex> ptr(out.first);

  if(!isValidVertex(out.first)) {
    m_countNumberOfFitsInvalid++;
    out.first = 0;

    ATH_MSG_WARNING("fitVertexWithSeed -- failed to get valid vertex");      
    return out;
  }

  out.second = m_secondaryVertexIndex++;
  
  (*m_indexDec)           (*out.first) = out.second;
  (*m_decoratorType)      (*out.first) = static_cast<int>(vtxType);

  decorateNewSecondaryVertex(input, out.first);

  //
  // Store pointer for memory management
  //
  m_storedVtxPtrs.push_back(std::move(ptr));

  return out;
}

//=============================================================================
bool Prompt::VertexFittingSvc::moveVertexPointerToContainer(xAOD::Vertex *vtx, xAOD::VertexContainer &container)
{
  //
  // Release pointer to vertex and record it with VertexContainer
  //
  ATH_MSG_DEBUG(__FILE__ << "::moveVertexPointerToContainer - process vertex: " << vtx);

  std::vector<vtxPtr_t>::iterator vit = m_storedVtxPtrs.begin();

  while(vit != m_storedVtxPtrs.end()) {
    if(vit->get() != vtx) {
      vit++;

      continue;
    }

    container.push_back(std::move(*vit));

    m_storedVtxPtrs.erase(vit);

    return true;
  }

  ATH_MSG_DEBUG(__FILE__ << "::moveVertexPointerToContainer - ALL DONE");

  return false;
}

//=============================================================================
void Prompt::VertexFittingSvc::resetEvent()
{  
  ATH_MSG_DEBUG("resetEvent - m_storedVtxPtrs size=" << m_storedVtxPtrs.size());

  m_storedVtxPtrs.clear();
}

//=============================================================================
bool Prompt::VertexFittingSvc::isValidVertex(const xAOD::Vertex *vtx) const
{
  //
  // Check if vertex is valid
  //
  bool bad_vtx = false;
  
  if(!vtx){
    ATH_MSG_WARNING("VertexFittingSvc::Validate_Vertex -- invalid vtx pointer!!!");
    return false;
  }
  
  if(vtx->covariance().empty()) {
    bad_vtx = true;
    ATH_MSG_WARNING("VertexFittingSvc::Validate_Vertex -- empty vtx covariance!!!");
  }
  
  float chisquared = -9999;

  if(!GetVar(vtx, chisquared, "chiSquared")) {
    bad_vtx = true;
    ATH_MSG_WARNING("VertexFittingSvc::Validate_Vertex -- not valid vtx chiSquared!!!");
  }
   
  float numberdof  = -9999;

  if(!GetVar(vtx, numberdof, "numberDoF")) {
    bad_vtx = true;
    ATH_MSG_WARNING("VertexFittingSvc::Validate_Vertex -- not valid vtx numberDoF!!!");
  }
  
  if(std::isnan(vtx->x()) || std::isnan(vtx->y()) || std::isnan(vtx->z())) {
    bad_vtx = true;
    ATH_MSG_WARNING("VertexFittingSvc::Validate_Vertex -- vertex coordinate is nan");
  }
  
  if(bad_vtx) {
    ATH_MSG_WARNING("VertexFittingSvc::Validate_Vertex -- bad vertex!!!");
    ATH_MSG_INFO(PrintPromptVertexAsStr(vtx, msg(MSG::WARNING)));
  }

  return !bad_vtx;
}

//=============================================================================
void Prompt::VertexFittingSvc::removeDoubleEntries(std::vector<const xAOD::TrackParticle*>& tracks)
{
  const unsigned nbefore = tracks.size();

  sort(tracks.begin(), tracks.end());

  typename std::vector<const xAOD::TrackParticle*>::iterator TransfEnd = std::unique(tracks.begin(), tracks.end());

  tracks.erase(TransfEnd, tracks.end());

  if(nbefore != tracks.size()) {
    ATH_MSG_DEBUG("removeDoubleEntries nbefore != tracks.size()): " << nbefore << " != " << tracks.size());

    if(m_debug) {
      int truthType = -99, truthOrigin = -99;
      
      if(GetVar(*TransfEnd, truthType, "truthType")) {
        ATH_MSG_DEBUG("removeDoubleEntries : removed track truthType = " << truthType); 
      }
      if(GetVar(*TransfEnd, truthOrigin, "truthOrigin")) {
        ATH_MSG_DEBUG("removeDoubleEntries : removed track truthOrigin = " << truthOrigin); 
      }
    }
  }
}

//=============================================================================
bool Prompt::VertexFittingSvc::decorateNewSecondaryVertex(const FittingInput &input, 
							  xAOD::Vertex *secVtx)
{ 
  //
  // Decorate secondary vertex with all useful information
  //
  if(!secVtx) {
    ATH_MSG_WARNING("decorateNewSecondaryVertex - invalid pointer");
    return false;
  } 

  //
  // Decorate secondary vertex with the distance/norm_distance it from Prmary Vertex, Refitted Primary vertex and Refitted Primary Vertex that removed lepton itself.
  //
  float distToPriVtx                  = -1;
  float normDistToPriVtx              = -1; 
  float distToRefittedPriVtx          = -1; 
  float normDistToRefittedPriVtx      = -1; 
  float distToRefittedRmLepPriVtx     = -1;
  float normDistToRefittedRmLepPriVtx = -1;

  if(input.priVtx) {
    distToPriVtx     = Prompt::GetDistance(input.priVtx->position(), secVtx->position());    
    normDistToPriVtx = Prompt::GetNormDist(input.priVtx->position(), secVtx->position(), secVtx->covariance(), msg(MSG::WARNING));
  }

  if(input.refittedPriVtx) {
    distToRefittedPriVtx     = Prompt::GetDistance(input.refittedPriVtx->position(), secVtx->position()); 
    normDistToRefittedPriVtx = Prompt::GetNormDist(input.refittedPriVtx->position(), secVtx->position(), secVtx->covariance(), msg(MSG::WARNING));
  }

  if(input.refittedPriVtxWithoutLep) {
    distToRefittedRmLepPriVtx     = Prompt::GetDistance(input.refittedPriVtxWithoutLep->position(), secVtx->position()); 
    normDistToRefittedRmLepPriVtx = Prompt::GetNormDist(input.refittedPriVtxWithoutLep->position(), secVtx->position(), secVtx->covariance(), msg(MSG::WARNING));
  }
 
  (*m_distToPriVtx)                 (*secVtx) = distToPriVtx;
  (*m_normDistToPriVtx)             (*secVtx) = normDistToPriVtx;
  (*m_distToRefittedPriVtx)         (*secVtx) = distToRefittedPriVtx;
  (*m_normDistToRefittedPriVtx)     (*secVtx) = normDistToRefittedPriVtx;
  (*m_distToRefittedRmLepPriVtx)    (*secVtx) = distToRefittedRmLepPriVtx;
  (*m_normDistToRefittedRmLepPriVtx)(*secVtx) = normDistToRefittedRmLepPriVtx;

  return true;
}

//=============================================================================
xAOD::Vertex* Prompt::VertexFittingSvc::getSecondaryVertexWithSeed(const std::vector<const xAOD::TrackParticle*> &tracks,
								   const xAOD::TrackParticleContainer *inDetTracks,
								   const Amg::Vector3D& seed)
{
  //  
  // Fit one vertex with given tracks
  //  
  std::vector<const xAOD::TrackParticle*> tracks_for_fit(tracks);

  ATH_MSG_DEBUG("getSecondaryVertexWithSeed -- before remove " << tracks_for_fit.size());

  removeDoubleEntries(tracks_for_fit);

  ATH_MSG_DEBUG("getSecondaryVertexWithSeed -- after remove " << tracks_for_fit.size());

  if(tracks_for_fit.size() < 2) {
    ATH_MSG_WARNING("getSecondaryVertexWithSeed -- cannot fit vertex with one or zero input track: ntrack=" << tracks_for_fit.size());
    return 0;
  }

  //  
  // Run fit
  // 
  if(m_debug) {
    ATH_MSG_INFO(name() << "::getSecondaryVertexWithSeed -- N tracks = " << tracks_for_fit.size()); 

    for(const xAOD::TrackParticle* track: tracks_for_fit) {
      ATH_MSG_INFO( name() << "::getSecondaryVertexWithSeed -- track pt, eta = " << track->pt() << "," << track->eta());
      ATH_MSG_INFO( name() << "::getSecondaryVertexWithSeed -- track chi2    = " << track->chiSquared());
    }
  }

  xAOD::Vertex *newVertex = 0;
  std::unique_ptr<xAOD::Vertex> seedVertex;

  if(m_doSeedVertexFit) {    
    seedVertex = std::unique_ptr<xAOD::Vertex>(m_seedVertexFitter->fit(tracks_for_fit, seed));
    
    if(seedVertex.get() && !isValidVertex(seedVertex.get())) {
      ATH_MSG_INFO("getSecondaryVertexWithSeed -- failed to fit seed vertex");
    
      seedVertex.reset();
    }
  }

  if(seedVertex.get()) {
    newVertex = m_vertexFitter->fit(tracks_for_fit, seedVertex->position());
  }
  else {
    newVertex = m_vertexFitter->fit(tracks_for_fit, seed);
  }
   
  if(!newVertex) {
    ATH_MSG_INFO("getSecondaryVertexWithSeed -- failed to fit vertex and fitter returned null xAOD::Vertex pointer");
    return 0;
  }

  //  
  // Save vertex tracks
  // 
  std::vector<ElementLink< xAOD::TrackParticleContainer> > tpLinks;

  if(inDetTracks) {
    //
    // Record links to ID tracks if container pointer is provided
    //
    for(const xAOD::TrackParticle *selectedtrack: tracks_for_fit) {
      ElementLink<xAOD::TrackParticleContainer> tpLink;
      
      tpLink.toContainedElement(*inDetTracks, selectedtrack);
      tpLinks.push_back(tpLink);
    }
  }

  newVertex->setTrackParticleLinks(tpLinks);
 
  TLorentzVector Momentum;

  for(const xAOD::TrackParticle* track: tracks_for_fit) {
    Momentum += static_cast<TLorentzVector>(track->p4());
  }

  xAOD::SecVtxHelper::setVertexMass(newVertex, Momentum.M()); // "mass"

  return newVertex;
}

