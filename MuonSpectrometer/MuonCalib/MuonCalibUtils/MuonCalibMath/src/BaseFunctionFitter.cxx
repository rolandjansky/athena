/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// 05.04.2005, AUTHOR: OLIVER KORTNER
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

///////////////////////////////////////////////////////////////
// IMPLEMENTATION OF NON-INLINE METHODS DEFINED IN THE CLASS //
//                    BaseFunctionFitter                     //
///////////////////////////////////////////////////////////////

#include "MuonCalibMath/BaseFunctionFitter.h"
#include <Eigen/Dense>
#include "cmath"

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
		std::cerr << "\n"
			<< "Class BaseFunctionFitter, method fit_parameters: "
			<< "ERROR!\n"
			<< "Illegal first point " << first_point
			<< ", must be >=1 and <=" << sample_point.size()
			<< ".\n";
			exit(1);
	}
	if (last_point<first_point || last_point>sample_point.size()) {
		std::cerr << "\n"
			<< "Class BaseFunctionFitter, method fit_parameters: "
			<< "ERROR!\n"
			<< "Illegal last point " << last_point
			<< ", must be >=" << first_point
			<< " and <=" << sample_point.size()
			<< ".\n";
			exit(1);
 	}

////////////////////////////////////////////////////////////
// IF ALL CHECK ARE PASSED, FILL THE MINIMIZATION OBJECTS //
////////////////////////////////////////////////////////////

// clear the objects //
	init(m_nb_coefficients);

// fill the objects //
// 	for (unsigned int k=first_point-1; k<last_point; k++) {
// 
//  		for (int j=0; j<m_nb_coefficients; j++) {
// 		for (int p=j; p<m_nb_coefficients; p++) {
// 
//  			m_A[j][p] = m_A[j][p]+base_function->value(j, 
//         					   sample_point[k].x1())
// 					*base_function->value(p, 
// 							sample_point[k].x1())/
// 					std::pow(sample_point[k].error(), 2);
// 
// 		}
// 
// 		m_b[j] = m_b[j]+sample_point[k].x2()*base_function->value(j, 
// 					sample_point[k].x1())/
// 					std::pow(sample_point[k].error(), 2);
// 
// 		}
// 
// 	}

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
