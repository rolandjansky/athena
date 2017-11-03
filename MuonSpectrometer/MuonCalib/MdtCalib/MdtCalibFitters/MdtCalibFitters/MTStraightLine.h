/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// 01.03.2006, AUTHORS: OLIVER KORTNER, FELIX RAUSCHER
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

//::::::::::::::::::::::::::
//:: CLASS MtStraightLine ::
//::::::::::::::::::::::::::
#include "GeoPrimitives/GeoPrimitives.h"

namespace MuonCalib {
/// \class MtStraightLine
/// This class contains a simple implementation of a straifht line with errors
/// on the slope and the intercept of the straight line.
///
/// \author Oliver.Kortner@CERN.CH \author Felix.Rauscher@CERN.CH
///
/// \date 01.03.2006
}

#ifndef MTStraightLineH
#define MTStraightLineH


//::::::::::::::::::
//:: HEADER FILES ::
//::::::::::::::::::

// CLHEP //
//#include "CLHEP/config/CLHEP.h"
//#include "CLHEP/Units/SystemOfUnits.h"
//#include "CLHEP/Units/PhysicalConstants.h"

namespace MuonCalib {

class MTStraightLine {

public:
// Constructors //
	MTStraightLine(void) {
		init();
		}	
	///< Default constructor: all internal parameters of the straight line
	///< are set to 0.

	MTStraightLine(const Amg::Vector3D & r_position,
                                const Amg::Vector3D & r_direction,
                                const Amg::Vector3D & r_position_error,
                                const Amg::Vector3D & r_direction_error) {
		init(r_position, r_direction,
                     r_position_error, r_direction_error);
		}
	///< Constructor:
	///< r_position: position vector of the straight line;
	///< r_direction: direction vector of the straight line;
	///< r_position_error: error on the position vector;
 	///< r_direction_error: error on the direction vector.

	MTStraightLine(const double & r_a_x1, const double & r_b_x1,
		const double & r_a_x2, const double & r_b_x2,
		const double & r_a_x1_err, const double & r_b_x1_err,
		const double & r_a_x2_err, const double & r_b_x2_err) {
		init(r_a_x1, r_b_x1, r_a_x2, r_b_x2,
                     r_a_x1_err, r_b_x1_err, r_a_x2_err, r_b_x2_err);
		}
	///< Coordinates: x1, x2, x3.
	///< Parametrization of the straight line: x1 = r_a_x1*x3 + r_b_x1;
	///<                                       x2 = r_a_x2*x3 + r_b_x2;
	///<                                       x3 arbitrary.
	///< r_a_x1_err: error on r_a_x1.
	///< r_b_x1_err: error on r_b_x1.
	///< r_a_x2_err: error on r_a_x2.
	///< r_b_x2_err: error on r_b_x2.

// Methods //
// get-methods //
	Amg::Vector3D positionVector(void) const;
	                                ///< get the position vector of the
                                        ///< straight line
	Amg::Vector3D directionVector(void) const;
	                                ///< get the direction vector of the
	                                ///< straight line
	Amg::Vector3D positionError(void) const;
	                                ///< get the error on the position 
                                        ///< vector of the straight line
	Amg::Vector3D directionError(void) const;
	                                ///< get the error on the direction 
	                                ///< vector of the straight line
 	double a_x1(void) const;
	                                ///< get the slope of the straight line 
                                        ///< in the x1-x3 plane
	double a_x1_error(void) const;
	                                ///< get the error on the slope of the 
	                                ///< straight line in the x1-x3 plane
	double b_x1(void) const;
	                                ///< get the intercept of the straight 
	                                ///< line in the x1-x3 plane
	double b_x1_error(void) const;
	                                ///< get the error on the intercept of 
	                                ///< the straight line in the x1-x3
                                        ///< plane
	double a_x2(void) const;
	                                ///< get the slope of the straight line
	                                ///< in the x2-x3 plane
	double a_x2_error(void) const;
	                                ///< get the error on the slope of the 
	                                ///< straight line in the x2-x3 plane
	double b_x2(void) const;
	                                ///< get the intercept of the straight 
	                                ///< line in the x2-x3 plane
	double b_x2_error(void) const;
	                                ///< get the slope of the intercept of
                                        ///< the straight line in the x2-x3
                                        ///< plane
	Amg::Vector3D pointOnLine(const double & lambda) const;
	                                ///< get the point on the line for the 
                                        ///< given scale factor lambda,
	                                ///< point=position_vector+lambda*direction_vector
	double signDistFrom(const MTStraightLine & h) const;
	                                ///< get the signed distance of two
                                        ///< lines
                                        ///< (if both are parallel, dist>0)
	double distFromLine(const Amg::Vector3D & point) const;
	                                ///< get the distance of point point
                                        ///< from straight line

private:
// internal representation of the straight line //
	Amg::Vector3D m_position; //position vector of the straight line
	Amg::Vector3D m_direction; //direction vector of the straight line

// errors on the position and direction vector //
	Amg::Vector3D m_position_error; //error on the position vector
	Amg::Vector3D m_direction_error; //error on the direction vector

// initializtion methods //
	void init(void); //default initialization method

	void init(const Amg::Vector3D & r_position,
				const Amg::Vector3D & r_direction,
				const Amg::Vector3D & r_position_error,
				const Amg::Vector3D & r_direction_error);
	                  // r_position: position vector of the straight line;
	                  // r_direction: direction vector of the straight line;
	                  // r_position_error: error on the position vector;
 	                  // r_direction_error: error on the direction vector

	void init(const double & r_a_x1, const double & r_b_x1,
		const double & r_a_x2, const double & r_b_x2,
		const double & r_a_x1_err, const double & r_b_x1_err,
		const double & r_a_x2_err, const double & r_b_x2_err);
	       // Coordinates: x1, x2, x3.
	       // Parametrization of the straight line: x1 = r_a_x1*x3 + r_b_x1;
	       //                                       x2 = r_a_x2*x3 + r_b_x2;
	       //                                       x3 arbitrary.
	       // r_a_x1_err: error on r_a_x1.
	       // r_b_x1_err: error on r_b_x1.
	       // r_a_x2_err: error on r_a_x2.
	       // r_b_x2_err: error on r_b_x2.

};

}

#endif
