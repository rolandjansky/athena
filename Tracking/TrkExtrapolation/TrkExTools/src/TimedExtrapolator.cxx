/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// TimedExtrapolator.cxx, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/PhysicalConstants.h"
// Trk include
#include "TrkExTools/TimedExtrapolator.h"
#include "TrkExInterfaces/IPropagator.h"
#include "TrkExInterfaces/IMultipleScatteringUpdator.h"
#include "TrkExInterfaces/IEnergyLossUpdator.h"
#include "TrkExUtils/IntersectionSolution.h"
#include "TrkSurfaces/SurfaceBounds.h"
#include "TrkSurfaces/DiscBounds.h"
#include "TrkSurfaces/PerigeeSurface.h"
#include "TrkSurfaces/StraightLineSurface.h"
#include "TrkSurfaces/CylinderSurface.h"
#include "TrkTrack/Track.h"
#include "TrkGeometry/EntryLayerProvider.h"
#include "TrkGeometry/DetachedTrackingVolume.h"
#include "TrkGeometry/Layer.h"
#include "TrkGeometry/CompoundLayer.h"
#include "TrkGeometry/CylinderLayer.h"
#include "TrkGeometry/SubtractedCylinderLayer.h"
#include "TrkGeometry/TrackingGeometry.h"
#include "TrkGeometry/MaterialProperties.h"
#include "TrkVolumes/BoundarySurface.h"
#include "TrkVolumes/BoundarySurfaceFace.h"
#include "TrkVolumes/CylinderVolumeBounds.h"
#include "TrkVolumes/Volume.h"
#include "TrkParticleBase/TrackParticleBase.h"
#include "TrkEventUtils/TrkParametersComparisonFunction.h"
#include "TrkDetDescrInterfaces/IDynamicLayerCreator.h"
#include "TrkDetDescrUtils/SharedObject.h"
#include "TrkDetDescrUtils/GeometrySignature.h"
#include "TrkMaterialOnTrack/EnergyLoss.h"
#include "TrkMaterialOnTrack/ScatteringAngles.h"
#include "TrkParameters/TrackParameters.h"
#include "TrkGeometry/MagneticFieldProperties.h"
// for the comparison with a pointer
#include <stdint.h>

// Amg
#include "EventPrimitives/EventPrimitives.h"
#include "GeoPrimitives/GeoPrimitives.h"

// Trk
#include "TrkSurfaces/PlaneSurface.h"

Trk::ParticleMasses Trk::TimedExtrapolator::s_particleMasses;

// constructor
Trk::TimedExtrapolator::TimedExtrapolator(const std::string& t, const std::string& n, const IInterface* p) :
  AthAlgTool(t,n,p),
  m_propagators(),
  m_stepPropagator("Trk::STEP_Propagator/AtlasSTEP_Propagator"), 
  m_navigator("Trk::Navigator/AtlasNavigator"), 
  m_updators(),
  m_msupdators(),
  m_elossupdators(),
  //m_dynamicLayerCreator(),
  m_subPropagators(Trk::NumberOfSignatures),
  m_subUpdators(Trk::NumberOfSignatures),
  m_propNames(),
  m_updatNames(),
  m_meotpIndex(0),
  m_configurationLevel(10),
  m_searchLevel(10),
  m_includeMaterialEffects(true),
  m_requireMaterialDestinationHit(false),
  m_stopWithNavigationBreak(false),
  m_stopWithUpdateZero(false),
  m_subSurfaceLevel(true),
  m_skipInitialLayerUpdate(false),
  m_referenceMaterial(false),
  m_extendedLayerSearch(true), 
  m_initialLayerAttempts(3),
  m_successiveLayerAttempts(1),
  m_tolerance(0.002),  
  m_caloMsSecondary(true),  
  m_activeOverlap(false),
  m_useDenseVolumeDescription(true),
  m_useMuonMatApprox(false),
  m_checkForCompundLayers(false),
  m_destinationSurface(0),
  m_boundaryVolume(0),
  m_recall(false),
  m_recallSurface(0),
  m_recallLayer(0),
  m_recallTrackingVolume(0),
  m_hitVector(0),
  m_lastValidParameters(0),
  m_currentStatic(0),
  m_currentDense(0),
  m_highestVolume(0),
  m_resolveActive(false),
  m_resolveMultilayers(true),
  m_muonEntrance(0),
  m_returnPassiveLayers(false),
  m_robustSampling(true),
  m_path(PathLimit(0.,0)),
  m_methodSequence(0),
  m_maxMethodSequence(2000),
  m_printHelpOutputAtInitialize(false),
  m_printRzOutput(true),
  m_extrapolateCalls(0),
  m_extrapolateBlindlyCalls(0),
  m_extrapolateDirectlyCalls(0),
  m_extrapolateStepwiseCalls(0),
  m_startThroughAssociation(0),        
  m_startThroughRecall(0),         
  m_startThroughGlobalSearch(0),       
  m_destinationThroughAssociation(0),  
  m_destinationThroughRecall(0),       
  m_destinationThroughGlobalSearch(0), 
  m_layerSwitched(0),
  m_navigationStatistics(false),
  m_navigationBreakDetails(false),
  m_navigationBreakLoop(0),
  m_navigationBreakOscillation(0),
  m_navigationBreakNoVolume(0),
  m_navigationBreakDistIncrease(0),
  m_navigationBreakVolumeSignature(0),
  m_overlapSurfaceHit(0),
  m_materialEffectsOnTrackValidation(false),
  m_meotSearchCallsFw(0),
  m_meotSearchCallsBw(0),
  m_meotSearchSuccessfulFw(0),  
  m_meotSearchSuccessfulBw(0),
  m_lastMaterialLayer(0),
  m_cacheLastMatLayer(false),
  m_particleMass(0.),
  m_fastField(false)
{
      declareInterface<ITimedExtrapolator>(this);
      
      // extrapolation steering
      declareProperty("StopWithNavigationBreak",        m_stopWithNavigationBreak);
      declareProperty("StopWithUpdateKill",             m_stopWithUpdateZero);      
      declareProperty("SkipInitialPostUpdate",          m_skipInitialLayerUpdate);
      declareProperty("MaximalMethodSequence",          m_maxMethodSequence);       
      // propagation steering
      declareProperty("Propagators",                    m_propagators);
      declareProperty("SubPropagators",                 m_propNames);
      declareProperty("STEP_Propagator",                m_stepPropagator);
      // material effects handling
      declareProperty("ApplyMaterialEffects",           m_includeMaterialEffects);      
      declareProperty("RequireMaterialDestinationHit",  m_requireMaterialDestinationHit);
      declareProperty("MaterialEffectsUpdators",        m_updators);  
      declareProperty("MultipleScatteringUpdators",     m_msupdators);  
      declareProperty("EnergyLossUpdators",             m_elossupdators);  
      declareProperty("SubMEUpdators",                  m_updatNames);  
      declareProperty("CacheLastMaterialLayer",         m_cacheLastMatLayer);
      // general behavior navigation      
      declareProperty("SearchLevelClosestParameters",   m_searchLevel);
      declareProperty("Navigator",                      m_navigator);
      declareProperty("UseDenseVolumeDescription",      m_useDenseVolumeDescription);
      // muon system specifics            
      declareProperty("UseMuonMatApproximation",        m_useMuonMatApprox);
      declareProperty("CheckForCompoundLayers",         m_checkForCompundLayers);
      declareProperty("ResolveMuonStation",             m_resolveActive);
      declareProperty("ResolveMultilayers",             m_resolveMultilayers);
      declareProperty("ConsiderMuonStationOverlaps",    m_activeOverlap);  
      //declareProperty("DynamicLayerCreator",          m_dynamicLayerCreator);  
      declareProperty("RobustSampling",                 m_robustSampling);  
      // material & navigation related steering
      declareProperty("MaterialEffectsOnTrackProviderIndex" , m_meotpIndex);  
      declareProperty("MaterialEffectsOnTrackValidation"    , m_materialEffectsOnTrackValidation);
      declareProperty("ReferenceMaterial"                   , m_referenceMaterial);
      declareProperty("ExtendedLayerSearch"                 , m_extendedLayerSearch);
      declareProperty("InitialLayerAttempts"                , m_initialLayerAttempts);
      declareProperty("SuccessiveLayerAttempts"             , m_successiveLayerAttempts);
      // debug and validation
      declareProperty("HelpOutput",                     m_printHelpOutputAtInitialize);
      declareProperty("positionOutput",                 m_printRzOutput);
      declareProperty("NavigationStatisticsOutput",     m_navigationStatistics);
      declareProperty("DetailedNavigationOutput",       m_navigationBreakDetails);
      declareProperty("Tolerance",                      m_tolerance);
      declareProperty("CaloMsSecondary",                m_caloMsSecondary);
      //Magnetic field properties
      declareProperty("MagneticFieldProperties",	m_fastField);  
}

// destructor
Trk::TimedExtrapolator::~TimedExtrapolator()
{}

// Athena standard methods
// initialize
StatusCode Trk::TimedExtrapolator::initialize()
{   
    m_fieldProperties = m_fastField ? Trk::MagneticFieldProperties(Trk::FastField) : Trk::MagneticFieldProperties(Trk::FullField);
    if(m_propagators.size() == 0) m_propagators.push_back("Trk::RungeKuttaPropagator/DefaultPropagator");
    if(m_updators.size() == 0) m_updators.push_back("Trk::MaterialEffectsUpdator/DefaultMaterialEffectsUpdator");
    if(m_msupdators.size() == 0) m_msupdators.push_back("Trk::MultipleScatteringUpdator/AtlasMultipleScatteringUpdator");
    if(m_elossupdators.size() == 0) m_elossupdators.push_back("Trk::EnergyLossUpdator/AtlasEnergyLossUpdator");
    
    if (m_propagators.size()) {
      if ( m_propagators.retrieve().isFailure() ) {
        ATH_MSG_FATAL( "Failed to retrieve tool " << m_propagators );
        return StatusCode::FAILURE;
      } else 
        ATH_MSG_INFO( "Retrieved tools " << m_propagators );      
    }

     
     // from the number of retrieved propagators set the configurationLevel
     unsigned int validprop = m_propagators.size();
     
     if (!validprop){
        ATH_MSG_WARNING( "None of the defined propagators could be retrieved!" );
        ATH_MSG_WARNING( "  Extrapolators jumps back in unconfigured mode, only strategy pattern methods can be used." ); 
      } else {
        m_configurationLevel = validprop - 1;
        ATH_MSG_VERBOSE( "Configuration level automatically set to " << m_configurationLevel );        
      }
    
    // Get the Navigation AlgTools
    if ( m_navigator.retrieve().isFailure() ) {
      ATH_MSG_FATAL( "Failed to retrieve tool " << m_navigator );
      return StatusCode::FAILURE;
    } else {
      ATH_MSG_INFO( "Retrieved tool " << m_navigator );
    }
    // Get the Material Updator
    if (m_includeMaterialEffects && m_updators.size()){
      if ( m_updators.retrieve().isFailure() ) {
        ATH_MSG_FATAL( "None of the defined material updatros could be retrieved!" );
        ATH_MSG_FATAL( "No multiple scattering and energy loss material update will be done."        );
        return StatusCode::FAILURE;
      } else 
        ATH_MSG_INFO( "Retrieved tools: " << m_updators );
    }

    // from the number of retrieved propagators set the configurationLevel
    unsigned int validmeuts = m_updators.size();
   
    // -----------------------------------------------------------
    // Sanity check 1
    
    if (!m_propNames.size() && m_propagators.size()){
       ATH_MSG_DEBUG( "Inconsistent setup of Extrapolator, no sub-propagators configured, doing it for you. " );
       m_propNames.push_back(m_propagators[0]->name().substr( 8, m_propagators[0]->name().size()-8 ));
    }
    
    if (!m_updatNames.size() && m_updators.size()){
       ATH_MSG_DEBUG( "Inconsistent setup of Extrapolator, no sub-materialupdators configured, doing it for you. " );
       m_updatNames.push_back(m_updators[0]->name().substr( 8, m_updators[0]->name().size()-8 ));
    }

    // -----------------------------------------------------------
    // Sanity check 2
    // fill the number of propagator names and updator names up with first one
    while ( int(m_propNames.size()) < int(Trk::NumberOfSignatures)) 
         m_propNames.push_back(m_propNames[0]); 
    while  (int(m_updatNames.size()) < int(Trk::NumberOfSignatures)) 
         m_updatNames.push_back(m_updatNames[0]); 
    if (validprop && validmeuts){
      // Per definition: if configured not found, take the lowest one
      for (unsigned int isign=0; int(isign) < int(Trk::NumberOfSignatures); ++isign){
                unsigned int index = 0;
           
          for (unsigned int iProp = 0; iProp < m_propagators.size(); iProp++) {
                 std::string pname = m_propagators[iProp]->name().substr( 8, m_propagators[iProp]->name().size()-8 );
            if ( m_propNames[isign] == pname ) index = iProp; 
              }
             ATH_MSG_DEBUG( " subPropagator:" << isign << " pointing to propagator: "<< m_propagators[index]->name() );
             m_subPropagators[isign] =  ( index < validprop ) ? &(*m_propagators[index]) : &(*m_propagators[Trk::Global]);     

          index = 0; 
          for (unsigned int iUp = 0; iUp < m_updators.size(); iUp++) {
          std::string uname = m_updators[iUp]->name().substr( 8, m_updators[iUp]->name().size()-8 );
          if ( m_updatNames[isign] == uname ) index = iUp; 
             }
            ATH_MSG_DEBUG( " subMEUpdator:" << isign << " pointing to updator: "<< m_updators[index]->name() );
            m_subUpdators[isign] =  ( index < validmeuts ) ? &(*m_updators[index]) : &(*m_updators[Trk::Global]);     
     }
   } else {
      ATH_MSG_FATAL( "Configuration Problem of Extrapolator: "                        
          << "  -- At least one IPropagator and IMaterialUpdator instance have to be given.! " );
   
   }
            
    /*
    // Get the DynamicLayerCreator
    if(m_doMuonDynamic && m_dynamicLayerCreator){
      if ( m_dynamicLayerCreator.retrieve().isFailure() ) {
        ATH_MSG_ERROR( "Failed to retrieve tool " << m_dynamicLayerCreator 
            << "No multiple scattering and energy loss material update will be done." );
        return StatusCode::SUCCESS;
      } else 
        ATH_MSG_DEBUG( "Retrieved tool " << m_dynamicLayerCreator );      
    }
    */

    m_maxNavigSurf = 1000;
    m_navigSurfs.reserve(m_maxNavigSurf); 
    m_maxNavigVol = 50;
    m_navigVols.reserve(m_maxNavigVol); 
    m_navigVolsInt.reserve(m_maxNavigVol); 

       
    ATH_MSG_INFO("initialize() successful" );    
    return StatusCode::SUCCESS;
}

