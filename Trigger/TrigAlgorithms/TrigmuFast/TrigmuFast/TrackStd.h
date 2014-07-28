// -*- C++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


#ifndef PHYSICS_MU_TRACKSTD_H
#define PHYSICS_MU_TRACKSTD_H

#include "TrigmuFast/ProcessBase.h"
#include "TrigmuFast/MuMessageInterface.h"

class TriggerData;
class TrackFit;
class RoadRegions;

class TrackStd: public ProcessBase {

public:
    TrackStd(MuMessageInterface& msg,
             const TriggerData& trig_data, TrackFit& track,
	     RoadRegions& roads);
    virtual ~TrackStd(void);

public:
    virtual bool run(MuonFeatureDetails* det=0) = 0;
    virtual void printout(void) const;
    virtual unsigned short int number(void) const;

public:
    virtual std::string type(void) const;
    virtual std::string name(void) const = 0;

protected:
    MuMessageInterface* m_msg;
  //    const RpcTriggerData& rpc_roi(void) const;
  //    TrackFit& track(void) const;

protected:
    const TriggerData& m_trig_data;
    TrackFit&    m_track;
    RoadRegions& m_roads;

protected:
    void setPhi(double X1, double Y1, double X2, double Y2);
    void setPhiDir(double phi1, double X1, double phi2, double X2);

private:
    static const std::string s_type;

};

inline unsigned short int
TrackStd::number() const
{
    return 0x1;
}

inline std::string
TrackStd::type(void) const
{
    return s_type;
}

/*
inline const RpcTriggerData&
TrackStd::rpc_roi(void) const
{
    return m_rpc_roi;
}

inline TrackFit&
TrackStd::track(void) const
{
    return m_track;
}
*/


#endif // PHYSICS_MU_TRACKSTD_H
