/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef PhysicsRegionProxy_H
#define PhysicsRegionProxy_H

// Proxy used to deal with Regions (on the G4 side): to be used from the Python
// interface...

#include "G4Region.hh"
#include <string>

class PhysicsRegionProxy {
public:
	PhysicsRegionProxy();
	PhysicsRegionProxy(G4Region *);
	PhysicsRegionProxy(const std::string, G4Region *);
	void addVolume(const std::string);
	void setCut(double, const std::string);
	void setRegion(G4Region *);
	void setName(const std::string);
	std::string getName();
private:
	std::string regionName;
	G4Region *theRegion;
};

#endif
