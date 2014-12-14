/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


#include  "EMExtrapolationTools.h"
#include  "TrkEventPrimitives/PropDirection.h"
#include  "TrkParameters/TrackParameters.h"

//extrapolation
#include "RecoToolInterfaces/IParticleCaloExtensionTool.h"
#include "TrkExInterfaces/IExtrapolator.h"
#include "InDetIdentifier/TRT_ID.h"
#include "TrkNeutralParameters/NeutralParameters.h"


#include "xAODCaloEvent/CaloCluster.h"
#include "xAODTracking/TrackParticle.h"
#include "xAODTracking/Vertex.h"

#include "TrkTrack/Track.h"
#include "TrkPseudoMeasurementOnTrack/PseudoMeasurementOnTrack.h"

#include "xAODEgamma/EgammaxAODHelpers.h"
#include "TrkParametersIdentificationHelpers/TrackParametersIdHelper.h"
#include "TrkParametersIdentificationHelpers/TrackParametersIdentificationHelper.h"
#include "TrkExInterfaces/IExtrapolator.h"
#include "TrkTrack/TrackStateOnSurface.h"
#include <tuple>

EMExtrapolationTools::EMExtrapolationTools(const std::string& type, 
                                           const std::string& name,
                                           const IInterface* parent) :
  AthAlgTool(type, name, parent),
  m_defaultParticleCaloExtensionTool("Trk::ParticleCaloExtensionTool"),
  m_perigeeParticleCaloExtensionTool("Trk::ParticleCaloExtensionTool/EgammaParticleCaloExtensionTool"),
  m_extrapolator("Trk::Extrapolator")
{

  declareProperty( "DefaultCaloExtentionTool", m_defaultParticleCaloExtensionTool );
  declareProperty( "PerigeeCaloExtentionTool", m_perigeeParticleCaloExtensionTool );
  declareProperty( "Extrapolator", m_extrapolator);

  // Name of the utility for conversion
  
  declareProperty( "BroadDeltaEta",         m_broadDeltaEta      = 0.05);
  declareProperty( "BroadDeltaPhi",         m_broadDeltaPhi      = 0.10);
  declareProperty( "NarrowDeltaEta",        m_narrowDeltaEta     = 0.05);
  declareProperty( "NarrowDeltaPhi",        m_narrowDeltaPhi     = 0.05);
  declareProperty( "NarrowDeltaPhiBrem",    m_narrowDeltaPhiBrem = 0.10);
  declareProperty( "NarrowDeltaPhiTRTbarrel",     m_narrowDeltaPhiTRTbarrel  = 0.02);
  declareProperty( "NarrowDeltaPhiBremTRTbarrel", m_narrowDeltaPhiBremTRTbarrel = 0.03);  
  declareProperty( "NarrowDeltaPhiTRTendcap",     m_narrowDeltaPhiTRTendcap  = 0.02);
  declareProperty( "NarrowDeltaPhiBremTRTendcap", m_narrowDeltaPhiBremTRTendcap = 0.03);  
  declareProperty( "TRTbarrelDeltaEta",     m_TRTbarrelDeltaEta  = 0.35);
  declareProperty( "TRTendcapDeltaEta",     m_TRTendcapDeltaEta  = 0.2);
  
  declareProperty("useCaching", m_useCaching = true, "Use the a cache for track Particle extrapolation");
  declareProperty("useTrkTrackForTRT", m_useTrkTrackForTRT = true, "Use Trk::Track instead of TrackParticle to determine TRT section");
  declareProperty("guessTRTsection", m_guessTRTsection = false, "Guess TRT section from eta, instead of using track parameters");

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

  if ( m_extrapolator.retrieve().isFailure() ) {
    ATH_MSG_ERROR("Cannot retrieve extrapolator" << m_extrapolator );
    return StatusCode::FAILURE;
  }
  else {
    ATH_MSG_DEBUG("Retrieved extropolator tool " << m_extrapolator);
  }

  // retrieve TRT-ID helper
  m_trtId = 0;
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
  if (!m_trtId) m_guessTRTsection = true;

  
  return StatusCode::SUCCESS;
}

