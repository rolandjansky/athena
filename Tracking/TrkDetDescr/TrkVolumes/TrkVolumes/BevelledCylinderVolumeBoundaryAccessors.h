/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// BevelledCylinderVolumeBoundaryAccessors.h, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

#ifndef TRKVOLUMES_BEVELLEDCYLINDERVOLUMEBOUNDARYACCESSORS_H
#define TRKVOLUMES_BEVELLEDCYLINDERVOLUMEBOUNDARYACCESSORS_H


#include "TrkDetDescrUtils/ObjectsAccessor.h"

namespace Trk {

  /** @enum BevelledCylinderAccessorType */
   enum  BevelledCylinderAccessorType {
         BevelledCylinderZincrease    = 0,   //!< Cylinder hit, then pos. Z face
         BevelledCylinderZdecrease    = 1,   //!< Cylinder hit, the neg. Z face
         BevelledCylinderPositiveFace = 2,   //!< Pos. Z face, Cylinder, neg. Z face
         BevelledCylinderNegativeFace = 3    //!< Neg. Z face, Cylinder, pos. Z face

   };

  /** @enum BevelledTubeAccessorType */
    enum BevelledTubeAccessorType {

           BevelledTubeRincreaseZincrease         = 0,   //!< Accessor type [ 2,1,0,3 ]
           BevelledTubeRincreaseZdecrease         = 1,   //!< Accessor type [ 2,0,1,3 ]
           BevelledTubeZincreaseRincrease         = 2,   //!< Accessor type [ 1,2,0,3 ]
           BevelledTubeZdecreaseRincrease         = 3,   //!< Accessor type [ 0,2,1,3 ]
           BevelledTubeRdecreaseZincrease         = 4,   //!< Accessor type [ 3,1,0,2 ]
           BevelledTubeRdecreaseZdecrease         = 5,   //!< Accessor type [ 3,0,1,2 ]
           BevelledTubeZincreaseRdecrease         = 6,   //!< Accessor type [ 1,3,0,2 ]
           BevelledTubeZdecreaseRdecrease         = 7,    //!< Accessor type [ 0,3,1,2 ]
           BevelledTubeOutsideRminRdecrease       = 8,   //!< Accessor type [ 3,1,0,2]  - inverse case
           BevelledTubeOutsideRmaxRincrease       = 9,   //!< Accessor type [ 2,1,0,3 ] - inverse case
           BevelledTubeOutsideZminZdecrease       = 10,  //!< Accessor type [ 0,3,2,1 ] - inverse case
           BevelledTubeOutsideZmaxZincrease       = 11   //!< Accessor type [ 1,3,2,0 ] - inverse case

   };

  /** @enum SectoralBevelledCylinderAccessorType
    \todo implement correcly when needed
     */
    enum SectoralBevelledCylinderAccessorType  {
          StandardSectoralBevelledCylinder = 0
      };

  /** @enum SectoralTubeAccessorType
    \todo implement correcly when needed
     */
    enum SectoralBevelledTubeAccessorType  {
          StandardSectoralBevelledTube = 0
      };


   /** @class BevelledCylinderVolumeBoundaryAccessors

      Different Accessor types for a cylindrical Volume,
      if returs accessors to the volume boundary surfaces
      to guarantee "best-guess" navigation, and follow-ups.

       @author Andreas.Salzburger@cern.ch, marcin.wolter@cern.ch
     */

   class BevelledCylinderVolumeBoundaryAccessors {
      public:
       /**Default Constructor*/
       BevelledCylinderVolumeBoundaryAccessors();

       /**Default Constructor*/
       ~BevelledCylinderVolumeBoundaryAccessors();

       /** Return the accessor for the cylindrical case */
       const ThreeObjectsAccessor& bevelledcylinderAccessor(BevelledCylinderAccessorType cat) const;

       /** Return the accessor for the tube case */
       const FourObjectsAccessor& bevelledtubeAccessor(BevelledTubeAccessorType tat) const;

       /** Return the accessor for the sectoral cylinciracl case */
       const FiveObjectsAccessor& sectoralBevelledCylinderAccessor(SectoralBevelledCylinderAccessorType scat) const;

       /** Return the accessor for the sectoral tube case */
       const SixObjectsAccessor& sectoralBevelledTubeAccessor(SectoralBevelledTubeAccessorType stat) const;

      private:

        std::array<ThreeObjectsAccessor,4> m_bevelledcylinderAccessors{};
        std::array<FourObjectsAccessor,12> m_bevelledtubeAccessors;
        std::array<FiveObjectsAccessor,1>  m_sectoralBevelledCylinderAccessors{};
        std::array<SixObjectsAccessor,1>   m_sectoralBevelledTubeAccessors{};

   };

inline const ThreeObjectsAccessor& BevelledCylinderVolumeBoundaryAccessors::bevelledcylinderAccessor(BevelledCylinderAccessorType cat) const
 { return m_bevelledcylinderAccessors[cat]; }

inline const FourObjectsAccessor& BevelledCylinderVolumeBoundaryAccessors::bevelledtubeAccessor(BevelledTubeAccessorType tat) const
 { return m_bevelledtubeAccessors[tat]; }

inline const FiveObjectsAccessor& BevelledCylinderVolumeBoundaryAccessors::sectoralBevelledCylinderAccessor(SectoralBevelledCylinderAccessorType scat) const
 { return m_sectoralBevelledCylinderAccessors[scat]; }

inline const SixObjectsAccessor& BevelledCylinderVolumeBoundaryAccessors::sectoralBevelledTubeAccessor(SectoralBevelledTubeAccessorType stat) const
 { return m_sectoralBevelledTubeAccessors[stat]; }


}


# endif