// finalize
StatusCode Trk::TimedExtrapolator::finalize()
{
  ATH_MSG_INFO( "finalize() successful" );
  return StatusCode::SUCCESS;
}



const Trk::TrackParameters*  Trk::TimedExtrapolator::extrapolateWithPathLimit(
									 const Trk::TrackParameters& parm,
									 Trk::PathLimit& pathLim, Trk::TimeLimit& timeLim,
									 Trk::PropDirection dir,
									 Trk::ParticleHypothesis particle,
									 std::vector<Trk::HitInfo>*& hitInfo,
                                                                         Trk::GeometrySignature& nextGeoID,  
									 const Trk::TrackingVolume* boundaryVol) const
{
// extrapolation method intended for simulation of particle decay; collects intersections with active layers
// possible outcomes:1/ returns curvilinear parameters after reaching the maximal path
//                   2/ returns parameters at destination volume boundary 
//                   3/ returns 0 ( particle stopped ) but keeps vector of hits    

  ATH_MSG_DEBUG( "M-[" << ++m_methodSequence << "] extrapolateWithPathLimit(...) " << pathLim.x0Max << ", from " << parm.position());
  ATH_MSG_DEBUG( "M-[" << ++m_methodSequence << "] extrapolateWithPathLimit(...): resolve active layers? " << m_resolveActive);
 
  if (!m_stepPropagator) {
    // Get the STEP_Propagator AlgTool
    if ( m_stepPropagator.retrieve().isFailure() ) {
      ATH_MSG_ERROR( "Failed to retrieve tool " << m_stepPropagator );
      ATH_MSG_ERROR("Configure STEP Propagator for extrapolation with path limit");
      return 0;
    } else {
      ATH_MSG_INFO( "Retrieved tool " << m_stepPropagator );
    }
  }

  // reset the path ( in x0 !!) 
  m_path = PathLimit( pathLim.x0Max - pathLim.x0Collected, pathLim.process);     // collect material locally

  // initialize hit vector
  m_hitVector = hitInfo;
 
  // if no input volume, define as highest volume
  //const Trk::TrackingVolume* destVolume = boundaryVol ? boundaryVol : m_navigator->highestVolume();
  m_currentStatic = 0;
  if (boundaryVol && !boundaryVol->inside(parm.position(),m_tolerance) ) return 0;
   
  // extrapolate to destination volume boundary with path limit
  const Trk::TrackParameters* returnParms = extrapolateToVolumeWithPathLimit( parm, timeLim, dir,particle, nextGeoID, boundaryVol);

  //save actual path on output  
  if (m_path.x0Collected>0.)  pathLim.updateMat(m_path.x0Collected, m_path.weightedZ/m_path.x0Collected, m_path.l0Collected);
   
  return returnParms;
} 
   
