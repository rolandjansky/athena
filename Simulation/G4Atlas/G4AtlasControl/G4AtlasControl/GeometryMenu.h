/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef GeometryMenu_H
#define GeometryMenu_H

#include "FadsGeometry/DetectorFacility.h"
#include "G4AtlasControl/SimpleMenu.h"
#include <string>

class GeometryMenu : public SimpleMenu {
friend class SimControl;
private:
	GeometryMenu();
public:
	static void ListDetectorFacilities();
	FADS::DetectorFacility* GetDetectorFacility(const std::string, const std::string) const;
	void PrintDetectorVolumes(const std::string detName);
	void PrintVolumes();
	void AssignSD(const std::string volName, const std::string sdName);
	void SetMaxStep(const std::string volName, double cut, const std::string part="*");
	void SetMinEkine(const std::string volName, double cut, const std::string part="*");
	void SetMaxTrackLength(const std::string volName, double cut, const std::string part="*");
	void SetMaxTime(const std::string volName, double cut, const std::string part="*");
	void SetMinRange(const std::string volName, double cut, const std::string part="*");
	void SetGeometryCheck(int i);
};

#endif
