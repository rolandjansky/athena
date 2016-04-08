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

inline double Tschebyscheff_polynomial::value(const unsigned int & order,
						const double & x) const {


//////////////////////////////////////////////////////////////////////////////
// LOWER ORDER LEGENDRE POLYNOMIALS ARE GIVEN EXPLICITLY, HIGHER ORDERS ARE //
// CALCULATE RECURSIVELY.                                                   //
//////////////////////////////////////////////////////////////////////////////

	switch(order) {
		case 0:
			return 1;
			break;
		case 1:
			return x;
			break;
		case 2:
		default:
			return 2.0*x*value(order-1, x) - value(order-2, x);
			break;
	}

}

}
