/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef LARG4FASTSIMULATION_FASTSHOWERCONFIGSTRUCT_H
#define LARG4FASTSIMULATION_FASTSHOWERCONFIGSTRUCT_H

#include <string>
#include "CLHEP/Units/SystemOfUnits.h"

struct FastShowerConfigStruct
{
  bool   m_e_FlagShowerLib;              //!< switch for electron frozen showers
  double m_e_MinEneShowerLib;            //!< lower energy limit for electron frozen showers
  double m_e_MaxEneShowerLib;            //!< upper energy limit for electron frozen showers

  bool   m_g_FlagShowerLib;              //!< switch for photon frozen showers
  double m_g_MaxEneShowerLib;            //!< lower energy limit for photon frozen showers
  double m_g_MinEneShowerLib;            //!< upper energy limit for photon frozen showers

  bool   m_Neut_FlagShowerLib;           //!< switch for neutron frozen showers
  double m_Neut_MinEneShowerLib;         //!< lower energy limit for neutron frozen showers
  double m_Neut_MaxEneShowerLib;         //!< upper energy limit for neutron frozen showers

  bool   m_Pion_FlagShowerLib;           //!< switch for pion frozen showers
  double m_Pion_MinEneShowerLib;         //!< lower energy limit for pion frozen showers
  double m_Pion_MaxEneShowerLib;         //!< upper energy limit for pion frozen showers

  bool   m_containLow;                   //!< switch for containment check at low eta
  double m_absLowEta;
  bool   m_containHigh;                  //!< switch for containment check at high eta
  double m_absHighEta;
  bool   m_containCrack;                 //!< switch for containment check in crack region
  double m_absCrackEta1;
  double m_absCrackEta2;
  std::string m_generated_starting_points_file;                 //!< switch for shower starting point record
  float       m_generated_starting_points_ratio;                //!< switch for shower starting point record
  int         m_detector_tag;                                   //!< name for the detector tag for the ShowerLibSvc
  std::string m_SensitiveDetectorName;
  std::string m_showerLibSvcName;
  FastShowerConfigStruct() :
    m_e_FlagShowerLib  (false),
    m_e_MinEneShowerLib(0.0*CLHEP::GeV),
    m_e_MaxEneShowerLib(1.0*CLHEP::GeV),

    m_g_FlagShowerLib (false),
    m_g_MaxEneShowerLib(0.0*CLHEP::GeV),
    m_g_MinEneShowerLib(1.0*CLHEP::GeV),

    m_Neut_FlagShowerLib  (false),
    m_Neut_MinEneShowerLib(0.0*CLHEP::GeV),
    m_Neut_MaxEneShowerLib(1.0*CLHEP::GeV),

    m_Pion_FlagShowerLib  (false),
    m_Pion_MinEneShowerLib(0.0*CLHEP::GeV),
    m_Pion_MaxEneShowerLib(1.0*CLHEP::GeV),

    m_containLow(false),
    m_absLowEta(3.8),
    m_containHigh(false),
    m_absHighEta(4.4),
    m_containCrack(false),
    m_absCrackEta1(0.5),
    m_absCrackEta2(1.1),
    m_generated_starting_points_file(""),
    m_generated_starting_points_ratio(0.02),
    m_detector_tag(0),
    m_SensitiveDetectorName(""),
    m_showerLibSvcName("LArG4ShowerLibSvc")
  {
  }
};

#endif //LARG4FASTSIMULATION_FASTSHOWERCONFIGSTRUCT_H
