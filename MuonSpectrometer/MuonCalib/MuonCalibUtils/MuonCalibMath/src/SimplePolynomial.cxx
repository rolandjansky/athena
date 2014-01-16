/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// 28.06.2007, AUTHOR: OLIVER KORTNER
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
//:: IMPLEMENTATIONS OF THE METHODS DEFINED IN THE CLASS SimplePolynomial ::
//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

//::::::::::::::::::
//:: HEADER FILES ::
//::::::::::::::::::

#include "cmath"
#include "MuonCalibMath/SimplePolynomial.h"

//::::::::::::::::::::::::
//:: NAMESPACE SETTINGS ::
//::::::::::::::::::::::::

using namespace MuonCalib;

//*****************************************************************************

//::::::::::::::::::
//:: METHOD value ::
//::::::::::::::::::

double SimplePolynomial::value(const int & k, const double & x) const {

	return std::pow(x, k);

}
