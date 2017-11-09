// The ResonanceLQ class modifies the default Pythia8 setup 
// to allow leptoquarks to decay to top quarks
// This is a copy of existing pythia ResonanceLeptoquark class
//
// author Katharine Leney
// based on ResonanceExcitedCI (Olya Igonkina and James Monk)

#include "Pythia8_i/UserResonanceFactory.h"
#include "Pythia8/ParticleData.h"

namespace Pythia8{
  class ResonanceLQ;
}

Pythia8_UserResonance::UserResonanceFactory::Creator<Pythia8::ResonanceLQ> resonanceLeptoquarkCreator("LQ");

namespace Pythia8{
  
  class ResonanceLQ : public ResonanceWidths {
    
  public:
    
    // Constructor. 
    ResonanceLQ(int idResIn): 
      m_kCoup(0.)
    {    
      initBasic(idResIn);
      std::cout << " ResonanceLQ constructor\n"; 
    } 
  
  private:
    
    void initConstants() {
      
      // Locally stored properties and couplings.
      m_kCoup = settingsPtr->parm("LeptoQuark:kCoup");
      
      // Check that flavour info in decay channel is correctly set.
      int id1Now = particlePtr->channel(0).product(0);
      int id2Now = particlePtr->channel(0).product(1);
      
      // ============================================================
      // Modify standard Pythia8 setup to allow decays to top quarks
      // ============================================================
      
      if (id1Now < 1 || id1Now > 6) {
	std::cout << "ERROR in ResonanceLQ::init: unallowed input quark flavour reset to u" << std::endl; 
	id1Now   = 2;
	particlePtr->channel(0).product(0, id1Now);
      }
      if (abs(id2Now) < 11 || abs(id2Now) > 16) {
	std::cout << "ERROR in ResonanceLQ::init:unallowed input lepton flavour reset to e-" << std::endl; 
	id2Now   = 11;
	particlePtr->channel(0).product(1, id2Now);
      }
      
      // Set/overwrite charge and name of particle.
      bool changed  = particlePtr->hasChanged();
      
      int chargeLQ  = particleDataPtr->chargeType(id1Now) 
	+ particleDataPtr->chargeType(id2Now);
      
      particlePtr->setChargeType(chargeLQ); 
      
      string nameLQ = "LQ_" + particleDataPtr->name(id1Now) + ","
	+ particleDataPtr->name(id2Now);
      
      particlePtr->setNames(nameLQ, nameLQ + "bar"); 
      if (!changed) particlePtr->setHasChanged(false);
      
      return;
    }
    
    //--------------------------------------------------------------------------
    
    // Calculate various common prefactors for the current mass.
    
    void calcPreFac(bool) {

      alpEM   = couplingsPtr->alphaEM(mHat * mHat);
      preFac  = 0.25 * alpEM * m_kCoup * mHat; 
      
      return;
    }
    
    //--------------------------------------------------------------------------
    
    // Calculate width for currently considered channel.
    
    void calcWidth(bool) {
      
      // Check that above threshold.
      if (ps == 0.) return;
      
      // Width into lepton plus quark.
      if (id1Abs > 10 && id1Abs < 17 && id2Abs < 7) widNow = preFac * pow3(ps);
      return;
    }
    
    // Locally stored properties and couplings.
    //double m_lambda, m_coupF, m_coupFprime, m_coupFcol, m_sin2tW, m_cos2tW;
    double m_kCoup;
  };
}
