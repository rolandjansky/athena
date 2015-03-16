/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// TimedExtrapolator.h, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

#ifndef TRKEXTOOLS_TIMEDEXTRAPOLATOR_H
#define TRKEXTOOLS_TIMEDEXTRAPOLATOR_H

// Gaudi
#include "AthenaBaseComps/AthAlgTool.h"
#include "GaudiKernel/ToolHandle.h"
// Trk
#include "TrkExInterfaces/ITimedExtrapolator.h"
#include "TrkExInterfaces/INavigator.h"
#include "TrkExInterfaces/ITimedMatEffUpdator.h"
#include "TrkEventPrimitives/PropDirection.h"
#include "TrkEventPrimitives/ParticleHypothesis.h"
#include "TrkParameters/TrackParameters.h"
#include "TrkSurfaces/BoundaryCheck.h"
#include "TrkSurfaces/PlaneSurface.h"
#include "TrkVolumes/BoundarySurface.h"
#include "TrkVolumes/BoundarySurfaceFace.h"
#include "TrkGeometry/TrackingVolume.h"
#include "TrkDetDescrUtils/GeometrySignature.h"
#include "TrkGeometry/MagneticFieldProperties.h"
// STL
#include <cstring>
#include <vector>
#include <map>

class MsgStream;

namespace Trk {

  class Track;
  class Surface;
  class Layer;
  class Volume;                  
  class DetachedTrackingVolume;
  class AlignableTrackingVolume;
  class TrackingGeometry;
  class TrackParticleBase;
  class IPropagator;
  class IDynamicLayerCreator;
  class INavigator;
  class IMultipleScatteringUpdator;
  class IEnergyLossUpdator;
  
  typedef std::vector<const Trk::TrackParameters*> TrackParametersVector;

  typedef std::pair< const Surface*, BoundaryCheck > DestSurf;

  struct DestBound
  {
    const Surface* surface;    // surface 
    double         distance;   // distance to surface
    unsigned int   bIndex;     // boundary index for fast navigation to the next volume 
    //
    DestBound( const Surface* surf, double dist, unsigned int index ) :
      surface(surf),distance(dist),bIndex(index)
      {}
  };

  struct BoundaryTrackParameters
  {
    const TrackParameters* trPar;
    const TrackingVolume* exitVol;
    const TrackingVolume* entryVol;
    //
    BoundaryTrackParameters( const TrackParameters* parms, 
			     const TrackingVolume* exitTV, 
			     const TrackingVolume* entryTV ) :
    trPar(parms), exitVol(exitTV), entryVol(entryTV)
    {} 
  };

  struct IdentifiedIntersection
  {
    float distance;
    int   identifier;
    const Trk::Material* material;
    //
    IdentifiedIntersection( float dist, int id, const Trk::Material* mat) :
    distance(dist), identifier(id), material(mat)
    {}
  };

  /** @struct ParametersAtBoundarySurface
      has only three member
      - BoundarySurface
      - TrackParameters
      - bool that indicated the deletion of the TrackParameters
   */
  
  struct ParamsNextVolume {
    //!< the members
    mutable const TrackingVolume*    nextVolume;
    mutable const TrackParameters*   nextParameters;
    mutable const TrackParameters*   navParameters;
    mutable BoundarySurfaceFace      exitFace;
    
    ParamsNextVolume(){
      nextVolume              = 0;
      nextParameters          = 0;
      navParameters           = 0;
        exitFace                = undefinedFace;
    }
    
    
    //!< update the boundaryInformation
    void boundaryInformation(const TrackingVolume* tvol,
			     const TrackParameters* nextPars,
			     const TrackParameters* navPars,
			     BoundarySurfaceFace    face=undefinedFace
                             ) const 
    {
      nextVolume       = tvol;
      nextParameters   = nextPars;
      navParameters    = navPars;
      exitFace         = face;
    }
    
    //!< reset the boundary information by invalidating it
    void resetBoundaryInformation(){
      nextVolume        = 0;
      nextParameters    = 0;
      navParameters     = 0;
      exitFace          = undefinedFace;
    }    
  };
 
  
  /** 
    @class TimedExtrapolator
      
    The TimedExtrapolator is to be used for the simulation purposes
             
    The output level is as follows:
       INFO    : initialize / finalize information
       DEBUG   : Method call sequence
       VERBOSE : Method call sequence with values
       
    @author sarka.todorova@cern.ch
   */
          
