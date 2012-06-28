/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// HECClampConstruction.h
// 
// Construct Outer Conneting Bars and Rails for HEC
// and apply them
//
// M. Fincke  25.Oct2007
//
#ifndef __HECClampConstruction_H__
#define __HECClampConstruction_H__

#include <string>

class GeoFullPhysVol;
class GeoPhysVol;

namespace LArGeo 
{

  class HECClampConstruction 
    {
    public:
      HECClampConstruction(bool front=true, bool posZSide=true);
      virtual ~HECClampConstruction();
      void AddClamps(GeoFullPhysVol*physiHECWheel);      


    private:
      GeoPhysVol* GetClampingBar(bool rail=false,bool left=false);
      bool     m_front;
      bool     m_rail;
      bool     m_left;
      bool     m_posZSide;
      int      m_moduleNumber   ;
      double   m_moduleRouter   ;
      double   m_modulePhistart ;
      double   m_rOuter         ;
      double   m_moduleDeltaPhi ;
    };
  
}
#endif // __HECClampConstruction_H__
