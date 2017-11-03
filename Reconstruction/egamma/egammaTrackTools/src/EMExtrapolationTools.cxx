/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include  "EMExtrapolationTools.h"
#include  "TrkEventPrimitives/PropDirection.h"
#include  "TrkParameters/TrackParameters.h"

//extrapolation
#include "TrkExInterfaces/IExtrapolator.h"
#include "InDetIdentifier/TRT_ID.h"
#include "TrkNeutralParameters/NeutralParameters.h"
//
#include "xAODCaloEvent/CaloCluster.h"
#include "xAODTracking/TrackParticle.h"
#include "xAODTracking/Vertex.h"
//
#include "TrkTrack/Track.h"
#include "TrkPseudoMeasurementOnTrack/PseudoMeasurementOnTrack.h"
//
#include "xAODEgamma/EgammaxAODHelpers.h"
#include "TrkParametersIdentificationHelpers/TrackParametersIdHelper.h"
#include "TrkParametersIdentificationHelpers/TrackParametersIdentificationHelper.h"
#include "TrkTrack/TrackStateOnSurface.h"
#include "FourMomUtils/P4Helpers.h"
#include "FourMomUtils/P4Helpers.h"
//
#include <tuple>

EMExtrapolationTools::EMExtrapolationTools(const std::string& type, 
                                           const std::string& name,
                                           const IInterface* parent) :
  AthAlgTool(type, name, parent),
  m_trtId(0)
{

  // Name of the utility for conversion
  
  declareInterface<IEMExtrapolationTools>(this);
}

// =========================================================
EMExtrapolationTools::~EMExtrapolationTools() {}

// =========================================================
StatusCode EMExtrapolationTools::initialize() 
{

  ATH_MSG_DEBUG("Initializing " << name() << "...");

  // Retrieve TrackToCalo tool for extrapolation to calorimeter
  if ( m_defaultParticleCaloExtensionTool.retrieve().isFailure() ) {
    ATH_MSG_ERROR("Cannot retrieve DefaultParticleCaloExtensionTool" << m_defaultParticleCaloExtensionTool );
    return StatusCode::FAILURE;
  }
  else {
    ATH_MSG_DEBUG("Retrieved DefaultParticleCaloExtensionTool tool " << m_defaultParticleCaloExtensionTool);
  }

  if ( m_perigeeParticleCaloExtensionTool.retrieve().isFailure() ) {
    ATH_MSG_ERROR("Cannot retrieve perigeeParticleCaloExtensionTool" << m_perigeeParticleCaloExtensionTool );
    return StatusCode::FAILURE;
  }
  else {
    ATH_MSG_DEBUG("Retrieved perigeeParticleCaloExtensionTool tool " << m_perigeeParticleCaloExtensionTool);
  }

  if (m_extrapolator.retrieve().isFailure() ) {
    ATH_MSG_ERROR("Cannot retrieve extrapolator" << m_extrapolator );
    return StatusCode::FAILURE;
  }
  else {
    ATH_MSG_DEBUG("Retrieved extropolator tool " << m_extrapolator);
  }

  // retrieve TRT-ID helper
  if (detStore()->contains<TRT_ID>("TRT_ID")) {
    StatusCode sc = detStore()->retrieve(m_trtId, "TRT_ID");
    if (sc.isFailure() || !m_trtId->is_valid()){
      // TRT is not present for sLHC
      ATH_MSG_DEBUG("Could not get TRT_ID helper !");
      m_trtId = 0;
    }
    ATH_MSG_DEBUG("m_trtId initialization successful");
  } else {
    ATH_MSG_DEBUG("Could not get TRT_ID helper !");
  }
  
  return StatusCode::SUCCESS;
}

// =====================================================================
StatusCode EMExtrapolationTools::finalize() 
{
  return StatusCode::SUCCESS;
}

// ======================= Electron Charged Track related interfaces

