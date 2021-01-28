/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

//////////////////////////////////////////////////////////////////
// MaterialAllocator.h
//   Header file for class MaterialAllocator
//
// (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

#ifndef TRKIPATFITTER_MATERIALALLOCATOR_H
#define TRKIPATFITTER_MATERIALALLOCATOR_H

//<<<<<< INCLUDES                                                       >>>>>>

#include "TrkExInterfaces/IPropagator.h"
#include "AthenaBaseComps/AthAlgTool.h"
#include "GaudiKernel/ServiceHandle.h"
#include "GaudiKernel/ToolHandle.h"
#include "GeoPrimitives/GeoPrimitives.h"
#include "TrkParameters/TrackParameters.h"
#include "TrkiPatFitterUtils/IMaterialAllocator.h"
#include "TrkGeometry/MagneticFieldProperties.h"
#include "TrkDetDescrInterfaces/ITrackingGeometrySvc.h"
#include "TrkDetDescrInterfaces/ITrackingVolumesSvc.h"
#include "TrkExInterfaces/IExtrapolator.h"
#include "TrkExInterfaces/IIntersector.h"
#include "StoreGate/ReadCondHandleKey.h"
#include "TrkGeometry/TrackingGeometry.h"
#include <mutex>
#include <memory>

//<<<<<< CLASS DECLARATIONS                                             >>>>>>


