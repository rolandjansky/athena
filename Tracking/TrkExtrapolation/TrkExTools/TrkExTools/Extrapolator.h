/*
   Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
 */

///////////////////////////////////////////////////////////////////
// Extrapolator.h, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

#ifndef TRKEXTOOLS_EXTRAPOLATOR_H
#define TRKEXTOOLS_EXTRAPOLATOR_H

// Gaudi/StoreGate
#include "AthenaBaseComps/AthAlgTool.h"
#include "GaudiKernel/ToolHandle.h"
#include "GaudiKernel/EventContext.h"
#include "StoreGate/ReadCondHandleKey.h"

// Trk
#include "TrkExInterfaces/IExtrapolator.h"
#include "TrkExInterfaces/INavigator.h"
#include "TrkExInterfaces/IMaterialEffectsUpdator.h"
#include "TrkEventPrimitives/PropDirection.h"
#include "TrkEventPrimitives/ParticleHypothesis.h"
#include "TrkParameters/TrackParameters.h"
#include "TrkNeutralParameters/NeutralParameters.h"
#include "TrkSurfaces/BoundaryCheck.h"
#include "TrkSurfaces/PlaneSurface.h"
#include "TrkVolumes/BoundarySurface.h"
#include "TrkVolumes/BoundarySurfaceFace.h"
#include "TrkGeometry/TrackingVolume.h"
#include "TrkGeometry/MagneticFieldProperties.h"
#include "TrkDetDescrUtils/GeometrySignature.h"
#include "TrkExUtils/ExtrapolationCache.h"
// STL
#include <cstring>
#include <vector>
#include <map>
// Amg
#include "GeoPrimitives/GeoPrimitives.h"
#include "EventPrimitives/EventPrimitives.h"
// xAOD
#include "xAODTracking/TrackParticle.h"
#include "xAODTracking/NeutralParticle.h"

#include <Gaudi/Accumulators.h>
#include "ObjContainer.h"

class MsgStream;
namespace Trk {
class Track;
class Surface;
class Layer;
class Volume;
class DetachedTrackingVolume;
class TrackingGeometry;
class TrackParticleBase;
class IPropagator;
class IDynamicLayerCreator;
class INavigator;
class IMultipleScatteringUpdator;
class IEnergyLossUpdator;
class IPropagator;
class AlignableTrackingVolume;
class ExtrapolationCache;

typedef std::vector<const Trk::TrackParameters*> TrackParametersVector;
typedef std::vector<std::unique_ptr<const Trk::TrackParameters> > TrackParametersUVector;
typedef std::pair< const Surface*,BoundaryCheck  > DestSurf;

using TrackParmContainer  = ObjContainer<const Trk::TrackParameters>;
using TrackParmPtr        = ObjRef<>;
using ManagedTrackParmPtr = ObjPtr<const Trk::TrackParameters>;

/** @struct ParametersAtBoundarySurface
  has only three member
  - BoundarySurface
  - TrackParameters
  - bool that indicated the deletion of the TrackParameters
  */
struct ParametersNextVolume {
  //!< the members
  const TrackingVolume*    nextVolume;
  ManagedTrackParmPtr      nextParameters;
  ManagedTrackParmPtr      navParameters;
  BoundarySurfaceFace      exitFace;

  ParametersNextVolume(TrackParmContainer &track_parm_container)
     : nextParameters(track_parm_container),
       navParameters(track_parm_container)
  {
    nextVolume              = nullptr;
    exitFace                = undefinedFace;
  }

  //!< update the boundaryInformation
  void boundaryInformation(const TrackingVolume* tvol,
                           ManagedTrackParmPtr   nextPars,
                           ManagedTrackParmPtr   navPars,
                           BoundarySurfaceFace   face=undefinedFace)
  {
    nextVolume       = tvol;
    nextParameters   = nextPars;
    navParameters    = navPars;
    exitFace         = face;
  }
  //!< reset the boundary information by invalidating it
  void resetBoundaryInformation(){
    nextVolume       = nullptr;
    exitFace         = undefinedFace;
    nextParameters   = ManagedTrackParmPtr();
    navParameters    = ManagedTrackParmPtr();
  }
};

/** 
  @class Extrapolator

  The Extrapolator can be used in different setups:

  - <b>Strategy-pattern</b> usage

  - <b>(Simple) Configured AlgTool</b> usage

  - <b>(Full) Configured AlgTool</b> usage


  This design allows external clients to choose their own propagation logic, 
  but also ensures an easy to use AlgTool for users

  The output level is as follows:
INFO    : initialize / finalize information
DEBUG   : Method call sequence
VERBOSE : Method call sequence with values

@author Andreas.Salzburger@cern.ch
*/