//////////////////////////////////////////////////////////////////////////////
//* Extrapolate TrackParticle to electron CaloCluster
//* Returns selection
//* i.e performs the cuts
//It also returns the quantities used for matching
//////////////////////////////////////////////////////////////////////////////
bool
EMExtrapolationTools::matchesAtCalo(const xAOD::CaloCluster*      cluster, 
                                    const xAOD::TrackParticle*    trkPB, 
                                    bool                          isTRT, 
                                    Trk::PropDirection            direction,
                                    std::vector<double>&          eta,
                                    std::vector<double>&          phi,
                                    std::vector<double>&          deltaEta,
                                    std::vector<double>&          deltaPhi,
                                    unsigned int                  extrapFrom) const  
{

  // Sample for testing match
  unsigned int               iSampling  = 2;  

  // Decide matching for TRT standalone
  // In case of standalone TRT tracks get an idea where the last measurement is
  int isTRTB = 0 ;
  if(isTRT){
    if (!m_trtId) {
      ATH_MSG_WARNING("Should have m_trtId defined for isTRT");
      return false;
    }
    isTRTB = getTRTsection(trkPB);
    // First pass on TRT tracks, skip barrel tracks matching endcap clusters and vice-versa
    if((isTRTB==2 && (cluster->eta()<=0.6  || cluster->eta()>=2.4)   ) ||
       (isTRTB==-2 && (cluster->eta()>=-0.6 || cluster->eta()<=-2.4)  ) ||
       (isTRTB==1 && (cluster->eta()<=-0.1 || cluster->eta()>=1.3)   ) ||
       (isTRTB==-1 && (cluster->eta()>=0.1  || cluster->eta()<=-1.3)  )
       ) {
      return false;
    }
  }
  
  //Call getMatchAtCalo
  if(getMatchAtCalo (cluster,trkPB,isTRT,direction, eta,phi, deltaEta, deltaPhi,extrapFrom).isFailure()){
    ATH_MSG_WARNING("getMatchAtCalo failed");
    return false;
  }  
  
  // Selection in the narrow eta/phi window
  if((isTRT && ((abs(isTRTB)==1 && deltaPhi[iSampling] < m_narrowDeltaPhiTRTbarrel && 
		 deltaPhi[iSampling] > -m_narrowDeltaPhiBremTRTbarrel) || 
		(abs(isTRTB)==2 && deltaPhi[iSampling] < m_narrowDeltaPhiTRTendcap && 
		 deltaPhi[iSampling] > -m_narrowDeltaPhiBremTRTendcap)))
     || 
     (!isTRT && fabs(deltaEta[iSampling]) < m_narrowDeltaEta && 
      deltaPhi[iSampling] < m_narrowDeltaPhi && 
      deltaPhi[iSampling] > -m_narrowDeltaPhiBrem) ) {
    return true;
  }

  return false;
}
/////////////////////////////////////////////////////////////////////////////////////////////
//* Extrapolate to calorimeter for trackParticle
/////////////////////////////////////////////////////////////////////////////////////////////
StatusCode
EMExtrapolationTools::getMatchAtCalo (const xAOD::CaloCluster*      cluster, 
                                      const xAOD::TrackParticle*    trkPB,
				      bool                          isTRT, 
                                      Trk::PropDirection            direction,
                                      std::vector<double>&          eta,
                                      std::vector<double>&          phi,
                                      std::vector<double>&          deltaEta,
                                      std::vector<double>&          deltaPhi,
                                      unsigned int                  extrapFrom) const
{
  // Extrapolate track to calo and return the extrapolated eta/phi and
  // the deta/dphi between cluster and track
  //   We allow different ways to extrapolate:
  // 
  //     1) from the last measurement point track parameters  
  //     2) from the perigee 
  //     3) from the perigee with the track momentum rescaled by the cluster energy
  //
  ATH_MSG_DEBUG("getMatchAtCalo");
   
  if(deltaEta.size() < 4  || deltaPhi.size()<4 || eta.size()<4 || phi.size()<4 ){
    ATH_MSG_WARNING("deltaEta, deltaPhi, eta , phi size should be at least 4");
    return StatusCode::SUCCESS;
  }

  //------------------------------------------------------------------------------------------------------------------------------------//
  //------------------- Create the extension, supports different methods of extrapolation (i.e last point, perigee, perigee rescaled ---//  
  const Trk::CaloExtension* extension = 0;      
  double atPerigeePhi(-999);
  double PerigeeTrkParPhi(-999);
  //TRT  only track. Could be in the standard or GSF container.
  //Use the std tool and the cached result always. For TRT only it does not matter if perigee or rescaled requested.
  if (isTRT){
    if(!m_defaultParticleCaloExtensionTool->caloExtension(*trkPB,extension,m_useCaching)){
      ATH_MSG_INFO("Could not create an extension for TRT only track with : "<< " Track Pt "
		      <<trkPB->pt()<< " Track Eta " << trkPB->eta()<<" Track Fitter " 
		      << trkPB->trackFitter() << " isTRT " << isTRT<<" Extrapolate From " <<  extrapFrom); 
      return StatusCode::SUCCESS;
    }
  }
  //extrapolation using  Rescaled Perigee, not caching possible as it is changed per cluster
  else if (fromPerigeeRescaled == extrapFrom) {
    const Trk::TrackParameters*  trkPar = getRescaledPerigee(trkPB, cluster);    
    if(!trkPar){
      ATH_MSG_ERROR("getMatchAtCalo: Cannot access track parameters"); 
      return StatusCode::FAILURE; 
    }
    extension = m_perigeeParticleCaloExtensionTool->caloExtension( *trkPar, direction, Trk::muon );
    Amg::Vector3D atPerigee(trkPar->position().x(), trkPar->position().y(), trkPar->position().z()); 
    atPerigeePhi=atPerigee.phi(); ;
    PerigeeTrkParPhi=trkPar->momentum().phi();
    ATH_MSG_DEBUG("Rescale (phi, eta , pt, charge) ( " << trkPar->momentum().phi() << " , " << trkPar->momentum().eta() << " , " 
		  << trkPar->momentum().perp() <<" , " << trkPar->charge()<< ")");
    ATH_MSG_DEBUG("Before Rescale (phi, eta , pt,charge) ( " << trkPB->phi() <<" , " << trkPB->eta() <<" , " 
		  << trkPB->pt()<< " , " << trkPB->charge()<<")");
    delete trkPar;    
  }
  //GSF track Particles, extrapolate from perigee , using the egamma tool instance and the egamma dedicated cache.
  else if( trkPB->trackFitter() == xAOD::GaussianSumFilter && fromPerigee == extrapFrom){
    if(!m_perigeeParticleCaloExtensionTool->caloExtension(*trkPB,extension, m_useCaching)){
      ATH_MSG_INFO("Could not create an extension from perigee for a silicon GSF track with : "<< " Track Pt "
		      <<trkPB->pt()<< " Track Eta " << trkPB->eta()<<" Track Fitter " 
		      << trkPB->trackFitter() << " isTRT " << isTRT<<" Extrapolate From " <<  extrapFrom); 
      return StatusCode::SUCCESS;
    }
  }
  //GSF track Particles, from last measurement , the cache for GSF is used for the perigee so do not use it here 
  else if( trkPB->trackFitter() == xAOD::GaussianSumFilter && fromLastMeasurement == extrapFrom){
    if(!m_defaultParticleCaloExtensionTool->caloExtension(*trkPB,extension, false)){
      ATH_MSG_INFO("Could not create an extension from last measurement for a silicon GSF track with : "<< " Track Pt "
		      <<trkPB->pt()<< " Track Eta " << trkPB->eta()<<" Track Fitter " 
		      << trkPB->trackFitter() << " isTRT " << isTRT<<" Extrapolate From " <<  extrapFrom); 
      return StatusCode::SUCCESS;
    }
  }
  //Else track Particles before GSF, or failed GSF, or last measurement use the std tool/cache
  else {
    if(!m_defaultParticleCaloExtensionTool->caloExtension(*trkPB,extension,m_useCaching)){
      ATH_MSG_INFO("Could not create an extension from last measurement for a standard (non-GSF) track with : "<< " Track Pt "
		      <<trkPB->pt()<< " Track Eta " << trkPB->eta()<<" Track Fitter " 
		      << trkPB->trackFitter() << " isTRT " << isTRT<<" Extrapolate From " <<  extrapFrom); 
      return StatusCode::SUCCESS;
    }
  }
  //------------------------------------------------------------------------------------------------------------------------------------//
  if(!extension){
    ATH_MSG_INFO("Could not create an extension for "<< " Track Pt "
		    <<trkPB->pt()<< " Track Eta " << trkPB->eta()<<" Track Fitter " 
		    << trkPB->trackFitter() << " isTRT " << isTRT<<" Extrapolate From " <<  extrapFrom); 
    return StatusCode::SUCCESS;
  }
  //----------------------------------------------Calculate intersections------------------------------------------------------------------------------//
  //Layers to calculate intersections
  CaloExtensionHelpers::LayersToSelect layersToSelect;  
  if ( xAOD::EgammaHelpers::isBarrel( cluster )  ) {
    // Barrel
    layersToSelect.insert(CaloSampling::PreSamplerB );  
    layersToSelect.insert(CaloSampling::EMB1 );  
    layersToSelect.insert(CaloSampling::EMB2 );  
    layersToSelect.insert(CaloSampling::EMB3 );  
  } else {
    // Endcap
    layersToSelect.insert(CaloSampling::PreSamplerE );  
    layersToSelect.insert(CaloSampling::EME1 );  
    layersToSelect.insert(CaloSampling::EME2 );  
    layersToSelect.insert(CaloSampling::EME3 );        
  }

  CaloExtensionHelpers::EtaPhiPerLayerVector intersections;
  CaloExtensionHelpers::midPointEtaPhiPerLayerVector( *extension, intersections, &layersToSelect );
  std::vector<bool> hasBeenHit(4,false); 
  // Should we flip the sign for deltaPhi? 
  bool flipSign = false; 
  if(trkPB->charge() > 0) {
    flipSign = true; 
  }
  
  for( const auto& p : intersections ){    
    int  i(0); 
    auto sample = std::get<0>(p);
    if (sample == CaloSampling::PreSamplerE || sample == CaloSampling::PreSamplerB  ){
      i = 0;
      hasBeenHit[i] =true;
    } else if (  sample == CaloSampling::EME1 || sample == CaloSampling::EMB1 ){
      i = 1;
      hasBeenHit[i] =true;
    } else if ( sample == CaloSampling::EME2 || sample == CaloSampling::EMB2 ){
      i = 2;
      hasBeenHit[i] =true;
    } else if (  sample == CaloSampling::EME3 || sample == CaloSampling::EMB3) {
      i = 3;
      hasBeenHit[i] =true;
    } else {
      continue;
    }
      
    eta[i]      = std::get<1>(p); 
    phi[i]      = std::get<2>(p); 
    deltaEta[i] = cluster->etaBE(i) - std::get<1>(p); 
    deltaPhi[i] = P4Helpers::deltaPhi(cluster->phiBE(i),std::get<2>(p)); 
    // Should we flip the sign for deltaPhi? 
    if(flipSign)  {
      deltaPhi[i] = -deltaPhi[i];
    }
    
    ATH_MSG_DEBUG("getMatchAtCalo: i, eta, phi, deta, dphi: "  
		  << i << " " << eta[i] << " " << phi[i] << " " 
		  << deltaEta[i] << " " << deltaPhi[i]);     
    
    if (fromPerigeeRescaled == extrapFrom && !isTRT) {
      if ( i == 2 && deltaPhi.size() > 4) { 
	// For rescaled perigee when at sampling 2, save the phi   
	// rotation of the track. This is defined as: ((phi of point at 
	// sampling 2 - phi of point at track vertex/perigee) - phi of 
	// direction of track at perigee) 
	double perToSamp2 = std::get<2>(p)  - atPerigeePhi; 
	deltaPhi[4] = fabs(P4Helpers::deltaPhi(perToSamp2, PerigeeTrkParPhi)); 
	ATH_MSG_DEBUG("getMatchAtCalo: phi-rot: " << deltaPhi[4]); 
      }
    }
  }
  int max = deltaEta.size(); 
  for( int i(0); i<  max; ++i){
    if(!hasBeenHit[i]){
      ATH_MSG_DEBUG("Here : Surface " <<i<< " has not been  hit ! "<<" Track Pt "<<trkPB->pt()<< " Track Eta " << trkPB->eta()); 
    }
  }
  return StatusCode::SUCCESS;   
}

