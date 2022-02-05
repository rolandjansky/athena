/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#include "LArStraightElectrodes.h"
#include "G4LogicalVolume.hh"
#include "G4VSolid.hh"
#include "G4Trap.hh"
#include "LArG4Code/LArVG4DetectorParameters.h"

LArStraightElectrodes* LArStraightElectrodes::s_instance=nullptr;

PhysicalVolumeAccessor* LArStraightElectrodes::s_theElectrodes=nullptr;

std::once_flag LArStraightElectrodes::s_flag;

LArStraightElectrodes*  LArStraightElectrodes::GetInstance(const std::string& strDetector)
{
  std::call_once( s_flag, [&](){ s_instance = new LArStraightElectrodes(strDetector); } );
  return s_instance;
}

LArStraightElectrodes::LArStraightElectrodes(std::string strDetector)
{
  const LArVG4DetectorParameters* parameters = LArVG4DetectorParameters::GetInstance();
  if (parameters->GetValue("LArEMBPhiAtCurvature",0)>0.)  m_parity=0;  // first wave goes up
  else                                                    m_parity=1;  // first wave goes down

  if (s_theElectrodes==nullptr) {
    if (strDetector.empty())
      s_theElectrodes=new PhysicalVolumeAccessor("LAr::EMB::STAC",
                                                 "LAr::EMB::Electrode::Straight");
    else
      s_theElectrodes=new PhysicalVolumeAccessor(strDetector+"::LAr::EMB::STAC",
                                                 strDetector+"::LAr::EMB::Electrode::Straight");
  }
  m_filled=false;
  for (int stackid=0; stackid<14; stackid++) {
    for (int cellid=0; cellid<1024; cellid++) {
      m_xcent[cellid][stackid] = XCentEle(stackid,cellid);
      m_ycent[cellid][stackid] = YCentEle(stackid,cellid);
      double slant=SlantEle(stackid,cellid);
      m_cosu[cellid][stackid] = cos(slant);
      m_sinu[cellid][stackid] = sin(slant);
      m_halflength[cellid][stackid] = HalfLength(stackid,cellid);
    }
  }
  m_filled=true;
}

double LArStraightElectrodes::XCentEle(int stackid, int cellid) const
{
  if (m_filled) {
    return m_xcent[cellid][stackid];
  }
  else {
    const int id=cellid+stackid*10000;
    const G4VPhysicalVolume *pv=s_theElectrodes->GetPhysicalVolume(id);
    if (!pv) return 0.;
    const G4ThreeVector& tv=pv->GetTranslation();
    const G4VPhysicalVolume *pv2=s_theElectrodes->GetPhysicalVolume(1000000+id);
    if (!pv2) return tv.x();
    else {
      const G4ThreeVector& tv2=pv2->GetTranslation();
      const G4LogicalVolume* lv = pv->GetLogicalVolume();
      const G4Trap* trap = (G4Trap*) lv->GetSolid();
      const G4LogicalVolume* lv2 = pv2->GetLogicalVolume();
      const G4Trap* trap2 = (G4Trap*) lv2->GetSolid();
      double xl1=trap->GetYHalfLength1();
      double xl2=trap2->GetYHalfLength1();
      double x = (tv.x()*xl1+tv2.x()*xl2)/(xl1+xl2);
      return x;
    }

  }
}
double LArStraightElectrodes::YCentEle(int stackid, int cellid) const
{
  if (m_filled) {
    return m_ycent[cellid][stackid];
  }
  else {
    const int id=cellid+stackid*10000;
    const G4VPhysicalVolume *pv=s_theElectrodes->GetPhysicalVolume(id);
    if (!pv) return 0.;
    const G4ThreeVector& tv=pv->GetTranslation();
    const G4VPhysicalVolume *pv2=s_theElectrodes->GetPhysicalVolume(1000000+id);
    if (!pv2) return tv.y();
    else {
      const G4ThreeVector& tv2=pv2->GetTranslation();
      const G4LogicalVolume* lv = pv->GetLogicalVolume();
      const G4Trap* trap = (G4Trap*) lv->GetSolid();
      const G4LogicalVolume* lv2 = pv2->GetLogicalVolume();
      const G4Trap* trap2 = (G4Trap*) lv2->GetSolid();
      double xl1=trap->GetYHalfLength1();
      double xl2=trap2->GetYHalfLength1();
      double y = (tv.y()*xl1+tv2.y()*xl2)/(xl1+xl2);
      return y;

    }

  }
}
double LArStraightElectrodes::SlantEle(int stackid, int cellid) const
{
  const int id=cellid+stackid*10000;
  const G4VPhysicalVolume *pv=s_theElectrodes->GetPhysicalVolume(id);
  if (!pv) return 0.;
  const G4RotationMatrix *rm=pv->GetRotation();
  double Slant;
  Slant = (stackid%2 ==m_parity) ? 180*CLHEP::deg-(rm->thetaY()):(rm->thetaY())-180*CLHEP::deg;
  if((stackid%2 == m_parity) && (rm->phiY() > 0)) Slant = 360.*CLHEP::deg - Slant;
  if((stackid%2 == (1-m_parity)) && (rm->phiY() < 0)) Slant = - Slant;
  return Slant;
}
double LArStraightElectrodes::HalfLength(int stackid, int cellid) const
{
  if (m_filled) {
    return m_halflength[cellid][stackid];
  }
  else {
    const int id=cellid+stackid*10000;
    const G4VPhysicalVolume *pv=s_theElectrodes->GetPhysicalVolume(id);
    if (!pv) return 0.;
    const G4LogicalVolume* lv = pv->GetLogicalVolume();
    const G4Trap* trap = (G4Trap*) lv->GetSolid();
    const G4VPhysicalVolume *pv2=s_theElectrodes->GetPhysicalVolume(1000000+id);
    if (!pv2) return trap->GetYHalfLength1();
    else {
      const G4LogicalVolume* lv2 = pv2->GetLogicalVolume();
      const G4Trap* trap2 = (G4Trap*) lv2->GetSolid();
      return (trap->GetYHalfLength1()+trap2->GetYHalfLength1());
    }

  }
}
double LArStraightElectrodes::Cosu(int stackid, int cellid) const
{
  if (m_filled) {
    return m_cosu[cellid][stackid];
  }
  else {
    return cos(SlantEle(stackid,cellid));
  }
}
double LArStraightElectrodes::Sinu(int stackid, int cellid) const
{
  if (m_filled) {
    return m_sinu[cellid][stackid];
  }
  else {
    return sin(SlantEle(stackid,cellid));
  }
}