const Trk::TrackParameters*  Trk::TimedExtrapolator::extrapolateToVolumeWithPathLimit(
									 const Trk::TrackParameters& parm,
									 Trk::TimeLimit& timeLim,
									 Trk::PropDirection dir,
									 Trk::ParticleHypothesis particle,
                                                                         Trk::GeometrySignature& nextGeoID,
									 const Trk::TrackingVolume* destVol) const
{ 
  // returns:
  //    A)  curvilinear track parameters if path limit reached 
  //    B)  boundary parameters (at destination volume boundary)

  // initialize the return parameters vector
  const Trk::TrackParameters* returnParameters = 0;
  const Trk::TrackParameters* currPar = &parm;
  const Trk::TrackingVolume*  currVol = 0;
  const Trk::TrackingVolume*  nextVol = 0;
  std::vector<unsigned int> solutions;
  const Trk::TrackingVolume* assocVol = 0;  
  int                        nEntryLays = 0;
  unsigned int               iDest = 0;
  
  // destination volume boundary ? 
  if ( destVol && m_navigator->atVolumeBoundary(currPar,destVol,dir,nextVol,m_tolerance) && nextVol != destVol ) { return &parm; }
  
  bool resolveActive = true;
  if ( m_lastMaterialLayer && !m_lastMaterialLayer->isOnLayer(parm.position()) ) m_lastMaterialLayer = 0;  
  if (!m_highestVolume ) m_highestVolume = m_navigator->highestVolume();

  emptyGarbageBin(&parm);
  // navigation surfaces
  if( m_navigSurfs.capacity() > m_maxNavigSurf ) m_navigSurfs.reserve(m_maxNavigSurf); 
  m_navigSurfs.clear();

  // target volume may not be part of tracking geometry 
  if (destVol) { 
    const Trk::TrackingVolume* tgVol =  m_navigator->trackingGeometry()->trackingVolume(destVol->volumeName());
    if (!tgVol || tgVol!=destVol) {
      const std::vector< SharedObject<const BoundarySurface<TrackingVolume> > > bounds = destVol->boundarySurfaces();
      for (unsigned int ib=0; ib< bounds.size(); ib++ ){
	const Trk::Surface& surf = (bounds[ib].getPtr())->surfaceRepresentation();
	m_navigSurfs.push_back(std::pair<const Trk::Surface*,Trk::BoundaryCheck>(&surf,true));
      }
      iDest = bounds.size();
    }		   
  }
    
  // resolve current position
  bool updateStatic = false;
  Amg::Vector3D gp = parm.position();
  if ( !m_currentStatic || !m_currentStatic->inside(gp,m_tolerance) ) {
    m_currentStatic =  m_navigator->trackingGeometry()->lowestStaticTrackingVolume(gp);   
    updateStatic = true; 
  }
  if ( m_navigator->atVolumeBoundary(currPar,m_currentStatic,dir,nextVol,m_tolerance) && nextVol != m_currentStatic ) {
    // no next volume found --- end of the world
    if ( !nextVol ) {
      ATH_MSG_DEBUG( "  [+] Word boundary reached        - at " << positionOutput(currPar->position()) );
      nextGeoID = Trk::GeometrySignature(Trk::Unsigned);
      return currPar;
    }
    m_currentStatic = nextVol;
    updateStatic = true;
  }

  // current frame volume known-retrieve geoID
  nextGeoID = m_currentStatic->geometrySignature(); 

  // update if new static volume
  if ( updateStatic ) {    // retrieve boundaries
    m_staticBoundaries.clear();
    const std::vector< SharedObject<const BoundarySurface<TrackingVolume> > > bounds = m_currentStatic->boundarySurfaces();
    for (unsigned int ib=0; ib< bounds.size(); ib++ ){
      const Trk::Surface& surf = (bounds[ib].getPtr())->surfaceRepresentation();
      m_staticBoundaries.push_back(std::pair<const Trk::Surface*,Trk::BoundaryCheck>(&surf,true));
    }

    m_detachedVols.clear();
    m_detachedBoundaries.clear();
    m_denseVols.clear();
    m_denseBoundaries.clear();
    m_layers.clear();
    m_navigLays.clear();

    // new: ID volumes may have special material layers ( entry layers ) - add them here
    if (m_currentStatic->entryLayerProvider()) {
      const std::vector<const Trk::Layer*>& entryLays = m_currentStatic->entryLayerProvider()->layers();
      for (unsigned int i=0; i < entryLays.size(); i++) { 
	if (entryLays[i]->layerType()>0 || entryLays[i]->layerMaterialProperties()) {
	  m_layers.push_back(std::pair<const Trk::Surface*,Trk::BoundaryCheck>(&(entryLays[i]->surfaceRepresentation()),true));
	  m_navigLays.push_back(std::pair<const Trk::TrackingVolume*,const Trk::Layer*> (m_currentStatic,entryLays[i]) );
	  Trk::DistanceSolution distSol = m_layers.back().first->straightLineDistanceEstimate(currPar->position(),
                                                                                          currPar->momentum().normalized());
	}
      } 
    }

    // detached volume boundaries
    const std::vector<const Trk::DetachedTrackingVolume*>* detVols = m_currentStatic->confinedDetachedVolumes();
    if (detVols) {
      std::vector<const Trk::DetachedTrackingVolume*>::const_iterator iTer = detVols->begin();
      for (; iTer != detVols->end(); iTer++) { 
        // active station ?
        const Trk::Layer* layR = (*iTer)->layerRepresentation();
        bool active = ( layR && layR->layerType() ) ? true : false;
	const std::vector< SharedObject<const BoundarySurface<TrackingVolume> > >  detBounds=(*iTer)->trackingVolume()->boundarySurfaces();
        if (active) {
	  m_detachedVols.push_back(std::pair<const Trk::DetachedTrackingVolume*,unsigned int> (*iTer,detBounds.size()) );
	  for (unsigned int ibb=0; ibb< detBounds.size(); ibb++ ){
	    const Trk::Surface& surf = (detBounds[ibb].getPtr())->surfaceRepresentation();
	    m_detachedBoundaries.push_back(std::pair<const Trk::Surface*,Trk::BoundaryCheck>(&surf,true));
	  }	  
	} else if (m_currentStatic->geometrySignature()!=Trk::MS ||
		   !m_useMuonMatApprox || (*iTer)->name().substr((*iTer)->name().size()-4,4)=="PERM" ) {  // retrieve inert detached objects only if needed
          if ((*iTer)->trackingVolume()->zOverAtimesRho()!=0. && 
	      (!(*iTer)->trackingVolume()->confinedDenseVolumes() || !(*iTer)->trackingVolume()->confinedDenseVolumes()->size() )
	      && ( !(*iTer)->trackingVolume()->confinedArbitraryLayers() ||
                   !(*iTer)->trackingVolume()->confinedArbitraryLayers()->size() ) ) {
            m_denseVols.push_back(std::pair<const Trk::TrackingVolume*,unsigned int> 
				  ((*iTer)->trackingVolume(),detBounds.size() ) );
	    for (unsigned int ibb=0; ibb< detBounds.size(); ibb++ ){
	      const Trk::Surface& surf = (detBounds[ibb].getPtr())->surfaceRepresentation();
	      m_denseBoundaries.push_back(std::pair<const Trk::Surface*,Trk::BoundaryCheck>(&surf,true));
	    }
	  } 
          const std::vector<const Trk::Layer*>* confLays = (*iTer)->trackingVolume()->confinedArbitraryLayers(); 
	  if ( (*iTer)->trackingVolume()->confinedDenseVolumes() || (confLays && confLays->size()> detBounds.size()) ) {
            m_detachedVols.push_back(std::pair<const Trk::DetachedTrackingVolume*,unsigned int>(*iTer,detBounds.size()));
	    for (unsigned int ibb=0; ibb< detBounds.size(); ibb++ ){
	      const Trk::Surface& surf = (detBounds[ibb].getPtr())->surfaceRepresentation();
	      m_detachedBoundaries.push_back(std::pair<const Trk::Surface*,Trk::BoundaryCheck>(&surf,true));
	    }
          } else if ( confLays ) {
	    std::vector<const Trk::Layer*>::const_iterator lIt = confLays->begin();
	    for ( ; lIt!= confLays->end(); lIt++ ){
	      m_layers.push_back(std::pair<const Trk::Surface*,Trk::BoundaryCheck>(&((*lIt)->surfaceRepresentation()),true) );
	      m_navigLays.push_back(std::pair<const Trk::TrackingVolume*,const Trk::Layer*> ((*iTer)->trackingVolume(),*lIt) );
	    }            
	  }         
	}
      }
    }
    m_denseResolved    = std::pair<unsigned int,unsigned int> (m_denseVols.size(), m_denseBoundaries.size());
    m_layerResolved    = m_layers.size();
  } 

  m_navigSurfs.insert(m_navigSurfs.end(),m_staticBoundaries.begin(),m_staticBoundaries.end());

  // resolve the use of dense volumes
  m_dense = (m_currentStatic->geometrySignature()==Trk::MS && m_useMuonMatApprox ) || (m_currentStatic->geometrySignature()!=Trk::MS && m_useDenseVolumeDescription );

  // reset remaining counters
  m_currentDense = m_dense ?  m_currentStatic : m_highestVolume;
  m_navigBoundaries.clear(); 
  if (m_denseVols.size()>m_denseResolved.first) {
    m_denseVols.resize(m_denseResolved.first);
    m_denseBoundaries.resize(m_denseResolved.second);
  }
  if (m_layers.size()>m_layerResolved) {
    m_layers.resize(m_layerResolved);
    m_navigLays.resize(m_layerResolved);
  }  

  if (m_currentStatic->entryLayerProvider()) nEntryLays = m_currentStatic->entryLayerProvider()->layers().size();

  // current detached volumes
  // collect : subvolume boundaries, ordered/unordered layers, confined dense volumes
  ////////////////////////////////////////////////////// 
  // const Trk::DetachedTrackingVolume* currentActive = 0;
  std::vector<std::pair<const Trk::TrackingVolume*,unsigned int> > navigVols;
   
  gp = currPar->position();    
  std::vector<const Trk::DetachedTrackingVolume*>* detVols =  m_navigator->trackingGeometry()->lowestDetachedTrackingVolumes(gp);    
  std::vector<const Trk::DetachedTrackingVolume*>::iterator dIter = detVols->begin();
  for (; dIter != detVols->end(); dIter++) {
    const Trk::Layer* layR = (*dIter)->layerRepresentation();
    bool active = ( layR && layR->layerType() ) ? true : false;
    if (active && !resolveActive) continue; 
    if (!active && m_currentStatic->geometrySignature()==Trk::MS && 
	m_useMuonMatApprox && (*dIter)->name().substr((*dIter)->name().size()-4,4)!="PERM") continue;
    const Trk::TrackingVolume* dVol = (*dIter)->trackingVolume();
    // detached volume exit ?
    bool dExit =  m_navigator->atVolumeBoundary(currPar,dVol,dir,nextVol,m_tolerance) && !nextVol;
    if (dExit) continue;
    // inert material 
    const std::vector<const Trk::TrackingVolume*>* confinedDense = dVol->confinedDenseVolumes();
    const std::vector<const Trk::Layer*>*          confinedLays  = dVol->confinedArbitraryLayers();
    
    if (!active && !confinedDense && !confinedLays) continue; 
    const std::vector< SharedObject<const BoundarySurface<TrackingVolume> > > bounds=dVol->boundarySurfaces();
    if (!active && !confinedDense && confinedLays->size()<= bounds.size()) continue;
    if (confinedDense || confinedLays ) {
      navigVols.push_back(std::pair<const Trk::TrackingVolume*,unsigned int> (dVol,bounds.size()) );
      for (unsigned int ib=0; ib< bounds.size(); ib++ ){
	const Trk::Surface& surf = (bounds[ib].getPtr())->surfaceRepresentation();
	m_navigBoundaries.push_back(std::pair<const Trk::Surface*,Trk::BoundaryCheck>(&surf,true));
      }
      // collect dense volume boundary
      if (confinedDense) {
	std::vector<const Trk::TrackingVolume*>::const_iterator vIter = confinedDense->begin();
	for (; vIter != confinedDense->end(); vIter++) {
	  const std::vector< SharedObject<const BoundarySurface<TrackingVolume> > > bounds=
	    (*vIter)->boundarySurfaces();
	  m_denseVols.push_back(std::pair<const Trk::TrackingVolume*,unsigned int> (*vIter,bounds.size()) );
	  for (unsigned int ib=0; ib< bounds.size(); ib++ ){
	    const Trk::Surface& surf = (bounds[ib].getPtr())->surfaceRepresentation();
	    m_denseBoundaries.push_back(std::pair<const Trk::Surface*,Trk::BoundaryCheck>(&surf,true));
	  }
	}
      }
      // collect unordered layers
      if (confinedLays) {
	for (unsigned int il = 0; il < confinedLays->size(); il++) {
	  m_layers.push_back(std::pair<const Trk::Surface*,Trk::BoundaryCheck>(&((*confinedLays)[il]->surfaceRepresentation()),true));   
	  m_navigLays.push_back(std::pair<const Trk::TrackingVolume*,const Trk::Layer*> (dVol,(*confinedLays)[il]) );
	}    
      }
    } else {   // active material
      const Trk::TrackingVolume* detVol= dVol->associatedSubVolume(gp);
      if ( !detVol && dVol->confinedVolumes()) {
	std::vector<const Trk::TrackingVolume*> subvols = dVol->confinedVolumes()->arrayObjects();
	for (unsigned int iv=0;iv<subvols.size();iv++) {
	  if ( subvols[iv]->inside(gp,m_tolerance) ) {
	    detVol = subvols[iv]; 
	    break;
	  }
	}
      }
      
      if (!detVol) detVol = dVol;
      bool vExit =  m_navigator->atVolumeBoundary(currPar,detVol,dir,nextVol,m_tolerance) && nextVol!=detVol;
      if ( vExit && nextVol && nextVol->inside(gp,m_tolerance)) { detVol = nextVol; vExit = false; } 
      if ( !vExit ) {
	const std::vector< SharedObject<const BoundarySurface<TrackingVolume> > > bounds= detVol->boundarySurfaces();
	navigVols.push_back(std::pair<const Trk::TrackingVolume*,unsigned int> (detVol,bounds.size()) );
	for (unsigned int ib=0; ib< bounds.size(); ib++ ){
	  const Trk::Surface& surf = (bounds[ib].getPtr())->surfaceRepresentation();
	  m_navigBoundaries.push_back(std::pair<const Trk::Surface*,Trk::BoundaryCheck>(&surf,true));
	}
	if ( detVol->zOverAtimesRho() != 0.) {
	  m_denseVols.push_back(std::pair<const Trk::TrackingVolume*,unsigned int> (detVol,bounds.size()) );
	  for (unsigned int ib=0; ib< bounds.size(); ib++ ){
	    const Trk::Surface& surf = (bounds[ib].getPtr())->surfaceRepresentation();
	    m_denseBoundaries.push_back(std::pair<const Trk::Surface*,Trk::BoundaryCheck>(&surf,true));
	  }
	} 
	// layers ?
	if ( detVol->confinedLayers() ) {         
          if (m_robustSampling || m_currentStatic->geometrySignature()==Trk::MS) {
	    std::vector<const Trk::Layer*> cLays = detVol->confinedLayers()->arrayObjects();
	    for (unsigned int i=0; i < cLays.size(); i++) { 
	      if (cLays[i]->layerType()>0 || cLays[i]->layerMaterialProperties()) {
		m_layers.push_back(std::pair<const Trk::Surface*,Trk::BoundaryCheck>(&(cLays[i]->surfaceRepresentation()),true));
		m_navigLays.push_back(std::pair<const Trk::TrackingVolume*,const Trk::Layer*> (m_currentStatic,cLays[i]) );
	      }
	    }            
	  } else { 
	    const Trk::Layer* lay = detVol->associatedLayer(gp);
	    //if (lay && ( (*dIter)->layerRepresentation()
	    //		 &&(*dIter)->layerRepresentation()->layerType()>0 ) ) currentActive=(*dIter);  
	    if (lay) {
	      m_layers.push_back(std::pair<const Trk::Surface*,Trk::BoundaryCheck>(&(lay->surfaceRepresentation()),true));
	      m_navigLays.push_back(std::pair<const Trk::TrackingVolume*,const Trk::Layer*> (detVol,lay) );
	    }
	    const Trk::Layer* nextLayer = detVol->nextLayer(currPar->position(),dir*currPar->momentum().normalized(),true);      
	    if (nextLayer && nextLayer != lay ) {
	      m_layers.push_back(std::pair<const Trk::Surface*,Trk::BoundaryCheck>(&(nextLayer->surfaceRepresentation()),true));
	      m_navigLays.push_back(std::pair<const Trk::TrackingVolume*,const Trk::Layer*> (detVol,nextLayer) );
	    }
	  }
	} else if ( detVol->confinedArbitraryLayers() ) {
	  const std::vector<const Trk::Layer*>* layers = detVol->confinedArbitraryLayers();
	  for (unsigned int il = 0; il < layers->size(); il++) {
	    m_layers.push_back(std::pair<const Trk::Surface*,Trk::BoundaryCheck>(&((*layers)[il]->surfaceRepresentation()),true));   
	    m_navigLays.push_back(std::pair<const Trk::TrackingVolume*,const Trk::Layer*> (detVol,(*layers)[il]) );
	  }    
	}
      }
    }
  }
  delete detVols;    

  // confined layers
  if ( m_currentStatic->confinedLayers() && updateStatic ) {
    //if ( m_currentStatic->confinedLayers() ) {
    if (m_robustSampling || m_currentStatic->geometrySignature()==Trk::MS) {
      std::vector<const Trk::Layer*> cLays = m_currentStatic->confinedLayers()->arrayObjects();
      for (unsigned int i=0; i < cLays.size(); i++) { 
	if (cLays[i]->layerType()>0 || cLays[i]->layerMaterialProperties()) {
	  m_layers.push_back(std::pair<const Trk::Surface*,Trk::BoundaryCheck>(&(cLays[i]->surfaceRepresentation()),true));
	  m_navigLays.push_back(std::pair<const Trk::TrackingVolume*,const Trk::Layer*> (m_currentStatic,cLays[i]) );
	}
      }
    } else { 
      //* this does not work - debug !
      const Trk::Layer* lay = m_currentStatic->associatedLayer(gp);       
      //if (!lay) {
      //  lay = m_currentStatic->associatedLayer(gp+m_tolerance*parm.momentum().normalized());
      //	std::cout<<" find input associated layer, second attempt:"<< lay<< std::endl;
      //} 
      if (lay) {
	m_layers.push_back(std::pair<const Trk::Surface*,Trk::BoundaryCheck>(&(lay->surfaceRepresentation()),false));
	m_navigLays.push_back(std::pair<const Trk::TrackingVolume*,const Trk::Layer*> (m_currentStatic,lay) );
	const Trk::Layer* nextLayer = lay->nextLayer(currPar->position(),dir*currPar->momentum().normalized());      
	if (nextLayer && nextLayer != lay ) {
	  m_layers.push_back(std::pair<const Trk::Surface*,Trk::BoundaryCheck>(&(nextLayer->surfaceRepresentation()),false));
	  m_navigLays.push_back(std::pair<const Trk::TrackingVolume*,const Trk::Layer*> (m_currentStatic,nextLayer) );
	}
	const Trk::Layer* backLayer = lay->nextLayer(currPar->position(),-dir*currPar->momentum().normalized());      
	if (backLayer && backLayer != lay ) {
	  m_layers.push_back(std::pair<const Trk::Surface*,Trk::BoundaryCheck>(&(backLayer->surfaceRepresentation()),false));
	  m_navigLays.push_back(std::pair<const Trk::TrackingVolume*,const Trk::Layer*> (m_currentStatic,backLayer) );
	}
      }
    }
  }
   
  // m_navigSurfs contains destination surface (if it exists), static volume boundaries  
  // complete with TG m_layers/dynamic layers, m_denseBoundaries, m_navigBoundaries, m_detachedBoundaries

  if (m_layers.size()) m_navigSurfs.insert(m_navigSurfs.end(),m_layers.begin(),m_layers.end());
  if (m_denseBoundaries.size()) m_navigSurfs.insert(m_navigSurfs.end(),m_denseBoundaries.begin(),m_denseBoundaries.end());
  if (m_navigBoundaries.size()) m_navigSurfs.insert(m_navigSurfs.end(),m_navigBoundaries.begin(),m_navigBoundaries.end());
  if (m_detachedBoundaries.size()) m_navigSurfs.insert(m_navigSurfs.end(),m_detachedBoundaries.begin(),m_detachedBoundaries.end());


  // current dense
  m_currentDense =  m_highestVolume;
  if (m_dense && !m_denseVols.size()) m_currentDense = m_currentStatic;
  else {
    for (unsigned int i=0;i<m_denseVols.size(); i++) {
      const Trk::TrackingVolume* dVol = m_denseVols[i].first;
      if ( dVol->inside(currPar->position(),m_tolerance)  && dVol->zOverAtimesRho()!=0. ) {
	if ( !m_navigator->atVolumeBoundary(currPar,dVol,dir,nextVol,m_tolerance) || nextVol == dVol ) m_currentDense = dVol;
      } 
    }
  }

  // before propagation, loop over layers and collect hits 
  if (m_hitVector) {

    for (unsigned int i=0; i<m_navigLays.size(); i++) {

      if (m_navigLays[i].second->layerType()>0 && m_navigLays[i].second->isOnLayer(currPar->position()) ) {
	if (m_navigLays[i].second->surfaceArray()) {
	  // perform the overlap Search on this layer
	  ATH_MSG_VERBOSE( "  [o] Calling overlapSearch() on input layer.");	
	  overlapSearch(*m_subPropagators[0],*currPar,*currPar,*m_navigLays[i].second,timeLim.time,dir,true,particle);       	   
	} else {
	  ATH_MSG_VERBOSE( "  [o] Collecting intersection with active input layer.");	
          m_hitVector->push_back(Trk::HitInfo(currPar->clone(),timeLim.time,m_navigLays[i].second->layerType(),0.));
	}
      } // ------------------------------------------------- Fatras mode off -----------------------------------

    }

  }
  
  // ready to propagate      
  // till: A/ static volume boundary(bcheck=true) , B/ material layer(bcheck=true), C/ destination surface(bcheck=false)
  // update of m_navigSurfs required if I/ entry into new navig volume, II/ exit from currentActive without overlaps
   
   nextVol = 0;
   while (currPar) {      
     std::vector<unsigned int> solutions;
     //double time_backup = timeLim.time;
     //double path_backup = m_path.x0Collected;
     ATH_MSG_DEBUG( "  [+] Starting propagation at position  " << positionOutput(currPar->position())  
		    << " (current momentum: " << currPar->momentum().mag() << ")" ); 
     ATH_MSG_DEBUG( "  [+] " << m_navigSurfs.size() << " target surfaces in '" << m_currentDense->volumeName() <<"'.");      // verify that material input makes sense
     if (!(m_currentDense->inside(currPar->position(),m_tolerance) 
	   || m_navigator->atVolumeBoundary(currPar,m_currentDense,dir,assocVol,m_tolerance) ) ) m_currentDense = m_highestVolume ;
     //const Trk::TrackParameters* nextPar = m_stepPropagator->propagateT(*currPar,m_navigSurfs,dir,*m_currentDense,particle,solutions,m_path,timeLim,true);
     const Trk::TrackParameters* nextPar = m_stepPropagator->propagateT(*currPar,m_navigSurfs,dir,m_fieldProperties,particle,solutions,m_path,timeLim,true,m_currentDense);
     ATH_MSG_VERBOSE( "  [+] Propagation done. " );
     if (nextPar)  
       ATH_MSG_DEBUG( "  [+] Position after propagation -   at " << positionOutput(nextPar->position())<<", timed at " << timeLim.time); 

     if (!nextPar) {
       ATH_MSG_DEBUG( "  [!] Propagation failed, return 0" );
       m_parametersAtBoundary.boundaryInformation(m_currentStatic,nextPar,nextPar);     
       return returnParameters;
     }

     throwIntoGarbageBin(nextPar);         

     // material update has been done already by the propagator
     if ( m_path.x0Max>0. && ( (m_path.process<100 && m_path.x0Collected >= m_path.x0Max) || (m_path.process>100 && m_path.l0Collected >= m_path.x0Max)) ) {

       // trigger presampled interaction, provide material properties if needed 
       // process interaction only if creation of secondaries allowed
       if ( m_currentStatic->geometrySignature()==Trk::ID || m_caloMsSecondary ) {
	 const Trk::Material* extMprop = m_path.process>100 ? m_currentDense : 0;
       
	 const Trk::TrackParameters* iPar = m_updators[0]->interact(timeLim.time,nextPar->position(),nextPar->momentum(),particle,m_path.process,extMprop);
   
	 if (!iPar) return returnParameters;
  
	 throwIntoGarbageBin(iPar);         
	 return extrapolateToVolumeWithPathLimit(*iPar,timeLim,dir,particle,nextGeoID,destVol);

       } else {    // kill the particle without trace ( some validation info can be included here eventually )
	 return returnParameters;
       }

     }
     // decay ?
     if ( timeLim.tMax > 0. &&  timeLim.time >= timeLim.tMax ) {
       // process interaction only if creation of secondaries allowed
       if ( m_currentStatic->geometrySignature()==Trk::ID || m_caloMsSecondary ) {
	 // trigger presampled interaction 
	 const Trk::TrackParameters* iPar = m_updators[0]->interact(timeLim.time,nextPar->position(),nextPar->momentum(),particle,timeLim.process);
	 if (!iPar) return returnParameters;
	 throwIntoGarbageBin(iPar);         
	 return extrapolateToVolumeWithPathLimit(*iPar,timeLim,dir,particle,nextGeoID,destVol);
       } else {    // kill the particle without trace ( some validation info can be included here eventually )
	 return returnParameters;
       }
     }

     // check missing volume boundary
     if (nextPar && !(m_currentDense->inside(nextPar->position(),m_tolerance) 
		      || m_navigator->atVolumeBoundary(nextPar,m_currentDense,dir,assocVol,m_tolerance) ) ) {
       ATH_MSG_DEBUG( "  [!] ERROR: missing volume boundary for volume"<< m_currentDense->volumeName() );
     }
     //  if ( m_currentDense->zOverAtimesRho() != 0.) {     
     //    ATH_MSG_DEBUG( "  [!] ERROR: trying to recover: repeat the propagation step in"<< m_highestVolume->volumeName() );
     //    m_currentDense = m_highestVolume;
     //    timeLim.time = time_backup; 
     //    continue;
     //  }
     //}

     ATH_MSG_DEBUG( "  [+] Number of intersection solutions: " << solutions.size() );

     unsigned int iSol  = 0;
     while ( iSol < solutions.size() ) {
       if ( solutions[iSol] < iDest ) {
         return nextPar->clone();          
       } else if ( solutions[iSol] < iDest + m_staticBoundaries.size() ) {
	 // static volume boundary; return to the main loop 
	 unsigned int index = solutions[iSol]-iDest;
	 // use global coordinates to retrieve attached volume (just for static!)
	 nextVol = (m_currentStatic->boundarySurfaces())[index].getPtr()->attachedVolume(nextPar->position(),nextPar->momentum(),dir);
	 // double check the next volume
	 if ( nextVol && !(nextVol->inside(nextPar->position()+0.01*nextPar->momentum().normalized(),m_tolerance) ) ) {
	   ATH_MSG_DEBUG( "  [!] WARNING: wrongly assigned static volume ?"<< m_currentStatic->volumeName()<<"->" << nextVol->volumeName() );
	   nextVol = m_navigator->trackingGeometry()->lowestStaticTrackingVolume(nextPar->position()+0.01*nextPar->momentum().normalized());
	   if (nextVol) ATH_MSG_DEBUG( "  new search yields: "<< nextVol->volumeName() );          
	 }
	 // end double check - to be removed after validation of the geometry gluing 
	 if (nextVol != m_currentStatic ) {
	   m_parametersAtBoundary.boundaryInformation(nextVol,nextPar,nextPar);         
	   ATH_MSG_DEBUG("  [+] StaticVol boundary reached of '" <<m_currentStatic->volumeName() << "'.");
	   if ( m_navigator->atVolumeBoundary(nextPar,m_currentStatic,dir,assocVol,m_tolerance) && assocVol != nextVol )
	     m_currentDense = m_dense ? nextVol : m_highestVolume;
	   // no next volume found --- end of the world
	   if ( !nextVol ) {
	     ATH_MSG_DEBUG( "  [+] World boundary reached        - at " << positionOutput(nextPar->position())<<", timed at " << timeLim.time);
             nextGeoID = Trk::GeometrySignature(Trk::Unsigned);
	     if (!destVol) { return nextPar;}
	   }
           // next volume found and parameters are at boundary
	   if ( nextVol && nextPar ){ 
	     ATH_MSG_DEBUG( "  [+] Crossing to next volume '" << nextVol->volumeName() << "'");
	     ATH_MSG_DEBUG( "  [+] Crossing position is         - at " <<  positionOutput(nextPar->position()) );
             if (!destVol && m_currentStatic->geometrySignature()!=nextVol->geometrySignature())
	       { nextGeoID=nextVol->geometrySignature(); return nextPar; }   
	   }     
	   return extrapolateToVolumeWithPathLimit(*nextPar,timeLim,dir,particle,nextGeoID,destVol);
	 }
       } else if ( solutions[iSol] < iDest + m_staticBoundaries.size() + m_layers.size() ) {
	 // next layer; don't return passive material layers unless required 
	 unsigned int index = solutions[iSol]-iDest-m_staticBoundaries.size();
	 const Trk::Layer* nextLayer = m_navigLays[index].second;
         // material update ?
         //bool matUp = nextLayer->layerMaterialProperties() && m_includeMaterialEffects && nextLayer->isOnLayer(nextPar->position());
         bool matUp = nextLayer->fullUpdateMaterialProperties(*nextPar) && m_includeMaterialEffects && nextLayer->isOnLayer(nextPar->position());
         // identical to last material layer ?
         if (matUp && nextLayer==m_lastMaterialLayer ) {
	   // cylinder layers !
	   const Trk::CylinderLayer* cyl = dynamic_cast<const Trk::CylinderLayer*> (nextLayer);
	   const Trk::SubtractedCylinderLayer* scyl = dynamic_cast<const Trk::SubtractedCylinderLayer*> (nextLayer);
	   if (!cyl && !scyl) matUp = false;            
	 }
	 // material update 
	 const ITimedMatEffUpdator* currentUpdator = subMaterialEffectsUpdator(*m_currentStatic);
         if ( matUp ) {
           double pIn = nextPar->momentum().mag(); 
	   nextPar = currentUpdator ? currentUpdator->update(nextPar, *nextLayer, timeLim, m_path, m_currentStatic->geometrySignature(), dir, particle) : nextPar;
	   if (!nextPar) {
	     ATH_MSG_VERBOSE( "  [+] Update may have killed track - return." );
	     m_parametersAtBoundary.resetBoundaryInformation();
	     return returnParameters;
	   } else {  
             ATH_MSG_VERBOSE(" Layer energy loss:"<< nextPar->momentum().mag()- pIn << "at position:"<< nextPar->position()<<", current momentum:"<<nextPar->momentum());
           }
	 }   
         // active surface intersections ( Fatras hits ...)
	 if (m_hitVector &&  particle!=Trk::neutron) {
	   if (nextLayer->surfaceArray()) {
	     // perform the overlap Search on this layer
	     ATH_MSG_VERBOSE( "  [o] Calling overlapSearch() on  layer.");	
	     overlapSearch(*m_subPropagators[0],*currPar,*nextPar,*nextLayer,timeLim.time,dir,true,particle);       	   
	   } else if (nextLayer->layerType()>0 && nextLayer->isOnLayer(nextPar->position())) {
	     ATH_MSG_VERBOSE( "  [o] Collecting intersection with active layer.");	
	     m_hitVector->push_back(Trk::HitInfo(nextPar->clone(),timeLim.time,nextLayer->layerType(),0.));
	   }
	 } // ------------------------------------------------- Fatras mode off -----------------------------------

         // TODO : debug the retrieval of next layer
	 if ( !m_robustSampling && !m_currentStatic->geometrySignature()==Trk::MS ) { 
	   if (m_navigLays[index].first && m_navigLays[index].first->confinedLayers()) {
	     const Trk::Layer* newLayer = nextLayer->nextLayer(nextPar->position(),dir*nextPar->momentum().normalized());
	     if (newLayer && newLayer!=nextLayer) {
               bool found = false;
               int replace = -1;
               for (unsigned int i=nEntryLays;i<m_navigLays.size();i++) { 
		 if (m_navigLays[i].second==newLayer) {found=true; break;}
                 if (m_navigLays[i].second !=nextLayer) replace = i;
	       }
               if (!found) { 
                 if (replace>-1) {
		   m_navigLays[replace].second = newLayer;
		   m_navigSurfs[solutions[iSol]+replace-index].first = &(newLayer->surfaceRepresentation());
                 } else {
                   // can't insert a surface in middle
		   return extrapolateToVolumeWithPathLimit(*nextPar,timeLim,dir,particle,nextGeoID,destVol);                   
		 }
	       }
	     }            
	   }
	 }
         currPar = nextPar;
       } else if ( solutions[iSol] < iDest + m_staticBoundaries.size() + m_layers.size() + m_denseBoundaries.size() ) {
	 // dense volume boundary
	 unsigned int index = solutions[iSol] - iDest -m_staticBoundaries.size()- m_layers.size();
	 std::vector< std::pair<const Trk::TrackingVolume*,unsigned int> >::iterator dIter = m_denseVols.begin();
	 while ( index >= (*dIter).second && dIter!= m_denseVols.end() ) {
	   index -= (*dIter).second ; 
	   dIter++;
	 }
	 if ( dIter != m_denseVols.end() ) {
	   currVol = (*dIter).first;
	   nextVol = ( (*dIter).first->boundarySurfaces())[index].getPtr()->attachedVolume(*nextPar,dir);
           // the boundary orientation is not reliable
	   Amg::Vector3D tp = nextPar->position()+2*m_tolerance*dir*nextPar->momentum().normalized();
           if (!nextVol || !nextVol->inside(tp,m_tolerance) ) {   // search for dense volumes
	     m_currentDense =  m_highestVolume;
	     if (m_dense && !m_denseVols.size()) m_currentDense = m_currentStatic;
	     else {
	       for (unsigned int i=0;i<m_denseVols.size(); i++) {
		 const Trk::TrackingVolume* dVol = m_denseVols[i].first;
		 if ( dVol->inside(tp,m_tolerance)  && dVol->zOverAtimesRho()!=0. ){
		   m_currentDense = dVol;
		   ATH_MSG_DEBUG( "  [+] Next dense volume found: '" << m_currentDense->volumeName() << "'."); 
		   break;
		 } 
	       } // loop over dense volumes
	     }
	   } else {
             m_currentDense = nextVol;
	     ATH_MSG_DEBUG( "  [+] Next dense volume: '" << m_currentDense->volumeName() << "'."); 
	   }
	 }

       } else if ( solutions[iSol] < iDest + m_staticBoundaries.size() + m_layers.size() + m_denseBoundaries.size() 
		   + m_navigBoundaries.size() ) {
	 // navig volume boundary
	 unsigned int index = solutions[iSol]-iDest-m_staticBoundaries.size()- m_layers.size()-m_denseBoundaries.size();
	 std::vector< std::pair<const Trk::TrackingVolume*,unsigned int> >::iterator nIter = navigVols.begin();
	 while ( index >= (*nIter).second && nIter!= navigVols.end() ) {
	   index -= (*nIter).second ; 
	   nIter++;
	 }
	 if ( nIter != navigVols.end() ) {
	   currVol = (*nIter).first;
	   nextVol = ( (*nIter).first->boundarySurfaces())[index].getPtr()->attachedVolume(*nextPar,dir);
	   if (!nextVol) ATH_MSG_DEBUG("  [+] Navigation volume boundary, leaving volume '"
				       << currVol->volumeName() << "'.");
	   else ATH_MSG_DEBUG("  [+] Navigation volume boundary, entering volume '" << nextVol->volumeName() << "'.");
	   currPar = nextPar;
	   // return only if detached volume boundaries not collected
	   //if ( nextVol || !detachedBoundariesIncluded )
	   if ( nextVol )
	     return extrapolateToVolumeWithPathLimit(*currPar,timeLim,dir,particle,nextGeoID,destVol);                 
	 } 
       } else if ( solutions[iSol] < iDest + m_staticBoundaries.size() + m_layers.size() + m_denseBoundaries.size() 
		   + m_navigBoundaries.size() + m_detachedBoundaries.size() ) {
	 // detached volume boundary
	 unsigned int index = solutions[iSol]-iDest-m_staticBoundaries.size()- m_layers.size()
	   - m_denseBoundaries.size() - m_navigBoundaries.size();
	 std::vector< std::pair<const Trk::DetachedTrackingVolume*,unsigned int> >::iterator dIter = m_detachedVols.begin();
	 while ( index >= (*dIter).second && dIter!= m_detachedVols.end() ) {
	   index -= (*dIter).second ; 
	   dIter++;
	 }
	 if ( dIter != m_detachedVols.end() ) {
	   currVol = (*dIter).first->trackingVolume();
	   nextVol = ( (*dIter).first->trackingVolume()->boundarySurfaces())[index].getPtr()->attachedVolume(*nextPar,dir);
	   if (!nextVol) 
	     ATH_MSG_DEBUG("  [+] Detached volume boundary, leaving volume '" << currVol->volumeName() << "'.");
	   else ATH_MSG_DEBUG("  [+] Detached volume boundary, entering volume '" << nextVol->volumeName() << "'.");
	   currPar = nextPar;
	   //if ( nextVol || !detachedBoundariesIncluded)
	   if ( nextVol )
	     return extrapolateToVolumeWithPathLimit(*currPar,timeLim,dir,particle,nextGeoID,destVol);   
	 } 
       }   
       iSol++;
     }
     currPar = nextPar;
   }

   return returnParameters;
}        