// =================================================================
// ======================= Photon/vertex Neutral track parameters Related Interfaces 

///////////////////////////////////////////////////////////////////////////////
//* Extrapolate double-track conversion with the neutral perigee
///////////////////////////////////////////////////////////////////////////////
bool
EMExtrapolationTools::matchesAtCalo(const xAOD::CaloCluster*           cluster, 
				    const Trk::NeutralParameters*      phPerigee, 
				    bool                               isTRT,
				    double&                            deltaEta,
				    double&                            deltaPhi) const 
{

  float etaAtCalo, phiAtCalo;
  Trk::PerigeeSurface surface( phPerigee->position() );
  const Trk::TrackParameters* trkPar = surface.createTrackParameters( phPerigee->position(), phPerigee->momentum().unit()*1e9,1,0);  
  if( !getEtaPhiAtCalo( trkPar, &etaAtCalo, &phiAtCalo) ){
    delete trkPar;
    return false;
  }
  delete trkPar;

  deltaEta = cluster->eta() - etaAtCalo;
  deltaPhi = P4Helpers::deltaPhi(cluster->phi(),phiAtCalo); 
  if ((!isTRT && (fabs(deltaEta) > m_broadDeltaEta) )
      || (isTRT && (fabs(deltaEta) > std::max(m_TRTbarrelDeltaEta, m_TRTendcapDeltaEta)) )         
      || fabs(deltaPhi) > m_broadDeltaPhi) {
    return false;
  }

  if ((isTRT && fabs(deltaPhi) < std::max(m_narrowDeltaPhiTRTbarrel, m_narrowDeltaPhiTRTendcap)) || 
      (!isTRT && fabs(deltaEta)< m_narrowDeltaEta && fabs(deltaPhi) < m_narrowDeltaPhi) ){
    return true;
  }
  return false;
}

