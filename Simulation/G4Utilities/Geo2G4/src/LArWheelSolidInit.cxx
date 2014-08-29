/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// LArWheelSolid

#include <cassert>
#include <stdexcept>

#include "GaudiKernel/ISvcLocator.h"
#include "GaudiKernel/Bootstrap.h"
#include "GaudiKernel/MsgStream.h"

#include "G4Polycone.hh"
#include "G4GeometryTolerance.hh"

#include "GeoSpecialShapes/LArWheelCalculator.h"
#include "CLHEP/Units/PhysicalConstants.h"

#include "Geo2G4/LArWheelSolid.h"

// these are internal technical constants, should not go in DB
const unsigned int LArWheelSolid::IterationsLimit = 50; // That's enough even for 10e-15 IterationPrecision
const G4double LArWheelSolid::Tolerance = G4GeometryTolerance::GetInstance()->GetSurfaceTolerance() / 2;
const G4double LArWheelSolid::IterationPrecision = 0.001*CLHEP::mm;
const G4double LArWheelSolid::IterationPrecision2 = IterationPrecision * IterationPrecision;

LArWheelSolid::LArWheelSolid(const G4String& name, LArWheelSolid_t type,
			     G4int zside)
  : G4VSolid(name)
{
        ISvcLocator* svcLocator = Gaudi::svcLocator();
        IMessageSvc* msgSvc;
	StatusCode status = svcLocator->service("MessageSvc", msgSvc);
	if(!status.isFailure()){
		msg = new MsgStream(msgSvc, "LArWheelSolid");
	} else {
		throw std::runtime_error("LArWheelSolid constructor: cannot initialze message service");
	}
	(*msg) << MSG::DEBUG << "constructor started" << endreq;

	Type = type;
	PhiPosition = CLHEP::halfpi;
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
	Calculator = new LArWheelCalculator(calc_type, zside);
	G4String bp_name = name + "-BoundingPolycone";
	Verbose = false;

	// Initialize code that depends on wheel type:
	FanHalfThickness = Calculator->GetFanHalfThickness();
	switch(Type){
	case InnerAbsorberWheel:
	case InnerElectrodWheel:
	case InnerAbsorberModule:
	case InnerElectrodModule:
	case InnerGlueWheel:
	case InnerLeadWheel:
		inner_solid_init(bp_name);
		break;
	case OuterAbsorberWheel:
	case OuterElectrodWheel:
	case OuterAbsorberModule:
	case OuterElectrodModule:
	case OuterGlueWheel:
	case OuterLeadWheel:
		outer_solid_init(bp_name);
		break;
	default:
	        G4Exception("LArWheelSolid", "UnknownSolidType", FatalException,
			    "Constructor: unknown LArWheelSolid_t");
	}

	init_tests();

	test(); // activated by env. variable

	(*msg) << MSG::DEBUG << "solid of type "
	       << LArWheelCalculator::LArWheelCalculatorTypeString(calc_type)
	       << " initialized" << endreq;
}

LArWheelSolid::~LArWheelSolid()
{
	delete [] FanSection;
	delete msg;
	clean_tests();
}

