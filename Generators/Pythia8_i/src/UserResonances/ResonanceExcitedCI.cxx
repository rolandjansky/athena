// The ResonanceExcited class handles excited-fermion resonances.
// This is a copy of existing pythia ResonanceExcited class
// with extention to include decays going via contact interactions (4-fermion vertexes)
//
// author Olya Igonkina, modified by James Monk for Pythia8_i

#include "Pythia8_i/UserResonanceFactory.h"

namespace Pythia8{
  class ResonanceExcitedCI;
}

Pythia8_UserResonance::UserResonanceFactory::Creator<Pythia8::ResonanceExcitedCI> resonanceExcitedCreator("ExcitedCI");

namespace Pythia8{
  
  class ResonanceExcitedCI : public ResonanceWidths {
    
  public:
    
    // Constructor. 
    ResonanceExcitedCI(int idResIn):
    m_lambda(0.), m_coupF(0.), m_coupFprime(0.), m_coupFcol(0.),
    m_sin2tW(0.), m_cos2tW(0.){
      initBasic(idResIn); std::cout << " ResonanceExcitedCI constructor\n"; 
    } 
    
  private:
    
    void initConstants() {
      
      // Locally stored properties and couplings.
      m_lambda        = settingsPtr->parm("ExcitedFermion:Lambda");
      m_coupF         = settingsPtr->parm("ExcitedFermion:coupF");
      m_coupFprime    = settingsPtr->parm("ExcitedFermion:coupFprime");
      m_coupFcol      = settingsPtr->parm("ExcitedFermion:coupFcol");
      m_sin2tW        = couplingsPtr->sin2thetaW();
      m_cos2tW        = 1. - m_sin2tW;
      
      return;
    }
    
    //--------------------------------------------------------------------------
    
    // Calculate various common prefactors for the current mass.
    
    void calcPreFac(bool) {
      // Common coupling factors.
      alpEM         = couplingsPtr->alphaEM(mHat * mHat);
      alpS          = couplingsPtr->alphaS(mHat * mHat);
      preFac        = pow3(mHat) / pow2(m_lambda);
      
      return;
    }
    
    //--------------------------------------------------------------------------
    
    // Calculate width for currently considered channel.
    
    void calcWidth(bool) {
      
      // Check that above threshold.
      if (ps == 0.) return;
      
      // f^* -> f g.
      if (id1Abs == 21) widNow = preFac * alpS * pow2(m_coupFcol) / 3.;
      
      // f^* -> f gamma.
      else if (id1Abs == 22) {
        double chgI3 = (id2Abs%2 == 0) ? 0.5 : -0.5;
        double chgY  = (id2Abs < 9) ? 1. / 6. : -0.5; 
        double chg   = chgI3 * m_coupF + chgY * m_coupFprime;
        widNow       = preFac * alpEM * pow2(chg) / 4.;      
      }
      // f^* -> f Z^0.
      else if (id1Abs == 23) {
        double chgI3 = (id2Abs%2 == 0) ? 0.5 : -0.5;
        double chgY  = (id2Abs < 9) ? 1. / 6. : -0.5; 
        double chg   = chgI3 * m_cos2tW * m_coupF - chgY * m_sin2tW * m_coupFprime;
        widNow       = preFac * (alpEM * pow2(chg) / (8. * m_sin2tW * m_cos2tW))
        * ps*ps * (2. + mr1);      
      }
      
      // f^* -> f' W^+-.
      else if (id1Abs == 24) widNow  = preFac * (alpEM * pow2(m_coupF) 
                                                 / (16. * m_sin2tW)) * ps*ps * (2. + mr1);      
      // idxAbs are sorted according to their abs value ; use idRes
      
      //  f^* -> f f' \bar{f'}
      else {
        //std::cout << " hei! in ResonanceExcitedCI processing F* -> f f'f'bar decays "<< id1Abs <<" "<<id2Abs<<" "<<id3Abs<< "\n";
        
        if( id1Abs < 17 && id2Abs < 17 && id3Abs>0 && id3Abs < 17 ){  // f* -> f f'\bar(f'} decay
          widNow = preFac * pow2(mHat) / pow2(m_lambda) / 96. / M_PI ;
          if( id3Abs < 10 ) widNow *= 3.; // quarks in final state. id1 is highest pdgId of f, f'
          if( id1Abs == id2Abs && id1Abs == id3Abs ){
            if( abs(idRes)-4000000 < 10 ) widNow *= 4./3.;  
            else                          widNow *= 2.;
          }
        }
      }
     
      return;
    }
    
    // Locally stored properties and couplings.
    double m_lambda, m_coupF, m_coupFprime, m_coupFcol, m_sin2tW, m_cos2tW;
    
  };
}
