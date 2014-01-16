//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// 27.12.2004, AUTHOR: OLIVER KORTNER
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

///////////////////////////////////////////////////////////
// IMPLEMENTATION OF INLINE METHODS DEFINED IN THE CLASS //
//              Tschebyscheff_polynomial                 //
///////////////////////////////////////////////////////////

//*****************************************************************************

//////////////////
// METHOD value //
//////////////////

namespace MuonCalib {

inline double Tschebyscheff_polynomial::value(const unsigned int & m_order,
						const double & x) const {


//////////////////////////////////////////////////////////////////////////////
// LOWER ORDER LEGENDRE POLYNOMIALS ARE GIVEN EXPLICITLY, HIGHER ORDERS ARE //
// CALCULATE RECURSIVELY.                                                   //
//////////////////////////////////////////////////////////////////////////////

	switch(m_order) {
		case 0:
			return 1;
			break;
		case 1:
			return x;
			break;
		case 2:
		default:
			return 2.0*x*value(m_order-1, x) - value(m_order-2, x);
			break;
	}

	return 0.0;

}

}
