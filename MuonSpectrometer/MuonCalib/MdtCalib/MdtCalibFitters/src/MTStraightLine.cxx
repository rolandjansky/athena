/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// 01.03.2006, AUTHORS: OLIVER KORTNER, FELIX RAUSCHER
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

//::::::::::::::::::
//:: HEADER FILES ::
//::::::::::::::::::

#include "MdtCalibFitters/MTStraightLine.h"
#include "cmath"
#include <iostream>
//:::::::::::::::::::::::
//:: NAMESPACE SETTING ::
//:::::::::::::::::::::::

using namespace MuonCalib;

//using namespace std;

//:::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
//:: IMPLEMENTATION OF METHODS DEFINED IN THE CLASS MTStraightLine ::
//:::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

//*****************************************************************************

//:::::::::::::::::
//:: METHOD init ::
//:::::::::::::::::

void MTStraightLine::init(void) {

	m_position = Amg::Vector3D(0.0, 0.0, 0.0);
	m_direction = Amg::Vector3D(0.0, 0.0, 0.0);
	m_position_error = Amg::Vector3D(0.0, 0.0, 0.0);
	m_direction_error = Amg::Vector3D(0.0, 0.0, 0.0);

	return;

}

//*****************************************************************************

//::::::::::::::::::::::::::::::::::::::::::::
//:: METHOD init(const Amg::Vector3D &, ...) ::
//::::::::::::::::::::::::::::::::::::::::::::

void MTStraightLine::init(const Amg::Vector3D & r_position,
                          const Amg::Vector3D & r_direction,
                          const Amg::Vector3D & r_position_error,
                          const Amg::Vector3D & r_direction_error) {

	m_position = r_position;
	m_direction = r_direction;
	m_position_error = r_position_error;
	m_direction_error = r_direction_error;

	return;

}

//*****************************************************************************

//:::::::::::::::::
//:: METHOD init ::
//:::::::::::::::::

void MTStraightLine::init(const double & r_a_x1,
		const double & r_b_x1,
		const double & r_a_x2, const double & r_b_x2,
		const double & r_a_x1_err, const double & r_b_x1_err,
		const double & r_a_x2_err, const double & r_b_x2_err) {

	m_position = Amg::Vector3D(r_b_x1, r_b_x2, 0.0);
	m_direction = Amg::Vector3D(r_a_x1, r_a_x2, 1.0);
	m_position_error = Amg::Vector3D(r_b_x1_err, r_b_x2_err, 0.0);
	m_direction_error = Amg::Vector3D(r_a_x1_err, r_a_x2_err, 1.0);

	return;

}

//*****************************************************************************

//:::::::::::::::::::::::::::
//:: METHOD positionVector ::
//:::::::::::::::::::::::::::

Amg::Vector3D MTStraightLine::positionVector(void) const {

	return m_position;

}

//*****************************************************************************

//::::::::::::::::::::::::::::
//:: METHOD directionVector ::
//::::::::::::::::::::::::::::

Amg::Vector3D MTStraightLine::directionVector(void) const {

	return m_direction;

}

//*****************************************************************************

//::::::::::::::::::::::::::
//:: METHOD positionError ::
//::::::::::::::::::::::::::

Amg::Vector3D MTStraightLine::positionError(void) const {

	return m_position_error;

}

//*****************************************************************************

//:::::::::::::::::::::::::::
//:: METHOD directionError ::
//:::::::::::::::::::::::::::

Amg::Vector3D MTStraightLine::directionError(void) const {

	return m_direction_error;

}

//*****************************************************************************

//:::::::::::::::::
//:: METHOD a_x1 ::
//:::::::::::::::::

double MTStraightLine::a_x1(void) const {

	if (m_direction.z() == 0.0) {
		return 0.0;
	}
	return m_direction.x()/m_direction.z();


}

//*****************************************************************************

//:::::::::::::::::::::::
//:: METHOD a_x1_error ::
//:::::::::::::::::::::::

double MTStraightLine::a_x1_error(void) const {

	if (m_direction.z()==0) {
		return 0.0;
	}

	return sqrt(std::pow(m_direction_error.x()/m_direction_error.z(), 2)
		+std::pow(m_direction_error.y()*a_x1()/m_direction_error.z(), 2));

}

//*****************************************************************************

//:::::::::::::::::
//:: METHOD b_x1 ::
//:::::::::::::::::

