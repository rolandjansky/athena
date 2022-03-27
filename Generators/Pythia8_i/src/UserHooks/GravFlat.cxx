/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

#include "UserSetting.h"
#include "Pythia8_i/UserHooksFactory.h"
#include "Pythia8/PhaseSpace.h"

#include <stdexcept>

namespace Pythia8 {
  class GravFlat;
}

Pythia8_UserHooks::UserHooksFactory::Creator<Pythia8::GravFlat> gravFlatCreator("GravFlat");


namespace Pythia8 {


  class GravFlat : public UserHooks {
  public:

    // Constructor.
    GravFlat()
      : m_energyMode("GravFlat:EnergyMode", 8),
        m_flatpT("GravFlat:FlatPt", false)
    {  }

    // Destructor.
    //~GravFlat() { }

    // Allow process cross section to be modified...
    virtual bool canModifySigma() { return true; }

    // ...which gives access to the event at the trial level, before selection.
    virtual double multiplySigmaBy(const SigmaProcess* sigmaProcessPtr, const PhaseSpace* phaseSpacePtr, bool /* inEvent */) {

      // All events should be 2 -> 1, but kill them if not.
      if (sigmaProcessPtr->nFinal() != 1) return 0.;

      // Weight cross section with BW propagator, i.e. to remove it.
      // (inEvent = false for initialization).
      // No inEvent criteria, want weight both for cross section
      // and MC generation.
      const int idRes   = sigmaProcessPtr->resonanceA();
      const double mRes = particleDataPtr->m0(idRes);
      const double wRes = particleDataPtr->mWidth(idRes);
      const double m2Res   = mRes*mRes;
      const double GamMRat = wRes/mRes;
      const double sHat = phaseSpacePtr->sHat();
      const double rH = std::sqrt(sHat);
      const double weightBW = pow2(sHat - m2Res) + pow2(sHat * GamMRat);

      // Calculate weight depending on energy mode, sHat, and flat-pT mode
      double weight = 1;
      double weightPL = 1;
      double weightpT_G = 1;

      const int emode = m_energyMode(settingsPtr);
      switch (emode) {
      case 8:

        if (rH < 50) {
          // do nothing
        }
        else if (rH < 2000) {
          // Completely Flat M=2 TeV, Above 0.05 TeV, Below 2 TeV
          // 1e5
          const double p0 = 12.4176;
          const double p1 = -0.364611;
          const double p2 = 0.00293827;
          const double p3 = 4.3106e-07;
          const double p4 = -2.61662e-09;
          const double p5 = 1.33037e-12;
          const double p6 = -2.07848e-16;
          weightPL = 1/(p0+(p1*rH)+(p2*std::pow(rH,2))+(p3*std::pow(rH,3))+(p4*std::pow(rH,4))+(p5*std::pow(rH,5))+(p6*std::pow(rH,6)));
        }
        else if (rH < 4000) {
          // Completely Flat M=2 TeV, Above 2 TeV, Below 4 TeV
          // 1e5
          const double p0 = 41237.9;
          const double p1 = -95.9745;
          const double p2 = 0.0979667;
          const double p3 = -5.14644e-05;
          const double p4 = 1.45857e-08;
          const double p5 = -2.13169e-12;
          const double p6 = 1.26473e-16;
          weightPL = 1/(p0+(p1*rH)+(p2*std::pow(rH,2))+(p3*std::pow(rH,3))+(p4*std::pow(rH,4))+(p5*std::pow(rH,5))+(p6*std::pow(rH,6)));
        }
        else if (rH < 5500) {
          // Completely Flat M=2 TeV, Above 4 TeV, Below 5.5 TeV
          // 1e5
          const double p0 = 1.11676e+06;
          const double p1 = -925.647;
          const double p2 = 0.311595;
          const double p3 = -5.38465e-05;
          const double p4 = 4.92286e-09;
          const double p5 = -2.1452e-13;
          const double p6 = 2.97112e-18;
          weightPL = 1/(p0+(p1*rH)+(p2*std::pow(rH,2))+(p3*std::pow(rH,3))+(p4*std::pow(rH,4))+(p5*std::pow(rH,5))+(p6*std::pow(rH,6)));
        }
        else if (rH <= 6500) {
          // Completely Flat M=2 TeV, Above 5.5 TeV, Below 6.5 TeV
          // 1e5
          const double p0 = 9.70964e+13;
          const double p1 = -4.17142e-03;
          const double p2 = -3.06415e-03;
          weightPL = 1/((p0*std::exp(p1*rH))+(p2*rH));
        }
        weight = weightBW * weightPL;
        break;


      case 13:

        if (m_flatpT(settingsPtr)) {// in flat mode

          if (rH < 4000) {
            const double p0 = 0.00384785;
            const double p1 = -1.72701e-05;
            const double p2 = 2.25365e-08;
            const double p3 = -6.15774e-12;
            const double p4 = 4.85585e-16;
            weightpT_G = 1/(p0+p1*std::pow(rH,1)+p2*std::pow(rH,2)+p3*std::pow(rH,3)+p4*std::pow(rH,4));
          }
          else if (rH < 6000) {
            const double p10 = 0.00659488 ;
            const double p11 = 2.68603e-05;
            const double p12 = -7.80009e-09;
            const double p13 = 5.54463e-13;
            weightpT_G = 1/(p10+p11*std::pow(rH,1)+p12*std::pow(rH,2)+p13*std::pow(rH,3));
          }
          else if (rH < 13000) {
            weightpT_G = 1./exp(2.14289e+00 - 1.17687e-03*rH);
          } else {
            weightpT_G = 0;
          }
          weight = weightpT_G * (1 + rH/13000.);

        } else { // not in flat mode

          if (rH < 50) {
            // do nothing
          } else if (rH < 2500) {
            // Completely flat M=5 TeV, k/M=0.1; above 0.05 TeV, below 2.5 TeV
            // 8e5
            const double
              p0 = 1.06963e+01,
              p1 = -1.86534e-01,
              p2 = 9.00278e-04,
              p3 = 1.01576e-06,
              p4 = -1.29332e-09,
              p5 = 4.64752e-13,
              p6 = -5.68297e-17;
            weightPL = 1./(p0+(p1*rH)+(p2*std::pow(rH,2))+(p3*std::pow(rH,3))+(p4*std::pow(rH,4))+(p5*std::pow(rH,5))+(p6*std::pow(rH,6)));
          }
          else if (rH < 6000) {
            // Completely flat M=5 TeV, k/M=0.1; above 2.5 TeV, below 6.0 TeV
            // 8e5
            const double
              p0 = -1.77843e+03,
              p1 = 3.70114e+00,
              p2 = -1.06644e-03,
              p3 = 3.77245e-08,
              p4 = 2.49922e-11,
              p5 = -3.96985e-15,
              p6 = 1.88504e-19;
            weightPL = 1/(p0+(p1*rH)+(p2*std::pow(rH,2))+(p3*std::pow(rH,3))+(p4*std::pow(rH,4))+(p5*std::pow(rH,5))+(p6*std::pow(rH,6)));
          }
          else if (rH < 8000) {
            // Completely flat M=5 TeV, k/M=0.1; above 6.0 TeV, below 8.0 TeV
            // 8e5
            const double
              p0 = 1.43663e+03,
              p1 = 3.40467e-01,
              p2 = -7.44453e-05,
              p3 = -1.08578e-08,
              p4 = 6.74486e-13,
              p5 = 2.82952e-16,
              p6 = -2.20149e-20;
            weightPL = 1/(p0+(p1*rH)+(p2*std::pow(rH,2))+(p3*std::pow(rH,3))+(p4*std::pow(rH,4))+(p5*std::pow(rH,5))+(p6*std::pow(rH,6)));
          }
          else if (rH <= 10500) {
            // Completely flat M=5 TeV, k/M=0.1; above 8.0 TeV, below 10.5 TeV
            // 8e5
            const double
              p0 = 1.21245e+03,
              p1 = -1.08389e-01,
              p2 = -1.02834e-05,
              p3 = 4.11376e-12,
              p4 = 8.55312e-14,
              p5 = 6.98307e-18,
              p6 = -6.52683e-22;
            weightPL = 1/(p0+(p1*rH)+(p2*std::pow(rH,2))+(p3*std::pow(rH,3))+(p4*std::pow(rH,4))+(p5*std::pow(rH,5))+(p6*std::pow(rH,6)));
          }
          weight = weightBW * weightPL;
        }
        break;


      default:
        throw std::runtime_error("Unknown GravFlat:EnergyMode = " + std::to_string(emode) + " (should be either 8 or 13!)");
      }

      return weight;
    }


  private:

    /// User-settable mode to set the collision energy (default is 8)
    Pythia8_UserHooks::UserSetting<int> m_energyMode;
    /// User-settable flag to get a flat pT distribution
    Pythia8_UserHooks::UserSetting<bool> m_flatpT;

  };


} // end namespace Pythia8
