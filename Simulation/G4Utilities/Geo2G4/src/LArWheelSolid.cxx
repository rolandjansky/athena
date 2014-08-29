/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// LArWheelSolid.cc (see also LArWheelSolid*.cc)
// Authors: A. M. Soukharev, with revisions by William G. Seligman
// 16-Oct-2001 WGS: Incorporated corrected for first and last wave
//                  from Jozsef Toth.
// Nov 2001 AMS: bugfixes and updates of algorithms
//                             also in files LArWheelSolid*.cc
// 01-Apr-2002 AMS: Added GetPhiGap routine.
// August 2002: speed improvements
// 15-Apr-2003 AMS: moved geometry code to LArWheelCalculator

#include "G4VGraphicsScene.hh"
#include "G4VisExtent.hh"
#include "G4Polycone.hh"
#include "G4ios.hh"

#include "GeoSpecialShapes/LArWheelCalculator.h"
#include "Geo2G4/LArWheelSolid.h"

class G4Polyhedron;
class G4NURBS;
class G4VoxelLimits;
class G4AffineTransform;

EInside LArWheelSolid::Inside(const G4ThreeVector &inputP) const
{
	static G4ThreeVector p;
	EInside inside_bp = BoundingPolycone->Inside(inputP);
	if(inside_bp == kOutside) return kOutside;
	p = inputP;
	G4double d = FanHalfThickness - fabs(Calculator->DistanceToTheNearestFan(p));
	if(d > Tolerance) return inside_bp;
	if(d > -Tolerance) return kSurface;
	return kOutside;
}

G4ThreeVector LArWheelSolid::SurfaceNormal (const G4ThreeVector &inputP) const
{
	static G4ThreeVector p, d;
	EInside inside_bp = BoundingPolycone->Inside(inputP);
	if(inside_bp == kOutside
	|| (inside_bp == kSurface && Inside(inputP) == kSurface))
	{
		return BoundingPolycone->SurfaceNormal(inputP);
	}
	p = inputP;
	Calculator->DistanceToTheNearestFan(p);
 	d = Calculator->NearestPointOnNeutralFibre(p);
	d.rotateZ(inputP.phi() - p.phi()); // rotate back to initial position
	p = inputP - d;
	return(p.unit());
}

G4int LArWheelSolid::select_fan_section(G4double z) const
{
	G4int i;
	for(i = 1; i <= MaxFanSectionLimits; i ++){
		if(FanSectionLimits[i] >= z) return i - 1;
	}
	return i - 2;
}

G4bool LArWheelSolid::CalculateExtent(const EAxis a, const G4VoxelLimits &vl,
                                      const G4AffineTransform &t, G4double &p,
				      				  G4double &q) const
{
	return BoundingPolycone->CalculateExtent(a, vl, t, p, q);
}

G4GeometryType LArWheelSolid::GetEntityType() const
{
	switch(Type){
	case InnerAbsorberWheel:
		return G4String("LArInnerAbsorberWheel");
		break;
	case OuterAbsorberWheel:
		return G4String("LArOuterAbsorberWheel");
		break;
	case InnerElectrodWheel:
		return G4String("LArInnerElecrodWheel");
		break;
	case OuterElectrodWheel:
		return G4String("LArOuterElecrodWheel");
		break;
	case InnerAbsorberModule:
		return G4String("LArInnerAbsorberModule");
		break;
	case OuterAbsorberModule:
		return G4String("LArOuterAbsorberModule");
		break;
	case InnerElectrodModule:
		return G4String("LArInnerElecrodModule");
		break;
	case OuterElectrodModule:
		return G4String("LArOuterElecrodModule");
		break;
	case InnerGlueWheel:
		return G4String("LArInnerGlueWheel");
		break;
	case OuterGlueWheel:
		return G4String("LArOuterGlueWheel");
		break;
	case InnerLeadWheel:
		return G4String("LArInnerLeadWheel");
		break;
	case OuterLeadWheel:
		return G4String("LArOuterLeadWheel");
		break;
	}
        G4Exception("LArWheelSolid", "UnknownSolidType", FatalException,"GetEntityType: Unknown LArWheelType.");
	return G4String("");
}

void LArWheelSolid::DescribeYourselfTo(G4VGraphicsScene &scene) const
{
	scene.AddSolid(*this);
}

G4VisExtent LArWheelSolid::GetExtent() const
{
	return BoundingPolycone->GetExtent();
}

G4Polyhedron* LArWheelSolid::CreatePolyhedron() const
{
	return BoundingPolycone->CreatePolyhedron();
}