void Trk::TimedExtrapolator::overlapSearch(const IPropagator& prop,
					   const TrackParameters& parm,
					   const TrackParameters& parsOnLayer,
					   const Layer& lay,
					   //const TrackingVolume& tvol,
					   float time,
					   PropDirection dir,
					   BoundaryCheck, //bcheck
					   ParticleHypothesis particle,
					   bool startingLayer) const 
{
 
       // indicate destination layer 
       bool isDestinationLayer = (&parsOnLayer.associatedSurface() == m_destinationSurface);       
       // start and end surface for on-layer navigation
       //  -> take the start surface if ther parameter surface is owned by detector element
       const Trk::Surface* startSurface = ((parm.associatedSurface()).associatedDetectorElement() && startingLayer) ?
           &parm.associatedSurface() : 0;
       const Trk::Surface* endSurface =  isDestinationLayer ? m_destinationSurface : 0;                     
       // - the best detSurface to start from is the one associated to the detector element        
       const Trk::Surface* detSurface = (parsOnLayer.associatedSurface()).associatedDetectorElement() ?
                                        &parsOnLayer.associatedSurface() : 0;

       ATH_MSG_VERBOSE("  [o] OverlapSearch called " << ( startSurface ? "with " : "w/o " ) << "start, " 
                                                       << ( endSurface ? "with " : "w/o " ) << "end surface." );       

       if (!detSurface){ 
          // of parsOnLayer are different from parm, then local position is safe, because the extrapolation
          //   to the detector surface has been done !     
          detSurface = isDestinationLayer ? lay.subSurface(parsOnLayer.localPosition()) : lay.subSurface(parsOnLayer.position()); 
          if (detSurface) ATH_MSG_VERBOSE("  [o] Detector surface found through subSurface() call" );
          else ATH_MSG_VERBOSE("  [o] No Detector surface found on this layer.");
       } else 
          ATH_MSG_VERBOSE("  [o] Detector surface found through parameter on layer association" );       
       
       // indicate the start layer
       bool isStartLayer = (detSurface && detSurface == startSurface);
       
       const Trk::TrackParameters* detParameters     = 0;
       // the temporary vector (might have to be ordered)
       std::vector<const Trk::TrackParameters*> detParametersOnLayer;
       bool reorderDetParametersOnLayer = false;             
       // the first test for the detector surface to be hit (false test) 
       // - only do this if the parameters aren't on the surface 
       // (i.e. search on the start layer or end layer)
       if (isDestinationLayer) 
          detParameters = (&parsOnLayer);
       else if (isStartLayer)
		  detParameters = (&parm);
       else        
          //detParameters = prop.propagate(parm, *detSurface, dir, false, tvol, particle);
          detParameters = prop.propagate(parm, *detSurface, dir, false, m_fieldProperties, particle);

      // set the surface hit to true, it is anyway overruled 
      bool surfaceHit = true;
      if (detParameters && 
          !isStartLayer &&
          !isDestinationLayer) {
          ATH_MSG_VERBOSE( "  [o] First intersection with Detector surface: " <<  *detParameters );
          // for the later use in the overlapSearch
          surfaceHit = detParameters && detSurface ? detSurface->isOnSurface(detParameters->position()) : 0; //,bcheck) - creates problems on start layer; 
          // check also for start/endSurface on this level
           
          surfaceHit = ( surfaceHit && startSurface ) ? 
             ((detParameters->position()-parm.position()).dot(dir*parm.momentum().normalized()) > 0) : surfaceHit;
          surfaceHit = ( surfaceHit && endSurface ) ?
             ((detParameters->position()-parsOnLayer.position()).dot(dir*parsOnLayer.momentum().normalized()) < 0) : surfaceHit;

          // surface is hit within bounds (or at least with given boundary check directive) -> it counts 
          // surface hit also survived start/endsurface search
          //
          // Convention for Fatras: always apply the full update on the last parameters 
          //                        of the gathered vector (no pre/post schema) 
          // don't record a hit on the destination surface
          if (surfaceHit && 
              detSurface != startSurface && 
              detSurface != m_destinationSurface) {
             ATH_MSG_VERBOSE( "  [H] Hit with detector surface recorded ! " );
             // push into the temporary vector
             detParametersOnLayer.push_back(detParameters);          
         } else if (detParameters){
             // no hit -> fill into the garbage bin 
             ATH_MSG_VERBOSE("  [-] Detector surface hit cancelled through bounds check or start/end surface check." );       
             throwIntoGarbageBin(detParameters);
           }
       }

       // search for the overlap ------------------------------------------------------------------------
       if (detParameters){     
         if (lay.overlapDescriptor()){
            // the surface ordering on BinnedArrays can not guarantee a strict radial, longitudinal ordering (overlaps)
            // search directive is only strict for start-at-the-layer-setup
             Trk::PropDirection overlapSearchDir = (startSurface || endSurface) ? dir : Trk::anyDirection;             
            // --------------------------- overlap search following RISC logics ------------------------------------
            const std::vector<Trk::OverlapCellCondition>& overlapCells  = lay.overlapDescriptor()->overlapCells(*detParameters,overlapSearchDir,startSurface,endSurface);
            std::vector<Trk::OverlapCellCondition>::const_iterator ocIter    = overlapCells.begin();
            std::vector<Trk::OverlapCellCondition>::const_iterator ocIterEnd = overlapCells.end();
            
            ATH_MSG_VERBOSE( "  [o] " << overlapCells.size() 
                << " candidates for overlap search provided with direction " << overlapSearchDir );
            for ( ; ocIter != ocIterEnd; ++ocIter){
                 // assign Surface and search direction
                 const Trk::Surface*  overlapSurface = (ocIter->first).first;
                 // skip the destination surface
                 if (overlapSurface == m_destinationSurface 
                    || overlapSurface == startSurface 
                    || overlapSurface == endSurface
                    || overlapSurface == detSurface) continue;
                 // re-assign the overlap search direction from what the overlap descriptor yielded
                 overlapSearchDir = (ocIter->first).second;
                 int                overlapCondition = (ocIter->second);                 
                 // base the decision condition on surfaceHit boolean                 
                 if (overlapCondition !=2){
                    if (surfaceHit && !overlapCondition) { 
                        ATH_MSG_VERBOSE( "  [-] Surface was hit, but overlap condition requires differently -> skip this surface.");
                        continue;
                    }
                    if (!surfaceHit && overlapCondition) {
                       ATH_MSG_VERBOSE( "  [-] Surface was not hit, but overlap condition requires it -> skip this surface.");
                       continue;
                    }
                 }
                 
                 // --------------------------------------------------------------------------------
                 ATH_MSG_VERBOSE( "  [o] Overlap surface retrieved, try to hit it with direction " << overlapSearchDir );
                 ATH_MSG_VERBOSE( "  [o] Overlap surface center       - at " << positionOutput(overlapSurface->center()) );
                 
                 // search with given boundary directive
                 const Trk::TrackParameters* overlapParameters = prop.propagate(parm,
                                                                                *overlapSurface,
                                                                                overlapSearchDir,
                                                                                true,
                                                                                //tvol,
                                                                                m_fieldProperties,
                                                                                particle);           
              if (overlapParameters) {
                  ATH_MSG_VERBOSE( "  [+] Overlap surface was hit, checking start/end surface condition." );
                  // check on start / end surface for on-layer navigaiton action
                  surfaceHit = ( startSurface ) ? 
                      ((overlapParameters->position()-parm.position()).dot(dir*parm.momentum().normalized()) > 0) : true;
                  surfaceHit = ( surfaceHit && endSurface ) ?
                      ((overlapParameters->position()-parsOnLayer.position()).dot(dir*parsOnLayer.momentum().normalized()) < 0) : surfaceHit;                  
                  if (surfaceHit){
                      ATH_MSG_VERBOSE( "  [H] Hit with detector surface recorded !" );
                      // count the overlap Surfaces hit 
                      ++m_overlapSurfaceHit;
                      // distinguish whether sorting is needed or not
                      if (overlapSearchDir==Trk::anyDirection) reorderDetParametersOnLayer=true;
                      // push back into the temporary vector
                      detParametersOnLayer.push_back(overlapParameters);        
                   } else { // the parameters have been cancelled by start/end surface
                       // no hit -> fill into the garbage bin 
                       ATH_MSG_VERBOSE("  [-] Detector surface hit cancelled through start/end surface check." );       
                       throwIntoGarbageBin(overlapParameters);
                       }
                } else // surface was not hit with the given bchek, search direction 
                  surfaceHit = false;                                                      
            }
         
         }     
       }  //---------------------------------------------------------------------------------------------

       // push them into the parameters vector
       std::vector<const Trk::TrackParameters*>::const_iterator parsOnLayerIter    = detParametersOnLayer.begin();
       std::vector<const Trk::TrackParameters*>::const_iterator parsOnLayerIterEnd = detParametersOnLayer.end();

       // reorder the track parameters if neccessary, the overlap descriptor did not provide the ordered surfaces
       if (reorderDetParametersOnLayer){
           // sort to reference of incoming parameters
          Trk::TrkParametersComparisonFunction parameterSorter(parm.position());
          sort(detParametersOnLayer.begin(), detParametersOnLayer.end(), parameterSorter);
       }   

       // after sorting : reset the iterators
       parsOnLayerIter    = detParametersOnLayer.begin();
       parsOnLayerIterEnd = detParametersOnLayer.end();
       // now fill them into the parameter vector -------> hit creation done <----------------------
       for ( ; parsOnLayerIter != parsOnLayerIterEnd; ++parsOnLayerIter) {
	 if (m_hitVector) m_hitVector->push_back(Trk::HitInfo(*parsOnLayerIter,time,0,0.));
       }
}

