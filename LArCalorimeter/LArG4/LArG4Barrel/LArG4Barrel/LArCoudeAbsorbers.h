/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef LArCoudeAbsorbers_H
#define LArCoudeAbsorbers_H

#include "LArG4Barrel/PhysicalVolumeAccessor.h"
#include <string>

class LArCoudeAbsorbers {
private:
	static PhysicalVolumeAccessor* s_theCoudes;
        double m_xcent[1024][15];
        double m_ycent[1024][15];
        double m_phirot[1024][15];
        bool m_filled;
        static LArCoudeAbsorbers* s_instance;
public:
        static LArCoudeAbsorbers* GetInstance(std::string strDetector="") ;
	double XCentCoude(int stackid, int cellid) ;
	double YCentCoude(int stackid, int cellid) ;
        double PhiRot(int stackid, int cellid);
protected:
	LArCoudeAbsorbers(std::string strDetector="") ;
};

#endif
