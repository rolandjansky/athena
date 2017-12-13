/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//////////////////////////////////////////////////////////////////////////////
// class FitProcedure
//  Basic iterative least square fit procedure producing a fitted Track
//
//  execute and constructTrack methods allow use from standard ITrackFitter
//  and iPatRec specific Algtools.
//  This class is kept transient rather than a tool as there are many state variables.
//
//  for clarity (i.e. to avoid an overly long class) the inner loop over
//  measurements is performed by MeasurementProcessor
//
//  (c) ATLAS tracking software
//////////////////////////////////////////////////////////////////////////////


#include <cmath>
#include <iomanip>
#include "CLHEP/GenericFunctions/CumulativeChiSquare.hh"
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/SystemOfUnits.h"
#include "TrkEventPrimitives/FitQuality.h"
#include "TrkExInterfaces/IIntersector.h"
#include "TrkExInterfaces/IPropagator.h"
#include "TrkGeometry/TrackingVolume.h"
#include "TrkMaterialOnTrack/EnergyLoss.h"
#include "TrkMaterialOnTrack/MaterialEffectsOnTrack.h"
#include "TrkMaterialOnTrack/ScatteringAngles.h"
#include "TrkMeasurementBase/MeasurementBase.h"
#include "TrkParameters/TrackParameters.h"
#include "TrkTrack/AlignmentEffectsOnTrack.h"
#include "TrkTrack/Track.h"
#include "TrkTrack/TrackInfo.h"
#include "TrkiPatFitterUtils/FitMatrices.h"
#include "TrkiPatFitterUtils/FitMeasurement.h"
#include "TrkiPatFitterUtils/FitParameters.h"
#include "TrkiPatFitterUtils/FitProcedure.h"
#include "TrkiPatFitterUtils/FitProcedureQuality.h"
#include "TrkiPatFitterUtils/MeasurementProcessor.h"