// =====================================================================
StatusCode EMExtrapolationTools::finalize() 
{
  return StatusCode::SUCCESS;
}

// ======================= Electron Track related interfaces

//////////////////////////////////////////////////////////////////////////////
//* Extrapolate TrackParticle to electron CaloCluster
//* Returns also selection
//* i.e performs the cuts
//////////////////////////////////////////////////////////////////////////////
bool
EMExtrapolationTools::matchesAtCalo(const xAOD::CaloCluster*      cluster,       
                                    const xAOD::TrackParticle*    trkPB, 
                                    bool                          isTRT,
                                    unsigned int                  extrapFrom)
{

  // Forward to move complex extrapolation
  std::vector<double>  eta(4, -999.0);
  std::vector<double>  phi(4, -999.0);
  std::vector<double>  deltaEta(4, -999.0);
  std::vector<double>  deltaPhi(4, -999.0);
  // extrapolate only to sampling 2
  return (matchesAtCalo(cluster, 
                        trkPB, 
                        isTRT, 
                        Trk::alongMomentum,
                        eta,
                        phi,
                        deltaEta,
                        deltaPhi,
                        extrapFrom) );
}


//////////////////////////////////////////////////////////////////////////////
//* Extrapolate TrackParticle to electron CaloCluster
//* Returns also selection
//* i.e performs the cuts
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
  int isTRTB = 0; // barrel or endcap TRT
  if(isTRT){
    if (!m_trtId) {
      ATH_MSG_WARNING("Should have m_trtId defined for isTRT");
      return false;
    }
    // Get last measurement for TRT check
    Trk::CurvilinearParameters temp = getLastMeasurement(trkPB);
    const Trk::TrackParameters*  trkParTRT = &temp;     
    const Trk::Surface& sf = trkParTRT->associatedSurface();
    const Identifier tid = sf.associatedDetectorElementIdentifier();
    isTRTB = m_trtId->barrel_ec(tid);
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
  // Selection in broad eta/phi window
  if ((isTRT && abs(isTRTB)==1 && fabs(deltaEta[iSampling]) > m_TRTbarrelDeltaEta)
      || (isTRT && abs(isTRTB)==2 && fabs(deltaEta[iSampling]) > m_TRTendcapDeltaEta)
      || (!isTRT && (fabs(deltaEta[iSampling]) > m_broadDeltaEta)) 
      || fabs(deltaPhi[iSampling]) > m_broadDeltaPhi) {
    return false;
  }
    
  // Selection in narrow eta/phi window
  if((isTRT && 
      ((abs(isTRTB)==1 && deltaPhi[iSampling] < m_narrowDeltaPhiTRTbarrel && 
        deltaPhi[iSampling] > -m_narrowDeltaPhiBremTRTbarrel) || 
       (abs(isTRTB)==2 && deltaPhi[iSampling] < m_narrowDeltaPhiTRTendcap && 
        deltaPhi[iSampling] > -m_narrowDeltaPhiBremTRTendcap)))
     || (!isTRT && fabs(deltaEta[iSampling]) < m_narrowDeltaEta && 
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
  // Checks
  //   require input vectors to be of the same size:

  ATH_MSG_DEBUG("getMatchAtCalo");
   
  // Should we flip the sign for deltaPhi? 
  bool flipSign = false; 
  if(trkPB->charge() > 0) flipSign = true; 

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
  
  const Trk::CaloExtension* extension = 0;      


  double atPerigeePhi(-999);
  double PerigeeTrkParPhi(-999);
  //TRT  only track. Could be in the standard or GSF container.
  //Extrapolate from last measurement and use caching (std cache container)
  if (isTRT){
    m_defaultParticleCaloExtensionTool->caloExtension(*trkPB,extension,m_useCaching);
  }
  //Perigee Rescaled
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
    delete trkPar;    
  }
  //GSF track Particles, from perigee , using the egamma cache
  else if( trkPB->trackFitter() == xAOD::GaussianSumFilter){
    m_perigeeParticleCaloExtensionTool->caloExtension(*trkPB,extension,m_useCaching);  
  }
  //Else track Particles beofre GSF or if failed GSF
  else {
    m_defaultParticleCaloExtensionTool->caloExtension(*trkPB,extension,m_useCaching);
  }

  if(!extension){
    ATH_MSG_WARNING("Could not create an extension for "<< " Track Pt "
		    <<trkPB->pt()<< " Track Eta " << trkPB->eta()<<" Track Fitter " 
		    << trkPB->trackFitter() << " isTRT " << isTRT<<" Extrapolate From " <<  extrapFrom); 
    return StatusCode::SUCCESS;
  }
  //////////////////////////////
  CaloExtensionHelpers::EtaPhiPerLayerVector intersections;
  CaloExtensionHelpers::midPointEtaPhiPerLayerVector( *extension, intersections, &layersToSelect );
  std::vector<bool> hasBeenHit(4,false); 
  for( const auto& p : intersections ){    
    int  i(0); 
    auto sample = std::get<0>(p);
    if ( ( sample == CaloSampling::PreSamplerE || sample == CaloSampling::PreSamplerB )  &&  deltaEta.size() > 0 ){
      i = 0;
      hasBeenHit[i] =true;
    } else if ( ( sample == CaloSampling::EME1 || sample == CaloSampling::EMB1 ) &&  deltaEta.size() > 1 ){
      i = 1;
      hasBeenHit[i] =true;
    } else if ( ( sample == CaloSampling::EME2 || sample == CaloSampling::EMB2 ) &&  deltaEta.size() > 2 ){
      i = 2;
      hasBeenHit[i] =true;
    } else if ( ( sample == CaloSampling::EME3 || sample == CaloSampling::EMB3 ) &&  deltaEta.size() > 3) {
      i = 3;
      hasBeenHit[i] =true;
    } else {
      continue;
    }
    eta[i]      = std::get<1>(p); 
    phi[i]      = std::get<2>(p); 
    deltaEta[i] = cluster->etaBE(i) - std::get<1>(p); 
    deltaPhi[i] = m_phiHelper.diff(cluster->phiBE(i),std::get<2>(p)); 
    if(flipSign)  deltaPhi[i] = -deltaPhi[i]; 

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
	deltaPhi[4] = fabs(m_phiHelper.diff(perToSamp2, PerigeeTrkParPhi)); 
	ATH_MSG_DEBUG("getMatchAtCalo: phi-rot: " << deltaPhi[4]); 
      }
    }
  }
  bool passall(true);
  int max = deltaEta.size(); 
  for( int i(0); i<  max; ++i){
    passall = passall && hasBeenHit[i];
    if(!hasBeenHit[i]){
      ATH_MSG_DEBUG("Surface " <<i<< " has not been  hit ! "<<" Track Pt "<<trkPB->pt()<< " Track Eta " << trkPB->eta()); 
    }
  }
  return StatusCode::SUCCESS;   
}