  class TimedExtrapolator : public AthAlgTool,
                       virtual public ITimedExtrapolator {
     public:
       /**Constructor */
       TimedExtrapolator(const std::string&,const std::string&,const IInterface*);
       /**Destructor*/
       ~TimedExtrapolator();
       
       /** AlgTool initailize method.
       In this method the extrapolator should retrieve the Propagator of highest order which is then passed through
       the extrapolate method. The Propagator itself should be specified whether to use propagators of a lower hirarchy
       level or not.
        */
       StatusCode initialize();
       /** AlgTool finalize method */
       StatusCode finalize();

       /** Extrapolation method for charged, possibly unstable particles.  
           The extrapolation is interrupted at subdetector boundary for surviving/stable particles.
       */
                                                                                                                                        
       const Trk::TrackParameters*  extrapolateWithPathLimit( const Trk::TrackParameters& parm,
							      Trk::PathLimit& pathLim, Trk::TimeLimit& time,
							      Trk::PropDirection dir,
							      Trk::ParticleHypothesis particle,
							      std::vector<Trk::HitInfo>*& hitVector,
                                                              Trk::GeometrySignature& nextGeoID,
							      const Trk::TrackingVolume* boundaryVol=0) const;

       /** Transport method for neutral, possibly unstable particles.  
           The extrapolation is interrupted at subdetector boundary for surviving/stable particles.
       */
          
       const Trk::TrackParameters*  transportNeutralsWithPathLimit( const Trk::TrackParameters& parm,
								    Trk::PathLimit& pathLim, Trk::TimeLimit& time,
								    Trk::PropDirection dir,                                    
								    Trk::ParticleHypothesis particle,
								    std::vector<Trk::HitInfo>*& hitVector,
								    Trk::GeometrySignature& nextGeoId,
								    const Trk::TrackingVolume* boundaryVol=0) const;


       /** Return the TrackingGeometry used by the Extrapolator (forward information from Navigator)*/
       const TrackingGeometry* trackingGeometry() const; 

      /** Validation Action:
          Can be implemented optionally, outside access to internal validation steps */
      virtual void validationAction() const;
                                           
  private:
     const Trk::TrackParameters*  extrapolateToVolumeWithPathLimit(
								   const Trk::TrackParameters& parm,
								   Trk::TimeLimit& time,
								   Trk::PropDirection dir,
								   Trk::ParticleHypothesis particle,
								   Trk::GeometrySignature& nextGeoID,
								   const Trk::TrackingVolume* destVol) const;
     
     BoundaryTrackParameters  extrapolateInAlignableTV(
						     const Trk::TrackParameters& parm,
						     Trk::TimeLimit& time,
						     Trk::PropDirection dir,                                    
						     Trk::ParticleHypothesis particle,
						     Trk::GeometrySignature& nextGeoId,
						     const Trk::AlignableTrackingVolume* aliTV) const;
     

     const Trk::TrackParameters*  transportToVolumeWithPathLimit(
								 const Trk::TrackParameters& parm,
								 Trk::TimeLimit& time,
								 Trk::PropDirection dir,                                    
								 Trk::ParticleHypothesis particle,
								 Trk::GeometrySignature& nextGeoId,
								 const Trk::TrackingVolume* boundaryVol) const;

     BoundaryTrackParameters  transportInAlignableTV(
						     const Trk::TrackParameters& parm,
						     Trk::TimeLimit& time,
						     Trk::PropDirection dir,                                    
						     Trk::ParticleHypothesis particle,
						     Trk::GeometrySignature& nextGeoId,
						     const Trk::AlignableTrackingVolume* aliTV) const;
       
    /** Access the subPropagator to the given volume*/
    const IPropagator* subPropagator(const TrackingVolume& tvol) const;
                                       
    /** Access the subPropagator to the given volume*/
    const ITimedMatEffUpdator* subMaterialEffectsUpdator(const TrackingVolume& tvol) const;

    /** Private method for throwing into the GarbageBin */
    void throwIntoGarbageBin(const Trk::TrackParameters* garbage) const;

    /** Private method for emptying the GarbageBin */
    void emptyGarbageBin() const;
    void emptyGarbageBin(const Trk::TrackParameters*) const;
    
    /** Private to search for overlap surfaces */
    void overlapSearch(const IPropagator& prop,
                       const TrackParameters& parm,
                       const TrackParameters& parsOnLayer,
                       const Layer& lay,
                       //const TrackingVolume& tvol,
                       float time,
                       PropDirection dir = anyDirection,
                       BoundaryCheck bcheck = true,
                       ParticleHypothesis particle=pion,
                       bool startingLayer = false) const;


    /** Private method for conversion of the synchronized geometry signature to the natural subdetector ordering */
    //unsigned int geoIDToDetOrder(Trk::GeometrySignature geoid) const;

    /** For the output - global position */
    std::string positionOutput(const Amg::Vector3D& pos) const;
    
    /** For the output - global momentum */
    std::string momentumOutput(const Amg::Vector3D& mom) const;

   // --------------- Used Tools ----------------------------- //
 
    ToolHandleArray< IPropagator >              m_propagators;                   //!<  Array of Propagators
    ToolHandle< IPropagator >                   m_stepPropagator;                //!<  Array of Propagators
    ToolHandle<  INavigator >                   m_navigator;                     //!<  Navigator for TrackingGeometry and magnetic fiels acces
    ToolHandleArray< ITimedMatEffUpdator >      m_updators;                      //!<  Array of Material Updators
    ToolHandleArray< IMultipleScatteringUpdator >  m_msupdators;                 //!<  Array of MultipleScattering Updators
    ToolHandleArray< IEnergyLossUpdator >       m_elossupdators;                      //!<  Array of EnergyLoss Updators

    // ---------------- For Extrapolation handling ------------ //
     
    std::vector<const IPropagator*>             m_subPropagators;                //!< Propagators to chose from (steered by signature)
    std::vector<const ITimedMatEffUpdator*>     m_subUpdators;                   //!< Updators to chose from (steered by signature)

    // ---------------- For Extrapolator configuration ------------ //

    std::vector<std::string>                    m_propNames;                    //!<  configuration of subPropagators
    std::vector<std::string>                    m_updatNames;                   //!<  configuration of subUpdators

    // --------------- General steering & Navigation -------------- //

    unsigned int                    m_meotpIndex;                    //!< if several meotps are available in a volume steer which one to use
    unsigned int                    m_configurationLevel;            //!< see the supported levels of configuration above
    unsigned int                    m_searchLevel;                   //!< see the supported search levels above
    bool                            m_includeMaterialEffects;        //!< boolean to switch on/off material effects
    bool                            m_requireMaterialDestinationHit; //!< require the destination surface hit for material collection
    bool                            m_stopWithNavigationBreak;       //!< return 0 if navigation breaks - for validation reasons
    bool                            m_stopWithUpdateZero;            //!< return 0 if update kills the trajectory 
    bool                            m_subSurfaceLevel;               //!< tep down to sub-surface level
    bool                            m_skipInitialLayerUpdate;        //!< skip the initial post-Update at the layer [Fatras conversion mode]
    bool                            m_referenceMaterial;             //!< use the reference material for the update
    bool                            m_extendedLayerSearch;           //!< extended layer search
    unsigned int                    m_initialLayerAttempts;          //!< allowed layer intersection attempts at the start of a volume
    unsigned int                    m_successiveLayerAttempts;       //!< layer intersection attemps after one layer has been hit sucessfully

    double                          m_tolerance;                    //!< surfacen & volume tolerance

    bool                            m_caloMsSecondary;               //!< handling of secondaries beyond ID 
    
    // ------------------------------------------------------- //      
   
    bool                            m_activeOverlap;                 //!<  consider overlaps between active muon volumes  
    bool                            m_useDenseVolumeDescription;     //!<  use dense volume description when available in ID/Calo
    bool                            m_useMuonMatApprox;              //!<  use approximative MS inert material
    bool                            m_checkForCompundLayers;         //!<  use the multi-layer tests for compound layers
    mutable bool                    m_dense;                         //!<  internal switch for resolved configuration 
    
    // ------------ Re-initialize (depending on volumes)  ----------------------- //    
    mutable const Surface*          m_destinationSurface;                    

    // ------------ Recall / Boundary Information ----------------------- //    
    mutable const Volume*           m_boundaryVolume;                 //!< the boundary volume check

    mutable bool                    m_recall;                         //!< Flag the recall solution
    mutable const Surface*          m_recallSurface;                  //!< Destination Surface for recall
    mutable const Layer*            m_recallLayer;                    //!< Destination Layer for recall
    mutable const TrackingVolume*   m_recallTrackingVolume;           //!< Destination TrackingVolume for recall
    
    mutable ParamsNextVolume    m_parametersAtBoundary;           //!< return helper for parameters and boundary  
    mutable ParamsNextVolume    m_parametersAtDetachedBoundary;   //!< return helper for parameters and boundary 
    mutable std::vector<Trk::HitInfo>*    m_hitVector;                //!< return helper for hit info

    mutable const Trk::TrackParameters* m_lastValidParameters;            //!< parameters to be used for final propagation in case of fallback

    mutable std::map<const Trk::TrackParameters*, bool> m_garbageBin; //!< garbage collection during extrapolation

    mutable const Trk::TrackingVolume*                                m_currentStatic;
    mutable const Trk::TrackingVolume*                                m_currentDense;
    mutable const Trk::TrackingVolume*                                m_highestVolume;
    mutable bool                                                      m_resolveActive;
    mutable bool                                                      m_resolveMultilayers;
    mutable std::pair<unsigned int, unsigned int>                     m_denseResolved;
    mutable unsigned int                                              m_layerResolved;  
    mutable std::vector<std::pair<const Trk::DetachedTrackingVolume*,unsigned int> >    m_detachedVols;
    mutable std::vector<std::pair<const Trk::TrackingVolume*,unsigned int> >            m_denseVols;
    mutable std::vector<std::pair<const Trk::TrackingVolume*,const Trk::Layer*> >       m_navigLays;
    mutable std::vector<DestSurf>                                     m_staticBoundaries;
    mutable std::vector<DestSurf>                                     m_detachedBoundaries;
    mutable std::vector<DestSurf>                                     m_denseBoundaries;
    mutable std::vector<DestSurf>                                     m_navigBoundaries;
    mutable std::vector<DestSurf>                                     m_layers;
    bool                                                              m_returnPassiveLayers;
    mutable bool                                                      m_robustSampling;
    mutable PathLimit                                                 m_path;    
    mutable double                                                    m_time;    
    mutable size_t                 m_currentLayerBin;

    //------------------------- NAVIGATION -------- ----------------------------------------------//
    mutable int                     m_methodSequence;
    mutable int                     m_maxMethodSequence;

    //-------------------------- SCREEN output steering -------------------------------------------//
    bool                            m_printHelpOutputAtInitialize;
    bool                            m_printRzOutput;

    //------------------------- VALIDATION MODE SECTION ------------------------------------------//

    mutable int                     m_extrapolateCalls;              //!< number of calls: extrapolate() method
    mutable int                     m_extrapolateBlindlyCalls;       //!< number of calls: extrapolateBlindly() method 
    mutable int                     m_extrapolateDirectlyCalls;      //!< number of calls: extrapolateDirectly() method
    mutable int                     m_extrapolateStepwiseCalls;      //!< number of calls: extrapolateStepwise() method

    mutable int                     m_startThroughAssociation;        //!< navigation intialization
    mutable int                     m_startThroughRecall;             //!< navigation intialization
    mutable int                     m_startThroughGlobalSearch;       //!< navigation intialization
    mutable int                     m_destinationThroughAssociation;  //!< navigation intialization
    mutable int                     m_destinationThroughRecall;       //!< navigation intialization
    mutable int                     m_destinationThroughGlobalSearch; //!< navigation intialization
    
    mutable int                     m_layerSwitched;                 //!< number of layers that have been switched 

    // ----------------------------- navigation validation section -----------------------------------------------------------

    bool                            m_navigationStatistics;           //!< steer the output for the navigaiton statistics
    bool                            m_navigationBreakDetails;         //!< steer the output for the navigation break details
    mutable int                     m_navigationBreakLoop;            //!< number of navigation breaks due to loop
    mutable std::map<const Trk::TrackingVolume*,int> m_loopVolumes;   //!< record name of the volumes where oscillation happened
    mutable int                     m_navigationBreakOscillation;     //!< number of navigation breaks due to oscillation
    mutable std::map<const Trk::TrackingVolume*,int> m_oscillationVolumes;    //!< record name of the volumes where oscillation happened
    mutable int                     m_navigationBreakNoVolume;       //!< number of navigation breaks due no Volume found
    mutable std::map<const Trk::TrackingVolume*,int> m_noNextVolumes;         //!< record names of the volumes where no next one is found
    mutable int                     m_navigationBreakDistIncrease;   //!< number of navigation breaks due to distance increase
    mutable std::map<const Trk::TrackingVolume*,int> m_distIncreaseVolumes;   //!< record name of  the voluems where the distance increases
    mutable int                     m_navigationBreakVolumeSignature;   //!< number of navigation breaks due to distance increase
    mutable std::map<const Trk::TrackingVolume*,int> m_volSignatureVolumes;   //!< record name of  the voluems where the distance increases
    mutable int                     m_overlapSurfaceHit;             //!< number of OverlapSurfaces found

    bool                            m_materialEffectsOnTrackValidation; //!< mat effects on track validation
    mutable int                     m_meotSearchCallsFw;                //!< how often the meot search is called: forward
    mutable int                     m_meotSearchCallsBw;                //!< how often the meot search is called: backward
    mutable int                     m_meotSearchSuccessfulFw;           //!< how often the meot search was successful: forward
    mutable int                     m_meotSearchSuccessfulBw;           //!< how often the meot search was successful: backward

    // ------------------------------- cache --------------------------------------------------------------------

    mutable const Layer*                                       m_lastMaterialLayer; //!< cache layer with last material update
    bool                                                       m_cacheLastMatLayer;   // steering of the material layer cache 

    // ------------------------------- static members --------------------------------------------------------------------
    static double                   s_distIncreaseTolerance;         //!< distance increatse tolerance to account for straight line approx.
	static double                   s_distEntryLayerMax;			//!< maximal allowed distance to the entry layer	 
    static PlaneSurface             s_referenceSurface;              //!< the reference Surface

    unsigned int m_maxNavigSurf;
    unsigned int m_maxNavigVol;
    mutable std::vector<std::pair<const Trk::Surface*,Trk::BoundaryCheck> >  m_navigSurfs;
    mutable std::vector<const Trk::DetachedTrackingVolume*>                  m_navigVols;
    mutable std::vector<std::pair<const Trk::TrackingVolume*,unsigned int> > m_navigVolsInt;
    mutable std::vector<std::pair<const Trk::Surface*,double> >              m_trSurfs;
    mutable std::vector< Trk::DestBound >                                    m_trStaticBounds;  // need to cache the boundary index, too
    mutable std::vector<std::pair<const Trk::Surface*,double>  >             m_trDetachBounds;
    mutable std::vector<std::pair<const Trk::Surface*,double>  >             m_trDenseBounds;
    mutable std::vector<std::pair<const Trk::Surface*,double> >              m_trLays;

    static  ParticleMasses                 s_particleMasses; 
    mutable double                         m_particleMass;
   //------------ Magnetic field properties
   bool m_fastField;
   Trk::MagneticFieldProperties m_fieldProperties;
  };

inline const TrackingGeometry* TimedExtrapolator::trackingGeometry() const 
 { 
   if (m_navigator) return m_navigator->trackingGeometry();
   return 0;
 }

 
inline const IPropagator* TimedExtrapolator::subPropagator(const Trk::TrackingVolume& tvol) const
{
  const IPropagator* currentPropagator = (tvol.geometrySignature() < m_subPropagators.size()) ?
    m_subPropagators[tvol.geometrySignature()] : 0;
  if (!currentPropagator)
      msg(MSG::ERROR) << "[!] Configuration problem: no Propagator found for volumeSignature: " << tvol.geometrySignature() << endreq;
  return currentPropagator;         
}

                                       
inline const ITimedMatEffUpdator* TimedExtrapolator::subMaterialEffectsUpdator(const Trk::TrackingVolume& tvol) const
{
  return (tvol.geometrySignature() < m_subUpdators.size()) ?
    m_subUpdators[tvol.geometrySignature()] : 0;
}


inline void TimedExtrapolator::throwIntoGarbageBin(const Trk::TrackParameters* pars) const
{ if (pars) m_garbageBin[pars] = true; }

/*
inline unsigned int TimedExtrapolator::geoIDToDetOrder(Trk::GeometrySignature geoid) const
{
  if ( geoid == Trk::ID ) return 0;
  else if ( geoid == Trk::Calo ) return 1;
  else if ( geoid == Trk::MS ) return 2;
  
  return 0; 
}
*/

} // end of namespace


#endif // TRKEXTOOLS_TIMEDEXTRAPOLATOR_H

