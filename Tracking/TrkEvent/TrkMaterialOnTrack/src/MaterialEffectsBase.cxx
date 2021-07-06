/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// MaterialEffectsBase.cxx (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

#include "TrkMaterialOnTrack/MaterialEffectsBase.h"
#include "GaudiKernel/MsgStream.h"
#include "TrkSurfaces/Surface.h"
#include <ostream>
#include <string>

Trk::MaterialEffectsBase::MaterialEffectsBase()
  : SurfacePtrHolderDetEl(nullptr)
  , m_tInX0(0.0)
  , m_typeFlags(0)
{}

Trk::MaterialEffectsBase::MaterialEffectsBase(
  double tInX0,
  const Surface& sf,
  const std::bitset<Trk::MaterialEffectsBase::NumberOfMaterialEffectsTypes>&
    typePattern)
  : SurfacePtrHolderDetEl(sf)
  , m_tInX0(tInX0)
  , m_typeFlags(typePattern.to_ulong())
{}

std::string
Trk::MaterialEffectsBase::dumpType() const
{
  std::string type;
  for (int i = 0; i < NumberOfMaterialEffectsTypes; ++i) {
    if ((1 << i) & m_typeFlags) {
      switch (i) {
        case Trk::MaterialEffectsBase::ScatteringEffects:
          type += "Scattering ";
          break;
        case Trk::MaterialEffectsBase::EnergyLossEffects:
          type += "EnergyLoss ";
          break;
        case Trk::MaterialEffectsBase::MaterialThickness:
          type += "UpdatorInput ";
          break;
        case Trk::MaterialEffectsBase::BremPoint:
          type += "BremPoint ";
          break;
        case Trk::MaterialEffectsBase::UsesMeasurement:
          type += "UsesMeasurement ";
          break;
        case Trk::MaterialEffectsBase::FittedMaterialEffects:
          type += "FittedMaterialEffects ";
          break;
        default:
          type += "Unknown ";
          break;
      }
    }
  }
  return type;
}

// Overload of << operator for MsgStream for debug output
MsgStream&
Trk::MaterialEffectsBase::dump(MsgStream& sl) const
{
  std::string name("MaterialEffects ");
  sl << name << "type        : " << dumpType() << endmsg;
  sl << name << "thickness/X0: " << thicknessInX0() << endmsg;
  sl << name << "Surface     : ";
  if (m_associatedSurface == nullptr) {
    sl << "NULL";
  } else {
    if (m_associatedSurface->associatedDetectorElement() != nullptr) {
      sl << "from detector Element";
    } else {
      sl << associatedSurface() << endmsg;
    }
  }
  sl << endmsg;
  return sl;
}
// Overload of << operator for std::ostream for debug output
std::ostream&
Trk::MaterialEffectsBase::dump(std::ostream& sl) const
{
  sl << "MaterialEffects type        : " << dumpType() << std::endl;
  sl << "MaterialEffects thickness/X0: " << thicknessInX0() << std::endl;
  sl << "MaterialEffects Surface     : ";
  if (m_associatedSurface == nullptr) {
    sl << "NULL";
  } else {
    if (m_associatedSurface->associatedDetectorElement() != nullptr) {
      sl << "from detector Element";
    } else {
      sl << associatedSurface() << std::endl;
    }
  }
  sl << std::endl;
  return sl;
}