// =================================================================

// ======================= Photon/vertex Related Interfaces 

///////////////////////////////////////////////////////////////////////////////
//* Extrapolate double-track conversion with the neutral perigee
///////////////////////////////////////////////////////////////////////////////
bool
EMExtrapolationTools::matchesAtCalo(const xAOD::CaloCluster*           cluster, 
                                    const Trk::NeutralParameters*      phPerigee, 
                                    bool                               isTRT,
                                    double&                            deltaEta,
                                    double&                            deltaPhi) 
{

  float etaAtCalo, phiAtCalo;
  // Hack to extrpolate neutrals for now;
  
  Trk::PerigeeSurface surface( phPerigee->position() );
  const Trk::TrackParameters* trkPar = surface.createTrackParameters( phPerigee->position(), phPerigee->momentum().unit()*1e9,1,0);  

  CaloExtensionHelpers::LayersToSelect layersToSelect;    
  if ( xAOD::EgammaHelpers::isBarrel( cluster )  ) {
    // Barrel
    layersToSelect.insert(CaloSampling::EMB2 );  
  } else {
    // Endcap
    layersToSelect.insert(CaloSampling::EME2 );  
  }

  if( !getHackEtaPhiAtCalo( trkPar, &etaAtCalo, &phiAtCalo , layersToSelect) ){
    delete trkPar;
    return false;
  }

  delete trkPar;

  deltaEta = cluster->eta() - etaAtCalo;
  deltaPhi = m_phiHelper.diff(cluster->phi(),phiAtCalo); 

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
  float deltaPhi = fabs(m_phiHelper.diff(cluster->phi(), phiAtCalo));
  
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
  
  CaloExtensionHelpers::LayersToSelect layersToSelect;    
  if ( fabs(momentum.eta()) < 1.425) {
    // Barrel
    layersToSelect.insert(CaloSampling::EMB2 );  
  } else {
    // Endcap
    layersToSelect.insert(CaloSampling::EME2 );  
  }

  // Protection against momentum = 0
  if (momentum.mag() < 1e-5)
    {
      ATH_MSG_DEBUG("Intersection failed");
      return false;
    }
  
  Trk::PerigeeSurface surface (vertex->position());
  // Hack create high pt track parameters
  const Trk::TrackParameters* trkPar =  surface.createTrackParameters( vertex->position(), momentum.unit() *1.e9, +1, 0);
  bool success = getHackEtaPhiAtCalo( trkPar, etaAtCalo, phiAtCalo, layersToSelect);

  delete trkPar;

  return success;   
}
//////////////////////////////////////////////////////////////////////


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
Amg::Vector3D EMExtrapolationTools::getMomentumAtVertex(const xAOD::Vertex& vertex, bool reuse /* = true */) const
{
  
  Amg::Vector3D momentum(0., 0., 0.);  

  static SG::AuxElement::Accessor<float> accPx("px");
  static SG::AuxElement::Accessor<float> accPy("py");
  static SG::AuxElement::Accessor<float> accPz("pz");
  
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
  else if (vertex.vxTrackAtVertexAvailable() && vertex.vxTrackAtVertex().size()){
    // Use the parameters at the vertex 
      // (the tracks should be parallel but we will do the sum anyway)
    ATH_MSG_DEBUG("getMomentumAtVertex : getting from vxTrackAtVertex");
    for (const auto& trkAtVertex : vertex.vxTrackAtVertex()){
      const Trk::TrackParameters* paramAtVertex = trkAtVertex.perigeeAtVertex();
      if (!paramAtVertex)
	ATH_MSG_WARNING("VxTrackAtVertex does not have perigee at vertex");
      else
	momentum += paramAtVertex->momentum();
    }
  }
  else if (vertex.nTrackParticles() == 1){
    // Use the first measurement
    ATH_MSG_DEBUG("getMomentumAtVertex : 1 track only, getting from first measurement");
    const xAOD::TrackParticle *tp = vertex.trackParticle(0);
    unsigned int index(0);
    if (!tp || !tp->indexOfParameterAtPosition(index, xAOD::FirstMeasurement)){
      ATH_MSG_WARNING("No TrackParticle or no have first measurement");
    }
      else
	momentum += tp->curvilinearParameters(index).momentum();
    // OR last 3 values of trackParameters(index)
  }
  else{
    // Extrapolate track particles to vertex
    // (the tracks should be parallel but we will do the sum anyway)
    ATH_MSG_DEBUG("getMomentumAtVertex : extrapolating to perigee surface");    
    const Trk::PerigeeSurface *surface = new Trk::PerigeeSurface(vertex.position());
    for (unsigned int i = 0; i < vertex.nTrackParticles(); ++i){
      const xAOD::TrackParticle* tp = vertex.trackParticle( i );
      if (!tp){
	ATH_MSG_WARNING("NULL pointer to TrackParticle in vertex");
	continue;
      }
      const Trk::TrackParameters* params = m_extrapolator->extrapolate(*tp, *surface, Trk::alongMomentum);
      if (!params)
	ATH_MSG_DEBUG("Extrapolation to vertex (perigee) failed");
      else{
	momentum += params->momentum();
	delete params;
      }
    }
      delete surface;
  }
  
  return momentum;	
}
// ========================================================================================================================

