// Single excited lepton production  - similar to original pythia8 code, 
// but polarization of final state is reversed [to match Calchep distributions

#include "Pythia8_i/UserProcessFactory.h"

#include <iostream>
#include <vector>

namespace Pythia8 {
  class Sigma2qqbar2lStarlBar;
  class Sigma2qqbar2nueStarnueBar;  
  class Sigma2qqbar2numuStarnumuBar;
  class Sigma2qqbar2nutauStarnutauBar;
  class Sigma2qqbar2eStareBar;        
  class Sigma2qqbar2muStarmuBar;      
  class Sigma2qqbar2tauStartauBar;    
}

// This one line registers this process with the factory
// An instance of this process can be created with 
// UserProcessFactory::create("qqbar2lStarlBar")
Pythia8_UserProcess::UserProcessFactory::Creator<Pythia8::Sigma2qqbar2nueStarnueBar>     qqbar2nueStarnueBarCreator(    "qqbar2nueStarnueBar");
Pythia8_UserProcess::UserProcessFactory::Creator<Pythia8::Sigma2qqbar2numuStarnumuBar>   qqbar2numuStarnumuBarCreator(  "qqbar2numuStarnumuBar");
Pythia8_UserProcess::UserProcessFactory::Creator<Pythia8::Sigma2qqbar2nutauStarnutauBar> qqbar2nutauStarnutauBarCreator("qqbar2nutauStarnutauBar");
Pythia8_UserProcess::UserProcessFactory::Creator<Pythia8::Sigma2qqbar2eStareBar>         qqbar2eStareBarCreator(        "qqbar2eStareBar");
Pythia8_UserProcess::UserProcessFactory::Creator<Pythia8::Sigma2qqbar2muStarmuBar>       qqbar2muStarmuBarCreator(      "qqbar2muStarmuBar");
Pythia8_UserProcess::UserProcessFactory::Creator<Pythia8::Sigma2qqbar2tauStartauBar>     qqbar2tauStartauBarCreator(    "qqbar2tauStartauBar");

namespace Pythia8 {
  
  //**************************************************************************
  
  // Sigma2qqbar2lStarlBar class.
  // Cross section for q qbar -> lStar lBar
  
  class Sigma2qqbar2lStarlBar: public Sigma2Process {
    
  public:
    //Constructor.
    Sigma2qqbar2lStarlBar(int idlIn) : idl(idlIn),
    idRes(0), codeSave(0),
    Lambda(0.), preFac(0.), openFracPos(0.), openFracNeg(0.), sigma(0.), m2ResTimes4(0.){};
    
    //*********
    
    // Initialize process. 
    
    void initProc() {
      
      // Set up process properties from the chosen lepton flavour.
      idRes         = 4000000 + idl;
      codeSave      = 4060 + idl;
      if      (idl == 11) nameSave = "q qbar -> e^*+- e^-+";
      else if (idl == 12) nameSave = "q qbar -> nu_e^* nu_ebar"; 
      else if (idl == 13) nameSave = "q qbar -> mu^*+- mu^-+"; 
      else if (idl == 14) nameSave = "q qbar -> nu_mu^* nu_mubar"; 
      else if (idl == 15) nameSave = "q qbar -> tau^*+- tau^-+"; 
      else                nameSave = "q qbar -> nu_tau^* nu_taubar";
      
      // Secondary open width fractions.
      openFracPos = particleDataPtr->resOpenFrac( idRes);
      openFracNeg = particleDataPtr->resOpenFrac(-idRes);
      
      // Locally stored properties and couplings.
      Lambda        = settingsPtr->parm("ExcitedFermion:Lambda");
      preFac        = (M_PI / pow4(Lambda)) * (openFracPos + openFracNeg) / 3.; // orig pythia
      //preFac        = (M_PI / pow4(Lambda)) * (openFracPos + openFracNeg) / 6.;
      
    } 
    
    //*********
    // Evaluate sigmaHat(sHat). 
    virtual double sigmaHat() {return sigma;}
    
    
    // Evaluate sigmaHat(sHat), part independent of incoming flavour. 
    
    void sigmaKin() {
      
      // later this gets multiplied further with sH*(1-s3/sH)
      
      sigma = preFac * (-uH) * (sH + tH) / sH2;  // orig pythia
      //sigma = preFac * (2. + s3/sH) *(1 - s3/sH) ;
      //Baur sigma = preFac * ( 1 + 1./3.*(sH - s3)/(sH+s3) ) * (1 - s3/sH) * (1 + s3/sH);
    }
    
