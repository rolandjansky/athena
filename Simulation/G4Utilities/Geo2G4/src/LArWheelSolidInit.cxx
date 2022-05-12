/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

#include <cassert>
#include <stdexcept>
#include <iostream>


#include "GeoSpecialShapes/EMECData.h"

#include "CLHEP/Units/PhysicalConstants.h"
#include "G4GeometryTolerance.hh"
#include "G4Polycone.hh"

#include "GeoSpecialShapes/LArWheelCalculator.h"
#include "LArWheelSolid.h"
#include "LArFanSection.h"
#include "G4ShiftedCone.h"

#ifndef PORTABLE_LAR_SHAPE
#include "AthenaBaseComps/AthMsgStreamMacros.h"
#endif

#ifdef DEBUG_LARWHEELSOLID
G4int LArWheelSolid::Verbose = 0;
#endif

// these are internal technical constants, should not go in DB
const unsigned int LArWheelSolid::s_IterationsLimit = 50; // That's enough even for 10e-15 IterationPrecision
const G4double LArWheelSolid::s_Tolerance = G4GeometryTolerance::GetInstance()->GetSurfaceTolerance() / 2;
const G4double LArWheelSolid::s_AngularTolerance = G4GeometryTolerance::GetInstance()->GetAngularTolerance() / 2;
const G4double LArWheelSolid::s_IterationPrecision = 0.001*CLHEP::mm;
const G4double LArWheelSolid::s_IterationPrecision2 = s_IterationPrecision * s_IterationPrecision;

LArWheelSolid::LArWheelSolid(const G4String& name, LArWheelSolid_t type,
                             G4int zside,
                             LArWheelCalculator *calc,
			     const EMECData *emecData
                             )
  : G4VSolid(name), m_Type(type), m_Calculator(calc), m_PhiPosition(CLHEP::halfpi), m_fs(0)
#ifndef PORTABLE_LAR_SHAPE
  , m_msg("LArWheelSolid")
