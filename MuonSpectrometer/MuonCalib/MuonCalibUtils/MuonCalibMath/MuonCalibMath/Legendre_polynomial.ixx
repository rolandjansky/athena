//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// 22.05.2004, AUTHOR: OLIVER KORTNER
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

///////////////////////////////////////////////////////////////////////////////
// IMPLEMENTATION OF INLINE METHODS DEFINED IN THE CLASS Legendre_polynomial //
///////////////////////////////////////////////////////////////////////////////

//*****************************************************************************

//////////////////
// METHOD value //
//////////////////

#include "cmath"
#include "cstdlib"

namespace MuonCalib {

inline double Legendre_polynomial::value(const int & order,
						const double & x) const {

//////////////////////////////////////////////////////////////////////////////
// LOWER ORDER LEGENDRE POLYNOMIALS ARE GIVEN EXPLICITLY, HIGHER ORDERS ARE //
// CALCULATE RECURSIVELY.                                                   //
//////////////////////////////////////////////////////////////////////////////

	switch(std::abs(order)) {
		case 0:
			return 1;
			break;
		case 1:
			return x;
			break;
		case 2:
			return 0.5*(3.0*x*x-1.0);
			break;
		case 3:
			return 0.5*(5.0*x*x*x-3.0*x);
			break;
		case 4:
			return (35.0*std::pow(x, 4)-30.0*x*x+3.0)/8.0;
			break;
		case 5:
			return (63.0*std::pow(x, 5)-70.0*x*x*x+15.0*x)/8.0;
			break;
		case 6:
			return (231.0*std::pow(x, 6)-315.0*std::pow(x, 4)+105.0*x*x-5.0)
									/16.0;
			break;
		case 7:
			return (429.0*std::pow(x, 7)-693*std::pow(x, 5)+315.0*x*x*x
				-35.0*x)/16.0;
			break;
		default:
//			return (2.0*order-1)*x*value(order-1, x)/
// 					static_cast<double>(order)
//				- (order-1)*value(order-2, x)/
//					static_cast<double>(order);
			break;
  	}
//  Move default option here to check order in order to satisfy Coverity
        if( order ) {		
          return (2.0*order-1)*x*value(order-1, x)/static_cast<double>(order)
	         - (order-1)*value(order-2, x)/static_cast<double>(order);
        }
        return 0;
}

}
