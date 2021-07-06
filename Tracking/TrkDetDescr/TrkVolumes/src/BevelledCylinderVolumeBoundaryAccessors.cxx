/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// BevelledCylinderVolumeBoundaryAccessors.cxx, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

// Trk
#include "TrkVolumes/BevelledCylinderVolumeBoundaryAccessors.h"

Trk::BevelledCylinderVolumeBoundaryAccessors::
  BevelledCylinderVolumeBoundaryAccessors()
{
  //-------------------------------------------------------------
  // the cylinder case
  // CylinderZincrease = 0
  m_bevelledcylinderAccessors[Trk::BevelledCylinderZincrease] =
    Trk::ThreeObjectsAccessor{ 2, 1, 0 };
  // CylinderZincrease = 1
  m_bevelledcylinderAccessors[Trk::BevelledCylinderZdecrease] =
    Trk::ThreeObjectsAccessor{ 2, 0, 1 };
  // CylinderZincrease = 2
  m_bevelledcylinderAccessors[Trk::BevelledCylinderPositiveFace] =
    Trk::ThreeObjectsAccessor{ 1, 2, 0 };
  // CylinderZincrease = 3
  m_bevelledcylinderAccessors[Trk::BevelledCylinderNegativeFace] =
    Trk::ThreeObjectsAccessor{ 0, 2, 1 };
  //-------------------------------------------------------------
  // the tube case
  //(A) non-inverse cases
  // RincreaseZincrease = 0
  m_bevelledtubeAccessors[Trk::BevelledTubeRincreaseZincrease] =
    Trk::FourObjectsAccessor{ { 2, 1, 0, 3 }, false };
  // RincreaseZdecrease = 1
  m_bevelledtubeAccessors[Trk::BevelledTubeRincreaseZdecrease] =
    Trk::FourObjectsAccessor{ { 2, 0, 1, 3 }, false };
  // ZincreaseRincrease = 2
  m_bevelledtubeAccessors[Trk::BevelledTubeZincreaseRincrease] =
    Trk::FourObjectsAccessor{ { 1, 2, 0, 3 }, false };
  // ZdecreaseRincrease = 3
  m_bevelledtubeAccessors[Trk::BevelledTubeZdecreaseRincrease] =
    Trk::FourObjectsAccessor{ { 0, 2, 1, 3 }, false };
  //  RdecreaseZincrease = 4
  m_bevelledtubeAccessors[Trk::BevelledTubeRdecreaseZincrease] =
    Trk::FourObjectsAccessor{ { 3, 1, 0, 2 }, false };
  // RdecreaseZdecrease = 5
  m_bevelledtubeAccessors[Trk::BevelledTubeRdecreaseZdecrease] =
    Trk::FourObjectsAccessor{ { 3, 0, 1, 2 }, false };
  // ZincreaseRdecrease = 6
  m_bevelledtubeAccessors[Trk::BevelledTubeZincreaseRdecrease] =
    Trk::FourObjectsAccessor{ { 1, 3, 0, 2 }, false };
  // ZdecreaseRdecrease = 7
  m_bevelledtubeAccessors[Trk::BevelledTubeZdecreaseRdecrease] =
    Trk::FourObjectsAccessor{ { 0, 3, 1, 2 }, false };
  //(B) inverse cases
  // TubeOutsideRminRdecrease = 8
  m_bevelledtubeAccessors[Trk::BevelledTubeOutsideRminRdecrease] =
    Trk::FourObjectsAccessor{ { 3, 1, 0, 2 }, true };
  // TubeOutsideRmaxRincrease = 9
  m_bevelledtubeAccessors[Trk::BevelledTubeOutsideRmaxRincrease] =
    Trk::FourObjectsAccessor{ { 2, 1, 0, 3 }, true };
  // TubeOutsideZminZdecrease = 10
  m_bevelledtubeAccessors[Trk::BevelledTubeOutsideZminZdecrease] =
    Trk::FourObjectsAccessor{ { 0, 3, 2, 1 }, true };
  // TubeOutsideZmaxZincrease = 11
  m_bevelledtubeAccessors[Trk::BevelledTubeOutsideZmaxZincrease] =
    Trk::FourObjectsAccessor{ { 1, 3, 2, 0 }, true };

  //-------------------------------------------------------------
  // the sectoral cylinder case
  m_sectoralBevelledCylinderAccessors[Trk::StandardSectoralBevelledCylinder] =
    Trk::FiveObjectsAccessor{ 0, 1, 2, 3, 4 };
  //-------------------------------------------------------------
  // the sectoral tube case
  m_sectoralBevelledTubeAccessors[Trk::StandardSectoralBevelledTube] =
    Trk::SixObjectsAccessor{ 0, 1, 2, 3, 4, 5 };
  //-------------------------------------------------------------
}

Trk::BevelledCylinderVolumeBoundaryAccessors::
  ~BevelledCylinderVolumeBoundaryAccessors()
{}
