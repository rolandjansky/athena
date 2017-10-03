// Double excited lepton production

#include "Pythia8_i/UserProcessFactory.h"

#include <iostream>
#include <vector>

namespace Pythia8 {
  class Sigma2qqbar2lStarlStarBar;
  class Sigma2qqbar2nueStarnueStarBar;    
  class Sigma2qqbar2numuStarnumuStarBar;  
  class Sigma2qqbar2nutauStarnutauStarBar;
  class Sigma2qqbar2eStareStarBar;        
  class Sigma2qqbar2muStarmuStarBar;      
  class Sigma2qqbar2tauStartauStarBar;    
}

// This one line registers this process with the factory
// An instance of this process can be created with 
// UserProcessFactory::create("qqbar2lStarlStarBar")
Pythia8_UserProcess::UserProcessFactory::Creator<Pythia8::Sigma2qqbar2nueStarnueStarBar>     qqbar2nueStarlnuetarBarCreator(    "qqbar2nueStarnueStarBar");
Pythia8_UserProcess::UserProcessFactory::Creator<Pythia8::Sigma2qqbar2numuStarnumuStarBar>   qqbar2numuStarlnumutarBarCreator(  "qqbar2numuStarnumuStarBar");
Pythia8_UserProcess::UserProcessFactory::Creator<Pythia8::Sigma2qqbar2nutauStarnutauStarBar> qqbar2nutauStarlnutautarBarCreator("qqbar2nutauStarnutauStarBar");
Pythia8_UserProcess::UserProcessFactory::Creator<Pythia8::Sigma2qqbar2eStareStarBar>         qqbar2eStarletarBarCreator(        "qqbar2eStareStarBar");
Pythia8_UserProcess::UserProcessFactory::Creator<Pythia8::Sigma2qqbar2muStarmuStarBar>       qqbar2muStarlmutarBarCreator(      "qqbar2muStarmuStarBar");
Pythia8_UserProcess::UserProcessFactory::Creator<Pythia8::Sigma2qqbar2tauStartauStarBar>     qqbar2tauStarltautarBarCreator(     "qqbar2tauStartauStarBar");

namespace Pythia8 {
  
  //**************************************************************************
  
  // Sigma2qqbar2lStarlStarBar class.
  // Cross section for q qbar -> lStar lStarBar
  
  // A derived class for q qbar -> lStar lStarBar
  
  class Sigma2qqbar2lStarlStarBar: public Sigma2Process {
    
  public:
    //Constructor.
    Sigma2qqbar2lStarlStarBar(int idlIn) : idl(idlIn),
    idRes(0), codeSave(0),
    Lambda(0.), preFac(0.), openFracPos(0.), openFracNeg(0.), sigma(0.),
    m2ResTimes4(0.){};
    
    // Initialize process. 
    
    void initProc() {
      
      // Set up process properties from the chosen lepton flavour.
      idRes         = 4000000 + idl;
      codeSave      = 4040 + idl;
      if      (idl == 11) nameSave = "q qbar -> e^*+- e^*-+";
      else if (idl == 12) nameSave = "q qbar -> nu_e^* nu_e^*bar"; 
      else if (idl == 13) nameSave = "q qbar -> mu^*+- mu^*-+"; 
      else if (idl == 14) nameSave = "q qbar -> nu_mu^* nu_mu^*bar"; 
      else if (idl == 15) nameSave = "q qbar -> tau^*+- tau^*-+"; 
      else                nameSave = "q qbar -> nu_tau^* nu_tau^*bar";
      
      // Secondary open width fractions.
      openFracPos = particleDataPtr->resOpenFrac( idRes);
      openFracNeg = particleDataPtr->resOpenFrac(-idRes);
      
      // Locally stored properties and couplings.
      Lambda        = settingsPtr->parm("ExcitedFermion:Lambda");
      preFac        = (M_PI / pow4(Lambda)) * (openFracPos + openFracNeg) / 9.; 
      
      double mRes          = particleDataPtr->m0(idRes);
      m2ResTimes4         = mRes*mRes*4.;
    } 
    
    //*********
    
    // Evaluate sigmaHat(sHat). 
    virtual double sigmaHat() {return sigma;}
    
    // Evaluate sigmaHat(sHat), part independent of incoming flavour. 
    
    void sigmaKin() {
      
      // use assumption that sigma is multiplied later with sH*(1-s3/sH)
      
      //sigma = preFac * sqrtpos( 1.- m2ResTimes4/sH );  
      sigma = preFac * sqrtpos( 1.- 4.*s3/sH );  
      
    }
    
    //*********
    
    // Select identity, colour and anticolour.
    
    void setIdColAcol() {
      
      // Flavours: both lepton and antilepton are excited.
      setId( id1, id2, idRes, -idRes);
      //if (id1 < 0) swapTU = true;   // OI how is this used?
      
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
    virtual int    id4Mass()    const {return abs(idRes);}
    
    virtual bool   convertM2()       const {return false;} //true is only suitable for resonance production pp->A->ll
    virtual bool   convert2mb()      const {return true;}
    
    // Parameters set at initialization or for current kinematics. 
    int    idl, idRes, codeSave;
    string nameSave;
    double Lambda, preFac, openFracPos, openFracNeg, sigma;
    double m2ResTimes4;
  };
  
  
  //**************************************************************************
  class Sigma2qqbar2nueStarnueStarBar: public Sigma2qqbar2lStarlStarBar {
  public:
    Sigma2qqbar2nueStarnueStarBar() : Sigma2qqbar2lStarlStarBar(12) {}
  };
  class Sigma2qqbar2numuStarnumuStarBar: public Sigma2qqbar2lStarlStarBar {
  public:
    Sigma2qqbar2numuStarnumuStarBar() : Sigma2qqbar2lStarlStarBar(14) {}
  };
  class Sigma2qqbar2nutauStarnutauStarBar: public Sigma2qqbar2lStarlStarBar {
  public:
    Sigma2qqbar2nutauStarnutauStarBar() : Sigma2qqbar2lStarlStarBar(16) {}
  };
  class Sigma2qqbar2eStareStarBar: public Sigma2qqbar2lStarlStarBar {
  public:
    Sigma2qqbar2eStareStarBar() : Sigma2qqbar2lStarlStarBar(11) {}
  };
  class Sigma2qqbar2muStarmuStarBar: public Sigma2qqbar2lStarlStarBar {
  public:
    Sigma2qqbar2muStarmuStarBar() : Sigma2qqbar2lStarlStarBar(13) {}
  };
  
  class Sigma2qqbar2tauStartauStarBar: public Sigma2qqbar2lStarlStarBar {
  public:
    Sigma2qqbar2tauStartauStarBar() : Sigma2qqbar2lStarlStarBar(15) {}
  };
  
}
