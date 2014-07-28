// -*- C++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


#ifndef PHYSICS_MU_ROADSDEFADM_H
#define PHYSICS_MU_ROADSDEFADM_H

#include "TrigmuFast/RoadsDefStd.h"

class TriggerData;
class TrackFit;
class RoadRegions;

class RoadsDefADM: public RoadsDefStd {
  
public:
    RoadsDefADM(MuMessageInterface& msg, 
                const TriggerData& trigger_data, const TrackFit& track,
	        RoadRegions& roads,const MDTGeometry* mdtgeo,
		const MuRoads* muRoads);
    virtual ~RoadsDefADM(void);

public:
    virtual bool run(MuonFeatureDetails* det=0);

public:
    virtual std::string name(void) const;
    
private:
    bool processInner    (int PhysicSector,int sector,int type,int tr);
    bool processMiddle   (int PhysicSector,int sector,int type,int tr);
    bool processOuter    (int PhysicSector,int sector,int type,int tr);
    bool processExtended (int PhysicSector,int sector,int type,int tr);
    
    int road_parameters(float Rmin, float Rmax, char s, int tr);

protected:
    static const std::string s_name;

};

inline std::string
RoadsDefADM::name(void) const
{
    return s_name;
}



#endif // PHYSICS_MU_ROADSDEFADM_H
