/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// -*-C++-*-
#ifndef BeamEffectTransformation_H
#define BeamEffectTransformation_H

#include <string>
#include "FadsKinematics/ParticleManipulator.h"
#include "CLHEP/Geometry/Transform3D.h"
#include "CLHEP/Vector/LorentzRotation.h"
namespace CLHEP { class HepRandomEngine; }
class IBeamCondSvc;


/// @brief Boost primary event particles to account for beam effects.
///
/// This class applies a Lorentz transformation to the momentum vectors of a
/// primary event to account for effect of the incoming beams, such as crossing
/// angle, beam divergence, and beam energy smearing.
class BeamEffectTransformation : public FADS::ParticleManipulator {
public:

  BeamEffectTransformation(const std::string& s)
    : ParticleManipulator(s)
    , p_beamcondsvc(0)
    , p_randomengine(0)
    , m_sigma_px_b1 (20.E-6) // angular divergence in x of beam 1 [rad]
    , m_sigma_px_b2 (21.E-6) // angular divergence in x of beam 2 [rad]
    , m_sigma_py_b1 (22.E-6) // angular divergence in y of beam 1 [rad]
    , m_sigma_py_b2 (23.E-6) // angular divergence in y of beam 2 [rad]
  // Crossing angles, note the convention here is taken from online https://atlas-dcs.cern.ch/index.php?page=LHC_INS::INS_BPM
    , m_xing_x_b1 (19.E-6)   // half-crossing in x for beam 1 at IP1, i.e. angle between beam 1 and z-axis
    , m_xing_x_b2 (1.E-6)    // half-crossing in x for beam 2 at IP1, i.e. angle between beam 2 and z-axis
    , m_xing_y_b1 ( 150.E-6)  // half-crossing in y for beam 1 at IP1, i.e. angle between beam 1 and z-axis
    , m_xing_y_b2 (-152.E-6) // half-crossing in y for beam 2 at IP1, i.e. angle between beam 2 and z-axis
    , m_dE (1.1E-4) // Delta_E/E theoretical value
    , m_pbeam1 (3500.0E3) /// @todo Get from a service
    , m_pbeam2 (3500.0E3) /// @todo Get from a service
  {  }

  void EventInitialization();

  bool EditParticle(G4PrimaryParticle*);

  void Set_sigma_px_b1(const double c) { m_sigma_px_b1=c; } // angular divergence in x of beam 1 [rad]
  void Set_sigma_px_b2(const double c) { m_sigma_px_b2=c; } // angular divergence in x of beam 2 [rad]
  void Set_sigma_py_b1(const double c) { m_sigma_py_b1=c; } // angular divergence in y of beam 1 [rad]
  void Set_sigma_py_b2(const double c) { m_sigma_py_b2=c; } // angular divergence in y of beam 2 [rad]
  // Crossing angles, note the convention here is taken from online https://atlas-dcs.cern.ch/index.php?page=LHC_INS::INS_BPM
  void Set_xing_x_b1(const double c) { m_xing_x_b1=c; } // half-crossing in x for beam 1 at IP1, i.e. angle between beam 1 and z-axis
  void Set_xing_x_b2(const double c) { m_xing_x_b2=c; } // half-crossing in x for beam 2 at IP1, i.e. angle between beam 2 and z-axis
  void Set_xing_y_b1(const double c) { m_xing_y_b1=c; } // half-crossing in y for beam 1 at IP1, i.e. angle between beam 1 and z-axis
  void Set_xing_y_b2(const double c) { m_xing_y_b2=c; } // half-crossing in y for beam 2 at IP1, i.e. angle between beam 2 and z-axis

  void Set_dE(const double c) { m_dE=c; } // Delta_E/E theoretical value
  void Set_pbeam1(const double c) { m_pbeam1=c; } /// @todo Get from a service
  void Set_pbeam2(const double c) { m_pbeam2=c; } /// @todo Get from a service


  double Get_sigma_px_b1() const { return m_sigma_px_b1; } // angular divergence in x of beam 1 [rad]
  double Get_sigma_px_b2() const { return m_sigma_px_b2; } // angular divergence in x of beam 2 [rad]
  double Get_sigma_py_b1() const { return m_sigma_py_b1; } // angular divergence in y of beam 1 [rad]
  double Get_sigma_py_b2() const { return m_sigma_py_b2; } // angular divergence in y of beam 2 [rad]
  // Crossing angles, note the convention here is taken from online https://atlas-dcs.cern.ch/index.php?page=LHC_INS::INS_BPM
  double Get_xing_x_b1() const { return m_xing_x_b1; } // half-crossing in x for beam 1 at IP1, i.e. angle between beam 1 and z-axis
  double Get_xing_x_b2() const { return m_xing_x_b2; } // half-crossing in x for beam 2 at IP1, i.e. angle between beam 2 and z-axis
  double Get_xing_y_b1() const { return m_xing_y_b1; } // half-crossing in y for beam 1 at IP1, i.e. angle between beam 1 and z-axis
  double Get_xing_y_b2() const { return m_xing_y_b2; } // half-crossing in y for beam 2 at IP1, i.e. angle between beam 2 and z-axis

  double Get_dE() const { return m_dE; } // Delta_E/E theoretical value
  double Get_pbeam1() const { return m_pbeam1; } /// @todo Get from a service
  double Get_pbeam2() const { return m_pbeam2; } /// @todo Get from a service

private:

  IBeamCondSvc* p_beamcondsvc;
  CLHEP::HepRandomEngine* p_randomengine;

  CLHEP::HepLorentzRotation m_trf;

  double m_sigma_px_b1; // angular divergence in x of beam 1 [rad]
  double m_sigma_px_b2; // angular divergence in x of beam 2 [rad]
  double m_sigma_py_b1; // angular divergence in y of beam 1 [rad]
  double m_sigma_py_b2; // angular divergence in y of beam 2 [rad]
  // Crossing angles, note the convention here is taken from online https://atlas-dcs.cern.ch/index.php?page=LHC_INS::INS_BPM
  double m_xing_x_b1;   // half-crossing in x for beam 1 at IP1, i.e. angle between beam 1 and z-axis
  double m_xing_x_b2;    // half-crossing in x for beam 2 at IP1, i.e. angle between beam 2 and z-axis
  double m_xing_y_b1;  // half-crossing in y for beam 1 at IP1, i.e. angle between beam 1 and z-axis
  double m_xing_y_b2; // half-crossing in y for beam 2 at IP1, i.e. angle between beam 2 and z-axis

  double m_dE; // Delta_E/E theoretical value
  double m_pbeam1; /// @todo Get from a service
  double m_pbeam2; /// @todo Get from a service

};

#endif
