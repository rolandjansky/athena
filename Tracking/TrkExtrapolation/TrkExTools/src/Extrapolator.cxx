/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// Extrapolator.cxx, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

#include "GaudiKernel/MsgStream.h"
// Trk inlcude
#include "TrkExTools/Extrapolator.h"
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
#include "TrkGeometry/DetachedTrackingVolume.h"
#include "TrkGeometry/AlignableTrackingVolume.h"
#include "TrkGeometry/Layer.h"
#include "TrkGeometry/CompoundLayer.h"
#include "TrkGeometry/CylinderLayer.h"
#include "TrkGeometry/SubtractedCylinderLayer.h"
#include "TrkGeometry/TrackingGeometry.h"
#include "TrkVolumes/BoundarySurface.h"
#include "TrkVolumes/BoundarySurfaceFace.h"
#include "TrkVolumes/Volume.h"
#include "TrkParticleBase/TrackParticleBase.h"
#include "TrkEventUtils/TrkParametersComparisonFunction.h"
#include "TrkDetDescrUtils/SharedObject.h"
#include "TrkDetDescrUtils/GeometrySignature.h"
#include "TrkMaterialOnTrack/EnergyLoss.h"
#include "TrkMaterialOnTrack/ScatteringAngles.h"
//#include "TrkParameters/CurvilinearParameters.h"
#include "TrkParameters/TrackParameters.h"
#include "TrkExUtils/ExtrapolationCache.h"
#include "CxxUtils/make_unique.h"
// for the comparison with a pointer
#include <stdint.h>

// Amg
#include "EventPrimitives/EventPrimitives.h"
#include "GeoPrimitives/GeoPrimitives.h"

// Trk
#include "TrkSurfaces/PlaneSurface.h"

#include <memory>

// screen output measures
// "[+] Text describing layer      - with " << layerRZoutput()
// "[+] Text describing position   -   at " << positionOutput()
// "[+] Text describing single parameter  - par = "

// reference surface for Blind extrapolation
//Trk::PlaneSurface Trk::Extrapolator::s_referenceSurface(new Amg::Transform3D(Trk::s_idTransform), 0.,0.);
double Trk::Extrapolator::s_distIncreaseTolerance = 100. * Gaudi::Units::millimeter;
// constructor
Trk::Extrapolator::Extrapolator(const std::string& t, const std::string& n, const IInterface* p) :
  AthAlgTool(t,n,p),
  m_propagators(),
  m_stepPropagator("Trk::STEP_Propagator/AtlasSTEP_Propagator"), 
  m_navigator("Trk::Navigator/AtlasNavigator"), 
  m_updators(),
  m_msupdators(),
  m_elossupdators(),
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
  m_activeOverlap(false),
  m_useMuonMatApprox(false),
  m_useDenseVolumeDescription(true),
  m_checkForCompundLayers(false),
  m_destinationSurface(0),
  m_boundaryVolume(0),
  m_recall(false),
  m_recallSurface(0),
  m_recallLayer(0),
  m_recallTrackingVolume(0),
  m_parametersOnDetElements(0),
  m_lastValidParameters(0),
  m_currentStatic(0),
  m_currentDense(0),
  m_highestVolume(0),
  m_resolveActive(false),
  m_resolveMultilayers(true),
  m_returnPassiveLayers(false),
  m_robustSampling(true),
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
  m_matstates(0),
  m_jacs(0),
  m_extrapolationCache(0),
  m_cacheEloss(0),
  m_dumpCache(false),
  m_fastField(false)
{
      declareInterface<IExtrapolator>(this);
      
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
      // muon system specifics            

      declareProperty("UseMuonMatApproximation",        m_useMuonMatApprox);
      declareProperty("UseDenseVolumeDescription",      m_useDenseVolumeDescription);
      declareProperty("CheckForCompoundLayers",         m_checkForCompundLayers);
      declareProperty("ResolveMuonStation",             m_resolveActive);
      declareProperty("ResolveMultilayers",             m_resolveMultilayers);
      declareProperty("ConsiderMuonStationOverlaps",    m_activeOverlap);  
      declareProperty("RobustSampling",            m_robustSampling);  
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
      //Magnetic field properties
      declareProperty("DumpCache",      m_dumpCache);  
      declareProperty("MagneticFieldProperties",  m_fastField);  
}

// destructor
Trk::Extrapolator::~Extrapolator()
{}

// Athena standard methods
// initialize
StatusCode Trk::Extrapolator::initialize()
{   

    s_referenceSurface=new Trk::PlaneSurface(new Amg::Transform3D(Trk::s_idTransform), 0.,0.);
    s_referenceSurface->setOwner(Trk::TGOwn);

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

    m_maxNavigSurf = 1000;
    m_navigSurfs.reserve(m_maxNavigSurf); 
    m_maxNavigVol = 50;
    m_navigVols.reserve(m_maxNavigVol); 
    m_navigVolsInt.reserve(m_maxNavigVol); 

       
    ATH_MSG_INFO("initialize() successful" );    
    return StatusCode::SUCCESS;
}

// finalize
StatusCode Trk::Extrapolator::finalize()
{
    
  if (m_navigationStatistics || msgLvl(MSG::DEBUG)){
    ATH_MSG_INFO(   " Perfomance Statistics  : "                                                          );
    ATH_MSG_INFO(   " [P] Methode Statistics ------- -----------------------------------------------------------"  );
    ATH_MSG_INFO(   "     -> Number of extrapolate() calls                : " << m_extrapolateCalls               );
    ATH_MSG_INFO(   "     -> Number of extrapolateBlindly() calls         : " << m_extrapolateBlindlyCalls        );
    ATH_MSG_INFO(   "     -> Number of extrapolateDirectly() calls        : " << m_extrapolateDirectlyCalls       );
    ATH_MSG_INFO(   "     -> Number of extrapolateStepwise() calls        : " << m_extrapolateStepwiseCalls       );
    ATH_MSG_INFO(   "     -> Number of layers switched in layer2layer     : " << m_layerSwitched                  );
    ATH_MSG_INFO(   "[P] Navigation Initialization ------------------------------------------------------------"  );
    ATH_MSG_INFO(   "      -> Number of start associations                : " << m_startThroughAssociation        ); 
    ATH_MSG_INFO(   "      -> Number of start recalls                     : " << m_startThroughRecall             ); 
    ATH_MSG_INFO(   "      -> Number of start global searches             : " << m_startThroughGlobalSearch       );
    ATH_MSG_INFO(   "      -> Number of destination associations          : " << m_destinationThroughAssociation  ); 
    ATH_MSG_INFO(   "      -> Number of destination recalls               : " << m_destinationThroughRecall       ); 
    ATH_MSG_INFO(   "      -> Number of destination global searches       : " << m_destinationThroughGlobalSearch ); 
    ATH_MSG_INFO(   "[P] Navigation Breaks --------------------------------------------------------------------"  );
    ATH_MSG_INFO(   "     -> Number of navigation breaks: loop            : " << m_navigationBreakLoop            );
    ATH_MSG_INFO(   "     -> Number of navigation breaks: oscillation     : " << m_navigationBreakOscillation     );
    ATH_MSG_INFO(   "     -> Number of navigation breaks: no volume found : " << m_navigationBreakNoVolume        );
    ATH_MSG_INFO(   "     -> Number of navigation breaks: dist. increase  : " << m_navigationBreakDistIncrease    );
    ATH_MSG_INFO(   "     -> Number of navigation breaks: dist. increase  : " << m_navigationBreakVolumeSignature );    
    if (m_navigationBreakDetails && msgLvl(MSG::DEBUG)){
      ATH_MSG_DEBUG(   "   Detailed output for Navigation breaks             : "                                   );        
      ATH_MSG_DEBUG(   "    o " << m_navigationBreakLoop << " loops occured in the following volumes:    "         );        
      std::map<const Trk::TrackingVolume*,int>::iterator volIter = m_loopVolumes.begin();
      std::map<const Trk::TrackingVolume*,int>::iterator volIterEnd = m_loopVolumes.end();
      for ( ; volIter != volIterEnd; ++volIter)
        ATH_MSG_DEBUG(   "          - " << volIter->second << '\t' << " times in '" << (volIter->first)->volumeName() << "'" );
      ATH_MSG_DEBUG(   "    o " << m_navigationBreakOscillation << " osillations occured in following volumes: "   );
      volIter = m_oscillationVolumes.begin();
      volIterEnd = m_oscillationVolumes.end();
      for ( ; volIter != volIterEnd; ++volIter)
        ATH_MSG_DEBUG(   "          - " << volIter->second << '\t' << " times in '" << (volIter->first)->volumeName() << "'" );
      ATH_MSG_DEBUG(   "    o " << m_navigationBreakNoVolume << " times no next volume found of  volumes: "        );       
      volIter = m_noNextVolumes.begin();
      volIterEnd = m_noNextVolumes.end();
      for ( ; volIter != volIterEnd; ++volIter)
        ATH_MSG_DEBUG(   "          - " << volIter->second << '\t' << " times in '" << (volIter->first)->volumeName() << "'" );
      ATH_MSG_DEBUG(   "    o " << m_navigationBreakDistIncrease << " distance increases detected at volumes: "    );        
      volIter = m_distIncreaseVolumes.begin();
      volIterEnd = m_distIncreaseVolumes.end();
      for ( ; volIter != volIterEnd; ++volIter)
         ATH_MSG_DEBUG(   "          - " << volIter->second << '\t' << " times in '" << (volIter->first)->volumeName() << "'" );
      ATH_MSG_DEBUG(   "    o " << m_navigationBreakVolumeSignature << " no propagator configured for volumes: "    );       
      volIter = m_volSignatureVolumes.begin();
      volIterEnd = m_volSignatureVolumes.end();
      for ( ; volIter != volIterEnd; ++volIter)
         ATH_MSG_DEBUG(   "          - " << volIter->second << '\t' << " times in '" << (volIter->first)->volumeName() << "'" );      
    }
    // validation of the overlap search
    ATH_MSG_INFO(   "[P] Overlaps found -----------------------------------------------------------------------"   );
    ATH_MSG_INFO(   "     -> Number of overlap Surface hit                : " << m_overlapSurfaceHit               );
    ATH_MSG_INFO(   " -----------------------------------------------------------------------------------------"   ); 
    // validation of the material collection methods
    if (m_materialEffectsOnTrackValidation){
      ATH_MSG_INFO(   "[P] MaterialEffectsOnTrack collection ----------------------------------------------------" );
      ATH_MSG_INFO(   "     -> Forward successful/calls (ratio)           : " << m_meotSearchSuccessfulFw << "/" 
                             <<  m_meotSearchCallsFw<< " (" << double(m_meotSearchSuccessfulFw)/m_meotSearchCallsFw << ")"  );
      ATH_MSG_INFO(   "     -> Backward successful/calls (ratio)          : " << m_meotSearchSuccessfulBw << "/" 
                             <<  m_meotSearchCallsBw<< " (" << double(m_meotSearchSuccessfulBw)/m_meotSearchCallsBw << ")"  );
      ATH_MSG_INFO(   " -----------------------------------------------------------------------------------------" );         
    }
  }  
 
  delete s_referenceSurface;

  emptyGarbageBin();

  ATH_MSG_INFO( "finalize() successful" );
  return StatusCode::SUCCESS;
}

const Trk::NeutralParameters* Trk::Extrapolator::extrapolate(const xAOD::NeutralParticle& xnParticle,
                                                             const Surface& sf,
                                                             PropDirection dir,
                                                             BoundaryCheck bcheck) const
{
    const Trk::NeutralPerigee& nPerigee = xnParticle.perigeeParameters();
    return extrapolate(nPerigee,sf,dir,bcheck);
}


const Trk::TrackParameters* Trk::Extrapolator::extrapolate(const xAOD::TrackParticle& xtParticle,
                                                           const Surface& sf,
                                                           PropDirection dir,
                                                           BoundaryCheck bcheck,
                                                           ParticleHypothesis particle,
                                                           MaterialUpdateMode matupmode) const
{

   const Trk::Perigee& tPerigee = xtParticle.perigeeParameters();
   //!< @TODO: search for closest parameter in on new curvilinear x/y/z and surface distance ... 
   // ... for the moment ... take the perigee
   return  extrapolate(tPerigee, sf, dir, bcheck, particle, matupmode); 

}



const Trk::TrackParameters* Trk::Extrapolator::extrapolate(const TrackParticleBase& particleBase,
                                                           const Surface& sf,
                                                           PropDirection dir,
                                                           BoundaryCheck bcheck,
                                                           ParticleHypothesis particle,
                                                           MaterialUpdateMode matupmode) const
{

  const Trk::TrackParameters*  closestParameters = 0;
      
  const Trk::CylinderSurface* ccsf = dynamic_cast<const Trk::CylinderSurface*>(&sf);
  if (ccsf) {
   Trk::ComparisonFunction<TrackParameters> tParFinderCylinder(ccsf->bounds().r());
   closestParameters = *(std::min_element(particleBase.trackParameters().begin(), particleBase.trackParameters().end(), tParFinderCylinder));
  } else {
  
    const Trk::StraightLineSurface* slsf = dynamic_cast<const Trk::StraightLineSurface*>(&sf);
    const Trk::PerigeeSurface*      persf = 0;
    if (!slsf) persf = dynamic_cast<const Trk::PerigeeSurface*>(&sf);
  
    if (slsf || persf) {
     Trk::ComparisonFunction<TrackParameters> tParFinderLine(sf.center(), sf.transform().rotation().col(2));
     closestParameters = *(std::min_element(particleBase.trackParameters().begin(), particleBase.trackParameters().end(), tParFinderLine));
    }

  }
  if (!closestParameters){
     Trk::ComparisonFunction<TrackParameters> tParFinderCenter(sf.center());
     closestParameters = *(std::min_element(particleBase.trackParameters().begin(), particleBase.trackParameters().end(), tParFinderCenter));
  }
  return closestParameters ? extrapolate(*closestParameters, sf, dir, bcheck, particle, matupmode) : 0;
}
const Trk::NeutralParameters* Trk::Extrapolator::extrapolate(const NeutralParameters& parameters,
                                              const Surface& sf,
                                              PropDirection dir,
                                              BoundaryCheck bcheck) const
{

   if (m_configurationLevel<10){
      const IPropagator* currentPropagator = m_subPropagators.size() ? m_subPropagators[Trk::Global] : 0;
      if (currentPropagator) return currentPropagator->propagate(parameters, sf, dir, bcheck);
   }
   ATH_MSG_ERROR( "  [!] No default Propagator is configured ! Please check jobOptions." );
   return 0;

}
// Stratetgy Pattern extrapolation methods ---------------------------------------------------------------------------------------/
const Trk::TrackParameters*  Trk::Extrapolator::extrapolate(const IPropagator& prop,
                                                            const Trk::TrackParameters& parm,
                                                            const Trk::Surface& sf,
                                                            Trk::PropDirection dir,
                                                            Trk::BoundaryCheck bcheck,
                                                            Trk::ParticleHypothesis particle,
                                                            MaterialUpdateMode matupmode) const
{ 
    // set the model action of the material effects updators
    for (unsigned int imueot = 0; imueot < m_subUpdators.size(); ++imueot) 
      m_subUpdators[imueot]->modelAction();      
      
    // reset the destination surface
    m_destinationSurface = 0;
    m_lastValidParameters = 0;
    // skip rest of navigation if particle hypothesis is nonInteracting
    if (particle==Trk::nonInteracting || int(dir) > 5) return extrapolateDirectly(prop,parm,sf,dir,bcheck,particle);

   // statistics && sequence output ----------------------------------------
    ++m_extrapolateCalls;
    ++m_methodSequence;
    // the resultParameters
    const Trk::TrackParameters* resultParameters = 0;
    // prepare the values for the startup and call the initialization ------------------------------------------
    const Trk::TrackingVolume*   startVolume     = 0;
    const Trk::TrackingVolume*   destVolume      = 0;
    const Trk::Layer*            nextLayer       = 0; 
    const Trk::TrackingVolume*   nextVolume      = 0;
    const Trk::TrackingVolume*   lastVolume      = 0;
    const Trk::TrackParameters*  refParameters   = 0;
    const Trk::TrackParameters*  lastParameters  = 0;
    const Trk::TrackParameters*  navParameters   = 0;
    const Trk::TrackParameters*  nextParameters  = (&parm);
    // initialize Navigation (calls as well initialize on garbe collection) -------------------------------------
    Trk::PropDirection navDir = initializeNavigation(prop,
                                                     parm,
                                                     sf,
                                                     dir,
                                                     particle,
                                                     refParameters,
                                                     nextLayer,
                                                     nextVolume,
                                                     destVolume);
    // ----------------------------------------------------------------------------------------------------------      
    // if anyDirection has been chosen as a start directive:
    //   -> overwrite the dir with the navigation direction
    dir = (dir == Trk::anyDirection) ? navDir : dir;
    // check for consistency 
    if (dir == Trk::anyDirection || navDir != dir){ 
       // navigation could not be resolved
       ATH_MSG_VERBOSE( "  [!] Navigation direction could not be resolved, switching to extrapolateDirectly()" ); 
       // the extrapolate directly call
       resultParameters = extrapolateDirectly(prop,parm,sf,navDir,bcheck,particle);
       // return and cleanup
       return returnResult(resultParameters,refParameters);
    }
    // ----------------------------------------------------------------------------------------------------------
    startVolume = nextVolume;
    // fallback setup  ------------------------------------------------------------------------------------------
    bool fallback = false;
    // ------- initial distance estimation ----------------------------------------------------------------------
    double currentDistance  = 0.;
    double previousDistance = 0.;
    // reference parameters and distance solution: use consistently one of each
    if (refParameters){
       ATH_MSG_VERBOSE( "  [+] Reference Parameters       -   at " << positionOutput(refParameters->position()) );
       currentDistance = (refParameters->position()-parm.position()).mag();
    } else {
      // using fast but accureate sl distance from surface 
      Trk::DistanceSolution distSol = sf.straightLineDistanceEstimate(parm.position(),dir*parm.momentum().normalized());
      if (distSol.numberOfSolutions()>0 )  
        currentDistance = distSol.absClosest(); 
      else
        currentDistance = fabs(distSol.toPointOfClosestApproach());
      // VERBOSE output
    }
    ATH_MSG_VERBOSE("  [+] Initial 3D-distance to destination - d3 = " << currentDistance);  
    // and for oscillation protection ---------------------------------------------------------------------------
    const Trk::TrackingVolume*   previousVolume  = 0;
    // ----------------------------------------------------------------------------------------------------------
    std::string startVolumeName = (nextVolume) ? nextVolume->volumeName() : "Unknown (ERROR)";
    std::string destVolumeName  = destVolume ? destVolume->volumeName() : "Unknown (blind extrapolation)";
    if (msgLvl(MSG::VERBOSE)){       
      ATH_MSG_VERBOSE( "  [" << m_methodSequence << "] extrapolate() " <<  startVolumeName << " ->  "<< destVolumeName );
      ATH_MSG_VERBOSE( "  [+] Starting position determined - at " << positionOutput(parm.position()) );
      if (nextLayer)
         ATH_MSG_VERBOSE( "  [+] Starting layer determined  - with " << layerRZoutput(*nextLayer) );
    }
    
    // ----------------------------------------------------------------------------------------------------------
    const IPropagator* currentPropagator = 0;
    // ----------------- extrapolation from One Volume to the next Volume  --------------------------------------
    // the loop continues while:
    //       - nextVolume extists
    //       - nextVolume is different from lastVolume (prevent infinite loops)
    //       - nextVolume is different from destinationVolume (change to extrapolateInsideVolume)
    //       - nextParameters exist
    //       - lastVolume is different from previousVolume (prevent oscillation loop, one-time-punch-through allowed)        
    //       - the reinforced navigation can find destination parameters
    //       - the maximum method sequence is not met

    // best starting parameters
    bool updateLastValid  = true;          
    // one-time-punch-through allows for volume2 - volume1 - volume2 (cosmics)
    bool punchThroughDone = false;

    while (nextVolume && 
           nextVolume!=destVolume &&
           nextVolume != lastVolume &&
           nextParameters &&
           m_methodSequence < m_maxMethodSequence){

      // chose the propagtor type
      currentPropagator = subPropagator(*nextVolume);
      if (!currentPropagator){
         // [0] Navigation break : configuration problem or consistency problem of TrackingGeometry
         // output
         ATH_MSG_DEBUG( "  [X] Navigation break [X]"  );
         ATH_MSG_DEBUG( "          - Reason      : No Propagator found for Volume '"<< nextVolume->volumeName() << "'"  );        
         // debug statistics
         ++m_navigationBreakVolumeSignature;
         if (m_navigationBreakDetails) ++m_volSignatureVolumes[nextVolume];
         // trigger the fallback solution
         fallback = true;                  
         break;
      }
             
      // check for the distance to destination ------------------------------------------------------------------------------------
      if (updateLastValid) m_lastValidParameters = nextParameters;  
         // re-initialize (will only overwrite destVolume)
         if (nextVolume->redoNavigation()){
             delete refParameters; refParameters = 0;
             dir = initializeNavigation(*currentPropagator,
                                        *nextParameters,
                                        sf,
                                        dir,
                                        particle,
                                        refParameters,
                                        nextLayer,
                                        nextVolume,
                                        destVolume);
           }
          // avoid the oszillation
          previousVolume = lastVolume; 
          // for the next step to termine if infinite loop occurs
          lastVolume = nextVolume;
          // for memory cleanup and backup
          lastParameters = nextParameters;

          // MS specific code ------------------
          // extrapolation within detached volumes - returns parameters on destination surfaces, or boundary solution
          // handles also dense volume description : for the moment, only Calo included
          if ( nextVolume->geometrySignature()==Trk::MS || (m_useDenseVolumeDescription && nextVolume->geometrySignature()==Trk::Calo) ) {

             if (m_parametersAtBoundary.navParameters && 
                 m_parametersAtBoundary.navParameters != m_parametersAtBoundary.nextParameters) {
                 // extrapolate to volume boundary to avoid navigation break
                 const Trk::TrackParameters* nextPar = currentPropagator->propagate(*m_parametersAtBoundary.nextParameters,
                                                                                    m_parametersAtBoundary.navParameters->associatedSurface(),
                                                                                     dir, 
                                                                                     bcheck,
                                                                                     //*previousVolume,
                                                                                     m_fieldProperties,
                        particle,false,previousVolume);
                 // propagation to boundary 
                 if (nextPar) 
                    throwIntoGarbageBin(nextPar);
                  // set boundary and next parameters
                  m_parametersAtBoundary.boundaryInformation(nextVolume,nextPar,nextPar);
                  nextParameters = m_parametersAtBoundary.nextParameters; 
                  navParameters  = m_parametersAtBoundary.navParameters;       
                 }
                 // start from the nextParameter (which are at volume boundary)
                if (nextParameters) {
      if (!m_stepPropagator) { 
        ATH_MSG_ERROR("extrapolation in Calo/MS called without configured STEP propagator, aborting"); 
        return 0;  
      }  
                       resultParameters = extrapolateWithinDetachedVolumes(*m_stepPropagator,
                                                                       *nextParameters,
                                                                       sf,
                                                                       *nextVolume,
                                                                       dir,
                                                                       bcheck,
                                                                       particle,
                                                                       matupmode);
                  }
               if (resultParameters){   
                  // destination reached : indicated through result parameters
                  // set the model action of the material effects updators
                  for (unsigned int imueot = 0; imueot < m_subUpdators.size(); ++imueot) m_subUpdators[imueot]->modelAction();            
                  // return the parameters at destination
                  ATH_MSG_DEBUG( "  [+] Destination surface successfully hit.");
                  // return the result (succesful)
                  return returnResult(resultParameters,refParameters);
                } else if (!m_parametersAtBoundary.nextParameters || !m_parametersAtBoundary.nextVolume) {
                  ATH_MSG_DEBUG( "  [-] Destination surface could not be hit.");
                  return returnResult(resultParameters,refParameters);
                 }
            } else {
           // ------------------------------------------------------------------------------------------------
           // standard loop over volumes (but last one)
           // extrapolate to volume boundary - void method as 'm_parametersAtBoundary' hold the information
           extrapolateToVolumeBoundary(*currentPropagator,
                                       *nextParameters,
                                        nextLayer,
                                        *nextVolume,
                                        dir,
                                        bcheck,
                                        particle,
                                        matupmode);
     }        
     // go on with the next volume / get next Volume and Boundary from the private member
     nextVolume     = m_parametersAtBoundary.nextVolume;
     nextParameters = m_parametersAtBoundary.nextParameters;
     navParameters  = m_parametersAtBoundary.navParameters;     
     // new distance estimation ( after step to next volume ) ------------------------------------------------
     previousDistance = currentDistance;
     // make it either from the navParmaters (if the exist) or the nextParameters
     const Trk::TrackParameters* distParameters = m_parametersAtBoundary.navParameters ? 
           m_parametersAtBoundary.navParameters : nextParameters;

     if (distParameters){
        // use consistently either the:
        // (A) reference parameters or the 
        if (refParameters)
            currentDistance = (refParameters->position()-distParameters->position()).mag();
        else {
        // (B) distance solution to surface    
           Trk::DistanceSolution newDistSol = sf.straightLineDistanceEstimate(distParameters->position(),
                                                                              dir*distParameters->momentum().normalized());
           currentDistance = newDistSol.numberOfSolutions() > 0 ? newDistSol.absClosest() : fabs(newDistSol.toPointOfClosestApproach());       
        }
    }          
    ATH_MSG_VERBOSE("  [+] New 3D-distance to destination     - d3 = " << currentDistance << " (from " << 
      ( m_parametersAtBoundary.navParameters ? "boundary parameters" : "last parameters within volume " ) << ")");    
      
    // -------------------------------------------------------------------------------------------------------
    // (1) NAVIGATION BREAK : next Volume is identical to last volume -- LOOP
    if (nextVolume == lastVolume && nextVolume){
        // output         
         ATH_MSG_DEBUG( "  [X] Navigation break [X]"  );
         ATH_MSG_DEBUG( "          - Reason      : Loop detected in TrackingVolume '"<< nextVolume->volumeName() << "'"  );         
         // statistics
          ++m_navigationBreakLoop;
         // record the oscillation volume -- increase the counter for the volume
         if (m_navigationBreakDetails) ++m_loopVolumes[nextVolume];
         // fallback flag
         fallback = true;
         // break it
         break;
    }
    // (2) NAVIGATION BREAK : Oscillation 
    else if (nextVolume == previousVolume && nextVolume) {
         // one time the loop oscillation has been allowed already
        if (punchThroughDone){
           // output
           ATH_MSG_DEBUG( "  [X] Navigation break [X]"  );
           ATH_MSG_DEBUG( "          - Reason      : Oscillation detected in TrackingVolume '"<< nextVolume->volumeName() << "'"  );           
           // statistics
           ++m_navigationBreakOscillation;
           // record the oscillation volume -- increase the counter for the volume
           if (m_navigationBreakDetails) ++m_oscillationVolumes[nextVolume];
           // fallback flag
           fallback = true;
           // break it
           break;
        } else {
          // set the punch-through to true
          punchThroughDone = true;
          ATH_MSG_DEBUG( "  [!] One time punch-through a volume done." );        
        }  
           
    }
    // ------------------- the output interpretationn of the extrapolateToVolumeBoundary
    // (3) NAVIGATION BREAK : no nextVolume found - but not in extrapolateBlindly() mode 
    else if (!nextVolume && !m_parametersOnDetElements && lastVolume && !m_stopWithUpdateZero){
           // output
           ATH_MSG_VERBOSE( "  [X] Navigation break [X]"  );
           ATH_MSG_VERBOSE( "          - Reason      : No next volume found of TrackingVolume '"<< lastVolume->volumeName() << "'"  );
           // statistics
           ++m_navigationBreakNoVolume;
           // record the "no next" volume -- increase the counter for the (last) volume
           if (m_navigationBreakDetails) ++m_noNextVolumes[lastVolume];
           // fallback flag
           fallback = true;
           // break it
         break;
    }
    // ------------------- the output interpretationn of the extrapolateToVolumeBoundary
    // (4) NAVIGATION BREAK : // nextParameters found but distance to surface increases      
     else if (nextParameters 
              && !m_parametersOnDetElements 
              && navParameters
              && nextVolume
              && currentDistance > s_distIncreaseTolerance+previousDistance){
         
          // output
              ATH_MSG_DEBUG( "  [X] Navigation break [X]"  );
              ATH_MSG_DEBUG( "          - Reason      : Distance increase [ " << previousDistance << " to "
                               << currentDistance
                               << "] in TrackingVolume '"<< nextVolume->volumeName() << "'"  );
          // statistics
          ++m_navigationBreakDistIncrease; 
          // record the "dist increase" volume -- increase the counter for the volume
          if (m_navigationBreakDetails) ++m_distIncreaseVolumes[nextVolume];           
          // fallback flag
          fallback = true;
          break;           
     }
    // ------------------- the output interpretationn of the extrapolateToVolumeBoundary
    // (+) update killed track    
    else if ( (!nextParameters && m_stopWithUpdateZero) || !nextVolume ){ 
         ATH_MSG_DEBUG( "  [+] Navigation stop : either the update killed the track, or end of detector/boundary volume reached" );
         return returnResult(resultParameters,refParameters);
    }
    // ------------------- the output interpretationn of the extrapolateToVolumeBoundary
    // (+) end of extrapolate blindly(volume*)
    else if (m_boundaryVolume 
             && navParameters
             && !(m_boundaryVolume->inside(navParameters->position()) ) ) {
        ATH_MSG_DEBUG( "  [+] Navigation stop : next navigation step would lead outside given boundary volume" );
        return returnResult(resultParameters,refParameters);
    }        
    // ------------------- the output interpretationn of the extrapolateToVolumeBoundary
    // (5) NAVIGATION BREAK : // nextParameters found but distance to surface increases 
    else if (nextVolume)
         ATH_MSG_DEBUG( "  [+] next Tracking Volume = " << nextVolume->volumeName() );   
      // set validity of last parameters to cache -----------------------------------------------------------------     
      if (nextParameters 
          && !m_parametersOnDetElements 
          && navParameters 
          && nextVolume
          && currentDistance > previousDistance) 
              updateLastValid = false;
      else 
             updateLastValid = true;
      // reset
      nextParameters = nextParameters ? nextParameters : lastParameters;
      // one volume step invalidates the nextLayer information
      nextLayer = 0;
   }
    
   // ------------------- fallback was triggered in volume to volume loop -------------------------------------- 
   if (fallback){
       // continue with the output
       ATH_MSG_DEBUG( "          - Consequence : "
           << (  m_stopWithNavigationBreak ? "return 0 (configured) " : "switch to extrapolateDirectly() ")   );           
       // stop with navigaiton break or zero update
       if (m_stopWithNavigationBreak || m_stopWithUpdateZero) {
         emptyGarbageBin();
         return 0;     
       }       
       // cleanup the garbage
       if(m_lastValidParameters && lastVolume)  currentPropagator = subPropagator(*lastVolume);
       if (!currentPropagator) return returnResult(resultParameters,refParameters);
       // create the result now
       resultParameters = currentPropagator->propagate(*m_lastValidParameters,sf,Trk::anyDirection,bcheck,m_fieldProperties,particle,false,lastVolume);
       // desperate try 
       resultParameters = resultParameters ? resultParameters : currentPropagator->propagate(parm,sf,dir,bcheck,m_fieldProperties,particle,false,startVolume);
       return returnResult(resultParameters,refParameters);
    }

   // ----------------- this is the exit of the extrapolateBlindly() call --------------------------------------
   if ( (&sf) == (s_referenceSurface) ) return returnResult(resultParameters,refParameters);

   // ---------------- extrapolation inside the Volume ----------------------------------------------------------
   if (nextVolume){ 
   
       // chose the propagator fromt he geometry signature
       currentPropagator = subPropagator(*nextVolume);
       // extrapolate inside the volume       
       if (currentPropagator) 
          resultParameters = extrapolateInsideVolume(*currentPropagator,
                                                     *nextParameters,
                                                     sf,
                                                     nextLayer,
                                                     *nextVolume,
                                                     dir,
                                                     bcheck,
                                                     particle,
                                                     matupmode);
                                                                                                          
    }
   // ------------------------------------------------------------------------------------------------------------ 
   // the final - desperate backup --- just try to hit the surface 
   if (!resultParameters && !m_stopWithNavigationBreak && !m_stopWithUpdateZero){
        ATH_MSG_DEBUG( "  [-] Fallback to extrapolateDirectly triggered ! " );
        resultParameters = prop.propagate(*nextParameters,
                                          sf,
                                          dir,
                                          bcheck,
                                          //*startVolume,
                                          m_fieldProperties,
                                          particle, false, startVolume);
   }
   // return whatever you have
   return returnResult(resultParameters,refParameters); 
}

