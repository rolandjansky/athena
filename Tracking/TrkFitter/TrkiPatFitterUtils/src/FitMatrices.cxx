/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//////////////////////////////////////////////////////////////////////////////
// class FitMatrices
//  Storage and manipulation of matrices during track fitting
//  (note the actual matrices are structs (FitMatrix.h) to give faster execution)
//
//  Given matrix of measurement derivatives wrt fit parameters (D)
//  and vector of differences between measurements and fitted trajectory,
//  solve for:
//   parameter weight matrix = (covariance)-1 = DT.D
//   parameter change        = (DT.D)-1 * (DT.differ)
//
//  NOTE:
//  covariances, derivatives etc, use d0, z0, phi, theta, qOverP as parameters
//  distinguish: full covariance with all parameters
//               (incl scattering, energy loss and misalignment uncertainties)
//               5*5 final covariance with external contribution representing
//               leading material and field gradient effects
//
//  (c) ATLAS tracking software
//////////////////////////////////////////////////////////////////////////////

//<<<<<< INCLUDES                                                       >>>>>>

#include <iomanip>
#include <iostream>
#include "GaudiKernel/SystemOfUnits.h"
#include "TrkAlgebraUtils/AlSpaMat.h"
#include "TrkExUtils/TrackSurfaceIntersection.h"
#include "TrkiPatFitterUtils/FitMatrices.h"
#include "TrkiPatFitterUtils/FitMatrix.h"
#include "TrkiPatFitterUtils/FitMeasurement.h"
#include "TrkiPatFitterUtils/FitParameters.h"

//<<<<<< CLASS STRUCTURE INITIALIZATION                                 >>>>>>