bool EMExtrapolationTools::matchesAtCalo(const xAOD::CaloCluster* cluster,
					 const xAOD::Vertex *vertex,
					 float etaAtCalo,
					 float phiAtCalo) const
{
  if (!cluster || !vertex) return false;  
  float deltaEta = fabs(etaAtCalo - cluster->etaBE(2));
  float deltaPhi = fabs(P4Helpers::deltaPhi(cluster->phi(), phiAtCalo));
  
  int TRTsection = 0;
  if (xAOD::EgammaHelpers::numberOfSiTracks(vertex) == 0)
    TRTsection = getTRTsection(vertex->trackParticle(0));
  
  // First pass on TRT tracks, skip barrel tracks matching endcap clusters and vice-versa
  if((TRTsection==2 && (cluster->eta()<=0.6  || cluster->eta()>=2.4)   ) ||
     (TRTsection==-2 && (cluster->eta()>=-0.6 || cluster->eta()<=-2.4)  ) ||
     (TRTsection==1 && (cluster->eta()<=-0.1 || cluster->eta()>=1.3)   ) ||
     (TRTsection==-1 && (cluster->eta()>=0.1  || cluster->eta()<=-1.3)  )
     ) {
    return false;
  }
  
  // The maximum deltaEta/deltaPhi for Si, TRT barrel, TRT endcap
  std::vector<double> dEtaV{m_narrowDeltaEta, m_TRTbarrelDeltaEta, m_TRTendcapDeltaEta};
  std::vector<double> dPhiV{m_narrowDeltaPhi, m_narrowDeltaPhiTRTbarrel, m_narrowDeltaPhiTRTendcap};

  return (deltaEta < dEtaV[abs(TRTsection)] && deltaPhi < dPhiV[abs(TRTsection)]);
}


