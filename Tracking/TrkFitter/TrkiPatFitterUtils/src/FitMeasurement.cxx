/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

/***************************************************************************
 for any measurement type (cluster, drift circle or material)
   stores the quantities needed during track fitting
   i.e. position, surface, weights, intersection, derivatives, residual etc
 ***************************************************************************/

#include "EventPrimitives/EventPrimitivesHelpers.h"
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/SystemOfUnits.h"
#include "GeoPrimitives/GeoPrimitives.h"
#include "TrkEventPrimitives/JacobianCotThetaPtToThetaP.h"
#include "TrkExUtils/TrackSurfaceIntersection.h"
#include "TrkMaterialOnTrack/EnergyLoss.h"
#include "TrkMaterialOnTrack/MaterialEffectsOnTrack.h"
#include "TrkMaterialOnTrack/ScatteringAngles.h"
#include "TrkMeasurementBase/MeasurementBase.h"
#include "TrkSurfaces/CylinderSurface.h"
#include "TrkSurfaces/DiscSurface.h"
#include "TrkSurfaces/PerigeeSurface.h"
#include "TrkSurfaces/PlaneSurface.h"
#include "TrkSurfaces/StraightLineSurface.h"
#include "TrkSurfaces/Surface.h"
#include "TrkSurfaces/RotatedTrapezoidBounds.h"
#include "TrkSurfaces/TrapezoidBounds.h"
#include "TrkTrack/AlignmentEffectsOnTrack.h"
#include "TrkTrack/TrackStateOnSurface.h"
#include "TrkiPatFitterUtils/FitMeasurement.h"

#include <cmath>
#include <iomanip>
#include <iostream>

