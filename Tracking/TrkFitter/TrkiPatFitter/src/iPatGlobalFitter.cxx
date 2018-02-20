/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//////////////////////////////////////////////////////////////////
// iPatGlobalFitter.cxx
//   access full derivative and covariance matrices 
//
// (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

//<<<<<< INCLUDES                                                       >>>>>>

#include "GaudiKernel/SystemOfUnits.h"
#include "TrkiPatFitterUtils/ExtrapolationType.h"
#include "TrkiPatFitterUtils/FitMeasurement.h"
#include "TrkiPatFitterUtils/FitParameters.h"
#include "TrkiPatFitterUtils/FitProcedure.h"
#include "TrkiPatFitter/iPatGlobalFitter.h"

namespace Trk
{ 

//<<<<<< CLASS STRUCTURE INITIALIZATION                                 >>>>>>

iPatGlobalFitter::iPatGlobalFitter (const std::string&	type, 
				    const std::string&	name,
				    const IInterface*	parent)
    :	iPatFitter			(type, name, parent),
	m_allParameters			(false),
	m_derivativeMatrix		(0)
{
    declareInterface<IGlobalTrackFitter>(this);
    declareProperty("AllParameters",	m_allParameters);
    m_globalFit	= true;
}

iPatGlobalFitter::~iPatGlobalFitter (void)
{
    delete m_derivativeMatrix;
}

//<<<<<< PUBLIC MEMBER FUNCTION DEFINITIONS                             >>>>>>

Amg::MatrixX*
iPatGlobalFitter::DerivMatrix() const
{
    // copy derivatives to a new HepMatrix
    delete m_derivativeMatrix;
    m_derivativeMatrix		= 0;
    if (! m_measurements || ! m_parameters)	return 0;
    
    int numberParameters	= 5;
    if (m_allParameters) numberParameters = m_parameters->numberParameters();
    int rows	= 0;
    for (std::vector<FitMeasurement*>::iterator m = m_measurements->begin();
         m != m_measurements->end();
	 ++m)
    {
	if (! (**m).isPositionMeasurement())	continue;
	rows += (**m).numberDoF();
    }
    
    if (! numberParameters || ! rows)		return 0;
    
    ATH_MSG_VERBOSE ( " DerivMatrix : " << m_measurements->size() << " measurement objects giving "
		      << rows << " rows and " << numberParameters << " columns (parameters)" );
    
    m_derivativeMatrix = new Amg::MatrixX(rows,numberParameters);
    int row	= 0;
    for (std::vector<FitMeasurement*>::iterator m = m_measurements->begin();
         m != m_measurements->end();
	 ++m)
    {
	if (! (**m).numberDoF() || ! (**m).isPositionMeasurement())	continue;
	double norm	= 0.;
	if ((**m).weight() > 0.) norm = 1./(**m).weight();
	
	for (int col = 0; col < numberParameters; ++col)
	{
	    (*m_derivativeMatrix)(row,col) = norm*(**m).derivative(col);
	}

	// take care of units for momentum derivs
	(*m_derivativeMatrix)(row,4) *= Gaudi::Units::TeV;
	if (m_parameters->fitEnergyDeposit()) (*m_derivativeMatrix)(row,5) *= Gaudi::Units::TeV;
	++row;
	if ((**m).numberDoF() < 2)	continue;

	// pixel measurements
	norm	= 0.;
	if ((**m).weight2() > 0.) norm = 1./(**m).weight2();
	for (int col = 0; col < numberParameters; ++col)
	{
	    (*m_derivativeMatrix)(row,col) = norm*(**m).derivative2(col);
	}
	(*m_derivativeMatrix)(row,4) *= Gaudi::Units::TeV;
	if (m_parameters->fitEnergyDeposit()) (*m_derivativeMatrix)(row,5) *= Gaudi::Units::TeV;
	++row;
    }

    if (row != rows) ATH_MSG_WARNING( "iPatGlobalFitter: inconsistent #rows in deriv matrix " );

    return m_derivativeMatrix;
}
    
Amg::MatrixX*
iPatGlobalFitter::FullCovarianceMatrix() const
{
    int numberParameters		= 5;
    if (m_allParameters) numberParameters = m_parameters->numberParameters();
    ATH_MSG_VERBOSE ( " FullCovarianceMatrix for " << numberParameters << " parameters" );

    return new Amg::MatrixX(m_fitProcedure->fullCovariance()->block(0,0,numberParameters,numberParameters));
}

int
iPatGlobalFitter::iterationsOfLastFit() const
{ return m_iterations; }

//<<<<<< PRIVATE MEMBER FUNCTION DEFINITIONS                            >>>>>>
  
void
iPatGlobalFitter::setMinIterations(int minIterations)
{
    m_fitProcedure->setMinIterations(minIterations);
}
  
} // end of namespace