std::string Trk::TimedExtrapolator::positionOutput(const Amg::Vector3D& pos) const
{
   std::stringstream outStream;
   if (m_printRzOutput) 
        outStream <<  "[r,phi,z] = [ " << pos.perp() << ", " << pos.phi() << ", " << pos.z() << " ]";
   else 
        outStream <<  "[xyz] = [ " << pos.x() << ", " << pos.y() << ", " << pos.z() << " ]";
   return outStream.str();
}

std::string Trk::TimedExtrapolator::momentumOutput(const Amg::Vector3D& mom) const
{
   std::stringstream outStream;
   outStream <<  "[eta,phi] = [ " << mom.eta() << ", " << mom.phi() << " ]";
   return outStream.str();
}

void Trk::TimedExtrapolator::emptyGarbageBin(const Trk::TrackParameters* trPar) const
{   
   // empty the garbage  
   std::map<const Trk::TrackParameters*, bool>::iterator garbageIter  = m_garbageBin.begin();
   std::map<const Trk::TrackParameters*, bool>::iterator garbageEnd   = m_garbageBin.end();

   bool throwCurrent = false;  
   bool throwLast    = false;
   for ( ; garbageIter != garbageEnd; ++garbageIter ){
     if (garbageIter->first && garbageIter->first!=trPar && garbageIter->first != m_lastValidParameters) delete (garbageIter->first);
     if (garbageIter->first && garbageIter->first==trPar) throwCurrent = true; 
     if (garbageIter->first && garbageIter->first==m_lastValidParameters) throwLast = true; 
   }

   m_garbageBin.clear();
   if (throwCurrent) throwIntoGarbageBin(trPar);
   if (throwLast) throwIntoGarbageBin(m_lastValidParameters);
}

