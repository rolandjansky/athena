/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include <cassert>
#include <stdexcept>
#include <iostream>

#include "CLHEP/Units/PhysicalConstants.h"
#include "AthenaBaseComps/AthMsgStreamMacros.h"
#include "G4GeometryTolerance.hh"
#include "G4Polycone.hh"

#include "GeoSpecialShapes/LArWheelCalculator.h"
#include "LArWheelSolid.h"
#include "LArFanSection.h"

#ifdef DEBUG_LARWHEELSOLID
G4int LArWheelSolid::Verbose = 0;
#endif

// these are internal technical constants, should not go in DB
const unsigned int LArWheelSolid::IterationsLimit = 50; // That's enough even for 10e-15 IterationPrecision
const G4double LArWheelSolid::Tolerance = G4GeometryTolerance::GetInstance()->GetSurfaceTolerance() / 2;
const G4double LArWheelSolid::AngularTolerance = G4GeometryTolerance::GetInstance()->GetAngularTolerance() / 2;
const G4double LArWheelSolid::IterationPrecision = 0.001*CLHEP::mm;
const G4double LArWheelSolid::IterationPrecision2 = IterationPrecision * IterationPrecision;

LArWheelSolid::LArWheelSolid(const G4String& name, LArWheelSolid_t type,
                             G4int zside)
  : G4VSolid(name), Type(type), PhiPosition(CLHEP::halfpi), m_fs(0),
    m_msg("LArWheelSolid"),
    f_area(0), f_vol(0), f_area_on_pc(0), f_length(0), f_side_area(0)
{
#ifdef LARWHEELSOLID_USE_FANBOUND
  ATH_MSG_INFO ( "compiled with G4 FanBound" );
#else
  ATH_MSG_INFO ( "compiled with private find_exit_point" );
#endif

  LArWheelCalculator::LArWheelCalculator_t calc_type = LArWheelCalculator::LArWheelCalculator_t(0);
  switch(Type){
  case InnerAbsorberWheel:
    calc_type = LArWheelCalculator::InnerAbsorberWheel;
    break;
  case OuterAbsorberWheel:
    calc_type = LArWheelCalculator::OuterAbsorberWheel;
    break;
  case InnerElectrodWheel:
    calc_type = LArWheelCalculator::InnerElectrodWheel;
    break;
  case OuterElectrodWheel:
    calc_type = LArWheelCalculator::OuterElectrodWheel;
    break;
  case InnerAbsorberModule:
    calc_type = LArWheelCalculator::InnerAbsorberModule;
    break;
  case OuterAbsorberModule:
    calc_type = LArWheelCalculator::OuterAbsorberModule;
    break;
  case InnerElectrodModule:
    calc_type = LArWheelCalculator::InnerElectrodModule;
    break;
  case OuterElectrodModule:
    calc_type = LArWheelCalculator::OuterElectrodModule;
    break;
  case InnerGlueWheel:
    calc_type = LArWheelCalculator::InnerGlueWheel;
    break;
  case OuterGlueWheel:
    calc_type = LArWheelCalculator::OuterGlueWheel;
    break;
  case InnerLeadWheel:
    calc_type = LArWheelCalculator::InnerLeadWheel;
    break;
  case OuterLeadWheel:
    calc_type = LArWheelCalculator::OuterLeadWheel;
    break;
  default:
    G4Exception("LArWheelSolid", "UnknownSolidType", FatalException,
                "Constructor: unknown LArWheelSolid_t");
  }
  m_Calculator = new LArWheelCalculator(calc_type, zside);
  const G4String bs_name = name + "-Bounding";
#ifdef DEBUG_LARWHEELSOLID
  const char *venv = getenv("LARWHEELSOLID_VERBOSE");
  if(venv) Verbose = atoi(venv);
#endif

  // Initialize code that depends on wheel type:
  FanHalfThickness = GetCalculator()->GetFanHalfThickness();
  FHTplusT = FanHalfThickness + Tolerance;
  FHTminusT = FanHalfThickness - Tolerance;
  switch(Type){
  case InnerAbsorberWheel:
  case InnerElectrodWheel:
  case InnerAbsorberModule:
  case InnerElectrodModule:
  case InnerGlueWheel:
  case InnerLeadWheel:
    inner_solid_init(bs_name);
    break;
  case OuterAbsorberWheel:
  case OuterElectrodWheel:
  case OuterAbsorberModule:
  case OuterElectrodModule:
  case OuterGlueWheel:
  case OuterLeadWheel:
    outer_solid_init(bs_name);
    break;
  default:
    G4Exception("LArWheelSolid", "UnknownSolidType", FatalException,
                "Constructor: unknown LArWheelSolid_t");
  }

  Zsect_start_search = (Zsect.size() - 1) - 1;

  init_tests();
  test(); // activated by env. variable
  clean_tests();

#ifdef DEBUG_LARWHEELSOLID
	m_fs->print();
	std::cout << "Limits: (" << Zsect.size() << ")" << std::endl;
	for(size_t i = 0; i < Zsect.size(); ++ i){
		std::cout << i << " " << Zsect[i] << std::endl;
	}
#endif
  ATH_MSG_DEBUG ( "solid of type "
                  << LArWheelCalculator::LArWheelCalculatorTypeString(calc_type)
                  << " initialized" );
}

