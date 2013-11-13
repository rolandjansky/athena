///////////////////////// -*- C++ -*- /////////////////////////////

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef FOURMOM_FOURMOM_TEST_H
#define FOURMOM_FOURMOM_TEST_H

#include <cassert>
#include <stdexcept>
#include <string>
#include <sstream>
#include <iomanip>
#include <iostream>
#include <vector>

class p4_test_err
  : public std::exception
{
public:
  explicit p4_test_err (const char* file,
                        int line,
                        const char* what);
  virtual ~p4_test_err() throw() {}
  virtual const char* what() const throw() { return m_what.c_str(); }
private:
  std::string m_what;
};


p4_test_err::p4_test_err (const char* file,
                          int line,
                          const char* what)
{
  std::ostringstream os;
  os << file << ":" << line << " P4-assertion failure: " << what;
  m_what = os.str();
}


void throw_p4_test_err (const char* file, int line, const char* what)
{
  throw (p4_test_err (file, line, what));
}

#define p4assert(X) do {     \
    if (!(X)) { \
      throw_p4_test_err (__FILE__, __LINE__, #X); \
    } \
  } while (0)

#define HLV_ASSERT(p1, p2) do {     \
    p4assert( std::abs( p1.px() - p2.px() ) < epsilon );     \
    p4assert( std::abs( p1.py() - p2.py() ) < epsilon );     \
    p4assert( std::abs( p1.pz() - p2.pz() ) < epsilon );     \
    p4assert( std::abs( p1.e () - p2.e () ) < epsilon );     \
    p4assert( p1.isNear(p2, epsilon) );                      \
  } while (0)


#define FOURMOM_ASSERT(X) p4assert(X)

//
// control of tolerance
// 
const double epsilon = 1e-10;

namespace FourMom {

template<class I4Mom1_t, class I4Mom2_t>
void testGetters( const double px, const double py, 
		  const double pz, const double e )
{
  CLHEP::HepLorentzVector hlv( px, py, pz, e );
  I4Mom1_t p1( hlv );

  p4assert( std::abs( p1.px() - hlv.px() ) < epsilon );
  p4assert( std::abs( p1.py() - hlv.py() ) < epsilon );
  p4assert( std::abs( p1.pz() - hlv.pz() ) < epsilon );
  p4assert( std::abs( p1.m () - hlv.m () ) < epsilon );
  
  p4assert( std::abs( p1.m2() - hlv.m2() ) < epsilon*p1.m2() );
  p4assert( std::abs( p1.p () - std::sqrt( hlv.px()*hlv.px() + 
					   hlv.py()*hlv.py() + 
					   hlv.pz()*hlv.pz() ) ) < epsilon );
  // loose a little bit the constraint on this test...
  p4assert( std::abs( p1.p2() - ( hlv.px()*hlv.px() + 
				  hlv.py()*hlv.py() + 
				  hlv.pz()*hlv.pz() ) ) < epsilon*p1.p2() );
  p4assert( std::abs( p1.eta() - hlv.eta() ) < epsilon );
  p4assert( std::abs( p1.phi() - hlv.phi() ) < epsilon );
  p4assert( std::abs( p1.e()   - hlv.e()   ) < epsilon );
  p4assert( std::abs( p1.et()  - hlv.et()  ) < epsilon );
  p4assert( std::abs( p1.pt()  - hlv.perp()  ) < epsilon );
  p4assert( std::abs( p1.iPt() - (1./hlv.perp()) ) < epsilon );
  p4assert( std::abs( p1.cosTh() - std::cos( hlv.theta() ) ) < epsilon );
  p4assert( std::abs( p1.sinTh() - std::sin( hlv.theta() ) ) < epsilon );
  p4assert( std::abs( p1.cotTh() - std::sinh( hlv.eta()  ) ) < epsilon );

  //#ifdef FOURMOM_PTETAPHIM_TEST_CXX
  //DR now yes
  //  not in I4Momentum interface - special to 'PtEtaPhiM'
  p4assert( std::abs( p1.tanTh() - std::tan( hlv.theta()  ) ) < epsilon );
  //#endif //FOURMOM_PTETAPHIM_TEST_CXX
  
  {
    const CLHEP::HepLorentzVector p4 = p1.hlv();
    HLV_ASSERT( p4, hlv );
  }

  I4Mom2_t p2( hlv );
  {
    const CLHEP::HepLorentzVector p4 = p2.hlv();
    HLV_ASSERT( p4, hlv );
  }

  const I4Momentum& p = p2;
  p4assert( std::abs( p1.px()    - p.px()    ) < epsilon );
  p4assert( std::abs( p1.py()    - p.py()    ) < epsilon );
  p4assert( std::abs( p1.pz()    - p.pz()    ) < epsilon );
  p4assert( std::abs( p1.m ()    - p.m ()    ) < epsilon );
  p4assert( std::abs( p1.m2()    - p.m2()    ) < epsilon*p1.m2() );
  p4assert( std::abs( p1.p ()    - p.p ()    ) < epsilon );
  p4assert( std::abs( p1.p2()    - p.p2()    ) < epsilon*p1.p2() ); // as above
  p4assert( std::abs( p1.eta()   - p.eta()   ) < epsilon );
  p4assert( std::abs( p1.phi()   - p.phi()   ) < epsilon );
  p4assert( std::abs( p1.e()     - p.e()     ) < epsilon );
  p4assert( std::abs( p1.et()    - p.et()    ) < epsilon );
  p4assert( std::abs( p1.pt()    - p.pt()    ) < epsilon );
  p4assert( std::abs( p1.iPt()   - p.iPt()   ) < epsilon );
  p4assert( std::abs( p1.cosTh() - p.cosTh() ) < epsilon );
  p4assert( std::abs( p1.sinTh() - p.sinTh() ) < epsilon );
  p4assert( std::abs( p1.cotTh() - p.cotTh() ) < epsilon );

  {
    const CLHEP::HepLorentzVector p4 = p.hlv();
    HLV_ASSERT( p4, hlv );
  }

  return;
}
} // end namespace FourMom

#endif // FOURMOM_FOURMOM_TEST_H
