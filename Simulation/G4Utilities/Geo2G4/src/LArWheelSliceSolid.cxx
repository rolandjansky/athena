/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#include "G4VGraphicsScene.hh"
#include "G4VisExtent.hh"

#include "GeoSpecialShapes/LArWheelCalculator.h"
#include "LArWheelSliceSolid.h"

class G4NURBS;
class G4VoxelLimits;
class G4AffineTransform;

EInside LArWheelSliceSolid::Inside(const G4ThreeVector &inputP) const
{
    LWSDBG(10, std::cout << std::setprecision(25));
    LWSDBG(1, std::cout << TypeStr() << " Inside " << MSG_VECTOR(inputP) << std::endl);
    const EInside inside_BS = m_BoundingShape->Inside(inputP);
    if(inside_BS == kOutside){
        LWSDBG(2, std::cout << "outside BS" << std::endl);
        return kOutside;
    }
    G4ThreeVector p(inputP);
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

G4ThreeVector LArWheelSliceSolid::SurfaceNormal(const G4ThreeVector &inputP) const
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

G4bool LArWheelSliceSolid::CalculateExtent(
    const EAxis a, const G4VoxelLimits &vl,
    const G4AffineTransform &t, G4double &p,
    G4double &q
) const
{
    return m_BoundingShape->CalculateExtent(a, vl, t, p, q);
}

G4String LArWheelSliceSolid::TypeStr(void) const
{
    G4String ret("");
    switch(m_Pos){
    case Inner:
        switch(m_Type){
        case Absorber: ret = "LArInnerAbsorberWheel"; break;
        case Electrode: ret = "LArInnerElectrodWheel"; break;
        case Glue: ret = "LArInnerGlueWheel"; break;
        case Lead: ret = "LArInnerLeadWheel"; break;
        }
        break;
    case Outer:
        switch(m_Type){
        case Absorber: ret = "LArOuterAbsorberWheel"; break;
        case Electrode: ret = "LArOuterElectrodWheel"; break;
        case Glue: ret = "LArOuterGlueWheel"; break;
        case Lead: ret = "LArOuterLeadWheel"; break;
        }
        break;
    }
    return ret;
}

void LArWheelSliceSolid::DescribeYourselfTo(G4VGraphicsScene &scene) const
{
    scene.AddSolid(*this);
}

G4VisExtent LArWheelSliceSolid::GetExtent() const
{
    return m_BoundingShape->GetExtent();
}

G4Polyhedron* LArWheelSliceSolid::CreatePolyhedron() const
{
    return m_BoundingShape->CreatePolyhedron();
}