// =================================================================
bool EMExtrapolationTools::getEtaPhiAtCalo (const xAOD::Vertex* vertex, 
					    float *etaAtCalo,
					    float *phiAtCalo) const
{
  if (!vertex) return false;
  // Get the momentum at the vertex
  Amg::Vector3D momentum = getMomentumAtVertex(*vertex);
  // Protection against momentum = 0
  if (momentum.mag() < 1e-5){
    ATH_MSG_DEBUG("Intersection failed");
    return false;
  }
  Trk::PerigeeSurface surface (vertex->position());
  // Hack create high pt track parameters
  const Trk::TrackParameters* trkPar =  surface.createTrackParameters( vertex->position(), momentum.unit() *1.e9, +1, 0);
  bool success = getEtaPhiAtCalo( trkPar, etaAtCalo, phiAtCalo);
  delete trkPar;
  return success;   
}

// =================================================================
bool EMExtrapolationTools::getEtaPhiAtCalo (const Trk::TrackParameters* trkPar, 
					    float *etaAtCalo,
					    float *phiAtCalo) const{

  if (!trkPar) return false;
  CaloExtensionHelpers::LayersToSelect layersToSelect;    
  if ( fabs(trkPar->eta()) < 1.425) {
    // Barrel
    layersToSelect.insert(CaloSampling::EMB2 );  
  }else {
    // Endcap
    layersToSelect.insert(CaloSampling::EME2 );  
  }
  bool success = getHackEtaPhiAtCalo( trkPar, etaAtCalo, phiAtCalo, layersToSelect);
  return success;   
}
// =================================================================