    //*********
    
    // Select identity, colour and anticolour.
    
    void setIdColAcol() {
      
      // Flavours: either lepton or antilepton may be excited.
      if (rndmPtr->flat() * (openFracPos + openFracNeg) < openFracPos) {
        setId( id1, id2, idRes, -idl);
        if (id1 < 0) swapTU = true; 
      } else {
        setId( id1, id2, -idRes, idl);
        if (id1 > 0) swapTU = true; 
      }  
      
      // Colour flow trivial.
      if (id1 > 0) setColAcol( 1, 0, 0, 1, 0, 0, 0, 0);
      else         setColAcol( 0, 1, 1, 0, 0, 0, 0, 0);
      
    }
    
    //**************************************************************************
    
    double weightDecay( Event& process, int iResBeg, 
                       int iResEnd) {
      
      // l* should sit in entry 5. and 6 Sequential Z/W decay assumed isotropic.
      if (iResBeg != 5 && iResEnd != 5 && iResBeg != 6 && iResEnd != 6 ) return 1.; 
      
      // Phase space factors.
      double mr1    = pow2(process[7].m() / process[5].m());
      double mr2    = pow2(process[8].m() / process[5].m()); 
      
      // Reconstruct decay angle in l* CoM frame.
      int  idAbs3 = process[7].idAbs();
      Vec4 pLStarCom = (idAbs3 < 20) ? process[8].p() : process[7].p();
      //Vec4 pLStarCom = (idAbs3 < 20) ? process[7].p() : process[8].p(); // orig pythia
      pLStarCom.bstback(process[5].p());
      double cosThe = costheta(pLStarCom, process[5].p());
      double wt     = 1.; 
      
      // Decay, l* -> l + gamma/Z^0/W^+-).
      int idBoson   = (idAbs3 < 20) ? process[8].idAbs() : process[7].idAbs();
      if (idBoson == 22) {
        wt          = 0.5 * (1. + cosThe);
      } else if (idBoson == 23 || idBoson == 24) {
        double mrB  = (idAbs3 < 20) ? mr2 : mr1;
        double kTrm = 0.5 * (mrB * (1. - cosThe));
        wt          = (1. + cosThe + kTrm) / (2 + mrB);
      } 
      
      // Done.
      return wt;
    }
    
    // Info on the subprocess.
    virtual string name()       const {return nameSave;}
    virtual int    code()       const {return codeSave;}
    virtual string inFlux()     const {return "qqbarSame";}
    virtual int    id3Mass()    const {return abs(idRes);}
    
    //virtual bool   convertM2()       const {return false;} //true is only suitable for resonance production pp->A->ll
    //virtual bool   convert2mb()      const {return true;}
    
    // Parameters set at initialization or for current kinematics. 
    int    idl, idRes, codeSave;
    string nameSave;
    double Lambda, preFac, openFracPos, openFracNeg, sigma;
    double m2ResTimes4;
  };
  //**************************************************************************
  class Sigma2qqbar2nueStarnueBar: public Sigma2qqbar2lStarlBar {
  public:
    Sigma2qqbar2nueStarnueBar() : Sigma2qqbar2lStarlBar(12) {}
  };
  class Sigma2qqbar2numuStarnumuBar: public Sigma2qqbar2lStarlBar {
  public:
    Sigma2qqbar2numuStarnumuBar() : Sigma2qqbar2lStarlBar(14) {}
  };
  class Sigma2qqbar2nutauStarnutauBar: public Sigma2qqbar2lStarlBar {
  public:
    Sigma2qqbar2nutauStarnutauBar() : Sigma2qqbar2lStarlBar(16) {}
  };
  class Sigma2qqbar2eStareBar: public Sigma2qqbar2lStarlBar {
  public:
    Sigma2qqbar2eStareBar() : Sigma2qqbar2lStarlBar(11) {}
  };
  class Sigma2qqbar2muStarmuBar: public Sigma2qqbar2lStarlBar {
  public:
    Sigma2qqbar2muStarmuBar() : Sigma2qqbar2lStarlBar(13) {}
  };
  class Sigma2qqbar2tauStartauBar: public Sigma2qqbar2lStarlBar {
  public:
    Sigma2qqbar2tauStartauBar() : Sigma2qqbar2lStarlBar(15) {}
  };
  
}
