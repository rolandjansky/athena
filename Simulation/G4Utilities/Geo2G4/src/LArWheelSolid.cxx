/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "G4VGraphicsScene.hh"
#include "G4VisExtent.hh"

#include "GeoSpecialShapes/LArWheelCalculator.h"
#include "LArWheelSolid.h"

class G4NURBS;
class G4VoxelLimits;
class G4AffineTransform;

EInside LArWheelSolid::Inside(const G4ThreeVector &inputP) const
{
	LWSDBG(10, std::cout << std::setprecision(25));
	LWSDBG(1, std::cout << TypeStr() << " Inside " << MSG_VECTOR(inputP) << std::endl);
	const EInside inside_BS = m_BoundingShape->Inside(inputP);
	if(inside_BS == kOutside){
		LWSDBG(2, std::cout << "outside BS" << std::endl);
		return kOutside;
	}
	G4ThreeVector p( inputP );
	int p_fan = 0;
	const G4double d = fabs(GetCalculator()->DistanceToTheNearestFan(p, p_fan));
	if(d > m_FHTplusT){
		LWSDBG(2, std::cout << "outside fan d=" << d << ", m_FHTplusT=" << m_FHTplusT << std::endl);
		return kOutside;
	}
	if(d < m_FHTminusT){
		LWSDBG(2, std::cout << "inside fan d=" << d << ", m_FHTminusT=" << m_FHTminusT << ", inside_BS=" << inside(inside_BS) << std::endl);
		return inside_BS;
	}
	LWSDBG(2, std::cout << "surface" << std::endl);
	return kSurface;
}

G4ThreeVector LArWheelSolid::SurfaceNormal(const G4ThreeVector &inputP) const
{
	LWSDBG(1, std::cout << TypeStr() << " SurfaceNormal" << MSG_VECTOR(inputP) << std::endl);
	EInside inside_BS = m_BoundingShape->Inside(inputP);
	if(inside_BS != kInside){
		LWSDBG(2, std::cout << "not inside BS" << std::endl);
		return m_BoundingShape->SurfaceNormal(inputP);
	}
	G4ThreeVector p( inputP );
	int p_fan = 0;
	GetCalculator()->DistanceToTheNearestFan(p, p_fan);
	G4ThreeVector d = GetCalculator()->NearestPointOnNeutralFibre(p, p_fan);
	d.rotateZ(inputP.phi() -  p.phi()); // rotate back to initial position
	LWSDBG(4, std::cout << "npnf" << MSG_VECTOR(d) << std::endl);
	p = inputP - d;
	LWSDBG(2, std::cout << "sn " << MSG_VECTOR(p.unit()) << std::endl);
	return(p.unit());
}

G4bool LArWheelSolid::CalculateExtent(const EAxis a, const G4VoxelLimits &vl,
                                      const G4AffineTransform &t, G4double &p,
                                      G4double &q) const
{
	return m_BoundingShape->CalculateExtent(a, vl, t, p, q);
}

G4GeometryType LArWheelSolid::GetEntityType() const
{
  switch(m_Type){
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
  return m_BoundingShape->GetExtent();
}

G4Polyhedron* LArWheelSolid::CreatePolyhedron() const
{
  return m_BoundingShape->CreatePolyhedron();
}

/*
 * returns the number of lower z boundary of z-section containing Z
 */
G4int LArWheelSolid::select_section(const G4double &Z) const
{
	for(G4int i = m_Zsect_start_search; i > 0; -- i){
		if(Z > m_Zsect[i]) return i;
	}
	return 0;
}
