/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// CylinderVolumeBoundaryAccessors.cxx, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

//Trk
#include "TrkVolumes/CylinderVolumeBoundaryAccessors.h"
#include "TrkVolumes/BoundarySurfaceFace.h"


Trk::CylinderVolumeBoundaryAccessors::CylinderVolumeBoundaryAccessors()
{
  //------------------------------------------------------------- 
  // the cylinder case
      // CylinderZincrease = 0
      m_cylinderAccessors[Trk::CylinderZincrease]
          = Trk::ThreeObjectsAccessor(Trk::cylinderCover,Trk::positiveFaceXY,Trk::negativeFaceXY);
      // CylinderZincrease = 1
      m_cylinderAccessors[Trk::CylinderZdecrease]
          = Trk::ThreeObjectsAccessor(Trk::cylinderCover,Trk::negativeFaceXY,Trk::positiveFaceXY);
      // CylinderZincrease = 2
      m_cylinderAccessors[Trk::CylinderPositiveFace]
       = Trk::ThreeObjectsAccessor(Trk::positiveFaceXY,Trk::cylinderCover,Trk::negativeFaceXY);
      // CylinderZincrease = 3
      m_cylinderAccessors[Trk::CylinderNegativeFace]
       = Trk::ThreeObjectsAccessor(Trk::negativeFaceXY,Trk::cylinderCover,Trk::positiveFaceXY);
  //------------------------------------------------------------- 
  // the tube case
     //(A) non-inverse cases
      // RincreaseZincrease = 0
      m_tubeAccessors[Trk::TubeRincreaseZincrease] 
        = Trk::FourObjectsAccessor(Trk::tubeOuterCover,Trk::positiveFaceXY,Trk::negativeFaceXY,Trk::tubeInnerCover);      
      // RincreaseZdecrease = 1
      m_tubeAccessors[Trk::TubeRincreaseZdecrease]
       = Trk::FourObjectsAccessor(Trk::tubeOuterCover,Trk::negativeFaceXY,Trk::positiveFaceXY,Trk::tubeInnerCover);      
      // ZincreaseRincrease = 2
      m_tubeAccessors[Trk::TubeZincreaseRincrease]
       = Trk::FourObjectsAccessor(Trk::positiveFaceXY,Trk::tubeOuterCover,Trk::negativeFaceXY,Trk::tubeInnerCover);      
      // ZdecreaseRincrease = 3
      m_tubeAccessors[Trk::TubeZdecreaseRincrease]
       = Trk::FourObjectsAccessor(Trk::negativeFaceXY,Trk::tubeOuterCover,Trk::positiveFaceXY,Trk::tubeInnerCover);      
      //  RdecreaseZincrease = 4
      m_tubeAccessors[Trk::TubeRdecreaseZincrease]
       = Trk::FourObjectsAccessor(Trk::tubeInnerCover,Trk::positiveFaceXY,Trk::negativeFaceXY,Trk::tubeOuterCover);      
      // RdecreaseZdecrease = 5
      m_tubeAccessors[Trk::TubeRdecreaseZdecrease]
       = Trk::FourObjectsAccessor(Trk::tubeInnerCover,Trk::negativeFaceXY,Trk::positiveFaceXY,Trk::tubeOuterCover);      
      // ZincreaseRdecrease = 6
      m_tubeAccessors[Trk::TubeZincreaseRdecrease]
       = Trk::FourObjectsAccessor(Trk::positiveFaceXY,Trk::tubeInnerCover,Trk::negativeFaceXY,Trk::tubeOuterCover);      
      // ZdecreaseRdecrease = 7
      m_tubeAccessors[Trk::TubeZdecreaseRdecrease]
       = Trk::FourObjectsAccessor(Trk::negativeFaceXY,Trk::tubeInnerCover,Trk::positiveFaceXY,Trk::tubeOuterCover);
    //(B) inverse cases
      // TubeOutsideRminRdecrease = 8
      m_tubeAccessors[Trk::TubeOutsideRminRdecrease]
       = Trk::FourObjectsAccessor(Trk::tubeInnerCover,Trk::positiveFaceXY,Trk::negativeFaceXY,Trk::tubeOuterCover, true);
      // TubeOutsideRmaxRincrease = 9
      m_tubeAccessors[Trk::TubeOutsideRmaxRincrease]
       = Trk::FourObjectsAccessor(Trk::tubeOuterCover,Trk::positiveFaceXY,Trk::negativeFaceXY,Trk::tubeInnerCover, true);
      // TubeOutsideZminZdecrease = Trk::positiveFaceXYTrk::negativeFaceXY
      m_tubeAccessors[Trk::TubeOutsideZminZdecrease]
       = Trk::FourObjectsAccessor(Trk::negativeFaceXY,Trk::tubeInnerCover,Trk::tubeOuterCover,Trk::positiveFaceXY, true);
      // TubeOutsideZmaxZincrease = Trk::positiveFaceXYTrk::positiveFaceXY
      m_tubeAccessors[Trk::TubeOutsideZmaxZincrease]
       = Trk::FourObjectsAccessor(Trk::positiveFaceXY,Trk::tubeInnerCover,Trk::tubeOuterCover,Trk::negativeFaceXY, true);
           

  //------------------------------------------------------------- 
  // the sectoral cylinder case
      m_sectoralCylinderAccessors[Trk::StandardSectoralCylinder]
       = Trk::FiveObjectsAccessor(Trk::negativeFaceXY,Trk::positiveFaceXY,Trk::tubeOuterCover,Trk::tubeInnerCover,4);
  //------------------------------------------------------------- 
  // the sectoral tube case
      m_sectoralTubeAccessors[Trk::StandardSectoralTube]
       = Trk::SixObjectsAccessor(Trk::negativeFaceXY,Trk::positiveFaceXY,Trk::tubeOuterCover,Trk::tubeInnerCover,4,5);
  //-------------------------------------------------------------      
}

Trk::CylinderVolumeBoundaryAccessors::~CylinderVolumeBoundaryAccessors()
{}