// the validation action -> propagated to the SubTools
void Trk::TimedExtrapolator::validationAction() const
{
  // record the updator validation information
  for (unsigned int imueot = 0; imueot < m_subUpdators.size(); ++imueot) 
      m_subUpdators[imueot]->validationAction();      
  // record the navigator validation information
  m_navigator->validationAction();
}

const Trk::TrackParameters*  Trk::TimedExtrapolator::transportNeutralsWithPathLimit( const Trk::TrackParameters& parm,
										     Trk::PathLimit& pathLim, Trk::TimeLimit& timeLim,
										     Trk::PropDirection dir,                                    
										     Trk::ParticleHypothesis particle,
										     std::vector<Trk::HitInfo>*& hitInfo,
										     Trk::GeometrySignature& nextGeoID,
										     const Trk::TrackingVolume* boundaryVol) const 
{

// extrapolation method intended for simulation of particle decay; collects the material up to pre-defined limit and triggers
// material interaction
// possible outcomes:1/ returns curvilinear parameters after reaching the maximal path (if to be destroyed)
//                   2/ returns parameters at destination volume boundary 
//                   3/ returns 0 ( particle stopped ) but keeps material and timing info

  ATH_MSG_DEBUG( "M-[" << ++m_methodSequence << "] transportNeutralsWithPathLimit(...) " << pathLim.x0Max << ", from " << parm.position());

  // reset the path ( in x0 !!) 
  m_path = PathLimit( pathLim.x0Max - pathLim.x0Collected, pathLim.process);     // collect material locally

  // initialize time info
  m_time = timeLim.time;

  // initialize hit vector
  m_hitVector = hitInfo;
 
  // if no input volume, define as highest volume
  //const Trk::TrackingVolume* destVolume = boundaryVol ? boundaryVol : m_navigator->highestVolume();
  m_currentStatic = 0;
  if (boundaryVol && !boundaryVol->inside(parm.position(),m_tolerance) ) return 0;

  m_particleMass = s_particleMasses.mass[particle];
   
  // extrapolate to destination volume boundary with path limit
  const Trk::TrackParameters* returnParms = transportToVolumeWithPathLimit( parm, timeLim, dir,particle, nextGeoID, boundaryVol);

  //save actual path on output 
  if (m_path.x0Collected>0.)  pathLim.updateMat(m_path.x0Collected, m_path.weightedZ/m_path.x0Collected, m_path.l0Collected);

  // return timing
  timeLim.time = m_time; 
   
  return returnParms;

}