// initialization of inner Absorber or Electrod wheels
void LArWheelSolid::inner_solid_init(const G4String &bp_name)
{
	FanPhiAmplitude = 0.065; // internal technical constant, should not go in DB
	set_phi_size();

	G4double zPlane[2], rInner[3], rOuter[2];
	zPlane[0] = 0;
	zPlane[1] = Calculator->GetWheelThickness();
	G4double wheel_thickness = zPlane[1] - zPlane[0];
	Calculator->GetWheelInnerRadius(rInner); // This can give us three radii - only want two...
	Calculator->GetWheelOuterRadius(rOuter);
	
	Zmin = zPlane[0]; Zmax = zPlane[1];
	Rmin = rInner[0]; Rmax = rOuter[1];

	BoundingPolycone = new G4Polycone(bp_name, MinPhi, MaxPhi - MinPhi,
		    		                        2, zPlane, rInner, rOuter);

	G4int number_of_half_waves = Calculator->GetNumberOfHalfWaves();
	FanSection = new G4Polycone* [number_of_half_waves + 2];
	MaxFanSection = number_of_half_waves + 1;
	// We heed two additional fan sections to handle begin and end folds correctly
	MaxFanSectionLimits = MaxFanSection + 1;
	FanSectionLimits = new G4double[MaxFanSectionLimits + 1];

	G4double sss = Calculator->GetStraightStartSection();
	G4double half_wave_length = Calculator->GetHalfWaveLength();

	G4double Ain = (rInner[1] - rInner[0]) / wheel_thickness;
	G4double Aout = (rOuter[1] - rOuter[0]) / wheel_thickness;
	G4double Bin = rInner[0], Bout = rOuter[0];

	G4double phi_min = CLHEP::halfpi - FanPhiAmplitude - Calculator->GetFanStepOnPhi() * 2;
	G4double phi_max = CLHEP::halfpi + FanPhiAmplitude + Calculator->GetFanStepOnPhi() * 2;

	G4int i;
	for(i = 2; i < MaxFanSection; i ++){
		FanSectionLimits[i] = half_wave_length * (i - 1) + sss;
	}
	FanSectionLimits[0] = 0.;
	FanSectionLimits[1] = sss + half_wave_length * 0.25;
	FanSectionLimits[MaxFanSectionLimits] = wheel_thickness - FanSectionLimits[0];
	FanSectionLimits[MaxFanSectionLimits - 1] = wheel_thickness - FanSectionLimits[1];

	char tmp_str[10];
	for(i = 0; i <= MaxFanSection; i ++){
		zPlane[0] = FanSectionLimits[i];
		zPlane[1] = FanSectionLimits[i + 1];
		rInner[0] = Bin  + zPlane[0] * Ain;
		rOuter[0] = Bout + zPlane[0] * Aout;
		rInner[1] = Bin  + zPlane[1] * Ain;
		rOuter[1] = Bout + zPlane[1] * Aout;

		sprintf(tmp_str, "%i", i);
		G4String fs_name = bp_name + "-FanSection-" + tmp_str;
#ifdef DEBUG_LARWHEELSOLID
		(*msg) << MSG::DEBUG << fs_name << G4endl
			   << "  z    = (" << zPlane[0] << "," << zPlane[1] << ")" << G4endl
			   << "  rIn  = (" << rInner[0] << "," << rInner[1] << ")" << G4endl
			   << "  rOut = (" << rOuter[0] << "," << rOuter[1] << ")" << endreq;
#endif
		FanSection[i] = new G4Polycone(fs_name, phi_min, phi_max - phi_min,
		    		                   2, zPlane, rInner, rOuter);
#ifdef DEBUG_LARWHEELSOLID
		(*msg) << MSG::DEBUG << "  phi  = (" << FanSection[i]->GetStartPhi()
			   << "," << FanSection[i]->GetEndPhi() << ")" << endreq;
#endif
	}
}

