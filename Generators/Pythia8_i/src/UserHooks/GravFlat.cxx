/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "Pythia8_i/UserHooksFactory.h"
#include "Pythia8/PhaseSpace.h"

namespace Pythia8{
  class GravFlat;
}

Pythia8_UserHooks::UserHooksFactory::Creator<Pythia8::GravFlat> gravFlatCreator("GravFlat");

namespace Pythia8 {

  class GravFlat : public UserHooks {
    
  public:
    
    // Constructor.
    GravFlat(){}
    
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
  
      double weight = weightBW * weightPL;
    
      return weight;

    }
    
  };  

} // end namespace Pythia8
