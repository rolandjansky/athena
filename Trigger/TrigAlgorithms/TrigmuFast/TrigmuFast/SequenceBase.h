// -*- C++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


#ifndef PHYSICS_MU_SEQUENCEBASE_H
#define PHYSICS_MU_SEQUENCEBASE_H


#include <string>

namespace MUFAST {

enum ErrorCode
{
    NO_ERROR,
    BAD_ROI_DELIVERED,
    EMPTY_ROIVEC_DELIVERED,
    TGC_GEOMETRY_MISCONFIGURED,
    BAD_RPC_ROI_INDEXING,
    TGC_CONTAINER_UNAVAILABLE,
    TGC_CONVERSION_FAILURE,
    TGC_COLLECTION_MISSING,
    RPC_CONTAINER_UNAVAILABLE,
    RPC_CONVERSION_FAILURE,
    RPC_COLLECTION_MISSING,
    RPC_TRIGGER_ERROR,
    TGC_DECODER_ERROR,
    TRIG_TRACKS_ERROR,
    MUON_ROADS_ERROR,
    MDT_CONVERSION_FAILURE,
    CSC_CONVERSION_FAILURE,
    MDT_DECODER_ERROR,
    MDT_CLUSTER_ERROR,
    STATION_FIT_ERROR,
    EST_SAGITTA_ERROR,
    EST_ALPHABETA_ERROR,
    EST_PT_ERROR
};

}


class MuonFeatureDetails;

class SequenceBase {

public:
    SequenceBase(void);
    virtual ~SequenceBase(void);

public:  
    virtual MUFAST::ErrorCode start(MuonFeatureDetails* det=0) = 0;

public:
    virtual std::string type(void) const = 0;
    virtual std::string name(void) const = 0;

};

#endif // PHYSICS_MU_SEQUENCEBASE_H
