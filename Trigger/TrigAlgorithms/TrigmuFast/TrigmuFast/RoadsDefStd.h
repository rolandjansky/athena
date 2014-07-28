// -*- C++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


#ifndef PHYSICS_MU_ROADSDEFSTD_H
#define PHYSICS_MU_ROADSDEFSTD_H

#include "TrigmuFast/ProcessBase.h"
#include "TrigmuFast/MuMessageInterface.h"

class TriggerData;
class TrackFit;
class RoadRegions;
class MDTGeometry;
class MuRoads;

class RoadsDefStd: public ProcessBase {

public:
    RoadsDefStd(MuMessageInterface& msg, const TriggerData& trigger_data,
                const TrackFit& track, RoadRegions& roads,
		const MDTGeometry* mdtgeo, const MuRoads* muRoads);
    virtual ~RoadsDefStd(void);

public:
    virtual bool run(MuonFeatureDetails* det=0) = 0;
    virtual void printout(void) const;
    virtual unsigned short int number(void) const;

protected:
    void get_address(const char sys, const char tag, int StationPhi,
                     int& station,int& special,int& type ) const;

public:
    virtual std::string type(void) const;
    virtual std::string name(void) const = 0;

protected:
    MuMessageInterface* m_msg;

protected:
    const TriggerData&  m_trig_data;
    const TrackFit&     m_track;
    RoadRegions&        m_roads;
    
    const MDTGeometry*  m_mdtgeo;
    const MuRoads*      m_muRoads;

protected:
    bool find_barrel_road_dim(int position,int type,float aw, float bw,
                              float Rmin,float Rmax,float *Zmin,float *Zmax);
    bool find_endcap_road_dim(float road,float aw, float bw,
                              float Zmin,float Zmax,float *Rmin,float *Rmax);

    void find_eta_min_max(float Zmin, float Rmin, float Zmax, float Rmax,
                          float& etaMin, float& etaMax);
    double compute_endcap_phi(int station, int tr);
    double compute_barrel_phi(int station, int tr);
    
private:
    static const std::string s_type;

};

inline unsigned short int
RoadsDefStd::number() const
{
    return 0x2;
}

inline std::string
RoadsDefStd::type(void) const
{
    return s_type;
}


inline void
RoadsDefStd::get_address(const char sys, const char tag, int StationPhi,
                         int& station,int& special, int &type) const
{
    station = 0;
    special = 0;
    
    if (sys=='B')
    {
        if(tag=='M') station = 1;
        if(tag=='O') station = 2;
        if(type == 0 && (StationPhi == 6 || StationPhi == 8)) 
            { special = 1; return; }
        if(type == 1 && (StationPhi == 6 || StationPhi == 7)) 
            { special = 1; return; }
    }
    
    if (sys=='E')
    {
        if(tag=='M') station = 1;
        if(tag=='O') station = 2;
	if(tag=='E') station = 3;
    }
}


#endif // PHYSICS_MU_ROADSDEFSTD_H