class MessageHelper;
namespace Trk
{
class FitMeasurement;
class FitParameters;
class IExtrapolator;
class IIntersector;    
class TrackingVolume;
class TrackStateOnSurface;
class Volume;    
    
/** Main Fitter tool providing the implementation for the different
 *  fitting, extension and refitting use cases.
 */
class MaterialAllocator: public AthAlgTool,
             virtual public IMaterialAllocator
{
public:
    // standard AlgTool methods
    MaterialAllocator        (const std::string& type, 
                 const std::string& name,
                 const IInterface* parent);
    ~MaterialAllocator() =default;     // destructor

    // standard Athena methods
    StatusCode initialize() override;
    StatusCode finalize() override;

    
    // add leading material effects to track
    void    addLeadingMaterial (std::vector<FitMeasurement*>&    measurements,
                    ParticleHypothesis            particleHypothesis,
                    FitParameters&            fitParameters,
                                    Garbage_t&                          garbage) const;

    // allocate material
    void allocateMaterial(std::vector<FitMeasurement*>& measurements,
                          ParticleHypothesis particleHypothesis,
                          FitParameters& fitParameters,
                          const TrackParameters& startParameters,
                          Garbage_t& garbage) const;

    // initialize scattering (needs to know X0 integral)
    void    initializeScattering (std::vector<FitMeasurement*>&    measurements) const;

    // material TSOS between spectrometer entrance surface and parameters given in spectrometer */
    std::vector<const TrackStateOnSurface*>*    leadingSpectrometerTSOS(
    const TrackParameters& spectrometerParameters,
        Garbage_t& garbage) const;
 
    // order measurements by distance from startPosition
    void    orderMeasurements(std::vector<FitMeasurement*>&    measurements,
                  Amg::Vector3D            startDirection,
                  Amg::Vector3D            startPosition) const;

    // has material been reallocated?
    bool reallocateMaterial(std::vector<FitMeasurement*>& measurements,
                            FitParameters& fitParameters,
                            Garbage_t& garbage) const;

  private:
    // add material delimiters to control aggregation
    void    addSpectrometerDelimiters (std::vector<FitMeasurement*>&    measurements) const;

    // memory management
    void    deleteMaterial (const std::vector<const TrackStateOnSurface*>* material,
                                Garbage_t& garbage) const;

    // extrapolateM wrapper
    const std::vector<const TrackStateOnSurface*>*    extrapolatedMaterial (
    const ToolHandle<IExtrapolator>&    extrapolator,
    const TrackParameters&            parameters,
    const Surface&                surface,
    PropDirection                dir,
    const BoundaryCheck&                boundsCheck,
    ParticleHypothesis            particleHypothesis,
        Garbage_t&                              garbage) const;

    // allocate material in inner detector
    void indetMaterial(std::vector<FitMeasurement*>& measurements,
                       ParticleHypothesis particleHypothesis,
                       const TrackParameters& startParameters,
                       Garbage_t& garbage) const;

    // material aggregation
    std::pair<FitMeasurement*,FitMeasurement*>  materialAggregation (const std::vector<const TrackStateOnSurface*>&  material,
                                                                     std::vector<FitMeasurement*>&  measurements,
                                                                     double particleMass) const;
    
    void    materialAggregation (std::vector<FitMeasurement*>&    measurements,
                                 double particleMass) const;

    // material measurement corresponding to TrackStateOnSurface
    FitMeasurement*    measurementFromTSOS (const TrackStateOnSurface&    tsos,
                                            double outwardsEnergy,
                                            double particleMass) const;
    
    // VERBOSE print of measurements 
    void    printMeasurements(std::vector<FitMeasurement*>& measurements) const;

    // allocate material in spectrometer
    void spectrometerMaterial(std::vector<FitMeasurement*>& measurements,
                              ParticleHypothesis particleHypothesis,
                              FitParameters& fitParameters,
                              const TrackParameters& startParameters,
                              Garbage_t& garbage) const;

    
    // configurables (svc/tools then options)
    ToolHandle<IExtrapolator>               m_extrapolator{this, "Extrapolator","Trk::Extrapolator/AtlasExtrapolator",""};
    ToolHandle<IIntersector>                m_intersector{this, "Intersector","Trk::RungeKuttaIntersector/RungeKuttaIntersector",""};
    ServiceHandle<ITrackingGeometrySvc>     m_trackingGeometrySvc{this,"TrackingGeometrySvc", "TrackingGeometrySvc/AtlasTrackingGeometrySvc",""};
    ServiceHandle<ITrackingVolumesSvc>      m_trackingVolumesSvc{this,"TrackingVolumesSvc", "Trk::TrackingVolumesSvc/TrackingVolumesSvc",""};    
    ToolHandle<IPropagator>                 m_stepPropagator {this,"STEP_Propagator","Trk::STEP_Propagator/AtlasSTEP_Propagator",""};
    
    SG::ReadCondHandleKey<TrackingGeometry>   m_trackingGeometryReadKey{this, "TrackingGeometryReadKey", "", "Key of the TrackingGeometry conditions data."};

    inline const Trk::TrackingVolume* getSpectrometerEntrance() const {
        static const std::string vol_name = "MuonSpectrometerEntrance";      
        /// Good old way of retrieving the volume via the geometry service
        if (m_trackingGeometryReadKey.empty()){
            return m_trackingGeometrySvc->trackingGeometry()->trackingVolume(vol_name);
        }        
        SG::ReadCondHandle<Trk::TrackingGeometry>  handle(m_trackingGeometryReadKey, Gaudi::Hive::currentContext());
        if (!handle.isValid()){
           ATH_MSG_WARNING("Could not retrieve a valid tracking geometry");
           return nullptr;
       }
       return handle.cptr()->trackingVolume(vol_name);
    }

    
    bool                        m_aggregateMaterial;
    bool                        m_allowReordering;
    int                         m_useStepPropagator;
    unsigned                        m_maxWarnings;

    // configurable parameters and tolerances (for aggregation rules and Coulomb scattering constants)
    double                        m_orderingTolerance;
    double                        m_scattererMinGap;
    double                        m_scatteringConstant;
    double                        m_scatteringLogCoeff;
    double                        m_sectorMaxPhi;
    double                        m_stationMaxGap;
    
    // constants 
    const Trk::Volume*  m_calorimeterVolume;
    const Trk::Volume*  m_indetVolume;

    Trk::MagneticFieldProperties                        m_stepField;

    // count warnings
    std::unique_ptr<MessageHelper>                m_messageHelper;
    
    class    compareByDistance {
    public:
        bool operator ()     (const std::pair<double, FitMeasurement*> &x,
                     const std::pair<double, FitMeasurement*> &y) const
            { return x.first < y.first; }
    };   
};

} // end of namespace

#endif // TRKIPATFITTER_MATERIALALLOCATOR_H