#endif
{
#ifndef PORTABLE_LAR_SHAPE
#ifdef LARWHEELSOLID_USE_FANBOUND
  ATH_MSG_INFO ( "compiled with G4 FanBound" );
#else
  ATH_MSG_INFO ( "compiled with private find_exit_point" );
#endif
#endif

  LArG4::LArWheelCalculator_t calc_type = LArG4::LArWheelCalculator_t(0);
  switch(m_Type){
  case InnerAbsorberWheel:
    calc_type = LArG4::InnerAbsorberWheel;
    break;
  case OuterAbsorberWheel:
    calc_type = LArG4::OuterAbsorberWheel;
    break;
  case InnerElectrodWheel:
    calc_type = LArG4::InnerElectrodWheel;
    break;
  case OuterElectrodWheel:
    calc_type = LArG4::OuterElectrodWheel;
    break;
  case InnerAbsorberModule:
    calc_type = LArG4::InnerAbsorberModule;
    break;
  case OuterAbsorberModule:
    calc_type = LArG4::OuterAbsorberModule;
    break;
  case InnerElectrodModule:
    calc_type = LArG4::InnerElectrodModule;
    break;
  case OuterElectrodModule:
    calc_type = LArG4::OuterElectrodModule;
    break;
  case InnerGlueWheel:
    calc_type = LArG4::InnerGlueWheel;
    break;
  case OuterGlueWheel:
    calc_type = LArG4::OuterGlueWheel;
    break;
  case InnerLeadWheel:
    calc_type = LArG4::InnerLeadWheel;
    break;
  case OuterLeadWheel:
    calc_type = LArG4::OuterLeadWheel;
    break;
  case InnerAbsorberCone:
    calc_type = LArG4::InnerAbsorberWheel;
    break;
  case InnerElectrodCone:
    calc_type = LArG4::InnerElectrodWheel;
    break;
  case InnerGlueCone:
    calc_type = LArG4::InnerGlueWheel;
    break;
  case InnerLeadCone:
    calc_type = LArG4::InnerLeadWheel;
    break;
  case OuterAbsorberFrontCone:
    calc_type = LArG4::OuterAbsorberWheel;
    break;
  case OuterElectrodFrontCone:
    calc_type = LArG4::OuterElectrodWheel;
    break;
  case OuterGlueFrontCone:
    calc_type = LArG4::OuterGlueWheel;
    break;
  case OuterLeadFrontCone:
    calc_type = LArG4::OuterLeadWheel;
    break;
  case OuterAbsorberBackCone:
    calc_type = LArG4::OuterAbsorberWheel;
    break;
  case OuterElectrodBackCone:
    calc_type = LArG4::OuterElectrodWheel;
    break;
  case OuterGlueBackCone:
    calc_type = LArG4::OuterGlueWheel;
    break;
  case OuterLeadBackCone:
    calc_type = LArG4::OuterLeadWheel;
    break;
  default:
    G4Exception("LArWheelSolid", "UnknownSolidType", FatalException,
                "Constructor: unknown LArWheelSolid_t");
  }

  if(m_Calculator == 0) m_Calculator = new LArWheelCalculator(*emecData,calc_type, zside);

  const G4String bs_name = name + "-Bounding";
#ifdef DEBUG_LARWHEELSOLID
  const char *venv = getenv("LARWHEELSOLID_VERBOSE");
  if(venv) Verbose = atoi(venv);
  std::cout << "The LArWheelSolid build " << __DATE__ << " " << __TIME__
            << std::endl;
  std::cout << "LArWheelSolid verbosity level is " << Verbose << std::endl;
#endif

  // Initialize code that depends on wheel type:
  m_FanHalfThickness = GetCalculator()->GetFanHalfThickness();
  m_FHTplusT = m_FanHalfThickness + s_Tolerance;
  m_FHTminusT = m_FanHalfThickness - s_Tolerance;
  switch(m_Type){
  case InnerAbsorberWheel:
  case InnerElectrodWheel:
  case InnerAbsorberModule:
  case InnerElectrodModule:
  case InnerGlueWheel:
  case InnerLeadWheel:
  case InnerAbsorberCone:
  case InnerElectrodCone:
  case InnerGlueCone:
  case InnerLeadCone:
    inner_solid_init(bs_name);
    break;
  case OuterAbsorberWheel:
  case OuterElectrodWheel:
  case OuterAbsorberModule:
  case OuterElectrodModule:
  case OuterGlueWheel:
  case OuterLeadWheel:
  case OuterAbsorberFrontCone:
  case OuterElectrodFrontCone:
  case OuterGlueFrontCone:
  case OuterLeadFrontCone:
  case OuterAbsorberBackCone:
  case OuterElectrodBackCone:
  case OuterGlueBackCone:
  case OuterLeadBackCone:
    outer_solid_init(bs_name);
    break;
  default:
    G4Exception("LArWheelSolid", "UnknownSolidType", FatalException,
                "Constructor: unknown LArWheelSolid_t");
  }

  m_Zsect_start_search = (m_Zsect.size() - 1) - 1;
#ifndef PORTABLE_LAR_SHAPE
  init_tests();
  test(); // activated by env. variable
  clean_tests();
#endif
  
#ifdef DEBUG_LARWHEELSOLID
  m_fs->print();
  std::cout << "Limits: (" << m_Zsect.size() << ")" << std::endl;
  for(size_t i = 0; i < m_Zsect.size(); ++ i){
    std::cout << i << " " << m_Zsect[i] << std::endl;
  }
#endif
#ifndef PORTABLE_LAR_SHAPE
  ATH_MSG_DEBUG ( "solid of type "
                  << LArWheelCalculator::LArWheelCalculatorTypeString(calc_type)
                  << " initialized" );
#endif
}

LArWheelSolid::~LArWheelSolid()
{
  if(m_fs) delete m_fs;
}