namespace Trk{
    
FitMatrices::FitMatrices(void)
    :	m_columns			(mxparam),	// reserve at maximum size
	m_covariance			(0),
	m_finalCovariance		(0),
	m_largePhiWeight		(10000.),	// arbitrary - equiv to 10um
	m_matrixFromCLHEP		(false),
	m_measurements			(0),
	m_numberDoF			(0),
	m_numberDriftCircles		(0),
	m_numberPerigee			(5),
	m_parameters			(0),
	m_perigee			(0),
	m_perigeeDifference		(Amg::MatrixX(1,m_numberPerigee)),
	m_perigeeWeight			(0),
	m_residuals			(0),
	m_rows				(0),
	m_usePerigee			(false),
	m_weight			(0),
	// m_weightCLHEP		(0),
	m_weightedDifference		(0)
	// m_weightedDifferenceCLHEP	(0)
{
    if (m_matrixFromCLHEP)
    {
	// m_weightCLHEP		= new CLHEP::HepSymMatrix(mxparam);
	// m_weightedDifferenceCLHEP	= new CLHEP::HepVector(mxparam);
    }
    else
    {
	m_weight			= new AlSpaMat(mxparam);
	m_weightedDifference		= new AlVec(mxparam);
    }
}

FitMatrices::~FitMatrices(void)
{
    delete m_covariance;
    delete m_finalCovariance;
    delete m_residuals;
    delete m_weight;
    // delete m_weightCLHEP;
    delete m_weightedDifference;
    // delete m_weightedDifferenceCLHEP;
}

//<<<<<< PUBLIC MEMBER FUNCTION DEFINITIONS                             >>>>>>
	
double
FitMatrices::chiSquaredChange (void) const
{
    // not applicable when matrix has been inverted already
    if (! m_numberDoF || ! m_weight || m_covariance) return 0.;
    AlVec diffAl	= m_parameters->differences();
    Amg::VectorX diff(m_columns);
    for (int i = 0; i != m_columns; ++i) diff[i] = diffAl[i];
    if (m_matrixFromCLHEP)
    {
	// return m_weightCLHEP->similarity(diff)/static_cast<double>(m_numberDoF);
	return 0;
    }
    else
    {
	Amg::MatrixX weight(m_columns,m_columns);
	for (int i = 0; i != m_columns; ++i)
	{
	    for (int j = 0; j != m_columns; ++j) weight(i,j) = (*m_weight)[j][i];
	}
	return (diff * weight * diff.transpose())(0,0) / static_cast<double>(m_numberDoF);
    }
}

const Amg::MatrixX*
FitMatrices::fullCovariance (void)
{
    // return result if matrix already inverted
    if (m_covariance)	return m_covariance;
    m_covariance	= new Amg::MatrixX(m_columns,m_columns);

    // fix weighting
    if (m_parameters->phiInstability()) solveEquations();

    // invert weight matrix
    Amg::MatrixX& covariance    = *m_covariance;
    int failure                 = 0;

    // CLHEP method
    if (m_matrixFromCLHEP)
    {
        // for (int i = 0; i != m_columns; ++i)
        // {
        //     for (int j = 0; j <= i; ++j)     covariance[i][j] = (*m_weightCLHEP)[i][j];
        // }
        // covariance.invert(failure);
    }
    else
    {
        // avoid singularity through ill-defined momentum
        avoidMomentumSingularity();

        // copy to AlSymMat for inversion
        AlSymMat weight(m_columns);
        for (int i = 0; i != m_columns; ++i)
        {
            for (int j = 0; j <= i; ++j)		weight[i][j] = (*m_weight)[i][j];
        }
        failure = weight.invert();
        for (int i = 0; i != m_columns; ++i)
        {
            for (int j = 0; j != m_columns; ++j)	covariance(j,i) = weight[i][j];
        }
    }   

    // trap singular matrix
    if (failure)
    {
        delete m_covariance;
        m_covariance    = 0;
        return 0;
    }

    // back convert curved fits to Tracking units (MeV)
    if (m_parameters->fitMomentum())
    {
        // transform to MeV
        double d4               =  1./Gaudi::Units::TeV;
        for (int row = 0; row < m_columns; ++row)
	{
	    covariance(4,row)	*= d4;
	    covariance(row,4)	=  covariance(4,row);
	}
        covariance(4,4)		*= d4;

        // transform units for fitted energy deposit (for now fit qOverP at calo exit) 
        if (m_parameters->fitEnergyDeposit())
        {
            double d5           =  1./Gaudi::Units::TeV;
            for (int row = 0; row < m_columns; ++row)
	    {
		covariance(5,row)	*= d5;
		covariance(row,5)	=  covariance(5,row);
	    }
	    covariance(5,5)	*= d5;
        }
    }

    // FIXME: errors underestimated on d0,z0 when large scatterer precedes precise measurements
    // final covariance starts with 5*5 representing perigee from full covariance
    delete m_finalCovariance;
    m_finalCovariance	= new Amg::MatrixX(5,5);  // AmgSymMatrix(5);
    for (int i = 0; i != 5; ++i)
    {
	for (int j = 0; j != 5; ++j)
	{
	    (*m_finalCovariance)(i,j) = covariance(i,j);
	}
    }
 
    // return pointer to full covariance
    return m_covariance;
}

double
FitMatrices::perigeeChiSquared (void)
{
    m_perigeeDifference = m_parameters->parameterDifference(*m_perigee);
    return (m_perigeeDifference * (*m_perigeeWeight) * m_perigeeDifference.transpose())(0,0);
} 

void
FitMatrices::printDerivativeMatrix (void)
{
    std::cout << "DerivativeMatrix:  rows * columns " << m_rows << " * "
	      << m_columns << "  numberDoF " << m_numberDoF << std::endl;

    int firstCol	= 0;
    int lastCol		= 0;
    if (! m_measurements)		return;
    std::list<FitMeasurement*>::iterator m = m_measurements->begin();
    std::vector<FitMeasurement*> alignmentfm;
    FitMeasurement* fm	= *m;
    bool singleRow	= true;
    
    for (int row = 0; row < m_rows; ++row)
    {
	// get corresponding FitMeasurement
	if (singleRow)
	{
	    while (m !=  m_measurements->end() && (! (**m).numberDoF() || (**m).isAlignment()))
	    {
		if ((**m).isAlignment())  alignmentfm.push_back(*m);
		++m;
	    }
	    if (m !=  m_measurements->end())
	    {
		fm	= *m;
	    }
	    else
	    {
		fm	= alignmentfm.back();
		alignmentfm.pop_back();
	    }

	    firstCol	= fm->firstParameter();
	    lastCol	= fm->lastParameter() - 1;
	    std::cout << std::endl << std::setiosflags(std::ios::fixed);
	    if (fm->isPositionMeasurement())
	    {
		std::cout << "measurement";
	    }
	    else if (fm->isScatterer())
	    {
		std::cout << "scatterer  ";
	    }
	    else if (fm->isEnergyDeposit())
	    {
		std::cout << "energyDepos";
	    }
	    else if (fm->isAlignment())
	    {
		std::cout << "alignment  ";
	    }
	    else
	    {
		std::cout << "   ??      ";
	    }
	    if (fm->is2Dimensional())
	    {
		singleRow = false;
	    }
	    else
	    {
		if (m !=  m_measurements->end()) ++m;
	    }
	    std::cout << " row " << std::setw(3) << row << " col  0     ";
	}
	else
	{
	    if (m !=  m_measurements->end()) ++m;
	    singleRow = true;
	    std::cout << std::endl  << std::setiosflags(std::ios::fixed)
		      << "            row " << std::setw(3) << row << " col  0     ";
	}
	for (int col = 0; col < m_columns; ++col)
	{
	    if (col < firstCol || col > lastCol)	// m_firstRowForParameter[row])
	    {
	    	if (fitMatrix.derivative[row][col] == 0.)
	    	{
	    	    std::cout << "            ";
	    	}
	    	else
	    	{
	    	    // flag out-of-order
	    	    std::cout << std::setiosflags(std::ios::scientific) << std::setbase(10)
	    		      << "<" <<std::setw(10) << fitMatrix.derivative[row][col] << ">";
	    	}
	    }
	    else
	    {
		std::cout << std::setiosflags(std::ios::scientific) << std::setbase(10)
			  << std::setw(10) << fitMatrix.derivative[row][col] << "  ";
	    }
	    
	    if ((col+1)%12 == 0 && col+1 < m_columns)
		std::cout << std::endl << std::setiosflags(std::ios::fixed)
			  << "                    col " << std::setw(3) << col+1 << "    ";
	}
    }
    std::cout << std::endl;
}

void
FitMatrices::printWeightMatrix (void)
{
    std::cout << std::endl << "WeightMatrix:  symmetric with rank " << m_columns;

    for (int row = 0; row < m_columns; ++row)
    {
	std::cout << std::endl << std::setiosflags(std::ios::fixed)
		  << " row " << std::setw(3) << row << " col  0     ";
	for (int col = 0; col <= row; ++col)
	{
	    std::cout << std::setiosflags(std::ios::scientific) << std::setbase(10)
		      << std::setw(10) << (*m_weight)[row][col] << "  ";
	    
	    if ((col+1)%13 == 0 && col < row)
		std::cout << std::endl << std::setiosflags(std::ios::fixed)
			  << "         col " << std::setw(3) << col+1 << "    ";
	}
    }
    std::cout << std::endl;	
}
    
int
FitMatrices::setDimensions (std::list<FitMeasurement*>&	measurements,
			    FitParameters*		parameters)
{
    // keep pointer for debug purposes
    m_measurements		= &measurements;
    
    // only use perigee on request (from special fit types)
    m_usePerigee		= false;

    // count rows, misalignments and scatterers from loop over FitMeasurements
    m_firstRowForParameter.clear();
    m_firstRowForParameter.reserve(40);
    m_parameters		= parameters;
    delete m_residuals;
    m_residuals			= new std::vector<double>(2*measurements.size(), 0.);
    m_numberDriftCircles	= 0;
    bool haveMeasurement	= false;
    int numberAlignments	= 0;
    int numberParameters	= 5;
    int numberScatterers	= 0;
    int row			= 0;
    for (std::list<FitMeasurement*>::iterator m = measurements.begin();
         m != measurements.end();
	 ++m)
    {
	// alignment rows come after scattering
	if ((**m).isAlignment())	continue;
	
	// identify leading material
	if (! haveMeasurement)
	{
	    if ((**m).isScatterer())
	    {
		(**m).numberDoF(0);
	    }
	    else if ((**m).isPositionMeasurement())
	    {
		haveMeasurement = true;
	    }
	}

	// only allocate rows to fitted measurements (DoF > 0)
	if (! (**m).numberDoF()) continue;
	if ((**m).isDrift()) ++m_numberDriftCircles;

	// fit energyDeposit unless momentum fixed or near infinite
	if ((**m).isEnergyDeposit())
	{
	    if (! m_parameters->fitMomentum() || m_parameters->extremeMomentum())
	    {
		(**m).numberDoF(0);
		continue;
	    }
	    else
	    {
		++numberParameters;
		m_parameters->fitEnergyDeposit((**m).minEnergyDeposit());
	    }
	}
	if ((**m).isScatterer()) ++numberScatterers;
	
	// set pointers into big matrix
	(**m).derivative(&fitMatrix.derivative[row][0]);
	(**m).residual(row+m_residuals->begin());
	++row;
	if ((**m).is2Dimensional())
	{
	    (**m).derivative2(&fitMatrix.derivative[row][0]);
	    ++row;
	}
    }

    // second loop puts alignment rows at bottom of matrix
    for (std::list<FitMeasurement*>::iterator m = measurements.begin();
         m != measurements.end();
	 ++m)
    {
	if (! (**m).isAlignment())	continue;
	++numberAlignments;
	
	// set pointers into big matrix
	(**m).derivative(&fitMatrix.derivative[row][0]);
	(**m).residual(row+m_residuals->begin());
	++row;
	if ((**m).is2Dimensional())
	{
	    (**m).derivative2(&fitMatrix.derivative[row][0]);
	    ++row;
	}
    }

    // keep first row with measurements up to each number of parameters
    if (measurements.front()->isVertex())
    {
	m_firstRowForParameter = std::vector<int>(numberParameters,1);
	m_firstRowForParameter[0] = 0;
    }
    else
    {
	m_firstRowForParameter = std::vector<int>(numberParameters,0);
    }

    parameters->numberAlignments(numberAlignments);
    parameters->numberScatterers(numberScatterers);
    bool afterCalo	= false;
    m_rows		= 0;
    for (std::list<FitMeasurement*>::iterator m = measurements.begin();
         m != measurements.end();
	 ++m)
    {
	if ((**m).numberDoF())
	{
	    if ((**m).isEnergyDeposit())
	    {
		afterCalo = true;
	    }
	    else if ((**m).isScatterer())
	    {
		m_firstRowForParameter.push_back(m_rows);
		m_firstRowForParameter.push_back(m_rows);
		parameters->addScatterer((**m).scattererPhi(),(**m).scattererTheta());
	    }
	    else if ((**m).isAlignment())
	    {
		// defer alignment
		continue;
	    }
	    m_rows += (**m).numberDoF();
	}
	(**m).lastParameter(m_firstRowForParameter.size(), afterCalo);
    }

    // second loop puts any alignment info into the final rows
    numberParameters		= m_firstRowForParameter.size();
    for (std::list<FitMeasurement*>::iterator m = measurements.begin();
         m != measurements.end();
	 ++m)
    {
	if ((**m).isEnergyDeposit())	afterCalo = true;
	if (! (**m).isAlignment() || ! (**m).numberDoF())	continue;
	parameters->addAlignment((**m).alignmentAngle(),(**m).alignmentOffset());
	m_firstRowForParameter.push_back(m_rows);
	m_firstRowForParameter.push_back(m_rows);
	(**m).firstParameter(numberParameters);
	numberParameters	+= 2;
	int alignmentParameter	=  parameters->numberAlignments();
	(**m).alignmentParameter(alignmentParameter);
	(**m).lastParameter(numberParameters,afterCalo);
	m_rows			+=  (**m).numberDoF();
    }

    // initialize number of parameters (including alignments and scatterers)
    numberParameters		= m_firstRowForParameter.size();
    parameters->numberParameters(numberParameters);

    // and degrees of freedom
    m_numberDoF	= m_rows - numberParameters;

    // make some checks: return fitCode in case of problem
    int fitCode	= 0;
    //if (row > mxmeas)   		fitCode = 2;	// too many measurements for fit matrix size
    if (2*measurements.size() > mxmeas)	fitCode = 2;	// too many measurements for fit matrix size
    if (numberParameters > mxparam)	fitCode = 3;	// too many parameters for fit matrix size
    if (m_numberDoF < 0)		fitCode = 4;	// unconstrained fit: negative numberDoF
    if (fitCode) return fitCode;
    
    // reserve derivatives for jacobian propagation to 'non-measurements'
    row	= m_rows;
    for (std::list<FitMeasurement*>::iterator m = measurements.begin();
         m != measurements.end();
	 ++m)
    {
	if (! (**m).isPositionMeasurement() || (**m).numberDoF() > 1) continue;
	if (! (**m).numberDoF())
	{
	    for (int i = 0; i != numberParameters; ++i) fitMatrix.derivative[row][i] = 0.;
	    (**m).derivative(&fitMatrix.derivative[row][0]);
	    (**m).residual(row+m_residuals->begin());
	    ++row;
	}
	for (int i = 0; i != numberParameters; ++i) fitMatrix.derivative[row][i] = 0.;
	(**m).derivative2(&fitMatrix.derivative[row][0]);
	++row;
    }

    // update partitioning of fit matrices
    for (int row = 0; row < m_rows; ++row)
    {
	for (int param = 0; param < numberParameters; ++param)
	{
	    fitMatrix.derivative[row][param] = 0.;
	}
    }

    // fix degrees of freedom for external customers
    if (! m_parameters->fitMomentum()) ++m_numberDoF;
    
    // we don't have any fit results yet
    delete m_covariance;
    m_covariance	= 0;
    delete m_finalCovariance;
    m_finalCovariance	= 0;
    
    // reallocate to get correct matrix sizes 
    if (numberParameters != m_columns)
    {
	m_columns			= numberParameters;
	if (m_matrixFromCLHEP)
	{
	    // delete m_weightCLHEP;
	    // m_weightCLHEP		= new CLHEP::HepSymMatrix(m_columns);
	    // delete m_weightedDifferenceCLHEP;
	    // m_weightedDifferenceCLHEP	= new CLHEP::HepVector(m_columns);
	}
	else
	{
	    delete m_weight;
	    m_weight			= new AlSpaMat(m_columns);
	    delete m_weightedDifference;
	    m_weightedDifference	= new AlVec(m_columns);
	}
    }
    
    return fitCode;
}

bool
FitMatrices::solveEquations(void)
{
    // using CLHEP solve method
    if (m_matrixFromCLHEP)
    {
	return solveEquationsCLHEP();
    }
    else
    {
	return solveEquationsAlMat();
    }
}

void
FitMatrices::usePerigee (const FitMeasurement& measurement)
{
    m_perigee		= measurement.perigee();
    m_perigeeWeight	= measurement.perigeeWeight();
    // TODO: needs AlMat equiv !!
    if (m_matrixFromCLHEP)
    {
	m_usePerigee	= true;
    }
}


//<<<<<< PRIVATE MEMBER FUNCTION DEFINITIONS                            >>>>>>

void
FitMatrices::addPerigeeMeasurement (void)
{
    // TODO mig5
    // TODO: needs AlMat equiv !!
    // const Amg::MatrixX&	perigeeWeight		= *m_perigeeWeight;
    // Amg::MatrixX&	weight			= *m_weightCLHEP;
    // AmgVectorX&    	weightedDifference	= *m_weightedDifferenceCLHEP;
    // AmgVectorX		diff_vector		= perigeeWeight*m_perigeeDifference.T();
    // for (int row = 0; row < m_numberPerigee; ++row)
    // {
    // 	weightedDifference[row] += diff_vector[row];
    // 	for (int col = 0; col <= row; ++col) weight[row][col] += perigeeWeight[row][col];
    // }
}

void
FitMatrices::avoidMomentumSingularity(void)
{
    // fix momentum if line-fit or fit attempted with negligible field integral
    AlSpaMat& weight		= *m_weight;
    if (m_parameters->fitEnergyDeposit() && weight[5][5] < 1./Gaudi::Units::TeV)
    {
	for (int i = 0; i != m_columns; ++i)
	{
	    weight[i][5]	=  0.;
	    weight[5][i]	=  0.;
	}
	weight[5][5]	+= 1./Gaudi::Units::TeV;
    }
    if (! m_parameters->fitMomentum() || weight[4][4] < 1./Gaudi::Units::TeV)
    {  
	m_parameters->fitMomentum(false);
	for (int i = 0; i != m_columns; ++i)
	{
	    weight[i][4]	=  0.;
	    weight[4][i]	=  0.;
	}
	weight[4][4]	+= 1./Gaudi::Units::TeV;
    }
}

bool
FitMatrices::solveEquationsAlMat(void)
{
    // otherwise using alignment matrix package
    //  Note: multiplication using for loops is much faster than CLHEP ...
    //        and fastest of all using an array from a struct !
    AlSpaMat& weight		= *m_weight;
    AlVec& weightedDifference	= *m_weightedDifference;
    for (int row = 0; row < m_columns; ++row)
    {
	for (int col = 0; col <= row; ++col)
	{
	    double element = 0.;
	    // FIXME:  disable smart pointers during alignment development
	    // for (int i = m_firstRowForParameter[row]; i < m_rows; ++i)
	    for (int i = 0; i < m_rows; ++i)	
		element += fitMatrix.derivative[i][row] * fitMatrix.derivative[i][col];
	    weight[row][col] = element;
	}
    }
    
    for (int row = 0; row < m_columns; ++row)
    {
	double element = 0.;
	// FIXME:  disable smart pointers during alignment development
	// for (int i = m_firstRowForParameter[row]; i < m_rows; ++i)
	for (int i = 0; i < m_rows; ++i)
	    element += (*m_residuals)[i] * fitMatrix.derivative[i][row];
	weightedDifference[row] = element;
    }

    if (m_parameters->phiInstability()) weight[0][0] += m_largePhiWeight;

    // avoid some possible singularities in matrix inversion
    avoidMomentumSingularity();
    
    // solve is faster than inverse: wait for explicit request for covariance before inversion
    // checked with O(50) param   (SA OK with invert; ***REMOVED*** much faster for CB)
    // trap singular matrix
    int failure		= weight.***REMOVED***Solve(weightedDifference);
    if (failure)
    {
	return false;
    }
    else
    {
	m_parameters->update(weightedDifference);
	return true;
    }
}

bool
FitMatrices::solveEquationsCLHEP(void)
{
    // TODO mig5
//     // fill matrices
//     //  Note: multiplication using for loops is much faster than CLHEP ...
//     //        and fastest of all using an array from a struct !
//     CLHEP::HepSymMatrix& weight			= *m_weightCLHEP;
//     CLHEP::HepVector& weightedDifference	= *m_weightedDifferenceCLHEP;
//     for (int row = 0; row < m_columns; ++row)
//     {
// 	for (int col = 0; col <= row; ++col)
// 	{
// 	    double element = 0.;
// 	    for (int i = m_firstRowForParameter[row]; i < m_rows; ++i)
// 		element += fitMatrix.derivative[i][row] * fitMatrix.derivative[i][col];
// 	    weight[row][col] = element;
// 	}
//     }
    
//     for (int row = 0; row < m_columns; ++row)
//     {
// 	double element = 0.;
// 	for (int i = m_firstRowForParameter[row]; i < m_rows; ++i)
// 	    element += (*m_residuals)[i] * fitMatrix.derivative[i][row];
// 	weightedDifference[row] = element;
//     }
    
//     if (m_usePerigee)
//     {
// 	addPerigeeMeasurement();

// // 	std::cout << " perigee difference " << std::setiosflags(std::ios::fixed)
// // 		  << std::setw(10) << std::setprecision(4) << m_perigeeDifference[0][0]
// // 		  << std::setw(10) << std::setprecision(4) << m_perigeeDifference[0][1]
// // 		  << std::setw(10) << std::setprecision(6) << m_perigeeDifference[0][2]
// // 		  << std::setw(10) << std::setprecision(6) << m_perigeeDifference[0][3]
// // 		  << std::setw(13) << std::setprecision(9) << m_perigeeDifference[0][4]
// // 		  << std::endl;
	
//     }

//     if (m_parameters->phiInstability()) weight[0][0] += m_largePhiWeight;
    
//     // fix momentum if fit attempted with negligible field integral
//     if (! m_parameters->fitMomentum() || weight[4][4] < 1./Gaudi::Units::TeV)
//     {
// 	m_parameters->fitMomentum(false);
// 	weight[4][4]		+= 1./Gaudi::Units::TeV;
//     }

//     // solve is faster than inverse: wait for explicit request for covariance before inversion
//     CLHEP::HepVector diff	=  solve(weight,weightedDifference);

//     // copy to alignment vector for parameter update
//     AlVec diffAl(m_columns);
//     for (int i = 0; i != m_columns; ++i) diffAl[i] = diff[i];
//     m_parameters->update(diffAl);
    return true;
}

}	// end of namespace