LArWheelSolid::~LArWheelSolid()
{
	if(m_fs) delete m_fs;
}

// initialization of inner Absorber or Electrod wheels
void LArWheelSolid::inner_solid_init(const G4String &bs_name)
{
	IsOuter = false;
	FanPhiAmplitude = 0.065; // internal technical constant, should not go in DB
	set_phi_size();

	G4double zPlane[2], rInner[2], rOuter[2];
	zPlane[0] = 0.;
	zPlane[1] = GetCalculator()->GetWheelThickness();
	G4double wheel_thickness = zPlane[1] - zPlane[0];
	GetCalculator()->GetWheelInnerRadius(rInner);
	GetCalculator()->GetWheelOuterRadius(rOuter);
	const G4double phi_min = PhiPosition - FanPhiAmplitude
	                       - GetCalculator()->GetFanStepOnPhi() * 2;

	Zmin = zPlane[0]; Zmax = zPlane[1];
	Rmin = rInner[0]; Rmax = rOuter[1];
	Ymin = Rmin * 0.9;
	Zmid = zPlane[1];

	BoundingPolycone = new G4Polycone(bs_name + "Polycone", MinPhi, MaxPhi - MinPhi,
	                                  2, zPlane, rInner, rOuter);

	BoundingShape = BoundingPolycone;
#ifdef LARWHEELSOLID_USE_FANBOUND
	const G4double phi_size = (FanPhiAmplitude + GetCalculator()->GetFanStepOnPhi() * 2) * 2;
	FanBound = new G4Polycone(bs_name + "ofFan", phi_min, phi_size,
	                          2, zPlane, rInner, rOuter);
#endif
	ATH_MSG_INFO(BoundingShape->GetName() + " is the BoundingShape");

	const G4double half_wave_length = GetCalculator()->GetHalfWaveLength();
	const G4double sss = GetCalculator()->GetStraightStartSection();
	Zsect.push_back(0.);
	Zsect.push_back(sss + half_wave_length * 0.25);
	const G4int num_fs = GetCalculator()->GetNumberOfHalfWaves() + 1;
	for(G4int i = 2; i < num_fs; i ++){
		const G4double zi = half_wave_length * (i - 1) + sss;
#if LARWHEELSOLID_ZSECT_MULT > 1
		for(G4int j = LARWHEELSOLID_ZSECT_MULT - 1; j > 0; -- j){
			Zsect.push_back(zi - half_wave_length * j / LARWHEELSOLID_ZSECT_MULT);
		}
#endif
		Zsect.push_back(zi);
	}
	Zsect.push_back(wheel_thickness - Zsect[1]);
	Zsect.push_back(wheel_thickness - Zsect[0]);

	m_fs = new LArFanSections(
		rInner[0], rInner[1], rOuter[0], rOuter[1],
		Rmax*cos(phi_min), Zsect.front(), Zsect.back()
	);
}

