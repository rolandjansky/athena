/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// CylinderVolumeBoundaryAccessors.h, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

#ifndef TRKVOLUMES_CYLINDERVOLUMEBOUNDARYACCESSORS_H
#define TRKVOLUMES_CYLINDERVOLUMEBOUNDARYACCESSORS_H

#include "TrkDetDescrUtils/ObjectsAccessor.h"

namespace Trk {

/** @enum CylinderAccessorType */
enum CylinderAccessorType {
  CylinderZincrease = 0,     //!< Cylinder hit, then pos. Z face
  CylinderZdecrease = 1,     //!< Cylinder hit, the neg. Z face
  CylinderPositiveFace = 2,  //!< Pos. Z face, Cylinder, neg. Z face
  CylinderNegativeFace = 3   //!< Neg. Z face, Cylinder, pos. Z face

};

/** @enum TubeAccessorType */
enum TubeAccessorType {

  TubeRincreaseZincrease = 0,     //!< Accessor type [ 2,1,0,3 ]
  TubeRincreaseZdecrease = 1,     //!< Accessor type [ 2,0,1,3 ]
  TubeZincreaseRincrease = 2,     //!< Accessor type [ 1,2,0,3 ]
  TubeZdecreaseRincrease = 3,     //!< Accessor type [ 0,2,1,3 ]
  TubeRdecreaseZincrease = 4,     //!< Accessor type [ 3,1,0,2 ]
  TubeRdecreaseZdecrease = 5,     //!< Accessor type [ 3,0,1,2 ]
  TubeZincreaseRdecrease = 6,     //!< Accessor type [ 1,3,0,2 ]
  TubeZdecreaseRdecrease = 7,     //!< Accessor type [ 0,3,1,2 ]
  TubeOutsideRminRdecrease = 8,   //!< Accessor type [ 3,1,0,2]  - inverse case
  TubeOutsideRmaxRincrease = 9,   //!< Accessor type [ 2,1,0,3 ] - inverse case
  TubeOutsideZminZdecrease = 10,  //!< Accessor type [ 0,3,2,1 ] - inverse case
  TubeOutsideZmaxZincrease = 11   //!< Accessor type [ 1,3,2,0 ] - inverse case

};

/** @enum SectoralCylinderAccessorType
  \todo implement correcly when needed
   */
enum SectoralCylinderAccessorType { StandardSectoralCylinder = 0 };

/** @enum SectoralTubeAccessorType
  \todo implement correcly when needed
   */
enum SectoralTubeAccessorType { StandardSectoralTube = 0 };

/** @class CylinderVolumeBoundaryAccessors

   Different Accessor types for a cylindrical Volume,
   if returs accessors to the volume boundary surfaces
   to guarantee "best-guess" navigation, and follow-ups.

    @author Andreas.Salzburger@cern.ch
  */

class CylinderVolumeBoundaryAccessors {
 public:
  /**Default Constructor*/
  CylinderVolumeBoundaryAccessors();

  /**Default Constructor*/
  ~CylinderVolumeBoundaryAccessors();

  /** Return the accessor for the cylindrical case */
  const ThreeObjectsAccessor& cylinderAccessor(CylinderAccessorType cat) const;

  /** Return the accessor for the tube case */
  const FourObjectsAccessor& tubeAccessor(TubeAccessorType tat) const;

  /** Return the accessor for the sectoral cylinciracl case */
  const FiveObjectsAccessor& sectoralCylinderAccessor(
      SectoralCylinderAccessorType scat) const;

  /** Return the accessor for the sectoral tube case */
  const SixObjectsAccessor& sectoralTubeAccessor(
      SectoralTubeAccessorType stat) const;

 private:
  std::array<ThreeObjectsAccessor, 4> m_cylinderAccessors{};
  std::array<FourObjectsAccessor, 12> m_tubeAccessors;
  std::array<FiveObjectsAccessor, 1> m_sectoralCylinderAccessors{};
  std::array<SixObjectsAccessor, 1> m_sectoralTubeAccessors{};
};

inline const ThreeObjectsAccessor&
CylinderVolumeBoundaryAccessors::cylinderAccessor(
    CylinderAccessorType cat) const {
  return m_cylinderAccessors[cat];
}

inline const FourObjectsAccessor& CylinderVolumeBoundaryAccessors::tubeAccessor(
    TubeAccessorType tat) const {
  return m_tubeAccessors[tat];
}

inline const FiveObjectsAccessor&
CylinderVolumeBoundaryAccessors::sectoralCylinderAccessor(
    SectoralCylinderAccessorType scat) const {
  return m_sectoralCylinderAccessors[scat];
}

inline const SixObjectsAccessor&
CylinderVolumeBoundaryAccessors::sectoralTubeAccessor(
    SectoralTubeAccessorType stat) const {
  return m_sectoralTubeAccessors[stat];
}

}  // namespace Trk

#endif