// ======================= HELPERS
const Trk::TrackParameters* 
EMExtrapolationTools::getRescaledPerigee(const xAOD::TrackParticle* trkPB, const xAOD::CaloCluster* cluster) const {
  const Trk::TrackParameters* oldPerigee = &trkPB->perigeeParameters();
  if (!oldPerigee) return 0;
  Amg::Vector3D  mom(oldPerigee->momentum().x(),oldPerigee->momentum().y(),oldPerigee->momentum().z());
  const double mag = mom.mag();
  if (mag == 0 || cluster->e() == 0) {
    // should this be a warning or debug? For now I assume that it should not happen
    ATH_MSG_WARNING("Trying to rescale perigee but cluster e = " << cluster->e() << " and perigee mag = " << mag);
    return 0;
  }
    
  // rescale the momentum
  mom = mom * Eigen::Scaling(cluster->e()/mom.mag());

  // Create new perigee
  double charge = oldPerigee->charge();
  //Construct the photon neutral parameters as neutral perigee

  Amg::Vector3D pos(oldPerigee->position().x(),oldPerigee->position().y(),oldPerigee->position().z()); 
  Trk::PerigeeSurface surface (pos); 
  AmgSymMatrix(5) *covariance = new AmgSymMatrix(5)(*(oldPerigee->covariance()));
  const Trk::TrackParameters* result 
    = surface.createParameters<5,Trk::Charged>(oldPerigee->parameters()[0],
					       oldPerigee->parameters()[1],
					       mom.phi(),
					       mom.theta(),
					       charge/mom.mag(),
					       covariance);
  return (result);
}