const std::vector<const Trk::TrackParameters*>* Trk::Extrapolator::extrapolateStepwise(
                                                            const IPropagator& prop,
                                                            const Trk::TrackParameters& parm,
                                                            const Trk::Surface& sf,
                                                            Trk::PropDirection dir,
                                                            Trk::BoundaryCheck bcheck,
                                                            Trk::ParticleHypothesis particle) const
{
   // statistics && sequence output ----------------------------------------
   ++m_extrapolateStepwiseCalls;
   ++m_methodSequence;
   ATH_MSG_DEBUG( "F-[" << m_methodSequence << "] extrapolateStepwise(...) " ); 

   // initialize the return parameters vector
   Trk::TrackParametersVector* returnParameters = 0;
   // create a new internal helper vector
   m_parametersOnDetElements = new std::vector<const Trk::TrackParameters*>;
   // run the extrapolation
   const Trk::TrackParameters* parameterOnSf = extrapolate(prop, parm, sf, dir, bcheck, particle);
   // assign the return parameter and set m_parametersOnDetElements = 0;
   returnParameters = m_parametersOnDetElements; m_parametersOnDetElements = 0;
   // add the parameters to the return parameters
   if (parameterOnSf)
       returnParameters->push_back(parameterOnSf);
   else {
    // memory cleanup and return 0    
        std::vector<const Trk::TrackParameters*>::const_iterator tpIter = returnParameters->begin();
        for (; tpIter != returnParameters->end(); ++tpIter) delete (*tpIter);
        delete returnParameters; returnParameters = 0;
   }   
   return returnParameters;
}

const Trk::TrackParameters* Trk::Extrapolator::extrapolate(const IPropagator& prop,
                                                           const Trk::Track& trk,
                                                           const Trk::Surface& sf,
                                                           Trk::PropDirection dir,
                                                           Trk::BoundaryCheck bcheck,
                                                           Trk::ParticleHypothesis particle,
                                                           MaterialUpdateMode matupmode) const
{
 
  // statistics && sequence output ----------------------------------------
  ATH_MSG_DEBUG( "T-[" << ++m_methodSequence << "] extrapolate(const Track&)" ); 

  // intialize the starting propagator
  const IPropagator* searchProp = 0;
  // get the propagator depending on the volume
  if (m_searchLevel<2){
      unsigned int iprop = (m_searchLevel > m_configurationLevel) ? m_searchLevel : m_configurationLevel;
      searchProp = &(*m_propagators[iprop]); 
  }
  // call the navigator
  const Trk::TrackParameters* closestTrackParameters = m_navigator->closestParameters(trk, sf, searchProp);
  if (closestTrackParameters) return(extrapolate(prop, *closestTrackParameters, sf, dir, bcheck, particle, matupmode));
  return 0;               
}  

const Trk::TrackParameters* Trk::Extrapolator::extrapolate(const IPropagator& prop,
                 const TrackParameters& parm,
                 const std::vector< MaterialEffectsOnTrack >& sfMeff,
                 const TrackingVolume& tvol,
                 PropDirection dir,
                 ParticleHypothesis particle,
                 MaterialUpdateMode matupmode) const
{


  // statistics && sequence output ----------------------------------------
  if (m_methodSequence) ++ m_methodSequence;
  ATH_MSG_DEBUG( "D-[" << m_methodSequence << "] extrapolate with given MaterialEffectsOnTrack in Volume '" << tvol.volumeName() << "'." ); 

  const Trk::TrackParameters* fallbackParameters  = 0;
  const Trk::TrackParameters* nextParameters      = &parm;
  
  // loop over the provided material effects on track   
  std::vector< MaterialEffectsOnTrack >::const_iterator sfMeffI = sfMeff.begin();
  std::vector< MaterialEffectsOnTrack >::const_iterator sfMeffE = sfMeff.end();

  for( ; sfMeffI != sfMeffE; ++sfMeffI) {
    // first propagate to the given surface
    //nextParameters = prop.propagate(*nextParameters, sfMeffI->associatedSurface(),dir,true,tvol, particle);
    nextParameters = prop.propagate(*nextParameters, sfMeffI->associatedSurface(),dir,true,m_fieldProperties, particle,false,&tvol);
    // user might have not calculated well which surfaces are intersected ... break if break
    if (!nextParameters) return fallbackParameters;
    throwIntoGarbageBin(nextParameters);
    // the fallback parameters
    fallbackParameters = nextParameters;
    //then update 
    const IMaterialEffectsUpdator* currentUpdator = subMaterialEffectsUpdator(tvol); 
    const Trk::TrackParameters* upnext = currentUpdator ? currentUpdator->update(nextParameters,*sfMeffI,particle,matupmode) : 0; 
    if (!upnext){        
      // update killed the track or config problem. Return
        ATH_MSG_VERBOSE("  [+] Update killed track.");
      break;
    } else if ( upnext != nextParameters ) throwIntoGarbageBin(upnext);
    nextParameters = upnext;
  }  
  return fallbackParameters;
}

const std::vector<const Trk::TrackParameters*>* Trk::Extrapolator::extrapolateBlindly(
                                                                  const IPropagator& prop,
                                                                  const Trk::TrackParameters& parm,
                                                                  Trk::PropDirection dir,
                                                                  Trk::BoundaryCheck bcheck,
                                                                  Trk::ParticleHypothesis particle,
                                                                  const Trk::Volume* boundaryVol) const
{
  
   // statistics && sequence output ----------------------------------------
   ++m_extrapolateBlindlyCalls; 
   ++m_methodSequence;
   ATH_MSG_DEBUG( "F-[" << m_methodSequence << "] extrapolateBlindly() " );
   // assign the boundaryVolume
   m_boundaryVolume = boundaryVol;      
   // initialize the return parameters vector
   Trk::TrackParametersVector* returnParameters = 0;
   // create a new internal helper vector
   m_parametersOnDetElements = new std::vector<const Trk::TrackParameters*>;
   // run the extrapolation    
   const Trk::TrackParameters* parameterOnSf = extrapolate(prop, parm, *s_referenceSurface, dir, bcheck, particle);
   // delete them if necessary
   if (parameterOnSf && parameterOnSf != (&parm)) delete parameterOnSf;
   // assign the return parameter and set m_parametersOnDetElements = 0;
   returnParameters = m_parametersOnDetElements; m_parametersOnDetElements = 0; 
   // reset the boundary Volume
   m_boundaryVolume = 0; 
   // return what you have
   return returnParameters;
}

std::pair<const Trk::TrackParameters*,const Trk::Layer*> Trk::Extrapolator::extrapolateToNextActiveLayer(
                                                        const IPropagator& prop,
                                                        const Trk::TrackParameters& parm,
                                                        PropDirection dir,
                                                        BoundaryCheck bcheck,
                                                        ParticleHypothesis particle,
                                                        MaterialUpdateMode matupmode) const
{
   // statistics && sequence output ----------------------------------------   
   ++m_methodSequence;
   ATH_MSG_DEBUG( "M-[" << m_methodSequence << "] extrapolateToNextActiveLayer(...) " );
   // initialize the return parameters vector
   const Trk::TrackParameters* currPar = &parm;
   const Trk::TrackingVolume*  staticVol = 0;
   const Trk::Surface*         destSurface = 0;
   const Trk::Layer*           assocLayer = 0;

   // -----------------------------------------------------------------------
   // 
   while ( currPar ) {
     //staticVol =  m_navigator->trackingGeometry()->lowestStaticTrackingVolume(currPar->position());    
     assocLayer = 0;
     const Trk::TrackParameters* nextPar =
                    extrapolateToNextMaterialLayer(prop,
                                                  *currPar,
                                                  destSurface,
                                                  staticVol,
                                                  dir,
                                                  bcheck,
                                                  particle,
                                                  matupmode);   
     if (nextPar) { 
         if (m_lastMaterialLayer && m_lastMaterialLayer->surfaceRepresentation().isOnSurface(nextPar->position(),bcheck,m_tolerance,m_tolerance) )
              assocLayer = m_lastMaterialLayer;
   if (!assocLayer) 
     ATH_MSG_ERROR( "  [!] No associated layer found  -   at " << positionOutput(nextPar->position()) );
     } else {
       // static volume boundary ?
       if (m_parametersAtBoundary.nextParameters && m_parametersAtBoundary.nextVolume) {
        if (m_parametersAtBoundary.nextVolume->geometrySignature()==Trk::MS ||
      ( m_parametersAtBoundary.nextVolume->geometrySignature()==Trk::Calo && m_useDenseVolumeDescription )) {
          staticVol = m_parametersAtBoundary.nextVolume;
          nextPar = m_parametersAtBoundary.nextParameters;
          ATH_MSG_DEBUG( "  [+] Static volume boundary: continue loop over active layers in '" 
              << staticVol->volumeName() << "'.");
        } else {   // MSentrance
          nextPar = m_parametersAtBoundary.nextParameters->clone();
          m_parametersAtBoundary.resetBoundaryInformation();
          return std::pair<const Trk::TrackParameters*,const Trk::Layer*>(nextPar,0);          
        }
       } else if (m_parametersAtBoundary.nextParameters ) { // outer boundary
        nextPar = m_parametersAtBoundary.nextParameters->clone();
        m_parametersAtBoundary.resetBoundaryInformation();
        return std::pair<const Trk::TrackParameters*,const Trk::Layer*>(nextPar,0);          
       }
     }
     currPar = nextPar;
     if (currPar && assocLayer && assocLayer->layerType()!=0 ) break;
   }
   // reset the boundary information
   m_parametersAtBoundary.resetBoundaryInformation();
   return std::pair<const Trk::TrackParameters*,const Trk::Layer*>(currPar,assocLayer); 

}

std::pair<const Trk::TrackParameters*,const Trk::Layer*> Trk::Extrapolator::extrapolateToNextActiveLayerM(
                       const IPropagator& prop,
                       const Trk::TrackParameters& parm,
                       PropDirection dir,
                       BoundaryCheck bcheck,
                       std::vector<const Trk::TrackStateOnSurface*>& material,
                       ParticleHypothesis particle,
                       MaterialUpdateMode matupmode) const
{
    
   ATH_MSG_DEBUG( "M-[" << ++m_methodSequence << "] extrapolateToNextActiveLayerM(...) " );
   // initialize the return parameters vector
   const Trk::TrackParameters* currPar = &parm;
   const Trk::TrackingVolume*  staticVol = 0;
   const Trk::Surface*         destSurface = 0;
   const Trk::Layer*           assocLayer = 0;
   // initialize material collection
   m_matstates = &material;

   while ( currPar ) {
     //staticVol =  m_navigator->trackingGeometry()->lowestStaticTrackingVolume(currPar->position());    
     assocLayer = 0;
     const Trk::TrackParameters* nextPar =
       extrapolateToNextMaterialLayer(prop,
                                      *currPar,
                                      destSurface,
                                      staticVol,
                                      dir,
                                      bcheck,
                                      particle,
                                      matupmode);   
     if (nextPar) { 
         if (m_lastMaterialLayer && m_lastMaterialLayer->surfaceRepresentation().isOnSurface(nextPar->position(),bcheck,m_tolerance,m_tolerance) )
          assocLayer = m_lastMaterialLayer;
        if (!assocLayer)  
            ATH_MSG_ERROR( "  [!] No associated layer found  -   at " << positionOutput(nextPar->position()) );
     } else {
       // static volume boundary ?
       if (m_parametersAtBoundary.nextParameters && m_parametersAtBoundary.nextVolume) {
        if (m_parametersAtBoundary.nextVolume->geometrySignature()==Trk::MS ||
            (m_parametersAtBoundary.nextVolume->geometrySignature()==Trk::Calo && m_useDenseVolumeDescription)) {
          staticVol = m_parametersAtBoundary.nextVolume;
          nextPar = m_parametersAtBoundary.nextParameters;
          ATH_MSG_DEBUG( "  [+] Static volume boundary: continue loop over active layers in '" 
              << staticVol->volumeName() << "'.");
        } else {   // MSentrance
          nextPar = m_parametersAtBoundary.nextParameters->clone();
          m_parametersAtBoundary.resetBoundaryInformation();
          return std::pair<const Trk::TrackParameters*,const Trk::Layer*>(nextPar,0);          
        }
       } else if (m_parametersAtBoundary.nextParameters ) { // outer boundary
         nextPar = m_parametersAtBoundary.nextParameters->clone();
        m_parametersAtBoundary.resetBoundaryInformation();
        return std::pair<const Trk::TrackParameters*,const Trk::Layer*>(nextPar,0);          
       }

     }
     currPar = nextPar;
     if (currPar && assocLayer && assocLayer->layerType()!=0 ) break;
   }
   // reset the boundary information
   m_parametersAtBoundary.resetBoundaryInformation();
   m_matstates = 0;   
   m_methodSequence = 0;
   return std::pair<const Trk::TrackParameters*,const Trk::Layer*>(currPar,assocLayer); 
}

