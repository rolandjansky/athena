// -*- C++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


#ifndef PHYSICS_MU_PATREC_H
#define PHYSICS_MU_PATREC_H

#include "TrigmuFast/SequenceBase.h"
#include "TrigmuFast/Datatypes.h"


class MuMessageInterface;
class MuServiceInterface;

class MdtPatRec: public SequenceBase {

public:
    MdtPatRec(MuMessageInterface& msg,MuServiceInterface& svc,
              const TrackFit& track_fit, const RoadRegions& muon_roads,
	      const MuData& mu_data);
    virtual ~MdtPatRec(void);

public:
    virtual MUFAST::ErrorCode start(MuonFeatureDetails* det=0);

public:
    virtual std::string type(void) const;
    virtual std::string name(void) const;

public:
    DigiPointer digits(void);
    DigiPointer overlap(void);

protected:
    void fill_data(MuonFeatureDetails* det=0) const;
    
protected:                      // sevices
    MuMessageInterface* m_msg;
    MuServiceInterface* m_svc;

protected:                      // input
    const TrackFit& m_track_fit;
    const RoadRegions& m_muon_roads;
    const MuData& m_data;

protected:                      // output
    LayerDigits m_digits[MAXSTATION][MAXLAYER];
    LayerDigits m_overlap[MAXSTATION][MAXLAYER];

protected:
    static const std::string s_type;
    static const std::string s_name;

};

inline std::string
MdtPatRec::type(void) const
{
    return s_type;
}

inline std::string
MdtPatRec::name(void) const
{
    return s_name;
}

inline DigiPointer
MdtPatRec::digits(void)
{
    DigiPointer dp;

    dp.digi = m_digits;

    return dp;
}

inline DigiPointer
MdtPatRec::overlap(void)
{
    DigiPointer dp;

    dp.digi = m_overlap;

    return dp;
}


#endif // PHYSICS_MU_PATREC_H