// initialization of inner Absorber or Electrod wheels
void LArWheelSolid::inner_solid_init(const G4String &bs_name)
{
    m_IsOuter = false;
    m_FanPhiAmplitude = 0.065; // internal technical constant, should not go in DB
    set_phi_size();

    G4double zPlane[2], rInner[2], rOuter[2];
    zPlane[0] = 0.;
    zPlane[1] = GetCalculator()->GetWheelThickness();
    G4double wheel_thickness = zPlane[1] - zPlane[0];
    GetCalculator()->GetWheelInnerRadius(rInner);
    GetCalculator()->GetWheelOuterRadius(rOuter);
    const G4double phi_min = m_PhiPosition - m_FanPhiAmplitude
                           - GetCalculator()->GetFanStepOnPhi() * 2;

    m_Zmin = zPlane[0]; m_Zmax = zPlane[1];
    m_Rmin = rInner[0]; m_Rmax = rOuter[1];
    m_Ymin = m_Rmin * 0.9;
    m_Zmid = zPlane[1];
    m_Ymid = (rInner[0] + rOuter[1]) * 0.5;

    if(m_Type == InnerAbsorberCone
    || m_Type == InnerElectrodCone
    || m_Type == InnerGlueCone
    || m_Type == InnerLeadCone
    ){
        m_BoundingShape = new G4ShiftedCone(
            bs_name + "Cone", zPlane[0], zPlane[1],
            rInner[0], rOuter[0], rInner[1], rOuter[1]
        );
    } else {
        m_BoundingShape = new G4Polycone(
            bs_name + "Polycone", m_MinPhi, m_MaxPhi - m_MinPhi,
            2, zPlane, rInner, rOuter
        );
    }
#ifdef LARWHEELSOLID_USE_FANBOUND
    const G4double phi_size = (m_FanPhiAmplitude + GetCalculator()->GetFanStepOnPhi() * 2) * 2;
    FanBound = new G4Polycone(bs_name + "ofFan", phi_min, phi_size,
                              2, zPlane, rInner, rOuter);
#endif
#ifndef PORTABLE_LAR_SHAPE
    ATH_MSG_INFO(m_BoundingShape->GetName() + " is the m_BoundingShape");
#endif
    
    const G4double half_wave_length = GetCalculator()->GetHalfWaveLength();
    const G4double sss = GetCalculator()->GetStraightStartSection();
    m_Zsect.push_back(0.);
    m_Zsect.push_back(sss + half_wave_length * 0.25);
    const G4int num_fs = GetCalculator()->GetNumberOfHalfWaves() + 1;
    for(G4int i = 2; i < num_fs; i ++){
          const G4double zi = half_wave_length * (i - 1) + sss;
#if LARWHEELSOLID_ZSECT_MULT > 1
          for(G4int j = LARWHEELSOLID_ZSECT_MULT - 1; j > 0; -- j){
            m_Zsect.push_back(zi - half_wave_length * j / LARWHEELSOLID_ZSECT_MULT);
          }
#endif
          m_Zsect.push_back(zi);
    }
    m_Zsect.push_back(wheel_thickness - m_Zsect[1]);
    m_Zsect.push_back(wheel_thickness - m_Zsect[0]);

    m_fs = new LArFanSections(
        rInner[0], rInner[1], rOuter[0], rOuter[1],
        m_Rmax*cos(phi_min), m_Zsect.front(), m_Zsect.back()
    );
}

