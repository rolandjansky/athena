/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/***************************************************************************
 least squared fit to track hit data => PerigeeParameters with covariance
 and fit quality. 
 ***************************************************************************/

//<<<<<< INCLUDES                                                       >>>>>>

#include <cmath>
#include <iomanip>
#include "EventPrimitives/EventPrimitives.h"
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/SystemOfUnits.h"
#include "GeoPrimitives/GeoPrimitives.h"
#include "Identifier/Identifier.h"
#include "TrkDetDescrInterfaces/ITrackingVolumesSvc.h"
#include "TrkDetElementBase/TrkDetElementBase.h"
#include "TrkExInterfaces/IIntersector.h"
#include "TrkExInterfaces/IPropagator.h"
#include "TrkExUtils/TrackSurfaceIntersection.h"
#include "TrkGeometry/TrackingVolume.h"
#include "TrkMaterialOnTrack/EnergyLoss.h"
#include "TrkMaterialOnTrack/MaterialEffectsOnTrack.h"
#include "TrkMaterialOnTrack/ScatteringAngles.h"
#include "TrkMeasurementBase/MeasurementBase.h"
#include "MuonCompetingRIOsOnTrack/CompetingMuonClustersOnTrack.h"
#include "TrkParameters/TrackParameters.h"
#include "TrkRIO_OnTrack/RIO_OnTrack.h"
#include "TrkSurfaces/PerigeeSurface.h"
#include "TrkSurfaces/PlaneSurface.h"
#include "TrkSurfaces/StraightLineSurface.h"
#include "TrkSurfaces/Surface.h"
#include "TrkTrack/AlignmentEffectsOnTrack.h"
#include "TrkTrack/Track.h"
#include "TrkTrack/TrackStateOnSurface.h"
#include "TrkiPatFitterUtils/ExtrapolationType.h"
#include "TrkiPatFitterUtils/FitMeasurement.h"
#include "TrkiPatFitterUtils/FitParameters.h"
#include "TrkiPatFitterUtils/FitProcedure.h"
#include "TrkiPatFitterUtils/FitProcedureQuality.h"
#include "TrkiPatFitterUtils/IMaterialAllocator.h"
#include "TrkiPatFitterUtils/MessageHelper.h"
#include "TrkiPatFitter/iPatFitter.h"

