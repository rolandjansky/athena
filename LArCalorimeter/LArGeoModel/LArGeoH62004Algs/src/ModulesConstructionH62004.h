/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

#ifndef LARGEOH62004ALGS_MODULESCONSTRUCTIONH62004_H
#define LARGEOH62004ALGS_MODULESCONSTRUCTIONH62004_H

#include "GeoModelKernel/GeoFullPhysVol.h"
#include "GeoModelKernel/GeoPhysVol.h"
#include "GeoModelKernel/GeoLogVol.h"

#include "GeoModelInterfaces/StoredMaterialManager.h"
#include "StoreGate/StoreGateSvc.h"

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
           GeoLogVol* construct(const StoredMaterialManager* materialManager,
                                int side, int dir, int calo);
           GeoTrf::Transform3D position(int side, int dir, int calo);
                        //  side = 0 - left, 1 - right
                        //  dir  = 0 - side, 1 - up, 2 - back
                        //  calo = 0 - EMEC, 1 - HEC1,  2 - HEC2, 3 - FCAL1
                        //         4 - FCAL2, 5 - ColdTail

	   GeoFullPhysVol*  m_ModulesPhys;
	   StoreGateSvc * m_detectorStore = nullptr;

	   const LArGeoTB2004Options      *m_Options;

           struct LeakGeom {
             LeakGeom();
             float m_dX[NUM_LEAK],     m_dY[NUM_LEAK],     m_dZ[NUM_LEAK];
             float m_shiftX[NUM_LEAK], m_shiftY[NUM_LEAK], m_shiftZ[NUM_LEAK];
             float m_angleX[NUM_LEAK], m_angleY[NUM_LEAK], m_angleZ[NUM_LEAK];
           };
           static const LeakGeom s_leakGeom;

           int m_fcalVisLimit;
};

}

#endif // LARGEOH62004ALGS_MODULESCONSTRUCTIONH62004_H