// initialization of outer Absorber or Electrod wheels
void LArWheelSolid::outer_solid_init(const G4String &bs_name)
{
    m_IsOuter = true;
    m_FanPhiAmplitude = 0.02; // internal technical constant, should not go in DB
    set_phi_size();

    G4double zPlane[3], rInner[3], rOuter[3];
    zPlane[0] = 0.;
    zPlane[2] = GetCalculator()->GetWheelThickness();
    G4double wheel_thickness = zPlane[2] - zPlane[0];
    zPlane[1] = GetCalculator()->GetWheelInnerRadius(rInner);
    GetCalculator()->GetWheelOuterRadius(rOuter);
    const G4double phi_min =
        m_PhiPosition - m_FanPhiAmplitude -
        GetCalculator()->GetFanStepOnPhi() * 2;

    m_Zmid = zPlane[1];
    m_Ymid = (rInner[0] + rOuter[2]) * 0.5;

    bool hasFrontSections = false;
    bool hasBackSections = false;
    if(m_Type == OuterAbsorberFrontCone
    || m_Type == OuterElectrodFrontCone
    || m_Type == OuterGlueFrontCone
    || m_Type == OuterLeadFrontCone
    ){
        m_Zmin = zPlane[0]; m_Zmax = zPlane[1];
        m_Rmin = rInner[0]; m_Rmax = rOuter[1];
        m_BoundingShape = new G4ShiftedCone(
            bs_name + "FrontCone", zPlane[0], zPlane[1],
            rInner[0], rOuter[0], rInner[1], rOuter[1]
        );
        hasFrontSections = true;
    } else if(m_Type == OuterAbsorberBackCone
    || m_Type == OuterElectrodBackCone
    || m_Type == OuterGlueBackCone
    || m_Type == OuterLeadBackCone
    ){
        m_Zmin = zPlane[1]; m_Zmax = zPlane[2];
        m_Rmin = rInner[1]; m_Rmax = rOuter[2];
        m_BoundingShape = new G4ShiftedCone(
            bs_name + "BackCone", zPlane[1], zPlane[2],
            rInner[1], rOuter[1], rInner[2], rOuter[2]
        );
        hasBackSections = true;
    } else {
        m_Zmin = zPlane[0]; m_Zmax = zPlane[2];
        m_Rmin = rInner[0]; m_Rmax = rOuter[2];
        m_BoundingShape = new G4Polycone(
            bs_name + "Polycone", m_MinPhi, m_MaxPhi - m_MinPhi,
            3, zPlane, rInner, rOuter
        );
        hasFrontSections = true;
        hasBackSections = true;
    }

    m_Ymin = m_Rmin * 0.9;

#ifdef LARWHEELSOLID_USE_FANBOUND
  const G4double phi_size = (m_FanPhiAmplitude + GetCalculator()->GetFanStepOnPhi() * 2) * 2;
  FanBound = new G4Polycone(bs_name + "ofFan", phi_min, phi_size,
                            3, zPlane, rInner, rOuter);
#endif
#ifndef PORTABLE_LAR_SHAPE
    ATH_MSG_INFO(m_BoundingShape->GetName() + " is the m_BoundingShape");
#endif
    const G4double half_wave_length = GetCalculator()->GetHalfWaveLength();
    const G4double sss = GetCalculator()->GetStraightStartSection();

    if(hasFrontSections){
        m_Zsect.push_back(0.);
        m_Zsect.push_back(sss + half_wave_length * 0.25);
    } else {
        m_Zsect.push_back(m_Zmid);
    }
    const G4int num_fs = GetCalculator()->GetNumberOfHalfWaves() + 1;

    for(G4int i = 2; i < num_fs; i ++){
        const G4double zi = half_wave_length * (i - 1) + sss;
#if LARWHEELSOLID_ZSECT_MULT > 1
        for(G4int j = LARWHEELSOLID_ZSECT_MULT - 1; j > 0; -- j){
            G4double zj = zi - half_wave_length * j / LARWHEELSOLID_ZSECT_MULT;
            if(hasFrontSections && hasBackSections
            && m_Zsect.back() < m_Zmid && zj >= m_Zmid){
                m_Zsect.push_back(m_Zmid);
            }
            if((zj < m_Zmid && hasFrontSections)
            || (zj > m_Zmid && hasBackSections)){
                m_Zsect.push_back(zj);
            }
        }
#endif
        if(hasFrontSections && hasBackSections
        && m_Zsect.back() < m_Zmid && zi >= m_Zmid){
            m_Zsect.push_back(m_Zmid);
        }
        if((zi < m_Zmid && hasFrontSections)
        || (zi > m_Zmid && hasBackSections)){
            m_Zsect.push_back(zi);
        }
    }
    if(hasBackSections){
        m_Zsect.push_back(wheel_thickness - sss - half_wave_length * 0.25);
        m_Zsect.push_back(wheel_thickness);
    } else {
        m_Zsect.push_back(m_Zmid);
    }

    m_fs = new LArFanSections(
        rInner[0], rInner[1], rOuter[0], rOuter[1],
        m_Rmax*cos(phi_min), m_Zsect.front(), m_Zmid
    );
}

// it should be called after m_FanPhiAmplitude has been set
// and before m_BoundingShape creation
void LArWheelSolid::set_phi_size(void)
{
    if(GetCalculator()->GetisModule()){
        m_MinPhi = m_PhiPosition - CLHEP::pi * (1. / 8.) - m_FanPhiAmplitude;
        m_MaxPhi = m_PhiPosition + CLHEP::pi * (1. / 8.) + m_FanPhiAmplitude;
    } else {
        m_MinPhi = 0.;
        m_MaxPhi = CLHEP::twopi;
    }
}