bool EMExtrapolationTools::getHackEtaPhiAtCalo (const Trk::TrackParameters* trkPar, 
						float *etaAtCalo,
						float *phiAtCalo,
						CaloExtensionHelpers::LayersToSelect& layersToSelect
						) const

{
  const Trk::CaloExtension* extension = 0;      
  extension = m_perigeeParticleCaloExtensionTool->caloExtension( *trkPar, Trk::alongMomentum, Trk::muon );

  if(!extension){
    ATH_MSG_WARNING("Could not create an extension from getHackEtaPhiAtCalo ");
    return false;
  } 

  CaloExtensionHelpers::EtaPhiPerLayerVector intersections;
  CaloExtensionHelpers::midPointEtaPhiPerLayerVector( *extension, intersections, &layersToSelect );
 
  bool hitEM2(false);
  for( const auto& p : intersections ){    
    int  i(0); 
    auto sample = std::get<0>(p);
    if ( ( sample == CaloSampling::EME2 || sample == CaloSampling::EMB2 )){
      *etaAtCalo = std::get<1>(p); 
      *phiAtCalo = std::get<2>(p); 
      hitEM2=true;
      ++i;
      ATH_MSG_DEBUG("getMatchAtCalo: i, eta, phi : "  
		    << i << " " << std::get<1>(p) << " " << std::get<2>(p) ); 
      
    }
  }
  return hitEM2;   
}

// =================================================================
Amg::Vector3D EMExtrapolationTools::getMomentumAtVertex(const xAOD::Vertex& vertex, unsigned int index) const
{
  Amg::Vector3D momentum(0., 0., 0.);
  if (vertex.nTrackParticles() <= index){
    ATH_MSG_WARNING("Invalid track index");
  }
  else if (vertex.vxTrackAtVertexAvailable() && vertex.vxTrackAtVertex().size()){
    // Use the parameters at the vertex 
    // (the tracks should be parallel but we will do the sum anyway)
    ATH_MSG_DEBUG("getMomentumAtVertex : getting from vxTrackAtVertex");
    const auto& trkAtVertex = vertex.vxTrackAtVertex()[index];
    const Trk::TrackParameters* paramAtVertex = trkAtVertex.perigeeAtVertex();
    if (!paramAtVertex){
      ATH_MSG_WARNING("VxTrackAtVertex does not have perigee at vertex");
    }else{
      return paramAtVertex->momentum();
    }
  }
  else if (vertex.nTrackParticles() == 1){
    // Use the first measurement
    ATH_MSG_DEBUG("getMomentumAtVertex : 1 track only, getting from first measurement");
    const xAOD::TrackParticle *tp = vertex.trackParticle(0);
    unsigned int paramindex(0);
    if (!tp || !tp->indexOfParameterAtPosition(paramindex, xAOD::FirstMeasurement)){
      ATH_MSG_WARNING("No TrackParticle or no have first measurement");
    }
    else{
      momentum += tp->curvilinearParameters(paramindex).momentum();
    }
  }
  else{
    // Extrapolate track particle to vertex
    ATH_MSG_DEBUG("getMomentumAtVertex : extrapolating to perigee surface");
    const xAOD::TrackParticle* tp = vertex.trackParticle( index );
    if (!tp) {ATH_MSG_WARNING("NULL pointer to TrackParticle in vertex");}
    else{
      const Trk::PerigeeSurface *surface = new Trk::PerigeeSurface(vertex.position());
      const Trk::TrackParameters* params = m_extrapolator->extrapolate(*tp, *surface, Trk::alongMomentum);
      delete surface;
      if (!params) ATH_MSG_DEBUG("Extrapolation to vertex (perigee) failed");
      else momentum += params->momentum();
      delete params;
    }
  }
  return momentum;  
}
// =================================================================
Amg::Vector3D EMExtrapolationTools::getMomentumAtVertex(const xAOD::Vertex& vertex, bool reuse /* = true */) const
{
  Amg::Vector3D momentum(0., 0., 0.);
  const static SG::AuxElement::Accessor<float> accPx("px");
  const static SG::AuxElement::Accessor<float> accPy("py");
  const static SG::AuxElement::Accessor<float> accPz("pz");
  if (vertex.nTrackParticles() == 0){
    ATH_MSG_WARNING("getMomentumAtVertex : vertex has no track particles!");
    return momentum;
  } 
  if (reuse &&
      accPx.isAvailable(vertex) && 
      accPy.isAvailable(vertex) && 
      accPz.isAvailable(vertex) 
      ){
    // Already decorated with parameters at vertex
    ATH_MSG_DEBUG("getMomentumAtVertex : getting from auxdata");
    return Amg::Vector3D(accPx(vertex), 
			 accPy(vertex),  
			 accPz(vertex)); 
  }
  else{
    for (unsigned int i = 0; i < vertex.nTrackParticles(); ++i){
      momentum += getMomentumAtVertex(vertex, i);
    }
  }
  return momentum;	
}
// ============================================================================