namespace Trk{
  
//<<<<<< CLASS STRUCTURE INITIALIZATION                                 >>>>>>


// constructor
FitProcedure::FitProcedure (bool				constrainedAlignmentEffects,
			    bool				extendedDebug,
			    bool				eigenMatrixTreatment,
			    bool				lineFit,
			    ToolHandle<IIntersector>&		rungeKuttaIntersector,
			    ToolHandle<IIntersector>&		solenoidalIntersector,
			    ToolHandle<IIntersector>&		straightLineIntersector,
                            const ToolHandle<IPropagator>&      stepPropagator,
			    const Volume*			indetVolume,
			    int					maxIterations,
                            int                                 useStepPropagator)
    :	m_chRatio1			(0.),
	m_chRatio2			(0.),
	m_chiSq				(0.),
	m_chiSqCut			(20.),
	m_chiSqMin			(0.),
	m_chiSqOld			(0.),
	m_chiSqWorst			(0.),
	m_convergence			(false),
	m_cutStep			(false),
	m_cutTaken			(0),
	m_debug				(false),
	m_driftSum			(0.),
	m_driftSumLast			(0.),
	m_extendedDebug 		(extendedDebug),
	m_extremeOneOverP		(1./(10.*Gaudi::Units::TeV)),
	m_fitMatrices  			(new FitMatrices(constrainedAlignmentEffects,
							 eigenMatrixTreatment)),
	m_fitProbability		(0.),
	m_fitQuality			(0),
	m_indetVolume			(indetVolume),
	m_iteration			(0),
	m_largeRadius			(1000.*Gaudi::Units::mm),
	m_lineFit			(lineFit),
	m_log				(0),
 	m_maxIter			(maxIterations),
 	m_minIter			(0),
	m_minPt				(0.05*Gaudi::Units::GeV),
	m_nCuts				(0),
	m_nearConvergence		(false),
	m_numberDoF	 		(0),
	m_numberParameters		(0),
	m_rungeKuttaIntersector 	(rungeKuttaIntersector),
	m_scatteringConstant		(13.6*Gaudi::Units::MeV),	// Coulomb scattering constant
	m_scatteringLogCoeff		(0.038),			// Coulomb scattering constant
	m_solenoidalIntersector 	(solenoidalIntersector),
	m_straightLineIntersector	(straightLineIntersector),
	m_stepPropagator 	        (stepPropagator),
        m_useStepPropagator             (useStepPropagator),
	m_verbose			(false),
	m_worstMeasurement		(0)
{}

// destructor
FitProcedure::~FitProcedure (void)
{
    delete m_fitMatrices;
    delete m_fitQuality;
}

//<<<<<< PUBLIC MEMBER FUNCTION DEFINITIONS                             >>>>>>

Track*
FitProcedure::constructTrack (const std::vector<FitMeasurement*>&		measurements,
			      const FitParameters&				parameters,
			      const TrackInfo&					trackInfo,
			      const DataVector<const TrackStateOnSurface>*	leadingTSOS)
{
    // debug
    if (m_debug) reportQuality(measurements,parameters);

    // NB keep first and last measurements distinct i.e. separate TSOS (no scatterers etc)
    // NB trackParameters outwards from TSOS i.e. always last FitMeas on surface
    
    // create vector of TSOS - reserve upper limit for size (+1 as starts with perigee)
    DataVector<const TrackStateOnSurface>* trackStateOnSurfaces =
	new DataVector<const TrackStateOnSurface>;
    unsigned size = measurements.size() + 1;
    if (leadingTSOS) size += leadingTSOS->size();
    trackStateOnSurfaces->reserve(size);
    const AlignmentEffectsOnTrack*	alignmentEffects	= 0;
    const FitMeasurement*		fitMeasurement		= measurements.front();
    const FitQualityOnSurface*		fitQoS			= 0;
    const MaterialEffectsBase*		materialEffects		= 0;
    const MeasurementBase*		measurementBase		= 0;
    const Surface*			surface			= 0;
    const TrackParameters*		trackParameters		= 0;
    std::bitset<TrackStateOnSurface::NumberOfTrackStateOnSurfaceTypes> defaultPattern;
    std::bitset<TrackStateOnSurface::NumberOfTrackStateOnSurfaceTypes> typePattern = defaultPattern;

    // start with (measured) perigee
    unsigned scatter		= 0;
    unsigned tsos		= 0;
    const Perigee* perigee	= parameters.perigee();
    typePattern.set(TrackStateOnSurface::Perigee);
    trackStateOnSurfaces->push_back(new TrackStateOnSurface(measurementBase,
							    perigee,
							    fitQoS,
							    materialEffects,
							    typePattern,
							    alignmentEffects));
    ++tsos;

    // append leading TSOS to perigee
    if (leadingTSOS)
    {
	for (DataVector<const TrackStateOnSurface>::const_iterator t = leadingTSOS->begin();
	     t != leadingTSOS->end();
	     ++t)
	{
	    if (! (**t).type(Trk::TrackStateOnSurface::Perigee))
	    {
		trackStateOnSurfaces->push_back((**t).clone());
		++tsos;
	    }
	}
    }
    
    // then append the fitted TSOS
    for (auto m : measurements)
    {
	if (m->isMaterialDelimiter()) continue;
	
	// push back previous TSOS when fresh surface reached
	if (m->surface() != surface || alignmentEffects || m->alignmentEffects())
	{
	    if (surface)
	    {
		if (typePattern == defaultPattern)
		{
		    if (m_debug)
		    {
			*m_log << MSG::DEBUG << " skip empty TSOS# " << tsos + 1;
			if (m->materialEffects()) *m_log << " with material";
			m->print(*m_log);
			*m_log << endmsg;
		    }
		}
		else
		{
		    // get the MeasuredParameters (with covariance)
		    bool withCovariance	= true;
		    trackParameters	= parameters.trackParameters(*m_log,
								     *fitMeasurement,
								     withCovariance);
		    if (! trackParameters)
		    {
			*m_log << MSG::WARNING
			       << " fail track with incomplete return TSOS: no trackParameters"
			       << endmsg;
			delete trackStateOnSurfaces;
			return 0;
		    }
		    typePattern.set(TrackStateOnSurface::Parameter);
		    trackStateOnSurfaces->push_back(new TrackStateOnSurface(measurementBase,
									    trackParameters,
									    fitQoS,
									    materialEffects,
									    typePattern,
									    alignmentEffects));
		    ++tsos;
		}
	    }
	    fitMeasurement	= m;
	    surface 		= m->surface();
	    measurementBase	= 0;
	    fitQoS		= 0;
	    materialEffects	= 0;
	    typePattern		= defaultPattern;
	    alignmentEffects	= 0;
	}
	else
	{
	    fitMeasurement	= m;
	    if (m_verbose) *m_log << MSG::VERBOSE << " tsos# " << tsos << " shared surface" << endmsg;
	}

	// it's a measurement
	if (m->measurementBase())
	{
	    // create an extra TSOS if there is already a measurement on this surface
	    // (dirty fix for pseudoMeasurements)
	    if (measurementBase)
	    {
		// get the MeasuredParameters (with covariance)
		bool withCovariance	= true;
		trackParameters		= parameters.trackParameters(*m_log,
								     *fitMeasurement,
								     withCovariance);
		if (! trackParameters)
		{
		    *m_log << MSG::WARNING
			   << " fail track with incomplete return TSOS: no trackParameters"
			   << endmsg;
		    delete trackStateOnSurfaces;
		    return 0;
		}
		typePattern.set(TrackStateOnSurface::Parameter);
		trackStateOnSurfaces->push_back(new TrackStateOnSurface(measurementBase,
									trackParameters,
									fitQoS,
									materialEffects,
									typePattern,
									alignmentEffects));
		++tsos;
		fitMeasurement		= m;
		fitQoS			= 0;
		materialEffects		= 0;
		typePattern		= defaultPattern;
		alignmentEffects	= 0;
	    }
	    
	    measurementBase	= m->measurementBase()->clone();
	    typePattern.set(TrackStateOnSurface::Measurement);
	    if (m->isOutlier()) typePattern.set(TrackStateOnSurface::Outlier);
	}

	// it's a CaloDeposit or Scatterer (scatterers may be fitted or not fitted)
	if (m->materialEffects())
	{
	    // update momentum to account for energy loss
	    delete materialEffects;

	    if (m->isEnergyDeposit())
	    {
		materialEffects	= m->materialEffects()->clone();
		typePattern.set(TrackStateOnSurface::CaloDeposit);
	    }
	    else if (m->isScatterer())
	    {
		// set materialPattern as the scattering parameters are fitted
		std::bitset<MaterialEffectsBase::NumberOfMaterialEffectsTypes> typeMaterial;
		typeMaterial.set(MaterialEffectsBase::FittedMaterialEffects);
		const MaterialEffectsOnTrack* meot	=
		    dynamic_cast<const MaterialEffectsOnTrack*>(m->materialEffects());
		if (meot && meot->energyLoss())	// standard scatterer
		{
		    const EnergyLoss* energyLoss = meot->energyLoss()->clone();
		    typeMaterial.set(Trk::MaterialEffectsBase::EnergyLossEffects);
		    if (m->numberDoF())	// fitted scatterer	
		    {
			materialEffects	=
			    new MaterialEffectsOnTrack(m->materialEffects()->thicknessInX0(),
						       parameters.scatteringAngles(*m,scatter), 
						       energyLoss,
						       m->materialEffects()->associatedSurface(),
						       typeMaterial);
			++scatter;
		    }
		    else			// unfitted (leading material)
		    {
			materialEffects	=
			    new MaterialEffectsOnTrack(m->materialEffects()->thicknessInX0(),
						       parameters.scatteringAngles(*m), 
						       energyLoss,
						       m->materialEffects()->associatedSurface(),
						       typeMaterial);
		    }
		}
		else				// no meot for special calo scattering centres
		{
		    if (m->numberDoF())	// fitted scatterer
		    {
			materialEffects	=
			    new MaterialEffectsOnTrack(m->materialEffects()->thicknessInX0(),
						       parameters.scatteringAngles(*m,scatter), 
						       m->materialEffects()->associatedSurface(),
						       typeMaterial);
			++scatter;
		    }
		    else			// unfitted (leading material)
		    {
			materialEffects	=
			    new MaterialEffectsOnTrack(m->materialEffects()->thicknessInX0(),
						       parameters.scatteringAngles(*m), 
						       m->materialEffects()->associatedSurface(),
						       typeMaterial);
		    }
		}
		
		typePattern.set(TrackStateOnSurface::Scatterer);
	    }
	    else
	    {
		*m_log << MSG::WARNING <<" deprecated TrackStateOnSurface::InertMaterial" << endmsg;
		materialEffects	= m->materialEffects()->clone();
		typePattern.set(TrackStateOnSurface::InertMaterial);
	    }
	}

	// additional perigee (e.g. at MS entrance)
	if (m->isPerigee())
	{
	    typePattern.set(TrackStateOnSurface::Perigee);
	}
	
	// or alignment effects
	else if (m->alignmentEffects())
	{
	    const AlignmentEffectsOnTrack&	AEOT	= *m->alignmentEffects();
	    unsigned align				= m->alignmentParameter() - 1;

            *m_log << MSG::VERBOSE  <<" Fitprocedure AEOT input deltaTranslation " << AEOT.deltaTranslation() << " deltaAngle " << AEOT.deltaAngle() << " output Trans " << parameters.alignmentOffset(align) << " deltaAngle " << parameters.alignmentAngle(align) << endmsg; 
	    alignmentEffects				=
		new Trk::AlignmentEffectsOnTrack(parameters.alignmentOffset(align),
						 AEOT.sigmaDeltaTranslation(),
						 parameters.alignmentAngle(align),
						 AEOT.sigmaDeltaAngle(),
						 AEOT.vectorOfAffectedTSOS(),
						 m->surface());
	    typePattern.set(TrackStateOnSurface::Alignment);
	}
	
	// passive types: hole for now
	else if (m->isPassive())
	{
	    if (m->type() == hole)		typePattern.set(TrackStateOnSurface::Hole);
	}
    }

    // remember the final TSOS !
    bool withCovariance	= true;
    trackParameters	= parameters.trackParameters(*m_log, *fitMeasurement, withCovariance);
    if (! trackParameters)
    {
	*m_log << MSG::WARNING
	       << " fail track with incomplete return TSOS: no trackParameters"
	       << endmsg;
	delete trackStateOnSurfaces;
	return 0;
    }
    typePattern.set(TrackStateOnSurface::Parameter);
    trackStateOnSurfaces->push_back(new TrackStateOnSurface(measurementBase,
							    trackParameters,
							    fitQoS,
							    materialEffects,
							    typePattern,
							    alignmentEffects));
    ++tsos;

    // construct track
    double chiSquared	= m_chiSq * static_cast<double>(m_numberDoF);
    Track* track	= new Track(trackInfo,
				    trackStateOnSurfaces,
				    new FitQuality(chiSquared,m_numberDoF));

    if (m_verbose) *m_log << MSG::VERBOSE << " track with " << tsos << " TSOS " << endmsg;
    return track;
}

const FitProcedureQuality&
FitProcedure::execute(bool				asymmetricCaloEnergy,
		      MsgStream&			log,
		      std::vector<FitMeasurement*>&	measurements,
		      FitParameters*&			parameters,
		      const FitQuality*			perigeeQuality,
		      bool				for_iPatTrack)
{
    // report start value
    m_log = &log;
    if (m_log->level() > MSG::DEBUG)
    {
	m_debug		= false;
	m_verbose	= false;
    }
    else
    {
	m_debug		= true;
	if (m_log->level() < MSG::DEBUG)  m_verbose = true;
	*m_log << MSG::DEBUG << "parameter start values:  ";
	parameters->print(*m_log);
	*m_log << endmsg;
    }
	
    // choose appropriate intersector
    ToolHandle<IIntersector>& intersector = chooseIntersector(measurements,*parameters);

    // resize matrices
    int fitCode		= m_fitMatrices->setDimensions(measurements,parameters);
    if (fitCode)
    {
	delete m_fitQuality;
	m_fitQuality	= new FitProcedureQuality(fitCode,m_fitMatrices->numberDoF());
	if (m_debug) reportQuality(measurements,*parameters);
	return *m_fitQuality;
    }

    // remaining initialization
    m_chiSq		= 0.;
    m_chiSqWorst	= 0.;
    m_driftSum		= 0.;
    m_driftSumLast	= 0.;
    m_fitProbability	= 0.;
    m_iteration		= -1;
    m_numberDoF		= m_fitMatrices->numberDoF();
    m_numberParameters	= parameters->numberParameters();
    m_worstMeasurement	= 0;
    MeasurementProcessor measurementProcessor(asymmetricCaloEnergy,
					      intersector,
					      measurements,
					      parameters,
					      m_rungeKuttaIntersector,
                                              m_stepPropagator,
                                              m_useStepPropagator);

    // perigee or vertex used as measurements in fit
    if (measurements.front()->isPerigee())
    {
	m_fitMatrices->usePerigee(*measurements.front());
    }
    
    // set requested options and initial values
    double ptInvCut	= 1./m_minPt; // protection against trapped particles        
    m_cutStep	  	= true;
    m_convergence	= false;
    m_nearConvergence	= false;
    
    // keep best (original if not reasonable quality) results
    double bestChiSq			= m_chiSqCut;
    FitParameters* bestParameters	= 0;

    // iteration loop to fit track parameters
    while (! fitCode && ! m_convergence)
    {
	bool forceIteration	= false;
	if (m_iteration > m_maxIter && bestParameters && ! for_iPatTrack)
	{
	    parameters->reset(*bestParameters);
	    m_convergence	= true;
	    m_cutStep		= false;
	    if (m_verbose)
		*m_log << MSG::VERBOSE << " convergence problem: accept after max iter " << endmsg;
	}
	else if (! m_cutStep)
	{
	    //  solve equations and update parameters
	    if (! m_iteration)
	    {
		m_fitMatrices->refinePointers();
		if (m_extendedDebug)	m_fitMatrices->checkPointers(*m_log);
		if (m_verbose)		m_fitMatrices->printDerivativeMatrix();
	    }
	    
	    if (! m_fitMatrices->solveEquations())
	    {
		fitCode			= 11;	// fails matrix inversion
	    }
	    else if (parameters->fitEnergyDeposit()
		     && ! parameters->extremeMomentum()
		     && std::abs(parameters->qOverP()) < m_extremeOneOverP)
	    {
		if (m_debug) *m_log << MSG::DEBUG << " extremeMomentum " << endmsg;
		parameters->extremeMomentum(true);
		delete bestParameters;
		fitCode			= m_fitMatrices->setDimensions(measurements,parameters);
		bestChiSq		= m_chiSqCut;
		bestParameters		= 0;
		forceIteration		= true;
		m_chiSq			= 0.;
		m_chiSqWorst		= 0.;
		m_driftSum		= 0.;
		m_driftSumLast		= 0.;
		m_numberParameters	= parameters->numberParameters();
	    }
	    if (m_verbose && ! m_iteration) m_fitMatrices->printWeightMatrix();
	}
	++m_iteration;
	
	// report parameters 
	if (m_verbose)
	{
	    *m_log << MSG::VERBOSE << " ===== start iteration " << m_iteration;
	    if (m_iteration)
	    {
		if (m_cutStep) *m_log << " ====== cutStep";
	    }
	    else
	    {
		if (for_iPatTrack) *m_log << "  ====== for_iPatTrack ";
	    }
	    parameters->printVerbose(*m_log);
	}
	
	//  check for some error conditions (if none found yet)
	if (fitCode)
	{
	    // e.g. fitCode == 11 (singular matrix)
	}
	else if (std::abs(parameters->ptInv0()) > ptInvCut)
	{
	    fitCode = 8;           //  fail with pt below cutoff
	}
	else if (measurements.front()->isVertex()
		 && m_indetVolume
		 && ! m_indetVolume->inside(parameters->position()))
	{
	    fitCode = 9;	// fail if vertex outside indet
	}
	else if (m_iteration
		 && (std::abs(parameters->difference(3)) > 1.0
		     || std::abs(parameters->difference(0)) > m_largeRadius)
		 && ! measurements.front()->isVertex())
	{
	    if (std::abs(parameters->difference(3)) > 1.0)
	    {
		fitCode = 10;	// fail with ill-defined cot_theta
	    }
	    else
	    {
		fitCode = 9;	// fail crazy impact parameter
	    }
	}
	else if (! fitCode)
	{
	    //  extrapolate to each measurement and calculate derivatives
	    if (! measurementProcessor.calculateFittedTrajectory(m_iteration)
		|| ! measurementProcessor.calculateDerivatives())
	    {
		fitCode = 5; //  fail as trapped
		delete bestParameters;
		delete m_fitQuality;
		m_fitQuality = new FitProcedureQuality(m_chiSq,
						       m_chiSqWorst,
						       m_fitProbability,
						       fitCode,
						       m_iteration,
						       parameters->numberAlignments(),
						       m_fitMatrices->numberDoF(),
						       parameters->numberScatterers(),
						       m_worstMeasurement);
		
		if (m_debug)
		{
		    if (m_verbose) *m_log << endmsg;
		    reportQuality(measurements,*parameters);
		}
		return *m_fitQuality;
	    }

	    //  have extrapolation and derivatives, calculate residual
	    measurementProcessor.calculateResiduals();
	    
	    // check for remaining error conditions. If OK then compute chisquared.
	    if (m_iteration > m_maxIter && ! m_cutStep && for_iPatTrack)
	    {
		fitCode = 6; //  fail with no convergence
	    }
	    else if (m_iteration == 4 && m_chiSq > 1000. && for_iPatTrack)
	    {
		fitCode = 7;  //  fail with too high chisquared 
	    }
	    else if (! fitCode)
	    {
		calculateChiSq(measurements);

		// check for cutstep conditions if no significant chi2 improvement
		if (! forceIteration && ! m_convergence && m_chRatio1 > 0.9)
		{
		    double cutStep	= 0.;
		    if (m_iteration > 4 && m_driftSum*m_driftSumLast < -1.)
		    {
			m_cutStep	= true;
			cutStep		= std::abs(m_driftSumLast / (m_driftSum - m_driftSumLast));
			if (cutStep < 0.001) cutStep = 0.001;
			if (m_verbose)
			    *m_log << MSG::VERBOSE
				   << " take cutStep following chi2 increase on iteration "
				   << m_iteration
				   << "  chi2Ratio " << m_chRatio1
				   << "   driftSum " << m_driftSum << " prev " << m_driftSumLast
				   << "   " << cutStep << endmsg;
		    }
		    else if (parameters->numberOscillations() > 2)
		    {
			m_cutStep	= true;
			cutStep		= 0.5;
			if (m_verbose)
			    *m_log << MSG::VERBOSE
				   << " take cutStep as oscillating, iteration "
				   << m_iteration << ", numberOscillations "
				   << parameters->numberOscillations() << endmsg;
		    }
		    
		    // perform cutstep
		    if (m_cutStep)
		    {
			m_convergence	= false;
			parameters->performCutStep(cutStep);
			if (m_verbose) parameters->printVerbose(*m_log);
			if (measurementProcessor.calculateFittedTrajectory(m_iteration))
			{
			    // note: derivatives should not be recalculated for cutstep
			    measurementProcessor.calculateResiduals();
			    calculateChiSq(measurements);
			    if (m_verbose) *m_log << "   after cutStep: "
						  << "  chi2Ratio " << m_chRatio1
						  << "   driftSum " << m_driftSum
						  << endmsg;
			}
		    }
		}

		// keep current best parameters
		if (! bestParameters || m_chiSq < bestChiSq)
		{
		    bestChiSq		= m_chiSq;
		    delete bestParameters;
		    bestParameters	= new FitParameters(*parameters);
		    parameters->resetOscillations();
		}
		
		if (bestParameters
		    && ((m_convergence && m_chiSq > bestChiSq + 0.5)
			|| (parameters->phiInstability() && m_iteration == m_maxIter)))
		{
		    parameters->reset(*bestParameters);
		    if (m_verbose)
		    {
			*m_log << MSG::VERBOSE << " revert to bestParameters ";
			parameters->printVerbose(*m_log);
		    }
		    if (measurementProcessor.calculateFittedTrajectory(m_iteration))
		    {
		        measurementProcessor.calculateDerivatives();
			measurementProcessor.calculateResiduals();
			calculateChiSq(measurements);
			m_convergence = true;
		    }
		}

		if (forceIteration) m_convergence = false;
	    }
	}	// if (std::abs(ptInv0) > ptInvCut)
	if (m_verbose) *m_log << endmsg;

	// try to rescue phi instability failures
	if (fitCode
	    && m_iteration
	    && bestParameters
	    && ! parameters->phiInstability()
	    && (**(measurements.rbegin())).position().perp() > m_largeRadius)
	{
	    if (m_verbose) *m_log << MSG::VERBOSE << " phi instability " << endmsg;
	    parameters->reset(*bestParameters);
	    parameters->setPhiInstability();
	    m_cutStep		= true;
	    m_convergence	= false;
	    fitCode		= 0;
	    m_iteration		= 0;
	}
     }	// while

    // store successful fit :
    if (! fitCode)
    {
	// set covariance in parameters class after inclusion of uncertainty from field integral
	const Amg::MatrixX* fullCovariance	= m_fitMatrices->fullCovariance();
	if (fullCovariance)
	{
	    Amg::MatrixX* finalCovariance	= m_fitMatrices->finalCovariance();
	    if (! for_iPatTrack)
	    {
		if (! m_lineFit) measurementProcessor.fieldIntegralUncertainty(*m_log,*finalCovariance);
		measurementProcessor.propagationDerivatives();
	    }
	    parameters->covariance(finalCovariance,fullCovariance);

	    // fit quality
	    if (perigeeQuality)
	    {
		// take care when mixing normalized with unnormalized
		m_chiSq		=  perigeeQuality->chiSquared() +
				   m_chiSq * static_cast<double>(m_numberDoF);
		m_numberDoF	+= perigeeQuality->numberDoF();
		m_chiSq		/= static_cast<double>(m_numberDoF); 
	    }
	    
	    // probability of chisquared
	    m_fitProbability = 1.;
	    if (m_numberDoF > 0 && m_chiSq > 0.)
	    {
		if (m_chiSq < 100.)
		{
		    double chiSquared = m_chiSq * static_cast<double>(m_numberDoF);
		    m_fitProbability -= Genfun::CumulativeChiSquare(m_numberDoF)(chiSquared);
		}
		else
		{
		    m_fitProbability = 0.;
		}
	    }

	    if (m_verbose)
	    {
		*m_log << MSG::VERBOSE << " fit converged";
		if (m_chiSqWorst > 6.25) *m_log << " with possible outlier #" << m_worstMeasurement
						<< " (residual " << std::sqrt(m_chiSqWorst) << ")";
		*m_log << endmsg;
	    }
	}
	else
	{
	    fitCode = 11; //  singular weight matrix
	}
    }

    delete bestParameters;
    delete m_fitQuality;
    m_fitQuality = new FitProcedureQuality(m_chiSq,
					   m_chiSqWorst,
					   m_fitProbability,
					   fitCode,
					   m_iteration,
					   parameters->numberAlignments(),
					   m_numberDoF,
					   parameters->numberScatterers(),
					   m_worstMeasurement);
    if (m_debug && (for_iPatTrack || fitCode)) reportQuality(measurements,*parameters);

    return *m_fitQuality;
}
     
Amg::MatrixX*
FitProcedure::fullCovariance () const
{
    // note const_cast - ughhh
    // return const_cast<Amg::MatrixX*>(m_fitMatrices->fullCovariance());
    return 0;  // NOT mig5
}
       
void
FitProcedure::setMinIterations (int minIter)
{
    m_minIter = minIter;
    if (m_minIter > m_maxIter) m_maxIter = m_minIter;
}

//<<<<<< PRIVATE MEMBER FUNCTION DEFINITIONS                            >>>>>>

void
FitProcedure::calculateChiSq(std::vector<FitMeasurement*>& measurements)
{
    // convergence criterion
    const double dChisqConv = 0.025;
    
    // compute total chisquared and sum of hit differences
    // flag hit with highest chisquared contribution (on entry if RoadFit)
    m_chiSq		= 0.;
    double driftResidual= 0.;
    double DSqMax	= 0.;
    for (auto m : measurements)
    {
	if (! m->numberDoF()) continue;
	// if (m->isPerigee())
	// {
	//     m_chiSq += m_fitMatrices->perigeeChiSquared();
	//     continue;
	// }
	
	double residual	=  m->residual();
	double DiffSq	=  residual*residual;
	m_chiSq	+=  DiffSq;             
	if (m->isPositionMeasurement())
	{
	    if (m->isDrift())  driftResidual += residual;
	    if (DiffSq > DSqMax)
	    {
		DSqMax			= DiffSq;
		m_worstMeasurement	= m->hitIndex() + 1;
		m_chiSqWorst		= std::min(999.,DSqMax);
	    }
	}
	if (m->is2Dimensional())
	{
	    residual		=  m->residual2();
	    DiffSq		=  residual*residual;
	    m_chiSq	+=  DiffSq;
	    if (m->isPositionMeasurement())
	    {
		if (DiffSq > DSqMax)
		{
		    DSqMax		= DiffSq;
		    m_worstMeasurement	= m->hitIndex() + 1;
		    m_chiSqWorst	= std::min(999.,DSqMax);
		}
	    }
	}
    }

    // assess chi squared per degree of freedom (and its stability)
    if (m_fitMatrices->numberDoF() > 0) m_chiSq /= static_cast<double>(m_fitMatrices->numberDoF());
    if (m_iteration == 0)
    {
	m_cutTaken	= 0;
	m_chRatio1	= 0.;
	m_chRatio2	= 0.;
	m_chiSqMin	= m_chiSq;
    }
    
    m_chiSqOld = m_chiSqMin;
    double DChiSq = m_chiSqOld - m_chiSq;
    if (DChiSq > -dChisqConv)
    {
	m_chiSqMin	= m_chiSq;
	m_nCuts     	= 0;
    }
//     //  increase in chi2: increment nCuts -
//     //                    but no action taken until 2 successive increases
//     else if (! fitlocal.TestNewClass)
//     {
// 	++m_cutTaken;
// 	if (m_cuts++)
// 	{
// 	    for (int n = 0; n < m_numberParameters; ++n)
// 		fitlocal.dParMin[n] += fitlocal.dParam[n];
// 	}
// 	else
// 	{
// 	    for (int n = 0; n < m_numberParameters; ++n)
// 		fitlocal.dParMin[n] = fitlocal.dParam[n];
// 	}
//     }
    
//     if (m_cutTaken)
//     {
// 	*m_log << " Cut ??? Chi2,DChiSq " << m_chiSq
// 		  << "  " << DChiSq
// 		  << "  nIter,nDegF,nCuts " << m_iteration
// 		  << "  " << m_fitMatrices->numberDoF()
// 		  << "  " << m_nCuts
// 		  << std::setiosflags(std::ios::fixed)
// 		  << " transverse impact parameter"
// 		  << std::setw(10) << std::setprecision(4) << parameters->d0()
// 		  << "    Z0"                     
// 		  << std::setw(10) << std::setprecision(4) << parameters->z0()
// 		  << "    1/Pt"                     
// 		      << std::setw(12) << std::setprecision(6) << parameters->ptInv0();
// // 	for (int n = 0; n < m_numberParameters; ++n)
// // 	    *m_log << "  " << fitlocal.dParam[n];
// // 	*m_log << std::endl;
// // 	for (int n = 0; n < m_numberParameters; ++n)
// // 	    *m_log << "  " << fitlocal.dParMin[n];
// 	*m_log << std::endl;
//     }
   
    if (m_iteration > 0)
    {
	m_chRatio2 = m_chRatio1;
	m_chRatio1 = m_chiSq/m_chiSqOld;       
    }
    if (m_fitMatrices->numberDriftCircles())
    {
	m_driftSumLast  = m_driftSum;
	m_driftSum	= driftResidual/static_cast<double>(m_fitMatrices->numberDriftCircles());
    }
    
    //      
    // debugging info
    if (m_verbose)
    {
	*m_log << "----------------------------------" << std::endl
	       << std::setiosflags(std::ios::fixed)
	       << " Debugging Info in ChiSquare method" << std::endl
	       << " # of track-fit iterations " << std::setw(3) << m_iteration << std::endl
	       << " fit ChiSquared (per degree of freedom) "
	       << std::setw(13) << std::setprecision(3) << m_chiSq
	       << "   # of degrees of freedom " << m_fitMatrices->numberDoF() << std::endl
	       << " ChSq Ratio1/2 " << std::setw(9) << std::setprecision(3) << m_chRatio1
	       << std::setw(10) << std::setprecision(3) << m_chRatio2 << std::endl
	       << " driftResidual " << std::setw(9) << std::setprecision(3) << m_driftSum
	       << " #driftCircles " << m_fitMatrices->numberDriftCircles() << std::endl     
	       << " CutTaken " << m_cutTaken << std::endl
	       << "----------------------------------" << std::endl << "   ";

	(**measurements.begin()).printHeading(*m_log);
	int n = 0;
	for (auto m : measurements)
	{
	    *m_log << std::setiosflags(std::ios::fixed)
		   << std::setw(3) << ++n;
	    if (m->isPerigee())
	    {
		*m_log << " perigee     ";
		// if (m->numberDoF())
		//     *m_log << std::setw(14) << std::setprecision(3)
		// 	   << m_fitMatrices->perigeeChiSquared()
		// 	   << "  (chi2 for 5 degF) ";
		*m_log << std::endl;
	    }
	    else
	    {
		m->print(*m_log);
	    }
	}
    }
 
    //      
    // check for possible convergence (nearConvergence forces extra iteration)   
    if (! m_cutStep
	&& ! m_nCuts
	&& ( m_chiSq < 0.1
	     || (m_chRatio2 < 1.1
		 && (std::abs(DChiSq) < dChisqConv
		     || std::abs((m_chiSq-m_chiSqOld)/m_chiSqOld) < 0.01))))
    {
	if ((m_chiSq < 2.0 || m_nearConvergence || m_iteration == 1) && m_iteration >= m_minIter)
	{
	    m_convergence = true;
	}
	else
	{	    
	    m_nearConvergence = true;
	    if (m_verbose) *m_log << MSG::VERBOSE << " near convergence " << endmsg;
	}
    }
    else
    {
	m_nearConvergence = false;
    }
    
    // else take cutstep if divergent or oscillating
    m_cutStep = false;
//     else if (m_nCuts < 2 || m_nCuts > 4)
//     {
// 	m_cutStep = false;
//     }
//     else
//     {
// 	m_cutStep = true;
// 	if (m_nCuts == 2)
// 	{
// 	    for (int n = 0; n < m_numberParameters; ++n)
// 		fitlocal.dParam[n] =  -0.6667*fitlocal.dParMin[n];
// 	}
// 	else
// 	{
// 	    for (int n = 0; n < m_numberParameters; ++n)
// 		fitlocal.dParam[n] = 0.3333*fitlocal.dParam[n];
// 	}

// 	if (m_verbose)
// 	{
// 	    *m_log << "----------------------------------"       
// 		      << " Debugging Info in ChiSquare method" 
// 		      << " cutstep - nCuts, -ve chisquared change " << m_nCuts  
// 		      << "----------------------------------" << std::endl;       
// 	}
// 	m_convergence	=  false;
// 	m_chiSq		= m_chiSqMin;
//     }
}

ToolHandle<IIntersector>&
FitProcedure::chooseIntersector (std::vector<FitMeasurement*>&	measurements,
				 const FitParameters&		parameters) const
{
    if (m_lineFit) return m_straightLineIntersector;
    
    // decide which intersector to use for curved tracks (default RungeKutta)
    // ToolHandle<IIntersector>& intersector = m_rungeKuttaIntersector;

    // solenoidal intersector must start close to origin with last measurement inside valid region
    for (std::vector<FitMeasurement*>::reverse_iterator m = measurements.rbegin();
	 m != measurements.rend();
	 ++m)
    {
	if (! (**m).isPositionMeasurement())						continue;
	if (! m_solenoidalIntersector->isValid(parameters.position(),(**m).position()))	break;  
	return m_solenoidalIntersector;
    }

    return m_rungeKuttaIntersector;
}

void
FitProcedure::reportQuality(const std::vector<FitMeasurement*>&	measurements,
			    const FitParameters&		parameters) const
{
    if (! m_fitQuality) return;
    
    int fitCode	= m_fitQuality->fitCode();
    if (fitCode)
    {
	*m_log << MSG::DEBUG << "failure: fitCode " << fitCode;
	std::string msg = "";
	switch (fitCode)
	{
	case 1:
	    *m_log << "  missing Trk::Surface ";
	    break;    
	case 2:
	    *m_log << "  too many measurements for fit matrix size: "
		   << measurements.size();
	    break;
	case 3:
	    *m_log << "  too many parameters for fit matrix size: "
		   << parameters.numberParameters();
	    break;
	case 4:
	    *m_log << "  unconstrained fit: negative numberDoF "
		   << m_fitQuality->numberDoF();
	    break;
	case 5:
	    *m_log << "  trapped in magnetic field: pT = " << 1./(parameters.ptInv0()*Gaudi::Units::GeV)
		   << "  at iteration# " << m_fitQuality->iterations();
	    break;
	case 6:
	    *m_log << "  no convergence: chiSq = " << m_fitQuality->chiSq()
		   << "  at iteration# " << m_fitQuality->iterations();
	    break;
	case 7:
	    *m_log << "  enormous chi squared: chiSq = " << m_fitQuality->chiSq()
		   << "  at iteration# "<< m_fitQuality->iterations();
	    break;
	case 8:
	    *m_log << "  below pT cutoff. pT = " << 1./(parameters.ptInv0()*Gaudi::Units::GeV)
		   << "  at iteration# "<< m_fitQuality->iterations();
	    break;
	case 9:
	    *m_log << "  ill-defined impact parameter "<< parameters.d0()
		   << "  with difference " << parameters.difference(0)
		   << "  at iteration# "<< m_fitQuality->iterations();
	    break;
	case 10:
	    *m_log << "  ill-defined cotTheta " << parameters.cotTheta()
		   << "  with difference " << parameters.difference(3)
		   << "  at iteration# "<< m_fitQuality->iterations();
	    break;	
	case 11:
	    *m_log << "  singular matrix fails inversion:"
		   << "  at iteration# "<< m_fitQuality->iterations();
	    break;
	case 12:
	    *m_log << "  maximum of one calorimeter permitted";
	    break;	
	default:
	    break;
	};
	*m_log << std::endl << endmsg;
    }
    else
    {
	*m_log << MSG::DEBUG << "fitted parameter values: ";
	parameters.print(*m_log);
	*m_log << endmsg;
	*m_log << MSG::DEBUG;
	m_fitQuality->print(*m_log);
	parameters.printCovariance(*m_log);
	*m_log << endmsg;
    }
}

}	// end of namespace
