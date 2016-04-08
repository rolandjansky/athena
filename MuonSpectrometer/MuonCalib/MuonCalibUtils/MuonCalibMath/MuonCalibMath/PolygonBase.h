/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// 29.06.2006, AUTHOR: OLIVER KORTNER
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

#ifndef PolygonBaseH
#define PolygonBaseH

//:::::::::::::::::::::::
//:: CLASS PolygonBase ::
//:::::::::::::::::::::::

namespace MuonCalib {
/// \class PolygonBase
/// This class provides functions which allow the user to express a polygon as
/// a linear combination of these functions.
///
/// \author Oliver.Kortner@CERN.CH
///
/// \date 29.06.2006
}

//::::::::::::::::::
//:: HEADER FILES ::
//::::::::::::::::::

// standard C++ //
#include "math.h"

// STL //
#include <vector>

// MuonCalib //
#include "MuonCalibMath/BaseFunction.h"


namespace MuonCalib {

class PolygonBase : public BaseFunction {

public:
// Constructor //
	PolygonBase(const std::vector<double> & x) {
		init(x);
		}
	///< Constructor: the vector x contains the abscissae of the
	///< base points of the polygon. x must contain the abscissae in
	///< increasing order.

// Method //
	double value(const int & k, const double & x) const;
	                                ///< get the value of the k-th base 
					///< function in x

private:
	std::vector<double> m_x; // vector containing the abscissae of the base
	                         // points of the polygon
	void init(const std::vector<double> & x);
	                         // initialization method,
	                         // x contains the abscissae of the base points
	                         // of the polygon.

};

}

#endif