const Trk::TrackParameters*  Trk::TimedExtrapolator::transportToVolumeWithPathLimit( const Trk::TrackParameters& parm,
										     Trk::TimeLimit& timeLim,
										     Trk::PropDirection dir,                                    
										     Trk::ParticleHypothesis particle,
										     Trk::GeometrySignature& nextGeoID,
										     const Trk::TrackingVolume* destVol) const 
{

  // returns:
  //    A)  curvilinear track parameters if path or time limit reached 
  //    B)  boundary parameters (at destination volume boundary)

  // initialize the return parameters vector
  const Trk::TrackParameters* returnParameters = 0;
  const Trk::TrackParameters* currPar = &parm;
  const Trk::TrackingVolume*  currVol = 0;
  const Trk::TrackingVolume*  nextVol = 0;
  const Trk::TrackingVolume* assocVol = 0;  
  //int                        nEntryLays = 0;
  unsigned int               iDest = 0;

  // std::cout << "transportNeutralsWithPathLimit(...) " << m_pathLim << ", from " << parm.position()<<" current time, path:"<< m_time<<","<<m_path<<std::endl;
  
  // destination volume boundary ? 
  if ( destVol && m_navigator->atVolumeBoundary(currPar,destVol,dir,nextVol,m_tolerance) && nextVol != destVol ) { return &parm; }
  
  // bool resolveActive = m_resolveActive;
  if (!m_highestVolume ) m_highestVolume = m_navigator->highestVolume();

  emptyGarbageBin(&parm);
  // transport surfaces:  collect only those with valid intersection (easy to calculate for neutrals)
  if( m_trSurfs.capacity() > m_maxNavigSurf ) m_trSurfs.reserve(m_maxNavigSurf); 
  m_trSurfs.clear();

  // target volume may not be part of tracking geometry 
  if (destVol) { 
    const Trk::TrackingVolume* tgVol =  m_navigator->trackingGeometry()->trackingVolume(destVol->volumeName());
    if (!tgVol || tgVol!=destVol) {
      const std::vector< SharedObject<const BoundarySurface<TrackingVolume> > > bounds = destVol->boundarySurfaces();
      for (unsigned int ib=0; ib< bounds.size(); ib++ ){
	const Trk::Surface& surf = (bounds[ib].getPtr())->surfaceRepresentation();
	Trk::DistanceSolution distSol = surf.straightLineDistanceEstimate(currPar->position(),
									  dir*currPar->momentum().normalized());
        if (distSol.numberOfSolutions()>0 && distSol.first()>0.) {
          // boundary check
	  Amg::Vector3D gp = currPar->position()+ distSol.first()*dir*currPar->momentum().normalized();
          if ( surf.isOnSurface(gp,true,0.001,0.001) ) {
	    iDest++;
	    m_trSurfs.push_back(std::pair<const Trk::Surface*,double>(&surf,distSol.first()));
	  }   // valid intersection
	}  // along path
        if (distSol.numberOfSolutions()>1 && distSol.second()>0.) {
          // boundary check
	  Amg::Vector3D gp = currPar->position()+ distSol.second()*dir*currPar->momentum().normalized();
          if ( surf.isOnSurface(gp,true,0.001,0.001) ) {
	    iDest++;
	    m_trSurfs.push_back(std::pair<const Trk::Surface*,double>(&surf,distSol.second()));
	  }   // valid intersection
	}
      } // end loop over boundaries
    } // end process external volume 		   
  }
    
  // resolve current position
  Amg::Vector3D gp = parm.position();
  if ( !m_currentStatic || !m_currentStatic->inside(gp,m_tolerance) ) {
    m_currentStatic =  m_navigator->trackingGeometry()->lowestStaticTrackingVolume(gp);    
    ATH_MSG_DEBUG( "  [+] current static volume resolved by navigator (inside): "<< 
		   positionOutput(currPar->position())<<":"<<m_currentStatic->volumeName() );
  }

  if ( m_navigator->atVolumeBoundary(currPar,m_currentStatic,dir,nextVol,m_tolerance) && nextVol != m_currentStatic ) {
    // no next volume found --- end of the world
    if ( !nextVol ) {
      ATH_MSG_DEBUG( "  [+] Word boundary reached        - at " << positionOutput(currPar->position()) );
      nextGeoID = Trk::GeometrySignature(Trk::Unsigned);
      return currPar;
    }
    ATH_MSG_DEBUG( "  [+] current static volume resolved by navigator at boundary: "<< positionOutput(currPar->position())
		   <<m_currentStatic->volumeName()<<"->"<< nextVol->volumeName()<<" with tolerance " << m_tolerance );
    m_currentStatic = nextVol;
  }

  // current frame volume known-retrieve geoID
  nextGeoID = m_currentStatic->geometrySignature(); 

  // distance to static volume boundaries recalculated
  // retrieve boundaries along path
  m_trStaticBounds.clear();
  const std::vector< SharedObject<const BoundarySurface<TrackingVolume> > > bounds = m_currentStatic->boundarySurfaces();
  for (unsigned int ib=0; ib< bounds.size(); ib++ ){
    const Trk::Surface& surf = (bounds[ib].getPtr())->surfaceRepresentation();
    Trk::DistanceSolution distSol = surf.straightLineDistanceEstimate(currPar->position(),
								      dir*currPar->momentum().normalized());
    if (distSol.numberOfSolutions()>0 && (distSol.currentDistance(false)>m_tolerance || distSol.numberOfSolutions()>1 ) && distSol.first()>m_tolerance ) {
      double dist = distSol.first();
      // resolve multiple intersection solutions
      if (distSol.numberOfSolutions()>1 && dist<m_tolerance && distSol.second()>dist ) dist = distSol.second();
      // boundary check
      Amg::Vector3D gp = currPar->position()+dist*dir*currPar->momentum().normalized();
      if ( surf.isOnSurface(gp,true,m_tolerance,m_tolerance) ) {
	m_trStaticBounds.insert(m_trStaticBounds.begin(),Trk::DestBound(&surf,dist,ib));
      }   
    }  // along path
    if (distSol.numberOfSolutions()>1 && distSol.second()>m_tolerance ) {
      double dist = distSol.second();
      // boundary check
      Amg::Vector3D gp = currPar->position()+dist*dir*currPar->momentum().unit();
      if ( surf.isOnSurface(gp,true,m_tolerance,m_tolerance) ) {
	if ( dist > m_tolerance ) {  // valid intersection
	  m_trStaticBounds.insert(m_trStaticBounds.begin(),Trk::DestBound(&surf,dist,ib));
	}
      }   
    }  // along path
  } // end loop over boundaries
  
  if ( !m_trStaticBounds.size() ) {
    ATH_MSG_WARNING( "  transportToVolumeWithPathLimit() - at " << currPar->position() <<", missing static volume boundary "
		     << m_currentStatic->volumeName() <<": transport interrupted" );

    ATH_MSG_WARNING("---> particle R,phi,z, momentum:"<< currPar->position().perp()<<","<<currPar->position().phi()<<","<<currPar->position().z() <<","<<currPar->momentum());
    ATH_MSG_WARNING("---> static volume position:"<< m_currentStatic->center());
    const Trk::CylinderVolumeBounds* cyl = dynamic_cast<const Trk::CylinderVolumeBounds*> (&(m_currentStatic->volumeBounds()));
    if (cyl) ATH_MSG_WARNING("---> cylinder volume dimensions:"<<cyl->innerRadius()<<","<<cyl->outerRadius()<<","<<cyl->halflengthZ());
  

    for (unsigned int ib=0; ib< bounds.size(); ib++ ){
      const Trk::Surface& surf = (bounds[ib].getPtr())->surfaceRepresentation();
      Trk::DistanceSolution distSol = surf.straightLineDistanceEstimate(currPar->position(),
								      dir*currPar->momentum().unit());
      ATH_MSG_WARNING("---> decomposed boundary surface position, normal, estimated distance:"<<ib<<","<<surf.center()<<","<<surf.normal());
      ATH_MSG_WARNING("---> estimated distance to (first solution):boundary check:"<<distSol.numberOfSolutions()<<","<<distSol.first()<<":"<<
		      surf.isOnSurface(currPar->position()+distSol.first()*dir*currPar->momentum().unit(),true,m_tolerance,m_tolerance));
      if (distSol.numberOfSolutions()>1)
       ATH_MSG_WARNING("---> estimated distance to (second solution):boundary check:" << distSol.second()<< ","<<
		       surf.isOnSurface(currPar->position()+distSol.second()*dir*currPar->momentum().unit(),true,m_tolerance,m_tolerance));  
    }

    return returnParameters;

  } else if (m_trStaticBounds[0].distance < m_tolerance ) {

    // TODO find out why this case (=exit from volume) haven't been handled by Navigator 
    //ATH_MSG_WARNING( " recovering from glitch at the static volume boundary:"<<m_trStaticBounds[0].distance );

    Amg::Vector3D gp = currPar->position()+ m_tolerance*dir*currPar->momentum().unit();
    m_currentStatic =  m_navigator->trackingGeometry()->lowestStaticTrackingVolume(gp);    

    if (m_currentStatic) return transportToVolumeWithPathLimit(parm, timeLim, dir, particle, nextGeoID, destVol);	  
    else {
	    ATH_MSG_DEBUG( "  [+] World boundary reached        - at " << positionOutput(currPar->position())<<", timed at " << m_time );
	    nextGeoID = Trk::GeometrySignature(Trk::Unsigned);
	    if (!destVol) { return currPar;}
    }
  }
  
  m_detachedVols.clear();
  m_trDetachBounds.clear();
  m_denseVols.clear();
  m_trDenseBounds.clear();
  m_trLays.clear();
  m_navigLays.clear();

  // new: ID volumes may have special material layers ( entry layers ) - add them here
  if (m_currentStatic->entryLayerProvider()) {
    const std::vector<const Trk::Layer*>& entryLays = m_currentStatic->entryLayerProvider()->layers();
    for (unsigned int i=0; i < entryLays.size(); i++) {
      if (entryLays[i]->layerType()>0 || entryLays[i]->layerMaterialProperties()) {
	  const Trk::Surface& surf=entryLays[i]->surfaceRepresentation(); 
	  Trk::DistanceSolution distSol = surf.straightLineDistanceEstimate(currPar->position(),
									    dir*currPar->momentum().normalized());
	  if (distSol.numberOfSolutions()>0 && distSol.first()>0.) {
	    // boundary check
	    Amg::Vector3D gp = currPar->position()+distSol.first()*dir*currPar->momentum().normalized();
	    if ( surf.isOnSurface(gp,true,0.001,0.001) ) {
	      m_trLays.push_back(std::pair<const Trk::Surface*,double>(&surf,distSol.first()));
	      m_navigLays.push_back(std::pair<const Trk::TrackingVolume*,const Trk::Layer*> (m_currentStatic,entryLays[i]) );
	    }   // valid intersection
	  }  // along path
      }
    } 
  }

  // detached volume boundaries
  const std::vector<const Trk::DetachedTrackingVolume*>* detVols = m_currentStatic->confinedDetachedVolumes();
  if (detVols) {
    std::vector<const Trk::DetachedTrackingVolume*>::const_iterator iTer = detVols->begin();
    for (; iTer != detVols->end(); iTer++) { 
      // active station ?
      const Trk::Layer* layR = (*iTer)->layerRepresentation();
      bool active = ( layR && layR->layerType() ) ? true : false;
      
      if (active) {
	// probably does not full resolution ?
	/*
          if ( resolveActive ) {
	  const std::vector< SharedObject<const BoundarySurface<TrackingVolume> > >  detBounds=
	      (*iTer)->trackingVolume()->boundarySurfaces();
	      // loop over boundaries, save only if valid intersection
	      int newB=0;
	      for (unsigned int ib=0; ib< detBounds.size(); ib++ ){
	      const Trk::Surface& surf = (detBounds[ib].getPtr())->surfaceRepresentation();
	      Trk::DistanceSolution distSol = surf.straightLineDistanceEstimate(currPar->position(),
	      dir*currPar->momentum().normalized());
	      if (distSol.numberOfSolutions()>0 && distSol.first()>0.) {
		// boundary check
		Amg::Vector3D gp = currPar->position()+distSol.first()*dir*currPar->momentum().normalized();
		if ( surf.isOnSurface(gp,true,0.001,0.001) ) {
		m_trDetachBounds.push_back(std::pair<const Trk::Surface*,double>(&surf,distSol.first()));
		newB++;
		}   // valid intersection
	      }  // along path
	      } // end loop over boundaries
	      if (newB>0) m_detachedVols.push_back(std::pair<const Trk::DetachedTrackingVolume*,unsigned int> (*iTer,newB) );
	      } else {
	*/
	if (!m_resolveMultilayers || !(*iTer)->multilayerRepresentation() ) {
	  const Trk::Surface& surf = layR->surfaceRepresentation();
	  Trk::DistanceSolution distSol = surf.straightLineDistanceEstimate(currPar->position(),
									    dir*currPar->momentum().normalized());
	  if (distSol.numberOfSolutions()>0 && distSol.first()>0.) {
	    // boundary check
	    Amg::Vector3D gp = currPar->position()+distSol.first()*dir*currPar->momentum().normalized();
	    if ( surf.isOnSurface(gp,true,0.001,0.001) ) {
	      m_trLays.push_back(std::pair<const Trk::Surface*,double>(&surf,distSol.first()));
	      m_navigLays.push_back(std::pair<const Trk::TrackingVolume*,const Trk::Layer*> ((*iTer)->trackingVolume(),layR));
	    }
	  }
	} else {
	  const std::vector<const Trk::Layer*>* multi = (*iTer)->multilayerRepresentation();
	  for (unsigned int i=0;i<multi->size();i++) {
	    const Trk::Surface& surf = (*multi)[i]->surfaceRepresentation();
	    Trk::DistanceSolution distSol = surf.straightLineDistanceEstimate(currPar->position(),
									      dir*currPar->momentum().normalized());
	    if (distSol.numberOfSolutions()>0 && distSol.first()>0.) {
	      // boundary check
	      Amg::Vector3D gp = currPar->position()+distSol.first()*dir*currPar->momentum().normalized();
	      if ( surf.isOnSurface(gp,true,0.001,0.001) ) {
		m_trLays.push_back(std::pair<const Trk::Surface*,double>(&surf,distSol.first()));
		m_navigLays.push_back(std::pair<const Trk::TrackingVolume*,const Trk::Layer*> ((*iTer)->trackingVolume(),(*multi)[i]));
	      }
	    }
	  }   // end loop over multilayers
	} // end unresolved active 
      } // active done  
      else if (m_currentStatic->geometrySignature()!=Trk::MS || !m_useMuonMatApprox ||
	       (*iTer)->name().substr((*iTer)->name().size()-4,4)=="PERM" ) {  // retrieve inert detached objects only if needed
	// dense volume boundaries
	if ((*iTer)->trackingVolume()->zOverAtimesRho()!=0. && 
	    (!(*iTer)->trackingVolume()->confinedDenseVolumes() || !(*iTer)->trackingVolume()->confinedDenseVolumes()->size() )
	    && ( !(*iTer)->trackingVolume()->confinedArbitraryLayers() ||
		 !(*iTer)->trackingVolume()->confinedArbitraryLayers()->size() ) ) {
	  const std::vector< SharedObject<const BoundarySurface<TrackingVolume> > >  detBounds=
	    (*iTer)->trackingVolume()->boundarySurfaces();
	  int newB=0;
	  for (unsigned int ibb=0; ibb< detBounds.size(); ibb++ ){
	    const Trk::Surface& surf = (detBounds[ibb].getPtr())->surfaceRepresentation();
	    Trk::DistanceSolution distSol = surf.straightLineDistanceEstimate(currPar->position(),
									dir*currPar->momentum().normalized());
	    if (distSol.numberOfSolutions()>0 && distSol.first()>0.) {
	      // boundary check
	      Amg::Vector3D gp = currPar->position()+distSol.first()*dir*currPar->momentum().normalized();
	      if ( surf.isOnSurface(gp,true,0.001,0.001) ) {
		m_trDenseBounds.push_back(std::pair<const Trk::Surface*,double>(&surf,distSol.first()));
		newB++;
	      }   // valid intersection
	    }  // along path
	  } // end loop over boundaries
	  if (newB>0) m_denseVols.push_back(std::pair<const Trk::TrackingVolume*,unsigned int> 
					    ((*iTer)->trackingVolume(),newB) );
	} 
	// subvolumes ?
	//if ((*iTer)->trackingVolume()->confinedDenseVolumes() &&  (*iTer)->trackingVolume()->confinedDenseVolumes()->size()) 
	//  ATH_MSG_WARNING( "  transportToVolumeWithPathLimit() - at " << currPar->position() <<", unresolved subvolumes for  "
	//		   << (*iTer)->trackingVolume()->volumeName() );
	
	const std::vector<const Trk::TrackingVolume*>* confinedDense = (*iTer)->trackingVolume()->confinedDenseVolumes();
        if (confinedDense && confinedDense->size()) {
	  std::vector<const Trk::TrackingVolume*>::const_iterator vIter = confinedDense->begin();
          for (; vIter!= confinedDense->end(); vIter++) {
	    const std::vector< SharedObject<const BoundarySurface<TrackingVolume> > >  bounds=(*vIter)->boundarySurfaces();
	    int newB=0;
	    for (unsigned int ibb=0; ibb< bounds.size(); ibb++ ){
	      const Trk::Surface& surf = (bounds[ibb].getPtr())->surfaceRepresentation();
	      Trk::DistanceSolution distSol = surf.straightLineDistanceEstimate(currPar->position(),
										dir*currPar->momentum().normalized());
	      if (distSol.numberOfSolutions()>0 && distSol.first()>0.) {
		// boundary check
		Amg::Vector3D gp = currPar->position()+distSol.first()*dir*currPar->momentum().normalized();
		if ( surf.isOnSurface(gp,true,0.001,0.001) ) {
		  m_trDenseBounds.push_back(std::pair<const Trk::Surface*,double>(&surf,distSol.first()));
		  newB++;
		}   // valid intersection
	      }  // along path
	    } // end loop over boundaries
	    if (newB>0) m_denseVols.push_back(std::pair<const Trk::TrackingVolume*,unsigned int> 
					      ((*vIter),newB) );
            if ((*vIter)->confinedDenseVolumes() || (*vIter)->confinedArbitraryLayers())
	      ATH_MSG_WARNING( "  transportToVolumeWithPathLimit() - at " << currPar->position() <<", unresolved sublayers/subvolumes for  "
			       << (*vIter)->volumeName() );
	  }
	}

	// confined layers
	const std::vector<const Trk::Layer*>* confLays = (*iTer)->trackingVolume()->confinedArbitraryLayers(); 
	if ( confLays ) {
	  std::vector<const Trk::Layer*>::const_iterator lIt = confLays->begin();
	  for ( ; lIt!= confLays->end(); lIt++ ){
	    const Trk::Surface& surf = (*lIt)->surfaceRepresentation();
	    Trk::DistanceSolution distSol = surf.straightLineDistanceEstimate(currPar->position(),
									dir*currPar->momentum().normalized());
	    if (distSol.numberOfSolutions()>0 && distSol.first()>0.) {
	      // boundary check
	      Amg::Vector3D gp = currPar->position()+distSol.first()*dir*currPar->momentum().normalized();
	      if ( surf.isOnSurface(gp,true,0.001,0.001) ) {
		m_trLays.push_back(std::pair<const Trk::Surface*,double>(&surf,distSol.first()));
		m_navigLays.push_back(std::pair<const Trk::TrackingVolume*,const Trk::Layer*> ((*iTer)->trackingVolume(),*lIt) );         
	      }   // valid intersection
	    }  // along path
	  } 
	}  // end confined layers       
      } // end inert material
    }
  } // end detached volumes
  m_denseResolved    = std::pair<unsigned int,unsigned int> (m_denseVols.size(), m_trDenseBounds.size());
  m_layerResolved    = m_trLays.size();

  std::vector< Trk::DestBound >::iterator bIter=m_trStaticBounds.begin();
  while (bIter != m_trStaticBounds.end()) {
    m_trSurfs.push_back(std::pair<const Trk::Surface*,double> ((*bIter).surface,(*bIter).distance));
    bIter++;
  }

  //std::cout <<"navigation in current static:"<< m_trSurfs.size()<<","<<m_trStaticBounds.size()<< std::endl;
  //for (unsigned int ib=0; ib<m_trSurfs.size(); ib++) std::cout <<"distance to static:"<< ib<<","<<m_trSurfs[ib].second<<std::endl;

  // resolve the use of dense volumes
  m_dense = (m_currentStatic->geometrySignature()==Trk::MS && m_useMuonMatApprox ) || (m_currentStatic->geometrySignature()!=Trk::MS && m_useDenseVolumeDescription );

  // reset remaining counters
  m_currentDense = m_dense ?  m_currentStatic : m_highestVolume;
  m_navigBoundaries.clear(); 
  if (m_denseVols.size()>m_denseResolved.first) {
    m_denseVols.resize(m_denseResolved.first);
    m_trDenseBounds.resize(m_denseResolved.second);
  }
  if (m_layers.size()>m_layerResolved) {
    m_trLays.resize(m_layerResolved);
    m_navigLays.resize(m_layerResolved);
  }  

  //if (m_currentStatic->entryLayerProvider()) nEntryLays = m_currentStatic->entryLayerProvider()->layers().size();

  // confined layers
  if ( m_currentStatic->confinedLayers()) {
    std::vector<const Trk::Layer*> cLays = m_currentStatic->confinedLayers()->arrayObjects();
    for (unsigned int i=0; i < cLays.size(); i++) { 
      if (cLays[i]->layerMaterialProperties()) {
	const Trk::Surface& surf = cLays[i]->surfaceRepresentation();
	Trk::DistanceSolution distSol = surf.straightLineDistanceEstimate(currPar->position(),
									  dir*currPar->momentum().normalized());
	if (distSol.numberOfSolutions()>0 && distSol.first()>0.) {
	  // boundary check
	  Amg::Vector3D gp = currPar->position()+distSol.first()*dir*currPar->momentum().normalized();
	  if ( surf.isOnSurface(gp,true,0.001,0.001) ) {
	    m_trLays.push_back(std::pair<const Trk::Surface*,double>(&surf,distSol.first()));
	    m_navigLays.push_back(std::pair<const Trk::TrackingVolume*,const Trk::Layer*> (m_currentStatic,cLays[i]) );         
	  }   // valid intersection
	}  // along path
      }
    }
  }
  
  // m_trSurfs contains destination surface (if it exists), static volume boundaries  
  // complete with TG m_layers/dynamic layers, m_denseBoundaries, m_navigBoundaries, m_detachedBoundaries

  if (m_trLays.size()) m_trSurfs.insert(m_trSurfs.end(),m_trLays.begin(),m_trLays.end());
  if (m_trDenseBounds.size()) m_trSurfs.insert(m_trSurfs.end(),m_trDenseBounds.begin(),m_trDenseBounds.end());
  if (m_trDetachBounds.size()) m_trSurfs.insert(m_trSurfs.end(),m_trDetachBounds.begin(),m_trDetachBounds.end()); 

  // current dense
  m_currentDense =  m_highestVolume;

  for (unsigned int i=0;i<m_denseVols.size(); i++) {
    const Trk::TrackingVolume* dVol = m_denseVols[i].first;
    if ( dVol->inside(currPar->position(),m_tolerance)  && dVol->zOverAtimesRho()!=0. ) {
      if ( !m_navigator->atVolumeBoundary(currPar,dVol,dir,nextVol,m_tolerance) ||
	   dVol->inside(currPar->position()+2*m_tolerance*currPar->momentum().unit(),m_tolerance) ) m_currentDense = dVol;
    } 
  }

  if (m_dense && m_currentDense==m_highestVolume) m_currentDense = m_currentStatic;
  
  // ready to process
  // 1/ order valid intersections ( already in trSurfs )
   
  std::vector<unsigned int> sols;
  for (unsigned int i=0;i<m_trSurfs.size(); i++) { sols.push_back(i); }

  if (sols.size()>1) {
    unsigned int itest=1;
    while ( itest<sols.size() ) {
      if ( m_trSurfs[sols[itest]].second < m_trSurfs[sols[itest-1]].second ) {
        unsigned int iex = sols[itest-1];
        sols[itest-1]=sols[itest];
        sols[itest]=iex;
        itest=1;
      } else itest++; 
    }
    // check ordering
    for (unsigned int is=1; is<sols.size(); is++) if (m_trSurfs[sols[is]].second<m_trSurfs[sols[is-1]].second) std::cout<<"wrong intersection ordering"<< std::endl;
  }


  // 2/ check time/material/boundary limit
      
  // update of m_navigSurfs required if I/ entry into new navig volume, II/ exit from currentActive without overlaps

   nextVol = 0;
   const Trk::TrackParameters* nextPar=0;

   double dist = 0.;  
   double mom=currPar->momentum().mag(); 
   double beta = mom/sqrt(mom*mom+m_particleMass*m_particleMass)*Gaudi::Units::c_light;

   ATH_MSG_DEBUG( "  [0] starting transport of neutral particle in (dense) volume "<< m_currentDense->volumeName() );

   for (unsigned int is=0; is<sols.size(); is++) {

     if ( m_trSurfs[sols[is]].second==0. ) continue;

     double step = m_trSurfs[sols[is]].second-dist;
 
     Amg::Vector3D nextPos = currPar->position()+dir*currPar->momentum().normalized()*m_trSurfs[sols[is]].second;
     //Amg::Vector3D halfStep = nextPos - 0.5*step*dir*currPar->momentum().normalized();

     // check missing volume boundary 
     if ( !(m_currentDense->inside(nextPos,m_tolerance) ) ) {
       ATH_MSG_DEBUG( "  [!] ERROR: missing volume boundary for volume"<< m_currentDense->volumeName() );
       // new search
       m_currentDense =  m_highestVolume;
       for (unsigned int i=0;i<m_denseVols.size(); i++) {
	 const Trk::TrackingVolume* dVol = m_denseVols[i].first;
	 if ( dVol->inside(nextPos,m_tolerance)  && dVol->zOverAtimesRho()!=0. ) m_currentDense = dVol; 
       }
       if (m_dense && m_currentDense==m_highestVolume) m_currentDense = m_currentStatic;

       ATH_MSG_DEBUG( "  [!] new search for dense volume : "<< m_currentDense->volumeName() );
     }
     
     double tDelta = step/beta;
     
     double mDelta = (m_currentDense->zOverAtimesRho() != 0.) ? step/m_currentDense->x0() : 0.;

     // in case of hadronic interaction retrieve nuclear interaction properties, too
     
     double frT = 1.;
     if (step>0 && timeLim.tMax>m_time && m_time+tDelta >= timeLim.tMax) frT = (timeLim.tMax-m_time)*beta/step;
     
     // TODO : compare x0 or l0 according to the process type     
     double frM = 1.;
     if (mDelta>0 && m_path.x0Max > 0.) {
       if (m_path.process < 100 && m_path.x0Collected+mDelta > m_path.x0Max ) frM = (m_path.x0Max-m_path.x0Collected)/mDelta;
       else {     // waiting for hadronic interaction,  retrieve nuclear interaction properties
         double mDeltaL = m_currentDense->L0 > 0. ? step/m_currentDense->L0 : mDelta/0.37/m_currentDense->averageZ();
	 if (m_path.l0Collected+mDeltaL > m_path.x0Max)   frM = (m_path.x0Max-m_path.l0Collected)/mDeltaL;
       }
     }

     double fr = fmin(frT,frM);

     //std::cout << "looping over intersections:"<<is<<","<< m_trSurfs[sols[is]].second<<","<<step << ","<< tDelta<<","<<mDelta << std::endl;

     if (fr<1.) { // decay or material interaction during the step

       int process = frT < frM ? timeLim.process : m_path.process;
       m_time += fr*step/beta; 
       if (mDelta>0 && m_currentDense->averageZ()>0) m_path.updateMat( fr*mDelta, m_currentDense->averageZ(), 0.); 

       nextPos = currPar->position()+dir*currPar->momentum().normalized()*(dist+fr*step);

       // process interaction only if creation of secondaries allowed
       if (m_currentStatic->geometrySignature()==Trk::ID || m_caloMsSecondary ) {

	 const Trk::TrackParameters* nextPar = m_updators[0]->interact(m_time,nextPos,currPar->momentum(),particle,process,m_currentDense);
	 
	 if (nextPar) ATH_MSG_DEBUG( " [!] WARNING: particle survives the interaction "<< process ); 
	 
	 if (nextPar && process==121) {
	   ATH_MSG_DEBUG( " [!] WARNING: failed hadronic interaction, killing the input particle anyway" );
	   return returnParameters;
	 }
   
         if (!nextPar) return returnParameters;
  
         throwIntoGarbageBin(nextPar);
         //return transportToVolumeWithPathLimit(*nextPar, timeLim, dir, particle, nextGeoID, destVol);
       } else {  // kill particle without trace
	   return returnParameters;
       }    

     }  // end decay or material interaction durign the step
 
     // update
     dist = m_trSurfs[sols[is]].second; 
     if (mDelta>0 && m_currentDense->averageZ()>0) m_path.updateMat( mDelta, m_currentDense->averageZ(), 0.); 
     m_time += tDelta;

     nextPar = new Trk::CurvilinearParameters(nextPos,currPar->momentum(),1.);  // fake charge
     throwIntoGarbageBin(nextPar);

     if (sols[is]<iDest) {      // destination volume (most often, subdetector boundary)
       return nextPar;          
     } else if ( sols[is] < iDest + m_trStaticBounds.size() ) {     // tracking geometry frame
       // static volume boundary; return to the main loop 
       unsigned int index = m_trStaticBounds[sols[is]-iDest].bIndex;
       // use global coordinates to retrieve attached volume (just for static!)
       nextVol = (m_currentStatic->boundarySurfaces())[index].getPtr()->attachedVolume(nextPar->position(),nextPar->momentum(),dir);
       // double check the next volume
       if ( nextVol && !(nextVol->inside(nextPar->position()+0.01*nextPar->momentum().normalized(),m_tolerance) ) ) {
	 ATH_MSG_DEBUG( "  [!] WARNING: wrongly assigned static volume ?"<< m_currentStatic->volumeName()<<"->" << nextVol->volumeName() );
	 nextVol = m_navigator->trackingGeometry()->lowestStaticTrackingVolume(nextPar->position()+0.01*nextPar->momentum().normalized());
	 if (nextVol) ATH_MSG_DEBUG( "  new search yields: "<< nextVol->volumeName() );          
       }
       // end double check - to be removed after validation of the geometry gluing 
       if (nextVol != m_currentStatic ) {
	 ATH_MSG_DEBUG("  [+] StaticVol boundary reached of '" <<m_currentStatic->volumeName() << "'.");
	 if ( m_navigator->atVolumeBoundary(nextPar,m_currentStatic,dir,assocVol,m_tolerance) && assocVol != m_currentStatic )
	   m_currentDense = m_dense ? nextVol : m_highestVolume;
	 // no next volume found --- end of the world
	 if ( !nextVol ) {
	   ATH_MSG_DEBUG( "  [+] World boundary reached        - at " << positionOutput(nextPar->position())<<", timed at " << m_time );
	   nextGeoID = Trk::GeometrySignature(Trk::Unsigned);
	   if (!destVol) { return nextPar;}
	 }
	 // next volume found and parameters are at boundary
	 if ( nextVol && nextPar ){ 
	   ATH_MSG_DEBUG( "  [+] Crossing to next volume '" << nextVol->volumeName() << "'");
	   ATH_MSG_DEBUG( "  [+] Crossing position is         - at " <<  positionOutput(nextPar->position()) );
	   if (!destVol && m_currentStatic->geometrySignature()!=nextVol->geometrySignature())
	     { nextGeoID=nextVol->geometrySignature(); return nextPar; }   
	 }     
	 return transportToVolumeWithPathLimit(*nextPar, timeLim, dir, particle, nextGeoID, destVol);
       }
       //if (dist>0.) return transportToVolumeWithPathLimit(*nextPar, timeLim, dir, particle, nextGeoID, destVol);
	 
     } else if ( sols[is] < iDest + m_trStaticBounds.size() + m_trLays.size() ) {     // layer

       // material thickness - simple approach
       unsigned int index = sols[is]-iDest-m_trStaticBounds.size();
       const Trk::Layer* nextLayer = m_navigLays[index].second;

       bool matUp = nextLayer->fullUpdateMaterialProperties(*nextPar) && m_includeMaterialEffects;
       // identical to last material layer ?
       if (matUp && nextLayer==m_lastMaterialLayer ) {
	 // cylinder layers !
	 const Trk::CylinderLayer* cyl = dynamic_cast<const Trk::CylinderLayer*> (nextLayer);
	 const Trk::SubtractedCylinderLayer* scyl = dynamic_cast<const Trk::SubtractedCylinderLayer*> (nextLayer);
	 if (!cyl && !scyl) matUp = false;            
       }

       // material update        
       if (matUp) {

	 const ITimedMatEffUpdator* currentUpdator = subMaterialEffectsUpdator(*m_currentStatic);

	 nextPar =  currentUpdator ?
	   currentUpdator->update(nextPar, *nextLayer, timeLim, m_path, m_currentStatic->geometrySignature(), dir, particle) : nextPar;

         if (!nextPar) {
	     ATH_MSG_VERBOSE( "  [+] Update may have killed neutral track - return." );
	     m_parametersAtBoundary.resetBoundaryInformation();
	     return returnParameters;
         }
      
       } 

     } else if ( sols[is] < iDest + m_trStaticBounds.size() + m_trLays.size() + m_trDenseBounds.size() ) {

       // dense volume boundary : no material update here, navigation only ( set m_currentDense for next step )

       unsigned int index = sols[is] - iDest -m_trStaticBounds.size()- m_trLays.size();
       std::vector< std::pair<const Trk::TrackingVolume*,unsigned int> >::iterator dIter = m_denseVols.begin();
       while ( index >= (*dIter).second && dIter!= m_denseVols.end() ) {
	 index -= (*dIter).second ; 
	 dIter++;
       }
       if ( dIter != m_denseVols.end() ) {
	 currVol = (*dIter).first;

         if ( m_navigator->trackingGeometry()->atVolumeBoundary(nextPos,nextPar->momentum(), currVol,assocVol, dir,m_tolerance) ) {
            if ( assocVol && assocVol->zOverAtimesRho()!= 0. ) m_currentDense = assocVol;
            else if ( currVol->inside(nextPos+0.002*dir*nextPar->momentum().normalized()) ) m_currentDense = currVol;
            else {
	      // new search
	      m_currentDense =  m_highestVolume;
	      if (m_useMuonMatApprox && !m_denseVols.size()) m_currentDense = m_currentStatic;
	      else {
		for (unsigned int i=0;i<m_denseVols.size(); i++) {
		  const Trk::TrackingVolume* dVol = m_denseVols[i].first;
		  if ( dVol->inside(nextPos+0.002*dir*nextPar->momentum().normalized(),m_tolerance)  && dVol->zOverAtimesRho()!=0. ) m_currentDense = dVol; 
		}
	      }
	    }
	 }
       }
     } else {   // detached volume bounds - not relevant ?
       
     }

     throwIntoGarbageBin(nextPar);

   }

   ATH_MSG_WARNING( "  transportToVolumeWithPathLimit() - return with navigation break from volume "<<  m_currentStatic->volumeName()   );

   return nextPar;

}