Trk::CurvilinearParameters EMExtrapolationTools::getLastMeasurement(const xAOD::TrackParticle* trkPB) const{
  // Get last measurement for TRT check
  Trk::CurvilinearParameters temp;
  //For TRT it is always the last
  int index(-1);
  for(unsigned int i(0); i< trkPB->numberOfParameters() ; ++i ){
    if( xAOD::LastMeasurement == trkPB->parameterPosition(i) ){
      index = i;
      break;
    }
  }
  if(index!=-1){
    temp= trkPB->curvilinearParameters(index);
  } else {
    ATH_MSG_WARNING("Track Particle does not contain Last Measurement track parameters");
  }
  return temp;
}
// =================================================================
int EMExtrapolationTools::getTRTsection(const xAOD::TrackParticle* trkPB) const
{
  if (!trkPB){
      ATH_MSG_DEBUG("Null pointer to TrackParticle");
      return 0;
  }
  
  if (!m_trtId || m_guessTRTsection){
      ATH_MSG_DEBUG("Guessing TRT section based on eta: " << trkPB->eta());
      return (trkPB->eta() > 0 ? 1 : -1) * (abs(trkPB->eta()) < 0.6 ? 1 : 2);
  }
  
  const Trk::TrackParameters* trkPar =0;
  Trk::CurvilinearParameters temp;
  if (!m_useTrkTrackForTRT){
    temp= getLastMeasurement(trkPB);
    trkPar = &temp;
  }
  else if( trkPB->trackLink().isValid() && trkPB->track() != 0 ) {
    ATH_MSG_DEBUG("Will get TrackParameters from Trk::Track");
    const DataVector<const Trk::TrackStateOnSurface> *trackStates = trkPB->track()->trackStateOnSurfaces();
    if (!trackStates) {
      ATH_MSG_WARNING("NULL pointer to trackStateOnSurfaces");
      return 0;
    }    
    //Loop over the TrkStateOnSurfaces
      // search last valid TSOS first
    for ( DataVector<const Trk::TrackStateOnSurface>::const_reverse_iterator rItTSoS = trackStates->rbegin(); rItTSoS != trackStates->rend(); ++rItTSoS)
      {
	if ( (*rItTSoS)->type(Trk::TrackStateOnSurface::Measurement) && (*rItTSoS)->trackParameters()!=0 && (*rItTSoS)->measurementOnTrack()!=0 
	     && !dynamic_cast<const Trk::PseudoMeasurementOnTrack*>((*rItTSoS)->measurementOnTrack()))
	  {
	    trkPar = (*rItTSoS)->trackParameters();
	    break;
	  }
      }
  }
  else {ATH_MSG_WARNING("Track particle without Trk::Track");}
  if (!trkPar) return 0;
  const Trk::Surface& sf = trkPar->associatedSurface();
  const Identifier tid = sf.associatedDetectorElementIdentifier();
  return m_trtId->barrel_ec(tid);
}
