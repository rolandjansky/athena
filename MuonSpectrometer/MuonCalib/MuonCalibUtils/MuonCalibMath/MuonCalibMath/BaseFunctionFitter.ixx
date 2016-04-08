//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// 05.04.2005, AUTHOR: OLIVER KORTNER
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

//////////////////////////////////////////////////////////////////////////////
// IMPLEMENTATION OF INLINE METHODS DEFINED IN THE CLASS BaseFunctionFitter //
//////////////////////////////////////////////////////////////////////////////


//*****************************************************************************

/////////////////////////
// METHOD init(void) //
/////////////////////////

namespace MuonCalib {

inline void BaseFunctionFitter::init(void) {

	init(5);
	return;

}

//*****************************************************************************

//////////////////////
// METHOD init(.) //
//////////////////////

inline void BaseFunctionFitter::init(const int & nb_coefficients) {

	m_A = Amg::MatrixX(nb_coefficients,nb_coefficients);
	m_A.setZero();
	m_b = Amg::VectorX(nb_coefficients);
	m_b.setZero();
	m_alpha = Amg::VectorX(nb_coefficients);
	m_alpha.setZero();
	m_nb_coefficients = nb_coefficients;

	return;

}

//*****************************************************************************

///////////////////////////////////
// METHOD number_of_coefficients //
///////////////////////////////////

inline int BaseFunctionFitter::number_of_coefficients(void) const {

	return m_nb_coefficients;

}

//*****************************************************************************

/////////////////////////
// METHOD coefficients //
/////////////////////////

inline Amg::VectorX BaseFunctionFitter::coefficients(void) const {

	return m_alpha;

}

//*****************************************************************************

///////////////////////////////////////
// METHOD set_number_of_coefficients //
///////////////////////////////////////

inline void BaseFunctionFitter::set_number_of_coefficients(
						const int & nb_coefficients) {

	init(nb_coefficients);

}

}
