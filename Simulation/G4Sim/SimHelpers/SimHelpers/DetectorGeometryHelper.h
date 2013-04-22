/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef DetectorGeometryHelper_H
#define DetectorGeometryHelper_H

#include <map>
#include <string>

enum DetectorType { Standard , GeoModel, Unknown=999 };

typedef std::map<std::string, DetectorType, std::less<std::string> > DetectorMap;

class DetectorGeometryHelper {
	friend class GeoModelDetectorFacility;
public:
	DetectorGeometryHelper() {}
	DetectorType GeometryType(std::string);
private:
	DetectorGeometryHelper(std::string, DetectorType);
	static void SetDetectorType(std::string, DetectorType);
	static DetectorMap theMap;
};

#endif
