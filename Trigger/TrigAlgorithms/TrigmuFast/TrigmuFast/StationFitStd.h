// -*- C++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


#ifndef PHYSICS_MU_STATIONFITSTD_H
#define PHYSICS_MU_STATIONFITSTD_H

#include "TrigmuFast/ProcessBase.h"
#include "TrigmuFast/Datatypes.h"
#include "TrigmuFast/MuMessageInterface.h"

#include <string>

class TrackFit;
class DigiPointer;
class MuFeature;
class SpointData;

class StationFitStd: public ProcessBase {

public:
    StationFitStd(MuMessageInterface& msg,
                  const TrackFit& track_fit,
                  LayerDigits (*digits)[MAXLAYER],
                  SpointData super_points[]);
    virtual ~StationFitStd(void);

public:
    virtual bool run(MuonFeatureDetails* det=0) = 0;
    virtual void printout(void) const;
    virtual unsigned short int number(void) const;

public:
    virtual std::string type(void) const;
    virtual std::string name(void) const = 0;

protected:
    MuMessageInterface* m_msg;
    const TrackFit& m_track_fit;
    LayerDigits (*m_digits)[MAXLAYER];
    SpointData* m_super_points;

private:
    static const std::string s_type;

};

inline unsigned short int
StationFitStd::number() const
{
    return 0x5;
}

inline std::string
StationFitStd::type(void) const
{
    return s_type;
}


#endif // PHYSICS_MU_STATIONFITSTD_H
