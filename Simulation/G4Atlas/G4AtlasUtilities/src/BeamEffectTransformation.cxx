/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "G4AtlasUtilities/BeamEffectTransformation.h"
#include "FadsKinematics/RandomNrCenter.h"
#include "FadsKinematics/GeneratorCenter.h"

#include "GaudiKernel/Bootstrap.h"
#include "GaudiKernel/ISvcLocator.h"
#include "AthenaKernel/IAtRndmGenSvc.h"
#include "InDetBeamSpotService/IBeamCondSvc.h"

#include "CLHEP/Geometry/Point3D.h"
#include "CLHEP/Geometry/Transform3D.h"
#include "AtlasCLHEP_RandomGenerators/RandGaussZiggurat.h"

#include "G4PrimaryParticle.hh"
#include "G4Proton.hh"

#include <iostream>
#include <cmath>

using namespace std;

// See https://twiki.cern.ch/twiki/bin/viewauth/Atlas/AtlasG4EventFilters for more details.

void BeamEffectTransformation::EventInitialization() {
  if (!onSwitch) return;
  /// @TODO Replace these placeholder beam transformation parameters using the BeamCondSvc
  const double mp = G4Proton::Proton()->GetPDGMass(); /// @todo Proton mass: generalise to non-proton beam particles

  // First check that the necessary Athena services have been retrieved
  if (!p_beamcondsvc) {
    ISvcLocator* svcLocator = Gaudi::svcLocator();
    StatusCode sc = svcLocator->service("BeamCondSvc", p_beamcondsvc);
    if (sc.isFailure()) {
      cout << "Error: unable to get BeamCondSvc!!" << endl;
    }
  }
  if (!p_randomengine) {
    ISvcLocator* svcLocator = Gaudi::svcLocator();
    IAtRndmGenSvc* atrndmgensvc;
    std::string randomNumberSvcName(FADS::GeneratorCenter::GetGeneratorCenter()->RandomNumberService());
    if (svcLocator->service(randomNumberSvcName, atrndmgensvc).isFailure()) {
      cerr << "BeamEffectTransformation::EditParticle: Error: unable to get " << randomNumberSvcName << "!!" << std::endl;
    }
    p_randomengine = atrndmgensvc->GetEngine("BEAM");
    if (!p_randomengine) {
      cerr << "BeamEffectTransformation::EditParticle: Error: unable to get the BEAM random number engine!!" << endl;
    }
  }

  // Reset the transformation
  m_trf = CLHEP::HepLorentzRotation();

  // Create beam 1 and 2 momenta, including divergence and crossing-angle effects
  CLHEP::RandGaussZiggurat gauss(*p_randomengine);
  const double px1 = m_pbeam1 * gauss.fire(m_xing_x_b2, m_sigma_px_b2);
  const double py1 = m_pbeam1 * gauss.fire(m_xing_y_b2, m_sigma_py_b2);
  const double pz1 = gauss.fire(sqrt(m_pbeam1*m_pbeam1 - px1*px1 - py1*py1), m_dE);
  const double e1 = sqrt(px1*px1 + py1*py1 + pz1*pz1 + mp*mp);
  CLHEP::HepLorentzVector pp1(px1, py1, pz1, e1);
  const double px2 = -m_pbeam2 * gauss.fire(m_xing_x_b1, m_sigma_px_b1); // crossing angle & divergence
  const double py2 = -m_pbeam2 * gauss.fire(m_xing_y_b1, m_sigma_py_b1);
  const double pz2 = gauss.fire(-sqrt(m_pbeam2*m_pbeam2 - px2*px2 - py2*py2), m_dE); // longitudinal component in + direction energy smeared
  const double e2 = sqrt(px2*px2 + py2*py2 + pz2*pz2 + mp*mp);
  CLHEP::HepLorentzVector pp2(px2, py2, pz2, e2);

  // Now set-up rotation-boost matrix
  const CLHEP::HepLorentzVector psum = pp1 + pp2;
  const CLHEP::HepLorentzVector dir = pp1;
  // Boost psum back on the direction of dir, adapted from bstback & fromCMframe PYTHIA8, credit to T.Sjostrand
  const double betaX = -psum.x() / psum.t();
  const double betaY = -psum.y() / psum.t();
  const double betaZ = -psum.z() / psum.t();
  const double beta2 = betaX*betaX + betaY*betaY + betaZ*betaZ;
  const double gamma = 1. / sqrt(1. - beta2);
  const double prod1 = betaX * dir.x() + betaY * dir.y() + betaZ * dir.z();
  const double prod2 = gamma * (gamma * prod1 / (1. + gamma) + dir.t());
  // Get the angle of rotation
  const CLHEP::HepLorentzVector back(dir.x() + prod2 * betaX,
                                     dir.y() + prod2 * betaY,
                                     dir.z() + prod2 * betaZ,
                                     gamma * (dir.t() + prod1));
  const double thback = back.theta();
  const double phback = back.phi();
  // Setting up two rotation matrices, via 3x3 rep. rather than even more messy EulerAngles
  const double sph = sin(phback), cph = cos(phback), sth = sin(thback), cth = cos(thback);
  const CLHEP::HepRotation rot1(CLHEP::HepRep3x3(cph, sph, 0.0, -sph, cph, 0.0, 0.0, 0.0, 1.0));
  const CLHEP::HepRotation rot2(CLHEP::HepRep3x3(cth*cph, -sph, sth*cph, cth*sph, cph, sth*sph, -sth, 0.0, cth));
  const CLHEP::HepBoost bst(psum.px()/psum.e(), psum.py()/psum.e(), psum.pz()/psum.e());
  // Combine the two rotations and the boost; matrix multiplication order matters
  const CLHEP::HepRotation rot = rot2*rot1;
  m_trf.set(rot,bst);
}


bool BeamEffectTransformation::EditParticle(G4PrimaryParticle* p) {
  if (!onSwitch) return true;
  if (!p_randomengine || !p_beamcondsvc) {
    return false;
  }

  // Apply the same transformation for EVERY G4PrimaryParticle
  CLHEP::Hep3Vector p3 = p->GetMomentum();
  const double pmass = p->GetMass();
  const double pe = sqrt(p3.mag2() + pmass*pmass);
  CLHEP::HepLorentzVector hv(p3, pe);
  hv.transform(m_trf);
  p->Set4Momentum(hv.px(), hv.py(), hv.pz(), hv.e());

  if (verboseLevel > 0) {
    cout << "BEAMBOOST initial momentum " << p3 << " energy " << pe << endl;
    // cout << "BEAMBOOST proton 1 momentum " << pp1 << endl;
    // cout << "BEAMBOOST proton 2 momentum " << pp2 << endl;
    cout << "BEAMBOOST transformed momentum " << hv << endl;
  }

  return true;
}
