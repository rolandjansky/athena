//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// 04.04.2005, AUTHOR: OLIVER KORTNER
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

///////////////////////////////////////////////////////////////////////
// IMPLEMENTATION OF INLINE METHODS DEFINED IN THE CLASS SamplePoint //
///////////////////////////////////////////////////////////////////////

//*****************************************************************************

//////////////////////////////////////
// METHOD init(double triplet[3]) //
//////////////////////////////////////

namespace MuonCalib {

inline void SamplePoint::init(double triplet[3]) {

	m_x1 = triplet[0];
	m_x2 = triplet[1];
	m_sigma = triplet[2];

	return;

}

//*****************************************************************************

////////////////////////////
// METHOD init(., ., .) //
////////////////////////////

inline void SamplePoint::init(const double & mx1, const double & mx2,
						const double & msigma) {

	m_x1 = mx1;
	m_x2 = mx2;
	m_sigma = msigma;

	return;

}

//*****************************************************************************

///////////////
// METHOD x1 //
///////////////

inline double SamplePoint::x1(void) const {

	return m_x1;

}

//*****************************************************************************

///////////////
// METHOD x2 //
///////////////

inline double SamplePoint::x2(void) const {

	return m_x2;

}

//*****************************************************************************

//////////////////
// METHOD error //
//////////////////

inline double SamplePoint::error(void) const {

	return m_sigma;

}

//*****************************************************************************

////////////////////////
// METHOD set_triplet //
////////////////////////

inline void SamplePoint::set_triplet(const double x[3]) {


	m_x1 = x[0];
	m_x2 = x[1];
	m_sigma = x[2];

	return;

}

//*****************************************************************************

///////////////////
// METHOD set_x1 //
///////////////////

inline void SamplePoint::set_x1(const double & mx1) {

	m_x1 = mx1;
	return;

}

//*****************************************************************************

///////////////////
// METHOD set_x2 //
///////////////////

inline void SamplePoint::set_x2(const double & mx2) {

	m_x2 =mx2;
	return;

}

//*****************************************************************************

//////////////////////
// METHOD set_error //
//////////////////////

inline void SamplePoint::set_error(const double & merror) {

	m_sigma = merror;
	return;

}

}