  class Extrapolator : public AthAlgTool,
  virtual public IExtrapolator {
  public:
    /**Constructor */
    Extrapolator(const std::string&,const std::string&,const IInterface*);
    /**Destructor*/
    ~Extrapolator();

    /** AlgTool initailize method.
      In this method the extrapolator should retrieve the Propagator of highest order which is then passed through
      the extrapolate method. The Propagator itself should be specified whether to use propagators of a lower hirarchy
      level or not.
      */
    virtual StatusCode initialize() override;
    /** AlgTool finalize method */
    virtual StatusCode finalize() override;

    /** [xAOD] interface ------------------------------------------------------------------ */

    /** xAOD 0) neutral xAOD particle */
    virtual const NeutralParameters* extrapolate(const xAOD::NeutralParticle& xnParticle,
                                                 const Surface& sf,
                                                 PropDirection dir=anyDirection,
                                                 const BoundaryCheck&  bcheck = true) const override final;



    /** xAOD 0) neutral xAOD particle */
    virtual const TrackParameters* extrapolate(const xAOD::TrackParticle& particleBase,
                                               const Surface& sf,
                                               PropDirection dir=anyDirection,
                                               const BoundaryCheck&  bcheck = true,
                                               ParticleHypothesis particle=pion,
                                               MaterialUpdateMode matupmode=addNoise) const override final;


    /** [NeutralParameters] ------------------------------------------------------------- */

    /** N 0) <b>Neutral parameters method </b>
      - returns a ParametersBase object as well, 0 if the extrapolation did not succeed
      */

    virtual const NeutralParameters* extrapolate(const NeutralParameters& parameters,
                                                 const Surface& sf,
                                                 PropDirection dir=anyDirection,
                                                 const BoundaryCheck&  bcheck=true) const override final;

    /** [TrackParameters] --------------------------------------------------------------- */

    /** S 1) <b>Strategy Pattern extrapolation method</b>
      - returns the TrackParameters at the Destination Surface (if extrapolation succeeds),
      0 if extrapolation to destination surface does not suceed */
    virtual const TrackParameters*  extrapolate(const IPropagator& prop,
                                                const TrackParameters& parm,
                                                const Surface& sf,
                                                PropDirection dir=anyDirection,
                                                const BoundaryCheck&  bcheck = true,
                                                ParticleHypothesis particle=pion,
                                                MaterialUpdateMode matupmode=addNoise) const override final;

    /** S 2) <b>Strategy Pattern extrapolation method</b>
      - returns a vector of TrackParameters representing the tracking detector elements
      hit in between and the TrackParameters at the destination Surface (if final extrapolation suceeds),
      empty if the extrapolation to the destination surface does not suceed*/
    virtual TrackParametersUVector extrapolateStepwise(const IPropagator& prop,
                                                       const TrackParameters& parm,
                                                       const Surface& sf,
                                                       PropDirection dir=anyDirection,
                                                       const BoundaryCheck&  bcheck = true,
                                                       ParticleHypothesis particle=pion ) const override final;

    /** S 3) <b>Strategy Pattern extrapolation method</b>: 
      - searches the closest TrackParameters of the Track to the destination Surface
      - returns the TrackParameters at the Destination Surface (if extrapolation succeeds),
      0 if extrapolation to destination surface does not suceed */
    virtual const TrackParameters* extrapolate(const IPropagator& prop,
                                               const Track& trk,
                                               const Surface& sf,
                                               PropDirection dir=anyDirection,
                                               const BoundaryCheck&  bcheck = true,
                                               ParticleHypothesis particle=pion,
                                               MaterialUpdateMode matupmode=addNoise) const override final;

    /** S 4) <b>Strategy Pattern extrapolation method</b>:
      - direct extrapolation to the destination surface, no material effects 
      or intermediate steps are taken into account
      */           
    virtual const TrackParameters* extrapolateDirectly(const IPropagator& prop,
                                                       const TrackParameters& parm,
                                                       const Surface& sf,
                                                       PropDirection dir=anyDirection,
                                                       const BoundaryCheck&  bcheck = true,
                                                       ParticleHypothesis particle=pion) const override final;

    /** S 5) <b>Strategy Pattern extrapolation method</b>:
      - blind extrapolation inside the given TrackingVolume
    */          
    virtual TrackParametersUVector extrapolateBlindly(const IPropagator& prop,
                                                      const TrackParameters& parm,
                                                      PropDirection dir,
                                                      const BoundaryCheck&  bcheck,
                                                      ParticleHypothesis particle=pion,
                                                      const Volume* boundaryVol=nullptr) const override final;


    /** S 6) <b>Strategy Pattern extrapolation method</b>:
      - extrapolation to the next active layer, based on the extrapolation to the next (material) layer
      and layer identification
      */          
    virtual std::pair<const TrackParameters*,const Layer*> extrapolateToNextActiveLayer(const IPropagator& prop,
                                                                                        const TrackParameters& parm,
                                                                                        PropDirection dir,
                                                                                        const BoundaryCheck&  bcheck,
                                                                                        ParticleHypothesis particle=pion,
                                                                                        MaterialUpdateMode matupmode=addNoise) const override final;

    /** S 7) <b>Strategy Pattern extrapolation method</b>:
      - extrapolation to the next active layer, based on the extrapolation to the next (material) layer
      and layer identification
      */          

    virtual std::pair<const TrackParameters*,const Layer*> extrapolateToNextActiveLayerM(const IPropagator& prop,
                                                                                         const TrackParameters& parm,
                                                                                         PropDirection dir,
                                                                                         const BoundaryCheck&  bcheck,
                                                                                         std::vector<const Trk::TrackStateOnSurface*>& material,
                                                                                         ParticleHypothesis particle=pion,
                                                                                         MaterialUpdateMode matupmode=addNoise) const override final;

    /** S 8) <b>Strategy Pattern extrapolation method</b>:
      - Extrapolation using specific intermediate surfaces and energy loss effects to be accounted for at
      each surface as specified by the corresponding MaterialEffectsOnTrack
      -Final boolean only relevant if LandauMode = true for the configured MaterialEffectsUpdator
      */                                                                           
    virtual const TrackParameters* extrapolate(const IPropagator& prop,
                                               const TrackParameters& parm,
                                               const std::vector< MaterialEffectsOnTrack >& sfMeff,
                                               const TrackingVolume& tvol,
                                               PropDirection dir,
                                               ParticleHypothesis particle=pion,
                                               MaterialUpdateMode matupmode=addNoise) const override final;

    /** S 9) <b>Strategy Pattern extrapolation method</b>:
      - extrapolation to the next active layer, based on the extrapolation to the next (material) layer
      and layer identification
      */          
    virtual std::pair<const TrackParameters*,const Layer*> extrapolateToNextStation (const IPropagator& prop,
                                                                                     const TrackParameters& parm,
                                                                                     PropDirection dir,
                                                                                     const BoundaryCheck&  bcheck,
                                                                                     ParticleHypothesis particle=pion,
                                                                                     MaterialUpdateMode matupmode=addNoise) const override final; 

    /** S 10) <b>Strategy Pattern extrapolation method</b>:
      - extrapolation to a volume boundary of an arbitrary tracking volume ( not necessarily part of a tracking geometry ) 
      */          
    virtual const TrackParameters* extrapolateToVolume(const IPropagator& prop,
                                                       const TrackParameters& parm,
                                                       const Trk::TrackingVolume& vol,
                                                       PropDirection dir=anyDirection,
                                                       ParticleHypothesis particle=pion) const override final;

    /** C 1) <b>Configured AlgTool extrapolation method</b> of S 1):*/
    virtual const TrackParameters* extrapolate(const TrackParameters& parm,
                                               const Surface& sf,
                                               PropDirection dir=anyDirection,
                                               const BoundaryCheck&  bcheck = true,
                                               ParticleHypothesis particle=pion,
                                               MaterialUpdateMode matupmode=addNoise,
                                               Trk::ExtrapolationCache* cache = nullptr) const override final;

    /** C 2) <b>Configured AlgTool extrapolation method</b> of S 2):*/
    virtual TrackParametersUVector extrapolateStepwise(const TrackParameters& parm,
                                                       const Surface& sf,
                                                       PropDirection dir=anyDirection,
                                                       const BoundaryCheck&  bcheck = true,
                                                       ParticleHypothesis particle=pion) const override final;

    /** C 3) <b>Configured AlgTool extrapolation method</b> of S 3):*/
    virtual const TrackParameters* extrapolate(const Track& trk,
                                               const Surface& sf,
                                               PropDirection dir=anyDirection,
                                               const BoundaryCheck&  bcheck = true,
                                               ParticleHypothesis particle=pion,
                                               MaterialUpdateMode matupmode=addNoise,
                                               Trk::ExtrapolationCache* cache = nullptr) const override final;

    /** C 4) <b>Configured AlgTool extrapolation method</b> of S 4):*/
    virtual const TrackParameters* extrapolateDirectly(const TrackParameters& parm,
                                                       const Surface& sf,
                                                       PropDirection dir=anyDirection,
                                                       const BoundaryCheck&  bcheck = true,
                                                       ParticleHypothesis particle=pion) const override final;

    /** C 5) <b>Configured AlgTool extrapolation method</b> of S 5):*/                        
    virtual TrackParametersUVector extrapolateBlindly(const TrackParameters& parm,
                                                      PropDirection dir=anyDirection,
                                                      const BoundaryCheck&  bcheck = true,
                                                      ParticleHypothesis particle=pion,
                                                      const Volume* boundaryVol=nullptr) const override final;


    /** C 6) <b>Configured AlgTool extrapolation method</b> of S 6):*/                        
    virtual std::pair<const TrackParameters*,const Layer*> extrapolateToNextActiveLayer(const TrackParameters& parm,
                                                                                        PropDirection dir=anyDirection,
                                                                                        const BoundaryCheck&  bcheck = true,
                                                                                        ParticleHypothesis particle=pion,
                                                                                        MaterialUpdateMode matupmode=addNoise) const override final;

    /** C 7) <b>Configured AlgTool extrapolation method</b> of S 7):*/                        
    virtual std::pair<const TrackParameters*,const Layer*> extrapolateToNextActiveLayerM(const TrackParameters& parm,
                                                                                         PropDirection dir,
                                                                                         const BoundaryCheck&  bcheck,
                                                                                         std::vector<const Trk::TrackStateOnSurface*>& material,
                                                                                         ParticleHypothesis particle=pion,
                                                                                         MaterialUpdateMode matupmode=addNoise) const override final;

    /** C 8) <b>Configured AlgTool extrapolation method</b> of S 8):*/                        
    virtual std::pair<const TrackParameters*,const Layer*> extrapolateToNextStation(const TrackParameters& parm,
                                                                                    PropDirection dir=anyDirection,
                                                                                    const BoundaryCheck&  bcheck = true,
                                                                                    ParticleHypothesis particle=pion,
                                                                                    MaterialUpdateMode matupmode=addNoise) const override final;

    /** C 9) <b>Configured AlgTool extrapolation method</b> of S 9):*/
    virtual const TrackParameters* extrapolateToVolume(const TrackParameters& parm,
                                                       const Trk::TrackingVolume& vol,
                                                       PropDirection dir=anyDirection,
                                                       ParticleHypothesis particle=pion) const override final;

    /** C 10) <b>Configured AlgTool extrapolation method</b>:
      - Extrapolate to a destination surface, while collecting all the material layers in between.
      */
    virtual std::vector<const TrackStateOnSurface*>* extrapolateM(const TrackParameters& parameters,
                                                                  const Surface& sf,
                                                                  PropDirection dir,
                                                                  const BoundaryCheck&  bcheck,
                                                                  ParticleHypothesis particle=pion,
                                                                  Trk::ExtrapolationCache* cache = nullptr) const override final;

    /** C 11) <b>Configured AlgTool extrapolation method</b>:
      - Extrapolate to a destination surface, while collecting all the material layers and transport jacobians in between.
      */
    virtual std::vector<const TrackParameters*>* extrapolateM(const TrackParameters& parameters,
                                                              const Surface& sf,
                                                              PropDirection dir,
                                                              const BoundaryCheck&  bcheck,
                                                              std::vector<MaterialEffectsOnTrack>& material,
                                                              std::vector<Trk::TransportJacobian *>& jacs,
                                                              ParticleHypothesis particle=pion,
                                                              Trk::ExtrapolationCache* cache = nullptr) const override final;

    virtual const Trk::TrackParameters*  extrapolateWithPathLimit(
                                                                  const Trk::TrackParameters& parm,
                                                                  double& pathLim,
                                                                  Trk::PropDirection dir,
                                                                  Trk::ParticleHypothesis particle,
                                                                  std::vector<const Trk::TrackParameters*>*& parmOnSf,
                                                                  std::vector<const Trk::TrackStateOnSurface*>*& material,
                                                                  const Trk::TrackingVolume* boundaryVol=nullptr,
                                                                  MaterialUpdateMode matupmod=Trk::addNoise) const override final;


    /** extrapolation method collecting intersections with subdetector boundaries and active volumes/layers.
      A primitive identification is provided - to be replaced with appropriate identifier, and possibly merged
      with TrackParameters. Material collection in option. Destination (subdetector boundary) : geoID (+ entry, -exit)
      ( default MS exit )
      */
    virtual const std::vector< std::pair< const Trk::TrackParameters*, int > >*  extrapolate(const Trk::TrackParameters& parm,
                                                                                             Trk::PropDirection dir,
                                                                                             Trk::ParticleHypothesis particle,
                                                                                             std::vector<const Trk::TrackStateOnSurface*>*&  material,
                                                                                             int destination = 3 ) const override final;



    /** Return the TrackingGeometry used by the Extrapolator (forward information from Navigator)*/
    virtual const TrackingGeometry* trackingGeometry() const override final; 

    /** Validation Action:
      Can be implemented optionally, outside access to internal validation steps */
    virtual void validationAction() const override final;

  private:
    ServiceHandle<IIncidentSvc> m_incidentSvc{this, "IncidentService", "IncidentSvc"};
    /*
     * Cache to be passed to and between the private methods
     */
    typedef std::vector< std::pair< const Trk::TrackParameters*, int > > identifiedParameters_t;
    struct Cache{
      unsigned int                                             m_methodSequence = 0;

      bool                                                     m_dense=false;                  //!<  internal switch for resolved configuration  
      bool                                                     m_recall=false;                 //!< Flag the recall solution
      bool                                                     m_robustSampling=true; 
      unsigned int                                             m_layerResolved{};  

      const Surface*                                           m_destinationSurface=nullptr;                    
      const Volume*                                            m_boundaryVolume=nullptr;      //!< the boundary volume check
      const Surface*                                           m_recallSurface=nullptr;       //!< Destination Surface for recall
      const Layer*                                             m_recallLayer=nullptr;         //!< Destination Layer for recall
      const TrackingVolume*                                    m_recallTrackingVolume=nullptr;     //!< Destination TrackingVolume for recall
      const Trk::TrackingVolume*                               m_currentStatic=nullptr;
      const Trk::TrackingVolume*                               m_currentDense=nullptr;
      const Trk::TrackingVolume*                               m_highestVolume=nullptr;
      TrackParametersVector*                                   m_parametersOnDetElements=nullptr;  //!< return helper for parameters on detector elements 
      TrackParmContainer                                       m_trackParmContainer;
      ManagedTrackParmPtr                                      m_lastValidParameters;              //!< parameters to be used for final propagation in case of fallback 
      const Layer*                                             m_lastMaterialLayer=nullptr;        //!< cache layer with last material update 
      Trk::ExtrapolationCache*                                 m_extrapolationCache=nullptr;       //!< cache for collecting the total X0 ans Eloss 
      const Trk::EnergyLoss*                                   m_cacheEloss=nullptr;               //!< cache pointer for Eloss
      std::vector<const Trk::TrackStateOnSurface*>*            m_matstates=nullptr;                //!< cache of TrackStateOnSurfaces
      std::vector<Trk::TransportJacobian*>*                    m_jacs=nullptr;                     //!< cache of Transport Jacobians 
      std::unique_ptr<identifiedParameters_t>                  m_identifiedParameters;             // for active volumes 

      double                                                   m_path{};    

      std::pair<unsigned int, unsigned int>                     m_denseResolved;
      std::vector<DestSurf>                                     m_staticBoundaries;
      std::vector<DestSurf>                                     m_detachedBoundaries;
      std::vector<DestSurf>                                     m_denseBoundaries;
      std::vector<DestSurf>                                     m_navigBoundaries;
      std::vector<DestSurf>                                     m_layers;
      std::vector<std::pair<const Trk::DetachedTrackingVolume*,unsigned int> >    m_detachedVols;
      std::vector<std::pair<const Trk::TrackingVolume*,unsigned int> >            m_denseVols;
      std::vector<std::pair<const Trk::TrackingVolume*,const Trk::Layer*> >       m_navigLays; 
      std::vector<std::pair<const Trk::Surface*,Trk::BoundaryCheck> >             m_navigSurfs;
      std::vector<const Trk::DetachedTrackingVolume*>                  m_navigVols;
      std::vector<std::pair<const Trk::TrackingVolume*,unsigned int> > m_navigVolsInt;

      std::map<const Trk::TrackParameters*, bool>              m_garbageBin;           //!< garbage collection during extrapolation
      ParametersNextVolume                                     m_parametersAtBoundary; //!< return helper for parameters and boundary

      bool                                                     m_ownParametersOnDetElements = true;
      TrackParmContainer &trackParmContainer() { return m_trackParmContainer; }

      ManagedTrackParmPtr manage(const Trk::TrackParameters &parm) { return ManagedTrackParmPtr(trackParmContainer(),parm); }
      ManagedTrackParmPtr manage(const Trk::TrackParameters *parm) { return ManagedTrackParmPtr(trackParmContainer(),parm); }
      ManagedTrackParmPtr manage(TrackParmPtr parm)                { return ManagedTrackParmPtr(trackParmContainer(),parm); }
      ManagedTrackParmPtr manage()                                 { return ManagedTrackParmPtr(trackParmContainer()); }

      Cache()
         : m_trackParmContainer(128), // always reserve some space; still occasionally more slots are needed; above 150 there are very few cases the max in q431 was 257
           m_lastValidParameters(m_trackParmContainer),
           m_parametersAtBoundary(m_trackParmContainer)
      {
        m_navigSurfs.reserve(1000);
        m_navigVols.reserve(50);
        m_navigVolsInt.reserve(50);
      }
      ~Cache() {
         s_navigSurfsMax.update(m_navigSurfs.size());
         s_navigVolsMax.update(m_navigVols.size()) ;
         s_navigVolsIntMax.update(m_navigVols.size());
         if (m_ownParametersOnDetElements && m_parametersOnDetElements) {
            for (const Trk::TrackParameters *parm : *m_parametersOnDetElements) {
               delete parm;
            }
         }
         s_containerSizeMax.update(trackParmContainer().size());
      }

      class AtomicMax {
      public:
         void update(size_t val) {
            while (val>m_maxVal) {
               val = m_maxVal.exchange(val);
            }
         }
         size_t val() const { return m_maxVal; }
         std::atomic<size_t> m_maxVal = 0;
      };
      static AtomicMax s_navigSurfsMax    ATLAS_THREAD_SAFE;
      static AtomicMax s_navigVolsMax     ATLAS_THREAD_SAFE;
      static AtomicMax s_navigVolsIntMax  ATLAS_THREAD_SAFE;
      static AtomicMax s_containerSizeMax ATLAS_THREAD_SAFE;
      static bool      s_reported         ATLAS_THREAD_SAFE;

    };

    /**
     * Actual heavy lifting implementation for 
     * S 1) <b>Strategy Pattern extrapolation method</b>
     * - returns the TrackParameters at the Destination Surface (if extrapolation succeeds),
     *   0 if extrapolation to destination surface does not suceed 
     */
    ManagedTrackParmPtr  extrapolateImpl(Cache& cache,
                                  const IPropagator& prop,
                                  TrackParmPtr parm,
                                  const Surface& sf,
                                  PropDirection dir=anyDirection,
                                  const BoundaryCheck&  bcheck = true,
                                  ParticleHypothesis particle=pion,
                                  MaterialUpdateMode matupmode=addNoise) const ;
    /**
     * Actual heavy lifting implementation for 
     * S 8) <b>Strategy Pattern extrapolation method</b>:
     * - Extrapolation using specific intermediate surfaces and energy loss effects to be accounted for at
     *   each surface as specified by the corresponding MaterialEffectsOnTrack
     *   -Final boolean only relevant if LandauMode = true for the configured MaterialEffectsUpdator
     */                                                                           
    ManagedTrackParmPtr extrapolateImpl(Cache& cache,
                                 const IPropagator& prop,
                                 TrackParmPtr parm,
                                 const std::vector< MaterialEffectsOnTrack >& sfMeff,
                                 const TrackingVolume& tvol,
                                 PropDirection dir,
                                 ParticleHypothesis particle=pion,
                                 MaterialUpdateMode matupmode=addNoise) const ;


    /** Actual heavy lifting implementation for  
     * C 1) <b>Configured AlgTool extrapolation method</b> of S 1):
     * */
    virtual ManagedTrackParmPtr extrapolateImpl(Cache& cache,
                                         TrackParmPtr parm,
                                         const Surface& sf,
                                         PropDirection dir=anyDirection,
                                         const BoundaryCheck&  bcheck = true,
                                         ParticleHypothesis particle=pion,
                                         MaterialUpdateMode matupmode=addNoise,
                                         Trk::ExtrapolationCache* extrapolationCache = nullptr) const;


    /** Actual heavy lifting implementation for  
     * C 5) <b>Configured AlgTool extrapolation method</b> of S 5):*/
    Trk::TrackParametersUVector extrapolateBlindlyImpl(Cache& cache,
                                                       const IPropagator &prop, 
                                                       TrackParmPtr parm,
                                                       PropDirection dir=anyDirection,
                                                       const BoundaryCheck&  bcheck = true,
                                                       ParticleHypothesis particle=pion,
                                                       const Volume* boundaryVol=nullptr) const;



    /** Private method for extrapolation in final volume to destination surface
      - Parameters are: IPropagator& prop            ... propagator to be used
      TrackParameters& parm        ... starting parameters
      Surface& sf                  ... destination surface
      TrackingVolume&              ... the initial volume
      Layer* associatedLayer       ... layer associatiated with starting parameters (steers postupdate)
      PropDirection dir            ... propagation direction
      const BoundaryCheck&  bcheck         ... boolean for bounday check
      ParticleHypothesis  particle ... the particle hypothesis
      std::vector<const TrackParameters*>* dethits ... for blind extrapolation     

      it will call:
      - A) insideVolumeStaticLayers() for a TrackingVolume with static layers
      - C) insideVolumeDetachedVolumes() for a TrackingVolume with detached inner Volumes
      */
    ManagedTrackParmPtr extrapolateInsideVolume(Cache& cache,
                                                   const IPropagator& prop,
                                                   TrackParmPtr parm,
                                                   const Surface& sf,
                                                   const Layer* associatedLayer,
                                                   const TrackingVolume& tvol,
                                                   PropDirection dir = anyDirection,
                                                   const BoundaryCheck&  bcheck = true,
                                                   ParticleHypothesis particle=pion,
                                                   MaterialUpdateMode matupmode=addNoise) const;



    /** A) call from extrapolateInsideVolume or toBoundary,
      if it is to boundary, the return parameters are the parameters at the boundary
      */
    ManagedTrackParmPtr insideVolumeStaticLayers(Cache& cache,
                                                    bool toBoundary,
                                                    const IPropagator& prop,
                                                    TrackParmPtr parm,
                                                    const Layer* associatedLayer,
                                                    const TrackingVolume& tvol,
                                                    PropDirection dir = anyDirection,
                                                    const BoundaryCheck&  bcheck = true,
                                                    ParticleHypothesis particle=pion,
                                                    MaterialUpdateMode matupmode=addNoise) const;

    /** C) call from extrapolateInsideVolume */
    ManagedTrackParmPtr extrapolateWithinDetachedVolumes(Cache& cache,
                                                            const IPropagator& prop,
                                                            TrackParmPtr parm,
                                                            const Surface& sf,
                                                            const TrackingVolume& tvol,
                                                            PropDirection dir = anyDirection,
                                                            const BoundaryCheck&  bcheck = true,
                                                            ParticleHypothesis particle=pion,
                                                            MaterialUpdateMode matupmode=addNoise) const;

    ManagedTrackParmPtr extrapolateToNextMaterialLayer(Cache& cache,
                                                          const IPropagator& prop,
                                                          TrackParmPtr parm,
                                                          const Trk::Surface* destSurf, 
                                                          const Trk::TrackingVolume* vol,
                                                          PropDirection dir,
                                                          const BoundaryCheck&  bcheck,
                                                          ParticleHypothesis particle=pion,
                                                          MaterialUpdateMode matupmode=addNoise) const;

    ManagedTrackParmPtr extrapolateInAlignableTV(Cache& cache,
                                                    const IPropagator& prop,
                                                    TrackParmPtr parm,
                                                    const Trk::Surface* destSurf, 
                                                    const Trk::AlignableTrackingVolume* vol,
                                                    PropDirection dir,
                                                    ParticleHypothesis particle=pion) const;

    ManagedTrackParmPtr  extrapolateToVolumeWithPathLimit(Cache& cache,
                                                                  TrackParmPtr parm,
                                                                  double pathLim,
                                                                  Trk::PropDirection dir,
                                                                  Trk::ParticleHypothesis particle,
                                                                  const Trk::TrackingVolume* destVol,
                                                                  MaterialUpdateMode matupmod=addNoise) const;


    /** Private method for extrapolation in intermediate volume to boundary surface
      - Parameters are: IPropagator& prop            ... propagator to be used
      TrackParameters& parm        ... starting parameters
      Surface& sf                  ... destination surface
      TrackingVolume&              ... the initial volume
      Layer* associatedLayer       ... layer associatiated with starting parameters (steers postupdate)
      PropDirection dir            ... propagation direction
      const BoundaryCheck&  bcheck         ... boolean for bounday check
      ParticleHypothesis  particle ... the particle hypothesis
      std::vector<const TrackParameters*>* dethits ... for blind extrapolation

      it will call:
      - A) toVolumeBoundaryStaticLayers() for a TrackingVolume with static layers
      - C) toVolumeBoundaryDetachedVolumes() for a TrackingVolume with detached inner Volumes
      */
    void extrapolateToVolumeBoundary(Cache& cache,
                                     const IPropagator& prop,
                                     TrackParmPtr parm,
                                     const Layer* associatedLayer,
                                     const TrackingVolume& tvol,
                                     PropDirection dir = anyDirection,
                                     const BoundaryCheck&  bcheck = true,
                                     ParticleHypothesis particle=pion,
                                     MaterialUpdateMode matupmode=addNoise
                                    ) const;

    /** Private method to step from one to the last 
      layer and stop at last layer (before 0) or before destination layer */
    ManagedTrackParmPtr extrapolateFromLayerToLayer(Cache& cache,
                                                       const IPropagator& prop,
                                                       TrackParmPtr parm,
                                                       const TrackingVolume& tvol,
                                                       const Layer* nextLayer,
                                                       const Layer* destinationLayer,
                                                       TrackParmPtr navParameters,
                                                       PropDirection dir = anyDirection,
                                                       const BoundaryCheck&  bcheck = true,
                                                       ParticleHypothesis particle=pion,
                                                       MaterialUpdateMode matupmode=addNoise
                                                      ) const;

    /** Private to extrapolate to the destination layer + surface
    */
    ManagedTrackParmPtr extrapolateToDestinationLayer(Cache& cache,
                                                         const IPropagator& prop,
                                                         TrackParmPtr parm,
                                                         const Surface& sf,
                                                         const Layer& lay,
                                                         const TrackingVolume& tvol,
                                                         const Layer* startLayer,
                                                         PropDirection dir = anyDirection,
                                                         const BoundaryCheck&  bcheck = true,
                                                         ParticleHypothesis particle=pion,
                                                         MaterialUpdateMode matupmode=addNoise
                                                        ) const;

    /** Private to extrapolate to the destination layer + surface, special treatment for exit layer
     * @return valid track parameters or nullptr, as first element and in case of nullptr as second element
     *         true to indicate to kill the loop from material update(?)
    */
   std::pair<ManagedTrackParmPtr,bool>
      extrapolateToIntermediateLayer(Cache& cache,
                                     const IPropagator& prop,
                                     TrackParmPtr parm,
                                     const Layer& lay,
                                     const TrackingVolume& tvol,
                                     PropDirection dir = anyDirection,
                                     const BoundaryCheck&  bcheck = true,
                                     ParticleHypothesis particle=pion,
                                     MaterialUpdateMode matupmode=addNoise,
                                     bool perpendicularCheck = true) const;

    /** Private to search for overlap surfaces */
    void overlapSearch(Cache& cache,
                       const IPropagator& prop,
                       TrackParmPtr parm,
                       TrackParmPtr parsOnLayer,
                       const Layer& lay,
                       const TrackingVolume& tvol,
                       PropDirection dir = anyDirection,
                       const BoundaryCheck&  bcheck = true,
                       ParticleHypothesis particle=pion,
                       bool startingLayer = false) const;


    /** Private method for chosing the propagator in the simple/full configured mode */
    unsigned int propagatorType(const TrackingVolume& tvol) const;

    /** Private method for Initial Extrapolation setup 
      -> overwrites the given pointers for the start and destination parameters
      -> returns a direction for the Navigation :
      - usually dir if dir @f$ \in  @f$ [ Trk::alongMomentum, Trk::oppositeMomentum ] 
      - a chosen one if dir == Trk::anyDirection
      */
    PropDirection initializeNavigation(Cache& cache,
                                       const Trk::IPropagator&             prop,
                                       TrackParmPtr         startPars,
                                       const Trk::Surface&                 destSurface,
                                       Trk::PropDirection                  dir,
                                       ParticleHypothesis                  particle, 
                                       ManagedTrackParmPtr&                referenceParameters,
                                       const Trk::Layer*&                  associatedLayer,
                                       const Trk::TrackingVolume*&         associatedVolume,
                                       const Trk::TrackingVolume*&         destinationVolume) const;

    /** RadialDirection helper method 
      - inbound : -1
      - outbound:  1  */
    int radialDirection(const Trk::TrackParameters& pars, PropDirection dir) const;                         

    /** Check for punchThrough in case of radial (perpendicular) direction change,
      returns true if the radial direction change is actually ok (i.e. punch-through allowed)
      */
    bool radialDirectionCheck(const IPropagator& prop,
                              const TrackParameters& startParm,
                              const TrackParameters& parsOnLayer,
                              const TrackingVolume& tvol,
                              PropDirection dir = anyDirection,
                              ParticleHypothesis particle=pion) const;

    /** Access the subPropagator to the given volume*/
    virtual const IPropagator* subPropagator(const TrackingVolume& tvol) const override;

    /** Access the subPropagator to the given volume*/
    const IMaterialEffectsUpdator* subMaterialEffectsUpdator(const TrackingVolume& tvol) const;

    /** Private method for setting recall Information */
    void setRecallInformation(Cache& cache,const Surface&, const Layer&, const TrackingVolume&) const;    

    /** Private method for resetting the recallInformation */
    void resetRecallInformation(Cache& cache) const;

    /** Private method for throwing into the GarbageBin */
    void throwIntoGarbageBin(Cache& cache,const Trk::TrackParameters* garbage) const;

    /** Private method for emptying the GarbageBin */
    void emptyGarbageBin(Cache& cache) const;
    void emptyGarbageBin(Cache& cache, const Trk::TrackParameters*) const;

    /** Private method to return from extrapolate() main method,
        cleans up, calls model action or validation action, empties garbage bin and leaves */
    const Trk::TrackParameters *returnResult(Cache& cache,const Trk::TrackParameters* result) const;

    /** For the output - layer */
    std::string layerRZoutput(const Trk::Layer& lay) const;

    /** For the output - global position */
    std::string positionOutput(const Amg::Vector3D& pos) const;
    /** For the output - global momentum */
    std::string momentumOutput(const Amg::Vector3D& mom) const;

    /** helper method for MaterialEffectsOnTrack to be added */
    void addMaterialEffectsOnTrack(Cache& cache,
                                   const Trk::IPropagator& prop,
                                   TrackParmPtr parm,
                                   const Trk::Layer& lay,
                                   const Trk::TrackingVolume& vol,
                                   Trk::PropDirection propDir,
                                   Trk::ParticleHypothesis) const;

    void dumpCache(Cache& cache,const std::string& txt) const; 
    bool checkCache(Cache& cache,const std::string& txt) const;

    /** Private method for conversion of the synchronized geometry signature to the natural subdetector ordering */
    //unsigned int geoIDToDetOrder(Trk::GeometrySignature geoid) const;

    // --------------- Used Tools ----------------------------- //
    ToolHandleArray< IPropagator >              m_propagators;                   //!<  Array of Propagators
    ToolHandle< IPropagator >                   m_stepPropagator;                //!<  Array of Propagators
    ToolHandle< INavigator >                    m_navigator;                     //!<  Navigator for TrackingGeometry and magnetic fiels acces
    ToolHandleArray< IMaterialEffectsUpdator >  m_updators;                      //!<  Array of Material Updators
    ToolHandleArray< IMultipleScatteringUpdator >  m_msupdators;                 //!<  Array of MultipleScattering Updators
    ToolHandleArray< IEnergyLossUpdator >       m_elossupdators;                 //!<  Array of EnergyLoss Updators

    // ---------------- For Extrapolation handling ------------ //

    std::vector<const IPropagator*>             m_subPropagators;                //!< Propagators to chose from (steered by signature)
    std::vector<const IMaterialEffectsUpdator*> m_subUpdators;                   //!< Updators to chose from (steered by signature)

    // ---------------- For Extrapolator configuration ------------ //

    std::vector<std::string>                    m_propNames;                    //!<  configuration of subPropagators
    std::vector<std::string>                    m_updatNames;                   //!<  configuration of subUpdators

    // --------------- General steering & Navigation -------------- //

    bool                            m_includeMaterialEffects;        //!< boolean to switch on/off material effects
    bool                            m_requireMaterialDestinationHit; //!< require the destination surface hit for material collection
    bool                            m_stopWithNavigationBreak;       //!< return 0 if navigation breaks - for validation reasons
    bool                            m_stopWithUpdateZero;            //!< return 0 if update kills the trajectory 
    bool                            m_subSurfaceLevel;               //!< tep down to sub-surface level
    bool                            m_skipInitialLayerUpdate;        //!< skip the initial post-Update at the layer [Fatras conversion mode]
    bool                            m_extendedLayerSearch;           //!< extended layer search
    bool                            m_robustSampling;
    bool                            m_referenceMaterial;             //!< use the reference material for the update
    bool                            m_resolveActive;
    bool                            m_resolveMultilayers;
    bool                            m_cacheLastMatLayer;             //!< steering of the material layer cache  
    bool                            m_returnPassiveLayers;
    unsigned int                    m_meotpIndex;                    //!< if several meotps are available in a volume steer which one to use
    unsigned int                    m_configurationLevel;            //!< see the supported levels of configuration above
    unsigned int                    m_searchLevel;                   //!< see the supported search levels above
    unsigned int                    m_initialLayerAttempts;          //!< allowed layer intersection attempts at the start of a volume
    unsigned int                    m_successiveLayerAttempts;       //!< layer intersection attemps after one layer has been hit sucessfully
    unsigned int                    m_maxMethodSequence;
    double                          m_tolerance;                    //!< surfacen & volume tolerance
    // ------------------------------------------------------- //      
    bool                            m_activeOverlap;                 //!<  consider overlaps between active muon volumes  
    bool                            m_useMuonMatApprox;              //!<  use approximative MS inert material
    bool                            m_useDenseVolumeDescription;     //!<  use dense volume description when available in ID/Calo
    bool                            m_checkForCompundLayers;         //!<  use the multi-layer tests for compound layers
    
    unsigned int                    m_maxNavigSurf;
    unsigned int                    m_maxNavigVol;
    bool                            m_dumpCache;
    //------------ Magnetic field properties
    bool                                       m_fastField; 
    Trk::MagneticFieldProperties               m_fieldProperties;
    
    //------------Reference surface --------------
    
    Surface*                        m_referenceSurface;
   
    //-------------------------- SCREEN output steering -------------------------------------------//
    bool                            m_printHelpOutputAtInitialize;
    bool                            m_printRzOutput;
    //------------------------- VALIDATION  SECTION ------------------------------------------//
    //flags
    bool                            m_navigationStatistics;             //!< steer the output for the navigaiton statistics
    bool                            m_navigationBreakDetails;           //!< steer the output for the navigation break details
    bool                            m_materialEffectsOnTrackValidation; //!< mat effects on track validation
    //extrapolation counters

    mutable Gaudi::Accumulators::Counter<int>                     m_extrapolateCalls;              //!< number of calls: extrapolate() method
    mutable Gaudi::Accumulators::Counter<int>                     m_extrapolateBlindlyCalls;       //!< number of calls: extrapolateBlindly() method 
    mutable Gaudi::Accumulators::Counter<int>                     m_extrapolateDirectlyCalls;      //!< number of calls: extrapolateDirectly() method
    mutable Gaudi::Accumulators::Counter<int>                     m_extrapolateStepwiseCalls;      //!< number of calls: extrapolateStepwise() method

    mutable Gaudi::Accumulators::Counter<int>                     m_startThroughAssociation;        //!< navigation intialization
    mutable Gaudi::Accumulators::Counter<int>                     m_startThroughRecall;             //!< navigation intialization
    mutable Gaudi::Accumulators::Counter<int>                     m_startThroughGlobalSearch;       //!< navigation intialization
    mutable Gaudi::Accumulators::Counter<int>                     m_destinationThroughAssociation;  //!< navigation intialization
    mutable Gaudi::Accumulators::Counter<int>                     m_destinationThroughRecall;       //!< navigation intialization
    mutable Gaudi::Accumulators::Counter<int>                     m_destinationThroughGlobalSearch; //!< navigation intialization 
    mutable Gaudi::Accumulators::Counter<int>                     m_layerSwitched;                 //!< number of layers that have been switched 

    //navigation counters
    mutable Gaudi::Accumulators::Counter<int>                     m_navigationBreakLoop;            //!< number of navigation breaks due to loop
    mutable Gaudi::Accumulators::Counter<int>                     m_navigationBreakOscillation;     //!< number of navigation breaks due to oscillation
    mutable Gaudi::Accumulators::Counter<int>                     m_navigationBreakNoVolume;       //!< number of navigation breaks due no Volume found
    mutable Gaudi::Accumulators::Counter<int>                     m_navigationBreakDistIncrease;   //!< number of navigation breaks due to distance increase
    mutable Gaudi::Accumulators::Counter<int>                     m_navigationBreakVolumeSignature;   //!< number of navigation breaks due to distance increase
    mutable Gaudi::Accumulators::Counter<int>                     m_overlapSurfaceHit;             //!< number of OverlapSurfaces found

    mutable Gaudi::Accumulators::Counter<int>                     m_meotSearchCallsFw;                //!< how often the meot search is called: forward
    mutable Gaudi::Accumulators::Counter<int>                     m_meotSearchCallsBw;                //!< how often the meot search is called: backward
    mutable Gaudi::Accumulators::Counter<int>                     m_meotSearchSuccessfulFw;           //!< how often the meot search was successful: forward
    mutable Gaudi::Accumulators::Counter<int>                     m_meotSearchSuccessfulBw;           //!< how often the meot search was successful: backward
  };

inline const TrackingGeometry* Extrapolator::trackingGeometry() const 
{ 
  if (m_navigator) return m_navigator->trackingGeometry();
  return nullptr;
}

inline const IPropagator* Extrapolator::subPropagator(const Trk::TrackingVolume& tvol) const
{
  const IPropagator* currentPropagator = (tvol.geometrySignature() < m_subPropagators.size()) ?
    m_subPropagators[tvol.geometrySignature()] : nullptr;

  if (tvol.geometrySignature()==Trk::Calo && m_useDenseVolumeDescription) currentPropagator= (Trk::MS < m_subPropagators.size()) ?
    m_subPropagators[Trk::MS] : nullptr; 

  if (!currentPropagator)
    msg(MSG::ERROR) << "[!] Configuration problem: no Propagator found for volumeSignature: " << tvol.geometrySignature() << endmsg;
  return currentPropagator;         
}

inline const IMaterialEffectsUpdator* Extrapolator::subMaterialEffectsUpdator(const Trk::TrackingVolume& tvol) const
{
  return (tvol.geometrySignature() < m_subUpdators.size()) ?
    m_subUpdators[tvol.geometrySignature()] : nullptr;
}


inline void Extrapolator::setRecallInformation(Cache& cache,
                                               const Surface& rsf, 
                                               const Layer& rlay, 
                                               const TrackingVolume& rvol) const
{
  cache.m_recall               = true;
  cache.m_recallSurface        = &rsf;
  cache.m_recallLayer          = &rlay;
  cache.m_recallTrackingVolume = &rvol;
}

inline void Extrapolator::resetRecallInformation(Cache& cache) const
{
  cache.m_recall               = false;
  cache.m_recallSurface        = nullptr;
  cache.m_recallLayer          = nullptr;
  cache.m_recallTrackingVolume = nullptr;
}

inline void Extrapolator::throwIntoGarbageBin(Cache& cache, 
                                              const Trk::TrackParameters* pars) const
{ 
  if (pars) cache.m_garbageBin[pars] = true; 
}

inline const Trk::TrackParameters* Extrapolator::returnResult(Cache& cache,
                                                              const Trk::TrackParameters* result) const
{
  (void) cache;
  // call the model action on the material effect updators
  for (unsigned int imueot = 0; imueot < m_subUpdators.size(); ++imueot){ 
    m_subUpdators[imueot]->modelAction();
  }
  // return the result
  return result;
}
} // end of namespace
#endif // TRKEXTOOLS_TRKEXTRAPOLATOR_H

