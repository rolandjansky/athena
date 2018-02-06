/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
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

#include "AthenaBaseComps/AthAlgTool.h"
#include "GaudiKernel/ServiceHandle.h"
#include "GaudiKernel/ToolHandle.h"
#include "GeoPrimitives/GeoPrimitives.h"
#include "TrkParameters/TrackParameters.h"
#include "TrkiPatFitterUtils/IMaterialAllocator.h"
#include "TrkGeometry/MagneticFieldProperties.h"

//<<<<<< CLASS DECLARATIONS                                             >>>>>>

class ITrackingGeometrySvc;
class ITrackingVolumesSvc;
class MessageHelper;
namespace Trk
{
class FitMeasurement;
class FitParameters;
class IExtrapolator;
class IIntersector;    
class IPropagator;
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
    MaterialAllocator		(const std::string& type, 
				 const std::string& name,
				 const IInterface* parent);
    ~MaterialAllocator		(void); 	// destructor

    // standard Athena methods
    StatusCode initialize();
    StatusCode finalize();
    
    // add leading material effects to track
    void	addLeadingMaterial (std::vector<FitMeasurement*>&	measurements,
				    ParticleHypothesis			particleHypothesis,
				    FitParameters&			fitParameters) const;

    // allocate material 
    void	allocateMaterial (std::vector<FitMeasurement*>&		measurements,
				  ParticleHypothesis			particleHypothesis,
				  const FitParameters&			fitParameters,
				  const TrackParameters&		startParameters) const;

    // initialize scattering (needs to know X0 integral)
    void	initializeScattering (std::vector<FitMeasurement*>&	measurements) const;

    // material TSOS between spectrometer entrance surface and parameters given in spectrometer */
    std::vector<const TrackStateOnSurface*>*    leadingSpectrometerTSOS(
	const TrackParameters& spectrometerParameters) const;
 
    // order measurements by distance from startPosition
    void	orderMeasurements(std::vector<FitMeasurement*>&	measurements,
				  Amg::Vector3D			startDirection,
				  Amg::Vector3D			startPosition) const;
    
    // has material been reallocated? 
    bool	reallocateMaterial (std::vector<FitMeasurement*>&	measurements,
				    const FitParameters&		fitParameters) const;

    // clear temporary TSOS
    void	clear (void);

private:
    // add material delimiters to control aggregation
    void	addSpectrometerDelimiters (std::vector<FitMeasurement*>&	measurements) const;

    // memory management
    void	deleteMaterial (const std::vector<const TrackStateOnSurface*>* material) const;

    // extrapolateM wrapper
    const std::vector<const TrackStateOnSurface*>*	extrapolatedMaterial (
	const ToolHandle<IExtrapolator>&	extrapolator,
	const TrackParameters&			parameters,
	const Surface&				surface,
	PropDirection				dir,
	BoundaryCheck				boundsCheck,
	ParticleHypothesis			particleHypothesis) const;

    // allocate material in inner detector
    void	indetMaterial (std::vector<FitMeasurement*>&		measurements,
			       ParticleHypothesis			particleHypothesis,
			       const TrackParameters&			startParameters) const;
    
    // material aggregation
    std::pair<FitMeasurement*,FitMeasurement*>	materialAggregation (
	const std::vector<const TrackStateOnSurface*>&	material,
	std::vector<FitMeasurement*>&			measurements,
	double						particleMass) const;
    
    void	materialAggregation (std::vector<FitMeasurement*>&	measurements,
				     double				particleMass) const;

    // material measurement corresponding to TrackStateOnSurface
    FitMeasurement*	measurementFromTSOS (const TrackStateOnSurface&	tsos,
					     double			outwardsEnergy,
					     double			particleMass) const;
    
    // VERBOSE print of measurements 
    void	printMeasurements(std::vector<FitMeasurement*>&		measurements) const;

    // allocate material in spectrometer
    void	spectrometerMaterial (std::vector<FitMeasurement*>&	measurements,
				      ParticleHypothesis		particleHypothesis,
				      const FitParameters&		fitParameters,
				      const TrackParameters&		startParameters) const;
    
    // configurables (svc/tools then options)
    ToolHandle<IExtrapolator>				m_extrapolator;
    mutable ToolHandle<IIntersector>			m_intersector;
    ToolHandle<IExtrapolator>				m_spectrometerExtrapolator;
    mutable ServiceHandle<ITrackingGeometrySvc> 	m_trackingGeometrySvc;	// init with callback
    ServiceHandle<ITrackingVolumesSvc>			m_trackingVolumesSvc;
    ToolHandle<IPropagator>				m_stepPropagator;
    bool						m_aggregateMaterial;
    bool						m_allowReordering;
    int 						m_useStepPropagator;
    unsigned						m_maxWarnings;

    // configurable parameters and tolerances (for aggregation rules and Coulomb scattering constants)
    double						m_materialMaxGap;
    double						m_orderingTolerance;
    double						m_scattererMinGap;
    double						m_scatteringConstant;
    double						m_scatteringLogCoeff;
    double						m_sectorMaxPhi;
    double						m_stationMaxGap;
    
    // constants 
    Trk::Volume*					m_calorimeterVolume;
    Trk::Volume*					m_indetVolume;

    Trk::MagneticFieldProperties                        m_stepField;

    // constant initialized the first time it's needed
    mutable const Trk::TrackingVolume*			m_spectrometerEntrance;

    // memory management
    mutable std::vector<const TrackStateOnSurface*>*	m_temporaryTSOS;

    // count warnings
    mutable MessageHelper*				m_messageHelper;
    
    class	compareByDistance
    {
    public:
	bool operator () 	(const std::pair<double, FitMeasurement*> &x,
				 const std::pair<double, FitMeasurement*> &y) const
	    { return x.first < y.first; }
    };   
};

} // end of namespace

#endif // TRKIPATFITTER_MATERIALALLOCATOR_H
