/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/***************************************************************************
 process measurements i.e. extrapolate to surface,
                           compute derivatives and residuals
 ***************************************************************************/

#include <cmath>
#include <iomanip>
#include <iostream>
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/SystemOfUnits.h"
#include "TrkExInterfaces/IIntersector.h"
#include "TrkExInterfaces/IPropagator.h"
#include "TrkExUtils/TrackSurfaceIntersection.h"
#include "TrkSurfaces/Surface.h"
#include "TrkiPatFitterUtils/FitMeasurement.h"
#include "TrkiPatFitterUtils/FitParameters.h"
#include "TrkiPatFitterUtils/MeasurementProcessor.h"
#include "TrkiPatFitterUtils/ParameterType.h" 

namespace Trk{
  
//<<<<<< CLASS STRUCTURE INITIALIZATION                                 >>>>>>


// constructor
MeasurementProcessor::MeasurementProcessor (bool				asymmetricCaloEnergy,
					    ToolHandle<IIntersector>&		intersector,
					    std::list<FitMeasurement*>&		measurements,
					    FitParameters*			parameters,
					    ToolHandle<IIntersector>&		rungeKuttaIntersector,
					    const ToolHandle<IPropagator>&	stepPropagator,
                                            int useStepPropagator)
    :	m_asymmetricCaloEnergy		(asymmetricCaloEnergy),
	m_caloEnergyMeasurement		(0),
	m_cosPhi0			(parameters->cosPhi()),
	m_cosTheta0			(parameters->cosTheta()),
	m_derivQOverP0			(0.),
	m_derivQOverP1			(0.),
	m_energyResidual		(0.),
	m_firstScatteringParameter      (parameters->firstScatteringParameter()),
        //m_havePhiPseudo			(false),
	m_intersector			(intersector),
	m_largeDeltaD0			(50.*Gaudi::Units::mm),
	m_largeDeltaPhi0		(0.05),	
	m_measurements			(measurements),
        //m_minDistanceForAngle		(2.*Gaudi::Units::mm),
	m_numericDerivatives		(false),
	m_parameters			(parameters),
	m_phiInstability		(false),
	m_qOverPbeforeCalo		(0.),
	m_qOverPafterCalo		(0.),
	m_rungeKuttaIntersector		(rungeKuttaIntersector),
	m_stepPropagator		(stepPropagator),
        m_useStepPropagator             (useStepPropagator),
	m_sinPhi0			(parameters->sinPhi()),
	m_sinTheta0			(parameters->sinTheta()),
        //m_toroidTurn			(0.1),
	m_vertexIntersect		(0),
	m_x0				(parameters->position().x()),
	m_y0				(parameters->position().y()),
	m_z0				(parameters->position().z())
        //m_zInstability			(false)
{
    m_alignments.reserve(10);
    m_intersectStartingValue	= m_parameters->intersection();
    // bool haveDrift		= false;
    int numberPositionMeas	= 0;
    // int numberPseudo		= 0;
    m_scatterers.reserve(100);

// Field for StepPropagator     
    m_stepField = Trk::MagneticFieldProperties(Trk::FullField);
    if(m_useStepPropagator==2)  m_stepField = Trk::MagneticFieldProperties(Trk::FastField);

    
    for (std::list<FitMeasurement*>::iterator m = m_measurements.begin();
	 m != m_measurements.end();
	 ++m)
    {
	if (! (**m).numberDoF())		continue;
	if ((**m).isAlignment()) m_alignments.push_back(*m);
	// if ((**m).isDrift())	haveDrift	= true;
	if ((**m).isPositionMeasurement())	++numberPositionMeas;
	// if ((**m).isPseudo())			++numberPseudo;
	if ((**m).isScatterer()) m_scatterers.push_back(*m);

	// cache some variables when we fit calo energy deposit
	if (! (**m).isEnergyDeposit() || ! m_parameters->fitMomentum())	continue;
	m_caloEnergyMeasurement	= *m;
	if (numberPositionMeas < 5)
	{
	    m_asymmetricCaloEnergy = false;
	    m_caloEnergyMeasurement->setSigmaSymmetric();
	}
	m_qOverPbeforeCalo      = m_parameters->qOverP();
	m_qOverPafterCalo	= (**m).qOverP();
	if (m_qOverPbeforeCalo*m_qOverPafterCalo < 0.) m_qOverPafterCalo = m_qOverPbeforeCalo;
	m_parameters->qOverP1(m_qOverPafterCalo);
	// std::cout << numberPositionMeas << " calo energy deposit "
	// 	  << m_caloEnergyMeasurement->energyLoss()/Gaudi::Units::GeV
	// 	  << "  sigma " << 1./(m_caloEnergyMeasurement->weight()*Gaudi::Units::GeV)
	// 	  << std::endl;
    }

    // // pseudoMeasurement projectivity constraint needed for some MS tracks
    // if (numberPseudo && (numberPseudo > 1 || m_measurements.front()->isVertex()))
    // 	m_phiInstability	= true;

    // for numerical derivs
    for (int typ = 0; typ != ExtrapolationTypes; ++typ)
    {
	m_delta[typ]	= 0.0001;
	m_qOverP[typ]	= m_parameters->qOverP();
    }
}

// destructor
MeasurementProcessor::~MeasurementProcessor (void)
{
    delete m_intersectStartingValue;
    delete m_vertexIntersect;
}

//<<<<<< MEMBER PUBLIC FUNCTION DEFINITIONS                             >>>>>>

bool
MeasurementProcessor::calculateDerivatives(void)
{
    // extrapolate to all measurement surfaces to compute intersects for momentum derivatives
    if (m_parameters->fitMomentum()	 && ! extrapolateToMeasurements(DeltaQOverP0)) return false;
    if (m_parameters->fitEnergyDeposit() && ! extrapolateToMeasurements(DeltaQOverP1)) return false;
    
    // extrapolate for any other numeric derivatives
    if (m_numericDerivatives)
    {
	double ptInv		= std::abs(m_parameters->ptInv0());
	m_delta[DeltaD0]       	= 0.010 + 10.0*ptInv;
	m_delta[DeltaZ0]       	= 0.010 + 10.0*ptInv;
	m_delta[DeltaPhi0]     	= 0.0001 + 2.0*ptInv;
	m_delta[DeltaTheta0]	= 0.0005 + 2.0*ptInv;
	
	const TrackSurfaceIntersection* intersection = m_vertexIntersect;
	m_vertexIntersect = new TrackSurfaceIntersection(
	    Amg::Vector3D(m_x0 - m_delta[DeltaD0]*m_sinPhi0,
			  m_y0 + m_delta[DeltaD0]*m_cosPhi0,
			  m_z0),
	    intersection->direction(),
	    0.);
	if (! extrapolateToMeasurements(DeltaD0)) return false;
	delete m_vertexIntersect;
       	m_vertexIntersect = new TrackSurfaceIntersection(
	    Amg::Vector3D(m_x0,
			  m_y0,
			  m_z0 + m_delta[DeltaZ0]),
	    intersection->direction(),
	    0.);
	if (! extrapolateToMeasurements(DeltaZ0)) return false;
	double sinTheta	= intersection->direction().perp();
	double delCF   	= 1. - (0.5*m_delta[DeltaPhi0]*m_delta[DeltaPhi0]);
	Amg::Vector3D direction(sinTheta*(m_cosPhi0*delCF - m_sinPhi0*m_delta[DeltaPhi0]),
				sinTheta*(m_sinPhi0*delCF + m_cosPhi0*m_delta[DeltaPhi0]),
				intersection->direction().z());
	delete m_vertexIntersect;
       	m_vertexIntersect = new TrackSurfaceIntersection(intersection->position(),
							 direction,
							 0.);
	if (! extrapolateToMeasurements(DeltaPhi0)) return false;
	double cotTheta =  m_vertexIntersect->direction().z()/sinTheta + m_delta[DeltaTheta0];
	sinTheta = 1./std::sqrt(1. + cotTheta*cotTheta);
	Amg::Vector3D directionTheta(sinTheta*m_cosPhi0,
				     sinTheta*m_sinPhi0,
				     sinTheta*cotTheta);
	delete m_vertexIntersect;
       	m_vertexIntersect = new TrackSurfaceIntersection(intersection->position(),
							 directionTheta,
							 0.);
	if (! extrapolateToMeasurements(DeltaTheta0)) return false;
	delete m_vertexIntersect;
	m_vertexIntersect = intersection;
    }

    // loop over measurements to compute derivatives:
    for (std::list<FitMeasurement*>::iterator m = m_measurements.begin();
	 m != m_measurements.end();
	 ++m)
    {
	// strip detector types
	if ((**m).isCluster())
	{
	    clusterDerivatives((**m).numberDoF(), **m);
	}
	else if ((**m).isDrift() || (**m).isPerigee() || (**m).isPseudo()) // else drift circles
	{
            const TrackSurfaceIntersection& intersection = (**m).intersection(FittedTrajectory);
	    Amg::Vector3D* driftDirection =
		new Amg::Vector3D((**m).sensorDirection().cross(intersection.direction()));
	    *driftDirection	= driftDirection->unit();
	    (**m).minimizationDirection(driftDirection);
	    driftDerivatives((**m).numberDoF(), **m);
	}
	else if ((**m).isVertex())
	{
	    const TrackSurfaceIntersection& intersection = (**m).intersection(FittedTrajectory);
	    Amg::Vector3D* minimizationDirection =
		new Amg::Vector3D((**m).sensorDirection().cross(intersection.direction()));
	    *minimizationDirection	= minimizationDirection->unit();
	    (**m).minimizationDirection(minimizationDirection);
	    (**m).derivative(D0,(**m).weight()*(m_cosPhi0*(**m).minimizationDirection().y() -
						m_sinPhi0*(**m).minimizationDirection().x()));
	    if ((**m).is2Dimensional())
		(**m).derivative2(Z0,(**m).weight2()*(**m).sensorDirection().z());
	}
	else if (! (**m).numberDoF())
	{
	    continue;
	}
	else if ((**m).isScatterer())
	{
	    unsigned param	= (**m).lastParameter() - 2;
	    (**m).derivative(param,(**m).weight() *
			     (**m).intersection(FittedTrajectory).direction().perp());
	    (**m).derivative2(++param,(**m).weight());
	}
	else if ((**m).isAlignment())
	{
	    // std::cout << " set alignment derivatives: "
	    // 	      << "   alignmentParameter " << (**m).alignmentParameter()
	    // 	      << "   firstParameter " << (**m).firstParameter()
	    // 	      << "   lastParameter " << (**m).lastParameter()
	    // 	      << std::endl;
	    unsigned param	= (**m).firstParameter();
	    (**m).derivative(param,(**m).weight());
	    (**m).derivative2(++param,(**m).weight2());
	}
	else if ((**m).isEnergyDeposit())
	{
	    double E0           = 1./std::abs(m_qOverPbeforeCalo);
            double E1           = 1./std::abs(m_qOverPafterCalo);
	    double weight	= (**m).weight();
	    (**m).derivative(QOverP0, weight*E0/(m_qOverPbeforeCalo*Gaudi::Units::TeV));
	    (**m).derivative(QOverP1,-weight*E1/(m_qOverPafterCalo*Gaudi::Units::TeV));
	}
	else
	{
	    // report any mysteries ... 
	    continue;
	}
    }

    // flip step sign of numerical derivatives for next iteration
    if (m_numericDerivatives)
    {
	for (int typ = 1; typ != ExtrapolationTypes; ++typ) m_delta[typ] = -m_delta[typ];
    }
    
    return true;
}

bool
MeasurementProcessor::calculateFittedTrajectory(int /*iteration*/)
{
    // cache frequently accessed parameters
    m_cosPhi0	= m_parameters->cosPhi();
    m_cosTheta0	= m_parameters->cosTheta();
    m_sinPhi0	= m_parameters->sinPhi();
    m_sinTheta0	= m_parameters->sinTheta();
    m_x0	= m_parameters->position().x();
    m_y0	= m_parameters->position().y();
    m_z0	= m_parameters->position().z();

    // start with intersection placed at perigee
    delete m_vertexIntersect;
    m_vertexIntersect	=  m_parameters->intersection();

    // increments for momentum derivatives (Mev^-1 : X-over at 30GeV)
    double floor	= 0.000000030;
    double fraction	= 0.0001;
    if (m_parameters->qOverP() < 0.)
    {
	m_delta[DeltaQOverP0]	=  floor - fraction*m_parameters->qOverP();
	m_delta[DeltaQOverP1]	=  m_delta[DeltaQOverP0];
    }
    else
    {
	m_delta[DeltaQOverP0]	= -floor - fraction*m_parameters->qOverP();
	m_delta[DeltaQOverP1]	=  m_delta[DeltaQOverP0];
    }
    if (m_parameters->fitEnergyDeposit())
	m_delta[DeltaQOverP1]	*= m_qOverPafterCalo/m_qOverPbeforeCalo;

    // TeV momentum scale to avoid magnitude mismatch (aka rounding)
    m_derivQOverP0		= 1./(m_delta[DeltaQOverP0]*Gaudi::Units::TeV);
    m_derivQOverP1		= 1./(m_delta[DeltaQOverP1]*Gaudi::Units::TeV);
    
    // and set qOverP
    for (int typ = 0; typ != ExtrapolationTypes; ++typ)
    {
	m_qOverP[typ] = m_parameters->qOverP();
    }
    m_qOverP[DeltaQOverP0]	+= m_delta[DeltaQOverP0];

    // use asymmetric error in case of significant energy deposit residual
    if (m_parameters->fitEnergyDeposit() && m_asymmetricCaloEnergy)
    {
	if (m_energyResidual*m_caloEnergyMeasurement->residual() < 0.)
	{
// 	    std::cout << " average energy deposit "
// 		      << m_energyResidual
// 		      << "  " << m_caloEnergyMeasurement->residual()
// 		      << "  sigma " << 1./(m_caloEnergyMeasurement->weight()*Gaudi::Units::GeV)
// 		      << std::endl;
	    m_caloEnergyMeasurement->setSigma();
	    m_energyResidual = m_caloEnergyMeasurement->residual();
	}    
	else if (m_energyResidual < 1. && m_caloEnergyMeasurement->residual() > 1.)
	{
// 	    std::cout << " significant energy loss "
// 		      << m_energyResidual
// 		      << "  " << m_caloEnergyMeasurement->residual()
// 		      << "  sigma " << 1./(m_caloEnergyMeasurement->weight()*Gaudi::Units::GeV)
// 		      << std::endl;
	    m_caloEnergyMeasurement->setSigmaPlus();
	    m_energyResidual = m_caloEnergyMeasurement->residual();
	}
	else if (m_energyResidual > -1. && m_caloEnergyMeasurement->residual() < -1.)
	{
// 	    std::cout << " significant energy gain "
// 		      << m_energyResidual
// 		      << "  " << m_caloEnergyMeasurement->residual()
// 		      << "  sigma " << 1./(m_caloEnergyMeasurement->weight()*Gaudi::Units::GeV)
// 		      << std::endl;
	    m_caloEnergyMeasurement->setSigmaMinus();
	    m_energyResidual = m_caloEnergyMeasurement->residual();
	}
    }

    // extrapolate to all measurement surfaces and compute intersects
    if (extrapolateToMeasurements(FittedTrajectory)) return true;

    return false;
}

void
MeasurementProcessor::calculateResiduals(void)
{
    int nAlign		= 0;
    int nScat		= 0;
    for (std::list<FitMeasurement*>::iterator m = m_measurements.begin();
	 m != m_measurements.end();
	 ++m)
    {
	if (! (**m).numberDoF())	continue;

	// special measurements to constrain additional parameters
	if (! (**m).isPositionMeasurement())
	{
	    if ((**m).isScatterer())	// scattering centres
	    {
		double phiResidual	= -(**m).weight()*m_parameters->scattererPhi(nScat) *
					  (**m).intersection(FittedTrajectory).direction().perp();
		(**m).residual(phiResidual);
		double thetaResidual	= -(**m).weight()*m_parameters->scattererTheta(nScat);
		(**m).residual2(thetaResidual);
		++nScat;
	    }
	    else if ((**m).isAlignment())	// alignment uncertainties
	    {
		(**m).residual(-(**m).weight()   * (m_parameters->alignmentAngle(nAlign) -
						  m_parameters->alignmentAngleConstraint(nAlign)));
		(**m).residual2(-(**m).weight2() * (m_parameters->alignmentOffset(nAlign) -
						    m_parameters->alignmentOffsetConstraint(nAlign)));
		++nAlign;
	    }
	    else if ((**m).isEnergyDeposit())
	    {
		// Add the energy loss as a measurement
		double E0           = 1./std::abs(m_qOverPbeforeCalo);
		double E1           = 1./std::abs(m_qOverPafterCalo);
		double residual	= (**m).weight()*(E0 - E1 - (**m).energyLoss());
		(**m).residual(residual);
            
		// 	    std::cout << std::setiosflags(std::ios::fixed)
		// 		      << " calo residual " << std::setw(8) << std::setprecision(2)
		// 		      << (**m).residual()
		// 		      << " (in GeV):" << std::setw(8) << std::setprecision(3)
		// 		      << (E0 - E1 - (**m).energyLoss())/Gaudi::Units::GeV
		// 		      << std::endl;
		// 	    const TrackSurfaceIntersection& intersection = (**m).intersection(FittedTrajectory);
		// 	    double fittedLoss = (std::abs(E0/Gaudi::Units::GeV) - std::abs(E1/Gaudi::Units::GeV)) * intersection.direction().perp();
		// 	    std::cout << std::setiosflags(std::ios::fixed)
		// 		      << "  pT"                     
		// 		      << std::setw(9) << std::setprecision(3) << 1./(m_parameters->ptInv0()*Gaudi::Units::GeV)
		// 		      << " E " << std::setw(9) << std::setprecision(3) << E0/Gaudi::Units::GeV
		// 		      << std::setw(9) << std::setprecision(3) << E1/Gaudi::Units::GeV
		// 		      << "  calorimeter at r,z "
		// 		      << std::setw(7) << std::setprecision(1) << intersection.position().perp()
		// 		      << std::setw(8) << std::setprecision(1) << intersection.position().z()
		// 		      << "     pT loss: measured "
		// 		      << std::setw(9) << std::setprecision(3)
		// 		      << (**m).energyLoss()*intersection.direction().perp()/Gaudi::Units::GeV
		// 		      << "  fitted "                     
		// 		      << std::setw(9) << std::setprecision(3) << fittedLoss
		// 		      << std::endl;
	    }
	    continue;
	}

	// position measurements
	const TrackSurfaceIntersection& intersection	= (**m).intersection(FittedTrajectory);
	const Amg::Vector3D& minimizationDirection	= (**m).minimizationDirection();
	Amg::Vector3D offset 				= (**m).position() - intersection.position();
	if ((**m).isCluster())		// strip detector types
	{
	    double residual	= (**m).weight() * minimizationDirection.dot(offset);
	    if ((**m).alignmentParameter())
	    {
		// propagate to residual using derivatives
		unsigned param  =  (**m).alignmentParameter() - 1;
		unsigned deriv 	=  m_parameters->numberParameters() - 2*m_parameters->numberAlignments() + 2*param;
		residual	-= m_parameters->alignmentAngle(param)  * (**m).derivative(deriv) +
				   m_parameters->alignmentOffset(param) * (**m).derivative(deriv+1);
		
		// FitMeasurement* fm	= m_alignments[(**m).alignmentParameter()-1];
		// std::cout << " cluster resid:  measurement.alignmentParameter " << (**m).alignmentParameter()
		// 	  << "   at z " << fm->surface()->center().z()
		// 	  << "   param " << param
		// 	  << "   minimizationDirection.z() " << minimizationDirection.z()
		// 	  << "   angle "
		// 	  << m_parameters->alignmentAngle(param)
		// 	  << " angle resid "
		// 	  << m_parameters->alignmentAngle(param)*(**m).derivative(deriv) 
		// 	  << "   offset "
		// 	  << m_parameters->alignmentOffset(param)
		// 	  << " offset resid"
		// 	  << m_parameters->alignmentOffset(param)*(**m).derivative(deriv+1)
		// 	  << "   residual "	<< residual
		// 	  << std::endl;

		if ((**m).alignmentParameter2())
		{
		    // propagate to residual using derivatives
		    param	=  (**m).alignmentParameter2() - 1;
		    deriv	=  m_parameters->numberParameters() - 2*m_parameters->numberAlignments() + 2*param;
		    residual	-= m_parameters->alignmentAngle(param)  * (**m).derivative(deriv) +
				   m_parameters->alignmentOffset(param) * (**m).derivative(deriv+1);
		}
	    }
	    (**m).residual(residual);
	    if (! (**m).is2Dimensional()) continue;
	    double residual2	=  (**m).weight2()*(**m).sensorDirection().dot(offset);
	    (**m).residual2(residual2);
	}
	else if ((**m).isDrift() || (**m).isPerigee())	// else drift circles (perigee is similar)
	{
	    double residual 	= (**m).weight() *
				  (minimizationDirection.dot(offset) + (**m).signedDriftDistance());
	    if ((**m).alignmentParameter())
	    {
		// propagate to residual using derivatives
		unsigned param  =  (**m).alignmentParameter() - 1;
		int deriv 	=  m_parameters->numberParameters() - 2*m_parameters->numberAlignments() + 2*param;
		residual	-= m_parameters->alignmentAngle(param)  * (**m).derivative(deriv) +
				   m_parameters->alignmentOffset(param) * (**m).derivative(deriv+1);
		if (deriv != m_parameters->firstAlignmentParameter())
		    // std::cout << " firstAlignmentParameter " << m_parameters->firstAlignmentParameter() + 2*param
		    // 	      << " deriv " << deriv << std::endl;
		
		// FitMeasurement* fm	= m_alignments[param];
		// std::cout << " drift resid:  measurement.alignmentParameter " << (**m).alignmentParameter()
		// 	  << "   at z " << fm->surface()->center().z()
		// 	  << "   param " << param
		// 	  << "   driftDirection.z() " << minimizationDirection.z()
		// 	  << "   angle "
		// 	  << m_parameters->alignmentAngle(param)
		// 	  << " angle resid "
		// 	  << m_parameters->alignmentAngle(param)*(**m).derivative(deriv) 
		// 	  << "   offset "
		// 	  << m_parameters->alignmentOffset(param)
		// 	  << " offset resid"
		// 	  << m_parameters->alignmentOffset(param)*(**m).derivative(deriv+1)
		// 	  << "   residual "	<< residual
		// 	  << std::endl;

		if ((**m).alignmentParameter2())
		{
		    // propagate to residual using derivatives
		    param	=  (**m).alignmentParameter2() - 1;
		    deriv	=  m_parameters->numberParameters() - 2*m_parameters->numberAlignments() + 2*param;
		    residual	-= m_parameters->alignmentAngle(param)  * (**m).derivative(deriv) +
				   m_parameters->alignmentOffset(param) * (**m).derivative(deriv+1);
		}
	    }
	    (**m).residual(residual);
	    // std::cout << " residual " << residual*(**m).sigma()
	    // 	      << "  drift distance " << (**m).signedDriftDistance() << std::endl;
	}
	else if ((**m).isPseudo())	// else pseudo measurement
	{
	    double residual	= (**m).weight() * minimizationDirection.dot(offset);
	    (**m).residual(residual);
	    if (! (**m).is2Dimensional()) continue;
	    double residual2	= (**m).weight2() * (**m).sensorDirection().dot(offset);
	    (**m).residual2(residual2);
	}
	else if ((**m).isVertex())
	{
	    double residual	=  (**m).weight() * (minimizationDirection.x()*offset.x() +
						     minimizationDirection.y()*offset.y()) /
				   minimizationDirection.perp() ;
	    (**m).residual(residual);
	    if (! (**m).is2Dimensional()) continue;
	    double residual2= (**m).weight2() * (**m).sensorDirection().dot(offset);
	    (**m).residual2(residual2);
	}
    }
}

void
MeasurementProcessor::fieldIntegralUncertainty (MsgStream& log, Amg::MatrixX& covariance)
{
    // check field integral stability if there is a large error on the start position/direction
    // but only meaningful when material taken into account
    if (! m_parameters->numberScatterers()
	|| covariance(0,0) <= 0.
	|| covariance(1,1) <= 0.) return;

    if (covariance(0,0) + covariance(1,1)	< m_largeDeltaD0*m_largeDeltaD0
	&& covariance(2,2)			< m_largeDeltaPhi0*m_largeDeltaPhi0) return;

    // FIXME: must also account for asymmetry in case of large momentum error
    //        when momentum correlation terms can be significantly overestimated

    // assume fieldIntegral proportional to angular deflection from vertex to last measurement
    // as problems are always in the toroid take the theta deflection contribution to qOverP
    double sinTheta		=  1./std::sqrt(1. + m_parameters->cotTheta()*m_parameters->cotTheta());
    Amg::Vector3D startDirection(sinTheta*m_parameters->cosPhi(),
				 sinTheta*m_parameters->sinPhi(),
				 sinTheta*m_parameters->cotTheta());
    const FitMeasurement& lastMeas	= **m_measurements.rbegin();
    Amg::Vector3D endDirection	= lastMeas.intersection(FittedTrajectory).direction();
    if (startDirection.z() == 0. || endDirection.z() == 0.) return;
    
    double deflectionPhi	= startDirection.x()*endDirection.y() -
				  startDirection.y()*endDirection.x();
    double deflectionTheta	= startDirection.perp()*endDirection.z() -
				  startDirection.z()*endDirection.perp();

    // poorly measured phi
    double shiftPhi0	= std::sqrt(covariance(2,2));
    if (shiftPhi0 > m_largeDeltaPhi0)
    {
	Amg::Vector3D vertex(m_parameters->position());
	double cosPhi	=  m_parameters->cosPhi() - m_parameters->sinPhi()*shiftPhi0;
	double sinPhi	=  m_parameters->sinPhi() + m_parameters->cosPhi()*shiftPhi0;
	double cotTheta	=  m_parameters->cotTheta();
	sinTheta	=  1./std::sqrt(1. + cotTheta*cotTheta);
	startDirection	=  Amg::Vector3D(sinTheta*cosPhi,
					 sinTheta*sinPhi,
					 sinTheta*cotTheta);
	delete m_vertexIntersect;
	m_vertexIntersect = new TrackSurfaceIntersection(vertex,startDirection,0.);
	if (! extrapolateToMeasurements(DeltaPhi0)) return;
    
	endDirection		=  lastMeas.intersection(DeltaPhi0).direction();
	double deltaPhi		=  startDirection.x()*endDirection.y() -
				   startDirection.y()*endDirection.x() - deflectionPhi;
	double deltaTheta	=  startDirection.perp()*endDirection.z() -
				   startDirection.z()*endDirection.perp() - deflectionTheta;
	covariance(3,3)	+= deltaTheta*deltaTheta;
	if (std::abs(deflectionTheta) > 0.0001)
	{
	    double deltaQOverP	=  (deltaTheta/deflectionTheta) * m_parameters->qOverP();
	    covariance(4,4)	+= deltaQOverP*deltaQOverP;
	}
	if (log.level() < MSG::DEBUG)
	{
	    log << MSG::VERBOSE << std::setiosflags(std::ios::fixed)
		<< " fieldIntegralUncertainty: "
		<< "  shiftPhi0 " << std::setw(8) << std::setprecision(3) << shiftPhi0
		<< " deflection (phi,theta) "
		<< std::setw(8) << std::setprecision(4) << deflectionPhi
		<< std::setw(8) << std::setprecision(4) << deflectionTheta
		<< "   diff "
		<< std::setw(8) << std::setprecision(4) << deltaPhi
		<< std::setw(8) << std::setprecision(4) << deltaTheta
		<< endmsg;
	    return;
	}
    }
    else
    {
	// compute average deflection for a one sigma move towards origin
	double shiftD0	= std::sqrt(covariance(0,0));
	double shiftZ0	= std::sqrt(covariance(1,1));
	if (m_parameters->d0() 			> 0.)	shiftD0 = -shiftD0;
	if (m_parameters->position().z()	> 0.)	shiftZ0 = -shiftZ0;

	Amg::Vector3D vertex(-shiftD0*m_parameters->sinPhi(), shiftD0*m_parameters->cosPhi(), shiftZ0);
	vertex			+= m_parameters->position();
	double dPhi		=  covariance(0,2)/shiftD0;
	double cosPhi		=  m_parameters->cosPhi() - m_parameters->sinPhi()*dPhi;
	double sinPhi		=  m_parameters->sinPhi() + m_parameters->cosPhi()*dPhi;
	double cotTheta		=  m_parameters->cotTheta();
	sinTheta		=  1./std::sqrt(1. + cotTheta*cotTheta);
	cotTheta		-= covariance(1,3)/(shiftZ0*sinTheta*sinTheta);
	startDirection		=  Amg::Vector3D(sinTheta*cosPhi,
						 sinTheta*sinPhi,
						 sinTheta*cotTheta);
	delete m_vertexIntersect;
	m_vertexIntersect = new TrackSurfaceIntersection(vertex,startDirection,0.);
	if (! extrapolateToMeasurements(DeltaD0)) return;
    
	endDirection		=  lastMeas.intersection(DeltaD0).direction();
	double deltaPhi		=  startDirection.x()*endDirection.y() -
				   startDirection.y()*endDirection.x() - deflectionPhi;
	double deltaTheta	=  startDirection.perp()*endDirection.z() -
				   startDirection.z()*endDirection.perp() - deflectionTheta;
    
	covariance(2,2)		+= deltaPhi*deltaPhi;
	covariance(3,3)		+= deltaTheta*deltaTheta;
	if (std::abs(deflectionTheta) > 0.0001)
	{
	    double deltaQOverP	=  (deltaTheta/deflectionTheta) * m_parameters->qOverP();
	    covariance(4,4)	+= deltaQOverP*deltaQOverP;
	}

	if (log.level() < MSG::DEBUG)
	{
	    log << MSG::VERBOSE << std::setiosflags(std::ios::fixed)
		<< " fieldIntegralUncertainty: "
		<< "  shiftD0 " << std::setw(8) << std::setprecision(1) << shiftD0
		<< "  shiftZ0 " << std::setw(8) << std::setprecision(1) << shiftZ0
		<< " deflection (phi,theta) "
		<< std::setw(8) << std::setprecision(4) << deflectionPhi
		<< std::setw(8) << std::setprecision(4) << deflectionTheta
		<< "   diff "
		<< std::setw(8) << std::setprecision(4) << deltaPhi
		<< std::setw(8) << std::setprecision(4) << deltaTheta
		<< endmsg;
	}
    }
}

void
MeasurementProcessor::propagationDerivatives(void)
{
    // compute additional derivatives when needed for covariance propagation.
    //   loop over measurements:
    for (std::list<FitMeasurement*>::iterator m = m_measurements.begin();
	 m != m_measurements.end();
	 ++m)
    {
	// compute the D0 and Z0 derivs that don't already exist
	if (! (**m).isPositionMeasurement() || (**m).numberDoF() > 1) continue;
	int derivativeFlag;
	if ((**m).numberDoF())
	{
	    derivativeFlag	= 0;
	}
	else
	{
	    derivativeFlag	= 2;
	}

	if ((**m).isCluster())
	{
	    clusterDerivatives(derivativeFlag, **m);
	}
	else if ((**m).isDrift() || (**m).isPerigee() || (**m).isPseudo())
	{
	    driftDerivatives(derivativeFlag, **m);
	}
    }
}

//<<<<<< MEMBER PRIVATE FUNCTION DEFINITIONS                            >>>>>>

void
MeasurementProcessor::clusterDerivatives(int derivativeFlag, const FitMeasurement& measurement)
{
    // derivativeFlag definition: 0 take wrt Z0, 1 take wrt D0, 2 take wrt D0 and Z0
    double weight				= measurement.weight();
    const TrackSurfaceIntersection& intersection= measurement.intersection(FittedTrajectory);
    const Amg::Vector3D& minimizationDirection	= measurement.minimizationDirection();
    const Amg::Vector3D& sensorDirection	= measurement.sensorDirection();

    // protect against grazing incidence 
    if (std::abs(measurement.normal().dot(intersection.direction())) < 0.001) return;
    
    // transverse distance to measurement
    double xDistance	= intersection.position().x() - m_x0;
    double yDistance	= intersection.position().y() - m_y0;
    double rDistance	= -(m_cosPhi0*xDistance + m_sinPhi0*yDistance)/(m_sinTheta0*m_sinTheta0);

    if (derivativeFlag != 0)
    {
	// momentum derivative - always numeric
	if (m_parameters->fitEnergyDeposit() && measurement.afterCalo())
	{
	    Amg::Vector3D offset	= measurement.intersection(DeltaQOverP1).position() - intersection.position();
	    measurement.derivative(QOverP1,weight*minimizationDirection.dot(offset)*m_derivQOverP1);
	}
	else if (m_parameters->fitMomentum())
	{
	    Amg::Vector3D offset	= measurement.intersection(DeltaQOverP0).position() - intersection.position();
	    measurement.derivative(QOverP0,weight*minimizationDirection.dot(offset)*m_derivQOverP0);
	}
    
	// analytic derivatives in longitudinal direction
	Amg::Vector3D weightedProjection = weight*sensorDirection.cross(intersection.direction()) /
					   measurement.normal().dot(intersection.direction());
	measurement.derivative(    Z0, weightedProjection.z());
	measurement.derivative(Theta0, weightedProjection.z()*rDistance);
    
	// numeric or analytic d0/phi derivative
	if (measurement.numericalDerivative())
	{
	    Amg::Vector3D offset	= measurement.intersection(DeltaD0).position() - intersection.position();
	    measurement.derivative(    D0,weight*minimizationDirection.dot(offset)/m_delta[DeltaD0]);
	    // 	offset = measurement.intersection(DeltaZ0).position() - intersection.position();
	    // 	measurement.derivative(    Z0,weight*minimizationDirection.dot(offset)/m_delta[DeltaZ0]);
	    offset			= measurement.intersection(DeltaPhi0).position() - intersection.position();
	    measurement.derivative(  Phi0,weight*minimizationDirection.dot(offset)/m_delta[DeltaPhi0]);
	    // 	offset = measurement.intersection(DeltaTheta0).position() - intersection.position();
	    // 	measurement.derivative(Theta0,
	    // 			       weight*minimizationDirection.dot(offset)/m_delta[DeltaTheta0]);
	}
	else
	{
	    // transverse derivatives
	    measurement.derivative(  D0,
		m_cosPhi0*weightedProjection.y() - m_sinPhi0*weightedProjection.x());
	    measurement.derivative(Phi0,
		xDistance*weightedProjection.y() - yDistance*weightedProjection.x());
	}
    
	// derivatives wrt multiple scattering parameters
	// similar to phi/theta
	unsigned param  = m_parameters->firstScatteringParameter();
	std::vector<FitMeasurement*>::const_iterator s = m_scatterers.begin();
	while (++param < measurement.lastParameter())
	{
	    const TrackSurfaceIntersection& scatteringCentre = (**s).intersection(FittedTrajectory);
	    double xDistScat	= intersection.position().x() - scatteringCentre.position().x();
	    double yDistScat	= intersection.position().y() - scatteringCentre.position().y();
	    double rDistScat	= -(scatteringCentre.direction().x()*xDistScat +
				    scatteringCentre.direction().y()*yDistScat) /
				  (scatteringCentre.direction().perp2() *
				   scatteringCentre.direction().perp());
	    measurement.derivative(param,
				   xDistScat*weightedProjection.y() - yDistScat*weightedProjection.x());
	    measurement.derivative(++param, weightedProjection.z()*rDistScat);
	    ++s;
	}

	if (measurement.alignmentParameter())
	{
	    // TODO: fix projection factors wrt principal measurement axis from alignment surface
	    param		= measurement.alignmentParameter();
	    FitMeasurement* fm	= m_alignments[param-1];
	    param		= m_parameters->numberParameters() -2*m_parameters->numberAlignments() + 2*(param-1);
	    double distance	= fm->surface()->normal().dot(measurement.intersection(FittedTrajectory).position() -
							      fm->intersection(FittedTrajectory).position());
	    double projection	= fm->surface()->normal().dot(measurement.intersection(FittedTrajectory).direction());
	    
	    // std::cout << " cluster deriv:  measurement.alignmentParameter " << measurement.alignmentParameter()
	    // 	      << "   at z " << fm->surface()->center().z()
	    // 	      << "   param " << param
	    // 	      << "   distance " << distance
	    // 	      << "   projection " << projection << std::endl;

	    measurement.derivative(param, weight*distance);
	    measurement.derivative(++param, weight*projection);
	    if (measurement.alignmentParameter2())
	    {
		param		= measurement.alignmentParameter2();
		fm	        = m_alignments[param-1];
		param		= m_parameters->numberParameters() -2*m_parameters->numberAlignments() + 2*(param-1);
		distance	= fm->surface()->normal().dot(measurement.intersection(FittedTrajectory).position() -
							      fm->intersection(FittedTrajectory).position());
		projection	= fm->surface()->normal().dot(measurement.intersection(FittedTrajectory).direction());
		measurement.derivative(param, weight*distance);
		measurement.derivative(++param, weight*projection);
            } 
	}
    }
    
    // similar derivatives for the 2nd dimension,
    // with minimization along sensor direction
    if (derivativeFlag == 1) return;
    weight 	= measurement.weight2();
    
    // momentum derivative - always numeric
    if (m_parameters->fitEnergyDeposit() && measurement.afterCalo())
    {
	Amg::Vector3D offset	= measurement.intersection(DeltaQOverP1).position() - intersection.position();
	measurement.derivative2(QOverP1,weight*sensorDirection.dot(offset)*m_derivQOverP1);
    }
    else if (m_parameters->fitMomentum())
    {
	//	std::cout << " intersection " << intersection.position() << std::endl;
	//	std::cout << " intersection " << measurement.intersection(DeltaQOverP0).position() << std::endl;
	Amg::Vector3D offset = measurement.intersection(DeltaQOverP0).position() - intersection.position();
	measurement.derivative2(QOverP0,weight*sensorDirection.dot(offset)*m_derivQOverP0);
    }
    
    // analytic derivatives in longitudinal direction
    // FIXME: orthogonal to 1st coord for DOF != 2 
    Amg::Vector3D weightedProjection	= -weight*minimizationDirection.cross(intersection.direction()) /
					  measurement.normal().dot(intersection.direction());
    measurement.derivative2(    Z0, weightedProjection.z());
    measurement.derivative2(Theta0, weightedProjection.z()*rDistance);
    
    // numeric or analytic d0/phi derivative
    if (measurement.numericalDerivative())
    {
 	Amg::Vector3D offset	= measurement.intersection(DeltaD0).position() - intersection.position();
 	measurement.derivative2(    D0,weight*sensorDirection.dot(offset)/m_delta[DeltaD0]);
// 	offset			= measurement.intersection(DeltaZ0).position() - intersection.position();
// 	measurement.derivative2(    Z0,weight*sensorDirection.dot(offset)/m_delta[DeltaZ0]);
	offset			= measurement.intersection(DeltaPhi0).position() - intersection.position();
	measurement.derivative2(  Phi0,weight*sensorDirection.dot(offset)/m_delta[DeltaPhi0]);
// 	offset			= measurement.intersection(DeltaTheta0).position() - intersection.position();
// 	measurement.derivative2(Theta0,
// 				weight*sensorDirection.dot(offset)/m_delta[DeltaTheta0]);
    }
    else
    {
	measurement.derivative2(  D0,
				m_cosPhi0*weightedProjection.y() - m_sinPhi0*weightedProjection.x());
	measurement.derivative2(Phi0,
				xDistance*weightedProjection.y() - yDistance*weightedProjection.x());
    }
    
    // derivatives wrt multiple scattering parameters
    // similar to phi/theta
    unsigned param  = m_parameters->firstScatteringParameter();
    std::vector<FitMeasurement*>::const_iterator s = m_scatterers.begin();
    while (++param < measurement.lastParameter())
    {
	const TrackSurfaceIntersection& scatteringCentre = (**s).intersection(FittedTrajectory);
	double xDistScat	= intersection.position().x() - scatteringCentre.position().x();
	double yDistScat	= intersection.position().y() - scatteringCentre.position().y();
	double rDistScat	= -(scatteringCentre.direction().x()*xDistScat +
				    scatteringCentre.direction().y()*yDistScat) /
				  (scatteringCentre.direction().perp2() *
				   scatteringCentre.direction().perp());
	measurement.derivative2(param,
				xDistScat*weightedProjection.y() - yDistScat*weightedProjection.x());
	measurement.derivative2(++param, weightedProjection.z()*rDistScat);
	++s;
    }
}

void
MeasurementProcessor::driftDerivatives(int derivativeFlag, const FitMeasurement& measurement)
{
    // transverse distance to measurement
    const TrackSurfaceIntersection& intersection= measurement.intersection(FittedTrajectory);
    double xDistance	= intersection.position().x() - m_x0;
    double yDistance	= intersection.position().y() - m_y0;
    double rDistance	= -(m_cosPhi0*xDistance + m_sinPhi0*yDistance)/(m_sinTheta0*m_sinTheta0);

    // derivativeFlag definition: 0 take wrt Z0, 1 take wrt D0, 2 take wrt D0 and Z0
    if (derivativeFlag != 0)
    {
	double weight				= measurement.weight();
	const Amg::Vector3D& driftDirection	= measurement.minimizationDirection();
	double xDrift  				= driftDirection.x();
	double yDrift  				= driftDirection.y();
	
	// momentum derivative - always numeric
	if (m_parameters->fitEnergyDeposit() && measurement.afterCalo())
	{
	    Amg::Vector3D offset	= measurement.intersection(DeltaQOverP1).position() - intersection.position();
	    measurement.derivative(QOverP1,weight*driftDirection.dot(offset)*m_derivQOverP1);
	}
	else if (m_parameters->fitMomentum())
	{
	    Amg::Vector3D offset 	= measurement.intersection(DeltaQOverP0).position() - intersection.position();
	    measurement.derivative(QOverP0,weight*driftDirection.dot(offset)*m_derivQOverP0);
	}

	if (measurement.numericalDerivative())
	{
	    Amg::Vector3D offset	= measurement.intersection(DeltaD0).position() - intersection.position();
	    measurement.derivative(    D0,weight*driftDirection.dot(offset)/m_delta[DeltaD0]);
	    // 	    offset		= measurement.intersection(DeltaZ0).position() - intersection.position();
	    // 	    measurement.derivative(    Z0,weight*driftDirection.dot(offset)/m_delta[DeltaZ0]);
	    offset = measurement.intersection(DeltaPhi0).position() - intersection.position();
	    measurement.derivative(  Phi0,weight*driftDirection.dot(offset)/m_delta[DeltaPhi0]);
	    // 	    offset		= measurement.intersection(DeltaTheta0).position() - intersection.position();
	    // 	    measurement.derivative(Theta0,weight*driftDirection.dot(offset)/m_delta[DeltaTheta0]);
	}
	else if (m_phiInstability
// 		 && std::abs(intersection.direction().z()*m_vertexIntersect->direction().perp() -
// 			 m_vertexIntersect->direction().z()*intersection.direction().perp()) > m_toroidTurn
		 && ! measurement.isPseudo())
	{
	    measurement.derivative(  D0,0.);
	    measurement.derivative(Phi0,0.);
	}
	else
	{
	    measurement.derivative(  D0,weight*(m_cosPhi0*yDrift - m_sinPhi0*xDrift));
	    measurement.derivative(Phi0,weight*(xDistance*yDrift - yDistance*xDrift));
	}

	measurement.derivative(    Z0,weight*driftDirection.z());
	measurement.derivative(Theta0,weight*driftDirection.z()*rDistance);

	// derivatives wrt multiple scattering parameters
	// similar to phi/theta
	unsigned param  = m_parameters->firstScatteringParameter();
	std::vector<FitMeasurement*>::const_iterator s = m_scatterers.begin();
	while (++param < measurement.lastParameter())
	{
	    const TrackSurfaceIntersection& scatteringCentre = (**s).intersection(FittedTrajectory);
	    double xDistScat	= intersection.position().x() - scatteringCentre.position().x();
	    double yDistScat	= intersection.position().y() - scatteringCentre.position().y();
	    double rDistScat	= -(scatteringCentre.direction().x()*xDistScat +
				    scatteringCentre.direction().y()*yDistScat) /
				  (scatteringCentre.direction().perp2() *
				   scatteringCentre.direction().perp());
 	    measurement.derivative(param, weight*(xDistScat*yDrift - yDistScat*xDrift));
	    measurement.derivative(++param, weight*driftDirection.z()*rDistScat);
	    ++s;
	}

	// derivatives wrt alignment parameters
	if (measurement.alignmentParameter())
	{
	    param		= measurement.alignmentParameter();
	    FitMeasurement* fm	= m_alignments[param-1];
	    param		= m_parameters->numberParameters() -2*m_parameters->numberAlignments() + 2*(param-1);

	    // angle derivative (delta_theta) similar to scatterer delta_theta
	    const TrackSurfaceIntersection& alignmentCentre = fm->intersection(FittedTrajectory);
	    double xDistance  = intersection.position().x() - alignmentCentre.position().x();
	    double yDistance  = intersection.position().y() - alignmentCentre.position().y();
	    double rDistance  = -(alignmentCentre.direction().x()*xDistance +
				  alignmentCentre.direction().y()*yDistance) /
				(alignmentCentre.direction().perp2() *
				 alignmentCentre.direction().perp());
	    measurement.derivative(param, weight*driftDirection.z()*rDistance);
	    
	    // offset derivative: barrel (endcap) projection factor onto the surface plane in the z (r) direction
	    double projection;
	    const Surface& surface	= *fm->surface();
	    if (std::abs(surface.normal().z()) > 0.5)
	    {
		projection = (driftDirection.x()*surface.center().x() + driftDirection.y()*surface.center().y()) /
			     surface.center().perp();
	    }
	    else
	    {
		projection = driftDirection.z();
	    }
	    measurement.derivative(++param, weight*projection);
	     
	    // std::cout << " drift deriv:  measurement.alignmentParameter "
	    // 	      << measurement.alignmentParameter()
	    // 	      << "   at z " << surface.center().z()
	    // 	      << "   from z,r " << alignmentCentre.position().z() 
	    // 	      << ", " << alignmentCentre.position().perp() 
	    // 	      << "   dot normal " << surface.normal().dot(surface.center().unit())
	    // 	      << "   param " << param
	    // 	      << "   distance " << rDistance
	    // 	      << "   driftDirection.z() " << driftDirection.z()
	    // 	      << "   angle derivative " << driftDirection.z()*rDistance
	    // 	      << "   offset derivative (projection factor) " << projection
	    // 	      << std::endl;

	    if (measurement.alignmentParameter2())
	    {
		param	= measurement.alignmentParameter2();
		fm	= m_alignments[param-1];
		param	= m_parameters->numberParameters() -2*m_parameters->numberAlignments() + 2*(param-1);

		// angle derivative (delta_theta) similar to scatterer delta_theta
		const TrackSurfaceIntersection& alignmentCentre = fm->intersection(FittedTrajectory);
		xDistance  = intersection.position().x() - alignmentCentre.position().x();
		yDistance  = intersection.position().y() - alignmentCentre.position().y();
		rDistance  = -(alignmentCentre.direction().x()*xDistance + alignmentCentre.direction().y()*yDistance) /
			     (alignmentCentre.direction().perp2() * alignmentCentre.direction().perp());
		measurement.derivative(param, weight*driftDirection.z()*rDistance);
	    
		// offset derivative: barrel (endcap) projection factor onto the surface plane in the z (r) direction
		const Surface&  surface	= *fm->surface();
		if (surface.normal().dot(surface.center().unit()) < 0.5)
		{
		    projection	= driftDirection.z();
		}
		else
		{
		    projection = (driftDirection.x()*surface.center().x() + driftDirection.y()*surface.center().y()) /
				 surface.center().perp();
		}
		measurement.derivative(++param, weight*projection);
            }
	}
    }
 
    // similar for derivatives along the wire direction
    if (derivativeFlag == 1) return;
    double weight			= measurement.weight2();
    const Amg::Vector3D& wireDirection	= measurement.sensorDirection();
    double xWire			= wireDirection.x();
    double yWire			= wireDirection.y();
    //std::cout << " intersection " << intersection.position() << std::endl;
    //std::cout << " intersection " << measurement.intersection(DeltaQOverP0).position() << std::endl;
	
    // momentum derivative - always numeric
    if (m_parameters->fitEnergyDeposit() && measurement.afterCalo())
    {
	Amg::Vector3D offset	= measurement.intersection(DeltaQOverP1).position() - intersection.position();
	measurement.derivative2(QOverP1,weight*wireDirection.dot(offset)*m_derivQOverP1);
    }
    else if (m_parameters->fitMomentum())
    {
	Amg::Vector3D offset	= measurement.intersection(DeltaQOverP0).position() - intersection.position();
	measurement.derivative2(QOverP0,weight*wireDirection.dot(offset)*m_derivQOverP0);
    }
    
    if (measurement.numericalDerivative())
    {
	Amg::Vector3D offset	= measurement.intersection(DeltaD0).position() - intersection.position();
	measurement.derivative2(    D0,weight*wireDirection.dot(offset)/m_delta[DeltaD0]);
	// 	    offset 	= measurement.intersection(DeltaZ0).position() - intersection.position();
	// 	    measurement.derivative(Z0,weight*wireDirection.dot(offset)/m_delta[DeltaZ0]);
	offset = measurement.intersection(DeltaPhi0).position() - intersection.position();
	measurement.derivative2(  Phi0,weight*wireDirection.dot(offset)/m_delta[DeltaPhi0]);
	// 	    offset	= measurement.intersection(DeltaTheta0).position() - intersection.position();
	// 	    measurement.derivative(Theta0,weight*wireDirection.dot(offset)/m_delta[DeltaTheta0]);
    }
    else
    {
	measurement.derivative2(  D0,weight*(m_cosPhi0*yWire - m_sinPhi0*xWire));
	measurement.derivative2(Phi0,weight*(xDistance*yWire - yDistance*xWire));
    }

    measurement.derivative2(    Z0,weight*wireDirection.z());
    measurement.derivative2(Theta0,weight*wireDirection.z()*rDistance);

    // derivatives wrt multiple scattering parameters
    // similar to phi/theta
    unsigned param  = m_parameters->firstScatteringParameter();
    std::vector<FitMeasurement*>::const_iterator s = m_scatterers.begin();
    while (++param < measurement.lastParameter())
    {
	const TrackSurfaceIntersection& scatteringCentre = (**s).intersection(FittedTrajectory);
	double xDistScat	= intersection.position().x() - scatteringCentre.position().x();
	double yDistScat	= intersection.position().y() - scatteringCentre.position().y();
	double rDistScat	= -(scatteringCentre.direction().x()*xDistScat +
				    scatteringCentre.direction().y()*yDistScat) /
				  (scatteringCentre.direction().perp2() *
				   scatteringCentre.direction().perp());
	measurement.derivative2(param, weight*(xDistScat*yWire - yDistScat*xWire));
	measurement.derivative2(++param, weight*wireDirection.z()*rDistScat);
	++s;
    }
}

bool
MeasurementProcessor::extrapolateToMeasurements(ExtrapolationType type)
{
    // extrapolate to all measurement surfaces
    int nScat  						= 0;
    double qOverP					= m_qOverP[type];
    const TrackSurfaceIntersection* intersection	= m_vertexIntersect;
    const Surface* surface				= 0;
    
    // careful: use RungeKutta for extrapolation to vertex measurement
    std::list<FitMeasurement*>::iterator m = m_measurements.begin();
    if ((**m).isVertex())
    {
        if(m_useStepPropagator==99) {
          const TrackSurfaceIntersection* newIntersectionSTEP =
          m_stepPropagator->intersectSurface(*(**m).surface(),
                                           intersection,
                                           qOverP,
                                           m_stepField,
                                           Trk::muon);
          double exdist = 0;
          if(newIntersectionSTEP) exdist = (newIntersectionSTEP->position()-intersection->position()).mag();
	  intersection	= m_rungeKuttaIntersector->intersectSurface(*(**m).surface(),
	 							    intersection,
								    qOverP);
          if(newIntersectionSTEP) {
            double dist = 1000.*(newIntersectionSTEP->position()-intersection->position()).mag();
            std::cout << " iMeasProc 1 distance STEP and Intersector " << dist << " ex dist " << exdist << std::endl;
            if(dist>10.) std::cout << " iMeasProc 1 ALARM distance STEP and Intersector " << dist << " ex dist " << exdist << std::endl;
            delete newIntersectionSTEP;
          } else {
            if(intersection) std::cout << " iMeasProc 1 ALARM STEP did not intersect! " << std::endl;
          }
        } else {
	  intersection	= m_useStepPropagator==0?
            m_rungeKuttaIntersector->intersectSurface(*(**m).surface(),
			  			      intersection,
						      qOverP):
            m_stepPropagator->intersectSurface(*(**m).surface(),
                                               intersection,
                                               qOverP,
                                               m_stepField,
                                               Trk::muon);
        }
	surface		= (**m).surface();	
	if (! intersection)		return false;
	(**m).intersection(type,intersection);
	if (type == FittedTrajectory) (**m).qOverP(qOverP);
	++m;
    }
    
    for ( ; m != m_measurements.end(); ++m)
    {
	if ((**m).afterCalo())
	{
	    if (type == DeltaQOverP0)	continue;
	}
	else if (type == DeltaQOverP1)
	{
	    intersection	= &(**m).intersection(FittedTrajectory);
	    qOverP		= (**m).qOverP();
	    if ((**m).numberDoF() && (**m).isScatterer()) ++nScat;
	    continue;
	}

	// to avoid rounding: copy intersection if at previous surface 
	if ((**m).surface() != surface)
	{
          if(m_useStepPropagator==99) {
            const TrackSurfaceIntersection* newIntersectionSTEP =
            m_stepPropagator->intersectSurface(*(**m).surface(),
                                           intersection,
                                           qOverP,
                                           m_stepField,
                                           Trk::muon);
            double exdist = 0;
            if(newIntersectionSTEP) exdist = (newIntersectionSTEP->position()-intersection->position()).mag();

	    intersection	= m_intersector->intersectSurface(*(**m).surface(),
								  intersection,
								  qOverP);
            if(newIntersectionSTEP) {
              double dist = 1000.*(newIntersectionSTEP->position()-intersection->position()).mag();
              std::cout << " iMeasProc 2 distance STEP and Intersector " << dist << " ex dist " << exdist << std::endl;
              if(dist>10.) std::cout << " iMeasProc 2 ALARM distance STEP and Intersector " << dist << " ex dist " << exdist << std::endl;
              delete newIntersectionSTEP;
            } else {
              if(intersection) std::cout << " iMeasProc 2 ALARM STEP did not intersect! " << std::endl;
            }
          } else {   
	    intersection	= m_useStepPropagator==0?
              m_rungeKuttaIntersector->intersectSurface(*(**m).surface(),
			  			      intersection,
						      qOverP):
              m_stepPropagator->intersectSurface(*(**m).surface(),
                                               intersection,
                                               qOverP,
                                               m_stepField,
                                               Trk::muon);
          }
	  surface		= (**m).surface();
	}
	else
	{
	    intersection	= new TrackSurfaceIntersection(*intersection);
	}
	if (! intersection) return false;

	// alignment and material effects (energy loss and trajectory deviations)
	if ((**m).isEnergyDeposit() || (**m).isScatterer())
	{
	    // apply fitted parameters
	    if ((**m).numberDoF())
	    {
		if ((**m).isEnergyDeposit()) // reserved for calorimeter
		{
		    if (type == FittedTrajectory)
		    {
			m_qOverPbeforeCalo	= qOverP;
			m_qOverPafterCalo	= m_parameters->qOverP1();
			qOverP			= m_qOverPafterCalo;
			(**m).qOverP(qOverP);
		    }
		    else if (type == DeltaQOverP1)
		    {
			delete intersection;
			intersection	=
			    new TrackSurfaceIntersection((**m).intersection(FittedTrajectory));
			qOverP		= m_qOverPafterCalo + m_delta[DeltaQOverP1];
		    }
		    else
		    {
			qOverP		= m_qOverPafterCalo;
		    }
		    (**m).intersection(type,intersection);
		    continue;
		}

		if ((**m).isScatterer())	// scattering centre
		{
		    // update track direction for fitted scattering
		    double sinDeltaPhi	=  std::sin(m_parameters->scattererPhi(nScat));
		    double cosDeltaPhi	=  std::sqrt(1. - sinDeltaPhi*sinDeltaPhi);
		    double sinDeltaTheta	=  std::sin(m_parameters->scattererTheta(nScat));
		    double tanDeltaTheta	=  sinDeltaTheta;
		    if (std::abs(sinDeltaTheta) < 1.)
			tanDeltaTheta	/= std::sqrt(1. - sinDeltaTheta*sinDeltaTheta);
		    Amg::Vector3D trackDirection(intersection->direction());
		    trackDirection		/= trackDirection.perp();
		    double cosPhi		=  trackDirection.x()*cosDeltaPhi -
						   trackDirection.y()*sinDeltaPhi;
		    double sinPhi		=  trackDirection.y()*cosDeltaPhi +
						   trackDirection.x()*sinDeltaPhi;
		    double cotTheta		=  (trackDirection.z() - tanDeltaTheta) /
						   (1. + trackDirection.z()*tanDeltaTheta);
		    trackDirection		=  Amg::Vector3D(cosPhi,sinPhi,cotTheta);
		    trackDirection		=  trackDirection.unit();
		    const TrackSurfaceIntersection* oldIntersection = intersection;
		    intersection		= new TrackSurfaceIntersection(intersection->position(),
									       trackDirection,
									       intersection->pathlength());
		    delete oldIntersection;
		    ++nScat;
		}
	    }
	}

	// apply unfitted energy loss
	if (m_parameters->fitMomentum())
	{
	    // at extreme momenta use series expansion to avoid rounding (or FPE)
	    if (m_parameters->extremeMomentum())
	    {
		double loss	=  (**m).energyLoss()*std::abs(qOverP);
		qOverP		*= 1. + loss + loss*loss + loss*loss*loss;
	    }
	    else
	    {		
		double momentum	=  1./qOverP;
		if (momentum > 0.)
		{
		    momentum	-= (**m).energyLoss();
		}
		else
		{
		    momentum	+= (**m).energyLoss();
		}
		qOverP		=  1./momentum;
	    }
	}

	// store intersection and momentum vector (leaving surface)
	if (type == FittedTrajectory) (**m).qOverP(qOverP);
	(**m).intersection(type,intersection);
    }

    return true;
}

}	// end of namespace