// initialization of outer Absorber or Electrod wheels
void LArWheelSolid::outer_solid_init(const G4String &bp_name)
{
	FanPhiAmplitude = 0.02; // internal technical constant, should not go in DB
	set_phi_size();

	G4double zPlane[3], rInner[3], rOuter[3];
	zPlane[0] = 0;
	zPlane[2] = Calculator->GetWheelThickness();
	G4double wheel_thickness = zPlane[2] - zPlane[0];
	zPlane[1] = Calculator->GetWheelInnerRadius(rInner);
	Calculator->GetWheelOuterRadius(rOuter);
	
	Zmin = zPlane[0]; Zmax = zPlane[2];
	Rmin = rInner[0]; Rmax = rOuter[2];
	
	BoundingPolycone = new G4Polycone(bp_name, MinPhi, MaxPhi - MinPhi,
		    		                        3, zPlane, rInner, rOuter);

	G4int number_of_half_waves = Calculator->GetNumberOfHalfWaves();
	FanSection = new G4Polycone* [number_of_half_waves + 2];
	MaxFanSection = number_of_half_waves + 1;
	// We heed two additional fan sections to handle begin and end folds correctly
	MaxFanSectionLimits = MaxFanSection + 1;
	FanSectionLimits = new G4double[MaxFanSectionLimits + 1];

	G4double sss = Calculator->GetStraightStartSection();
	G4double half_wave_length = Calculator->GetHalfWaveLength();

	G4double phi_min = CLHEP::halfpi - FanPhiAmplitude - Calculator->GetFanStepOnPhi() * 2;
	G4double phi_max = CLHEP::halfpi + FanPhiAmplitude + Calculator->GetFanStepOnPhi() * 2;

	G4int i;
	for(i = 2; i < MaxFanSection; i ++){
		FanSectionLimits[i] = half_wave_length * (i - 1) + sss;
	}
	FanSectionLimits[0] = 0.;
	FanSectionLimits[1] = sss + half_wave_length * 0.25;
	FanSectionLimits[MaxFanSectionLimits] = wheel_thickness - FanSectionLimits[0];
	FanSectionLimits[MaxFanSectionLimits - 1] = wheel_thickness - FanSectionLimits[1];

	// Note that as we go through the following loop, the [0] and [1]
	// elements of the arrays change, but the [2] index remains
	// constant except at the "bend."

	// Save the values at the "bend."
	G4double zMid = zPlane[1];
	G4double rMidInner = rInner[1];
	G4double rMidOuter = rOuter[1];

	char tmp_str[10];

	// Calculate slopes and intercepts for beginning part
	G4double Ain = (rInner[1] - rInner[0]) / (zPlane[1] - zPlane[0]);
	G4double Aout = (rOuter[1] - rOuter[0]) / (zPlane[1] - zPlane[0]);
	G4double Bin = rInner[0], Bout = rOuter[0];

	for(i = 0; i <= MaxFanSection; i ++){
		zPlane[0] = FanSectionLimits[i];
		zPlane[1] = FanSectionLimits[i + 1];
#ifdef DEBUG_LARWHEELSOLID
		(*msg) << MSG::DEBUG << "zPlane[0]=" << zPlane[0]
		       << ", zMid=" << zMid << ", zPlane[1]="
			   << zPlane[1] << endreq;
#endif
		if(zPlane[0] <= zMid  &&  zMid < zPlane[1]){
		  // We're creating a fan section with three z-planes (that is,
		  // this fan section includes the "bend" in the outer wheel).

			rInner[0] = Bin  + zPlane[0] * Ain;
			rOuter[0] = Bout + zPlane[0] * Aout;
			// Recalculate the slopes and intercepts.
			Ain  = (rInner[2] - rMidInner) / (zPlane[2] - zMid);
			Aout = (rOuter[2] - rMidOuter) / (zPlane[2] - zMid);
			Bin  = (zPlane[2] * rMidInner - rInner[2] * zMid) /
			                                               (zPlane[2] - zMid);
			Bout = (zPlane[2] * rMidOuter - rOuter[2] * zMid) /
			                                               (zPlane[2] - zMid);

			zPlane[2] = zPlane[1];
			rInner[2] = Bin  + zPlane[2] * Ain;
			rOuter[2] = Bout + zPlane[2] * Aout;
			zPlane[1] = zMid;
			rInner[1] = rMidInner;
			rOuter[1] = rMidOuter;

			sprintf(tmp_str, "%i", i);
			G4String fs_name = bp_name + "-FanSection-" + tmp_str;
#ifdef DEBUG_LARWHEELSOLID
			(*msg) << MSG::DEBUG << fs_name << G4endl
			       << "  z	  = (" << zPlane[0] << "," << zPlane[1] << "," << zPlane[2] << ")" << G4endl
				   << "  rIn  = (" << rInner[0] << "," << rInner[1] << "," << rInner[2] << ")" << G4endl
				   << "  rOut = (" << rOuter[0] << "," << rOuter[1] << "," << rOuter[2] << ")" << endreq;
#endif
			FanSection[i] = new G4Polycone(fs_name, phi_min, phi_max - phi_min,
					                       3, zPlane, rInner, rOuter);
#ifdef DEBUG_LARWHEELSOLID
			(*msg) << MSG::DEBUG << "  phi  = (" << FanSection[i]->GetStartPhi()
				   << "," << FanSection[i]->GetEndPhi() << ")" << endreq;
#endif
		} else {
		  // We're creating a fan section with two z-planes (the more
		  // typical case).
			rInner[0] = Bin  + zPlane[0] * Ain;
			rOuter[0] = Bout + zPlane[0] * Aout;
			rInner[1] = Bin  + zPlane[1] * Ain;
			rOuter[1] = Bout + zPlane[1] * Aout;

			sprintf(tmp_str, "%i", i);
			G4String fs_name = bp_name + "-FanSection-" + tmp_str;
#ifdef DEBUG_LARWHEELSOLID
			(*msg) << MSG::DEBUG << fs_name << G4endl
			       << "  z	  = (" << zPlane[0] << "," << zPlane[1] << ")" << G4endl
				   << "  rIn  = (" << rInner[0] << "," << rInner[1] << ")" << G4endl
				   << "  rOut = (" << rOuter[0] << "," << rOuter[1] << ")" << endreq;
#endif
			FanSection[i] = new G4Polycone(fs_name, phi_min, phi_max - phi_min,
				                           2, zPlane, rInner, rOuter);
#ifdef DEBUG_LARWHEELSOLID
			(*msg) << MSG::DEBUG << "  phi  = (" << FanSection[i]->GetStartPhi()
				   << "," << FanSection[i]->GetEndPhi() << ")" << endreq;
#endif
		}
	}
}

// it should be called after FanPhiAmplitude has been set
// and before BoundingPolycone creation
void LArWheelSolid::set_phi_size(void)
{
	if(Type == InnerAbsorberModule
	|| Type == InnerElectrodModule
	|| Type == OuterAbsorberModule
	|| Type == OuterElectrodModule)
	{
		MinPhi = PhiPosition - M_PI * (1. / 8.) - FanPhiAmplitude;
		MaxPhi = PhiPosition + M_PI * (1. / 8.) + FanPhiAmplitude;
	} else {
		MinPhi = 0.;
		MaxPhi = CLHEP::twopi;
	}
}