double MTStraightLine::b_x1(void) const {

	if (m_direction.z() == 0.0) {
		std::cerr << "Class MTStraightLine, method b_x1: WARNING!\n"
			  << "b_x1 not uniquely defined." << std::endl;
		return m_position.x();
	}
	return (m_position.x()-m_position.z()*a_x1());
					
}

//*****************************************************************************

//:::::::::::::::::::::::
//:: METHOD b_x1_error ::
//:::::::::::::::::::::::

double MTStraightLine::b_x1_error(void) const {

	return sqrt(std::pow(m_position_error.x(), 2)+
			std::pow(a_x1()*m_position_error.z(), 2)+
			std::pow(m_position.z()*a_x1_error(),2));

}

//*****************************************************************************

//:::::::::::::::::
//:: METHOD a_x2 ::
//:::::::::::::::::

double MTStraightLine::a_x2(void) const {

	if (m_direction.z() == 0.0) {
		return 0.0;
	}
	return m_direction.y()/m_direction.z();

}

//*****************************************************************************

//:::::::::::::::::::::::
//:: METHOD a_x2_error ::
//:::::::::::::::::::::::

double MTStraightLine::a_x2_error(void) const {

	if (m_direction.z()==0) {
		return 0.0;
	}

	return sqrt(std::pow(m_direction_error.y()/m_direction_error.z(), 2)
		+std::pow(m_direction_error.y()*a_x2()/m_direction_error.z(), 2));

}

//*****************************************************************************

//:::::::::::::::::
//:: METHOD b_x2 ::
//:::::::::::::::::

double MTStraightLine::b_x2(void) const {

	if (m_direction.z() == 0.0) {
		std::cerr << "Class MTStraightLine, method b_x2: WARNING!\n"
			  << "b_x2 not uniquely defined." << std::endl;
		return m_position.x();
	}
	return (m_position.y()-m_position.z()*a_x2());

}

//*****************************************************************************

//:::::::::::::::::::::::
//:: METHOD b_x2_error ::
//:::::::::::::::::::::::

double MTStraightLine::b_x2_error(void) const {

	return sqrt(std::pow(m_position_error.y(), 2)+
			std::pow(a_x2()*m_position_error.z(), 2)+
			std::pow(m_position.z()*a_x2_error(),2));

}

//*****************************************************************************

//::::::::::::::::::::::::
//:: METHOD pointOnLine ::
//::::::::::::::::::::::::

Amg::Vector3D MTStraightLine::pointOnLine(const double & lambda) const {

	return m_position+lambda*m_direction;

}

//*****************************************************************************

//:::::::::::::::::::::::::
//:: METHOD signDistFrom ::
//:::::::::::::::::::::::::

double MTStraightLine::signDistFrom(const MTStraightLine & h) const {

//:::::::::::::::::::::::::
//:: AUXILIARY VARIABLES ::
//:::::::::::::::::::::::::

	Amg::Vector3D a = m_position, u = m_direction;
					// position and direction vectors of g
	Amg::Vector3D b = h.positionVector(), v = h.directionVector();
					// position and direction vectors of h
	Amg::Vector3D n; // normal vector of plane spanned by g and h
	Amg::Vector3D d; // distance vector

//:::::::::::::
//:: PROGRAM ::
//:::::::::::::

//::::::::::::::::::::::::
//:: collinearity check ::
//::::::::::::::::::::::::

	n = u.cross(v);
	d = a-b;
	if (n.dot(n) == 0.0) {
		// straight lines are parallel
		// no sign given
	  return sqrt(d.dot(d)-(u.unit().dot(d))*(u.unit().dot(d)));
	}
	
	return (d.dot(n.unit()));

}

//*****************************************************************************

//:::::::::::::::::::::::::
//:: METHOD distFromLine ::
//:::::::::::::::::::::::::

double MTStraightLine::distFromLine(const Amg::Vector3D & point) const {

//:::::::::::::::::::::::::
//:: AUXILIARY VARIABLES ::
//:::::::::::::::::::::::::

	Amg::Vector3D u = m_direction;

//::::::::::::::::::::::::
//:: CALCULATE DISTANCE ::
//::::::::::::::::::::::::

	return sqrt((point-m_position).dot(point-m_position) -
		    ((point-m_position).dot(u.unit()))*((point-m_position).dot(u.unit())));

}
