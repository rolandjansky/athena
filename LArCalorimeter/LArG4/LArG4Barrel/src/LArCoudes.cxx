/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "LArCoudes.h"

PhysicalVolumeAccessor* LArCoudes::s_theCoudes=nullptr;

LArCoudes::LArCoudes(std::string strDetector)
{
  if (s_theCoudes==nullptr)
    {
      if(strDetector.empty())
        {
          s_theCoudes=
            new PhysicalVolumeAccessor("LAr::EMB::STAC",
                                       "LAr::EMB::Electrode::CornerDownFold");
          s_theCoudes->SetPhysicalVolumeList("LAr::EMB::Electrode::CornerUpFold");
        }
      else
        {
          s_theCoudes=
            new PhysicalVolumeAccessor(strDetector+"::LAr::EMB::STAC",
                                       strDetector+"::LAr::EMB::Electrode::CornerDownFold");
          s_theCoudes->SetPhysicalVolumeList(strDetector+"::LAr::EMB::Electrode::CornerUpFold");
        }
    }
}
double LArCoudes::XCentCoude(int stackid, int cellid) const
{
  const int id=cellid+stackid*10000;
  const G4VPhysicalVolume *pv=s_theCoudes->GetPhysicalVolume(id);
  if (!pv) std::abort();
  const G4ThreeVector& tv=pv->GetTranslation();
  return tv.x();
}
double LArCoudes::YCentCoude(int stackid, int cellid) const
{
  const int id=cellid+stackid*10000;
  const G4VPhysicalVolume *pv=s_theCoudes->GetPhysicalVolume(id);
  if (!pv) std::abort();
  const G4ThreeVector& tv=pv->GetTranslation();
  return tv.y();
}