namespace Trk
{ 

//<<<<<< CLASS STRUCTURE INITIALIZATION                                 >>>>>>

iPatFitter::iPatFitter (const std::string&	type, 
			const std::string&	name,
			const IInterface*	parent)
    :	AthAlgTool			(type, name, parent),
	m_fitProcedure			(0),
	m_globalFit			(false),
	m_iterations			(0),
	m_measurements			(0),
	m_parameters			(0),
	m_aggregateMaterial		(true),
	m_asymmetricCaloEnergy		(true),
	m_eigenMatrixTreatment		(true),
	m_fullCombinedFit		(true),
	m_lineFit			(false),
	m_lineMomentum			(100.*Gaudi::Units::GeV),
	m_materialAllocator		("Trk::MaterialAllocator/MaterialAllocator"),
	m_rungeKuttaIntersector		("Trk::RungeKuttaIntersector/RungeKuttaIntersector"),
	m_solenoidalIntersector		("Trk::SolenoidalIntersector/SolenoidalIntersector"),
	m_stepPropagator                ("Trk::STEP_Propagator/AtlasSTEP_Propagator"),
	m_straightLineIntersector	("Trk::StraightLineIntersector/StraightLineIntersector"),
	m_trackingVolumesSvc		("TrackingVolumesSvc/TrackingVolumesSvc",name),
 	m_orderingTolerance		(1.*Gaudi::Units::mm),
	m_maxWarnings			(10),
	m_constrainedAlignmentEffects	(false),
	m_extendedDebug			(false),
	m_forcedRefitsForValidation	(0),
	m_maxIterations			(25),
	m_useStepPropagator		(1),
	m_calorimeterVolume		(0),
	m_indetVolume			(0),
	m_stepField			(Trk::MagneticFieldProperties(Trk::FullField)),
	m_countFitAttempts		(0),
	m_countGoodFits			(0),
	m_countIterations		(0),
	m_countRefitAttempts		(0),
	m_countGoodRefits		(0),
	m_countRefitIterations		(0),
	m_messageHelper			(0)
{
    m_messageHelper	= new MessageHelper(*this);
    declareInterface<ITrackFitter>(this);
    declareProperty("AggregateMaterial",	       	m_aggregateMaterial);
    declareProperty("AsymmetricCaloEnergy",	       	m_asymmetricCaloEnergy);
    declareProperty("EigenMatrixTreatment",		m_eigenMatrixTreatment);
    declareProperty("FullCombinedFit",			m_fullCombinedFit);
    declareProperty("LineFit",				m_lineFit);
    declareProperty("LineMomentum",			m_lineMomentum);
    declareProperty("MaterialAllocator",		m_materialAllocator);
    declareProperty("RungeKuttaIntersector",		m_rungeKuttaIntersector);
    declareProperty("SolenoidalIntersector",		m_solenoidalIntersector);
    declareProperty("StraightLineIntersector",		m_straightLineIntersector);
    declareProperty("TrackingVolumesSvc",		m_trackingVolumesSvc);
    declareProperty("OrderingTolerance",		m_orderingTolerance);
    declareProperty("MaxNumberOfWarnings",		m_maxWarnings,
		    "Maximum number of permitted WARNING messages per message type.");

    // validation options
    declareProperty("ConstrainedAlignmentEffects",	m_constrainedAlignmentEffects);
    declareProperty("ExtendedDebug",			m_extendedDebug);
    declareProperty("ForcedRefitsForValidation",	m_forcedRefitsForValidation);
    declareProperty("MaxIterations",			m_maxIterations);

    // m_useStepPropagator 0 means not used (so Intersector used)
    // 1 Intersector not used and StepPropagator used with FullField
    // 2 StepPropagator with FastField propagation 
    // 99 debug mode where both are ran with FullField
    declareProperty("UseStepPropagator",		m_useStepPropagator);
}

iPatFitter::~iPatFitter (void)
{}

//<<<<<< PUBLIC MEMBER FUNCTION DEFINITIONS                             >>>>>>

StatusCode
iPatFitter::initialize()
{
    // print name and package version
    if (! msgLvl(MSG::DEBUG)) m_extendedDebug = false;
    ATH_MSG_INFO( "iPatFitter::initialize() - package version " << PACKAGE_VERSION );
    msg(MSG::INFO) << " with options: ";
    if (m_aggregateMaterial)		msg() << " AggregateMaterial";
    if (m_asymmetricCaloEnergy) 	msg() << " AsymmetricCaloEnergy";
    if (m_eigenMatrixTreatment)		msg() << " EigenMatrixTreatment";
    if (m_fullCombinedFit)		msg() << " FullCombinedFit";
    if (m_globalFit)			msg() << " GlobalFitter";
    if (m_lineFit)			msg() << " LineFit with p = "
					      << m_lineMomentum/Gaudi::Units::GeV << " GeV";
    if (m_constrainedAlignmentEffects)	msg() << " ConstrainedAlignmentEffects";
    if (m_extendedDebug)		msg() << " ExtendedDebug";
    if (m_forcedRefitsForValidation)	msg() << " ForcedRefitsForValidation = "
					      << m_forcedRefitsForValidation;
    if (m_useStepPropagator)		msg() << " UseStepPropagator = "
					      << m_useStepPropagator;
    msg() << endmsg;
  
    // fill WARNING messages
    m_messageHelper->setMaxNumberOfMessagesPrinted(m_maxWarnings);
    m_messageHelper->setNumberOfMessages(26);
    m_messageHelper->setMessage(0, "fit (Track): outlier removal not implemented");
    m_messageHelper->setMessage(1, "fit (Track): track without perigee");
    m_messageHelper->setMessage(2, "fit (Track): track without trackStateOnSurfaces");
    m_messageHelper->setMessage(3, "fit (Track + PrepRawDataSet): interface not implemented");
    m_messageHelper->setMessage(4, "fit (PrepRawDataSet): interface not implemented");
    m_messageHelper->setMessage(5, "fit (Track + MeasurementSet): outlier removal not implemented");
    m_messageHelper->setMessage(6, "fit (Track + MeasurementSet): track without trackStateOnSurfaces");
    m_messageHelper->setMessage(7, "fit (Track + MeasurementSet): track without measuredPerigee");
    m_messageHelper->setMessage(8, "fit (Track + MeasurementSet): FIX material may get double counted");
    m_messageHelper->setMessage(9, "fit (Perigee + MeasurementSet): outlier removal not implemented");
    m_messageHelper->setMessage(10,"fit (Perigee + MeasurementSet): null perigee");
    m_messageHelper->setMessage(11,"fit (combined muon): outlier removal not implemented");
    m_messageHelper->setMessage(12,"fit (combined muon): no perigee start value");
    m_messageHelper->setMessage(13,"fit (combined muon): indet track without trackStateOnSurfaces");
    m_messageHelper->setMessage(14,"fit (combined muon): indet track without measuredPerigee");
    m_messageHelper->setMessage(15,"addMeasurements: no intersection to MeasurementSet");
    m_messageHelper->setMessage(16,"addMeasurements: skip TSOS as not understood. Type: ");
    m_messageHelper->setMessage(17,"addMeasurements: skip TSOS with missing trackParameters. Type: ");
    m_messageHelper->setMessage(18,"addMeasurements: skip measurement as fail to intersect associated surface from given starting parameters");
    m_messageHelper->setMessage(19,"addMeasurements: TSOS skipped. Type: ");
    m_messageHelper->setMessage(20,"fail fit as CaloDeposit outside calo volume");
    m_messageHelper->setMessage(21,"conflicting energy deposit sign for inDet material");
    m_messageHelper->setMessage(22,"conflicting energy deposit sign for spectrometer material");
    m_messageHelper->setMessage(23,"excessive calorimeter energy loss : ");
    m_messageHelper->setMessage(24,"excessive spectrometer energy loss : ");
    m_messageHelper->setMessage(25,"flipped track measurement order");

    // retrieve the MaterialProvider and various Intersectors
    if (m_materialAllocator.retrieve().isFailure())
    {
	ATH_MSG_FATAL( "Failed to retrieve tool " << m_materialAllocator );
        return StatusCode::FAILURE;
    }
    else
    {
	ATH_MSG_INFO( "Retrieved tool " << m_materialAllocator );
    }
    if (m_rungeKuttaIntersector.retrieve().isFailure())
    {
	ATH_MSG_FATAL( "Failed to retrieve tool " << m_rungeKuttaIntersector );
	return StatusCode::FAILURE;
    }
    else
    {
	ATH_MSG_INFO( "Retrieved tool " << m_rungeKuttaIntersector );
    }
    if (m_solenoidalIntersector.retrieve().isFailure())
    {
	ATH_MSG_FATAL( "Failed to retrieve tool " << m_solenoidalIntersector );
	return StatusCode::FAILURE;
    }
    else
    {
	ATH_MSG_INFO( "Retrieved tool " << m_solenoidalIntersector );
    }
    if (m_useStepPropagator)
    {
	if (m_useStepPropagator == 2)  m_stepField = Trk::MagneticFieldProperties(Trk::FastField);
	if (m_stepPropagator.retrieve().isFailure())
	{
	    ATH_MSG_FATAL( "Failed to retrieve tool " <<  m_stepPropagator );
	    return StatusCode::FAILURE;
	}
	else
	{
	    ATH_MSG_INFO( "Retrieved tool " << m_stepPropagator );
	}
    }
    if (m_straightLineIntersector.retrieve().isFailure())
    {
	ATH_MSG_FATAL( "Failed to retrieve tool " << m_straightLineIntersector );
	return StatusCode::FAILURE;
    }
    else
    {
	ATH_MSG_INFO( "Retrieved tool " << m_straightLineIntersector );
    }
    
    // need to create the IndetExit and MuonEntrance TrackingVolumes
    if (m_trackingVolumesSvc.retrieve().isFailure())
    {
	ATH_MSG_FATAL( "Failed to retrieve Svc " << m_trackingVolumesSvc );
	return StatusCode::FAILURE;
    } 
    else
    {
	ATH_MSG_INFO( "Retrieved Svc " << m_trackingVolumesSvc );
	m_calorimeterVolume	= new Volume(
	    m_trackingVolumesSvc->volume(ITrackingVolumesSvc::MuonSpectrometerEntryLayer));
	m_indetVolume		= new Volume(
	    m_trackingVolumesSvc->volume(ITrackingVolumesSvc::CalorimeterEntryLayer));
    }

    // can now create FitProcedure class
    m_fitProcedure = new FitProcedure(m_constrainedAlignmentEffects,
				      m_extendedDebug,
				      m_eigenMatrixTreatment,
				      m_lineFit,
				      m_rungeKuttaIntersector,
				      m_solenoidalIntersector,
				      m_straightLineIntersector,
				      m_stepPropagator,
				      m_indetVolume,
				      m_maxIterations,
				      m_useStepPropagator);
    
    return StatusCode::SUCCESS;
}

StatusCode
iPatFitter::finalize()
{
    // print summary statistics
    double fits		= static_cast<double>(m_countFitAttempts);
    double goodFit	= 0.;
    double iterations	= 0.;
    if (m_countFitAttempts) goodFit	= static_cast<double>(100*m_countGoodFits) / fits;
    if (m_countGoodFits) iterations	= static_cast<double>(m_countIterations) /
					  static_cast<double>(m_countGoodFits);
    ATH_MSG_INFO( std::setiosflags(std::ios::fixed)
		  << "finalized after " << m_countFitAttempts
		  << " track-fits attempted, out of which "
		  << std::setw(5) << std::setprecision(1) << goodFit
		  << "% converged, taking an average "
		  << std::setw(5) << std::setprecision(2) << iterations
		  << " iterations" );
    if (m_forcedRefitsForValidation)
    {
	double refits		= static_cast<double>(m_countRefitAttempts);
	double goodRefit	= 0.;
	double refitIterations	= 0.;
	if (m_countRefitAttempts) goodRefit	= static_cast<double>(100*m_countGoodRefits) / refits;
	if (m_countGoodRefits) refitIterations	= static_cast<double>(m_countRefitIterations) /
						  static_cast<double>(m_countGoodRefits);
	ATH_MSG_INFO( std::setiosflags(std::ios::fixed)
		      << "finalized after " << m_countRefitAttempts
		      << "     refits attempted, out of which "
		      << std::setw(5) << std::setprecision(1) << goodRefit
		      << "% converged, taking an average "
		      << std::setw(5) << std::setprecision(2) << refitIterations
		      << " iterations" );
    }
    
    m_messageHelper->printSummary();

    // delete all objects owned by class
    if (m_globalFit && m_measurements)
    {
	for (std::list<Trk::FitMeasurement*>::iterator m = m_measurements->begin();
	     m != m_measurements->end();
	     ++m)
	{
	    delete *m;
	}
	m_measurements->clear();
    }
    delete m_calorimeterVolume;
    delete m_fitProcedure;
    delete m_indetVolume;
    delete m_measurements;
    delete m_messageHelper;
    delete m_parameters;
    
    return StatusCode::SUCCESS;
}

Track*
iPatFitter::fit	(const Track&			track,
		 const RunOutlierRemoval	runOutlier,
		 const ParticleHypothesis	particleHypothesis) const
{
    ATH_MSG_VERBOSE( " track fit " );
    ++m_countFitAttempts;
    // outlier removal not implemented
    if (runOutlier) m_messageHelper->printWarning(0);    

    // create Perigee if starting parameters are for a different surface type
    const Perigee* perigee		= track.perigeeParameters();
    PerigeeSurface* perigeeSurface	= 0;
    if (! perigee)
    {
	DataVector<const TrackStateOnSurface>::const_iterator s = track.trackStateOnSurfaces()->begin();
	while (s != track.trackStateOnSurfaces()->end() && ! (**s).trackParameters()) ++s;
	if (! (**s).trackParameters())
	{
	    // input track without parameters
	    m_messageHelper->printWarning(1);
	    return 0;
	}
	
	Amg::Vector3D origin((**s).trackParameters()->position());
	perigeeSurface	= new PerigeeSurface(origin);
	perigee		= new Perigee((**s).trackParameters()->position(),
				      (**s).trackParameters()->momentum(),
				      (**s).trackParameters()->charge(),
				      *perigeeSurface);
    }

    delete m_parameters;
    m_parameters	= new FitParameters(*perigee);
    
    // set up the measurements
    if (! track.trackStateOnSurfaces())
    {
	// input track without trackStateOnSurfaces
	m_messageHelper->printWarning(2);
	return 0;
    }
    
    std::list<FitMeasurement*>* measurements = measurementList();
    bool haveMaterial = addMeasurements(*measurements,
					*m_parameters,
					particleHypothesis,
					*track.trackStateOnSurfaces());

    // allocate material
    if (! haveMaterial && particleHypothesis != Trk::nonInteracting)
    {
	m_materialAllocator->allocateMaterial(*measurements,
					      particleHypothesis,
					      *m_parameters,
					      *perigee);
    }

    // perform fit and return fitted track
    TrackInfo trackInfo(TrackInfo::iPatTrackFitter,particleHypothesis);
    trackInfo.addPatternReco(track.info());
    Trk::Track* fittedTrack = performFit(measurements, m_parameters, particleHypothesis, trackInfo);
    if (perigeeSurface)
    {
	delete perigeeSurface;
	delete perigee;
    }

    // validation
    for (int i = 0; i < m_forcedRefitsForValidation; ++i)
	if (fittedTrack) refit(*fittedTrack,runOutlier,particleHypothesis);
    
    return fittedTrack;	
}

Track*
iPatFitter::fit	(const Track&			/*track*/,
		 const PrepRawDataSet&		/*prepRawDataSet*/,
		 const RunOutlierRemoval	/*trackrunOutlier*/,
		 const ParticleHypothesis	/*trackparticleHypothesis*/) const
{
    ++m_countFitAttempts;
    // track + PrepRawDataSet interface not implemented
    m_messageHelper->printWarning(3);
    return 0;
}

Track*
iPatFitter::fit	(const PrepRawDataSet&		/*prepRawDataSet*/,
		 const TrackParameters&		/*estimatedParametersNearOrigin*/,
		 const RunOutlierRemoval	/*trackrunOutlier*/,
		 const ParticleHypothesis	/*trackparticleHypothesis*/) const
{
    ++m_countFitAttempts;
    // PrepRawDataSet interface not implemented
    m_messageHelper->printWarning(4);
    return 0;
}

Track*
iPatFitter::fit	(const Track&			track,
		 const MeasurementSet&		measurementSet,
		 const RunOutlierRemoval	runOutlier,
		 const ParticleHypothesis	particleHypothesis) const
{
    ATH_MSG_VERBOSE( " track + measurementSet fit " );
    ++m_countFitAttempts;
    // outlier removal not implemented
    if (runOutlier) m_messageHelper->printWarning(5);    

    // create starting parameters
    if (! track.trackStateOnSurfaces())
    {
	// track without trackStateOnSurfaces
	m_messageHelper->printWarning(6);
	return 0;
    }
    const Perigee* perigee = dynamic_cast<const Perigee*>
			     ((**track.trackStateOnSurfaces()->begin()).trackParameters());
    if (! perigee)
    {
	// track without measuredPerigee
	m_messageHelper->printWarning(7);
	return 0;
    }
    
    delete m_parameters;
    m_parameters	= new FitParameters(*perigee);
    
    // set up the measurements (and material)
    std::list<FitMeasurement*>* measurements = measurementList();
    if (addMeasurements(*measurements,*m_parameters,particleHypothesis,*track.trackStateOnSurfaces()))
	m_messageHelper->printWarning(8);	// FIX needed: material may get double counted
    addMeasurements(*measurements,
		    measurementSet,
		    *m_parameters);
    if (particleHypothesis != Trk::nonInteracting)
    {
	const TrackParameters& endParams = *(track.trackStateOnSurfaces()->back()->trackParameters());
	m_materialAllocator->allocateMaterial(*measurements,
					      particleHypothesis,
					      *m_parameters,
					      endParams);
    }
    
    // perform fit and return fitted track
    TrackInfo trackInfo(TrackInfo::iPatTrackFitter,particleHypothesis);
    trackInfo.addPatternReco(track.info());
    return performFit(measurements,
		      m_parameters,
		      particleHypothesis,
		      trackInfo,
		      track.trackStateOnSurfaces(),
		      track.fitQuality());
}

Track*
iPatFitter::fit	(const MeasurementSet&		measurementSet,
		 const TrackParameters&		perigeeStartValue,
		 const RunOutlierRemoval	runOutlier,
		 const ParticleHypothesis	particleHypothesis) const
{
    ATH_MSG_VERBOSE( " fit from measurement set + perigeeStartValue " );
    ++m_countFitAttempts;
    // outlier removal not implemented
    if (runOutlier) m_messageHelper->printWarning(9);

    const Perigee* perigee = dynamic_cast<const Perigee*>(&perigeeStartValue);
    if (! perigee)
    {
	// track without Perigee start value
	m_messageHelper->printWarning(10);
	return 0;
    }
    delete m_parameters;
    m_parameters	= new FitParameters(*perigee);
    
    // set up the measurements (and material)
    std::list<FitMeasurement*>* measurements = measurementList();
    addMeasurements(*measurements,measurementSet,*m_parameters);
    if (particleHypothesis != Trk::nonInteracting)
    {
	m_materialAllocator->allocateMaterial(*measurements,
					      particleHypothesis,
					      *m_parameters,
					      perigeeStartValue);
    }
    
    // perform fit and return fitted track
    TrackInfo trackInfo(TrackInfo::iPatTrackFitter,particleHypothesis);
    return performFit(measurements, m_parameters, particleHypothesis, trackInfo);
}

Track*
iPatFitter::fit	(const Track&			indetTrack,
		 const Track&			spectrometerTrack,
		 const RunOutlierRemoval	runOutlier,
		 const ParticleHypothesis	particleHypothesis) const
{
    ATH_MSG_VERBOSE( " combined muon fit " );
    ++m_countFitAttempts;
    // outlier removal not implemented
    if (runOutlier) m_messageHelper->printWarning(11);

    // indet (full refit to measurements or use measured perigee)
    bool haveMaterial	= true;
    std::list<FitMeasurement*>* measurements = measurementList();
    delete m_parameters;
    if (indetTrack.perigeeParameters())
    {
	m_parameters	= new FitParameters(*indetTrack.perigeeParameters());
    }
    else if (spectrometerTrack.perigeeParameters()
	     && m_indetVolume->inside(spectrometerTrack.perigeeParameters()->associatedSurface().center()))
    {
	m_parameters	= new FitParameters(*spectrometerTrack.perigeeParameters());
    }
    else
    {
	// combined muon fit without Perigee start value"
	m_messageHelper->printWarning(12);
	return 0;
    }
    
    // get both perigee parameters, use most precise for momentum starting value
    const Perigee* indetPerigee		= dynamic_cast<const Perigee*>(indetTrack.perigeeParameters());
    const Perigee* spectrometerPerigee	= dynamic_cast<const Perigee*>(spectrometerTrack.perigeeParameters());
    if (spectrometerPerigee
	&& ! m_indetVolume->inside(spectrometerPerigee->associatedSurface().center()))
	spectrometerPerigee = 0;
    if (! spectrometerTrack.info().trackProperties(Trk::TrackInfo::StraightTrack)
	&& spectrometerPerigee)
    {
	if (indetTrack.info().trackProperties(Trk::TrackInfo::StraightTrack)
	    || ! indetPerigee
	    || ! indetPerigee->covariance())
	{
	    m_parameters->qOverP(spectrometerPerigee->parameters()[Trk::qOverP]);
	    ATH_MSG_VERBOSE(" set starting momentum from spectrometer "
			    << 1./(m_parameters->qOverP()*Gaudi::Units::GeV) << " GeV" );
	}
	else if (indetPerigee)
	{
	    if (spectrometerPerigee->covariance()
		&& (*spectrometerPerigee->covariance())(Trk::qOverP,Trk::qOverP) <
		(*indetPerigee->covariance())(Trk::qOverP,Trk::qOverP))
	    {
		m_parameters->qOverP(spectrometerPerigee->parameters()[Trk::qOverP]);
		ATH_MSG_VERBOSE(" set starting momentum from spectrometer "
				<< 1./(m_parameters->qOverP()*Gaudi::Units::GeV) << " GeV" );
	    }
	}
    }

    if (m_fullCombinedFit)
    {
	// set up the measurements
	if (! indetTrack.trackStateOnSurfaces())
	{
	    // fail as indet track without trackStateOnSurfaces
	    m_messageHelper->printWarning(13);
	    return 0;
	}
	if (! addMeasurements(*measurements,
			      *m_parameters,
			      particleHypothesis,
			      *indetTrack.trackStateOnSurfaces())) haveMaterial = false;
    }

    // add the spectrometer measurements
    if (! addMeasurements(*measurements,
			  *m_parameters,
			  particleHypothesis,
			  *spectrometerTrack.trackStateOnSurfaces())) haveMaterial = false;
    if (! haveMaterial && particleHypothesis != Trk::nonInteracting)
    {
	Perigee* startingPerigee	= m_parameters->startingPerigee();
	if (startingPerigee)
	{
	    m_materialAllocator->allocateMaterial(*measurements,
						  particleHypothesis,
						  *m_parameters,
						  *startingPerigee);
	    delete startingPerigee;
	}
    }
    
    // create starting parameters, perform fit and return fitted track
    TrackInfo trackInfo(TrackInfo::iPatTrackFitter,particleHypothesis);
    trackInfo.addPatternReco(indetTrack.info());
    trackInfo.addPatternReco(spectrometerTrack.info());
    if (m_fullCombinedFit)
    {
	Trk::Track* fittedTrack =  performFit(measurements, m_parameters, particleHypothesis, trackInfo);
	
	// validation
	for (int i = 0; i < m_forcedRefitsForValidation; ++i)
	    if (fittedTrack) refit(*fittedTrack,runOutlier,particleHypothesis);
	
	return fittedTrack;
    }
    else	// hybrid fit
    {
	if (! indetPerigee)
	{
	    // fail combined muon fit as indet track without measuredPerigee
	    m_messageHelper->printWarning(14);
	    return 0;
	}
	measurements->push_front(new FitMeasurement(*indetPerigee));
	FitParameters measuredParameters(*indetPerigee);
	Trk::Track* fittedTrack = performFit(measurements,
					     &measuredParameters,
					     particleHypothesis,
					     trackInfo,
					     indetTrack.trackStateOnSurfaces(),
					     indetTrack.fitQuality());
		
	// validation
	for (int i = 0; i < m_forcedRefitsForValidation; ++i)
	    if (fittedTrack) refit(*fittedTrack,runOutlier,particleHypothesis);
	
	return fittedTrack;
    }
}

//<<<<<< PRIVATE MEMBER FUNCTION DEFINITIONS                            >>>>>>

void
iPatFitter::addMeasurements (std::list<FitMeasurement*>&	measurements,
			     const MeasurementSet&		measurementSet,
			     const FitParameters&		parameters) const
{
    // extrapolation to set FittedTrajectory
    double qOverP					= parameters.qOverP();
    double previousDistance				= -m_orderingTolerance;
    double previousDistanceR				= -m_orderingTolerance;
    double previousDistanceZ				= -m_orderingTolerance;
    bool reorder					= false;
    Amg::Vector3D startDirection			= parameters.direction();
    Amg::Vector3D startPosition				= parameters.position();
    ExtrapolationType type				= FittedTrajectory;
    const TrackSurfaceIntersection* vertex		= parameters.intersection();
    const TrackSurfaceIntersection* intersection	= vertex;

    int hit = measurements.size();
    for (MeasurementSet::const_iterator m = measurementSet.begin();
	 m != measurementSet.end();
	 ++m, ++hit)
    {
	const TrackSurfaceIntersection* newIntersection = m_useStepPropagator>=1?
            m_stepPropagator->intersectSurface((**m).associatedSurface(),
					       intersection,
					       qOverP,
					       m_stepField,
					       Trk::muon):
	    m_rungeKuttaIntersector->intersectSurface((**m).associatedSurface(),
						      intersection,
						      qOverP);
        if (m_useStepPropagator==99 && newIntersection)
	{
            const TrackSurfaceIntersection* newIntersectionSTEP = 
		m_stepPropagator->intersectSurface((**m).associatedSurface(),
						   intersection,
						   qOverP,
						   m_stepField,
						   Trk::muon);
            if (newIntersectionSTEP)
	    {
//              double dist = 1000.*(newIntersectionSTEP->position()-newIntersection->position()).mag();
//              std::cout << " iPat 1 distance STEP and Intersector " << dist << std::endl; 
//              if(dist>10.) std::cout << " iPat 1 ALARM distance STEP and Intersector " << dist << std::endl; 
              delete newIntersectionSTEP;
//            } else {
//              std::cout << " iPat 1 ALARM STEP did not intersect! " << std::endl;
            }
        }
	if (newIntersection)
	{
	    intersection = newIntersection;
	    
	    // check if ordering OK
	    if (! reorder)
	    {
		double distance 		= startDirection.dot(
		    intersection->position() - startPosition );
		Amg::Vector3D positionMst	= (**m).globalPosition();
		double distanceR		= sqrt(
		    (positionMst.x() - startPosition.x())*(positionMst.x() - startPosition.x()) +
		    (positionMst.y() - startPosition.y())*(positionMst.y() - startPosition.y()) );
		double distanceZ		= (positionMst.z() - startPosition.z());
		if (startDirection.z() < 0) distanceZ = -distanceZ;
		if (distance < previousDistance
		    && distanceR < previousDistanceR
		    && distanceZ < previousDistanceZ)
		{
		    reorder = true;
		    ATH_MSG_DEBUG( " reorder 3D distance " << distance - previousDistance
				   << " R distance " << distanceR - previousDistanceR
				   << " Z distance " << distanceZ - previousDistanceZ ); 
                }
		previousDistance	= distance - m_orderingTolerance;
		previousDistanceR	= distanceR - m_orderingTolerance;
		previousDistanceZ	= distanceZ - m_orderingTolerance;
	    }
	}
	else
	{
	    // FIXME
	    // no intersection to MeasurementSet
	    m_messageHelper->printWarning(15);
	    intersection = new TrackSurfaceIntersection(*intersection);
	}
	FitMeasurement* measurement = new FitMeasurement(hit,0,*m);
	measurement->intersection(type,intersection);
	measurement->qOverP(qOverP);
	measurements.push_back(measurement);
    }
   
    // reorder if necessary
    if (reorder) m_materialAllocator->orderMeasurements(measurements,startDirection,startPosition);

    delete vertex;
}

bool
iPatFitter::addMeasurements (std::list<FitMeasurement*>&		  measurements,
			     const FitParameters&			  parameters,
			     ParticleHypothesis				  particleHypothesis,
			     const DataVector<const TrackStateOnSurface>& trackStateOnSurfaces) const
{
    // create vector of any TSOS'es which require fitted alignment corrections
    std::vector<Identifier> misAlignedTSOS;
    std::vector<int> misAlignmentNumbers;
    int	misAlignmentNumber		= 0;
    int	tsos				= 0;
    //  BUG that shifts ...   misAlignmentNumbers.push_back(misAlignmentNumber);
    for (DataVector<const TrackStateOnSurface>::const_iterator
	     r = trackStateOnSurfaces.begin();
	 r != trackStateOnSurfaces.end();
	 ++r, ++tsos)
    {
	if (! (**r).alignmentEffectsOnTrack() || ! (**r).trackParameters()) continue;
	const AlignmentEffectsOnTrack& AEOT	= *(**r).alignmentEffectsOnTrack();
	++misAlignmentNumber;
	for (std::vector<Identifier>::const_iterator
		 a = AEOT.vectorOfAffectedTSOS().begin();
	     a != AEOT.vectorOfAffectedTSOS().end();
	     ++a)
	{
	    misAlignedTSOS.push_back(*a);
	    misAlignmentNumbers.push_back(misAlignmentNumber);
	}
	if (m_extendedDebug) ATH_MSG_DEBUG( " tsos " << tsos
					    << " misAlignedTSOS.size() "	<< misAlignedTSOS.size()
					    << "        misAlignmentNumber "	<< misAlignmentNumber
					    << "  offset "			<< AEOT.deltaTranslation()
					    << " +- "				<< AEOT.sigmaDeltaTranslation()
					    << "  rotation "			<< AEOT.deltaAngle()
					    << " +- "				<< AEOT.sigmaDeltaAngle() );
    }
    
    // create ordered list of FitMeasurements	    
    bool haveMaterial					= false;
    bool haveMeasurement				= false;
    int hit						= measurements.size();
    double previousDistance				= -m_orderingTolerance;
    double previousDistanceR				= -m_orderingTolerance;
    double previousDistanceZ				= -m_orderingTolerance;
    bool reorder					= false;
    bool skipVertexMeasurement				= measurements.size();
    Amg::Vector3D startDirection			= parameters.direction();
    Amg::Vector3D startPosition				= parameters.position();
    const TrackSurfaceIntersection* vertex		= parameters.intersection();
    const TrackSurfaceIntersection* intersection	= vertex;
    bool measurementsFlipped				= false;
    double qOverP					= parameters.qOverP();
    ExtrapolationType type				= FittedTrajectory;
    tsos						= 0;
    for (DataVector<const TrackStateOnSurface>::const_iterator
	     s = trackStateOnSurfaces.begin();
	 s != trackStateOnSurfaces.end();
	 ++s, ++hit, ++tsos)
    {
	FitMeasurement* measurement1	= 0;
	FitMeasurement* measurement2	= 0;
	const Surface* surface		= 0;
	if ((**s).materialEffectsOnTrack() && (**s).trackParameters())
	{
	    Amg::Vector3D position	= (**s).trackParameters()->position();
	    bool calo			= (! m_indetVolume->inside(position)
					   && m_calorimeterVolume->inside(position));
	    qOverP			= (**s).trackParameters()->parameters()[Trk::qOverP];
	    surface			= &(**s).trackParameters()->associatedSurface();

	    // skip negligibly thin scatterers (exception for CaloEnergy)
	    bool keepScatterer		= true;
	    if ((**s).materialEffectsOnTrack()->thicknessInX0() < 0.0001)
	    {
		keepScatterer = false;
		if (calo)
		{
		    const MaterialEffectsOnTrack* meot =
			dynamic_cast<const MaterialEffectsOnTrack*>((**s).materialEffectsOnTrack());
		    if (meot)
		    {
			const EnergyLoss* energyLoss	= meot->energyLoss();
			if (energyLoss && std::abs(energyLoss->deltaE()) > 0.1*Gaudi::Units::MeV)
			    keepScatterer = true;
		    }
		}
	    }
	    if (keepScatterer)
	    {
		measurement1		= new FitMeasurement((**s).materialEffectsOnTrack(),
							     ParticleMasses().mass[particleHypothesis],
							     position,
							     qOverP,
							     calo);
		if (! calo
		    && ! haveMaterial
		    && (haveMeasurement || (**s).measurementOnTrack()))
		    haveMaterial	= true;
	    }
	}
	else if ((**s).alignmentEffectsOnTrack() && (**s).trackParameters())
	{
	    Amg::Vector3D direction	= (**s).trackParameters()->momentum().unit();
	    Amg::Vector3D position	= (**s).trackParameters()->position();
	    measurement1		= new FitMeasurement((**s).alignmentEffectsOnTrack(),
							     direction,
							     position);

	}
	if ((**s).measurementOnTrack())
	{
	    // option to skip vertex measurement (i.e. when not at front of list)
	    if (skipVertexMeasurement && dynamic_cast<const PerigeeSurface*>(&(**s).surface()))
	    {
		delete measurement1;
		continue;
	    }
	    haveMeasurement		= true;
	    surface			= &(**s).measurementOnTrack()->associatedSurface();
	    measurement2		= new FitMeasurement(hit,0,(**s).measurementOnTrack());
	    if ((**s).type(TrackStateOnSurface::Outlier)) measurement2->setOutlier();
	    // redundant surely??
	    // if (measurement2->isCluster() || measurement2->isDrift()) haveMeasurement = true;
//	    if (misAlignmentNumber && misAlignedTSOS.back() == *s)
//	    {
//		measurement2->alignmentParameter(misAlignmentNumber);
//		misAlignedTSOS.pop_back();
//		misAlignmentNumbers.pop_back();
//		misAlignmentNumber	= misAlignmentNumbers.back();
//	    }
//
//	    Peter 
//	    measurement2->alignmentParameter(0);
	    if (misAlignmentNumber)
	    {
		const Trk::MeasurementBase* meas = (**s).measurementOnTrack();
		Identifier id = Identifier();
		if(meas) {
		    const Trk::RIO_OnTrack* rot = dynamic_cast<const Trk::RIO_OnTrack*>(meas);
		    if(rot) {
			id = rot->identify();
		    } else {
			const Muon::CompetingMuonClustersOnTrack* crot = dynamic_cast<const Muon::CompetingMuonClustersOnTrack*>(meas);
			if(crot&&!crot->containedROTs().empty()&&crot->containedROTs().front()) {
			    id = crot->containedROTs().front()->identify();
			}
		    }
		}
		for (unsigned int im = 0; im < misAlignedTSOS.size(); ++im)
		{
		    if (misAlignedTSOS[im] != id)	continue;
		    measurement2->alignmentParameter(misAlignmentNumbers[im]);
		}
		if (m_extendedDebug)
		{
		    for (unsigned int im = 0; im < misAlignedTSOS.size(); ++im)
		    {
			if (misAlignedTSOS[im] != id)	continue;
			if (measurement2->isDrift())
			{
			    ATH_MSG_DEBUG( " tsos "				<< tsos
					   << "   Drift Measurement im "	<< im
					   << "  with misAlignmentNumber "	<< misAlignmentNumbers[im] );
			}
			else
			{ 
			    ATH_MSG_DEBUG( " tsos "				<< tsos
					   << " Cluster Measurement im "	<< im
					   << "  with misAlignmentNumber "	<< misAlignmentNumbers[im] );
			}
		    }
		}
	    }
	}
	else if (! measurement1 && (**s).trackParameters())
	{
	    if ((**s).type(TrackStateOnSurface::Hole))
	    {		
		// ATH_MSG_VERBOSE( " addMeasurements: adding hole" );
		measurement2            = new FitMeasurement(**s);
	    }
	    else if ((**s).type(TrackStateOnSurface::Perigee))
	    {
	    	if (s == trackStateOnSurfaces.begin())	continue;
	    	const Perigee* perigee	= dynamic_cast<const Perigee*>((**s).trackParameters());
	    	if (! perigee)				continue;
	    	measurement2		= new FitMeasurement(*perigee);
	    }
	    else if ((**s).type(TrackStateOnSurface::Parameter))
	    {
		continue;
	    }
	    else
	    {
		// TSOS type not understood.
		m_messageHelper->printWarning(16, (**s).dumpType());
		continue;
	    }
	}
	else if ((**s).materialEffectsOnTrack())
	{	
	    surface			= &(**s).materialEffectsOnTrack()->associatedSurface();
	}
	else if ((**s).alignmentEffectsOnTrack())
	{	
	    surface			= &(**s).alignmentEffectsOnTrack()->associatedSurface();
	}
	else
	{
	    // skip TSOS with missing trackParameters
	    // this should never happen (i.e. where's the surface?)
	    m_messageHelper->printWarning(17, (**s).dumpType());
	    continue;
	}

	// current intersection
	if ((**s).trackParameters() && (measurement1 || measurement2))
	{
	    Amg::Vector3D direction	= (**s).trackParameters()->momentum().unit();
	    if (startDirection.dot(direction) < 0.) 
	    { 
		measurementsFlipped	= true; 
		direction		= -direction; 
		if (measurement1) measurement1->flipDriftDirection(); 
		if (measurement2) measurement2->flipDriftDirection(); 
	    } 
	    qOverP	= (**s).trackParameters()->parameters()[Trk::qOverP];
	    intersection=  new TrackSurfaceIntersection((**s).trackParameters()->position(),
	    						direction,
	    						0.);
	}
	else if (surface)
	{
	    const TrackSurfaceIntersection* newIntersection = m_useStepPropagator>=1?
                m_stepPropagator->intersectSurface(*surface,
						   intersection,
						   qOverP,
						   m_stepField,
						   Trk::muon):
		m_rungeKuttaIntersector->intersectSurface(*surface,
							  intersection,
							  qOverP);
            if (m_useStepPropagator == 99 && newIntersection)
	    {
		const TrackSurfaceIntersection* newIntersectionSTEP = 
		    m_stepPropagator->intersectSurface(*surface,
						       intersection,
						       qOverP,
						       m_stepField,
						       Trk::muon);
		if (newIntersectionSTEP)
		{
		    //                double dist = 1000.*(newIntersectionSTEP->position()-newIntersection->position()).mag();
		    //                std::cout << " iPat 2 distance STEP and Intersector " << dist << std::endl; 
		    //                if(dist>10.) std::cout << " iPat 2 ALARM distance STEP and Intersector " << dist << std::endl; 
		    delete newIntersectionSTEP;
		    //              } else {
		    //                std::cout << " iPat 2 ALARM STEP did not intersect! " << std::endl;
		}
            }

	    if (! newIntersection)
	    {
		// addMeasurements: skip measurement as fail to intersect
		//                  associated surface from given starting parameters
		m_messageHelper->printWarning(18);
		delete measurement2;
		continue;
	    }
	    
	    if (newIntersection == intersection)
	    {
		intersection	= new TrackSurfaceIntersection(*intersection);
	    }
	    else
	    {
		intersection	= newIntersection;
	    }
	    if ((**s).materialEffectsOnTrack())
	    {
		Amg::Vector3D position	= intersection->position();
		bool calo		= (! m_indetVolume->inside(position)
					   && m_calorimeterVolume->inside(position));
		measurement1	= new FitMeasurement((**s).materialEffectsOnTrack(),
						     ParticleMasses().mass[particleHypothesis],
						     intersection->position(),
						     qOverP,
						     calo);
		if (! calo
		    && ! haveMaterial
		    && haveMeasurement)
		    haveMaterial	= true;
	    }
	    else if (! measurement2)
	    {
		// TSOS skipped
		m_messageHelper->printWarning(19, (**s).dumpType());
		// delete intersection;
		continue;
	    }
	}

	// check if ordering OK
	if (! reorder)
	{
	    double distance		= startDirection.dot(intersection->position() - startPosition);
	    Amg::Vector3D positionMst	= startPosition;
	    if ((**s).measurementOnTrack()) positionMst = (**s).measurementOnTrack()->globalPosition();
	    if ((**s).materialEffectsOnTrack())
		positionMst = (**s).materialEffectsOnTrack()->associatedSurface().center();
	    double distanceR		= sqrt(
		(positionMst.x() - startPosition.x())*(positionMst.x() - startPosition.x()) +
		(positionMst.y() - startPosition.y())*(positionMst.y() - startPosition.y()) );
	    double distanceZ		= (positionMst.z() - startPosition.z());
	    if (startDirection.z() < 0) distanceZ = -distanceZ;
	    if (distance < previousDistance
		&& distanceR < previousDistanceR
		&& distanceZ < previousDistanceZ)
	    {
              reorder = true;
              ATH_MSG_DEBUG( " reorder 3D distance " << distance - previousDistance
			     << " R distance " << distanceR - previousDistanceR
			     << " Z distance " << distanceZ - previousDistanceZ ); 
            }
	    previousDistance	= distance - m_orderingTolerance;
	    previousDistanceR	= distanceR - m_orderingTolerance;
	    previousDistanceZ	= distanceZ - m_orderingTolerance;
	}
	
	// insert measurement(s) in list
	if (measurement1)
	{
	    if (measurement2)
	    {
		const TrackSurfaceIntersection* intersectionCopy =
		    new TrackSurfaceIntersection(*intersection);
		
		measurement1->intersection(type,intersectionCopy);
		measurements.push_back(measurement1);
	    }
	    else
	    {
		measurement1->intersection(type,intersection);
		measurement1->qOverP(qOverP);
		measurements.push_back(measurement1);
	    }
	}
	if (measurement2)
	{
	    measurement2->intersection(type,intersection);
	    measurement2->qOverP(qOverP);
	    measurements.push_back(measurement2);
	}
    }

    // reorder if necessary
    if (reorder) m_materialAllocator->orderMeasurements(measurements,startDirection,startPosition);
    if (measurementsFlipped)
    {
	m_messageHelper->printWarning(25);
    }
    
    // flag whether material has already been allocated
    delete vertex;
    return haveMaterial;
}

std::list<FitMeasurement*>*&
iPatFitter::measurementList (void) const
{
    if (m_globalFit && m_measurements)
    {
	for (std::list<Trk::FitMeasurement*>::iterator m = m_measurements->begin();
	     m != m_measurements->end();
	     ++m)
	{
	    delete *m;
	}
	m_measurements->clear();
    }
    else
    {
	m_measurements	= new std::list<FitMeasurement*>;
    }

    return m_measurements;
}
     
Track*
iPatFitter::performFit(std::list<FitMeasurement*>*			measurements,
		       FitParameters*					parameters,
		       const ParticleHypothesis				particleHypothesis,
		       const TrackInfo&					trackInfo,
		       const DataVector<const TrackStateOnSurface>*	leadingTSOS,
		       const FitQuality*				perigeeQuality) const
{
    // initialize the scattering centres
    m_materialAllocator->initializeScattering(*measurements);
    
    // set fixed momentum in lineFit case according to configuration
    if (m_lineFit)
    {
	parameters->fitMomentum(false);
	if (m_lineMomentum > 0.) parameters->qOverP(1./m_lineMomentum);
    }
    
    // perform fit
    MsgStream log(msgSvc(), name());
    const FitProcedureQuality& quality	= m_fitProcedure->execute(m_asymmetricCaloEnergy,
								  log,
								  *measurements,
								  parameters,
								  perigeeQuality);
    Track* fittedTrack			= 0;
    if (! quality.fitCode())
    {
// 	// iterate if material reallocation needed
// 	if (m_materialAllocator->reallocateMaterial(*measurements,parameters))
// 	{
// 	    // do something
// 	    quality	= m_fitProcedure->execute(log,
// 						  *measurements,
// 						  parameters,
// 						  perigeeQuality);
// 	}

	// include leading material
	m_materialAllocator->addLeadingMaterial(*measurements,particleHypothesis,*parameters);


	// construct the fitted track
	fittedTrack		=  m_fitProcedure->constructTrack(*measurements,
								  *parameters,
								  trackInfo,
								  leadingTSOS);
	if (fittedTrack)
	{
	    // set StraightLine when momentum unfitted
	    if (! parameters->fitMomentum())
		fittedTrack->info().setTrackProperties(TrackInfo::StraightTrack);

	    // special check for CaloDeposit - parameters must be inside calorimeter
	    for (DataVector<const TrackStateOnSurface>::const_iterator
		     s = fittedTrack->trackStateOnSurfaces()->begin();
		 s != fittedTrack->trackStateOnSurfaces()->end();
		 ++s)
	    {
		if (! (**s).type(TrackStateOnSurface::CaloDeposit))	continue;
		if ((**s).trackParameters())
		{
		    Amg::Vector3D position	= (**s).trackParameters()->position();
		    if (! m_indetVolume->inside(position)
			&& m_calorimeterVolume->inside(position))	break;
		}

		// something badly wrong: WARN and kill track
		// fail fit as CaloDeposit outside calo volume
		m_messageHelper->printWarning(20);
		delete fittedTrack;
		fittedTrack	 = 0;
		break;
	    }
	}
	
	// statistics for successful fit
	if (fittedTrack)
	{
	    ++m_countGoodFits;
	    m_countIterations	+= quality.iterations();
	    m_iterations	=  quality.iterations();

	    // report material
	    if (msgLvl(MSG::DEBUG))
	    {
		int calo		= 0;
		double caloX0		= 0.;
		double caloEloss	= 0.;
		int indet		= 0;
		double indetX0		= 0.;
		double indetEloss	= 0.;
		int spect		= 0;
		double spectX0		= 0.;
		double spectEloss	= 0.;
		for (std::list<FitMeasurement*>::const_iterator m = measurements->begin();
		     m != measurements->end();
		     ++m)
		{
		    if ((**m).isEnergyDeposit())
		    {
			++calo;
			caloEloss	+= (**m).energyLoss();
		    }
		    else if (! (**m).isScatterer())
		    {
			continue;
		    }
		
		    if (m_indetVolume->inside((**m).position()))
		    {
			++indet;
			indetX0	+= (**m).materialEffects()->thicknessInX0();
			indetEloss	+= (**m).energyLoss();
			// conflicting energy deposit sign for inDet material
			if ((**m).energyLoss()*indetEloss < 0.) m_messageHelper->printWarning(21);
			continue;
		    }
		    else if (m_calorimeterVolume->inside(
				 (**m).intersection(FittedTrajectory).position()))
		    {
			++calo;
			caloX0	+= (**m).materialEffects()->thicknessInX0();
			continue;
		    }
		
		    ++spect;
		    spectX0	+= (**m).materialEffects()->thicknessInX0();
		    spectEloss	+= (**m).energyLoss();
		    // conflicting energy deposit sign for spectrometer material
		    if ((**m).energyLoss()*spectEloss < 0.) m_messageHelper->printWarning(22);
		}

		// WARN in case of bug #56297
		if (calo)
		{
		    // excessive calorimeter energy loss
		    if (std::abs(caloEloss) > 200.*Gaudi::Units::GeV
			&& m_messageHelper->wouldPrintWarning(23))
		    {
			std::stringstream ss;
			ss << caloEloss/Gaudi::Units::GeV << " for track with P "
			   << fittedTrack->perigeeParameters()->momentum().mag()/Gaudi::Units::GeV
			   << " GeV" ;
			m_messageHelper->printWarning(23, ss.str());
		    }

		    // excessive spectrometer energy loss
		    if (std::abs(spectEloss) > 0.5*Gaudi::Units::GeV
			&& std::abs(spectEloss*caloX0) > std::abs(4.*caloEloss*spectX0)
			&& m_messageHelper->wouldPrintWarning(24))
		    { 
			std::stringstream ss;
			ss << spectEloss/Gaudi::Units::GeV
			   << " ( " << caloEloss/Gaudi::Units::GeV 
			   << " calorimeter energy loss). Track has P "
			   << fittedTrack->perigeeParameters()->momentum().mag()/Gaudi::Units::GeV
			   << " GeV" ;
			m_messageHelper->printWarning(24, ss.str());
		    }
		}
	    
		msg(MSG::VERBOSE) << std::setiosflags(std::ios::fixed);
		if (indet)
		{
		    msg() << std::setw(5) << indet << " indet scatterers with"
			  << " material: X0" << std::setw(6) << std::setprecision(3) << indetX0;
		    if (calo && caloEloss < 0.)
		    {
			msg() << "  energyGain";
		    }
		    else
		    {
			msg() << "  energyLoss";
		    }
		    msg() << std::setw(6) << std::setprecision(3) << indetEloss/Gaudi::Units::GeV << ",";
		}
	    
		if (spect)
		{
		    msg() << std::setw(5) << spect << " spectrometer scatterers with"
			  << " material: X0" << std::setw(8) << std::setprecision(3) << spectX0;
		    if (calo && caloEloss < 0.)
		    {
			msg() << "  energyGain";
		    }
		    else
		    {
			msg() << "  energyLoss";
		    }
		    msg() << std::setw(7) << std::setprecision(3) << spectEloss/Gaudi::Units::GeV;
		}
	    
		if (! indet && ! spect) msg() << "    0 scatterers - no tracking material";
		msg() << endmsg;
	    }
	}
    }
    
    // clean up and return
    if (! m_globalFit || ! fittedTrack)
    {
	for (std::list<Trk::FitMeasurement*>::iterator m = measurements->begin();
	     m != measurements->end();
	     ++m)
	{
	    delete *m;
	}
	measurements->clear();
	delete measurements;
	delete parameters;
	m_measurements	= 0;
	m_parameters	= 0;
    }
    m_materialAllocator->clear();

    return fittedTrack;
}

void
iPatFitter::printTSOS (const Track& track) const
{
    // debugging aid
    MsgStream log(msgSvc(), name());
    msg(MSG::INFO) << " track with " << track.trackStateOnSurfaces()->size() << " TSOS " << endmsg;
    int tsos = 0;
    for (DataVector<const TrackStateOnSurface>::const_iterator t = track.trackStateOnSurfaces()->begin();
	 t !=  track.trackStateOnSurfaces()->end();
	 ++t, ++tsos)
    {
	msg() << std::setiosflags(std::ios::fixed|std::ios::right)
	      << " TSOS# " << std::setw(3)	<< tsos
	      << "   parameters:   "
	      << std::setw(7) << std::setprecision(1)
	      << (**t).trackParameters()->position().perp()
	      << std::setw(8) << std::setprecision(4)
	      << (**t).trackParameters()->position().phi()
	      << std::setw(9) << std::setprecision(1)
	      << (**t).trackParameters()->position().z()
	      << " position  "
	      << std::setw(8) << std::setprecision(4)
	      << (**t).trackParameters()->momentum().phi()
	      << " phi  "
	      << std::setw(7) << std::setprecision(4)
	      << (**t).trackParameters()->momentum().theta()
	      << " theta  "
	      << std::setw(9) << std::setprecision(4)
	      << (**t).trackParameters()->momentum().mag()/Gaudi::Units::GeV << " GeV";

	if ((**t).measurementOnTrack())
	{
	    msg() << "  meas ";
	}
	else
	{
	    msg() << "       ";
	}
	if ((**t).materialEffectsOnTrack())
	{
	    msg() << "  scat ";
	}
	else
	{
	    msg() << "       ";
	}
	msg() << (**t).dumpType() << endmsg;
    }
}
 
void
iPatFitter::refit(const Track&			track,
		  const RunOutlierRemoval	runOutlier,
		  const ParticleHypothesis	particleHypothesis) const
{
    ATH_MSG_VERBOSE( " refit " );
    unsigned countGoodFits	=  m_countGoodFits;
    unsigned countIterations	=  m_countIterations;
    ++m_countRefitAttempts;
    // outlier removal not implemented
    if (runOutlier) m_messageHelper->printWarning(0);    

    // create Perigee if starting parameters are for a different surface type
    const Perigee* perigee		= track.perigeeParameters();
    PerigeeSurface* perigeeSurface	= 0;
    if (! perigee)
    {
	DataVector<const TrackStateOnSurface>::const_iterator s = track.trackStateOnSurfaces()->begin();
	while (s != track.trackStateOnSurfaces()->end() && ! (**s).trackParameters()) ++s;
	if (! (**s).trackParameters())
	{
	    // input track without parameters
	    m_messageHelper->printWarning(1);
	    m_countGoodRefits		+= m_countGoodFits - countGoodFits;
	    m_countGoodFits		=  countGoodFits;
	    m_countRefitIterations	+= m_countIterations - countIterations;
	    m_countIterations		=  countIterations;
	    return;
	}
	
	Amg::Vector3D origin((**s).trackParameters()->position());
	perigeeSurface	= new PerigeeSurface(origin);
	perigee		= new Perigee((**s).trackParameters()->position(),
				      (**s).trackParameters()->momentum(),
				      (**s).trackParameters()->charge(),
				      *perigeeSurface);
    }

    delete m_parameters;
    m_parameters	= new FitParameters(*perigee);
    
    // set up the measurements
    if (! track.trackStateOnSurfaces())
    {
	// input track without trackStateOnSurfaces
	m_messageHelper->printWarning(2);
	m_countGoodRefits	+= m_countGoodFits - countGoodFits;
	m_countGoodFits		=  countGoodFits;
	m_countRefitIterations	+= m_countIterations - countIterations;
	m_countIterations	=  countIterations;
	return;
    }
    
    std::list<FitMeasurement*>* measurements = measurementList();
    bool haveMaterial = addMeasurements(*measurements,
					*m_parameters,
					particleHypothesis,
					*track.trackStateOnSurfaces());

    // allocate material
    if (! haveMaterial && particleHypothesis != Trk::nonInteracting)
    {
	m_materialAllocator->allocateMaterial(*measurements,
					      particleHypothesis,
					      *m_parameters,
					      *perigee);
    }

    // perform fit and return fitted track
    TrackInfo trackInfo(TrackInfo::iPatTrackFitter,particleHypothesis);
    trackInfo.addPatternReco(track.info());
    Trk::Track* fittedTrack = performFit(measurements, m_parameters, particleHypothesis, trackInfo);
    if (perigeeSurface)
    {
	delete perigeeSurface;
	delete perigee;
    }
    m_countGoodRefits		+= m_countGoodFits - countGoodFits;
    m_countGoodFits		=  countGoodFits;
    m_countRefitIterations	+= m_countIterations - countIterations;
    m_countIterations		=  countIterations;
    delete fittedTrack;	
}

} // end of namespace
