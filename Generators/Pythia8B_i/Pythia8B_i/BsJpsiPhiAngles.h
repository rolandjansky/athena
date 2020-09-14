/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef BsJpsiPhiAngles_H
#define BsJpsiPhiAngles_H

#include "CLHEP/Vector/LorentzVector.h"
class BsJpsiPhiAngles{

  
private:

    // Member data 
  double m_theta1;
  double m_theta2;
  double m_chi;
  double m_thetatr; 
  double m_phitr;

public:
  double thetaK()  const { return m_theta1;  }
  double thetaL()  const { return m_theta2;  }
  double chi()     const { return m_chi;     }
  double thetatr() const { return m_thetatr; }
  double phitr()   const { return m_phitr;   } 
  
   double thetaKfix()  const { return cos(m_theta1);  }
  double thetaLfix()  const { return cos(m_theta2);  }
  double chifix()     const { return -(m_chi - M_PI) ;     }
  double thetatrfix() const { return -cos(m_thetatr); }
  double phitrfix()   const { return -(m_phitr - M_PI);   }  

//      Input: 
//            For J/Psi(l+l-) K*(K pi):
//             o cand1 = K    (K from K* decay)
//             o cand2 = l+   (positive lepton from psi decay)
//             o aBcand = B   (the B from which the decay originates)

  BsJpsiPhiAngles(const CLHEP::HepLorentzVector &Kplus,
                  const CLHEP::HepLorentzVector &Muplus,
                  const CLHEP::HepLorentzVector &Phi,
                  const CLHEP::HepLorentzVector &Jpsi,
                  const CLHEP::HepLorentzVector &Bs)
  {
    using CLHEP::HepLorentzVector;
    using CLHEP::Hep3Vector;

     m_theta1  = 0.;
     m_theta2  = 0.;
     m_chi     = 0.;
     m_thetatr = 0.;
     m_phitr   = 0.;
     
     
       // 0.1 Get the Mummy1 & Mummy2 4-vectors in lab
  HepLorentzVector Mum1QV ( Phi ) ; // this should be phi
  HepLorentzVector Mum2QV ( Jpsi ) ; // this should be jpsi
  // 0.2 Boost the Mummy1 & Mummy2 to the B(aBcand) rest frame
  HepLorentzVector BQV(Bs); // this should be Bs Meson

  Hep3Vector Bboost ( BQV.boostVector() );
  Mum1QV.boost ( -( Bboost ) );
  Mum2QV.boost ( -( Bboost ) ); 
    // 1. Compute helicity angle theta1
  // 1.1 Boost cand1 to the B rest frame (grandMa)
  HepLorentzVector cand1QV ( Kplus );
  cand1QV.boost ( - (Bboost) );
  // 1.2 Boost now cand1 in his mother rest frame
  cand1QV.boost ( - (Mum1QV.boostVector() ) ); 
  // 1.3 compute the theta1 angle
  Hep3Vector Mum1TV = Mum1QV.boostVector(); 
  Hep3Vector cand1TV = cand1QV.boostVector();
  Hep3Vector ucand1TV = cand1TV.unit();
  m_theta1 = cand1TV.angle(Mum1TV);
  
  
  
  // 2. Compute helicity angle theta2
  // 2.1 Boost cand2 to the B rest frame (grandMa)
  HepLorentzVector cand2QV ( Muplus );
  cand2QV.boost ( - (Bboost) );
  // 2.2 Boost now cand2 in his mother rest frame
  cand2QV.boost ( - (Mum2QV.boostVector() ) ); 
  // 2.3 compute the theta2 angle
  Hep3Vector Mum2TV = Mum2QV.boostVector(); 
  Hep3Vector cand2TV = cand2QV.boostVector();
  Hep3Vector ucand2TV = cand2TV.unit();
  m_theta2 = cand2TV.angle(Mum2TV);

  // 3. Compute the chi angle
  // 3.1 Define the c and d vectors (see Physbook) 
  Hep3Vector c;
  Hep3Vector d;
  Hep3Vector uMum1TV = Mum1TV.unit();
  Hep3Vector uMum2TV = Mum2TV.unit();

  // 3.2 Calculate these vectors
  c = (uMum1TV.cross(ucand1TV)).cross(uMum1TV);
  d = (uMum2TV.cross(ucand2TV)).cross(uMum2TV);
  double sinphi = (c.cross(uMum1TV)).dot(d);
  double cosphi = c.dot(d);
  // 3.3 Now get chi = angle between the 2 decay planes
  m_chi = std::atan2(sinphi,cosphi);
  // 3.4 Put chi in [0;2pi]
  if ( m_chi < 0 )
  {
    m_chi = 2.*M_PI + m_chi;
  }
  
  // 4. Transversity angles
  double cosThetaTr = std::sin ( m_theta2 ) * std::sin ( m_chi ) ;
  double sinThetaTr = std::sqrt ( 1. - cosThetaTr * cosThetaTr ) ; 
  double cosPhiTr = std::cos(m_theta2) / sinThetaTr ;
  double sinPhiTr = std::sin(m_theta2) * std::cos( m_chi ) / sinThetaTr ;
  m_phitr   = std::atan2 ( sinPhiTr, cosPhiTr ) ;
  if (m_phitr < 0) 
  {
    m_phitr = 2.*M_PI + m_phitr;
  }
  m_thetatr = std::acos(cosThetaTr) ;
  
  
  	
  }


};
#endif
