/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef __ModulesConstructionH62004_H__
#define __ModulesConstructionH62004_H__

#include "GeoModelKernel/GeoFullPhysVol.h"
#include "GeoModelKernel/GeoPhysVol.h"
#include "GeoModelKernel/GeoLogVol.h"

#include "StoreGate/DataHandle.h"
#include "GeoModelInterfaces/StoredMaterialManager.h"

class LArGeoTB2004Options;

namespace LArGeo {
   
#define NUM_LEAK 18

   class ModulesConstructionH62004 {
         public:
            ModulesConstructionH62004();
            virtual ~ModulesConstructionH62004();
            GeoVFullPhysVol* GetEnvelope();
         private:
	    ModulesConstructionH62004 (const ModulesConstructionH62004 &);
            ModulesConstructionH62004 & operator= (const ModulesConstructionH62004 &);

           int GetID(int side, int dir, int calo);
           GeoLogVol* construct(int side, int dir, int calo);
           HepGeom::Transform3D position(int side, int dir, int calo);
                        //  side = 0 - left, 1 - right
                        //  dir  = 0 - side, 1 - up, 2 - back
                        //  calo = 0 - EMEC, 1 - HEC1,  2 - HEC2, 3 - FCAL1
                        //         4 - FCAL2, 5 - ColdTail

	   GeoFullPhysVol*  m_ModulesPhys;
	   StoreGateSvc * m_detectorStore;
	   DataHandle<StoredMaterialManager> m_materialManager;

	   const LArGeoTB2004Options      *m_Options;

           static float dX[NUM_LEAK], dY[NUM_LEAK], dZ[NUM_LEAK];
           static float shiftX[NUM_LEAK], shiftY[NUM_LEAK], shiftZ[NUM_LEAK];
           static float angleX[NUM_LEAK], angleY[NUM_LEAK], angleZ[NUM_LEAK];

           int m_fcalVisLimit;

};

}

#endif // __ModulesConstructionH62004_H__
