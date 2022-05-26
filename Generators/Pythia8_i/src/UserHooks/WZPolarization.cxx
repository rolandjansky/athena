/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/
#include "Pythia8_i/UserHooksFactory.h"
#include "UserHooksUtils.h"
#include "UserSetting.h"
#include "Pythia8/PhaseSpace.h"

namespace Pythia8 {


  /// Convert all resonant W/Z boson decays to the angular distribution of a pure polarisation state
  //
  // Polarisation state set via Main:spareMode1 = 0,1,2,3,4
  // for isotropic, longitudinal, transverse, and trans+, trans-.
  struct WZPolarization : public UserHooks {

    WZPolarization() : m_polmode("WZPolarization:mode", 1) { }

    bool canVetoResonanceDecays() { return true; }

    bool doVetoResonanceDecays(Event& process) {
      const int polmode = m_polmode(settingsPtr);

      for (int i = 1; i < process.size(); ++i) {
        // Select vector bosons
        Particle& v = process[i];
        if (v.id() != 23 && v.idAbs() != 24) continue;
        const Vec4 pv = v.p();

        // Find W/Z daughter particles
        Particle& d1 = process[v.daughter1()];
        Particle& d2 = process[v.daughter2()];

        // Get daughter momenta in the boson rest frame
        Vec4 pd1 = d1.p();
        Vec4 pd2 = d2.p();
        pd1.bstback(pv);
        pd2.bstback(pv);

        // Randomly reorient and rejection-sample decay configuration
        // (done inline here, rather than reboosting to the W/Z frame on each decay-hook iteration)
        while (true) {

          // Random reorientation
          const double dtheta = std::acos(2*rand01() - 1);
          const double dphi = 2*M_PI*rand01();
          pd1.rot(dtheta, dphi);
          pd2.rot(dtheta, dphi);

          // Angle w.r.t. W/Z flight direction, in rest-frame
          const double th = theta(pv, pd1);

          // Accept/reject the angle according to the polarisation being modelled
          if (polmode == 0) { // Isotropic
            break; // it's already been isotropically randomised
          } else if (polmode == 1) { // Longitudinal
            if (rand01() < sqr(std::sin(th))) break;
          } else if (polmode == 2) { // Trans-sum
            if (rand01() < (1 + sqr(std::cos(th)))/2) break;
          } else if (polmode == 3) { // Trans+
            if (rand01() < sqr(1 + std::cos(th))/4) break;
          } else if (polmode == 4) { // Trans-
            if (rand01() < sqr(1 - std::cos(th))/4) break;
          }
        }

        // Boost the vectors back to the lab frame
        pd1.bst(pv); d1.p(pd1);
        pd2.bst(pv); d2.p(pd2);

      }

      return false;
    }

    double sqr(double x) { return x*x; }

    double rand01() { return rndmPtr->flat(); }

    Pythia8_UserHooks::UserSetting<int> m_polmode;

  };


  Pythia8_UserHooks::UserHooksFactory::Creator<Pythia8::WZPolarization> WZPOL("WZPolarization");

}
