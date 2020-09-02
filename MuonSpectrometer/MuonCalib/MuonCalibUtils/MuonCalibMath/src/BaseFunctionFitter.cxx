/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#include "MuonCalibMath/BaseFunctionFitter.h"
#include <Eigen/Dense>

#include <TString.h> // for Form
#include <cmath>

namespace MuonCalib {

//*****************************************************************************

///////////////////////////
// METHOD fit_parameters //
///////////////////////////

bool BaseFunctionFitter::fit_parameters(
        		       const std::vector<SamplePoint> & sample_point,
        		       const unsigned int & first_point,
        		       const unsigned int & last_point,
        		       BaseFunction * base_function) {

////////////
// CHECKS //
////////////

	if (first_point<1 || first_point>sample_point.size()) {
		throw std::runtime_error(Form("File: %s, Line: %d\nBaseFunctionFitter::fit_parameters() - ERROR: Illegal first point %d, must be >=1 and <=%lu", __FILE__, __LINE__, first_point, sample_point.size()));
	}
	if (last_point<first_point || last_point>sample_point.size()) {
		throw std::runtime_error(Form("File: %s, Line: %d\nBaseFunctionFitter::fit_parameters() - ERROR: Illegal last point %d, must be >=%d and <=%lu", __FILE__, __LINE__, last_point, first_point, sample_point.size()));
 	}

////////////////////////////////////////////////////////////
// IF ALL CHECK ARE PASSED, FILL THE MINIMIZATION OBJECTS //
////////////////////////////////////////////////////////////

// clear the objects //
	init(m_nb_coefficients);

 	for (int j=0; j<m_nb_coefficients; j++) {
		for (int p=j; p<m_nb_coefficients; p++) {

		  for (unsigned int k=first_point-1; k<last_point; k++) {
		    m_A.fillSymmetric(j,p,m_A(j,p)+base_function->value(j,sample_point[k].x1())
				      *base_function->value(p,sample_point[k].x1())/
				      std::pow(sample_point[k].error(), 2));

		  }

		}

		for (unsigned int k=first_point-1; k<last_point; k++) {
 			m_b[j] = m_b[j]+sample_point[k].x2()*
					base_function->value(j, 
					sample_point[k].x1())/
					std::pow(sample_point[k].error(), 2);
		}

	}

// perform the minimization //
	Amg::MatrixX aInv = m_A.inverse();
	m_alpha = aInv*m_b;

	return false;

}

}
