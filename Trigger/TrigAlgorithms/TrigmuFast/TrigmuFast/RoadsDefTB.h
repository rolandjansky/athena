// -*- C++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


#ifndef PHYSICS_MU_ROADSDEFTB_H
#define PHYSICS_MU_ROADSDEFTB_H

#include "TrigmuFast/RoadsDefStd.h"

class TriggerData;
class TrackFit;
class RoadRegions;

class RoadsDefTB: public RoadsDefStd {
  
public:
    RoadsDefTB(MuMessageInterface& msg, 
                const TriggerData& rpc_data, const TrackFit& track,
	        RoadRegions& roads,const MDTGeometry* mdtgeo,
		const MuRoads* muRoads);
    virtual ~RoadsDefTB(void);

public:
    virtual bool run(MuonFeatureDetails* det=0);
    bool processInner    (int PhysicSector,int sector,int type,int tr);
    bool processMiddle   (int PhysicSector,int sector,int type,int tr);
    bool processOuter    (int PhysicSector,int sector,int type,int tr);
    bool processExtended (int PhysicSector,int sector,int type,int tr);

public:
    virtual std::string name(void) const;

protected:
    static const std::string s_name;

};

inline std::string
RoadsDefTB::name(void) const
{
    return s_name;
}



#endif // PHYSICS_MU_ROADSDEFTB_H