namespace Trk{
  
// MeasurementBase
FitMeasurement::FitMeasurement (int		       	hitIndex,
				HitOnTrack*		hitOnTrack,
				const MeasurementBase*	measurementBase)
    : m_afterCalo		(false),
      m_alignmentEffects	(nullptr),
      m_alignmentParameter	(0),
      m_alignmentParameter2	(0),
      m_betaSquared		(1.),
      m_derivative		(nullptr),
      m_derivative2		(nullptr),
      m_derivativeRow		(-1),
      m_d0         		(0.),
      m_energyLoss		(0.),
      m_firstParameter		(0),
      m_flippedDriftDistance	(false),
      m_hitIndex		(hitIndex),
      m_hitOnTrack		(hitOnTrack),
      m_lastParameter		(0),
      m_materialEffects		(nullptr),
      m_materialEffectsOwner	(false),
      m_measurementBase		(measurementBase),
      m_minEnergyDeposit	(0.),
      m_minimizationDirection  	(nullptr),
      m_normal			(nullptr),
      m_numberDoF		(measurementBase->localCovariance().cols()),
      m_numericalDerivative	(false),
      m_outlier			(false),
      m_particleMassSquared	(0.),
      m_perigee			(nullptr),
      m_perigeeWeight		(nullptr),
      m_position		(measurementBase->associatedSurface().center()),
      m_qOverP			(0.),
      m_radiationThickness	(0.),
      m_residual		(0),
      m_scaleFactor		(0.),
      m_scatterPhi		(0.),
      m_scatterTheta		(0.),
      m_scatteringAngle		(0.),
      m_scatteringAngleOffSet	(0.),
      m_secondResidual		(0.),
      m_sensorDirection		(nullptr),
      m_sigma			(0.),
      m_sigmaMinus		(0.),
      m_sigmaPlus		(0.),
      m_signedDriftDistance	(0.),
      m_status			(0),
      m_surface			(&measurementBase->associatedSurface()),
      m_type			(stripCluster),
      m_weight			(1.),
      m_weight2			(1.)
{
    double sigma	= 0.;
    if (m_numberDoF > 0) sigma	= Amg::error(measurementBase->localCovariance(),locX);
    double sigma2	= 0.;
    if (m_numberDoF > 1) sigma2	= Amg::error(measurementBase->localCovariance(),locY);

    // remaining data according to surface (normal, sensor, minimization directions etc)
    if (dynamic_cast<const PlaneSurface*>(m_surface))
    {
	m_normal		= new Amg::Vector3D(m_surface->transform().rotation().col(2));
	const Amg::Vector3D* posptr = m_surface->localToGlobal(measurementBase->localParameters());
	m_position      = *posptr;
	delete posptr;	
	
	// special case to get sensor for endcap trapezoids (discs represented as planes)
	// thus sensor direction rotates according to localX - rather than being parallel
	// and there is only 1 'real' degree of freedom for covariance
	if (m_numberDoF == 2 && dynamic_cast<const TrapezoidBounds*>(&m_surface->bounds()))
	{
	    m_numberDoF		= 1;
	    m_type		= trapezoidCluster;
	    double aa		= measurementBase->localCovariance()(locX,locX);
	    double ab		= measurementBase->localCovariance()(locX,locY);
	    double bb		= measurementBase->localCovariance()(locY,locY);
	    double sum		= aa + bb;
	    double diff		= std::sqrt(sum*sum - 4.*(aa*bb-ab*ab));
	    // used by obsolete scaling
	    // double lengthSq	= 0.5*(sum + diff);
	    double widthSq	= 0.5*(sum - diff);
	    sigma		= std::sqrt(widthSq);
	    double term		= 0.5*(aa - bb)/diff;
	    double cosStereo	= std::sqrt(0.5 - term);
	    double sinStereo	= 0.0;
	    if (term > -0.5)
	    {
		sinStereo	= std::sqrt(0.5 + term);
		if (ab*m_normal->z() < 0.) sinStereo = -sinStereo;
	    }

	    const Amg::Vector3D& axis	= m_surface->transform().rotation().col(1);
	    m_sensorDirection		= new Amg::Vector3D(axis(0)*cosStereo + axis(1)*sinStereo,
							    axis(1)*cosStereo - axis(0)*sinStereo,
							    axis(2));
	}
	else
	{
	    m_sensorDirection		= new Amg::Vector3D(m_surface->transform().rotation().col(1));
	}
	
	m_minimizationDirection	= new Amg::Vector3D(m_sensorDirection->cross(*m_normal));
	if (m_numberDoF == 2) m_type	= pixelCluster;
    }
    else if (dynamic_cast<const StraightLineSurface*>(m_surface))
    {
	// StraightLines can be driftCircles or pseudoMeasurements along the wire
	if (! measurementBase->localParameters().contains(locY))
	{
	    // driftCircles will eventually have sagged surfaces.
	    // then may need something like:
	    //    get z-along wire from (globPos-center).dot(sensor)
	    //    then loc3D = (0,0,zalongwire)
	    //    position = surface->transform() * loc3D
	    // but probably just same as using centre with wire direction ...
	    m_sensorDirection		= new Amg::Vector3D(m_surface->transform().rotation().col(2));
	    m_signedDriftDistance	= measurementBase->localParameters()[driftRadius];
	    m_type			= driftCircle;
	}
	else
	{
	    // pseudomeasurement - minimize along wire direction
	    m_minimizationDirection	= new Amg::Vector3D(m_surface->transform().rotation().col(2));
            double mag = m_surface->center().mag();
	    m_normal		= mag > 1e-6 ? new Amg::Vector3D(m_surface->center()/mag) : new Amg::Vector3D(m_surface->normal());
	    m_position		= measurementBase->globalPosition();
	    m_sensorDirection	= new Amg::Vector3D(m_normal->cross(*m_minimizationDirection));
	    m_signedDriftDistance	= 0.;
	    m_type			= pseudoMeasurement;
	}
    }
    
    const PerigeeSurface* perigee	= dynamic_cast<const PerigeeSurface*>(m_surface);
    if (perigee)
    {
	m_position		= m_surface->center(); 
	m_sensorDirection	= new Amg::Vector3D(0.,0.,1.);
	m_type			= transverseVertex;
	if (m_numberDoF == 2)	m_type	= vertex;
    }

    // there are no measurements from Atlas detectors of cylinder or disc type
    //     const CylinderSurface* cylinder		= dynamic_cast<const CylinderSurface*>(m_surface);
    //     if (cylinder)
    //     {
    //     }
	
    //     const DiscSurface* disc			= dynamic_cast<const DiscSurface*>(m_surface);
    //     if (disc)
    //     {
    //     }	

    // add protection against junk input
    if (sigma > 0.)	m_weight	= 1./sigma;
    if (sigma2 > 0.)	m_weight2	= 1./sigma2;

}

// MaterialEffectsBase constructor
FitMeasurement::FitMeasurement (const MaterialEffectsBase*	materialEffects,
				double				particleMass,
				const Amg::Vector3D&		position,
				double				qOverP,
				bool				calo)
    : m_afterCalo		(false),
      m_alignmentEffects	(nullptr),
      m_alignmentParameter	(0),
      m_alignmentParameter2	(0),
      m_betaSquared		(1.),
      m_derivative		(nullptr),
      m_derivative2		(nullptr),
      m_derivativeRow		(-1),
      m_d0         		(0.),
      m_energyLoss		(0.),
      m_firstParameter		(0),
      m_flippedDriftDistance	(false),
      m_hitIndex		(0),
      m_hitOnTrack		(nullptr),
      m_lastParameter		(0),
      m_materialEffects		(materialEffects),
      m_materialEffectsOwner	(false),
      m_measurementBase		(nullptr),
      m_minEnergyDeposit	(0.),
      m_minimizationDirection	(nullptr),
      m_normal			(nullptr),
      m_numberDoF		(0),
      m_numericalDerivative	(false),
      m_outlier			(false),
      m_particleMassSquared	(particleMass*particleMass),
      m_perigee			(nullptr),
      m_perigeeWeight		(nullptr),
      m_position		(position),
      m_qOverP			(qOverP),
      m_radiationThickness	(materialEffects->thicknessInX0()), 
      m_residual		(0),
      m_scaleFactor		(0.),
      m_scatterPhi		(0.),
      m_scatterTheta		(0.),
      m_scatteringAngle		(0.),
      m_scatteringAngleOffSet	(0.),
      m_secondResidual		(0.),
      m_sensorDirection		(nullptr),
      m_sigma			(0.),
      m_sigmaMinus		(0.),
      m_sigmaPlus		(0.),
      m_signedDriftDistance	(0.),
      m_status			(0),
      m_surface			(&materialEffects->associatedSurface()),
      m_type			(endcapScatterer),
      m_weight			(0.),
      m_weight2			(0.)
{
    if (dynamic_cast<const CylinderSurface*>(m_surface)
	|| std::abs(m_surface->normal()(2)) < 0.5) m_type = barrelScatterer;

    if(calo) m_type = calorimeterScatterer;

    // set any energy loss
    const EnergyLoss* energyLoss	= nullptr;
    const ScatteringAngles* scattering	= nullptr;
    const MaterialEffectsOnTrack* meot	= dynamic_cast<const MaterialEffectsOnTrack*>(materialEffects);
    if (meot)
    {
	energyLoss	= meot->energyLoss();
	scattering	= meot->scatteringAngles();
    }
    
    if (energyLoss)
    {
	// note: EDM defines energy loss to be negative
	//       but here take opposite as this convention is not accepted by CaloEnergy clients
	//       (exception to take verbatim for CaloEnergy)
	//m_energyLoss	= -energyLoss->deltaE();
	m_energyLoss    = std::abs(energyLoss->deltaE());

	// calo energy deposit treated as a single pure energy loss measurement,
	// with fit taking error into account
	if (calo && ! scattering && energyLoss->sigmaDeltaE() > 0.)
	{
	    m_energyLoss	= energyLoss->deltaE();
	    m_numberDoF		= 1;
	    m_sigma		= energyLoss->sigmaDeltaE();
	    m_sigmaMinus	= energyLoss->sigmaMinusDeltaE();
	    m_sigmaPlus		= energyLoss->sigmaPlusDeltaE();
	    m_type		= energyDeposit;
	    m_weight		= 1./m_sigma;
	    m_minEnergyDeposit	= 0.5*m_energyLoss;
	    if (m_energyLoss > 0. && m_energyLoss > 5.0*m_sigmaMinus)
	    {
		m_minEnergyDeposit	= m_energyLoss - 2.5*m_sigmaMinus;
	    }
	    else if (m_energyLoss < 0. && m_energyLoss < -5.0*m_sigmaMinus)
	    {
		m_minEnergyDeposit	= m_energyLoss + 2.5*m_sigmaMinus;
	    }	
	}
    }
    
    // initialize scattering angles
    if (scattering)
    {
	m_scatterPhi	=  scattering->deltaPhi();
	m_scatterTheta	=  scattering->deltaTheta();
        if(calo) m_scatteringAngleOffSet = scattering->sigmaDeltaTheta();
//        if(calo) std::cout << " Calorimeter scaterrer with sigmaDeltaPhi " << scattering->sigmaDeltaPhi() << " sigmaDeltaTheta " << scattering->sigmaDeltaTheta() << std::endl; 
    }
}

// constructor creating MaterialEffects for merged scatterers
FitMeasurement::FitMeasurement (double				radiationThickness,
				double				deltaE,
				double				particleMass,
				const Amg::Vector3D&		position,
				const Amg::Vector3D&		direction,
				double				qOverP,
				const Surface*			surface)
    : m_afterCalo		(false),
      m_alignmentEffects	(nullptr),
      m_alignmentParameter	(0),
      m_alignmentParameter2	(0),
      m_betaSquared		(1.),
      m_derivative		(nullptr),
      m_derivative2		(nullptr),
      m_derivativeRow		(-1),
      m_d0         		(0.),
      m_energyLoss		(-deltaE),
      m_firstParameter		(0),
      m_flippedDriftDistance	(false),
      m_hitIndex		(0),
      m_hitOnTrack		(nullptr),
      m_lastParameter		(0),
      m_materialEffects		(nullptr),
      m_materialEffectsOwner	(true),
      m_measurementBase		(nullptr),
      m_minEnergyDeposit	(0.),
      m_minimizationDirection	(nullptr),
      m_normal			(nullptr),
      m_numberDoF		(0),
      m_numericalDerivative	(false),
      m_outlier			(false),
      m_particleMassSquared	(particleMass*particleMass),
      m_perigee			(nullptr),
      m_perigeeWeight		(nullptr),
      m_position		(position),
      m_qOverP			(qOverP),
      m_radiationThickness	(radiationThickness), 
      m_residual		(0),
      m_scaleFactor		(0.),
      m_scatterPhi		(0.),
      m_scatterTheta		(0.),
      m_scatteringAngle		(0.),
      m_scatteringAngleOffSet	(0.),
      m_secondResidual		(0.),
      m_sensorDirection		(nullptr),
      m_sigma			(0.),
      m_sigmaMinus		(0.),
      m_sigmaPlus		(0.),
      m_signedDriftDistance	(0.),
      m_status			(0),
      m_surface			(surface),
      m_type			(endcapScatterer),
      m_weight			(0.),
      m_weight2			(0.)
{
    // plane surface with normal along input direction
    if (m_surface
	&& (dynamic_cast<const CylinderSurface*>(m_surface)
	    || std::abs(m_surface->normal()(2)) < 0.5))	m_type = barrelScatterer;
    else if (std::abs(direction(2)) < 0.5)		m_type = barrelScatterer;
    if (! m_surface)
    {	
	CurvilinearUVT uvt(direction);
	m_surface = new PlaneSurface(position,uvt);
    }
    
    // create MaterialEffects
    std::bitset<MaterialEffectsBase::NumberOfMaterialEffectsTypes> typeMaterial;
    if (deltaE != 0.) typeMaterial.set(MaterialEffectsBase::EnergyLossEffects);
    const EnergyLoss* energyLoss	= new EnergyLoss(deltaE,0.,0.,0.);
    m_materialEffects			= new MaterialEffectsOnTrack(radiationThickness,
								     energyLoss,
								     *m_surface,
								     typeMaterial);
    if (! surface) delete m_surface;
    m_surface = &m_materialEffects->associatedSurface();

    m_intersection[FittedTrajectory] = std::make_unique<TrackSurfaceIntersection>(position,direction,0.);
}

// constructor for adding (mis-)alignment effects
FitMeasurement::FitMeasurement (const AlignmentEffectsOnTrack*	alignmentEffects,
				const Amg::Vector3D&		direction,
				const Amg::Vector3D&		position)
    : m_afterCalo		(false),
      m_alignmentEffects	(alignmentEffects),
      m_alignmentParameter	(0),
      m_alignmentParameter2	(0),
      m_betaSquared		(1.),
      m_derivative		(nullptr),
      m_derivative2		(nullptr),
      m_derivativeRow		(-1),
      m_d0         		(0.),
      m_energyLoss		(0.),
      m_firstParameter		(0),
      m_flippedDriftDistance	(false),
      m_hitIndex		(0),
      m_hitOnTrack		(nullptr),
      m_lastParameter		(0),
      m_materialEffects		(nullptr),
      m_materialEffectsOwner	(false),
      m_measurementBase		(nullptr),
      m_minEnergyDeposit	(0.),
      m_minimizationDirection	(nullptr),
      m_normal			(nullptr),
      m_numberDoF		(2),
      m_numericalDerivative	(false),
      m_outlier			(false),
      m_particleMassSquared	(0.),
      m_perigee			(nullptr),
      m_perigeeWeight		(nullptr),
      m_position		(position),
      m_qOverP			(0.),
      m_radiationThickness	(0.),
      m_residual		(0),
      m_scaleFactor		(0.),
      m_scatterPhi		(alignmentEffects->deltaAngle()),
      m_scatterTheta		(alignmentEffects->deltaTranslation()),
      m_scatteringAngle		(0.),
      m_scatteringAngleOffSet	(0.),
      m_secondResidual		(0.),
      m_sensorDirection		(nullptr),
      m_sigma			(0.),
      m_sigmaMinus		(alignmentEffects->sigmaDeltaAngle()),
      m_sigmaPlus		(alignmentEffects->sigmaDeltaTranslation()),
      m_signedDriftDistance	(0.),
      m_status			(0),
      m_surface			(&alignmentEffects->associatedSurface()),
      m_type			(alignment),
      m_weight			(1.),
      m_weight2			(1.)
{
    // set weights
    if (m_sigmaMinus) m_weight	= 1./m_sigmaMinus;
    if (m_sigmaPlus)  m_weight2	= 1./m_sigmaPlus;
				      
    m_intersection[FittedTrajectory] = std::make_unique<TrackSurfaceIntersection>(position,
										  direction,
										  0.);
}
    
// constructor creating placeholder Surface for delimiting material aggregation
FitMeasurement::FitMeasurement (const TrackSurfaceIntersection&	intersection,
				double				shift)
    : m_afterCalo		(false),
      m_alignmentEffects	(nullptr),
      m_alignmentParameter	(0),
      m_alignmentParameter2	(0),
      m_betaSquared		(1.),
      m_derivative		(nullptr),
      m_derivative2		(nullptr),
      m_derivativeRow		(-1),
      m_d0         		(0.),
      m_energyLoss		(0.),
      m_firstParameter		(0),
      m_flippedDriftDistance	(false),
      m_hitIndex		(0),
      m_hitOnTrack		(nullptr),
      m_lastParameter		(0),
      m_materialEffects		(nullptr),
      m_materialEffectsOwner	(false),
      m_measurementBase		(nullptr),
      m_minEnergyDeposit	(0.),
      m_minimizationDirection	(nullptr),
      m_normal			(nullptr),
      m_numberDoF		(0),
      m_numericalDerivative	(false),
      m_outlier			(false),
      m_particleMassSquared	(0.),
      m_perigee			(nullptr),
      m_perigeeWeight		(nullptr),
      m_position		(intersection.position()),
      m_qOverP			(0.),
      m_radiationThickness	(0.),
      m_residual		(0),
      m_scaleFactor		(0.),
      m_scatterPhi		(0.),
      m_scatterTheta		(0.),
      m_scatteringAngle		(0.),
      m_scatteringAngleOffSet	(0.),
      m_secondResidual		(0.),
      m_sensorDirection		(nullptr),
      m_sigma			(0.),
      m_sigmaMinus		(0.),
      m_sigmaPlus		(0.),
      m_signedDriftDistance	(0.),
      m_status			(0),
      m_surface			(nullptr),
      m_type			(materialDelimiter),
      m_weight			(0.),
      m_weight2			(0.)
{
    // plane surface with normal along input direction and shift wrt position
    Amg::Vector3D offset	=  intersection.direction()*shift;
    m_position			+= offset;
    CurvilinearUVT uvt(intersection.direction());
    m_surface = new PlaneSurface(m_position,uvt); 

    m_intersection[FittedTrajectory] = std::make_unique<TrackSurfaceIntersection>(m_position,
										  intersection.direction(),
										  0.);
}
    
// other TrackStateOnSurface types
FitMeasurement::FitMeasurement (const TrackStateOnSurface&	TSOS)
    : m_afterCalo		(false),
      m_alignmentEffects	(nullptr),
      m_alignmentParameter	(0),
      m_alignmentParameter2	(0),
      m_betaSquared		(1.),
      m_derivative		(nullptr),
      m_derivative2		(nullptr),
      m_derivativeRow		(-1),
      m_d0         		(0.),
      m_energyLoss		(0.),
      m_firstParameter		(0),
      m_flippedDriftDistance	(false),
      m_hitIndex		(0),
      m_hitOnTrack		(nullptr),
      m_lastParameter		(0),
      m_materialEffects		(nullptr),
      m_materialEffectsOwner	(false),
      m_measurementBase		(nullptr),
      m_minEnergyDeposit	(0.),
      m_minimizationDirection	(nullptr),
      m_normal			(nullptr),
      m_numberDoF		(0),
      m_numericalDerivative	(false),
      m_outlier			(false),
      m_particleMassSquared	(0.),
      m_perigee			(nullptr),
      m_perigeeWeight		(nullptr),
      m_position		(TSOS.trackParameters()->position()),
      m_qOverP			(0.),
      m_radiationThickness	(0.),
      m_residual		(0),
      m_scaleFactor		(0.),
      m_scatterPhi		(0.),
      m_scatterTheta		(0.),
      m_scatteringAngle		(0.),
      m_scatteringAngleOffSet	(0.),
      m_secondResidual		(0.),
      m_sensorDirection		(nullptr),
      m_sigma			(0.),
      m_sigmaMinus		(0.),
      m_sigmaPlus		(0.),
      m_signedDriftDistance	(0.),
      m_status			(0),
      m_surface			(&TSOS.trackParameters()->associatedSurface()),
      m_type			(hole),
      m_weight			(0.),
      m_weight2			(0.)
{
    // set type if necessary (hole = default)
    if (TSOS.type(TrackStateOnSurface::Outlier))
    {
	m_outlier	= true;
    }
//     else if (TSOS.type(TrackStateOnSurface::Perigee))
//     {
// 	m_type = MSperigee;
//     }
    
}
    
// SiliconTracker constructor (from iPatRec)
FitMeasurement::FitMeasurement (int			hitIndex,
				HitOnTrack*		hitOnTrack,
				const Amg::Vector3D&	position,
				double                 	sigma,
				double                 	sigma2,
				double                 	sinStereo,
				int	               	status,
				const Surface*		surface,
				MeasurementType		type)
    : m_afterCalo		(false),
      m_alignmentEffects	(nullptr),
      m_alignmentParameter	(0),
      m_alignmentParameter2	(0),
      m_betaSquared		(1.),
      m_derivative		(nullptr),
      m_derivative2		(nullptr),
      m_derivativeRow		(-1),
      m_d0         		(0.),
      m_energyLoss		(0.),
      m_firstParameter		(0),
      m_flippedDriftDistance	(false),
      m_hitIndex		(hitIndex),
      m_hitOnTrack		(hitOnTrack),
      m_lastParameter		(0),
      m_materialEffects		(nullptr),
      m_materialEffectsOwner	(false),
      m_measurementBase		(nullptr),
      m_minEnergyDeposit	(0.),
      m_numberDoF		(1),
      m_numericalDerivative	(false),
      m_outlier			(false),
      m_particleMassSquared	(0.),
      m_perigee			(nullptr),
      m_perigeeWeight		(nullptr),
      m_position		(position),
      m_qOverP			(0.),
      m_radiationThickness	(0.),
      m_residual		(0),
      m_scaleFactor		(0.),
      m_scatterPhi		(0.),
      m_scatterTheta		(0.),
      m_scatteringAngle		(0.),
      m_scatteringAngleOffSet	(0.),
      m_secondResidual		(0.),
      m_sigma			(0.),
      m_sigmaMinus		(0.),
      m_sigmaPlus		(0.),
      m_signedDriftDistance	(0.),
      m_status			(status),
      m_surface			(surface),
      m_type			(type),
      m_weight			(1.),
      m_weight2			(1.)
{

    // pixel has 2-D measurement
    if (type == pixelCluster)
    {
	m_numberDoF	= 2;
    }

    // special treatment for projective trapezoidal chambers in the endcap
    // take sensorDirection from stereo angle as I don't understand Surface axes in this case
    m_normal			= new Amg::Vector3D(m_surface->transform().rotation().col(2));
    if (m_numberDoF == 1
	&& std::abs(m_normal->z()) > 0.99
	&& std::abs(sinStereo) < 0.5)		// end-cap projective geometry
    {
	double cosStereo	=  std::sqrt(1. - sinStereo*sinStereo);
	m_sensorDirection	=  new Amg::Vector3D(position(0)*cosStereo + position(1)*sinStereo,
						     -position(0)*sinStereo + position(1)*cosStereo,
						     0.);
	(*m_sensorDirection)	/= m_sensorDirection->perp();

// 	Amg::Vector3D sensor = surface->transform().getRotation().colY();
// 	double temp = sensor(0)*cosStereo + sensor(1)*sinStereo;
// 	sensor.setY(-sensor(0)*sinStereo + sensor(1)*cosStereo);
// 	sensor.setX(temp);
// 	std::cout << " projective   x " << surface->transform().getRotation().co(0)
// 		  << "   y " << surface->transform().getRotation().colY()
// 		  << "   z " << surface->transform().getRotation().colZ()
// 		  << "   m_normal " << *m_normal
// 		  << "   m_sensor " << *m_sensorDirection
// 		  << "   projective " << sensor << std::endl;
    }
    else // otherwise chambers have parallel strips with sensor direction = appropriate module axis
    {
	m_sensorDirection		= new Amg::Vector3D(m_surface->transform().rotation().col(1));
    }
    m_minimizationDirection = new Amg::Vector3D(m_sensorDirection->cross(*m_normal));
    
    // add protection against junk input
    if (sigma > 0.)
    {
	m_weight	= 1./sigma;
    }
    else
    {
	m_numberDoF	= 0;
	m_outlier	= true;
    }
    if (m_numberDoF == 2)
    {
	if (sigma2 > 0.)
	{
	    m_weight2	= 1./sigma2;
	}
	else
	{
	    m_numberDoF	= 0;
	    m_outlier	= true;
	}
    }
}

// drift constructor (TRT from iPatRec)
FitMeasurement::FitMeasurement (int	       		hitIndex,
				HitOnTrack*		hitOnTrack,
				const Amg::Vector3D&	position,
				double			sigma,
				double			signedDriftDistance,
				double			,
				const Surface*		surface)
    : m_afterCalo		(false),
      m_alignmentEffects	(nullptr),
      m_alignmentParameter	(0),
      m_alignmentParameter2	(0),
      m_betaSquared		(1.),
      m_derivative		(nullptr),
      m_derivative2		(nullptr),
      m_derivativeRow		(-1),
      m_d0         		(0.),
      m_energyLoss		(0.),
      m_firstParameter		(0),
      m_flippedDriftDistance	(false),
      m_hitIndex		(hitIndex),
      m_hitOnTrack		(hitOnTrack),
      m_lastParameter		(0),
      m_materialEffects		(nullptr),
      m_materialEffectsOwner	(false),
      m_measurementBase		(nullptr),
      m_minEnergyDeposit	(0.),
      m_minimizationDirection  	(nullptr),
      m_normal			(nullptr),
      m_numberDoF		(1),
      m_numericalDerivative	(false),
      m_outlier			(false),
      m_particleMassSquared	(0.),
      m_perigee			(nullptr),
      m_perigeeWeight		(nullptr),
      m_position		(position),
      m_qOverP			(0.),
      m_radiationThickness	(0.),
      m_residual		(0),
      m_scaleFactor		(0.),
      m_scatterPhi		(0.),
      m_scatterTheta		(0.),
      m_scatteringAngle		(0.),
      m_scatteringAngleOffSet	(0.),
      m_secondResidual		(0.),
      m_sigma			(0.),
      m_sigmaMinus		(0.),
      m_sigmaPlus		(0.),
      m_signedDriftDistance	(signedDriftDistance),
      m_status			(0),
      m_surface			(surface),
      m_type			(driftCircle),
      m_weight			(1.),
      m_weight2			(1.)
{
    m_sensorDirection		= new Amg::Vector3D(m_surface->transform().rotation().col(2));

    // add protection against junk input
    if (sigma > 0.)
    {
	m_weight	= 1./sigma;
    }
    else
    {
	m_numberDoF	= 0;
	m_outlier	= true;
    }
}

// perigee (TrackParameters) constructor
FitMeasurement::FitMeasurement (const TrackParameters&	perigee)
    : m_afterCalo		(false),
      m_alignmentEffects	(nullptr),
      m_alignmentParameter	(0),
      m_alignmentParameter2	(0),
      m_betaSquared		(1.),
      m_derivative		(nullptr),
      m_derivative2		(nullptr),
      m_derivativeRow		(-1),
      m_d0         		(0.),
      m_energyLoss		(0.),
      m_firstParameter		(0),
      m_flippedDriftDistance	(false),
      m_hitIndex		(0),
      m_hitOnTrack		(nullptr),
      m_lastParameter		(0),
      m_materialEffects		(nullptr),
      m_materialEffectsOwner	(false),
      m_measurementBase		(nullptr),
      m_minEnergyDeposit	(0.),
      m_minimizationDirection	(nullptr),
      m_normal			(nullptr),
      m_numberDoF		(0),
      m_numericalDerivative	(false),
      m_outlier			(true),	// use base class for additional trackParameters at detector boundary
      m_particleMassSquared	(0.),
      m_perigee			(nullptr),
      m_perigeeWeight		(nullptr),
      m_position		(perigee.associatedSurface().center()),
      m_qOverP			(0.),
      m_radiationThickness	(0.),
      m_residual		(0),
      m_scaleFactor		(0.),
      m_scatterPhi		(0.),
      m_scatterTheta		(0.),
      m_scatteringAngle		(0.),
      m_scatteringAngleOffSet	(0.),
      m_secondResidual		(0.),
      m_sensorDirection		(nullptr),
      m_sigma			(0.),
      m_sigmaMinus		(0.),
      m_sigmaPlus		(0.),
      m_signedDriftDistance	(0.),
      m_status			(0),
      m_surface			(&perigee.associatedSurface()),
      m_type			(perigeeParameters),
      m_weight			(1.),
      m_weight2			(1.)
{
    // perigee axis needed for propagation of fitted parameters
    m_sensorDirection		= new Amg::Vector3D(m_surface->transform().rotation().col(2));

    // is this perigee to be used as a measurement?
    if (perigee.covariance() && ! m_outlier)
    {
	// use as measurement
	m_numberDoF		=  5;
	m_outlier		=  false;
	
	// perigeeParameters as HepVector
	Amg::Vector3D momentum	=  perigee.momentum();
	double ptInv0		=  1./momentum.perp();
	double cosPhi		=  ptInv0*momentum(0);
	double sinPhi		=  ptInv0*momentum(1);
	double cotTheta		=  ptInv0*momentum(2);
	ptInv0			*= perigee.charge();
    
	Amg::VectorX parameters(6);
	parameters(0)		= perigee.parameters()[Trk::d0];
	parameters(1)		= perigee.parameters()[Trk::z0];
	parameters(2)		= cosPhi;
	parameters(3)		= sinPhi;
	parameters(4)		= cotTheta;
	parameters(5)		= ptInv0;
	m_perigee		= new Amg::VectorX(parameters);

	// ///////
        // // weight = inverse covariance
        // JacobianCotThetaPtToThetaP jacob(cotTheta, ptInv0);
        // CLHEP::HepSymMatrix cov  = measuredPerigee->localErrorMatrix().covariance();

        // // convert to internal units (TeV) to avoid rounding
        // for (int row = 0; row < 5; ++row)
        // {
        //     cov[row][4]  *= Gaudi::Units::TeV;
        // }
        // cov[4][4]        *= Gaudi::Units::TeV;
        // int     fail;
        // cov.invert(fail);
        // CLHEP::HepSymMatrix wtCLHEP(cov.similarityT(jacob));
	// // convert to eigen
	// Amg::MatrixX* weight = new Amg::MatrixX(5,5);
	// std::cout << " weight from CLHEP :" << std::endl;
	// for (int i = 0; i != 5; ++i)
	// {
	//     for (int j = 0; j != 5; ++j)
	//     {
	// 	(*weight)(i,j) = wtCLHEP[j][i];
	//     }
	//     std::cout << "  " << (*weight)(i,0) << "  " << (*weight)(i,1) << "  " << (*weight)(i,2)
	// 	      << "  " << (*weight)(i,3) << "  " << (*weight)(i,4) << std::endl;
	    
	// }
	// std::cout << std::endl;
	
	// m_perigeeWeight	= weight;
	// //////
	
	// weight = inverse covariance
	AmgSymMatrix(5) covariance(*perigee.covariance());
	// for (int i = 0; i != 5; ++i)
	// {
	//     for (int j = 0; j != 5; ++j)
	// 	covariance(i,j) = measuredPerigee->localErrorMatrix().covariance()[j][i];
	// }

	// convert to internal units (TeV) to avoid rounding
	for (int row = 0; row < 5; ++row)
	{
	    covariance(4,row)	*= Gaudi::Units::TeV;
	    covariance(row,4)	=  covariance(4,row);
	}
	covariance(4,4)		*= Gaudi::Units::TeV;
	covariance.inverse();

	// TODO: fix in mig5
	JacobianCotThetaPtToThetaP jacobian(cotTheta, ptInv0);
	// Amg::MatrixX jac(5,5);
	// for (int i = 0; i != 5; ++i)
	// {
	//     for (int j = 0; j != 5; ++j)	jac(i,j) = jacobian[j][i];
	// }
	// TODO: check eigen transformation gives same as CLHEP code above (transpose first??)
	//
	m_perigeeWeight		=  new const Amg::MatrixX (jacobian * covariance * jacobian.transpose());
	// std::cout << " weight :" << std::endl;
    }

}

// transverseVertex constructor
FitMeasurement::FitMeasurement (double			d0,
				const Amg::Vector3D&	position,
				double			sigma)
    : m_afterCalo		(false),
      m_alignmentEffects	(nullptr),
      m_alignmentParameter	(0),
      m_alignmentParameter2	(0),
      m_betaSquared		(1.),
      m_derivative		(nullptr),
      m_derivative2		(nullptr),
      m_derivativeRow		(-1),
      m_d0         		(d0),	// FIXME:: kept for cache tag as d0 is never used anywhere
      m_energyLoss		(0.),
      m_firstParameter		(0),
      m_flippedDriftDistance	(false),
      m_hitIndex		(0),
      m_hitOnTrack		(nullptr),
      m_lastParameter		(0),
      m_materialEffects		(nullptr),
      m_materialEffectsOwner	(false),
      m_measurementBase		(nullptr),
      m_minEnergyDeposit	(0.),
      m_minimizationDirection	(nullptr),
      m_normal			(nullptr),
      m_numberDoF		(1),
      m_numericalDerivative	(false),
      m_outlier			(false),
      m_particleMassSquared	(0.),
      m_perigee			(nullptr),
      m_perigeeWeight		(nullptr),
      m_position		(position),
      m_qOverP			(0.),
      m_radiationThickness	(0.),
      m_residual		(0),
      m_scaleFactor		(0.),
      m_scatterPhi		(0.),
      m_scatterTheta		(0.),
      m_scatteringAngle		(0.),
      m_scatteringAngleOffSet	(0.),
      m_secondResidual		(0.),
      m_sensorDirection		(new Amg::Vector3D(0.,0.,1.)),
      m_sigma			(0.),
      m_sigmaMinus		(0.),
      m_sigmaPlus		(0.),
      m_signedDriftDistance	(0.),
      m_status			(0),
      m_surface			(new const Trk::PerigeeSurface(position)),
      m_type			(transverseVertex),
      m_weight			(1./sigma),
      m_weight2			(1.)
{
}

// destructor
FitMeasurement::~FitMeasurement (void)
{
    delete m_minimizationDirection;
    delete m_normal;
    delete m_perigee;
    delete m_perigeeWeight;
    delete m_sensorDirection;
    if ((m_type == materialDelimiter || m_type == transverseVertex)
	&& ! m_measurementBase)	delete m_surface;
    if (m_materialEffects && m_materialEffectsOwner) delete m_materialEffects;
}

void
FitMeasurement::intersection (ExtrapolationType type,
			      const TrackSurfaceIntersection* value)
{
  m_intersection[type].reset(value);
}


void
FitMeasurement::printHeading (MsgStream& log) const
{
    log << "                 residual 1........2         r      phi         z"
	<< "      sigma 1.......2      energy  energyLoss scatteringAngle  integral X0" << std::endl;
}

void
FitMeasurement::print (MsgStream& log) const
{
    Amg::Vector3D position = m_position;
    log << m_type
	<< std::setiosflags(std::ios::fixed);
    if (numberDoF())
    {
	log << std::setw(9) << std::setprecision(3) << *m_residual;
	if (m_numberDoF > 1)
	{
	    log << std::setw(9) << std::setprecision(3) << *(m_residual+1);
	}
	else if (m_alignmentParameter2)
	{
	    log << " A" << std::setw(1)	<< m_alignmentParameter
		<< " A" << std::setw(1) << m_alignmentParameter2 << "   ";
	}
	else if (m_alignmentParameter)
	{
	    log << " A" << std::setw(1) << m_alignmentParameter << "      ";
	}
	else
	{
	    log << "         ";
	}
    }
    else
    {
	if (isPositionMeasurement())
	{
	    log << std::setw(9) << std::setprecision(3) << *m_residual << " outlier ";
	}
	else
	{
	    log << "                  ";
	}
	if (m_intersection[FittedTrajectory]) position = m_intersection[FittedTrajectory]->position();
    }
    log << std::setw(10) << std::setprecision(1) << position.perp()
	<< std::setw(9)  << std::setprecision(4) << position.phi()
	<< std::setw(10) << std::setprecision(1) << position(2);
    
    if (isPositionMeasurement())
    {
	log << std::setw(13) << std::setprecision(3) << 1./m_weight;
	if (m_numberDoF == 2)
	{
	    log << std::setw(8) << std::setprecision(3) << 1./m_weight2;
	}
	else if (isDrift())
	{
	    log << "(" << std::setw(7) << std::setprecision(3) << m_signedDriftDistance << ")";
	}    
	else
	{
	    log << "        ";
	}
    }
    else if (isScatterer())
    {
	log << std::setw(33) << std::setprecision(3) << 1./std::abs(m_qOverP*Gaudi::Units::GeV)
	    << std::setw(12) << std::setprecision(4) << m_energyLoss/Gaudi::Units::GeV;
	if (m_type < barrelInert || m_scatteringAngle > 0.) {
            double totScat = sqrt(m_scatteringAngle*std::abs(m_qOverP)*m_scatteringAngle*std::abs(m_qOverP)+m_scatteringAngleOffSet*m_scatteringAngleOffSet);
	    log << std::setw(16) << std::setprecision(6) << totScat
		<< std::setw(13) << std::setprecision(3) << m_radiationThickness;
        }
    }
    else if (isAlignment())
    {
	log << std::setw(13) << std::setprecision(3) << 1./m_weight;
	if (m_numberDoF == 2)
	{
	    log << std::setw(8) << std::setprecision(3) << 1./m_weight2;
	}
    }
    else if (isEnergyDeposit())
    {
	if (m_numberDoF)
	{
	    log << std::setw(13) << std::setprecision(3) << 1./(m_weight*Gaudi::Units::GeV)
		<< std::setw(32) << std::setprecision(4) << m_energyLoss/Gaudi::Units::GeV;
	}
	else
	{
	    log << std::setw(33) << std::setprecision(3) << 1./std::abs(m_qOverP*Gaudi::Units::GeV);
	    log << std::setw(12) << std::setprecision(4) << m_energyLoss/Gaudi::Units::GeV;
	}
    }
    log << std::endl;
}

void
FitMeasurement::qOverP (double value)
{
    m_qOverP		= value;
    
    // for scatterer measurements: correct the weight for the local momentum value
    if ((m_type == barrelScatterer || m_type == endcapScatterer || m_type == calorimeterScatterer)
	&& m_scatteringAngle > 0.)
    {
	double pSquare	= 1./(value*value);
	m_betaSquared	= pSquare/(pSquare + m_particleMassSquared);
	m_weight       	= std::sqrt(m_betaSquared*pSquare) / m_scatteringAngle;
    }

    if( m_type == calorimeterScatterer && m_scatteringAngleOffSet > 0 ) {
        if( m_weight > 0) {
          m_weight = sqrt(1./ (1./m_weight/m_weight + m_scatteringAngleOffSet*m_scatteringAngleOffSet));
        } else {
          m_weight = 1./m_scatteringAngleOffSet;
        }
    }
}

void
FitMeasurement::scatteringAngle (double angle, double totalRadiationThickness)
{

// update the m_scatteringAngleOffSet at initialisation 

    if(m_scatteringAngle==0.&& m_scatteringAngleOffSet>0 && m_qOverP != 0.) {
// 
      double angle_iPat = angle*std::abs(m_qOverP);

//      std::cout << " scatteringAngle type " <<  m_type << " angle_iPat " << angle_iPat << " m_scatteringAngleOffSet " << m_scatteringAngleOffSet;

      if(angle_iPat<m_scatteringAngleOffSet) {
        m_scatteringAngleOffSet = sqrt(m_scatteringAngleOffSet*m_scatteringAngleOffSet-angle_iPat*angle_iPat);
      } else {
        m_scatteringAngleOffSet = 0.;
      } 
//      std::cout << " corrected m_scatteringAngleOffSet " << m_scatteringAngleOffSet << std::endl;
    }

    m_scatteringAngle		=  angle;
    m_radiationThickness	=  totalRadiationThickness;
    if (m_type == barrelInert)
    {
	m_type = barrelScatterer;
    }
    else if (m_type == endcapInert)
    {	
	m_type = endcapScatterer;
    }
    
    if (m_qOverP != 0.)
    {
	double pSquare	= 1./(m_qOverP*m_qOverP);
	m_betaSquared	= pSquare/(pSquare + m_particleMassSquared);
	m_weight       	= std::sqrt(m_betaSquared*pSquare) / m_scatteringAngle;
        if( m_type == calorimeterScatterer && m_scatteringAngleOffSet > 0) {
          if( m_weight > 0) {
            m_weight = sqrt(1./ (1./m_weight/m_weight + m_scatteringAngleOffSet*m_scatteringAngleOffSet));
          } else {
            m_weight = 1./m_scatteringAngleOffSet;
          }
        }

    }
}

void
FitMeasurement::setSigmaSymmetric (void)
{
    double sigma	= std::sqrt(0.5*(m_sigmaPlus*m_sigmaPlus + m_sigmaMinus*m_sigmaMinus));
    m_weight		= 1./sigma;
}

}	// end of namespace
