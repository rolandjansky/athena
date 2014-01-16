//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// 05.04.2005, AUTHOR: OLIVER KORTNER
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

//////////////////////////////////////////////////////////////////////////////
// IMPLEMENTATION OF INLINE METHODS DEFINED IN THE CLASS BaseFunctionFitter //
//////////////////////////////////////////////////////////////////////////////


//*****************************************************************************

/////////////////////////
// METHOD m_init(void) //
/////////////////////////

namespace MuonCalib {

inline void BaseFunctionFitter::m_init(void) {

	m_init(5);
	return;

}

//*****************************************************************************

//////////////////////
// METHOD m_init(.) //
//////////////////////

inline void BaseFunctionFitter::m_init(const int & nb_coefficients) {

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

	m_init(nb_coefficients);

}

}