// ======================= HELPERS==============================================
const Trk::TrackParameters* 
EMExtrapolationTools::getRescaledPerigee(const xAOD::TrackParticle* trkPB, const xAOD::CaloCluster* cluster) const {
  const Trk::TrackParameters* oldPerigee = &trkPB->perigeeParameters();
  if (!oldPerigee) return 0;
  Amg::Vector3D mom(oldPerigee->momentum().x(),oldPerigee->momentum().y(),oldPerigee->momentum().z());
  const double mag = mom.mag();
  if (mag == 0 || cluster->e() == 0) {
    // should this be a warning or debug? For now I assume that it should not happen
    ATH_MSG_WARNING("Trying to rescale perigee but cluster e = " << cluster->e() << " and perigee mag = " << mag);
    return 0;
  }
  //Same eta/phi
  double phi=mom.phi();
  double theta= mom.theta();
  // Rescale the momentum
  double charge = oldPerigee->charge();
  double qoverp = charge/cluster->e();
  // Create new perigee
  Trk::PerigeeSurface surface (oldPerigee->position());
  //This surface has the correct offset in x and y
  const Trk::TrackParameters* result  = surface.createParameters<5,Trk::Charged>(0,
										 0,
										 phi,
										 theta,
										 qoverp);
  return (result);
}

// =================================================================
int EMExtrapolationTools::getTRTsection(const xAOD::TrackParticle* trkPB) const{
  if (!trkPB){
    ATH_MSG_DEBUG("Null pointer to TrackParticle");
    return 0;
  } 
  if (!m_trtId){
    ATH_MSG_DEBUG("No trt ID guessing TRT section based on eta: " << trkPB->eta());
    return (trkPB->eta() > 0 ? 1 : -1) * (fabs(trkPB->eta()) < 0.6 ? 1 : 2);
  }
  const Trk::MeasurementBase* trkPar =0;  
  if( trkPB->trackLink().isValid() && trkPB->track() != 0 ) {
    ATH_MSG_DEBUG("Will get TrackParameters from Trk::Track");
    const DataVector<const Trk::TrackStateOnSurface> *trackStates = trkPB->track()->trackStateOnSurfaces();
    if (!trackStates) {
      ATH_MSG_WARNING("NULL pointer to trackStateOnSurfaces");
      return 0;
    }   
    //Loop over the TrkStateOnSurfaces search last valid TSOS first
    for ( DataVector<const Trk::TrackStateOnSurface>::const_reverse_iterator rItTSoS = trackStates->rbegin(); rItTSoS != trackStates->rend(); ++rItTSoS){
      if ( (*rItTSoS)->type(Trk::TrackStateOnSurface::Measurement) && !((*rItTSoS)->type(Trk::TrackStateOnSurface::Outlier)) && (*rItTSoS)->measurementOnTrack()!=0 
	   && !dynamic_cast<const Trk::PseudoMeasurementOnTrack*>((*rItTSoS)->measurementOnTrack())){
	trkPar = (*rItTSoS)->measurementOnTrack();
	break;
      }
    }
  }
  else {
    ATH_MSG_WARNING("Track particle without Trk::Track");
  }
  if (!trkPar) {
    return 0;
  }
  const Trk::Surface& sf = trkPar->associatedSurface();
  const Identifier tid = sf.associatedDetectorElementIdentifier();
  return m_trtId->barrel_ec(tid);
}