const Trk::TrackParameters* Trk::Extrapolator::extrapolateToNextMaterialLayer(const IPropagator& prop,
                        const Trk::TrackParameters& parm,
                        const Trk::Surface* destSurf, 
                        const Trk::TrackingVolume* vol,
                        PropDirection dir,
                        BoundaryCheck bcheck,
                        ParticleHypothesis particle,
                        MaterialUpdateMode matupmode) const
{
  ATH_MSG_DEBUG( "M-[" << ++m_methodSequence << "] extrapolateToNextMaterialLayer(...) " );
 
  // this is the core of the material loop 
  // extrapolation without target surface returns:
  //    A)    trPar at next material layer
  //    B)    boundary parameters (static volume boundary)
  // if target surface:
  //    C)    trPar at target surface
  //
  
  // initialize the return parameters vector
  const Trk::TrackParameters* returnParameters = 0;
  const Trk::TrackParameters* currPar = &parm;
  const Trk::TrackingVolume*  staticVol = 0;
  const Trk::TrackingVolume*  currVol = 0;
  const Trk::TrackingVolume*  nextVol = 0;
  std::vector<unsigned int> solutions;
  const Trk::TrackingVolume* assocVol = 0;  
  //double tol = 0.001;
  double path = 0.;
  bool resolveActive = destSurf ? false : true;
  if (!resolveActive && m_resolveActive ) resolveActive = m_resolveActive;
  if ( m_lastMaterialLayer && !m_lastMaterialLayer->isOnLayer(parm.position()) ) m_lastMaterialLayer = 0;  
  if (!m_highestVolume ) m_highestVolume = m_navigator->highestVolume();
  emptyGarbageBin(&parm);
  // resolve current position
  Amg::Vector3D gp = parm.position();
  if ( vol && vol->inside(gp,m_tolerance) ) {
    staticVol = vol; 
  } else {
    staticVol =  m_navigator->trackingGeometry()->lowestStaticTrackingVolume(gp);    
    const Trk::TrackingVolume* nextStatVol = 0;
    if ( m_navigator->atVolumeBoundary(currPar,staticVol,dir,nextStatVol,m_tolerance) && nextStatVol != staticVol ) 
      staticVol = nextStatVol;
  }

  // navigation surfaces
  if( m_navigSurfs.capacity() > m_maxNavigSurf ) m_navigSurfs.reserve(m_maxNavigSurf); 
  m_navigSurfs.clear();
  if (destSurf) m_navigSurfs.push_back(std::pair<const Trk::Surface*,Trk::BoundaryCheck>(destSurf,false));
  // alignable frame volume ?
  if ( staticVol && staticVol->geometrySignature()==Trk::Calo ) {
    const Trk::AlignableTrackingVolume* alignTV = dynamic_cast<const Trk::AlignableTrackingVolume*> (staticVol);
    if (alignTV) {
      m_identifiedParameters.reset();
      return extrapolateInAlignableTV(prop,*currPar,destSurf,alignTV,dir,particle);
    }
  }
  
  // update if new static volume
  if ( staticVol && ( staticVol!=m_currentStatic || resolveActive!=m_resolveActive ) ) {    // retrieve boundaries
    m_currentStatic = staticVol;
    m_staticBoundaries.clear();
    const std::vector< SharedObject<const BoundarySurface<TrackingVolume> > > bounds = staticVol->boundarySurfaces();
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

    const std::vector<const Trk::DetachedTrackingVolume*>* detVols = staticVol->confinedDetachedVolumes();
    if (detVols ) {
      std::vector<const Trk::DetachedTrackingVolume*>::const_iterator iTer = detVols->begin();
      for (; iTer != detVols->end(); iTer++) { 
        // active station ?
        const Trk::Layer* layR = (*iTer)->layerRepresentation();
        bool active = ( layR && layR->layerType() ) ? true : false;
        const std::vector< SharedObject<const BoundarySurface<TrackingVolume> > >  detBounds=(*iTer)->trackingVolume()->boundarySurfaces();
        if (active) {
        	if ( resolveActive ) {
            m_detachedVols.push_back(std::pair<const Trk::DetachedTrackingVolume*,unsigned int> (*iTer,detBounds.size()) );
            for (unsigned int ibb=0; ibb< detBounds.size(); ibb++ ){
              const Trk::Surface& surf = (detBounds[ibb].getPtr())->surfaceRepresentation();
              m_detachedBoundaries.push_back(std::pair<const Trk::Surface*,Trk::BoundaryCheck>(&surf,true));
            }
          } else {
            if (!m_resolveMultilayers || !(*iTer)->multilayerRepresentation() ) {
              m_layers.push_back(std::pair<const Trk::Surface*,Trk::BoundaryCheck>(&(layR->surfaceRepresentation()),true));
              m_navigLays.push_back(std::pair<const Trk::TrackingVolume*,const Trk::Layer*> ((*iTer)->trackingVolume(),layR));
            } else {
              const std::vector<const Trk::Layer*>* multi = (*iTer)->multilayerRepresentation();
              for (unsigned int i=0;i<multi->size();i++) {
                m_layers.push_back(std::pair<const Trk::Surface*,Trk::BoundaryCheck>(&((*multi)[i]->surfaceRepresentation()),true));
    						m_navigLays.push_back(std::pair<const Trk::TrackingVolume*,const Trk::Layer*> ((*iTer)->trackingVolume(),(*multi)[i]));
        			}
      			}
          }
  			} else if (staticVol->geometrySignature()!=Trk::MS ||
       			!m_useMuonMatApprox || (*iTer)->name().substr((*iTer)->name().size()-4,4)=="PERM" ) {  // retrieve inert detached objects only if needed
          	if ((*iTer)->trackingVolume()->zOverAtimesRho()!=0. && 
        				(!(*iTer)->trackingVolume()->confinedDenseVolumes() || !(*iTer)->trackingVolume()->confinedDenseVolumes()->size() )
        				&& ( !(*iTer)->trackingVolume()->confinedArbitraryLayers() ||
                !(*iTer)->trackingVolume()->confinedArbitraryLayers()->size() ) ) {
            	m_denseVols.push_back(std::pair<const Trk::TrackingVolume*,unsigned int> ((*iTer)->trackingVolume(),detBounds.size() ) );
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
		if (staticVol){
			m_dense = (staticVol->geometrySignature()==Trk::MS && m_useMuonMatApprox ) || (staticVol->geometrySignature()!=Trk::MS && m_useDenseVolumeDescription );
		}
		while (currPar && staticVol && !staticVol->confinedDetachedVolumes() ) {
			// propagate to closest surface
			solutions.resize(0);
			const Trk::TrackingVolume* propagVol = m_dense ? staticVol : m_highestVolume; 
			ATH_MSG_DEBUG( "  [+] Starting propagation (static)  at " << positionOutput(currPar->position()) 
					<< " in '" << propagVol->volumeName() << "'") ;
			// current static may carry non-trivial material properties, their use is optional; 
			// use highest volume as B field source 
			//const Trk::TrackParameters* nextPar = prop.propagate(*currPar,m_navigSurfs,dir,*propagVol,particle,solutions,path);
			const Trk::TrackParameters* nextPar = prop.propagate(*currPar,m_navigSurfs,dir, m_fieldProperties,particle,solutions,path,false,false,propagVol);
			ATH_MSG_VERBOSE( "  [+] Propagation done" ); 
			if (nextPar) 
					ATH_MSG_DEBUG( "  [+] Position after propagation -   at " << positionOutput(nextPar->position())); 
			if (!nextPar) { m_parametersAtBoundary.resetBoundaryInformation(); return returnParameters; }         
			if (nextPar) {    
				// collect material
				if (propagVol->zOverAtimesRho() != 0. &&  !m_matstates && m_extrapolationCache) {     
					if(checkCache(" extrapolateToNextMaterialLayer" )) {
						if(m_dumpCache) dumpCache(" extrapolateToNextMaterialLayer");
						double dInX0 = fabs(path)/propagVol->x0();    
						ATH_MSG_DEBUG(" add x0 " << dInX0); 
						m_extrapolationCache->updateX0(dInX0);
						Trk::MaterialProperties materialProperties(*propagVol, fabs(path));
						double currentqoverp=nextPar->parameters()[Trk::qOverP];
						Trk::EnergyLoss* eloss = m_elossupdators[0]->energyLoss(materialProperties,fabs(1./currentqoverp),1.,dir,particle);
						ATH_MSG_DEBUG( "  [M] Energy loss: STEP,EnergyLossUpdator:" 
								 << nextPar->momentum().mag()-currPar->momentum().mag() << ","<< eloss->deltaE() );
						m_extrapolationCache->updateEloss(eloss->meanIoni(),eloss->sigmaIoni(),eloss->meanRad(),eloss->sigmaRad());
						if(m_dumpCache) dumpCache(" After");
						delete eloss;
					}
				}
				if (propagVol->zOverAtimesRho() != 0. &&  m_matstates ) {     
				double dInX0 = fabs(path)/propagVol->x0();    
				Trk::MaterialProperties materialProperties(*propagVol, fabs(path));
				double scatsigma=sqrt(m_msupdators[0]->sigmaSquare(materialProperties,1./fabs(nextPar->parameters()[qOverP]),1.,particle));
				Trk::ScatteringAngles *newsa=new Trk::ScatteringAngles(0,0,scatsigma/sin(nextPar->parameters()[Trk::theta]),scatsigma);
				//energy loss
				double currentqoverp=nextPar->parameters()[Trk::qOverP];
				Trk::EnergyLoss* eloss = m_elossupdators[0]->energyLoss(materialProperties,fabs(1./currentqoverp),1.,dir,particle);
				// compare energy loss 
				ATH_MSG_DEBUG( "  [M] Energy loss: STEP,EnergyLossUpdator:" 
							<< nextPar->momentum().mag()-currPar->momentum().mag() << ","<< eloss->deltaE() );
				// use curvilinear TPs to simplify retrieval by fitters
				Trk::CurvilinearParameters* cvlTP = new Trk::CurvilinearParameters(nextPar->position(),nextPar->momentum(),nextPar->charge());
				Trk::MaterialEffectsOnTrack* mefot =  new Trk::MaterialEffectsOnTrack(dInX0,newsa,eloss,cvlTP->associatedSurface());   
				m_matstates->push_back(new TrackStateOnSurface(0,cvlTP,0,mefot));
				if(m_extrapolationCache) {
					if(m_dumpCache) dumpCache(" mat states extrapolateToNextMaterialLayer");
					m_extrapolationCache->updateX0(dInX0);
					m_extrapolationCache->updateEloss(eloss->meanIoni(),eloss->sigmaIoni(),eloss->meanRad(),eloss->sigmaRad());
					if(m_dumpCache) dumpCache(" After");
				}
				ATH_MSG_DEBUG("  [M] Collecting material from static volume '"<<propagVol->volumeName()<< "', t/X0 = " << dInX0);
			}
		}
		throwIntoGarbageBin(nextPar);
		currPar = nextPar;
		unsigned int isurf = destSurf ? 1 : 0 ; 
		if (destSurf && solutions[0]==0) return nextPar->clone();
		if (destSurf && solutions.size()>1 && solutions[1]==0) return nextPar->clone();
		if (solutions[0] <= isurf + m_staticBoundaries.size() ) {  // static volume boundary
			// use global coordinates to retrieve attached volume (just for static!)
			const Trk::TrackingVolume* nextVol = 
					m_currentStatic->boundarySurfaces()[solutions[0]-isurf].getPtr()->attachedVolume(nextPar->position(),nextPar->momentum(),dir) ;
			m_parametersAtBoundary.boundaryInformation(nextVol,currPar,currPar);
			if ( !nextVol ) ATH_MSG_DEBUG( "  [!] World boundary at position R,z: "  << nextPar->position().perp()<<"," << nextPar->position().z());
			else {
				ATH_MSG_DEBUG( "M-S Crossing to static volume '"<< nextVol->volumeName() << "'.'");  
			}
		}
		return returnParameters;
	}  
 
  if (!staticVol || (!staticVol->confinedDetachedVolumes()) || !currPar ) {
    return returnParameters;
  }

  // reset remaining counters
  m_currentDense = m_dense ?  m_currentStatic : m_highestVolume;
  m_navigBoundaries.clear(); 
  if (m_denseVols.size()>m_denseResolved.first) m_denseVols.resize(m_denseResolved.first);
  while (m_denseBoundaries.size()>m_denseResolved.second) m_denseBoundaries.pop_back();
  if (m_layers.size()>m_layerResolved)  m_navigLays.resize(m_layerResolved);
  while (m_layers.size()>m_layerResolved) m_layers.pop_back();

  // current detached volumes
  // collect : subvolume boundaries, ordered/unordered layers, confined dense volumes
  ////////////////////////////////////////////////////// 
  //const Trk::DetachedTrackingVolume* currentActive = 0;
  if( m_navigVolsInt.capacity() > m_maxNavigVol ) m_navigVolsInt.reserve(m_maxNavigVol); 
  m_navigVolsInt.clear();

  gp = currPar->position();    
  std::vector<const Trk::DetachedTrackingVolume*>* detVols =  m_navigator->trackingGeometry()->lowestDetachedTrackingVolumes(gp);    
  std::vector<const Trk::DetachedTrackingVolume*>::iterator dIter = detVols->begin();
  for (; dIter != detVols->end(); dIter++) {
    const Trk::Layer* layR = (*dIter)->layerRepresentation();
    bool active = ( layR && layR->layerType() ) ? true : false;
    if (active && !resolveActive) continue; 
    if (!active && staticVol->geometrySignature()==Trk::MS && m_useMuonMatApprox && (*dIter)->name().substr((*dIter)->name().size()-4,4)!="PERM") continue;
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
      m_navigVolsInt.push_back(std::pair<const Trk::TrackingVolume*,unsigned int> (dVol,bounds.size()) );
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
  			m_navigVolsInt.push_back(std::pair<const Trk::TrackingVolume*,unsigned int> (detVol,bounds.size()) );
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
					const Trk::Layer* lay = detVol->associatedLayer(gp);
					//if (lay && ( (*dIter)->layerRepresentation()
					//         &&(*dIter)->layerRepresentation()->layerType()>0 ) ) currentActive=(*dIter);  
					if (lay) {
						m_layers.push_back(std::pair<const Trk::Surface*,Trk::BoundaryCheck>(&(lay->surfaceRepresentation()),true));
						m_navigLays.push_back(std::pair<const Trk::TrackingVolume*,const Trk::Layer*> (detVol,lay) );
					}
					const Trk::Layer* nextLayer = detVol->nextLayer(currPar->position(),dir*currPar->momentum().unit(),true);      
					if (nextLayer && nextLayer != lay ) {
						m_layers.push_back(std::pair<const Trk::Surface*,Trk::BoundaryCheck>(&(nextLayer->surfaceRepresentation()),true));
						m_navigLays.push_back(std::pair<const Trk::TrackingVolume*,const Trk::Layer*> (detVol,nextLayer) );
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
   
  // ready to propagate      
  // till: A/ static volume boundary(bcheck=true) , B/ material layer(bcheck=true), C/ destination surface(bcheck=false)
  // update of m_navigSurfs required if I/ entry into new navig volume, II/ exit from currentActive without overlaps
   
   nextVol = 0;
   while (currPar) {      
     double path=0.;
     std::vector<unsigned int> solutions;
     // verify that material input makes sense
     Amg::Vector3D tp = currPar->position()+2*m_tolerance*dir*currPar->momentum().normalized();
     if (!(m_currentDense->inside(tp,0.)) ) { 
       m_currentDense =  m_highestVolume;
       if (m_dense && !m_denseVols.size()) m_currentDense = m_currentStatic;
       else {
   			 for (unsigned int i=0;i<m_denseVols.size(); i++) {
     			 const Trk::TrackingVolume* dVol = m_denseVols[i].first;
     			 if ( dVol->inside(tp,0.)  && dVol->zOverAtimesRho()!=0. ) m_currentDense = dVol;
   			 }
       }
     } 
     // propagate now
     ATH_MSG_DEBUG( "  [+] Starting propagation at position  " << positionOutput(currPar->position())  
         << " (current momentum: " << currPar->momentum().mag() << ")" ); 
     ATH_MSG_DEBUG( "  [+] " << m_navigSurfs.size() << " target surfaces in '" << m_currentDense->volumeName() <<"'.");   
     //const Trk::TrackParameters* nextPar = prop.propagate(*currPar,m_navigSurfs,dir,*m_currentDense,particle,solutions,path);
     const Trk::TrackParameters* nextPar = prop.propagate(*currPar,m_navigSurfs,dir, m_fieldProperties,particle,solutions,path,false,false,m_currentDense);
     ATH_MSG_VERBOSE( "  [+] Propagation done. " ); 
     if (nextPar)  
        ATH_MSG_DEBUG( "  [+] Position after propagation -   at " << positionOutput(nextPar->position())); 
     // check missing volume boundary
     if (nextPar && !(m_currentDense->inside(nextPar->position(),m_tolerance)
          || m_navigator->atVolumeBoundary(nextPar,m_currentDense,dir,assocVol,m_tolerance) ) ) {
       ATH_MSG_DEBUG( "  [!] ERROR: missing volume boundary for volume"<< m_currentDense->volumeName() );
       if ( m_currentDense->zOverAtimesRho() != 0.) {     
         ATH_MSG_DEBUG( "  [!] ERROR: trying to recover: repeat the propagation step in"<< m_highestVolume->volumeName() );
         m_currentDense = m_highestVolume;
         delete nextPar;
         continue;
       }
     }
     if (nextPar)  ATH_MSG_DEBUG( "  [+] Number of intersection solutions: " << solutions.size() );
     if (nextPar) throwIntoGarbageBin(nextPar);
     if (nextPar) {    
       if (m_currentDense->zOverAtimesRho() != 0. &&  !m_matstates && m_extrapolationCache) {     
         if(checkCache( " extrapolateToNextMaterialLayer dense")) {
           if(m_dumpCache) dumpCache(" extrapolateToNextMaterialLayer dense ");
           double dInX0 = fabs(path)/m_currentDense->x0();  
           m_extrapolationCache->updateX0(dInX0);
     			 Trk::MaterialProperties materialProperties(*m_currentDense, fabs(path) );
           double currentqoverp=nextPar->parameters()[Trk::qOverP];
           Trk::EnergyLoss* eloss = m_elossupdators[0]->energyLoss(materialProperties,fabs(1./currentqoverp),1.,dir,particle);
            m_extrapolationCache->updateEloss(eloss->meanIoni(),eloss->sigmaIoni(),eloss->meanRad(),eloss->sigmaRad());
           if(m_dumpCache) dumpCache(" After");
           delete eloss;
         }
       }
       // collect material
       if ( m_currentDense->zOverAtimesRho() != 0. && m_matstates) {     
         double dInX0 = fabs(path)/m_currentDense->x0();  
  			 if( path*dir < 0. ) ATH_MSG_WARNING(" got negative path!! " << path );
  			 Trk::MaterialProperties materialProperties(*m_currentDense, fabs(path) );
         double scatsigma=sqrt(m_msupdators[0]->sigmaSquare(materialProperties,1./fabs(nextPar->parameters()[qOverP]),1.,particle));
         Trk::ScatteringAngles *newsa=new Trk::ScatteringAngles(0,0,scatsigma/sin(nextPar->parameters()[Trk::theta]),scatsigma);   
        	//energy loss
        	double currentqoverp=nextPar->parameters()[Trk::qOverP];
        	Trk::EnergyLoss* eloss = m_elossupdators[0]->energyLoss(materialProperties,fabs(1./currentqoverp),1.,
                                                                       dir,particle);
        	// compare energy loss 
  				ATH_MSG_DEBUG( "  [M] Energy loss: STEP,EnergyLossUpdator:" 
            	<< nextPar->momentum().mag()-currPar->momentum().mag() << ","<< eloss->deltaE() );
    
       
        // use curvilinear TPs to simplify retrieval by fitters
  			Trk::CurvilinearParameters* cvlTP = new Trk::CurvilinearParameters(nextPar->position(),nextPar->momentum(),nextPar->charge());
        Trk::MaterialEffectsOnTrack* mefot = new Trk::MaterialEffectsOnTrack(dInX0,newsa,eloss,cvlTP->associatedSurface());           
        m_matstates->push_back(new TrackStateOnSurface(0,cvlTP,0,mefot));
        if(m_extrapolationCache) {
          if(m_dumpCache) dumpCache(" extrapolateToNextMaterialLayer dense");
          m_extrapolationCache->updateX0(dInX0);
          m_extrapolationCache->updateEloss(eloss->meanIoni(),eloss->sigmaIoni(),eloss->meanRad(),eloss->sigmaRad());
          if(m_dumpCache) dumpCache(" After");
  			}
        ATH_MSG_DEBUG("  [M] Collecting material from dense volume '" 
            << m_currentDense->volumeName()<< "', t/X0 = " << dInX0);
       }
       // destination surface
       if (destSurf && solutions[0]==0 ) return nextPar->clone();
       if (destSurf && solutions.size()>1 && solutions[1]==0 ) return nextPar->clone();
       // destination surface missed ? 
       if (destSurf) {
         double dist = 0.;
         Trk::DistanceSolution distSol = destSurf->straightLineDistanceEstimate(nextPar->position(),
                                                                                          nextPar->momentum().normalized());
         if (distSol.numberOfSolutions()>0 ) {
           dist = distSol.first();
           if ( distSol.numberOfSolutions()>1 && fabs(dist) < m_tolerance ) dist = distSol.second();
           if ( distSol.numberOfSolutions()>1 && dist*dir < 0. && distSol.second()*dir > 0. ) dist = distSol.second();    
         } else {
           dist = distSol.toPointOfClosestApproach();
         } 
        if (dist*dir < 0.) {
          ATH_MSG_DEBUG( "  [+] Destination surface missed ? "<< dist << "," << dir );
          m_parametersAtBoundary.resetBoundaryInformation();
          return returnParameters;
        }
        ATH_MSG_DEBUG( "  [+] New 3D-distance to destinatiion    - d3 = " << dist*dir ); 
       }
       
       int iDest = destSurf ? 1 : 0;
       unsigned int iSol  = 0;
       while ( iSol < solutions.size() ) {         
         if ( solutions[iSol] < iDest + m_staticBoundaries.size() ) {
     
					 // material attached ?
					 const Trk::Layer*  mb =  m_navigSurfs[solutions[iSol]].first->materialLayer();  
					 if (mb) {
						 if (mb->layerMaterialProperties() && mb->layerMaterialProperties()->fullMaterial(nextPar->position()) ) {
				 
							 const IMaterialEffectsUpdator* currentUpdator = subMaterialEffectsUpdator(*m_currentStatic);
							 if ( currentUpdator)  {
					 			 const Trk::TrackParameters* upNext = currentUpdator->update(nextPar, *mb, dir, particle, matupmode);
					       if (upNext && upNext!= nextPar ) throwIntoGarbageBin(upNext);
					       nextPar = upNext;
							 }
					 if (!nextPar) {
					   ATH_MSG_VERBOSE( "  [+] Update may have killed track - return." );
					   m_parametersAtBoundary.resetBoundaryInformation();
					   return returnParameters;
           }
         
         // collect material
         const Trk::MaterialProperties* lmat = mb->fullUpdateMaterialProperties(*nextPar);
         double lx0 = lmat->x0();
         double layThick = mb->thickness();
            
         double thick = 0.; 
         double costr = fabs(nextPar->momentum().normalized().dot(mb->surfaceRepresentation().normal())) ;  
            
         if ( mb->surfaceRepresentation().isOnSurface(mb->surfaceRepresentation().center(),false,0.,0.) )        
     thick = fmin(mb->surfaceRepresentation().bounds().r(),
                             layThick/fabs(nextPar->momentum().normalized().dot(mb->surfaceRepresentation().normal())) );
         else {
     //const Trk::CylinderBounds* cyl = dynamic_cast<const Trk::CylinderBounds*> (&(nextLayer->surfaceRepresentation().bounds()));
     //double hmax = cyl ? cyl->halflengthZ() : nextLayer->surfaceRepresentation().bounds().r();
     thick = fmin(2*mb->thickness(), layThick/(1-costr));                  
         }

         if (!m_matstates && m_extrapolationCache) {     
     if(checkCache(" extrapolateToNextMaterialLayer thin")) {
       double dInX0 = thick/lx0;    
       if(m_dumpCache) dumpCache(" extrapolateToNextMaterialLayer thin ");
       m_extrapolationCache->updateX0(dInX0);
       double currentqoverp=nextPar->parameters()[Trk::qOverP];
       EnergyLoss* eloss = m_elossupdators[0]->energyLoss(*lmat,fabs(1./currentqoverp),1./costr,dir,particle);
       m_extrapolationCache->updateEloss(eloss->meanIoni(),eloss->sigmaIoni(),eloss->meanRad(),eloss->sigmaRad());
       if(m_dumpCache) dumpCache(" After");
       delete eloss;
     }
         }
         
         if (m_matstates) {
     double dInX0 = thick/lx0;    
     double scatsigma=sqrt(m_msupdators[0]->sigmaSquare(*lmat,1./fabs(nextPar->parameters()[qOverP]),1.,particle));
     Trk::ScatteringAngles *newsa=new Trk::ScatteringAngles(0,0,scatsigma/sin(nextPar->parameters()[Trk::theta]),scatsigma);
     //energy loss
     double currentqoverp=nextPar->parameters()[Trk::qOverP];
     EnergyLoss* eloss = m_elossupdators[0]->energyLoss(*lmat,fabs(1./currentqoverp),1./costr,dir,particle);
     
     // use curvilinear TPs to simplify retrieval by fitters
     Trk::CurvilinearParameters* cvlTP = new Trk::CurvilinearParameters(nextPar->position(),nextPar->momentum(),nextPar->charge());
     Trk::MaterialEffectsOnTrack* mefot =  new Trk::MaterialEffectsOnTrack(dInX0,newsa,eloss,cvlTP->associatedSurface());
     if(m_extrapolationCache) {
       if(checkCache(" mat states extrapolateToNextMaterialLayer thin" )) {
         if(m_dumpCache) dumpCache(" extrapolateToNextMaterialLayer thin");
         m_extrapolationCache->updateX0(dInX0);
         m_extrapolationCache->updateEloss(eloss->meanIoni(),eloss->sigmaIoni(),eloss->meanRad(),eloss->sigmaRad());
         if(m_dumpCache) dumpCache(" After");
       }
     }     
     m_matstates->push_back(new TrackStateOnSurface(0,cvlTP,0,mefot));
         }         
       }
     }  // end material update at massive (static volume) boundary     

           // static volume boundary; return to the main loop 
           unsigned int index = solutions[iSol]-iDest;
           // use global coordinates to retrieve attached volume (just for static!)
     nextVol = (m_currentStatic->boundarySurfaces())[index].getPtr()->attachedVolume(nextPar->position(),nextPar->momentum(),dir);
     // double check the next volume
     if ( nextVol && !(nextVol->inside(nextPar->position()+0.01*dir*nextPar->momentum().normalized(),m_tolerance) ) ) {
       ATH_MSG_DEBUG( "  [!] WARNING: wrongly assigned static volume ?"<< m_currentStatic->volumeName()<<"->" << nextVol->volumeName() );
       nextVol = m_navigator->trackingGeometry()->lowestStaticTrackingVolume(nextPar->position()+0.01*nextPar->momentum().normalized());
       if (nextVol) ATH_MSG_DEBUG( "  new search yields: "<< nextVol->volumeName() );          
     }
     // end double check - to be removed after validation of the geometry gluing 
     if (nextVol != m_currentStatic ) {
       m_parametersAtBoundary.boundaryInformation(nextVol,nextPar,nextPar);         
       ATH_MSG_DEBUG("  [+] StaticVol boundary reached of '" <<m_currentStatic->volumeName() << "'.");
       if ( m_navigator->atVolumeBoundary(nextPar,m_currentStatic,dir,assocVol,m_tolerance) && assocVol != m_currentStatic )
         m_currentDense = m_useMuonMatApprox ? nextVol : m_highestVolume;
       // no next volume found --- end of the world
       if ( !nextVol )
         ATH_MSG_DEBUG( "  [+] Word boundary reached        - at " << positionOutput(nextPar->position()) );
       // next volume found and parameters are at boundary
       if ( nextVol && nextPar ){ 
         ATH_MSG_DEBUG( "  [+] Crossing to next volume '" << nextVol->volumeName() << "'");
         ATH_MSG_DEBUG( "  [+] Crossing position is         - at " <<  positionOutput(nextPar->position()) );  
       }     
       return returnParameters;
     }
         } else if ( solutions[iSol] < iDest + m_staticBoundaries.size() + m_layers.size() ) {
           // next layer; don't return passive material layers unless required 
    unsigned int index = solutions[iSol]-iDest-m_staticBoundaries.size();
          const Trk::Layer* nextLayer = m_navigLays[index].second;
          // material update HERE and NOW (pre/post udpdate ? ) 
          // don't repeat if identical to last update && input parameters on the layer
    bool collect = true;
    if ( nextLayer == m_lastMaterialLayer &&  nextLayer->surfaceRepresentation().type()!=Trk::Surface::Cylinder ) {
      ATH_MSG_DEBUG( "  [!] This layer is identical to the one with last material update, return layer without repeating the update" );
      collect = false;
      if (!destSurf && (nextLayer->layerType()>0 || m_returnPassiveLayers) ) return nextPar->clone();
          }
          double layThick = nextLayer->thickness();
          if (collect && layThick>0.) {                         // collect material
      // get the right updator
      const IMaterialEffectsUpdator* currentUpdator = subMaterialEffectsUpdator(*m_currentStatic);
      const Trk::TrackParameters* upNext = currentUpdator ? currentUpdator->update(nextPar, *nextLayer, dir, particle,matupmode) : nextPar;
      if (!upNext) {
        ATH_MSG_VERBOSE( "  [+] Update may have killed track - return." );
        m_parametersAtBoundary.resetBoundaryInformation();
        return returnParameters;
      } else if ( upNext != nextPar ) throwIntoGarbageBin(upNext);

            nextPar = upNext;               
            
            // collect material
            double lx0 = nextLayer->fullUpdateMaterialProperties(*nextPar)->x0();
            
            double thick = 0.;
             double costr = fabs(nextPar->momentum().normalized().dot(nextLayer->surfaceRepresentation().normal())) ;  
            
            if ( nextLayer->surfaceRepresentation().isOnSurface(nextLayer->surfaceRepresentation().center(),false,0.,0.) )        
              thick = fmin(nextLayer->surfaceRepresentation().bounds().r(),
                             layThick/fabs(nextPar->momentum().normalized().dot(nextLayer->surfaceRepresentation().normal())) );
            else {
              //const Trk::CylinderBounds* cyl = dynamic_cast<const Trk::CylinderBounds*> (&(nextLayer->surfaceRepresentation().bounds()));
              //double hmax = cyl ? cyl->halflengthZ() : nextLayer->surfaceRepresentation().bounds().r();
              thick = fmin(2*nextLayer->thickness(), layThick/(1-costr));                  
            }

            if (!m_matstates && m_extrapolationCache) {     
              if(checkCache(" extrapolateToNextMaterialLayer thin")) {
                double dInX0 = thick/lx0;    
                if(m_dumpCache) dumpCache(" extrapolateToNextMaterialLayer thin ");
                m_extrapolationCache->updateX0(dInX0);
                Trk::MaterialProperties materialProperties(*nextLayer->fullUpdateMaterialProperties(*nextPar)); //!< @TODO check        
                double currentqoverp=nextPar->parameters()[Trk::qOverP];
                EnergyLoss* eloss = m_elossupdators[0]->energyLoss(materialProperties,fabs(1./currentqoverp),1./costr,dir,particle);
                m_extrapolationCache->updateEloss(eloss->meanIoni(),eloss->sigmaIoni(),eloss->meanRad(),eloss->sigmaRad());
                if(m_dumpCache) dumpCache(" After");
                delete eloss;
              }
            }

            if (m_matstates) {
               double dInX0 = thick/lx0;    
               Trk::MaterialProperties materialProperties(*nextLayer->fullUpdateMaterialProperties(*nextPar)); //!< @TODO check        
               double scatsigma=sqrt(m_msupdators[0]->sigmaSquare(materialProperties,1./fabs(nextPar->parameters()[qOverP]),1.,particle));
               Trk::ScatteringAngles *newsa=new Trk::ScatteringAngles(0,0,scatsigma/sin(nextPar->parameters()[Trk::theta]),scatsigma);
               //energy loss
               double currentqoverp=nextPar->parameters()[Trk::qOverP];
               EnergyLoss* eloss = m_elossupdators[0]->energyLoss(materialProperties,fabs(1./currentqoverp),1./costr,
                                                                 dir,particle);

         // use curvilinear TPs to simplify retrieval by fitters
         Trk::CurvilinearParameters* cvlTP = new Trk::CurvilinearParameters(nextPar->position(),nextPar->momentum(),nextPar->charge());
         Trk::MaterialEffectsOnTrack* mefot =  new Trk::MaterialEffectsOnTrack(dInX0,newsa,eloss,cvlTP->associatedSurface());
               if(m_extrapolationCache) {
                 if(checkCache(" mat states extrapolateToNextMaterialLayer thin" )) {
                   if(m_dumpCache) dumpCache(" extrapolateToNextMaterialLayer thin");
                   m_extrapolationCache->updateX0(dInX0);
                   m_extrapolationCache->updateEloss(eloss->meanIoni(),eloss->sigmaIoni(),eloss->meanRad(),eloss->sigmaRad());
                   if(m_dumpCache) dumpCache(" After");
           }
               }     
         m_matstates->push_back(new TrackStateOnSurface(0,cvlTP,0,mefot));
            }
            //     
             ATH_MSG_VERBOSE("  [M] Collecting material at material layer t/X0 = " << thick/lx0 );
       if (m_cacheLastMatLayer) m_lastMaterialLayer = nextLayer; 
            if (!destSurf && (nextLayer->layerType()>0 || m_returnPassiveLayers) ) return nextPar->clone();
          }
          if ( resolveActive ) { 
            // if ordered layers, retrieve the next layer and replace the current one in the list
            if (m_navigLays[index].first && m_navigLays[index].first->confinedLayers()) {
              const Trk::Layer* newLayer = m_navigLays[index].first->nextLayer(nextPar->position(),dir*nextPar->momentum().normalized(),true);
              if (newLayer) { 
                 m_navigLays[index].second = newLayer;
                 m_navigSurfs[solutions[iSol]].first = &(newLayer->surfaceRepresentation());
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
             // boundary orientation not reliable 
       Amg::Vector3D tp = nextPar->position()+2*m_tolerance*dir*nextPar->momentum().normalized();
       if (currVol->inside(tp,m_tolerance)) {
         m_currentDense = currVol;
       } else if (!nextVol || !nextVol->inside(tp,m_tolerance) ) {   // search for dense volumes
         m_currentDense =  m_highestVolume;
         if (m_dense && !m_denseVols.size()) m_currentDense = m_currentStatic;
         else {
     for (unsigned int i=0;i<m_denseVols.size(); i++) {
       const Trk::TrackingVolume* dVol = m_denseVols[i].first;
       if ( dVol->inside(tp,0.)  && dVol->zOverAtimesRho()!=0. ){
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
     std::vector< std::pair<const Trk::TrackingVolume*,unsigned int> >::iterator nIter = m_navigVolsInt.begin();
           while ( index >= (*nIter).second && nIter!= m_navigVolsInt.end() ) {
             index -= (*nIter).second ; 
             nIter++;
           }
           if ( nIter != m_navigVolsInt.end() ) {
             currVol = (*nIter).first;
             nextVol = ( (*nIter).first->boundarySurfaces())[index].getPtr()->attachedVolume(*nextPar,dir);
             // boundary orientation not reliable 
       Amg::Vector3D tp = nextPar->position()+2*m_tolerance*dir*nextPar->momentum().normalized();
       if (nextVol && nextVol->inside(tp,0.)) {
         ATH_MSG_DEBUG("  [+] Navigation volume boundary, entering volume '" << nextVol->volumeName() << "'.");
       } else if ( currVol->inside(tp,0.) ) {
         nextVol = currVol;    
         ATH_MSG_DEBUG("  [+] Navigation volume boundary, entering volume '" << nextVol->volumeName() << "'.");
       } else {
         nextVol = 0;
         ATH_MSG_DEBUG("  [+] Navigation volume boundary, leaving volume '" << currVol->volumeName() << "'.");
       } 
             currPar = nextPar;
             // return only if detached volume boundaries not collected
             //if ( nextVol || !detachedBoundariesIncluded )
             if ( nextVol )
         return extrapolateToNextMaterialLayer(prop,*currPar,destSurf,m_currentStatic,dir,bcheck,particle,matupmode);                 
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
             // boundary orientation not reliable 
             nextVol = ( (*dIter).first->trackingVolume()->boundarySurfaces())[index].getPtr()->attachedVolume(*nextPar,dir);
       Amg::Vector3D tp = nextPar->position()+2*m_tolerance*dir*nextPar->momentum().normalized();
       if (nextVol && nextVol->inside(tp,0.)) {
         ATH_MSG_DEBUG("  [+] Detached volume boundary, entering volume '" << nextVol->volumeName() << "'.");
       } else if ( currVol->inside(tp,0.) ) {
         nextVol = currVol;    
         ATH_MSG_DEBUG("  [+] Detached volume boundary, entering volume '" << nextVol->volumeName() << "'.");
       } else {
         nextVol = 0;
         ATH_MSG_DEBUG("  [+] Detached volume boundary, leaving volume '" << currVol->volumeName() << "'.");
       } 
             currPar = nextPar;
             //if ( nextVol || !detachedBoundariesIncluded)
             if ( nextVol )
         return extrapolateToNextMaterialLayer(prop,*currPar,destSurf,m_currentStatic,dir,bcheck,particle,matupmode);   
           } 
  }   
  iSol++;
       }
     } else {
       ATH_MSG_DEBUG( "  [!] Propagation failed, return 0" );
       m_parametersAtBoundary.boundaryInformation(m_currentStatic,nextPar,nextPar);     
       return returnParameters;
     }
     currPar = nextPar;
   }

   return returnParameters;
}        


const Trk::TrackParameters* Trk::Extrapolator::extrapolateInAlignableTV(const IPropagator& prop,
                        const Trk::TrackParameters& parm,
                        const Trk::Surface* destSurf, 
                        const Trk::AlignableTrackingVolume* vol,
                        PropDirection dir,
                        ParticleHypothesis particle) const
{
  ATH_MSG_DEBUG( "M-[" << ++m_methodSequence << "] extrapolateInAlignableTV(...) " );
 
  // material loop in sensitive Calo volumes 
  // extrapolation without target surface returns:
  //    A)    boundary parameters (static volume boundary)
  // if target surface:
  //    B)    trPar at target surface
  // material collection done by the propagator ( binned material used ) 
  
  // initialize the return parameters vector
  const Trk::TrackParameters* returnParameters = 0;
  const Trk::TrackParameters* currPar = &parm;
  const Trk::AlignableTrackingVolume*  staticVol = 0;
  const Trk::TrackingVolume*  currVol = 0;
  const Trk::TrackingVolume*  nextVol = 0;
  std::vector<unsigned int> solutions;
  const Trk::TrackingVolume* assocVol = 0;  
  //double tol = 0.001;
  //double path = 0.;
  if (!m_highestVolume ) m_highestVolume = m_navigator->highestVolume();

  emptyGarbageBin(&parm);
    
  // verify current position
  Amg::Vector3D gp = parm.position();
  if ( vol && vol->inside(gp,m_tolerance) ) {
    staticVol = vol; 
  } else {
    currVol =  m_navigator->trackingGeometry()->lowestStaticTrackingVolume(gp);    
    const Trk::TrackingVolume* nextStatVol = 0;
    if ( m_navigator->atVolumeBoundary(currPar,currVol,dir,nextStatVol,m_tolerance) && nextStatVol != currVol ) 
      currVol = nextStatVol;
    if (currVol && currVol != vol) {
      const Trk::AlignableTrackingVolume* aliTG = dynamic_cast<const Trk::AlignableTrackingVolume*> (currVol);
      if (aliTG) staticVol = aliTG;
    }
  }

  if (!staticVol) {
     ATH_MSG_DEBUG( "  [!] failing in retrieval of AlignableTV, return 0" );
     return returnParameters;
  } 

  // TODO if volume entry go to entry of misaligned volume

  // save volume entry if collection present

  if (m_identifiedParameters) {
    const Trk::BinnedMaterial* binMat = staticVol->binnedMaterial();
    if (binMat) {
      const Trk::IdentifiedMaterial* binIDMat = binMat->material(currPar->position());
      if (binIDMat->second>0) m_identifiedParameters->push_back(std::pair<const Trk::TrackParameters* , int>  ( currPar->clone(), binIDMat->second));  
    }
  }
   
  // navigation surfaces
  if( m_navigSurfs.capacity() > m_maxNavigSurf ) m_navigSurfs.reserve(m_maxNavigSurf); 
  m_navigSurfs.clear();

  if (destSurf) m_navigSurfs.push_back(std::pair<const Trk::Surface*,Trk::BoundaryCheck>(destSurf,false));
  
  // assume new static volume, retrieve boundaries
  m_currentStatic = staticVol;
  m_staticBoundaries.clear();
  const std::vector< SharedObject<const BoundarySurface<TrackingVolume> > > bounds = staticVol->boundarySurfaces();
  for (unsigned int ib=0; ib< bounds.size(); ib++ ){
    const Trk::Surface& surf = (bounds[ib].getPtr())->surfaceRepresentation();
    m_staticBoundaries.push_back(std::pair<const Trk::Surface*,Trk::BoundaryCheck>(&surf,true));
  }
  
  m_navigSurfs.insert(m_navigSurfs.end(),m_staticBoundaries.begin(),m_staticBoundaries.end());

  // current dense
  m_currentDense =  staticVol;
   
  // ready to propagate      
  // till: A/ static volume boundary(bcheck=true) , B/ destination surface(bcheck=false)
   
   nextVol = 0;
   while (currPar) {      
     double path=0.;
     std::vector<unsigned int> solutions;
     // propagate now
     ATH_MSG_DEBUG( "  [+] Starting propagation at position  " << positionOutput(currPar->position())  
         << " (current momentum: " << currPar->momentum().mag() << ")" ); 
     ATH_MSG_DEBUG( "  [+] " << m_navigSurfs.size() << " target surfaces in '" << m_currentDense->volumeName() <<"'.");   
     //  arguments : inputParameters, vector of navigation surfaces, propagation direction, b field service, particle type, result,
     //              material collection, intersection collection, path limit, switch for use of path limit, switch for curvilinear on return, current TG volume
     if(m_dumpCache&&m_extrapolationCache) ATH_MSG_DEBUG( "  prop.propagateM " << m_extrapolationCache );
     // propagateM takes intersections by non-const reference to a pointer.
     // however, it does not modify the pointer, so the parameter
     // should really be passed just by pointer.
     identifiedParameters_t* intersections = m_identifiedParameters.get();
     const Trk::TrackParameters* nextPar = prop.propagateM(*currPar,m_navigSurfs,dir, m_fieldProperties,particle,solutions,
                 m_matstates,intersections,path,false,false,m_currentDense, m_extrapolationCache);

     ATH_MSG_VERBOSE( "  [+] Propagation done. " ); 
     if (nextPar)  
        ATH_MSG_DEBUG( "  [+] Position after propagation -   at " << positionOutput(nextPar->position())); 

     if (nextPar)  ATH_MSG_DEBUG( "  [+] Number of intersection solutions: " << solutions.size() );
     if (nextPar) throwIntoGarbageBin(nextPar);
     if (nextPar) {    
       // destination surface
       if (destSurf && solutions[0]==0 ) return nextPar->clone();
       if (destSurf && solutions.size()>1 && solutions[1]==0 ) return nextPar->clone();
       // destination surface missed ? 
       if (destSurf) {
   double dist = 0.;
   Trk::DistanceSolution distSol = destSurf->straightLineDistanceEstimate(nextPar->position(),
                    nextPar->momentum().normalized());
   if (distSol.numberOfSolutions()>0 ) {
     dist = distSol.first();
     if ( distSol.numberOfSolutions()>1 && fabs(dist) < m_tolerance ) dist = distSol.second();
     if ( distSol.numberOfSolutions()>1 && dist*dir < 0. && distSol.second()*dir > 0. ) dist = distSol.second();    
         } else {
           dist = distSol.toPointOfClosestApproach();
         } 
   if (dist*dir < 0.) {
     ATH_MSG_DEBUG( "  [+] Destination surface missed ? "<< dist << "," << dir );
     m_parametersAtBoundary.resetBoundaryInformation();
     return returnParameters;
   }
   ATH_MSG_DEBUG( "  [+] New 3D-distance to destinatiion    - d3 = " << dist*dir ); 
       }
       
       int iDest = destSurf ? 1 : 0;
       unsigned int iSol  = 0;
       while ( iSol < solutions.size() ) {         
   if ( solutions[iSol] < iDest + m_staticBoundaries.size() ) {
           // TODO if massive boundary coded, add the material effects here 
     // static volume boundary; return to the main loop : TODO move from misaligned to static
     unsigned int index = solutions[iSol]-iDest;
     // use global coordinates to retrieve attached volume (just for static!)
     nextVol = (m_currentStatic->boundarySurfaces())[index].getPtr()->attachedVolume(nextPar->position(),nextPar->momentum(),dir);
     // double check the next volume
     if ( nextVol && !(nextVol->inside(nextPar->position()+0.01*dir*nextPar->momentum().normalized(),m_tolerance) ) ) {
       ATH_MSG_DEBUG( "  [!] WARNING: wrongly assigned static volume ?"<< m_currentStatic->volumeName()<<"->" << nextVol->volumeName() );
       nextVol = m_navigator->trackingGeometry()->lowestStaticTrackingVolume(nextPar->position()+0.01*nextPar->momentum().normalized());
       if (nextVol) ATH_MSG_DEBUG( "  new search yields: "<< nextVol->volumeName() );          
     }
     // end double check - to be removed after validation of the geometry gluing 
     // lateral exit from calo sample can be handled here 
     if (m_identifiedParameters) {
       const Trk::BinnedMaterial* binMat = staticVol->binnedMaterial();
       if (binMat) {
         const Trk::IdentifiedMaterial* binIDMat = binMat->material(nextPar->position());
               // save only if entry to the sample present, the exit missing and non-zero step in the sample
               if (binIDMat && binIDMat->second>0 && m_identifiedParameters->size() &&  m_identifiedParameters->back().second== binIDMat->second) {
                 double s = (nextPar->position()-m_identifiedParameters->back().first->position()).mag(); 
     if (s>0.001) m_identifiedParameters->push_back(std::pair<const Trk::TrackParameters*,int> (nextPar->clone(), -binIDMat->second));
         }  
       }
     }
           // end lateral exit handling
     if (nextVol != m_currentStatic ) {
       m_parametersAtBoundary.boundaryInformation(nextVol,nextPar,nextPar);         
       ATH_MSG_DEBUG("  [+] StaticVol boundary reached of '" <<m_currentStatic->volumeName() << "'.");
       if ( m_navigator->atVolumeBoundary(nextPar,m_currentStatic,dir,assocVol,m_tolerance) && assocVol != m_currentStatic )
         m_currentDense = m_useMuonMatApprox ? nextVol : m_highestVolume;
       // no next volume found --- end of the world
       if ( !nextVol )
         ATH_MSG_DEBUG( "  [+] Word boundary reached        - at " << positionOutput(nextPar->position()) );
       // next volume found and parameters are at boundary
       if ( nextVol && nextPar ){ 
         ATH_MSG_DEBUG( "  [+] Crossing to next volume '" << nextVol->volumeName() << "'");
         ATH_MSG_DEBUG( "  [+] Crossing position is         - at " <<  positionOutput(nextPar->position()) );  
               if (!destSurf) return nextPar->clone();    //  return value differs between e->surface (cached boundary values used)
                                                          //     implicit : parameters at boundary returned    
       }     
       return returnParameters;
     }
   }
   iSol++;
       }
     } else {
       ATH_MSG_DEBUG( "  [!] Propagation failed, return 0" );
       m_parametersAtBoundary.boundaryInformation(m_currentStatic,nextPar,nextPar);     
       return returnParameters;
     }
     currPar = nextPar;
   }
   
   return returnParameters;
}        

std::pair<const Trk::TrackParameters*,const Trk::Layer*> Trk::Extrapolator::extrapolateToNextStation(
                                                                                        const IPropagator& prop,
                                                                                        const Trk::TrackParameters& parm,
                                                                                        PropDirection dir,
                                                                                        BoundaryCheck bcheck,
                                                                                        ParticleHypothesis particle,
                                                                                        MaterialUpdateMode matupmode) const
{
   ATH_MSG_DEBUG( "M-[" << ++m_methodSequence << "] extrapolateToNextStation(...) " );
  // material update on detached volume layer representation only

  //double tol = 0.001;

  emptyGarbageBin(&parm);

  // initialize the return parameters vector
  const Trk::TrackParameters* returnParameters = 0;

  //resolve position: static or detached ?
  const Trk::TrackingVolume* currVol = m_navigator->trackingGeometry()->lowestStaticTrackingVolume(parm.position());
  const Trk::TrackingVolume* nextVol=0;
  if ( m_navigator->atVolumeBoundary(&parm,currVol,dir,nextVol,m_tolerance) && nextVol != currVol ) currVol = nextVol;
  if (!m_highestVolume ) m_highestVolume = m_navigator->highestVolume();
  
  const Trk::TrackParameters* currPar = &parm;
  double path = 0.;
  while ( currPar && currVol && !currVol->confinedDetachedVolumes() ) {
    const Trk::NavigationCell cross = m_navigator->nextDenseTrackingVolume(prop,*currPar,0,dir,particle,*currVol,path);
    currPar = cross.parametersOnBoundary;
    if (currPar) {
      // currPar = cross.parametersOnBoundary->clone();
      currPar = cross.parametersOnBoundary;
      throwIntoGarbageBin(currPar);
    }
    currVol = cross.nextVolume;    
  } 
  
  // no luck
  if (!currPar || !currVol || !currVol->confinedDetachedVolumes() ) return std::pair<const Trk::TrackParameters*,const Trk::Layer*>(returnParameters,0);

  // prepare vector of surfaces
  if( m_navigSurfs.capacity() > m_maxNavigSurf ) m_navigSurfs.reserve(m_maxNavigSurf); 
  if( m_navigVols.capacity() > m_maxNavigVol ) m_navigVols.reserve(m_maxNavigVol); 
  m_navigSurfs.clear();
  m_navigVols.clear();

  // retrieve static volume boundary
  const std::vector< SharedObject<const BoundarySurface<TrackingVolume> > > bounds = currVol->boundarySurfaces();
  for (unsigned int ib=0; ib< bounds.size(); ib++ ){
    const Trk::Surface* nextSurface = &((bounds[ib].getPtr())->surfaceRepresentation());
    m_navigSurfs.push_back(std::pair<const Trk::Surface*,Trk::BoundaryCheck>(nextSurface,true));
  }

  // retrieve DV layer representations
  const std::vector<const Trk::DetachedTrackingVolume*>* detVols = currVol->confinedDetachedVolumes();
  if (detVols) {
    std::vector<const Trk::DetachedTrackingVolume*>::const_iterator dIter = detVols->begin();
    for (; dIter != detVols->end(); dIter++) {
      const Trk::Layer* lay = (*dIter)->layerRepresentation(); 
      if ( lay ) {
  Trk::BoundaryCheck checkBounds = lay->layerType() > 0 ? bcheck : Trk::BoundaryCheck(true);
         std::pair<const Trk::Surface*, Trk::BoundaryCheck>  newSurf(&(lay->surfaceRepresentation()),checkBounds); 
         m_navigSurfs.push_back(newSurf);
         m_navigVols.push_back(*dIter);
      }
    } 
  }
  // ready to propagate      
  // till: A/ static volume boundary (loop back) , B/ successful hit of an active layer representation

  // resolve the use of dense volumes
  m_dense = (currVol->geometrySignature()==Trk::MS && m_useMuonMatApprox ) || (currVol->geometrySignature()!=Trk::MS && m_useDenseVolumeDescription );

  nextVol = 0;     
  double totalPath=0.;
  while (currPar) {      
    std::vector<unsigned int> solutions;
    const Trk::TrackingVolume* propagVol = m_dense ? currVol : m_highestVolume;
    //const Trk::TrackParameters* nextPar = prop.propagate(*currPar,m_navigSurfs,dir,*propagVol,particle,solutions,path);
    const Trk::TrackParameters* nextPar = prop.propagate(*currPar,m_navigSurfs,dir, m_fieldProperties,particle,solutions,path,false,false,propagVol);
    totalPath +=path;
    if (nextPar) throwIntoGarbageBin(nextPar);
    if (nextPar) {         
      Amg::Vector3D gp = nextPar->position();
      // static volume boundary ?
      if ( m_navigator->atVolumeBoundary(nextPar,currVol,dir,nextVol,m_tolerance) ) {
         if ( nextVol != currVol ) {
         const Trk::TrackParameters* atBoundary = nextPar->clone();
         m_parametersAtBoundary.boundaryInformation(nextVol,atBoundary,atBoundary);
         if (!nextVol) return std::pair<const Trk::TrackParameters*,const Trk::Layer*>(atBoundary,0);      
         throwIntoGarbageBin(atBoundary);     
         if ( nextVol && nextPar ) {
           if (nextVol->geometrySignature()!=Trk::MS) return std::pair<const Trk::TrackParameters*,const Trk::Layer*>(atBoundary,0);
           ATH_MSG_DEBUG( "  [+] Crossing to next volumat     - at " <<  positionOutput(nextPar->position()) );  
           return extrapolateToNextStation(prop,*nextPar,dir,bcheck,particle,matupmode);
         }
       } 
      }
      // next layer representation ? active(->return) or passive(->loop back) ?
      std::vector<std::pair<const Trk::Surface*,Trk::BoundaryCheck> >::iterator vsIter = m_navigSurfs.begin();
      for (; vsIter != m_navigSurfs.end(); vsIter++) if ( (*vsIter).first->isOnSurface(gp,bcheck,m_tolerance,m_tolerance) ) break; 
      if ( vsIter!= m_navigSurfs.end() ) {
        bool identified = false;
  std::vector<const Trk::DetachedTrackingVolume*>::const_iterator dIter = m_navigVols.begin();
  for (; dIter != m_navigVols.end(); dIter++)
    if ( (*dIter)->layerRepresentation()->surfaceRepresentation().isOnSurface(nextPar->position(),bcheck,m_tolerance,m_tolerance) ) break;
        if ( dIter != m_navigVols.end()) identified = true;
        if (!identified) {
    dIter = m_navigVols.begin();
    for (; dIter != m_navigVols.end(); dIter++) if ( (*dIter)->trackingVolume()->inside((*vsIter).first->center(),m_tolerance) ) break;
    if ( dIter != m_navigVols.end()) identified = true;
        }
        if (identified) {
          const Trk::Layer* lay = (*dIter)->layerRepresentation();
          if (lay && lay !=m_lastMaterialLayer ) {
      // material update (from detached trackingvolume)
      const IMaterialEffectsUpdator* currentUpdator = m_subUpdators[(*dIter)->geometrySignature()];
            if (currentUpdator) {
              const Trk::TrackParameters* upNext =  currentUpdator->update(nextPar, *lay, dir, particle,matupmode);
              if (upNext != nextPar) throwIntoGarbageBin(upNext);
              nextPar = upNext;
      }  
      if (m_cacheLastMatLayer) m_lastMaterialLayer = lay;
            if (nextPar) {  
        if ( lay->layerType() > 0 ) return std::pair<const Trk::TrackParameters*,const Trk::Layer*>(nextPar->clone(),lay);
      } else return std::pair<const Trk::TrackParameters*,const Trk::Layer*>(nextPar,0); 
    }         
        }          
      }     
    } else {
      ATH_MSG_DEBUG( "  [!] Propagation loop fails -> return 0." );
    }
    currPar = nextPar;
  } 
  return std::pair<const Trk::TrackParameters*,const Trk::Layer*>(returnParameters,0);
}        

const Trk::TrackParameters* Trk::Extrapolator::extrapolateDirectly(const IPropagator& prop,
                                                                   const Trk::TrackParameters& parm,
                                                                   const Trk::Surface& sf,
                                                                   Trk::PropDirection dir,
                                                                   Trk::BoundaryCheck bcheck,
                                                                   Trk::ParticleHypothesis particle) const
{   
    // statistics && sequence output ----------------------------------------
    ++m_extrapolateDirectlyCalls; 
    if (m_methodSequence) ++m_methodSequence;

    // extrapolateDirectly invalidates recall
    resetRecallInformation();
    // now du the stuff
    const Trk::TrackingVolume*  currentVolume = m_navigator->highestVolume();

    // --------------------------------------------------------------------------------------
    ATH_MSG_DEBUG( "P-[" << m_methodSequence << "] extrapolateDirectly(...) inside '"
          << currentVolume->volumeName() << "' to destination surface. " ); 
    
    //if (currentVolume) return prop.propagate(parm, sf, dir, bcheck, *currentVolume, particle);
    if (currentVolume) return prop.propagate(parm, sf, dir, bcheck,  m_fieldProperties, particle);
    return 0;
}

const Trk::TrackParameters* Trk::Extrapolator::extrapolateToVolume(const IPropagator& prop,
                   const TrackParameters& parm,
                   const TrackingVolume& vol,
                   PropDirection dir,
                   ParticleHypothesis particle) const
{
  ATH_MSG_DEBUG( "V-[" << m_methodSequence <<"] extrapolateToVolume(...) to volume '" << vol.volumeName() << "'.");
  const Trk::TrackParameters* returnParms = 0;
  Trk::PropDirection propDir = dir==Trk::oppositeMomentum ? dir : Trk::alongMomentum;
  double dist = 0.;

  // retrieve boundary surfaces, order them according to distance estimate  
  std::vector<std::pair<const Trk::Surface*,double> >* surfaces = 
    new std::vector<std::pair<const Trk::Surface*,double> > ;
  std::vector<std::pair<const Trk::Surface*,double> >::iterator sIter; 

  const std::vector< SharedObject<const BoundarySurface<TrackingVolume> > > bounds = vol.boundarySurfaces();
  for (unsigned int ib=0; ib< bounds.size(); ib++ ){
    const Trk::Surface* nextSurface = &((bounds[ib].getPtr())->surfaceRepresentation());
    if (nextSurface) { 
      Trk::DistanceSolution distSol = nextSurface->straightLineDistanceEstimate(parm.position(),propDir*parm.momentum().normalized());
      if (distSol.numberOfSolutions()>0 ) {
          dist = distSol.first();
      } else {
          dist = distSol.toPointOfClosestApproach();
      }
      if ( surfaces->size() && distSol.numberOfSolutions()>=0 && dist<surfaces->back().second ) {
        sIter = surfaces->begin();
        while (sIter != surfaces->end()) {
          if ( dist < (*sIter).second ) break;
    sIter++;
  }
  sIter = surfaces->insert(sIter,(std::pair<const Trk::Surface*,double>(nextSurface,dist)));

      } else {
  surfaces->push_back(std::pair<const Trk::Surface*,double>(nextSurface,dist));
      }
    }
  }

  // solution along path
  sIter = surfaces->begin();
  while (sIter != surfaces->end()) {
    if ( (*sIter).second> 0 ) {
      returnParms = extrapolate(prop,parm,*((*sIter).first),propDir,true,particle) ;
      if (returnParms) break; 
    }
    sIter++;
  }

  if ( !returnParms && dir==anyDirection ) {
    std::vector<std::pair<const Trk::Surface*,double> >::reverse_iterator rsIter = surfaces->rbegin();
    while (rsIter != surfaces->rend()) {
      if ( (*rsIter).second < 0 ) {
  returnParms = extrapolate(prop,parm,*((*rsIter).first),Trk::oppositeMomentum,true,particle) ;
  if (returnParms) break; 
      }
      rsIter++;
    }
  } 

  delete surfaces;
  m_methodSequence = 0;
  return returnParms;
}

// Configured AlgTool extrapolation methods ---------------------------------------------------------------------------------------/

const Trk::TrackParameters*  Trk::Extrapolator::extrapolate(const TrackParameters& parm,
                                                            const Surface& sf,
                                                            PropDirection dir,
                                                            BoundaryCheck bcheck,
                                                            ParticleHypothesis particle,
                                                            MaterialUpdateMode matupmode,
                  Trk::ExtrapolationCache* cache) const
{

  m_extrapolationCache = cache;
  m_cacheEloss = cache ? dynamic_cast<const Trk::EnergyLoss*>(cache->eloss()) : 0;
  
  if(cache && m_dumpCache) {
    ATH_MSG_DEBUG( "  In extrapolate cache pointer input: " << cache << " m_extrapolationCache " << m_extrapolationCache ); 
    if(m_extrapolationCache) dumpCache(" In extrapolate ");
  }
  
  if (m_configurationLevel<10){
    // chose the propagator fromt he geometry signature -- start with default
    const IPropagator* currentPropagator = m_subPropagators.size() ? m_subPropagators[Trk::Global] : 0;
    if (currentPropagator) return(extrapolate((*currentPropagator), parm, sf, dir, bcheck, particle, matupmode));
  }
   ATH_MSG_ERROR( "  [!] No default Propagator is configured ! Please check jobOptions." );
   return 0;  
}


const std::vector<const Trk::TrackParameters*>*  Trk::Extrapolator::extrapolateStepwise(
                                                            const Trk::TrackParameters& parm,
                                                            const Trk::Surface& sf,
                                                            Trk::PropDirection dir,
                                                            Trk::BoundaryCheck bcheck,
                                                            Trk::ParticleHypothesis particle) const
{   
   if (m_configurationLevel<10){
    // set propagator to the sticky one, will be adopted if m_stickyConfiguration == false
    const IPropagator* currentPropagator = m_subPropagators.size() ? m_subPropagators[Trk::Global] : 0;
     if (currentPropagator) return(extrapolateStepwise((*currentPropagator), parm, sf, dir, bcheck, particle));
   }
   ATH_MSG_ERROR( "  [!] No default Propagator is configured ! Please check jobOptions." );
   return 0;   
}


const Trk::TrackParameters*  Trk::Extrapolator::extrapolate(const Trk::Track& trk,
                                                            const Trk::Surface& sf,
                                                            Trk::PropDirection dir,
                                                            Trk::BoundaryCheck bcheck,
                                                            Trk::ParticleHypothesis particle,
                                                            MaterialUpdateMode matupmode,
                  Trk::ExtrapolationCache* cache) const
{

  const IPropagator* searchProp = 0;
  // use global propagator for the search
  if (m_searchLevel<2 && m_subPropagators.size())
      searchProp = m_subPropagators[Trk::Global]; 
  
  const Trk::TrackParameters* closestTrackParameters = m_navigator->closestParameters(trk, sf, searchProp);
  if (closestTrackParameters){
    return(extrapolate(*closestTrackParameters, sf, dir, bcheck, particle, matupmode, cache));
  } else {
    closestTrackParameters = *(trk.trackParameters()->begin());
    if (closestTrackParameters) return(extrapolate(*closestTrackParameters, sf, dir, bcheck, particle, matupmode,cache));
  }
  
  return 0;
}

const std::vector<const Trk::TrackParameters*>*  Trk::Extrapolator::extrapolateBlindly(
                                                                   const Trk::TrackParameters& parm,
                                                                   Trk::PropDirection dir,
                                                                   Trk::BoundaryCheck bcheck,
                                                                   Trk::ParticleHypothesis particle,
                                                                   const Trk::Volume* boundaryVol) const
{
   if (m_configurationLevel<10){
    // set propagator to the global one
    const IPropagator* currentPropagator = m_subPropagators.size() ? m_subPropagators[Trk::Global] : 0;
     if (currentPropagator) return(extrapolateBlindly((*currentPropagator), parm, dir, bcheck, particle, boundaryVol));
   }
   ATH_MSG_ERROR( "  [!] No default Propagator is configured ! Please check jobOptions." );
   return 0; 
}

const Trk::TrackParameters*  Trk::Extrapolator::extrapolateDirectly(const Trk::TrackParameters& parm,
                                                                    const Trk::Surface& sf,
                                                                    Trk::PropDirection dir,
                                                                    Trk::BoundaryCheck bcheck,
                                                                    Trk::ParticleHypothesis particle) const
{
   if (m_configurationLevel<10){
    // set propagator to the global one - can be reset inside the next methode (once volume information is there)
    const IPropagator* currentPropagator = m_subPropagators.size() ? m_subPropagators[Trk::Global] : 0;
    if (currentPropagator) return (extrapolateDirectly((*currentPropagator), parm, sf, dir, bcheck, particle));
   }
   ATH_MSG_ERROR( "  [!] No default Propagator is configured ! Please check jobOptions." );
   return 0; 
}

std::pair<const Trk::TrackParameters*,const Trk::Layer*>  Trk::Extrapolator::extrapolateToNextActiveLayer(const TrackParameters& parm,
                                                                            PropDirection dir,
                                                                            BoundaryCheck bcheck,
                                                                            ParticleHypothesis particle,
                                                                            MaterialUpdateMode matupmode) const
{
   if (m_configurationLevel<10){
    // set propagator to the MS one - can be reset inside the next methode (once volume information is there)
    const IPropagator* currentPropagator = m_subPropagators.size() ? m_subPropagators[Trk::MS] : 0;
    if (currentPropagator) return(extrapolateToNextActiveLayer((*currentPropagator), parm, dir, bcheck, particle, matupmode));
   }
   ATH_MSG_ERROR( "[!] No default Propagator is configured ! Please check jobOptions." );
   return std::pair<const Trk::TrackParameters*,const Trk::Layer*>(0,0); 

}

std::pair<const Trk::TrackParameters*,const Trk::Layer*> Trk::Extrapolator::extrapolateToNextActiveLayerM(const TrackParameters& parm,
                       PropDirection dir,
                       BoundaryCheck bcheck,
                       std::vector<const Trk::TrackStateOnSurface*>& material,
                       ParticleHypothesis particle,
                       MaterialUpdateMode matupmode) const
{
   if (m_configurationLevel<10){
    // set propagator to the MS one - can be reset inside the next methode (once volume information is there)
        // set propagator to the MS one - can be reset inside the next methode (once volume information is there)
    const IPropagator* currentPropagator = m_subPropagators.size() ? m_subPropagators[Trk::MS] : 0;
    if (currentPropagator) return(extrapolateToNextActiveLayerM((*currentPropagator), parm, dir, bcheck, material, particle, matupmode));
   }
   ATH_MSG_ERROR( "  [!] No default Propagator is configured ! Please check jobOptions." );
   return std::pair<const Trk::TrackParameters*,const Trk::Layer*>(0,0); 

}

std::pair<const Trk::TrackParameters*,const Trk::Layer*> Trk::Extrapolator::extrapolateToNextStation(const TrackParameters& parm,
                                                                                                               PropDirection dir,
                                                                                                               BoundaryCheck bcheck,
                                                                                                               ParticleHypothesis particle,
                                                                                                               MaterialUpdateMode matupmode) const
{
   if (m_configurationLevel<10){
    // set propagator to the global one - can be reset inside the next methode (once volume information is there)
    const IPropagator* currentPropagator = m_subPropagators.size() ? m_subPropagators[Trk::MS] : 0;
    if (currentPropagator) return(extrapolateToNextStation((*currentPropagator), parm, dir, bcheck, particle,matupmode));
   }
   ATH_MSG_ERROR( "  [!] No default Propagator is configured ! Please check jobOptions." );
   return std::pair<const Trk::TrackParameters*,const Trk::Layer*>(0,0); 

}

const Trk::TrackParameters*  Trk::Extrapolator::extrapolateToVolume(const Trk::TrackParameters& parm,
                    const Trk::TrackingVolume& vol,
                    PropDirection dir,
                    ParticleHypothesis particle) const
{
   if (m_configurationLevel<10){
     // take the volume signatrue to define the right propagator
     const IPropagator* currentPropagator = m_subPropagators.size() ? m_subPropagators[vol.geometrySignature()] : 0;
     if (currentPropagator) return (extrapolateToVolume(*currentPropagator, parm, vol, dir, particle));
   }
   ATH_MSG_ERROR( "  [!] No default Propagator is configured ! Please check jobOptions." );
   return 0;
}

const std::vector<const Trk::TrackStateOnSurface*>* Trk::Extrapolator::extrapolateM(const TrackParameters& parameters,
                                                                                    const Surface& sf,
                                                                                    PropDirection dir,
                                                                                    BoundaryCheck bcheck,
                                                                                    ParticleHypothesis particle,
                                                                                    Trk::ExtrapolationCache* cache) const
{

  ATH_MSG_DEBUG( "C-[" << m_methodSequence << "] extrapolateM()" );
  // create a new vector for the material to be collected  
  m_matstates = new std::vector<const Trk::TrackStateOnSurface*>;

  if(m_dumpCache&&cache) ATH_MSG_DEBUG(" extrapolateM pointer m_extrapolationCache " << cache << " x0tot "  << cache->x0tot());

  // collect the material
  const Trk::TrackParameters* parameterAtDestination = extrapolate(parameters,sf,dir,bcheck,particle,Trk::addNoise, cache);
  // there are no parameters 
  if (!parameterAtDestination && m_requireMaterialDestinationHit){
    ATH_MSG_VERBOSE( "  [!] Destination surface for extrapolateM has not been hit (required through configuration). Return 0");
    // loop over and clean up
    std::vector<const Trk::TrackStateOnSurface*>::iterator tsosIter    = m_matstates->begin();
    std::vector<const Trk::TrackStateOnSurface*>::iterator tsosIterEnd = m_matstates->end();
    for ( ; tsosIter != tsosIterEnd; ++tsosIter)  delete (*tsosIter);
    delete m_matstates; m_matstates = 0;
    // bail out
    return 0;
  } 
  if (parameterAtDestination){
    ATH_MSG_VERBOSE( "  [+] Adding the destination surface to the TSOS vector in extrapolateM() ");
    m_matstates->push_back(new TrackStateOnSurface(0,parameterAtDestination,0,0));
  } else 
    ATH_MSG_VERBOSE( "  [-] Destination surface was not hit extrapolateM(), but not required through configuration.");
  // assign the temporary states
  std::vector<const Trk::TrackStateOnSurface*> *tmpMatStates = m_matstates;
  m_matstates=0;
  // retunr the material states
  return tmpMatStates;  
}

const std::vector<const Trk::TrackParameters*>* Trk::Extrapolator::extrapolateM(const TrackParameters& ,
                                                                                const Surface& ,
                                                                                PropDirection ,
                                                                                BoundaryCheck ,
                                                                                std::vector<MaterialEffectsOnTrack>& ,
                                                                                std::vector<Trk::TransportJacobian*>& ,
                                                                                ParticleHypothesis, 
                                                                                Trk::ExtrapolationCache*) const
{
  ATH_MSG_DEBUG( "C-[" << ++m_methodSequence << "] extrapolateM(..) with jacobian collection - Not implemented yet." );
  return 0;
}

// the validation action -> propagated to the SubTools
void Trk::Extrapolator::validationAction() const
{
  // record the updator validation information
  for (unsigned int imueot = 0; imueot < m_subUpdators.size(); ++imueot) 
      m_subUpdators[imueot]->validationAction();      
  // record the navigator validation information
  m_navigator->validationAction();
}

// ----------------------- The private Volume extrapolation methods -------------------------------------------------
const Trk::TrackParameters* Trk::Extrapolator::extrapolateInsideVolume(const IPropagator& prop,
                                                                       const TrackParameters& parm,
                                                                       const Surface& sf,
                                                                       const Layer* assLayer,
                                                                       const TrackingVolume& tvol,
                                                                       PropDirection dir,
                                                                       BoundaryCheck bcheck,
                                                                       ParticleHypothesis particle,
                                                                       MaterialUpdateMode matupmode
                                                                       ) const
{
  // ---> C) detached volumes exist
  if (tvol.confinedDetachedVolumes() )
      return extrapolateWithinDetachedVolumes(prop, parm, sf, tvol, dir, bcheck, particle,matupmode);
  // ---> A) static layers exist
  return insideVolumeStaticLayers(false, prop, parm, assLayer, tvol, dir, bcheck, particle,matupmode);
}


const Trk::TrackParameters* Trk::Extrapolator::extrapolateWithinDetachedVolumes( 
                                                              const IPropagator& prop ,
                                                              const Trk::TrackParameters& parm,
                                                              const Surface& sf,
                                                              const TrackingVolume& tvol ,
                                                              PropDirection dir,
                                                              BoundaryCheck bcheck,
                                                              ParticleHypothesis particle,
                                                              MaterialUpdateMode matupmode
                                                              ) const
{
  
   // method sequence output ---------------------------------
   ++m_methodSequence;
   ATH_MSG_DEBUG( "M-["<< m_methodSequence << "] extrapolateWithinDetachedVolumes() inside '"
           << tvol.volumeName() << "' to destination surface. " );

   double dist = 0.;
   //double tol = 0.001;

   // initialization
   const Trk::TrackParameters* nextParameters   = (&parm);
   const Trk::TrackingVolume*  currVol          = &tvol;
   const Trk::TrackParameters* returnParameters = 0;
   //============================================================
  
   // arbitrary surface or destination layer ?
   //bool loopOverLayers = false;
   const Trk::Layer* destinationLayer = m_navigator->trackingGeometry()->associatedLayer(sf.center());
   //if ( destinationLayer ) loopOverLayers = true;
   
   // initial distance to surface
   Trk::DistanceSolution distSol = sf.straightLineDistanceEstimate(nextParameters->position(),dir*nextParameters->momentum().normalized());
   if (distSol.numberOfSolutions()>0 ) {
     dist = distSol.first();
     //if ( distSol.numberOfSolutions()>1 && fabs(dist) < m_tolerance ) dist = distSol.second();
   } else {
     dist = distSol.toPointOfClosestApproach();
   }

   if ( destinationLayer && destinationLayer->isOnLayer(nextParameters->position()) ) {
     ATH_MSG_DEBUG( "  [-] Already at destination layer, distance:" << dist );
     //if ( dist >= 0 ) return prop.propagate(*nextParameters,sf, dir,bcheck,*currVol,particle);
     //const Trk::TrackParameters* fwd = prop.propagate(*nextParameters,sf, dir,bcheck,*currVol,particle);  
     const Trk::TrackParameters* fwd = prop.propagate(*nextParameters,sf, dir,bcheck,  m_fieldProperties ,particle, false, currVol);  
         
     if (fwd ) return fwd;
     else  {
       Trk::PropDirection oppDir = ( dir!=Trk::oppositeMomentum  ) ? Trk::oppositeMomentum : Trk::alongMomentum;       
       //return prop.propagate(*nextParameters,sf,oppDir,bcheck,*currVol,particle);
       return prop.propagate(*nextParameters,sf,oppDir,bcheck, m_fieldProperties,particle, false, currVol);
     }
   }

   if ( fabs(dist)<m_tolerance ) {
     ATH_MSG_DEBUG( "  [-] Already at the destination surface." );
     
     //if (dist >= 0.) return prop.propagate(*nextParameters,sf,dir,bcheck,*currVol,particle);
     if (dist >= 0.) return prop.propagate(*nextParameters,sf,dir,bcheck, m_fieldProperties,particle, false, currVol);
     else  {
       Trk::PropDirection oppDir = ( dir!=Trk::oppositeMomentum  ) ? Trk::oppositeMomentum : Trk::alongMomentum;       
       //return prop.propagate(*nextParameters,sf,oppDir,bcheck,*currVol,particle);
       return prop.propagate(*nextParameters,sf,oppDir,bcheck, m_fieldProperties,particle,false,currVol);
     }
   } else if (dist<0. ) {
     ATH_MSG_DEBUG( "  [!] Initial 3D-distance to the surface negative (" << dist << ") -> skip extrapolation." );
     m_parametersAtBoundary.resetBoundaryInformation();     
     return returnParameters;
   }

   ATH_MSG_DEBUG( "  [+] Initial 3D-distance to destination - d3 = " << dist );

   // loop over material layers till a/ destination layer found (accept solutions outside surface boundary)
   //                                b/ boundary reached
   //                                c/ negative distance to destination surface ( propagate directly to the surface )
     
   // ---------------------------- main loop over next material layers ---------------------------------------------------
   while (nextParameters) {
           
     Trk::BoundaryCheck bchk = false;
     const Trk::TrackParameters* onNextLayer =  extrapolateToNextMaterialLayer(prop,
                                                                               *nextParameters,
                                                                               &sf,
                                                                               currVol,
                                                                               dir,bchk,
                                                                               particle,
                                                                               matupmode);       
     if (onNextLayer) {      // solution with the destination surface ?     
       throwIntoGarbageBin(onNextLayer);
       // isOnSurface dummy for Perigee, use straightline distance estimate instead
       // if ( sf.isOnSurface(onNextLayer->position(),bchk,m_tolerance,m_tolerance) ) {
       Trk::DistanceSolution distSol = sf.straightLineDistanceEstimate(onNextLayer->position(),dir*onNextLayer->momentum().normalized());
       double currentDistance =  (distSol.numberOfSolutions()>0 )?  distSol.absClosest() : fabs(distSol.toPointOfClosestApproach());
       if ( currentDistance<=m_tolerance && sf.isOnSurface(onNextLayer->position(),bchk,m_tolerance,m_tolerance) ) {
         m_parametersAtBoundary.boundaryInformation(0,0,0);
         if ( !bcheck || sf.isOnSurface(onNextLayer->position(),bcheck,m_tolerance,m_tolerance)) {
           if (sf.type() != onNextLayer->associatedSurface().type()) {
       ATH_MSG_DEBUG("mismatch in destination surface type:"<<sf.type()<<","<<  onNextLayer->associatedSurface().type()
           <<":distance to the destination surface:"<<currentDistance);
             const Trk::TrackParameters* cParms=prop.propagate(*onNextLayer,sf,dir,bchk,m_fieldProperties,particle);
             return cParms;
     } 
     return onNextLayer->clone();
   }
         else return returnParameters; 
       }
     } else {
       // world boundary ?
       if (m_parametersAtBoundary.nextParameters && !m_parametersAtBoundary.nextVolume ) {
         nextParameters = onNextLayer;         
         break;
       }        
       if (!m_parametersAtBoundary.nextParameters) 
           return returnParameters;
       
       // static volume boundary:  check distance to destination
       Trk::DistanceSolution distSol = sf.straightLineDistanceEstimate(m_parametersAtBoundary.nextParameters->position(),
                                                                       dir*m_parametersAtBoundary.nextParameters->momentum().normalized());
       if (distSol.numberOfSolutions()>0 ) {
                dist= distSol.first();
       } else {
         dist = distSol.toPointOfClosestApproach();
       }        
       if ( dist < 0.) {
//             ATH_MSG_DEBUG( "  [!] destination surface missed or distance unknown, propagate directly" );
             m_parametersAtBoundary.boundaryInformation(0,0,0); 
             return returnParameters;        
       }
       if (m_parametersAtBoundary.nextVolume && (m_parametersAtBoundary.nextVolume->geometrySignature()==Trk::MS ||
             (m_parametersAtBoundary.nextVolume->geometrySignature()==Trk::Calo && m_useDenseVolumeDescription))   ) {
               if ( m_parametersAtBoundary.nextParameters ) onNextLayer = m_parametersAtBoundary.nextParameters;
               currVol = m_parametersAtBoundary.nextVolume;             
       }
     }
     nextParameters = onNextLayer; 
   }  // end loop over material layers

//   const Trk::Layer* nextLayer = 0;
//   
//   if (nextParameters) {   
//     nextLayer = m_navigator->trackingGeometry()->associatedLayer(nextParameters->position());
//     // propagate to the destination surface, final step
//     currVol = m_navigator->trackingGeometry()->lowestTrackingVolume(nextParameters->position()) ;
//     const Trk::TrackParameters* finalStep = 0;
//     if (&sf == nextParameters->associatedSurface()) {
//       if (!bcheck || sf.isOnSurface(nextParameters->position(),true,m_tolerance,m_tolerance)) finalStep = nextParameters->clone();       
//     } else if ( loopOverLayers && nextLayer == destinationLayer) {
//       finalStep = prop.propagate(*nextParameters,sf,Trk::anyDirection,bcheck,*currVol,particle);       
//     } else {    
//       finalStep = prop.propagate(*nextParameters,sf,dir,bcheck,*currVol,particle);        
//     }    
//     return finalStep;
//   }
   
   // boundary reached , return to the main loop
   ATH_MSG_DEBUG( "  [+] extrapolateWithinDetachedVolumes(...) reached static boundary, return to the main loop." );   
   return nextParameters;
}

void Trk::Extrapolator::extrapolateToVolumeBoundary( const IPropagator& prop,
                                                     const Trk::TrackParameters& parm,
                                                     const Layer* assLayer,
                                                     const TrackingVolume& tvol,
                                                     PropDirection dir,
                                                     BoundaryCheck bcheck,
                                                     ParticleHypothesis particle,
                                                     MaterialUpdateMode matupmode
                                                     ) const
{
  // ---> C) detached volumes exist
  if (tvol.confinedDetachedVolumes()) {
    ATH_MSG_ERROR("  [!] toVolumeBoundaryDetachedVolumes(...) with confined detached volumes? This should not happen ! " ); 
  }    
  // ---> A) static layers exist
  if (insideVolumeStaticLayers(true, prop, parm, assLayer, tvol, dir, bcheck,particle,matupmode) && m_parametersAtBoundary.navParameters)
      ATH_MSG_VERBOSE( "  [+] Boundary intersection      -   at " << positionOutput(m_parametersAtBoundary.navParameters->position()));
  return;   
}
                              
const Trk::TrackParameters* Trk::Extrapolator::insideVolumeStaticLayers(
                                                     bool toBoundary,
                                                     const IPropagator& prop,
                                                     const Trk::TrackParameters& parm,
                                                     const Trk::Layer* assLayer,
                                                     const TrackingVolume& tvol,
                                                     PropDirection dir,
                                                     BoundaryCheck bcheck,
                                                     ParticleHypothesis particle,
                                                     MaterialUpdateMode matupmode
                                                     ) const
{
   // method sequence output ---------------------------------
   ++m_methodSequence;
   // the next volume as given from the navigator
   const Trk::TrackingVolume*  nextVolume = 0;
   // initialization
   // nextParameters : parameters to be used for the extrapolation stream
   const Trk::TrackParameters* nextParameters        = (&parm);
   // navParameters : parameters to be used for the navigation stream (if possible, start from boundary parameters)
   const Trk::TrackParameters* navParameters         = m_parametersAtBoundary.navParameters ? 
         m_parametersAtBoundary.navParameters : nextParameters;
   // fallbackParameters : catch the last valid nextParameters in these ones
   const Trk::TrackParameters* fallbackParameters    = 0;

   // adjust the radial scaling for the layer search, this is for inwards vs. outwards moving 
   double rPos        = parm.position().perp();
   double rComponent  = parm.momentum().normalized().perp();
   // numerical stability
   rComponent         = rComponent < 10e-5 ? 10e-5 : rComponent;
   // a special case for closed cylinders, check if rScalor is not below numerical tolerance
   double rScalor     = ( toBoundary  && tvol.boundarySurfaces().size() == 3 ) ? 2.*rPos/rComponent : 0.5*rPos/rComponent;
   rScalor = rScalor*rScalor < 10e-10 ? 0.1 : rScalor;

   // output and fast exit if the volume does not have confined layers       
   if (toBoundary)
       ATH_MSG_VERBOSE( "S-["<< m_methodSequence << "] insideVolumeStaticLayers(...) to volume boundary of '"
                    << tvol.volumeName() << "'" );
   else { // to destination surface
       ATH_MSG_VERBOSE( "S-["<< m_methodSequence << "] insideVolumeStaticLayers(...) to destination surface in '"
                    << tvol.volumeName() << "'" );   
       // no layer case - just do the extrapolation to the destination surface
       if (!tvol.confinedLayers()){   
          ATH_MSG_VERBOSE( "  [+] Volume does not contain layers, just propagate to destination surface." );     
          // the final extrapolation to the destinationLayer
          //nextParameters = prop.propagate(parm, *m_destinationSurface, dir, bcheck, tvol, particle);
          nextParameters = prop.propagate(parm, *m_destinationSurface, dir, bcheck,  m_fieldProperties, particle);
          //nextParameters = nextParameters ? nextParameters : prop.propagate(parm, *m_destinationSurface, Trk::anyDirection, bcheck, tvol, particle);
          nextParameters = nextParameters ? nextParameters : prop.propagate(parm, *m_destinationSurface, Trk::anyDirection, bcheck,  m_fieldProperties, particle);
          return nextParameters;
     }
   }
      
   // print out the perpendicular direction best guess parameters
   ATH_MSG_VERBOSE( "  [+] Perpendicular direction of the track   : " << radialDirection(*navParameters,dir));
   // check whether to do a postupdate with the assoicated Layer
   const Trk::Layer* associatedLayer = assLayer;
   // chache the assLayer given, because this may be needed for the destination layer 
   const Trk::Layer* assLayerReference = assLayer;

   // the exit face of the last volume
   Trk::BoundarySurfaceFace exitFace = Trk::undefinedFace;

   // ============================ RESOLVE DESTINATION / STARTPOINT ============================
   // (1) ASSOCIATION
   const Trk::Layer* destinationLayer = 0;
   // will be only executed if directive is not to go to the boundary
   if (!toBoundary) {
       destinationLayer = m_destinationSurface->associatedLayer();
       if (!destinationLayer)     // (2) RECALL (very unlikely) // (3) GLOBAL SEARCH    
          destinationLayer = (m_recallSurface==m_destinationSurface && m_destinationSurface->associatedDetectorElement()) ? 
                              m_recallLayer : tvol.associatedLayer(m_destinationSurface->globalReferencePoint());
          if (destinationLayer)
              ATH_MSG_VERBOSE( "  [+] Destination layer found    - with " << layerRZoutput(*destinationLayer) );
    } // destination layer only gather if extrapolation does not go to boundary
    
    // The update on the starting layer if necessary ---------------------------------------------------------
    //    - only done in static volume setup
    //    - only done if required
    //    - only done if the parameter is on the layer
    //    - only if no volume skip has been done
    //    - only if associated layer is not destination layer (and both exist)        
    if (!m_skipInitialLayerUpdate 
        && associatedLayer
        && associatedLayer != destinationLayer
        && associatedLayer->layerMaterialProperties()
        && tvol.confinedLayers() ){

        ATH_MSG_VERBOSE("  [+] In starting volume: check for eventual necessary postUpdate and overlapSearch." );
    
    // check if the parameter is on the layer
    const Trk::Layer* parsLayer = &nextParameters->associatedSurface() ?
                    (nextParameters->associatedSurface()).associatedLayer() : 0;
    if ( (parsLayer && parsLayer==associatedLayer) 
         || associatedLayer->surfaceRepresentation().isOnSurface(parm.position(),
                                   false,
                                   0.5*associatedLayer->thickness(),
                                   0.5*associatedLayer->thickness())){
      // call the overlap search for the starting layer if asked for 
            if (m_parametersOnDetElements
            && associatedLayer->surfaceArray()  
          && m_subSurfaceLevel)
        {
                ATH_MSG_VERBOSE( "  [o] Calling overlapSearch() on start layer.");  
          overlapSearch(prop,parm,*nextParameters,*associatedLayer,tvol,dir,bcheck,particle,true);                              
      }           
                                    
      // the post-update is valid
      ATH_MSG_VERBOSE("  [+] Calling postUpdate on inital track parameters." );
            // do the post-update according to the associated Layer - parameters are either (&parm) or newly created ones
            // chose current updator
            const IMaterialEffectsUpdator* currentUpdator = subMaterialEffectsUpdator(tvol);
            if (currentUpdator) {
        const Trk::TrackParameters* upNext = currentUpdator->postUpdate(*nextParameters, *associatedLayer, dir, particle, matupmode);
              if (upNext && upNext!=nextParameters) throwIntoGarbageBin(upNext);
        nextParameters = upNext;               
      }
            // collect the material : either for extrapolateM or for the valdiation
            if (nextParameters && (m_matstates || m_materialEffectsOnTrackValidation) )
                addMaterialEffectsOnTrack(prop,*nextParameters,*associatedLayer,tvol,dir,particle);          
            // throw into garbage if an update was done
            if (nextParameters && nextParameters!= &parm) throwIntoGarbageBin( nextParameters );
            else if (!m_stopWithUpdateZero) // re-assign the start parameters
                nextParameters = (&parm);
            else {
        ATH_MSG_VERBOSE( "  [-] Initial postUpdate killed track.");
                m_parametersAtBoundary.boundaryInformation(0,0,0);  
                resetRecallInformation();     
                return 0;     
       }
    }
    } else 
        assLayer = 0; // reset the provided Layer in case no postUpdate happened: search a new one for layer2layer start

   // ============================ RESOLVE STARTPOINT  =============================
   // only if you do not have an input associated Layer
   //   - this means that a volume step has been done
 
   if (!associatedLayer){   
     ATH_MSG_VERBOSE( "  [+] Volume switch has happened, searching for entry layers." );  
       // reset the exitFace
       exitFace = m_parametersAtBoundary.exitFace;       
       // Step [1] Check for entry layers -------------------------------------------------
       associatedLayer = tvol.associatedLayer(navParameters->position());
       if ( associatedLayer && associatedLayer->layerMaterialProperties() ){
          //-------------------------------------------------------------------------------
          ATH_MSG_VERBOSE( "  [+] Entry layer to volume found  with " << layerRZoutput(*associatedLayer) );          
          // try to go to the entry Layer first - do not delete the parameters (garbage collection done by method) - set entry flag
          nextParameters = extrapolateToIntermediateLayer(prop,parm,*associatedLayer,tvol,dir,bcheck,particle,matupmode);
          // ------------------------------------------------------------------------------
          if (m_stopWithUpdateZero && intptr_t(nextParameters) == 1){
               ATH_MSG_VERBOSE( "  [+] Update may have killed track - return." );
               // set the new boundary information       
               m_parametersAtBoundary.boundaryInformation(0,0,0);
               resetRecallInformation();
               return 0;              
          } else if (m_boundaryVolume && nextParameters && !m_boundaryVolume->inside(nextParameters->position()) ){
              ATH_MSG_VERBOSE( "  [+] Parameter outside the given boundary/world stopping loop.");
               // set the new boundary information       
               m_parametersAtBoundary.boundaryInformation(0,0,0);
               resetRecallInformation();
               return 0;              
          }
          //-------------------------------------------------------------------------------
          if (nextParameters)
              ATH_MSG_VERBOSE( "  [+] Entry layer successfully hit - at " << positionOutput(nextParameters->position()));
          //-------------------------------------------------------------------------------
          // check whether it worked or not
          nextParameters = nextParameters ? nextParameters : (&parm);
       }
    }

    // Step [2] Associate the starting point to the layer ------------------------------------------
    // if an action has been taken, the nextParameters are not identical with the provided parameters anymore
    navParameters = ( nextParameters == (&parm) ) ? navParameters : nextParameters;
    // only associate the layer if the  destination layer is not the assigned reference
    if (destinationLayer != assLayerReference || toBoundary){
      // get the starting layer for the layer - layer loop
      associatedLayer = assLayer ? assLayer : tvol.associatedLayer(navParameters->position());
      // ignore closest material layer if it is equal to the initially given layer (has been handled by the post update )
      associatedLayer = (associatedLayer && associatedLayer == assLayerReference ) ?
        associatedLayer->nextLayer(navParameters->position(), dir*rScalor*navParameters->momentum().normalized()) : associatedLayer; 
    }
     
    if (associatedLayer)
    ATH_MSG_VERBOSE("  [+] Associated layer at start    with " << layerRZoutput(*associatedLayer) );  
   
    // the layer to layer step and the final destination layer step can be done
    if (destinationLayer || toBoundary){
       // the layer to layer step only makes sense here
       if (associatedLayer && associatedLayer != destinationLayer ){
       // screen output
         ATH_MSG_VERBOSE("  [+] First layer for layer2layer  with " << layerRZoutput(*associatedLayer) ); 
  
         fallbackParameters = nextParameters;
         // now do the loop from the associatedLayer to one before the destinationLayer
         nextParameters = extrapolateFromLayerToLayer(prop, 
                                                     *nextParameters, 
                                                     tvol,
                                                     associatedLayer,
                                                     destinationLayer,
                                                     navParameters,
                                                     dir,
                                                     bcheck,
                                                     particle,
                                                     matupmode);
         // kill the track when the update --------------------------------------------- 
         if (m_stopWithUpdateZero && !nextParameters) {
              ATH_MSG_VERBOSE( "  [+] Update may have killed track - return." );
              // set the new boundary information    
              m_parametersAtBoundary.boundaryInformation(0,0,0);
              resetRecallInformation();
              return 0;              
          } else if (m_boundaryVolume && nextParameters && !m_boundaryVolume->inside(nextParameters->position()) ){
              ATH_MSG_VERBOSE( "  [+] Parameter outside the given boundary/world stopping loop.");
              // set the new boundary information       
              m_parametersAtBoundary.boundaryInformation(0,0,0);
              resetRecallInformation();
              return 0;              
          }
         // the fallback if only one step was done - solve cleaner
         nextParameters = nextParameters ? nextParameters : fallbackParameters;      
      }
      // Case Ia: To Destination after LayerToLayer sequence
      if (!toBoundary){
         // the final extrapolation to the destinationLayer
         nextParameters = extrapolateToDestinationLayer(prop,
                                                        *nextParameters,
                                                        *m_destinationSurface,
                                                        *destinationLayer,
                                                        tvol,
                                                        assLayerReference,
                                                        dir,
                                                        bcheck,
                                                        particle,
                                                        matupmode);
         
         // set the recallInformation <- everything went fine
         setRecallInformation(*m_destinationSurface,*destinationLayer,tvol);
         // done
         return nextParameters;   
      }
   // ----------------------------------------------------------------------------------------------------------
   // Case Ib: To Destination directly since no destination layer has been found
   } else if (!toBoundary){
     //nextParameters = prop.propagate(*nextParameters,*m_destinationSurface,dir,bcheck,tvol,particle);
      nextParameters = prop.propagate(*nextParameters,*m_destinationSurface,dir,bcheck,m_fieldProperties,particle);
     // job done: cleanup and go home
     // reset the recallInformation
     resetRecallInformation();
     // return the directly extrapolated ones
     return nextParameters;
   }

   // the reset to the initial in case the extrapolationFromLayerToLayer
   nextParameters = nextParameters ? nextParameters : (&parm);
      
   // start the search with the simplest possible propagator  
   unsigned int navprop = 0;

   const Trk::TrackParameters* bParameters = 0;
      
   if (m_configurationLevel < 10){    
       // loop over propagators to do the search
       while (navprop <= m_configurationLevel ) {  
           const IPropagator* navPropagator = &(*m_propagators[navprop]);

           // we veto the navigaiton parameters for calo-volumes with calo dynamic
           bool vetoNavParameters = false; //(tvol.geometrySignature() == Trk::Calo && m_doCaloDynamic);
           // the next Parameters are usually better, because they're closer to the boundary
           //  --- in the initial volume (assLayerReference!=0), the parm are good if no action taken
           if (nextParameters != (&parm) || assLayerReference)
               navParameters = nextParameters;
           else 
               navParameters = (m_parametersAtBoundary.navParameters && !vetoNavParameters) ? 
                 m_parametersAtBoundary.navParameters : nextParameters;
                 
           ATH_MSG_VERBOSE( "  [+] Starting next boundary search from " << positionOutput(navParameters->position()));
         ATH_MSG_VERBOSE( "  [+] Starting next boundary search with " << momentumOutput(navParameters->momentum()));
                 
           // get the new navigaiton cell from the Navigator
           Trk::NavigationCell nextNavCell = m_navigator->nextTrackingVolume(*navPropagator, *navParameters, dir, tvol);
           nextVolume    = nextNavCell.nextVolume;
           navParameters = nextNavCell.parametersOnBoundary;
           bParameters   = nextNavCell.parametersOnBoundary;
           // set the new exit Cell
           exitFace      = nextNavCell.exitFace;
           navprop++;
          if (nextVolume) break;  
          if (navprop <= m_configurationLevel && navParameters != &parm)
            throwIntoGarbageBin(navParameters);
          } 
   } else {
           Trk::NavigationCell nextNavCell = m_navigator->nextTrackingVolume(prop, *navParameters, dir, tvol);
           nextVolume = nextNavCell.nextVolume;
           navParameters = nextNavCell.parametersOnBoundary;
           bParameters   = nextNavCell.parametersOnBoundary;
           // set the new exit Cell
           exitFace      = nextNavCell.exitFace;
   }
    
       
   if (!nextVolume && !m_parametersOnDetElements){
       ATH_MSG_DEBUG( "  [-] Cannot find nextVolume of TrackingVolume:   "<< tvol.volumeName() );
       if (navParameters)
           ATH_MSG_VERBOSE( "    Starting Parameters : " << navParameters);
       else 
           ATH_MSG_VERBOSE( "    Starting Parameters not defined.");
       // reset the recall information as it is invalid
       resetRecallInformation();
   }
   
   // the navigation parameters need to go into the garbage bin
   if (navParameters != (&parm))
      throwIntoGarbageBin( navParameters );

   
   if (bParameters && bParameters->associatedSurface().materialLayer() ){
       ATH_MSG_VERBOSE(" [+] parameters on BoundarySurface with material.");
       if (m_includeMaterialEffects){
           const IMaterialEffectsUpdator* currentUpdator = m_subUpdators[tvol.geometrySignature()];
           bParameters =  currentUpdator ? currentUpdator->update(bParameters, 
                                                                  *(bParameters->associatedSurface().materialLayer()), 
                                                                  dir,
                                                                  particle,
                                                                  matupmode) : bParameters;
           // collect the material                                                          
           if (bParameters && (m_matstates || m_materialEffectsOnTrackValidation) )
               addMaterialEffectsOnTrack(prop,*bParameters,*(bParameters->associatedSurface().materialLayer()),tvol,dir,particle);                                                                    
                                                                  
           // the bParameters need to be put into the gargabe bin if they differ from the navParmaeters
           if (bParameters != navParameters){
               throwIntoGarbageBin(bParameters);
               navParameters = bParameters;
           }
       }
   }
    
   // set the new boundary information       
   m_parametersAtBoundary.boundaryInformation(nextVolume,
                                              nextParameters,
                                              navParameters,
                                              exitFace);

   // return the navParameters
  return navParameters;     
}


// ----------------------- The private Layer extrapolation methods -------------------------------------------------

const Trk::TrackParameters* Trk::Extrapolator::extrapolateFromLayerToLayer(
                                                       const IPropagator& prop,
                                                       const TrackParameters& parm,
                                                       const TrackingVolume& tvol,
                                                       const Layer* startLayer,
                                                       const Layer* destinationLayer,
                                                       const TrackParameters* navParm,
                                                       PropDirection dir,
                                                       BoundaryCheck bcheck,
                                                       ParticleHypothesis particle,
                                                       MaterialUpdateMode matupmode) const 
{
   // method sequence output ---------------------------------
   ++m_methodSequence;
   ATH_MSG_DEBUG( "S-["<< m_methodSequence << "] extrapolateFromLayerToLayer(...) in '" << tvol.volumeName() << "'. " );

  // initialize the loop
  const Trk::Layer*             nextLayer = startLayer;
  // avoiding straight loops and oszillations
  const Trk::Layer*             lastLayer = 0;
  const Trk::Layer*             previousLayer = 0;
  // pars & fallback
  const Trk::TrackParameters*   nextParameters = (&parm);
  const Trk::TrackParameters*   navParameters  = navParm;
  const Trk::TrackParameters*   fallbackParameters = 0;
  // avoid initial perpendicular check if:
  // -  navParameters and nextParameters have different perpendicular direction (resolved in navigaiton)
  bool perpCheck =  radialDirection(*nextParameters,dir) * radialDirection(*navParameters,dir) > 0;
  
  // break conditions: --------- handeled by layerAttempts
  unsigned int failedAttempts  = 0;

  // get the max attempts from the volume : only for Fatras - for reco take the maximum number
  Trk::BoundarySurfaceFace lastExitFace = m_parametersAtBoundary.exitFace;
  unsigned int layersInVolume = tvol.confinedLayers() ? tvol.confinedLayers()->arrayObjects().size() : 0;
  unsigned int maxAttempts    =  (!m_parametersOnDetElements && !m_extendedLayerSearch) ? 
                 tvol.layerAttempts( lastExitFace ) : int(layersInVolume*0.5);
                 
  // set the maximal attempts to at least m_initialLayerAttempts
  maxAttempts = (maxAttempts < m_initialLayerAttempts) ? m_initialLayerAttempts : maxAttempts;

  ATH_MSG_VERBOSE( "  [+] Maximum number of failed layer attempts: " << maxAttempts );

  // conditions for the loop are :
  //    - nextLayer exists
  //    - nextLayer is not the previous one, Exception : inbound cosmics
  //    - nextLayer is not the last layer, Exception: formerly inbound cosmics
  //    - nextLayer is not the destination layer
  //    - the number of attempts does not exceed a set maximum

  while (nextLayer && 
         nextLayer != previousLayer &&
       nextLayer != lastLayer &&
         nextLayer != destinationLayer && 
       failedAttempts < maxAttempts){
     // screen output
           ATH_MSG_VERBOSE( "  [+] Found next "
           << (  (nextLayer->layerMaterialProperties() ? "material layer  - with " : "navigation layer  with ") ) 
           <<  layerRZoutput(*nextLayer) );

     // skip the navigation layers
     if (nextLayer->layerMaterialProperties() || (m_parametersOnDetElements && nextLayer->surfaceArray()) ){
        // prepare for fallback
        fallbackParameters = nextParameters; 
        // the next step - do not delete the parameters (garbage collection done by method)
        nextParameters = extrapolateToIntermediateLayer(prop,
                                                        *nextParameters,
                                                        *nextLayer,
                                                        tvol,
                                                        dir,
                                                        bcheck,
                                                        particle,
                                                        matupmode,
                                                        perpCheck);
        // previous and last layer setting for loop and oscillation protection
        previousLayer = lastLayer;
        lastLayer = nextLayer;              
        // the breaking condition -----------------------------------------------------------
        if (!nextParameters){
           ++failedAttempts;
           ++m_layerSwitched; // record for statistics output
           // reset until break condition is fullfilled
           nextParameters = fallbackParameters;           
        } else  if (intptr_t(nextParameters)==1) {
             ATH_MSG_VERBOSE( "  [+] Material update killed the track parameters - return 0" );
             // kill the track - Fatras case
             return 0;
        } else if (m_boundaryVolume && !m_boundaryVolume->inside(nextParameters->position())){ 
             ATH_MSG_VERBOSE( "  [+] Parameter outside the given boundary/world stopping loop.");
             // set the new boundary information       
             return nextParameters;
        } else { // reset the failed attempts
             ATH_MSG_VERBOSE( "  [+] Intersection successful: allowing for " << maxAttempts << " more failed attempt." );
             failedAttempts = 0;   
             // but a hit sets the max attempts to m_successiveLayerAttempts => navigation machine started !
             //maxAttempts = m_successiveLayerAttempts;   
             // new navParameters are nextParameters
             navParameters = nextParameters;
             // enforce the perpendicular check
             perpCheck = true;
        }
      }
     
     // cache of radiatl direction and next layer request
     nextLayer = nextLayer->nextLayer(navParameters->position(), dir*navParameters->momentum().normalized());

     // screen output
     if (!nextLayer)
         ATH_MSG_VERBOSE( "  [+] No next Layer provided by the previous layer -> stop of layer2layer" );
  }
  if (failedAttempts >= maxAttempts)
      ATH_MSG_VERBOSE( "  [-] Maximum number of Attempts triggered in '"<< tvol.volumeName() << "'.");
        
   // return the result
   return nextParameters;

}


const Trk::TrackParameters* Trk::Extrapolator::extrapolateToDestinationLayer(
                                                         const IPropagator& prop,
                                                         const TrackParameters& parm,
                                                         const Surface& sf,
                                                         const Layer& lay,
                                                         const TrackingVolume& tvol,
                                                         const Layer* startLayer,
                                                         PropDirection dir,
                                                         BoundaryCheck bcheck,
                                                         ParticleHypothesis particle,
                                                         MaterialUpdateMode matupmode) const
{  
   // method sequence output ---------------------------------
   ATH_MSG_DEBUG( "S-[" << ++m_methodSequence << "] extrapolateToDestinationLayer(...) in '" << tvol.volumeName() << "'." );
   // start is destination layer -> on layer navigation, take care   
   bool startIsDestLayer = startLayer==(&lay);

   Trk::TransportJacobian *jac=0;
   // get the Parameters on the destination surface   
   //const Trk::TrackParameters* destParameters = m_jacs ? prop.propagate(parm, sf, dir, bcheck, tvol, jac, particle) 
   //                                                    : prop.propagate(parm, sf, dir, bcheck, tvol, particle);
   double pathLimit = -1.;
   const Trk::TrackParameters* destParameters = m_jacs ? prop.propagate(parm, sf, dir, bcheck, MagneticFieldProperties(), jac, pathLimit, particle) 
     : prop.propagate(parm, sf, dir, bcheck, MagneticFieldProperties(), particle);

   // fallback to anyDirection 
   //destParameters = destParameters ?  destParameters : ( m_jacs ? prop.propagate(parm, sf, Trk::anyDirection, bcheck, tvol, jac, particle) : prop.propagate(parm, sf, Trk::anyDirection, bcheck, tvol, particle));
   destParameters = destParameters ?  destParameters : ( m_jacs ? prop.propagate(parm, sf, Trk::anyDirection, bcheck,  MagneticFieldProperties(), jac, pathLimit, particle) : prop.propagate(parm, sf, Trk::anyDirection, bcheck,  m_fieldProperties, particle));
     
   // return the pre-updated ones
   const IMaterialEffectsUpdator* currentUpdator = subMaterialEffectsUpdator(tvol);

   const Trk::TrackParameters* preUpdatedParameters = destParameters;
   if (currentUpdator && destParameters && !startIsDestLayer) {
     preUpdatedParameters = currentUpdator->preUpdate(destParameters, lay, dir, particle, matupmode);
     if (preUpdatedParameters && preUpdatedParameters != destParameters ) {
       throwIntoGarbageBin(destParameters);
       //throwIntoGarbageBin(preUpdatedParameters);            // ST : memory leak here ???
     }
   }

   // collect the material : either for extrapolateM or for the valdiation
   if ( (m_matstates || m_materialEffectsOnTrackValidation) && preUpdatedParameters && currentUpdator && !startIsDestLayer && lay.preUpdateMaterialFactor(*destParameters, dir)>=0.01 )
     addMaterialEffectsOnTrack(prop,*preUpdatedParameters,lay,tvol,dir,particle);  
   
   // check if 0 was returned from the updater: if so, delete the destParameters 
   if (!preUpdatedParameters && preUpdatedParameters != destParameters && destParameters != &parm) 
     throwIntoGarbageBin(destParameters);
   
   // call the overlap search on the destination parameters - we are at the surface already
  if (m_parametersOnDetElements
        && preUpdatedParameters
        && lay.surfaceArray() 
      && m_subSurfaceLevel)
  {
        ATH_MSG_VERBOSE( "  [o] Calling overlapSearch() on destination layer.");
        // start is destination layer
    overlapSearch(prop,parm,*preUpdatedParameters,lay,tvol,dir,bcheck,particle,startIsDestLayer);   
   }

   if (preUpdatedParameters)
       ATH_MSG_VERBOSE("  [+] Destination surface successfully hit.");

    // return the pre-updated parameters (can be 0 though)
    return preUpdatedParameters;    
}

const Trk::TrackParameters* Trk::Extrapolator::extrapolateToIntermediateLayer(
                                                         const IPropagator& prop,
                                                         const TrackParameters& parm,
                                                         const Layer& lay,
                                                         const TrackingVolume& tvol,
                                                         PropDirection dir,
                                                         BoundaryCheck bcheck,
                                                         ParticleHypothesis particle,
                                                         MaterialUpdateMode matupmode,
                                                         bool doPerpCheck) const
{  
   // method sequence output ---------------------------------
   ++m_methodSequence;
   ATH_MSG_DEBUG( "S-[" << m_methodSequence << "] to extrapolateToIntermediateLayer(...) layer " << lay.layerIndex() << " in '"
       << tvol.volumeName() << "'." );

   // chose the current updator
   const IMaterialEffectsUpdator* currentUpdator = subMaterialEffectsUpdator(tvol);   
   // then go onto the Layer
   const Trk::TrackParameters* parsOnLayer = 0;
   
   if(m_checkForCompundLayers) {
     const Trk::CompoundLayer* cl = dynamic_cast<const Trk::CompoundLayer*>(&lay);
     if(cl) {
       // try each surface in turn
       const std::vector<const Surface*> cs = cl->constituentSurfaces();
       for(unsigned int i = 0; i < cs.size(); ++i) {
   //parsOnLayer = prop.propagate(parm,*(cs[i]),dir,true,tvol,particle);
   parsOnLayer = prop.propagate(parm,*(cs[i]),dir,true, m_fieldProperties,particle);
   if(parsOnLayer) {
     break;
   }
       }
     } else
       //parsOnLayer = prop.propagate(parm,lay.surfaceRepresentation(),dir,true,tvol,particle);
       parsOnLayer = prop.propagate(parm,lay.surfaceRepresentation(),dir,true, m_fieldProperties,particle);
   } else
     //parsOnLayer = prop.propagate(parm,lay.surfaceRepresentation(),dir,true,tvol,particle);
     parsOnLayer = prop.propagate(parm,lay.surfaceRepresentation(),dir,true, m_fieldProperties,particle);
   
   // return if there is nothing to do
   if (!parsOnLayer) return 0;   
   // the layer has been intersected ----------------------------------------------------------------------------
   // throw them into the garbage bin immediately -------------------------------------------------------------     
   throwIntoGarbageBin(parsOnLayer); 
   // check for radial direction change ---------------------------------------------------------------------
   int rDirection    = radialDirection(parm,dir);
   int newrDirection = radialDirection(*parsOnLayer,dir);
   if (newrDirection != rDirection && doPerpCheck){
      // it is unfortunate that the cancelling could invalidate the material collection 
      ATH_MSG_DEBUG("  [!] Perpendicular direction of track has changed -- checking");
      // reset the nextParameters if the radial change is not allowed 
      //  resetting is ok - since the parameters are in the garbage bin already
      if (!radialDirectionCheck(prop, parm, *parsOnLayer,tvol,dir,particle)){
          ATH_MSG_DEBUG("  [+] Perpendicular direction check cancelled this layer intersection.");
          return 0;
      }        
   }     
   // ---------------------------------------------------------------------------------------------------------
   ATH_MSG_VERBOSE( "  [+] Layer intersection successful  at " << positionOutput(parsOnLayer->position()));
   ATH_MSG_VERBOSE( "  [+] Layer intersection successful  with " << momentumOutput(parsOnLayer->momentum()));

   // Fatras mode ----------------------------------------------------------------------------------------------
     if (m_parametersOnDetElements 
         && lay.surfaceArray()
         && m_subSurfaceLevel){
      // ceck the parameters size before the search 
        size_t sizeBeforeSearch = m_parametersOnDetElements->size();
        // perform the overlap Search on this layer
        ATH_MSG_VERBOSE( "  [o] Calling overlapSearch() on intermediate layer."); 
        overlapSearch(prop,parm,*parsOnLayer,lay,tvol,dir,bcheck,particle);       
        size_t sizeAfterSearch = m_parametersOnDetElements->size();
        // the Fatras mode was successful -> postUpdate and garbage collection
        int lastElement = (int)m_parametersOnDetElements->size()-1;
        // we have created hits in the vector
        if (lastElement >= 0 && sizeBeforeSearch < sizeAfterSearch){
          // get the last element
          // it's ok to reassign parOnLayer as the pointer to the first one is in the garbage bin already          
          // get the latest Fatras hit to start from this one     
          parsOnLayer = (*m_parametersOnDetElements)[lastElement];
          ATH_MSG_DEBUG( "  [+] Detector element & overlapSearch successful,"
                      << " call update on last parameter on this layer." ); 
       }
     } // ------------------------------------------------- Fatras mode off -----------------------------------
    
     // return the full-updated ones - may create a new object
     if ( lay.layerMaterialProperties() && currentUpdator ) {
       const Trk::TrackParameters* upLayPars =   currentUpdator->update(parsOnLayer, lay, dir, particle,matupmode) ;
       if (upLayPars && upLayPars != parsOnLayer) throwIntoGarbageBin(upLayPars);
       parsOnLayer = upLayPars; 
     }
     // there are layers that have a surfaceArray but no material properties
     if (parsOnLayer 
         && lay.layerMaterialProperties() 
         && (m_matstates || m_materialEffectsOnTrackValidation) )
       addMaterialEffectsOnTrack(prop,*parsOnLayer,lay,tvol,dir,particle);
     // kill the track if the update killed the track -----------------------------------------------------------------------
     if (!parsOnLayer && m_stopWithUpdateZero) 
         return (TrackParameters*)1; // the indicator to kill the loopfrom material update ----------------------------------
   // ------------ the return of the parsOnLayer --- they're in the garbage bin already
   return parsOnLayer;
}


void Trk::Extrapolator::overlapSearch(const IPropagator& prop,
                                      const TrackParameters& parm,
                                      const TrackParameters& parsOnLayer,
                                      const Layer& lay,
                                      const TrackingVolume& /*tvol*/,
                                      PropDirection dir,
                                      BoundaryCheck bcheck, //bcheck
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
         // retrive compatible subsurfaces
   std::vector<Trk::SurfaceIntersection> cSurfaces;
   
   size_t ncSurfaces = lay.compatibleSurfaces(cSurfaces,*detParameters,Trk::anyDirection,bcheck,false);
            
   // import from StaticEngine.icc
   if (ncSurfaces){
     ATH_MSG_VERBOSE( "found " <<  ncSurfaces << " candidate sensitive surfaces to test."); 
     // now loop over the surfaces:
     // the surfaces will be sorted @TODO integrate pathLength propagation into this
     for (auto& csf : cSurfaces ) {
       // propagate to the compatible surface, return types are (pathLimit failure is excluded by Trk::anyDirection for the moment):
       const Trk::TrackParameters* overlapParameters = prop.propagate(parm,
                     *(csf.object),
                                                                           Trk::anyDirection,
                     true,
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
     reorderDetParametersOnLayer=true;
     // push back into the temporary vector
     detParametersOnLayer.push_back(overlapParameters);        
         } else { // the parameters have been cancelled by start/end surface
     // no hit -> fill into the garbage bin 
     ATH_MSG_VERBOSE("  [-] Detector surface hit cancelled through start/end surface check." );       
     throwIntoGarbageBin(overlapParameters);
         }
       }
     } // loop over test surfaces done
   } // there are compatible surfaces  
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
       for ( ; parsOnLayerIter != parsOnLayerIterEnd; ++parsOnLayerIter)
        m_parametersOnDetElements->push_back(*parsOnLayerIter);


}




unsigned int Trk::Extrapolator::propagatorType(const Trk::TrackingVolume& tvol) const
{
  return tvol.geometrySignature();  
}

// ----------------------- The Initialization -------------------------------------------------
Trk::PropDirection Trk::Extrapolator::initializeNavigation(
                                             const IPropagator&      prop,
                                             const TrackParameters&  parm,
                                             const Surface&          sf,
                                             PropDirection           dir,
                                             ParticleHypothesis      particle,
                                             const TrackParameters*& refParameters,
                                             const Layer*&           associatedLayer,
                                             const TrackingVolume*&  associatedVolume,
                                             const TrackingVolume*&  destVolume) const 
{
   // output for initializeNavigation should be an eye-catcher
   if (!m_destinationSurface) {
      ATH_MSG_DEBUG( "  [I] initializeNaviagtion() ------------------------------------------------- " );
      m_methodSequence = 0;
   } else
      ATH_MSG_DEBUG( "  [I] (re)initializeNaviagtion() --------------------------------------------- " );   
            
   Trk::PropDirection navigationDirection = dir;    
   // only for the initial and not for the redoNavigation - give back the navigation direction
   if (!m_destinationSurface){

     ATH_MSG_VERBOSE( "  [I] Starting with Start Layer/Volume search: ------------------------------" );   
     ATH_MSG_VERBOSE( "  [I] Destination surface : " << sf );
     emptyGarbageBin();
     // clear the garbage collection
     m_garbageBin.clear();
     // reset the boundary information
     m_parametersAtBoundary.resetBoundaryInformation();
     // and set the destination surface
     m_destinationSurface = (&sf);
     // prepare for screen output
     const char* startSearchType = "association";
     
     // ---------------------------------- ASSOCIATED VOLUME ----------------------------------     
     // 1 - TRY the association method
     const Trk::Surface* associatedSurface = &parm.associatedSurface();
     associatedLayer = (associatedSurface) ? associatedSurface->associatedLayer() : associatedLayer;
     associatedVolume = associatedLayer ? associatedLayer->enclosingTrackingVolume() : associatedVolume;         
     // 2 - TRY the recall method -> only if association method didn't work
     // only if associated detector element exists to protect against dynamic surfaces
     if (!associatedVolume && 
          associatedSurface &&
          associatedSurface == m_recallSurface && 
          associatedSurface->associatedDetectorElement() ) {
       // statistics output
       ++m_startThroughRecall;
       associatedVolume      = m_recallTrackingVolume;
       associatedLayer       = m_recallLayer;
       // change the association type
       startSearchType = "recall";
       
     } else if (!associatedVolume){       
       // 3 - GLOBAL SEARCH METHOD
       ++m_startThroughGlobalSearch;
       // non-perigee surface
       resetRecallInformation();    
       associatedVolume       = m_navigator->volume(parm.position());
       associatedLayer        = associatedVolume->associatedLayer(parm.position());
       
       // change the association type
       startSearchType = "global search";

       // ---------------------------------- ASSOCIATED STATIC VOLUME --------------------------------------
       // this is not necessary for ( association & recall )
       const Trk::TrackingVolume* lowestStaticVol = 
             m_navigator->trackingGeometry()->lowestStaticTrackingVolume(parm.position()); 
     
       if (lowestStaticVol && lowestStaticVol != associatedVolume ) associatedVolume = lowestStaticVol;
       // --------------------------------------------------------------------------------------------------
 
     } else
       ++m_startThroughAssociation;

     // verify if not exit point from associated volume
     if (associatedVolume && navigationDirection != Trk::anyDirection) {
       const Trk::TrackingVolume* nextAssVol = 0;
       if (m_navigator->atVolumeBoundary(&parm,
                                         associatedVolume,
                                         dir,nextAssVol,
                                         m_tolerance) && nextAssVol != associatedVolume ) {
   if (nextAssVol) associatedVolume = nextAssVol;
         else ATH_MSG_WARNING("  [X] Navigation break occurs in volume " << associatedVolume->volumeName() << " no action taken" );
       } 
     }
    // ---------------- anyDirection given : navigation direction has to be estimated ---------
     if (navigationDirection == Trk::anyDirection) {
       
       ATH_MSG_VERBOSE( "  [I] 'AnyDirection' has been chosen: approaching direction must be determined." );   
     
       //refParameters = prop.propagateParameters(parm,sf,dir,false,*associatedVolume);
       refParameters = prop.propagateParameters(parm,sf,dir,false,m_fieldProperties,particle,false,associatedVolume);
       // chose on projective method
       if (refParameters){
         // check the direction on basis of a vector projection
         Amg::Vector3D surfaceDir(refParameters->position()-parm.position());
         if (surfaceDir.dot(parm.momentum()) > 0.) 
             navigationDirection = Trk::alongMomentum;
         else 
             navigationDirection = Trk::oppositeMomentum; 
         
         // really verbose statement, but needed for debugging 
         ATH_MSG_VERBOSE( "  [+] Approaching direction determined as: "
                <<  ( (navigationDirection < 0 ) ?  "oppositeMomentum." : "alongMomentum" ) );            
       } else 
             ATH_MSG_VERBOSE( "  [+] Approaching direction could not be determined, they remain: anyDirection." );
     }    
      ATH_MSG_VERBOSE( "  [I] Starting Information gathered through : " << startSearchType << "." );
   }
   // ----------------------------------------------------------------------------------------

   // ---------------------------------- DESTINATION VOLUME ----------------------------------  
   // only do it if sf is not the reference Surface   
   ATH_MSG_VERBOSE( "  [I] Starting with destination Volume search: -----------------------------" );   

   if ( (&sf) != (s_referenceSurface) ){ 
     // (1) - TRY the association method
     destVolume = (sf.associatedLayer()) ? sf.associatedLayer()->enclosingTrackingVolume() : 0;
     // for the summary output
     std::string destinationSearchType = "association";
     if (destVolume) ++m_destinationThroughAssociation;
     // (2) - RECALL
     // only if associated detector element exists to protect against dynamic surfaces
     if (!destVolume && ((&sf)==m_recallSurface) && sf.associatedDetectorElement() ){       
       destVolume =  m_recallTrackingVolume;
       destinationSearchType = "recall";
       // the recal case ----------
       ++m_destinationThroughRecall;
     } else if (!destVolume) { 
       // (3) GLOBAL SEARCH
       destinationSearchType = "global search";
       ++m_destinationThroughGlobalSearch;
       // if the propagation has not been done already (for direction estimation)
       // do the global search always with a reference propagation
       if (!refParameters && associatedVolume){
         //refParameters = prop.propagateParameters(parm, sf, dir, false, *associatedVolume);
         refParameters = prop.propagateParameters(parm, sf, dir, false, m_fieldProperties, particle, false, associatedVolume);
       }
       // get the destination Volume
       if (refParameters)
          destVolume = m_navigator->volume(refParameters->position());
       // ------ the last chance : associate to the globalReferencePoint
       //std::cout << "destVolume: " << destVolume << " ref par: " << refParameters << " associatedVolume: " << associatedVolume << std::endl;
       if (!destVolume) destVolume = m_navigator->volume(sf.globalReferencePoint());     
     }
     ATH_MSG_VERBOSE( "  [I] Destination Information gathered through : " << destinationSearchType << "." );
   }       
   // screen output summary ----------------------------------------------------------------------------------
   if (msgLvl(MSG::VERBOSE)){
       ATH_MSG_VERBOSE( "  [+] Association Volume search ...... " << ( associatedVolume ? "ok." : "failed." ) );
       ATH_MSG_VERBOSE( "  [+] Association Layer  search ...... " << ( associatedLayer ? "ok." : "failed." )  );
       ATH_MSG_VERBOSE( "  [+] Destinaiton Volume search ...... " << ( destVolume ? "ok." : "failed." )  );
       // give a line of output when start volume is destination volume
       if (destVolume == associatedVolume)
         ATH_MSG_VERBOSE( "  [+] Start volume is destination volume." ); 
       std::string navDirString = ( (navigationDirection < 0) ? "oppositeMomentum" : (navigationDirection > 0) ? "alongMomentum" : "undefined" );
       ATH_MSG_VERBOSE( "  [+] NavigationDirection is         : " << navDirString );
       ATH_MSG_VERBOSE( "  [I] initializeNaviagtion() end --------------------------------------------- "     );   
   }  
        
   //---------------------------------------------------------------------------------------------------
   return navigationDirection;
}

int Trk::Extrapolator::radialDirection(const Trk::TrackParameters& pars, PropDirection dir) const
{ 

    // safe inbound/outbound estimation
    double prePositionR = pars.position().perp();
    return (prePositionR > (pars.position() + dir*0.5*prePositionR*pars.momentum().normalized()).perp()) ? -1 : 1;
}
    
bool Trk::Extrapolator::radialDirectionCheck(const IPropagator& prop,
                                             const TrackParameters& startParm,
                                             const TrackParameters& parsOnLayer,
                                             const TrackingVolume& tvol,
                                             PropDirection dir,
                                             ParticleHypothesis particle) const
{
  const Amg::Vector3D& startPosition = startParm.position();
  const Amg::Vector3D& onLayerPosition = parsOnLayer.position();
  
  // the 3D distance to the layer intersection
  double distToLayer = (startPosition-onLayerPosition).mag();
  // get the innermost contained surface for crosscheck
  const std::vector< SharedObject<const BoundarySurface<TrackingVolume> > >& boundarySurfaces = tvol.boundarySurfaces();
  // only for tubes the crossing makes sense to check for validity
  if (boundarySurfaces.size()==4) {
    // propagate to the inside surface and compare the distance:
    // it can be either the next layer from the initial point, or the inner tube boundary surface
    const Trk::Surface& insideSurface = (boundarySurfaces[Trk::tubeInnerCover].getPtr())->surfaceRepresentation();                  
    //const Trk::TrackParameters* parsOnInsideSurface = prop.propagateParameters(startParm,insideSurface,dir,true,tvol,particle);
    const Trk::TrackParameters* parsOnInsideSurface = prop.propagateParameters(startParm,insideSurface,dir,true,m_fieldProperties,particle);
    double distToInsideSurface = parsOnInsideSurface ? (startPosition-(parsOnInsideSurface->position())).mag() : 10e10;
    
    ATH_MSG_VERBOSE( "  [+] Radial direction check start - at " << positionOutput(startPosition) );
    ATH_MSG_VERBOSE( "  [+] Radial direction check layer - at " << positionOutput(onLayerPosition) );
    if (parsOnInsideSurface)
       ATH_MSG_VERBOSE( "  [+] Radial direction check inner - at " << positionOutput(parsOnInsideSurface->position()) );
    
    // memory cleanup (no garbage bin, this is faster)
    delete parsOnInsideSurface;
    ATH_MSG_VERBOSE( "  [+] Check radial direction: distance layer / boundary = " << distToLayer << " / " << distToInsideSurface);            
    // the intersection with the original layer is valid if it is before the inside surface
    return (distToLayer < distToInsideSurface) ? true : false;
  }    
  return true;
}    

std::string Trk::Extrapolator::layerRZoutput(const Trk::Layer& lay) const
{
   std::stringstream outStream;
   outStream <<  "[r,z] = [ " << lay.surfaceRepresentation().bounds().r() 
             << ", " << lay.surfaceRepresentation().center().z() << " ] - Index ";
   outStream << lay.layerIndex().value();                
   return outStream.str();
}

std::string Trk::Extrapolator::positionOutput(const Amg::Vector3D& pos) const
{
   std::stringstream outStream;
   if (m_printRzOutput) 
        outStream <<  "[r,phi,z] = [ " << pos.perp() << ", " << pos.phi() << ", " << pos.z() << " ]";
   else 
        outStream <<  "[xyz] = [ " << pos.x() << ", " << pos.y() << ", " << pos.z() << " ]";
   return outStream.str();
}

std::string Trk::Extrapolator::momentumOutput(const Amg::Vector3D& mom) const
{
   std::stringstream outStream;
   outStream <<  "[eta,phi] = [ " << mom.eta() << ", " << mom.phi() << " ]";
   return outStream.str();
}


void Trk::Extrapolator::emptyGarbageBin() const
{   
   // reset the boundary information
   m_parametersAtBoundary.resetBoundaryInformation();
   // empty the garbage  
   std::map<const Trk::TrackParameters*, bool>::iterator garbageIter  = m_garbageBin.begin();
   std::map<const Trk::TrackParameters*, bool>::iterator garbageEnd   = m_garbageBin.end();

   for ( ; garbageIter != garbageEnd; ++garbageIter ){
     delete (garbageIter->first);
   }

   m_garbageBin.clear();
   // reset the method sequence for output information
   m_methodSequence = 0;
   m_parametersAtBoundary.exitFace = undefinedFace;
}

void Trk::Extrapolator::emptyGarbageBin(const Trk::TrackParameters* trPar) const
{   
   // empty the garbage  
   std::map<const Trk::TrackParameters*, bool>::iterator garbageIter  = m_garbageBin.begin();
   std::map<const Trk::TrackParameters*, bool>::iterator garbageEnd   = m_garbageBin.end();

   
   bool throwCurrent = false;  
   bool throwLast    = false;
   bool throwBounds  = false;
   for ( ; garbageIter != garbageEnd; ++garbageIter ){
     if (garbageIter->first && garbageIter->first!=trPar && garbageIter->first != m_lastValidParameters &&
   garbageIter->first!=m_parametersAtBoundary.nextParameters) delete (garbageIter->first);
     if (garbageIter->first && garbageIter->first==trPar) throwCurrent = true; 
     if (garbageIter->first && garbageIter->first==m_lastValidParameters) throwLast = true; 
     if (garbageIter->first && garbageIter->first==m_parametersAtBoundary.nextParameters) throwBounds = true; 
   }

   m_garbageBin.clear();
   if (throwCurrent) throwIntoGarbageBin(trPar);
   if (throwLast) throwIntoGarbageBin(m_lastValidParameters);
   if (throwBounds) throwIntoGarbageBin(m_parametersAtBoundary.nextParameters);
}

void Trk::Extrapolator::addMaterialEffectsOnTrack(const Trk::IPropagator& prop,
                                                  const Trk::TrackParameters& parms,
                                                  const Trk::Layer& lay,
                                                  const Trk::TrackingVolume& /*tvol*/,
                                                  Trk::PropDirection propDir,
                                                  Trk::ParticleHypothesis particle) const{

  ATH_MSG_VERBOSE( "  [+] addMaterialEffectsOnTrack()  - at " << positionOutput(parms.position()) );
  // statistics counter Fw/Bw
  if (propDir==Trk::alongMomentum) ++m_meotSearchCallsFw;
  else ++m_meotSearchCallsBw;
  // preparation for the material effects on track
  const Trk::MaterialProperties* materialProperties  = 0;
  double pathCorrection                              = 0.;  
  const Trk::TrackParameters* parsOnLayer            = 0;
  // make sure the parameters are on surface
  if (parms.associatedSurface() != lay.surfaceRepresentation()) {
    if(m_checkForCompundLayers) {
      const Trk::CompoundLayer* cl = dynamic_cast<const Trk::CompoundLayer*>(&lay);
      if(cl) {
        // try each surface in turn
        const std::vector<const Surface*> cs = cl->constituentSurfaces();
        for(unsigned int i = 0; i < cs.size(); ++i) {
            //parsOnLayer = prop.propagateParameters(parms,*(cs[i]),Trk::anyDirection,false,tvol);
            parsOnLayer = prop.propagateParameters(parms,*(cs[i]),Trk::anyDirection,false,m_fieldProperties);
            if(parsOnLayer)
                break;
        }
      } else
  //parsOnLayer = prop.propagateParameters(parms,lay.surfaceRepresentation(),Trk::anyDirection,false,tvol);
  parsOnLayer = prop.propagateParameters(parms,lay.surfaceRepresentation(),Trk::anyDirection,false,m_fieldProperties);
    } else
      //parsOnLayer = prop.propagateParameters(parms,lay.surfaceRepresentation(),Trk::anyDirection,false,tvol);
      parsOnLayer = prop.propagateParameters(parms,lay.surfaceRepresentation(),Trk::anyDirection,false,m_fieldProperties);
    // originally, the code for the enclosing if was just
    //parsOnLayer = prop.propagateParameters(parms,lay.surfaceRepresentation(),Trk::anyDirection,false,tvol);
  } else parsOnLayer = parms.clone();
  // should not really happen         
  if (!parsOnLayer) return;
  // reference material section: 
  pathCorrection = pathCorrection > 0. ? pathCorrection :
    lay.surfaceRepresentation().pathCorrection(parsOnLayer->position(),parsOnLayer->momentum());
  
  // material properties are not given by the reference material, get them from the layer
  if (!materialProperties) 
    materialProperties = lay.fullUpdateMaterialProperties(*parsOnLayer);
  
  if (!materialProperties) {
    ATH_MSG_DEBUG( "  [!] No MaterialProperties on Layer after intersection." );
    delete parsOnLayer;
    return;
  }
  // statistics
  if (propDir==Trk::alongMomentum) ++m_meotSearchSuccessfulFw;
  else ++m_meotSearchSuccessfulBw;
  // pure validation mode
  if (!m_matstates){
    if(m_extrapolationCache) {
      double tInX0 = pathCorrection*materialProperties->thicknessInX0();
      if(m_dumpCache) dumpCache(" addMaterialEffectsOnTrack");
      m_extrapolationCache->updateX0(tInX0);
      double currentQoP = parsOnLayer->parameters()[Trk::qOverP];            
      Trk::EnergyLoss* energyLoss=m_elossupdators[0]->energyLoss(*materialProperties,fabs(1./currentQoP),pathCorrection,propDir,particle);
      m_extrapolationCache->updateEloss(energyLoss->meanIoni(),energyLoss->sigmaIoni(),energyLoss->meanRad(),energyLoss->sigmaRad());
      if(m_dumpCache) dumpCache(" After");
      delete energyLoss;
    } 
    ATH_MSG_VERBOSE( "  [V] Validation mode: MaterialProperties found on this layer." );
    delete parsOnLayer;
  } else { // collection mode 
    // material properties from the layer
    double tInX0 = pathCorrection*materialProperties->thicknessInX0();
    Trk::EnergyLoss* energyLoss       = 0;
    Trk::ScatteringAngles* scatAngles = 0;
    // get the q/p for the energyLoss object
    double currentQoP = parsOnLayer->parameters()[Trk::qOverP];            
    energyLoss=m_elossupdators[0]->energyLoss(*materialProperties,fabs(1./currentQoP),pathCorrection,propDir,particle);
    // get the scattering angle
    double sigmaMS = sqrt(m_msupdators[0]->sigmaSquare(*materialProperties,fabs(1./currentQoP),pathCorrection,particle));
    scatAngles = new ScatteringAngles(0,0,sigmaMS/sin(parsOnLayer->parameters()[Trk::theta]),sigmaMS);
    Trk::MaterialEffectsOnTrack* meot = new Trk::MaterialEffectsOnTrack(tInX0,scatAngles,energyLoss,*lay.surfaceRepresentation().baseSurface());
    // push it to the material states
    m_matstates->push_back(new TrackStateOnSurface(0,parsOnLayer,0,meot));
    // update cache 
    if(m_extrapolationCache) {
      if(energyLoss->meanIoni()==0.&&tInX0>0.) ATH_MSG_WARNING( " Extrapolator: the ExtrapolationCache cannot work because the ElossUpdator is wrongly configured: switch joboption DetailedEloss on ");
      if(m_dumpCache) dumpCache(" addMaterialEffectsOnTrack");
      m_extrapolationCache->updateX0(tInX0);
      m_extrapolationCache->updateEloss(energyLoss->meanIoni(),energyLoss->sigmaIoni(),energyLoss->meanRad(),energyLoss->sigmaRad());
      if(m_dumpCache) dumpCache(" After");
    }
  }
}

void Trk::Extrapolator::dumpCache(std::string txt) const {

//        std::cout << " pointer " << m_extrapolationCache << std::endl;
//        std::cout << " X0 " << m_extrapolationCache->x0tot() << std::endl;
//        std::cout << " Eloss " <<   m_extrapolationCache->eloss() << std::endl; 

        if( m_cacheEloss!=0&&m_cacheEloss!=m_extrapolationCache->eloss() ) {
          ATH_MSG_DEBUG(" NO dumpCache: Eloss cache pointer overwritten " << m_cacheEloss << " from extrapolationCache " << m_extrapolationCache->eloss());
          return; 
        } 

        ATH_MSG_DEBUG(txt << " X0 " << m_extrapolationCache->x0tot() << " Eloss deltaE " <<   m_extrapolationCache->eloss()->deltaE()  << " Eloss sigma " << m_extrapolationCache->eloss()->sigmaDeltaE() << " meanIoni " << m_extrapolationCache->eloss()->meanIoni()  << " sigmaIoni " << m_extrapolationCache->eloss()->sigmaIoni() << " meanRad " <<  m_extrapolationCache->eloss()->meanRad() << " sigmaRad " << m_extrapolationCache->eloss()->sigmaRad());

}

bool Trk::Extrapolator::checkCache( std:: string txt ) const {

    if( m_cacheEloss!=0&&m_cacheEloss!=m_extrapolationCache->eloss() ) {
        ATH_MSG_DEBUG( txt << " PROBLEM Eloss cache pointer overwritten " << m_cacheEloss << " from extrapolationCache " << m_extrapolationCache->eloss());
        return false;
    } else return true;
 
}

const std::vector< std::pair< const Trk::TrackParameters*, int > >*  Trk::Extrapolator::extrapolate(
                    const Trk::TrackParameters& parm,
                    Trk::PropDirection dir,
                    Trk::ParticleHypothesis particle,
                    std::vector<const Trk::TrackStateOnSurface*>*&  material,
                    int destination) const
{
// extrapolation method intended for collection of intersections with active layers/volumes
// extrapolation stops at indicated geoID subdetector exit

  ATH_MSG_DEBUG( "M-[" << ++m_methodSequence << "] extrapolate(through active volumes), from " << parm.position());
 
  if (!m_stepPropagator) {
    // Get the STEP_Propagator AlgTool
    if ( m_stepPropagator.retrieve().isFailure() ) {
      ATH_MSG_ERROR( "Failed to retrieve tool " << m_stepPropagator );
      ATH_MSG_ERROR("Configure STEP Propagator for extrapolation through active volumes");
      return 0;
    } else {
      ATH_MSG_INFO( "Retrieved tool " << m_stepPropagator );
    }
  }

  // reset the path 
  m_path = 0.; 
  // initialize parameters vector
  m_identifiedParameters = CxxUtils::make_unique<identifiedParameters_t>();
  // initialize material collection
  m_matstates = material;
  // dummy input
  m_currentStatic = 0;
  const Trk::TrackingVolume* boundaryVol=0;
  // cleanup
  m_parametersAtBoundary.resetBoundaryInformation();

  // extrapolate to subdetector boundary
  const Trk::TrackParameters* subDetBounds = extrapolateToVolumeWithPathLimit( parm, -1.,dir,particle, boundaryVol);

  while (subDetBounds) {


    ATH_MSG_DEBUG( "  Identified subdetector boundary crossing saved " << positionOutput(subDetBounds->position()) );
    m_identifiedParameters->push_back(std::pair<const Trk::TrackParameters* , int> 
              (subDetBounds, m_currentStatic? m_currentStatic->geometrySignature() : 0));  

    if (m_currentStatic &&  m_currentStatic->geometrySignature()==destination) break;

    if ( !m_parametersAtBoundary.nextVolume ) break;               // world boundary

    subDetBounds = extrapolateToVolumeWithPathLimit( *subDetBounds, -1.,dir,particle, boundaryVol);
  }
  
  emptyGarbageBin();  

  return ( m_identifiedParameters->size() ? m_identifiedParameters.release() : 0  ) ;  
} 

const Trk::TrackParameters*  Trk::Extrapolator::extrapolateWithPathLimit(
                   const Trk::TrackParameters& parm,
                   double& pathLim,
                   Trk::PropDirection dir,
                   Trk::ParticleHypothesis particle,
                   std::vector<const Trk::TrackParameters*>*& parmOnSf,
                   std::vector<const Trk::TrackStateOnSurface*>*& material,
                   const Trk::TrackingVolume* boundaryVol,
                                                                         MaterialUpdateMode matupmod) const
{
// extrapolation method intended for simulation of particle decay; collects intersections with active layers
// possible outcomes:1/ returns curvilinear parameters after reaching the maximal path
//                   2/ returns parameters at destination volume boundary 
//                   3/ returns 0 ( particle stopped ) but keeps vector of hits    

  ATH_MSG_DEBUG( "M-[" << ++m_methodSequence << "] extrapolateWithPathLimit(...) " << pathLim << ", from " << parm.position());
 
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

  // reset the path 
  m_path = 0.; 
  // initialize parameters vector
  m_parametersOnDetElements = parmOnSf;
  // initialize material collection
  m_matstates = material;
  // cleanup
  m_parametersAtBoundary.resetBoundaryInformation();
 
  // if no input volume, define as highest volume
  //const Trk::TrackingVolume* destVolume = boundaryVol ? boundaryVol : m_navigator->highestVolume();
  m_currentStatic = 0;
  if (boundaryVol && !boundaryVol->inside(parm.position(),m_tolerance) ) return 0;

  // for debugging
  bool old_robust = m_robustSampling;
  if (pathLim == -5.) m_robustSampling = true;
  if (pathLim == -6.) m_robustSampling = false;
   
  // extrapolate to destination volume boundary with path limit
  const Trk::TrackParameters* returnParms = extrapolateToVolumeWithPathLimit( parm, pathLim,dir,particle, boundaryVol, matupmod );

  // folr debugging
  m_robustSampling = old_robust;

  //save actual path on output 
  pathLim = m_path; 
   
  return returnParms;
} 
   
const Trk::TrackParameters*  Trk::Extrapolator::extrapolateToVolumeWithPathLimit(
                   const Trk::TrackParameters& parm,
                   double pathLim,
                   Trk::PropDirection dir,
                   Trk::ParticleHypothesis particle,
                   const Trk::TrackingVolume* destVol,
                   MaterialUpdateMode matupmod) const
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
  unsigned int               iDest = 0;
  
  // destination volume boundary ? 
  if ( destVol && m_navigator->atVolumeBoundary(currPar,destVol,dir,nextVol,m_tolerance) && nextVol != destVol ) { pathLim = m_path; return &parm; }
  
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

  // the navigation sequence may have been evaluated already - check the cache

  bool navigDone = false;
  if (m_parametersAtBoundary.nextParameters && m_parametersAtBoundary.nextVolume) {
    if ( (m_parametersAtBoundary.nextParameters->position()-currPar->position()).mag()<0.001 && 
   m_parametersAtBoundary.nextParameters->momentum().dot(currPar->momentum())>0.001 ) {
      nextVol = m_parametersAtBoundary.nextVolume;
      navigDone = true; 
      if ( nextVol != m_currentStatic ) {
  m_currentStatic = nextVol;
  updateStatic = true;          
      }
    }
  }    
  
  if ( !navigDone && m_navigator->atVolumeBoundary(currPar,m_currentStatic,dir,nextVol,m_tolerance) && nextVol != m_currentStatic ) {
    // no next volume found --- end of the world
    if ( !nextVol ) {
      ATH_MSG_DEBUG( "  [+] Word boundary reached        - at " << positionOutput(currPar->position()) );
      if (!destVol) { pathLim=m_path;}
      //return currPar->clone();
      return currPar;
    }
    m_currentStatic = nextVol;
    updateStatic = true;    
  }

  // alignable volume ? 
  if ( m_currentStatic && m_currentStatic->geometrySignature()==Trk::Calo ) {
    const Trk::AlignableTrackingVolume* alignTV = dynamic_cast<const Trk::AlignableTrackingVolume*> (m_currentStatic);
    if (alignTV) {
      const Trk::TrackParameters* nextPar=extrapolateInAlignableTV(*m_stepPropagator,*currPar,0,alignTV,dir,particle) ;
      if (nextPar) {
  throwIntoGarbageBin(nextPar); 
        return extrapolateToVolumeWithPathLimit( *nextPar, pathLim, dir, particle, destVol, matupmod );
      }
      else return returnParameters; 
    }      
  }

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
    //if (m_currentStatic->entryLayerProvider()) {
    //  const std::vector<const Trk::Layer*>& entryLays = m_currentStatic->entryLayerProvider()->layers();
    //  for (unsigned int i=0; i < entryLays.size(); i++) { 
    //  if (entryLays[i]->layerType()>0 || entryLays[i]->layerMaterialProperties()) {
    //    m_layers.push_back(std::pair<const Trk::Surface*,Trk::BoundaryCheck>(&(entryLays[i]->surfaceRepresentation()),true));
    //    m_navigLays.push_back(std::pair<const Trk::TrackingVolume*,const Trk::Layer*> (m_currentStatic,entryLays[i]) );
    //    Trk::DistanceSolution distSol = m_layers.back().first->straightLineDistanceEstimate(currPar->position(),
    //                                                                                      currPar->momentum().normalized());
    //       //if (distSol.numberOfSolutions()>0) std::cout<<"input distance to entry layer:"<<i<<","<<distSol.first()<< std::endl;
    //      //if (distSol.numberOfSolutions()>1) std::cout<<"input distance2 to entry layer:"<<i<<","<<distSol.second()<< std::endl;
    //      
    //  }
    // } 
    //}

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
  if (m_denseVols.size()>m_denseResolved.first)  m_denseVols.resize(m_denseResolved.first);
  while (m_denseBoundaries.size()>m_denseResolved.second)  m_denseBoundaries.pop_back();

  if (m_layers.size()>m_layerResolved) m_navigLays.resize(m_layerResolved);
  while (m_layers.size()>m_layerResolved)  m_layers.pop_back();

  // current detached volumes
  // collect : subvolume boundaries, ordered/unordered layers, confined dense volumes
  ////////////////////////////////////////////////////// 
  //const Trk::DetachedTrackingVolume* currentActive = 0;
  std::vector<std::pair<const Trk::TrackingVolume*,unsigned int> > navigVols;
   
  gp = currPar->position();    
  std::vector<const Trk::DetachedTrackingVolume*>* detVols =  m_navigator->trackingGeometry()->lowestDetachedTrackingVolumes(gp);    
  std::vector<const Trk::DetachedTrackingVolume*>::iterator dIter = detVols->begin();
  for (; dIter != detVols->end(); dIter++) {
    const Trk::Layer* layR = (*dIter)->layerRepresentation();
    bool active = ( layR && layR->layerType() ) ? true : false;
    if (active && !resolveActive) continue; 
    if (!active && m_currentStatic->geometrySignature()==Trk::MS && m_useMuonMatApprox
  && (*dIter)->name().substr((*dIter)->name().size()-4,4)!="PERM") continue;
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
          if (m_robustSampling) {
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
      //     &&(*dIter)->layerRepresentation()->layerType()>0 ) ) currentActive=(*dIter);  
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
    if (m_robustSampling) {
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
      //  lay = m_currentStatic->associatedLayer(gp+m_tolerance*parm.momentum().unit());
      //  std::cout<<" find input associated layer, second attempt:"<< lay<< std::endl;
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
  
  // ready to propagate      
  // till: A/ static volume boundary(bcheck=true) , B/ material layer(bcheck=true), C/ destination surface(bcheck=false)
  // update of m_navigSurfs required if I/ entry into new navig volume, II/ exit from currentActive without overlaps
   
   nextVol = 0;
   while (currPar) {      
     double path=0.;
     if (pathLim>0.) path = pathLim;
     std::vector<unsigned int> solutions;
     ATH_MSG_DEBUG( "  [+] Starting propagation at position  " << positionOutput(currPar->position())  
        << " (current momentum: " << currPar->momentum().mag() << ")" ); 
     ATH_MSG_DEBUG( "  [+] " << m_navigSurfs.size() << " target surfaces in '" << m_currentDense->volumeName() <<"'.");      // verify that material input makes sense
     ATH_MSG_DEBUG( "  [+] " << " with path limit"<< pathLim  <<",");      // verify that material input makes sense
     ATH_MSG_DEBUG( "  [+] " << " in the direction"<< dir  <<".");      // verify that material input makes sense
     if (!(m_currentDense->inside(currPar->position(),m_tolerance) 
     || m_navigator->atVolumeBoundary(currPar,m_currentDense,dir,assocVol,m_tolerance) ) ) m_currentDense = m_highestVolume ;
     //const Trk::TrackParameters* nextPar = m_stepPropagator->propagate(*currPar,m_navigSurfs,dir,*m_currentDense,particle,solutions,path,true);
     if(m_extrapolationCache&&m_dumpCache) std::cout << "  m_stepPropagator->propagate " << m_extrapolationCache << std::endl;
     const Trk::TrackParameters* nextPar = m_stepPropagator->propagate(*currPar,m_navigSurfs,dir,m_fieldProperties,particle,solutions,path,true,false,m_currentDense);
     ATH_MSG_VERBOSE( "  [+] Propagation done. " );
     if (nextPar)  {
       ATH_MSG_DEBUG( "  [+] Position after propagation -   at " << positionOutput(nextPar->position())); 
       ATH_MSG_DEBUG( "  [+] Momentum after propagation - " << nextPar->momentum()); 
     }

     if ( pathLim>0. && m_path+path>=pathLim ) {
       m_path+=path;
       return nextPar;
     }
     // check missing volume boundary
     if (nextPar && !(m_currentDense->inside(nextPar->position(),m_tolerance) 
          || m_navigator->atVolumeBoundary(nextPar,m_currentDense,dir,assocVol,m_tolerance) ) ) {
       ATH_MSG_DEBUG( "  [!] ERROR: missing volume boundary for volume"<< m_currentDense->volumeName() );
       if ( m_currentDense->zOverAtimesRho() != 0.) {     
         ATH_MSG_DEBUG( "  [!] ERROR: trying to recover: repeat the propagation step in"<< m_highestVolume->volumeName() );
         m_currentDense = m_highestVolume;
   throwIntoGarbageBin(nextPar);         
         continue;
       }
     }
     if (!nextPar) {
       ATH_MSG_DEBUG( "  [!] Propagation failed, return 0" );
       m_parametersAtBoundary.boundaryInformation(m_currentStatic,nextPar,nextPar);     
       return returnParameters;
     }
     m_path += path;
     if (pathLim>0.) pathLim -= path; 
     ATH_MSG_DEBUG( "  [+] Number of intersection solutions: " << solutions.size() );
     throwIntoGarbageBin(nextPar);         
     // collect material
     if ( m_currentDense->zOverAtimesRho() != 0. && !m_matstates && m_extrapolationCache) {     
       double dInX0 = fabs(path)/m_currentDense->x0();  
       double currentqoverp=nextPar->parameters()[Trk::qOverP];
       MaterialProperties materialProperties(*m_currentDense, fabs(path));
       Trk::EnergyLoss* eloss = m_elossupdators[0]->energyLoss(materialProperties,fabs(1./currentqoverp),1.,dir,particle); 
       if(m_dumpCache) dumpCache(" extrapolateToVolumeWithPathLimit");
       m_extrapolationCache->updateX0(dInX0);
       m_extrapolationCache->updateEloss(eloss->meanIoni(),eloss->sigmaIoni(),eloss->meanRad(),eloss->sigmaRad());
       if(m_dumpCache) dumpCache(" After");
       delete eloss;
     }
     if ( m_currentDense->zOverAtimesRho() != 0. && m_matstates) {     
       double dInX0 = fabs(path)/m_currentDense->x0();  
       MaterialProperties materialProperties(*m_currentDense, fabs(path));
       double scatsigma=sqrt(m_msupdators[0]->sigmaSquare(materialProperties,1./fabs(nextPar->parameters()[qOverP]),1.,particle));
       Trk::ScatteringAngles *newsa=new Trk::ScatteringAngles(0,0,scatsigma/sin(nextPar->parameters()[Trk::theta]),scatsigma);   
       //energy loss
       double currentqoverp=nextPar->parameters()[Trk::qOverP];
       Trk::EnergyLoss* eloss = m_elossupdators[0]->energyLoss(materialProperties,fabs(1./currentqoverp),1.,dir,particle); 
       // compare energy loss
       ATH_MSG_DEBUG(" [M] Energy loss: STEP , EnergyLossUpdator:"
                  << nextPar->momentum().mag()-currPar->momentum().mag() << ","<< eloss->deltaE() );
       // adjust energy loss ?       
       // double adj = (particle!=nonInteracting && particle!=nonInteractingMuon && fabs(eloss0->deltaE())>0) ? (nextPar->momentum().mag()-currPar->momentum().mag())/eloss0->deltaE() : 1;
       // Trk::EnergyLoss* eloss = new Trk::EnergyLoss(adj*eloss0->deltaE(),adj*eloss0->sigmaDeltaE()); 
       // delete eloss0;
       
       Trk::MaterialEffectsOnTrack* mefot = new Trk::MaterialEffectsOnTrack(dInX0,newsa,eloss,
                      *((nextPar->associatedSurface()).baseSurface()));
       
       m_matstates->push_back(new TrackStateOnSurface(0,nextPar->clone(),0,mefot));
       if(m_extrapolationCache) {
              if(m_dumpCache) dumpCache(" extrapolateToVolumeWithPathLimit");
              m_extrapolationCache->updateX0(dInX0);
              m_extrapolationCache->updateEloss(eloss->meanIoni(),eloss->sigmaIoni(),eloss->meanRad(),eloss->sigmaRad());
              if(m_dumpCache) dumpCache(" After");
       }
       ATH_MSG_DEBUG("  [M] Collecting material from dense volume '" 
         << m_currentDense->volumeName()<< "', t/X0 = " << dInX0);
     }
     
     //int iDest = 0;
     unsigned int iSol  = 0;
     while ( iSol < solutions.size() ) {
       if ( solutions[iSol] < iDest ) {
         return nextPar->clone();          
       } else if ( solutions[iSol] < iDest + m_staticBoundaries.size() ) {

   // material attached ?
   const Trk::Layer*  mb =  m_navigSurfs[solutions[iSol]].first->materialLayer();  
   if (mb) {
     if (mb->layerMaterialProperties() && mb->layerMaterialProperties()->fullMaterial(nextPar->position()) ) {

       double pIn = nextPar->momentum().mag(); 
       const IMaterialEffectsUpdator* currentUpdator = subMaterialEffectsUpdator(*m_currentStatic);
       if ( currentUpdator)  {
         const Trk::TrackParameters* upNext = currentUpdator->update(nextPar, *mb, dir, particle, matupmod);
               if (upNext && upNext!= nextPar ) throwIntoGarbageBin(upNext);
               nextPar = upNext;
       }
       if (!nextPar) {
         ATH_MSG_VERBOSE( "  [+] Update may have killed track - return." );
         m_parametersAtBoundary.resetBoundaryInformation();
         return returnParameters;
       } else {   // the MEOT will be saved at the end
         ATH_MSG_VERBOSE(" Update energy loss:"<< nextPar->momentum().mag()- pIn << "at position:"<< nextPar->position());
         if (m_matstates) addMaterialEffectsOnTrack(*m_stepPropagator,*nextPar,*mb,*m_currentStatic,dir,particle);
       }
     }
   }

   // static volume boundary; return to the main loop 
   unsigned int index = solutions[iSol]-iDest;
   // use global coordinates to retrieve attached volume (just for static!)
   nextVol = (m_currentStatic->boundarySurfaces())[index].getPtr()->attachedVolume(nextPar->position(),nextPar->momentum(),dir);
   if (nextVol != m_currentStatic ) {
     m_parametersAtBoundary.boundaryInformation(nextVol,nextPar,nextPar);         
     ATH_MSG_DEBUG("  [+] StaticVol boundary reached of '" <<m_currentStatic->volumeName() << "', geoID: "
       <<m_currentStatic->geometrySignature());
     if ( m_navigator->atVolumeBoundary(nextPar,m_currentStatic,dir,assocVol,m_tolerance) && assocVol != m_currentStatic )
       m_currentDense = m_dense ? nextVol : m_highestVolume;
     // no next volume found --- end of the world
     if ( !nextVol ) {
       ATH_MSG_DEBUG( "  [+] World boundary reached        - at " << positionOutput(nextPar->position()) );
       if (!destVol) { pathLim=m_path; return nextPar->clone();}
     }
           // next volume found and parameters are at boundary
     if ( nextVol && nextPar ){ 
       ATH_MSG_DEBUG( "  [+] Crossing to next volume '" << nextVol->volumeName() << "', next geoID: "<<nextVol->geometrySignature());
       ATH_MSG_DEBUG( "  [+] Crossing position is         - at " <<  positionOutput(nextPar->position()) );
             if (!destVol && m_currentStatic->geometrySignature()!=nextVol->geometrySignature()) { pathLim=m_path; return nextPar->clone(); }   
     }     
     return extrapolateToVolumeWithPathLimit(*nextPar,pathLim,dir,particle,destVol,matupmod);
   }
       } else if ( solutions[iSol] < iDest + m_staticBoundaries.size() + m_layers.size() ) {
   // next layer; don't return passive material layers unless required 
   unsigned int index = solutions[iSol]-iDest-m_staticBoundaries.size();
   const Trk::Layer* nextLayer = m_navigLays[index].second;
         // material update ?
         //bool matUp = nextLayer->layerMaterialProperties() && m_includeMaterialEffects && nextLayer->isOnLayer(nextPar->position());
         bool matUp = nextLayer->fullUpdateMaterialProperties(*nextPar) && m_includeMaterialEffects && nextLayer->isOnLayer(nextPar->position());
         // identical to last material layer ?
         if (matUp && nextLayer==m_lastMaterialLayer &&  nextLayer->surfaceRepresentation().type()!=Trk::Surface::Cylinder ) matUp = false;

   // material update: pre-update 
   const IMaterialEffectsUpdator* currentUpdator = subMaterialEffectsUpdator(*m_currentStatic);
         if ( matUp && nextLayer->surfaceArray() ) {
           double pIn = nextPar->momentum().mag();
           if (currentUpdator) {  
       const Trk::TrackParameters* upNext = currentUpdator->preUpdate(nextPar, *nextLayer, dir, particle, matupmod);
             if (upNext && upNext!=nextPar) throwIntoGarbageBin(upNext);
             nextPar = upNext;
     }
     if (!nextPar) {
       ATH_MSG_VERBOSE( "  [+] Update may have killed track - return." );
       m_parametersAtBoundary.resetBoundaryInformation();
       return returnParameters;
     } else {   // the MEOT will be saved at the end
             ATH_MSG_VERBOSE(" Pre-update energy loss:"<< nextPar->momentum().mag()- pIn << "at position:"<< nextPar->position()<<", current momentum:"<<nextPar->momentum());
           }
   }   
         // active surface intersections ( Fatras hits ...)
   if (m_parametersOnDetElements &&  particle!=Trk::neutron) {
     if (nextLayer->surfaceArray()) {
       // perform the overlap Search on this layer
       ATH_MSG_VERBOSE( "  [o] Calling overlapSearch() on  layer.");  
       overlapSearch(*m_subPropagators[0],*currPar,*nextPar,*nextLayer,*m_currentStatic,dir,true,particle);            
     } else if (nextLayer->layerType()>0 && nextLayer->isOnLayer(nextPar->position())) {
       ATH_MSG_VERBOSE( "  [o] Collecting intersection with active layer.");  
             m_parametersOnDetElements->push_back(nextPar->clone());
     }
   } // ------------------------------------------------- Fatras mode off -----------------------------------


         if ( matUp ) {
           if ( nextLayer->surfaceArray() ) {
       // verify there is material for postUpdate
       double postFactor = nextLayer->postUpdateMaterialFactor( *nextPar, dir);
             if (postFactor>0.1) {
         double pIn = nextPar->momentum().mag(); 
         const Trk::TrackParameters* updatedPar = currentUpdator ? currentUpdator->postUpdate(*nextPar, *nextLayer, dir, particle, matupmod) : nextPar;
         if (!updatedPar) {
     ATH_MSG_VERBOSE("postUpdate failed for input parameters:"<<nextPar->position()<<","<<nextPar->momentum());
     ATH_MSG_VERBOSE( "  [+] Update may have killed track - return." );
     m_parametersAtBoundary.resetBoundaryInformation();
     return returnParameters;
         } else {   // the MEOT will be saved at the end
                 if (updatedPar != nextPar) throwIntoGarbageBin(updatedPar);
     throwIntoGarbageBin(nextPar);
     nextPar = updatedPar;
     ATH_MSG_VERBOSE(" Post-update energy loss:"<< nextPar->momentum().mag()- pIn << "at position:"<< nextPar->position());
         }
       }
           } else {
       double pIn = nextPar->momentum().mag(); 
       if ( currentUpdator)  {
         const Trk::TrackParameters* upNext = currentUpdator->update(nextPar, *nextLayer, dir, particle, matupmod);
               if (upNext && upNext!= nextPar ) throwIntoGarbageBin(upNext);
               nextPar = upNext;
       }
       if (!nextPar) {
         ATH_MSG_VERBOSE( "  [+] Update may have killed track - return." );
         m_parametersAtBoundary.resetBoundaryInformation();
         return returnParameters;
       } else {   // the MEOT will be saved at the end
         ATH_MSG_VERBOSE(" Update energy loss:"<< nextPar->momentum().mag()- pIn << "at position:"<< nextPar->position());
       }
     }
           if (m_matstates) addMaterialEffectsOnTrack(*m_stepPropagator,*nextPar,*nextLayer,*m_currentStatic,dir,particle);
     if (m_cacheLastMatLayer) m_lastMaterialLayer = nextLayer;   
   }   
       
   if ( !m_robustSampling ) { 
     if (m_navigLays[index].first && m_navigLays[index].first->confinedLayers()) {
       const Trk::Layer* newLayer = nextLayer->nextLayer(nextPar->position(),dir*nextPar->momentum().normalized());
       if (newLayer && newLayer!=nextLayer) {
               bool found = false;
               int replace = -1;
               for (unsigned int i=0;i<m_navigLays.size();i++) { 
     if (m_navigLays[i].second==newLayer) {found=true; break;}
                 if (m_navigLays[i].second !=nextLayer) replace = i;
         }
               if (!found) { 
                 if (replace>-1) {
       m_navigLays[replace].second = newLayer;
       m_navigSurfs[solutions[iSol]+replace-index].first = &(newLayer->surfaceRepresentation());
                 } else {
                   // can't insert a surface in middle
       return extrapolateToVolumeWithPathLimit(*nextPar,pathLim,dir,particle,destVol,matupmod);                   
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
           if (currVol->inside(tp,0.)) {
             m_currentDense = currVol;
           } else if (!nextVol || !nextVol->inside(tp,0.) ) {   // search for dense volumes
       m_currentDense =  m_highestVolume;
       if (m_dense && !m_denseVols.size()) m_currentDense = m_currentStatic;
       else {
         for (unsigned int i=0;i<m_denseVols.size(); i++) {
     const Trk::TrackingVolume* dVol = m_denseVols[i].first;
     if ( dVol->inside(tp,0.)  && dVol->zOverAtimesRho()!=0. ){
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
           // the boundary orientation is not reliable
     Amg::Vector3D tp = nextPar->position()+2*m_tolerance*dir*nextPar->momentum().normalized();
           if (nextVol && nextVol->inside(tp,0.)) {
       ATH_MSG_DEBUG("  [+] Navigation volume boundary, entering volume '" << nextVol->volumeName() << "'.");
           } else if ( currVol->inside(tp,0.) ) {
             nextVol = currVol;    
       ATH_MSG_DEBUG("  [+] Navigation volume boundary, entering volume '" << nextVol->volumeName() << "'.");
     } else {
             nextVol = 0;
       ATH_MSG_DEBUG("  [+] Navigation volume boundary, leaving volume '" << currVol->volumeName() << "'.");
     } 
     currPar = nextPar;
     // return only if detached volume boundaries not collected
     //if ( nextVol || !detachedBoundariesIncluded )
     if ( nextVol )
       return extrapolateToVolumeWithPathLimit(*currPar,pathLim,dir,particle,destVol,matupmod);                 
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
           // the boundary orientation is not reliable
     Amg::Vector3D tp = nextPar->position()+2*m_tolerance*dir*nextPar->momentum().normalized();
           if (nextVol && nextVol->inside(tp,0.)) {
       ATH_MSG_DEBUG("  [+] Detached volume boundary, entering volume '" << nextVol->volumeName() << "'.");
           } else if ( currVol->inside(tp,0.) ) {
             nextVol = currVol;    
       ATH_MSG_DEBUG("  [+] Detached volume boundary, entering volume '" << nextVol->volumeName() << "'.");
     } else {
             nextVol = 0;
       ATH_MSG_DEBUG("  [+] Detached volume boundary, leaving volume '" << currVol->volumeName() << "'.");
     } 
     currPar = nextPar;
     //if ( nextVol || !detachedBoundariesIncluded)
     if ( nextVol )
       return extrapolateToVolumeWithPathLimit(*currPar,pathLim,dir,particle,destVol,matupmod);   
   } 
       }   
       iSol++;
     }
     currPar = nextPar;
   }

   return returnParameters;
}        


 


