/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/***************************************************************************
 local parameter values used during fitter
 ***************************************************************************/

//<<<<<< INCLUDES                                                       >>>>>>

#include <cmath>
#include <iomanip>
#include <iostream>
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/SystemOfUnits.h"
#include "TrkEventPrimitives/ParamDefs.h"
#include "TrkExUtils/TrackSurfaceIntersection.h"
#include "TrkMaterialOnTrack/ScatteringAngles.h"
#include "TrkSurfaces/Surface.h"
#include "TrkiPatFitterUtils/ExtrapolationType.h"
#include "TrkiPatFitterUtils/FitMeasurement.h"
#include "TrkiPatFitterUtils/FitParameters.h"
#include "TrkiPatFitterUtils/ParameterType.h"

namespace Trk{
  
//<<<<<< CLASS STRUCTURE INITIALIZATION                                 >>>>>>

FitParameters::FitParameters (const Perigee& perigee)
    : m_cosPhi1				(0.),
      m_cosTheta1			(0.),
      m_d0         	       		(perigee.parameters()[Trk::d0]),
      m_differences			(0),
      m_differencesAlSpaMat 		(0),
      m_eigen				(true),
      m_extremeMomentum 		(false),
      m_finalCovariance 		(0),
      m_firstAlignmentParameter 	(0),
      m_firstScatteringParameter	(0),
      m_fitEnergyDeposit		(false),
      m_fitMomentum			(true),
      m_fullCovariance	       		(0),
      m_minEnergyDeposit		(0.),
      m_numberAlignments		(0),
      m_numberOscillations		(0),
      m_numberParameters       		(0),
      m_numberScatterers		(0),
      m_oldDifference			(0.),
      m_perigee				(&perigee),
      m_phiInstability			(false),
      m_qOverP1        	       		(0.),
      m_sinPhi1				(0.),
      m_sinTheta1			(0.),
      m_surface		       		(&perigee.associatedSurface()),
      m_vertex		       		(perigee.associatedSurface().center()),
      m_z0				(perigee.position().z())
{
    Amg::Vector3D momentum	=  perigee.momentum(); 
    double ptInv0		=  1./momentum.perp();
    m_cosPhi			=  ptInv0*momentum.x();
    m_sinPhi			=  ptInv0*momentum.y();
    m_cotTheta			=  ptInv0*momentum.z();
    m_sinTheta			=  1./std::sqrt(1. + m_cotTheta*m_cotTheta);
    m_cosTheta			=  m_cotTheta*m_sinTheta;
    m_qOverP			=  perigee.charge()*ptInv0*m_sinTheta;
    m_position			=  Amg::Vector3D(m_vertex.x() - m_d0*m_sinPhi,
						 m_vertex.y() + m_d0*m_cosPhi,
						 m_z0);
}

FitParameters::FitParameters (double			d0,
			      double			z0,
			      double			cosPhi,
			      double			sinPhi,
			      double			cotTheta,
			      double			ptInv0,
			      const PerigeeSurface&	surface)
    : m_cosPhi  	       		(cosPhi),
      m_cosPhi1 			(0.),
      m_cosTheta1			(0.),
      m_cotTheta        		(cotTheta),
      m_d0         	       		(d0),
      m_differences			(0),
      m_differencesAlSpaMat 		(0),
      m_eigen				(true),
      m_extremeMomentum 		(false),
      m_finalCovariance 		(0),
      m_firstAlignmentParameter 	(0),
      m_firstScatteringParameter	(0),
      m_fitEnergyDeposit		(false),
      m_fitMomentum			(true),
      m_fullCovariance	       		(0),
      m_minEnergyDeposit		(0.),
      m_numberAlignments		(0),
      m_numberOscillations		(0),
      m_numberParameters       		(0),
      m_numberScatterers		(0),
      m_oldDifference			(0.),
      m_perigee				(0),
      m_phiInstability			(false),
      m_qOverP1        	       		(0.),
      m_sinPhi         	       		(sinPhi),
      m_sinPhi1				(0.),
      m_sinTheta1			(0.),
      m_surface				(&surface),
      m_vertex		       		(surface.center()),
      m_z0				(z0)
{
    m_sinTheta		=  1./std::sqrt(1. + m_cotTheta*m_cotTheta);
    m_cosTheta		=  m_cotTheta*m_sinTheta;
    m_qOverP		=  ptInv0*m_sinTheta;
    m_z0		=  z0;
    m_position		=  Amg::Vector3D(m_vertex.x() - m_d0*m_sinPhi,
					 m_vertex.y() + m_d0*m_cosPhi,
					 m_z0);
}

FitParameters::FitParameters (const FitParameters&	parameters)
    : m_alignmentAngle 			(parameters.m_alignmentAngle),
      m_alignmentAngleConstraint	(parameters.m_alignmentAngleConstraint),
      m_alignmentOffset			(parameters.m_alignmentOffset),
      m_alignmentOffsetConstraint	(parameters.m_alignmentOffsetConstraint),
      m_cosPhi         	       		(parameters.m_cosPhi),
      m_cosPhi1				(parameters.m_cosPhi1),
      m_cosTheta			(parameters.m_cosTheta),
      m_cosTheta1			(parameters.m_cosTheta1),
      m_cotTheta			(parameters.m_cotTheta),
      m_d0				(parameters.m_d0),
      m_differences			(0),
      m_differencesAlSpaMat 		(0),
      m_eigen				(parameters.m_eigen),
      m_extremeMomentum 		(parameters.m_extremeMomentum),
      m_finalCovariance 		(parameters.m_finalCovariance),
      m_firstAlignmentParameter 	(parameters.m_firstAlignmentParameter),
      m_firstScatteringParameter	(parameters.m_firstScatteringParameter),
      m_fitEnergyDeposit		(parameters.m_fitEnergyDeposit),
      m_fitMomentum			(parameters.m_fitMomentum),
      m_fullCovariance 			(parameters.m_fullCovariance),
      m_minEnergyDeposit		(parameters.m_minEnergyDeposit),
      m_numberAlignments		(parameters.m_numberAlignments),
      m_numberOscillations		(parameters.m_numberOscillations),
      m_numberParameters       		(parameters.m_numberParameters),
      m_numberScatterers		(parameters.m_numberScatterers),
      m_oldDifference			(parameters.m_oldDifference),
      m_perigee				(parameters.m_perigee),
      m_phiInstability 			(parameters.m_phiInstability),
      m_position			(parameters.m_position),
      m_qOverP 				(parameters.m_qOverP),
      m_qOverP1        	       		(parameters.m_qOverP1),
      m_scattererPhi			(parameters.m_scattererPhi),
      m_scattererTheta 			(parameters.m_scattererTheta),
      m_sinPhi         	       		(parameters.m_sinPhi),
      m_sinPhi1				(parameters.m_sinPhi1),
      m_sinTheta			(parameters.m_sinTheta),
      m_sinTheta1			(parameters.m_sinTheta1),
      m_surface 			(parameters.m_surface),
      m_vertex  			(parameters.m_vertex),
      m_z0				(parameters.m_z0)
{
    if (m_eigen)
    {
	if (parameters.m_differences) m_differences = new Amg::VectorX(*(parameters.m_differences));
    }
    else if (parameters.m_differencesAlSpaMat) 
    {
	m_differencesAlSpaMat = new AlVec(*parameters.m_differencesAlSpaMat);
    }
}

FitParameters::~FitParameters (void)
{
    delete m_differences;
    delete m_differencesAlSpaMat;
}

//<<<<<< MEMBER FUNCTION DEFINITIONS                                    >>>>>>

void
FitParameters::addAlignment (bool constrained, double angle, double offset)
{
    m_alignmentAngle[m_numberAlignments]	= angle;
    m_alignmentOffset[m_numberAlignments]	= offset;
    if (constrained)
    {
	m_alignmentAngleConstraint[m_numberAlignments]	= angle;
	m_alignmentOffsetConstraint[m_numberAlignments]	= offset;
    }	
    ++m_numberAlignments;
}

void
FitParameters::addScatterer (double phi, double theta)
{
    m_scattererPhi[m_numberScatterers]		= phi;
    m_scattererTheta[m_numberScatterers]	= theta;
    ++m_numberScatterers;
}

const Surface*
FitParameters::associatedSurface (void) const
{
    if (! m_perigee) return 0;
    return &m_perigee->associatedSurface();
}

void
FitParameters::covariance (Amg::MatrixX* finalCovariance, const Amg::MatrixX* fullCovariance)
{
    m_finalCovariance	= finalCovariance;
    m_fullCovariance	= fullCovariance;
}

void
FitParameters::d0 (double value)
{
    m_d0	= value;
    m_position	= Amg::Vector3D(m_vertex.x() - m_d0*m_sinPhi,
				m_vertex.y() + m_d0*m_cosPhi,
				m_z0);
}

void
FitParameters::extremeMomentum (bool value)
{
    m_extremeMomentum	= value;
    m_fitEnergyDeposit	= ! value;
}

void
FitParameters::fitEnergyDeposit (double minEnergyDeposit)
{
    m_fitEnergyDeposit	= true;
    m_minEnergyDeposit	= minEnergyDeposit;
}

void
FitParameters::fitMomentum (bool value)
{
    m_fitMomentum	= value;
}

TrackSurfaceIntersection*
FitParameters::intersection (void) const
{
    return new TrackSurfaceIntersection(
	m_position,
	Amg::Vector3D(m_cosPhi*m_sinTheta,m_sinPhi*m_sinTheta,m_cosTheta),
	0.);
}

void
FitParameters::numberAlignments (int numberAlignments)
{
    m_numberAlignments		= 0;
    if (! numberAlignments) return;
    m_alignmentAngle		= std::vector<double>(numberAlignments,0.);
    m_alignmentAngleConstraint	= std::vector<double>(numberAlignments,0.);
    m_alignmentOffset		= std::vector<double>(numberAlignments,0.);
    m_alignmentOffsetConstraint	= std::vector<double>(numberAlignments,0.);
}

void
FitParameters::numberParameters (int numberParameters)
{
    m_numberParameters		= numberParameters;
    m_firstAlignmentParameter	= numberParameters - 2*m_numberAlignments - 1;
    m_firstScatteringParameter	=  m_firstAlignmentParameter - 2*m_numberScatterers;
}

void
FitParameters::numberScatterers (int numberScatterers)
{
    m_numberScatterers		= 0;
    if (! numberScatterers) return;
    m_scattererPhi		= std::vector<double>(numberScatterers,0.);
    m_scattererTheta		= std::vector<double>(numberScatterers,0.);
}

const Amg::MatrixX
FitParameters::parameterDifference (const Amg::VectorX& parameters) const
{
    Amg::MatrixX difference(1,5);
    difference(0,0) = parameters(0) - m_d0;
    difference(0,1) = parameters(1) - m_z0;
    difference(0,2) = parameters(3)*m_cosPhi - parameters(2)*m_sinPhi;

    // FIXME: diff is now delta(theta) : check sign
    // difference[0][3] = parameters[4] - m_cotTheta;
    difference(0,3) = std::sin(parameters(4))*m_cosTheta - std::cos(parameters(4))*m_sinTheta;
    difference(0,4) = (parameters(5) - m_qOverP)*Gaudi::Units::TeV;
    return difference;
}
 
void
FitParameters::performCutStep (double cutStep)
{
    // revert parameters to previous parameter change with cutStep*value
    // i.e. 0 < cutstep < 1 such that cutStep = 0 gives complete reversion

    if (m_eigen)
    {
	Amg::VectorX cutDifferences(*m_differences);
	cutDifferences	*= (cutStep - 1.);
	Amg::VectorX oldDifferences(*m_differences);
	oldDifferences	*= cutStep;

	// leave phi alone when unstable
	if (m_phiInstability)
	{
	    cutDifferences(2) = 0.;
	    oldDifferences(2) = (*m_differences)(2);
	}

	// apply cut
	update(cutDifferences);
	delete m_differences;
	m_differences		= new Amg::VectorX(oldDifferences);
    }
    else
    {
	AlVec cutDifferences(*m_differencesAlSpaMat);
	cutDifferences	*= (cutStep - 1.);
	AlVec oldDifferences(*m_differencesAlSpaMat);
	oldDifferences	*= cutStep;

	// leave phi alone when unstable
	if (m_phiInstability)
	{
	    cutDifferences[2] = 0.;
	    oldDifferences[2] = (*m_differencesAlSpaMat)[2];
	}

	// apply cut
	update(cutDifferences);
	delete m_differencesAlSpaMat;
	m_differencesAlSpaMat	= new AlVec(oldDifferences);
    }
    
    m_numberOscillations	= 0;
    m_oldDifference		= 0.;
    //std::cout << " after cutstep " << std::endl;
}

Perigee*
FitParameters::perigee (void) const
{
    // copy 'final' covariance
    AmgSymMatrix(5)* covMatrix	= new AmgSymMatrix(5) (*m_finalCovariance);
    double		pT		= std::abs(m_sinTheta/m_qOverP);
    double		charge		= 1.;
    if (m_qOverP < 0.)	charge		= -1.;
    Amg::Vector3D	momentum(pT*m_cosPhi,pT*m_sinPhi,pT*m_cotTheta);

    if (m_surface)
    {
	return new Perigee(m_position,
			   momentum,
			   charge,
			   dynamic_cast<const Trk::PerigeeSurface&>(*m_surface),
			   covMatrix);
    }
    else
    {
	return new Perigee(m_position,
			   momentum,
			   charge,
			   m_vertex,
			   covMatrix);
    }
}

bool
FitParameters::phiInstability (void) const
{ return m_phiInstability; }

void
FitParameters::print (MsgStream& log) const
{
    log << std::setiosflags(std::ios::fixed|std::ios::right)
	<< std::setw(16) << std::setprecision(1) << m_position.perp()
	<< " perigee radius"
	<< std::setw(10) << std::setprecision(3) << m_d0
	<< " d0"
	<< std::setw(11) << std::setprecision(3) << m_position.z()
	<< " z0"
	<< std::setw(11) << std::setprecision(6) << std::atan2(m_sinPhi,m_cosPhi)
	<< " phi"
	<< std::setw(11) << std::setprecision(6) << std::acos(m_cosTheta)
	<< " theta" 
	<< std::setw(13) << std::setprecision(3) << m_sinTheta/(m_qOverP*Gaudi::Units::GeV)
	<< " pT (GeV)";
}

void
FitParameters::printCovariance (MsgStream& log) const
{
    double error00	= 0.;
    if ((*m_finalCovariance)(0,0) > 0.) error00 = std::sqrt((*m_finalCovariance)(0,0));
    double error11	= 0.;
    if ((*m_finalCovariance)(1,1) > 0.) error11 = std::sqrt((*m_finalCovariance)(1,1));
    double error22	= 0.;
    if ((*m_finalCovariance)(2,2) > 0.) error22 = std::sqrt((*m_finalCovariance)(2,2));
    double error33	= 0.;
    if ((*m_finalCovariance)(3,3) > 0.) error33 = std::sqrt((*m_finalCovariance)(3,3));
    double error44	= 0.;
    if ((*m_finalCovariance)(4,4) > 0.) error44 = std::sqrt((*m_finalCovariance)(4,4));
    double correl02 	= 0.;
    double denom02	= (*m_finalCovariance)(0,0)*(*m_finalCovariance)(2,2);
    if (denom02 > 0.) correl02	= (*m_finalCovariance)(0,2) / std::sqrt(denom02);
    double correl13	= 0.;
    double denom13	= (*m_finalCovariance)(1,1)*(*m_finalCovariance)(3,3);
    if (denom13 > 0.) correl13	= (*m_finalCovariance)(1,3) / std::sqrt(denom13);
    log << std::setiosflags(std::ios::fixed|std::ios::right)
	<< std::setw(10) << std::setprecision(3) << error00
	<< std::setw(14) << std::setprecision(3) << error11
	<< std::setw(14) << std::setprecision(6) << error22
	<< std::setw(15) << std::setprecision(6) << error33
	<< std::setw(19) << std::setprecision(3)
	<< (error44*m_sinTheta)/(m_qOverP*m_qOverP*Gaudi::Units::GeV)
	<< " (covariance)"
	<< std::setw(9) << std::setprecision(3) << correl02 << " Cd0phi"
	<< std::setw(8) << std::setprecision(3) << correl13 << " Cz0theta"
	<< std::endl;
}

void
FitParameters::printVerbose (MsgStream& log) const
{
    log << std::endl;

    if (m_differences && m_differences->size())
    {
	const Amg::VectorX& differences  = *m_differences;
	log << "      dParams ===="
	    << std::setiosflags(std::ios::fixed)
	    << std::setw(10) << std::setprecision(4) << differences(0)
	    << " (0) "
	    << std::setw(10) << std::setprecision(4) << differences(1)
	    << " (1) "                                                          
	    << std::setw(10) << std::setprecision(5) << differences(2)
	    << " (2) "                          
	    << std::setw(10) << std::setprecision(5) << differences(3)
	    << " (3) "                     
	    << std::setw(13) << std::setprecision(9)
	    << differences(4)*Gaudi::Units::GeV/Gaudi::Units::TeV	<< " (4) ";
	if (m_fitEnergyDeposit)
	    log << std::setiosflags(std::ios::fixed)
		<< std::setw(13) << std::setprecision(9)
		<< differences(5)*Gaudi::Units::GeV/Gaudi::Units::TeV	<< " (5) ";
	log << std::endl;
	    
	if (m_numberAlignments)
	{
	    log << "       dAlign ==== ";
	    unsigned param = m_firstAlignmentParameter;
	    for (int scat = 0; scat < m_numberAlignments; ++scat)
	    {
		++param;
		if (scat%5 == 0 && scat > 0) log << std::endl << "                   ";
		log << std::setiosflags(std::ios::fixed)
		    << std::setw(10) << std::setprecision(6)
		    << differences(param);
		++param;
		log << std::setiosflags(std::ios::fixed)
		    << std::setw(10) << std::setprecision(6)
		    << differences(param)
		    << " ("<< std::setw(2) << scat << "A)     ";
	    }
	    log << std::endl;
	}

	if (m_numberScatterers)
	{
	    log << "        dScat ==== ";
	    unsigned param = m_firstScatteringParameter;
	    for (int scat = 0; scat < m_numberScatterers; ++scat)
	    {
		++param;
		if (scat%5 == 0 && scat > 0) log << std::endl << "                   ";
		log << std::setiosflags(std::ios::fixed)
		    << std::setw(10) << std::setprecision(6)
		    << differences(param);
		++param;
		log << std::setiosflags(std::ios::fixed)
		    << std::setw(10) << std::setprecision(6)
		    << differences(param)
		    << " ("<< std::setw(2) << scat << "S)     ";
	    }
	    log << std::endl;
	}
    }

    if (m_differencesAlSpaMat && m_differencesAlSpaMat->size())
    {
	const AlVec& differences = *m_differencesAlSpaMat;
	log << "      dParams ===="
	    << std::setiosflags(std::ios::fixed)
	    << std::setw(10) << std::setprecision(4) << differences[0]
	    << " (0) "
	    << std::setw(10) << std::setprecision(4) << differences[1]
	    << " (1) "
	    << std::setw(10) << std::setprecision(5) << differences[2]
	    << " (2) "
	    << std::setw(10) << std::setprecision(5) << differences[3]
	    << " (3) "
	    << std::setw(13) << std::setprecision(9)
	    << differences[4]*Gaudi::Units::GeV/Gaudi::Units::TeV	<< " (4) ";
	if (m_fitEnergyDeposit)
	    log << std::setiosflags(std::ios::fixed)
		<< std::setw(13) << std::setprecision(9)
		<< differences[5]*Gaudi::Units::GeV/Gaudi::Units::TeV	<< " (5) ";
	log << std::endl;
	    
	if (m_numberAlignments)
	{
	    log << "       dAlign ==== ";
	    unsigned param = m_firstAlignmentParameter;
	    for (int scat = 0; scat < m_numberAlignments; ++scat)
	    {
		++param;
		if (scat%5 == 0 && scat > 0) log << std::endl << "                   ";
		log << std::setiosflags(std::ios::fixed)
		    << std::setw(10) << std::setprecision(6)
		    << differences[param];
		++param;
		log << std::setiosflags(std::ios::fixed)
		    << std::setw(10) << std::setprecision(6)
		    << differences[param]
		    << " ("<< std::setw(2) << scat << "A)     ";
	    }
	    log << std::endl;
	}

	if (m_numberScatterers)
	{
	    log << "        dScat ==== ";
	    unsigned param = m_firstScatteringParameter;
	    for (int scat = 0; scat < m_numberScatterers; ++scat)
	    {
		++param;
		if (scat%5 == 0 && scat > 0) log << std::endl << "                   ";
		log << std::setiosflags(std::ios::fixed)
		    << std::setw(10) << std::setprecision(6)
		    << differences[param];
		++param;
		log << std::setiosflags(std::ios::fixed)
		    << std::setw(10) << std::setprecision(6)
		    << differences[param]
		    << " ("<< std::setw(2) << scat << "S)     ";
	    }
	    log << std::endl;
	}
    }
    log << std::setiosflags(std::ios::fixed|std::ios::right)
	<< "   parameters:   "
	<< std::setw(12) << std::setprecision(4) << m_d0
	<< " transverse impact  "
	<< std::setw(10) << std::setprecision(4) << m_position.z()
	<< " z0  "  
	<< std::setw(10) << std::setprecision(6) << std::atan2(m_sinPhi,m_cosPhi)
	<< std::setw(10) << std::setprecision(6) << m_cotTheta
	<< " phi,cotTheta  " 
	<< std::setw(13) << std::setprecision(9) << m_qOverP/m_sinTheta
	<< " inverse pT  "
	<< std::setw(12) << std::setprecision(6) << m_sinTheta/(m_qOverP*Gaudi::Units::GeV)
	<< " signed pT  ";
    if (m_fitEnergyDeposit)
    {
	// TODO: should give fitted energy loss 
	log << std::endl
	    << "    E before/after energy deposit"
	    << std::setw(12) << std::setprecision(3) << 1./std::abs(m_qOverP*Gaudi::Units::GeV)
	    << std::setw(12) << std::setprecision(3) << 1./std::abs(m_qOverP1*Gaudi::Units::GeV);
    }
    if (m_numberAlignments)
    {
	log << std::endl
	    << "   alignment number, angle, offset:  ";
	for (int align = 0; align < m_numberAlignments; ++align)
	{
	    log << std::setiosflags(std::ios::fixed)
		<< std::setw(6) << align
		<< std::setw(10) << std::setprecision(6)
		<< m_alignmentAngle[align]
		<< std::setw(10) << std::setprecision(6)
		<< m_alignmentOffset[align];
	    if ((align+1)%5 == 0)
		log << std::endl
		    << "                                                ";
	}
    }
    if (m_numberScatterers)
    {
	log << std::endl
	    << "   scatterer number, delta(phi), delta(theta):  ";
	for (int scat = 0; scat < m_numberScatterers; ++scat)
	{
	    log << std::setiosflags(std::ios::fixed)
		<< std::setw(6) << scat
		<< std::setw(10) << std::setprecision(6)
		<< m_scattererPhi[scat]
		<< std::setw(10) << std::setprecision(6)
		<< m_scattererTheta[scat];
	    if ((scat+1)%5 == 0)
		log << std::endl
		    << "                                                ";
	}
    }
    log << std::endl;
}

void
FitParameters::qOverP (double value)
{ m_qOverP = value; }

void
FitParameters::qOverP1 (double value)
{ m_qOverP1 = value; }

void
FitParameters::reset (const FitParameters& parameters)
{
    // method is needed to complement copy in places where design uses
    // a reference to a FitParameter pointer
    // essentially a copy, with history of previous iteration removed 
    m_cosPhi         	       	= parameters.m_cosPhi;
    m_cosPhi1			= parameters.m_cosPhi1;
    m_cosTheta			= parameters.m_cosTheta;
    m_cosTheta1			= parameters.m_cosTheta1;
    m_cotTheta               	= parameters.m_cotTheta;
    m_d0         	       	= parameters.m_d0;
    m_extremeMomentum		= parameters.m_extremeMomentum;
    m_finalCovariance	       	= parameters.m_finalCovariance;
    m_firstScatteringParameter	= parameters.m_firstScatteringParameter;
    m_firstScatteringParameter	= parameters.m_firstScatteringParameter;
    m_fitEnergyDeposit		= parameters.m_fitEnergyDeposit;
    m_fitMomentum		= parameters.m_fitMomentum;
    m_fullCovariance	       	= parameters.m_fullCovariance;
    m_minEnergyDeposit		= parameters.m_minEnergyDeposit;
    m_numberAlignments		= parameters.m_numberAlignments;
    m_numberOscillations	= parameters.m_numberOscillations;
    m_numberParameters       	= parameters.m_numberParameters;
    m_numberScatterers		= parameters.m_numberScatterers;
    m_oldDifference		= parameters.m_oldDifference;
    m_perigee			= parameters.m_perigee;
    m_phiInstability		= parameters.m_phiInstability;
    m_position        	       	= parameters.m_position;
    m_qOverP        	       	= parameters.m_qOverP;
    m_qOverP1        	       	= parameters.m_qOverP1;
    m_sinPhi         	       	= parameters.m_sinPhi;
    m_sinPhi1			= parameters.m_sinPhi1;
    m_sinTheta			= parameters.m_sinTheta;
    m_sinTheta1			= parameters.m_sinTheta1;
    m_vertex		       	= parameters.vertex();
    m_z0			= parameters.m_z0;
    for (int s = 0; s != m_numberAlignments; ++s)
    {
	m_alignmentAngle[s]		= parameters.m_alignmentAngle[s];
	m_alignmentAngleConstraint[s]	= parameters.m_alignmentAngleConstraint[s];
	m_alignmentOffset[s]		= parameters.m_alignmentOffset[s];
	m_alignmentOffsetConstraint[s]	= parameters.m_alignmentOffsetConstraint[s];
    }
    for (int s = 0; s != m_numberScatterers; ++s)
    {
	m_scattererPhi[s]	= parameters.m_scattererPhi[s];
	m_scattererTheta[s]	= parameters.m_scattererTheta[s];
    }
    
    // restore difference history
    if (m_eigen)
    {	
	delete m_differences;
	if (parameters.m_differences)
	{
	    m_differences		= new Amg::VectorX(*parameters.m_differences);
	}
	else
	{
	    m_differences		= new Amg::VectorX(m_numberParameters);
	    m_differences->setZero();
	}
    }
    else
    {
	delete m_differencesAlSpaMat;
	if (parameters.m_differencesAlSpaMat)
	{
	    m_differencesAlSpaMat	= new AlVec(*parameters.m_differencesAlSpaMat);
	}
	else
	{
	    m_differencesAlSpaMat	= new AlVec(m_numberParameters);
	}
    }

    m_numberOscillations	= 0;
    m_oldDifference		= 0.;
}

const ScatteringAngles*
FitParameters::scatteringAngles (const FitMeasurement& fitMeasurement, int scatterer) const
{
    // scattering sigma used in chi2 computation
    double scattererSigmaTheta	= 1./fitMeasurement.weight();
    double scattererSigmaPhi	= scattererSigmaTheta /
				  fitMeasurement.intersection(FittedTrajectory).direction().perp();
    if (scatterer < 0)
    {
	return new ScatteringAngles(0.,
				    0.,
				    scattererSigmaPhi,
				    scattererSigmaTheta);
    }
    else
    {
	return new ScatteringAngles(m_scattererPhi[scatterer],
				    m_scattererTheta[scatterer],
				    scattererSigmaPhi,
				    scattererSigmaTheta);
    }
}
    
void
FitParameters::setPhiInstability (void)
{ m_phiInstability = true; }

Perigee*
FitParameters::startingPerigee (void) const
{
    // create momentum
    double		pT		= std::abs(m_sinTheta/m_qOverP);
    double		charge		= 1.;
    if (m_qOverP < 0.)	charge		= -1.;
    Amg::Vector3D	momentum(pT*m_cosPhi,pT*m_sinPhi,pT*m_cotTheta);
    
    return new Perigee(m_position,
		       momentum,
		       charge,
		       m_vertex);
}

const TrackParameters*
FitParameters::trackParameters (MsgStream&		log,
				const FitMeasurement&	measurement,
				bool			withCovariance) const
{
    // make checks necessary for the TrackParameters to be computed
    //   1) a Surface is required
    if (! measurement.surface())
    {
	log << MSG::WARNING << "FitParameters::trackParameters - measurement lacks Surface" << endmsg;
	return 0;
    }

    //   2) a SurfaceIntersection is required
    if (! measurement.hasIntersection(FittedTrajectory))
    {
	log << MSG::WARNING << "FitParameters::trackParameters - invalid measurement" << endmsg;
	return 0;
    }
	
    //   3) the intersection position has to lie sufficiently close to the Surface
    const TrackSurfaceIntersection&	intersection	= measurement.intersection(FittedTrajectory);
    Amg::Vector2D localPos;
    if (! measurement.surface()->globalToLocal(intersection.position(),
					       intersection.direction(),
					       localPos))
    {
	log << MSG::WARNING << "FitParameters::trackParameters - globalToLocal failure" << endmsg;
	return 0;
    }
	
    // cache parameters at EnergyDeposit
    if (measurement.isEnergyDeposit())
    {
	m_sinTheta1	= intersection.direction().perp();
	m_cosPhi1	= intersection.direction().x() / m_sinTheta1;
	m_sinPhi1	= intersection.direction().y() / m_sinTheta1;
	m_cosTheta1	= intersection.direction().z();
	m_qOverP1	= measurement.qOverP();
    }
    
    // propagate full covariance to form localCovariance
    AmgSymMatrix(5)* covMatrix	= 0;
    if (withCovariance
	&& (measurement.isDrift() || measurement.isCluster() || measurement.isPerigee()))
    {
	Amg::Vector3D direction	= intersection.direction();
	double sigma		= 1./measurement.weight();
	double sigma2		= 1./measurement.weight2();
	int lastParameter	= measurement.lastParameter();
	Amg::MatrixX jacobian	= Amg::MatrixX::Zero(5,lastParameter);
	for (int i = 0; i != lastParameter; ++i)
	{
	    jacobian(0,i) =  sigma*measurement.derivative(i);
	    jacobian(1,i) =  sigma2*measurement.derivative2(i);
	}

	// phi,theta derivs into jac
	jacobian(2,2)	=  1.;
 	jacobian(3,3)	=  1.;
	int i		= m_firstScatteringParameter;
	while(++i < lastParameter)
	{
	    jacobian(2,i) =  1.;
	    ++i;
	    jacobian(3,i) =  1.;
	}

	// only if fit to curvature
	if (m_fitMomentum && m_qOverP)
	{
	    double sinTheta		=  direction.perp();
	    if (m_fitEnergyDeposit && measurement.afterCalo())
	    {
		double deltaPhi		=  (direction.y()*m_cosPhi1 - direction.x()*m_sinPhi1)/sinTheta;
		double deltaTheta	=  (sinTheta*m_cosTheta1 - direction.z()*m_sinTheta1);
		jacobian(0,5)		*= Gaudi::Units::TeV;
		jacobian(1,5)		*= Gaudi::Units::TeV;
		jacobian(2,5)		=  deltaPhi/measurement.qOverP();
		jacobian(3,5)		=  deltaTheta/measurement.qOverP();
 		jacobian(4,5)		=  measurement.qOverP()/m_qOverP1;
	    }
	    else
	    {
		double deltaPhi		=  (direction.y()*m_cosPhi - direction.x()*m_sinPhi)/sinTheta;
		double deltaTheta	=  (sinTheta*m_cosTheta - direction.z()*m_sinTheta);
		jacobian(0,4)		*= Gaudi::Units::TeV;
		jacobian(1,4)		*= Gaudi::Units::TeV;
		jacobian(2,4)		=  deltaPhi/measurement.qOverP();
		jacobian(3,4)		=  deltaTheta/measurement.qOverP();
		jacobian(4,4)		=  measurement.qOverP()/m_qOverP;
	    }
	}
	else
	{
	    jacobian(4,4)		=  1.;
	}

	// similarity transform
	covMatrix	= new AmgSymMatrix(5) (jacobian *
					       m_fullCovariance->block(0,0,lastParameter,lastParameter) *
					       jacobian.transpose());
    }

    double phi				= intersection.direction().phi();
    double theta			= intersection.direction().theta();
    if (measurement.isFlipped())
    {
	if (phi > 0.)
	{
	    phi	-= M_PI;
	}
	else
	{
	    phi	+= M_PI;
	}
	theta	= M_PI - theta;
    }
	
    // finally can create the appropriate 'concrete' TrackParameters
    const TrackParameters* parameters	= 0;
    const StraightLineSurface* line	= dynamic_cast<const StraightLineSurface*>(measurement.surface());
    if (line)
    {
	parameters	= new AtaStraightLine(localPos[locR],
					      localPos[locZ],
					      phi,
					      theta,
					      measurement.qOverP(),
					      *line,
					      covMatrix);
	return parameters;
    }

    const PlaneSurface* plane = dynamic_cast<const PlaneSurface*>(measurement.surface());
    if (plane)
    {
	parameters	= new AtaPlane(localPos[locR],
				       localPos[locZ],
				       phi,
				       theta,
				       measurement.qOverP(),
				       *plane,
				       covMatrix);
	return parameters;
    }
    
    const CylinderSurface* cylinder = dynamic_cast<const CylinderSurface*>(measurement.surface());
    if (cylinder)
    {
	parameters	= new AtaCylinder(localPos[locR],
					  localPos[locZ],
					  phi,
					  theta,
					  measurement.qOverP(),
					  *cylinder,
					  covMatrix);
	return parameters;
    }

    const DiscSurface* disc = dynamic_cast<const DiscSurface*>(measurement.surface());
    if (disc)
    {
	parameters	= new AtaDisc(localPos[locR],
				      localPos[locZ],
				      phi,
				      theta,
				      measurement.qOverP(),
				      *disc,
				      covMatrix);
	return parameters;
    }
    
    const PerigeeSurface* peri	= dynamic_cast<const PerigeeSurface*>(measurement.surface());
    if (peri)
    {
	parameters	= new Perigee(localPos[locR],
				      localPos[locZ],
				      phi,
				      theta,
				      measurement.qOverP(),
				      *peri,
				      covMatrix);
	return parameters;
    }
    
    log << MSG::WARNING << "FitParameters::trackParameters - unrecognized surface" << endmsg;
    delete covMatrix;
    return 0;
}

void
FitParameters::update (const AlVec& differences)
{
    // this is the AlSpaMat version
    m_eigen		= false;
    
    // keep update values in case of cutStep procedure
    if (m_numberOscillations && m_oldDifference*differences[4] < 0.)
    {
	++m_numberOscillations;
    }
    else
    {
	m_numberOscillations = 1;
    }
    delete m_differencesAlSpaMat;
    m_differencesAlSpaMat 	= new AlVec(differences);
    m_oldDifference	= differences[4];

    // misalignment parameters
    std::vector<double>::iterator a = m_alignmentAngle.begin();
    std::vector<double>::iterator o = m_alignmentOffset.begin();
    int align = m_firstAlignmentParameter;
    for (int i = 0; i != m_numberAlignments; ++i)
    {
	(*a++) += differences[++align];
	(*o++) += differences[++align];
    }
    
    // scattering angles
    std::vector<double>::iterator p = m_scattererPhi.begin();
    std::vector<double>::iterator t = m_scattererTheta.begin();
    int scat = m_firstScatteringParameter;
    for (int i = 0; i != m_numberScatterers; ++i)
    {
	(*p++) += differences[++scat];
	(*t++) += differences[++scat];
    }
    
    // qOverP, cotTheta
    if (m_fitMomentum)		m_qOverP += differences[4]/Gaudi::Units::TeV;
    m_cotTheta	-= differences[3]/(m_sinTheta*m_sinTheta);
    
    // impose charge conservation and decreasing energy
    if (m_fitEnergyDeposit)
    {	
	m_qOverP1 += differences[5]/Gaudi::Units::TeV;
	double deposit = 1./std::abs(m_qOverP) - 1./std::abs(m_qOverP1);
	if (std::abs(deposit) < std::abs(m_minEnergyDeposit)
	    || deposit*m_minEnergyDeposit < 0.
	    || m_qOverP*m_qOverP1 < 0.)
	{
	    m_qOverP = 1./(1./std::abs(m_qOverP1) + m_minEnergyDeposit);
	    if (m_qOverP1 < 0.) m_qOverP = -m_qOverP;
	}
    }
    
    // protect phi against some rounding instabilities
    double sinDPhi	=  differences[2];
    double cosDPhi	=  0.;
    if (std::abs(sinDPhi) < 1.0)
    {
	cosDPhi	=  std::sqrt(1. - sinDPhi*sinDPhi);
    }
    else
    {
	if (sinDPhi > 0.)
	{
	    sinDPhi = 1.0;
	}
	else
	{
	    sinDPhi = -1.0;
	}
    }

    double cosPhi      	=  m_cosPhi*cosDPhi - m_sinPhi*sinDPhi;
    m_sinPhi		=  m_sinPhi*cosDPhi + m_cosPhi*sinDPhi;
    m_cosPhi		=  cosPhi;
    m_z0		+= differences[1];
    m_d0		+= differences[0];
    m_sinTheta		=  1./std::sqrt(1. + m_cotTheta*m_cotTheta);
    m_cosTheta		=  m_cotTheta*m_sinTheta;
    m_position		=  Amg::Vector3D(m_vertex.x() - m_d0*m_sinPhi,
					 m_vertex.y() + m_d0*m_cosPhi,
					 m_z0);
}

void
FitParameters::update (const Amg::VectorX& differences)
{
    // keep update values in case of cutStep procedure
    if (m_numberOscillations && m_oldDifference*differences(4) < 0.)
    {
	++m_numberOscillations;
    }
    else
    {
	m_numberOscillations = 1;
    }
    delete m_differences;
    m_differences 	= new Amg::VectorX(differences);
    m_oldDifference	= differences(4);

    // misalignment parameters
    std::vector<double>::iterator a = m_alignmentAngle.begin();
    std::vector<double>::iterator o = m_alignmentOffset.begin();
    int align = m_firstAlignmentParameter;
    for (int i = 0; i != m_numberAlignments; ++i)
    {
	(*a++) += differences(++align);
	(*o++) += differences(++align);
    }
    
    // scattering angles
    std::vector<double>::iterator p = m_scattererPhi.begin();
    std::vector<double>::iterator t = m_scattererTheta.begin();
    int scat = m_firstScatteringParameter;
    for (int i = 0; i != m_numberScatterers; ++i)
    {
	(*p++) += differences(++scat);
	(*t++) += differences(++scat);
    }
    
    // qOverP, cotTheta
    if (m_fitMomentum)		m_qOverP += differences(4)/Gaudi::Units::TeV;
    m_cotTheta	-= differences(3)/(m_sinTheta*m_sinTheta);
        
    // impose charge conservation and decreasing energy
    if (m_fitEnergyDeposit)
    {	
	m_qOverP1 += differences(5)/Gaudi::Units::TeV;
	double deposit = 1./std::abs(m_qOverP) - 1./std::abs(m_qOverP1);
	if (std::abs(deposit) < std::abs(m_minEnergyDeposit)
	    || deposit*m_minEnergyDeposit < 0.
	    || m_qOverP*m_qOverP1 < 0.)
	{
	    m_qOverP = 1./(1./std::abs(m_qOverP1) + m_minEnergyDeposit);
	    if (m_qOverP1 < 0.) m_qOverP = -m_qOverP;
	}
    }
    
    // protect phi against some rounding instabilities
    double sinDPhi	=  differences(2);
    double cosDPhi	=  0.;
    if (std::abs(sinDPhi) < 1.0)
    {
	cosDPhi	=  std::sqrt(1. - sinDPhi*sinDPhi);
    }
    else
    {
	if (sinDPhi > 0.)
	{
	    sinDPhi = 1.0;
	}
	else
	{
	    sinDPhi = -1.0;
	}
    }

    double cosPhi      	=  m_cosPhi*cosDPhi - m_sinPhi*sinDPhi;
    m_sinPhi		=  m_sinPhi*cosDPhi + m_cosPhi*sinDPhi;
    m_cosPhi		=  cosPhi;
    m_z0		+= differences(1);
    m_d0		+= differences(0);
    m_sinTheta		=  1./std::sqrt(1. + m_cotTheta*m_cotTheta);
    m_cosTheta		=  m_cotTheta*m_sinTheta;
    m_position		=  Amg::Vector3D(m_vertex.x() - m_d0*m_sinPhi,
					 m_vertex.y() + m_d0*m_cosPhi,
					 m_z0);
}
   
void
FitParameters::update (Amg::Vector3D		position,
		       Amg::Vector3D		direction,
		       double			qOverP,
		       const Amg::MatrixX&	leadingCovariance)
{
    // update parameters after leading material corrections
    m_position		= position;
    m_sinTheta		= direction.perp();
    double sinThetaInv	= 1./m_sinTheta;
    m_cotTheta		= sinThetaInv*m_cosTheta;
    m_cosPhi		= sinThetaInv*direction.x();
    m_sinPhi		= sinThetaInv*direction.y();
    m_cotTheta		= sinThetaInv*direction.z();
    m_cosTheta		= direction.z();
    m_qOverP		= qOverP;
    m_z0		= position.z();
    m_d0		= (m_vertex.x() - position.x())*m_sinPhi -
			  (m_vertex.y() - position.y())*m_cosPhi;

    // update covariance
    // (*m_finalCovariance) += leadingCovariance;	// ??
    for (int i = 0; i != 5; ++i)
    {
	for (int j = 0; j != 5; ++j)
	{
	    (*m_finalCovariance)(i,j) += leadingCovariance(i,j);
	}
    }
}

} // end of namespace