// initialization of outer Absorber or Electrod wheels
void LArWheelSolid::outer_solid_init(const G4String &bs_name)
{
	IsOuter = true;
	FanPhiAmplitude = 0.02; // internal technical constant, should not go in DB
	set_phi_size();

	G4double zPlane[3], rInner[3], rOuter[3];
	zPlane[0] = 0.;
	zPlane[2] = GetCalculator()->GetWheelThickness();
	G4double wheel_thickness = zPlane[2] - zPlane[0];
	zPlane[1] = GetCalculator()->GetWheelInnerRadius(rInner);
	GetCalculator()->GetWheelOuterRadius(rOuter);
	const G4double phi_min = PhiPosition - FanPhiAmplitude
	                       - GetCalculator()->GetFanStepOnPhi() * 2;

	Zmin = zPlane[0]; Zmax = zPlane[2];
	Rmin = rInner[0]; Rmax = rOuter[2];
	Ymin = Rmin * 0.9;
	Zmid = zPlane[1];

	BoundingPolycone = new G4Polycone(bs_name + "Polycone", MinPhi, MaxPhi - MinPhi,
	                                  3, zPlane, rInner, rOuter);
	BoundingShape = BoundingPolycone;
#ifdef LARWHEELSOLID_USE_FANBOUND
	const G4double phi_size = (FanPhiAmplitude + GetCalculator()->GetFanStepOnPhi() * 2) * 2;
	FanBound = new G4Polycone(bs_name + "ofFan", phi_min, phi_size,
	                          3, zPlane, rInner, rOuter);
#endif
	ATH_MSG_INFO(BoundingShape->GetName() + "is the BoundingShape");

	const G4double half_wave_length = GetCalculator()->GetHalfWaveLength();
	const G4double sss = GetCalculator()->GetStraightStartSection();
	Zsect.push_back(0.);
	Zsect.push_back(sss + half_wave_length * 0.25);
	const G4int num_fs = GetCalculator()->GetNumberOfHalfWaves() + 1;
	for(G4int i = 2; i < num_fs; i ++){
		const G4double zi = half_wave_length * (i - 1) + sss;
#if LARWHEELSOLID_ZSECT_MULT > 1
		for(G4int j = LARWHEELSOLID_ZSECT_MULT - 1; j > 0; -- j){
			G4double zj = zi - half_wave_length * j / LARWHEELSOLID_ZSECT_MULT;
			Zsect.push_back(zj);
			if(Zsect.back() <= Zmid && Zmid < zj){
				Zsect.push_back(Zmid);
			}
		}
#endif
		if(Zsect.back() <= Zmid && Zmid < zi){
			Zsect.push_back(Zmid);
		}
		Zsect.push_back(zi);
	}
	Zsect.push_back(wheel_thickness - Zsect[1]);
	Zsect.push_back(wheel_thickness - Zsect[0]);

	m_fs = new LArFanSections(
		rInner[0], rInner[1], rOuter[0], rOuter[1],
		Rmax*cos(phi_min), Zsect.front(), Zmid
	);
}

// it should be called after FanPhiAmplitude has been set
// and before BoundingShape creation
void LArWheelSolid::set_phi_size(void)
{
	if(GetCalculator()->GetisModule()){
		MinPhi = PhiPosition - CLHEP::pi * (1. / 8.) - FanPhiAmplitude;
		MaxPhi = PhiPosition + CLHEP::pi * (1. / 8.) + FanPhiAmplitude;
	} else {
		MinPhi = 0.;
		MaxPhi = CLHEP::twopi;
	}
}
