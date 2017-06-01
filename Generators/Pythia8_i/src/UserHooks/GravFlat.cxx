#include "UserSetting.h"
#include "Pythia8_i/UserHooksFactory.h"
#include "Pythia8/PhaseSpace.h"

#include <stdexcept>

namespace Pythia8{
  class GravFlat;
}

Pythia8_UserHooks::UserHooksFactory::Creator<Pythia8::GravFlat> gravFlatCreator("GravFlat");

namespace Pythia8 {
  
  class GravFlat : public UserHooks {
    
  public:
    
    // Constructor.
    GravFlat(): m_energyMode("GravFlat:EnergyMode", 8){}
    
    // Destructor.
    ~GravFlat(){}
    
    // Allow process cross section to be modified...
    virtual bool canModifySigma() {return true;}
    
    // ...which gives access to the event at the trial level, before selection.
    virtual double multiplySigmaBy(const SigmaProcess* sigmaProcessPtr,
                                   const PhaseSpace* phaseSpacePtr,
                                   bool /* inEvent */) {
      // All events should be 2 -> 1, but kill them if not.
      if (sigmaProcessPtr->nFinal() != 1) return 0.;
      
      // Weight cross section with BW propagator, i.e. to remove it.
      // (inEvent = false for initialization).
      // No inEvent criteria, want weight both for cross section
      // and MC generation.
      int idRes   = sigmaProcessPtr->resonanceA();
      double mRes = particleDataPtr->m0(idRes);
      double wRes = particleDataPtr->mWidth(idRes);
      double m2Res   = mRes*mRes;
      double GamMRat = wRes/mRes;
      double sHat = phaseSpacePtr->sHat();
      
      double weightBW = pow2(sHat - m2Res) + pow2(sHat * GamMRat);
      
      double weightPL = 1;
      double rH = sqrt(sHat);
      
      double weight=1;
      switch(m_energyMode(settingsPtr)){
        case 8:
          
          if (rH >= 50 && rH < 2000){
            
            // Completely Flat M=2 TeV, Above 0.05 TeV, Below 2 TeV
            // 1e5
            double p0 = 12.4176;
            double p1 = -0.364611;
            double p2 = 0.00293827;
            double p3 = 4.3106e-07;
            double p4 = -2.61662e-09;
            double p5 = 1.33037e-12;
            double p6 = -2.07848e-16;
            
            weightPL = 1/(p0+(p1*rH)+(p2*pow(rH,2))+(p3*pow(rH,3))+(p4*pow(rH,4))+(p5*pow(rH,5))+(p6*pow(rH,6)));
            
          }
          else if (rH >= 2000 && rH < 4000){
            
            // Completely Flat M=2 TeV, Above 2 TeV, Below 4 TeV
            // 1e5
            double p0 = 41237.9;
            double p1 = -95.9745;
            double p2 = 0.0979667;
            double p3 = -5.14644e-05;
            double p4 = 1.45857e-08;
            double p5 = -2.13169e-12;
            double p6 = 1.26473e-16;
            
            weightPL = 1/(p0+(p1*rH)+(p2*pow(rH,2))+(p3*pow(rH,3))+(p4*pow(rH,4))+(p5*pow(rH,5))+(p6*pow(rH,6)));
            
          }
          else if (rH >= 4000 && rH < 5500){
            
            // Completely Flat M=2 TeV, Above 4 TeV, Below 5.5 TeV
            // 1e5
            double p0 = 1.11676e+06;
            double p1 = -925.647;
            double p2 = 0.311595;
            double p3 = -5.38465e-05;
            double p4 = 4.92286e-09;
            double p5 = -2.1452e-13;
            double p6 = 2.97112e-18;
            
            weightPL = 1/(p0+(p1*rH)+(p2*pow(rH,2))+(p3*pow(rH,3))+(p4*pow(rH,4))+(p5*pow(rH,5))+(p6*pow(rH,6)));
            
          }
          else if (rH >= 5500 && rH <= 6500){
            
            // Completely Flat M=2 TeV, Above 5.5 TeV, Below 6.5 TeV
            // 1e5
            double p0 = 9.70964e+13;
            double p1 = -4.17142e-03;
            double p2 = -3.06415e-03;
            
            weightPL = 1/((p0*exp(p1*rH))+(p2*rH));
            
          }
          
          weight = weightBW * weightPL;
          return weight;
          
        case 13:
          
          if (rH >= 50 && rH < 2500){
            
            // Completely flat M=5 TeV, k/M=0.1; above 0.05 TeV, below 2.5 TeV
            // 8e5
            double p0 = 1.06963e+01;
            double p1 = -1.86534e-01;
            double p2 = 9.00278e-04;
            double p3 = 1.01576e-06;
            double p4 = -1.29332e-09;
            double p5 = 4.64752e-13;
            double p6 = -5.68297e-17;
            
            weightPL = 1/(p0+(p1*rH)+(p2*pow(rH,2))+(p3*pow(rH,3))+(p4*pow(rH,4))+(p5*pow(rH,5))+(p6*pow(rH,6)));
            
          }
          else if (rH >= 2500 && rH < 6000){
            
            // Completely flat M=5 TeV, k/M=0.1; above 2.5 TeV, below 6.0 TeV
            // 8e5
            double p0 = -1.77843e+03;
            double p1 = 3.70114e+00;
            double p2 = -1.06644e-03;
            double p3 = 3.77245e-08;
            double p4 = 2.49922e-11;
            double p5 = -3.96985e-15;
            double p6 = 1.88504e-19;
            
            weightPL = 1/(p0+(p1*rH)+(p2*pow(rH,2))+(p3*pow(rH,3))+(p4*pow(rH,4))+(p5*pow(rH,5))+(p6*pow(rH,6)));
            
          }
          else if (rH >= 6000 && rH < 8000){
            
            // Completely flat M=5 TeV, k/M=0.1; above 6.0 TeV, below 8.0 TeV
            // 8e5
            double p0 = 1.43663e+03;
            double p1 = 3.40467e-01;
            double p2 = -7.44453e-05;
            double p3 = -1.08578e-08;
            double p4 = 6.74486e-13;
            double p5 = 2.82952e-16;
            double p6 = -2.20149e-20;
            
            weightPL = 1/(p0+(p1*rH)+(p2*pow(rH,2))+(p3*pow(rH,3))+(p4*pow(rH,4))+(p5*pow(rH,5))+(p6*pow(rH,6)));
            
          }
          else if (rH >= 8000 && rH <= 10500){
            
            // Completely flat M=5 TeV, k/M=0.1; above 8.0 TeV, below 10.5 TeV
            // 8e5
            double p0 = 1.21245e+03;
            double p1 = -1.08389e-01;
            double p2 = -1.02834e-05;
            double p3 = 4.11376e-12;
            double p4 = 8.55312e-14;
            double p5 = 6.98307e-18;
            double p6 = -6.52683e-22;
            
            weightPL = 1/(p0+(p1*rH)+(p2*pow(rH,2))+(p3*pow(rH,3))+(p4*pow(rH,4))+(p5*pow(rH,5))+(p6*pow(rH,6)));
            
          }
          
          weight = weightBW * weightPL;
          return weight;
          
        default:
          
          throw std::runtime_error("Unknown GravFlat:EnergyMode - should be either 8 or 13!");
          
      }
    }
    
  private:
    
    /// User-settable mode to set the collision energy (default is 8)
    Pythia8_UserHooks::UserSetting<int> m_energyMode;
    
  };  
  
} // end namespace Pythia8
