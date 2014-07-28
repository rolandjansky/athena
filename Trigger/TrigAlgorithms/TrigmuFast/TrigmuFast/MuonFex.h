// -*- C++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


#ifndef PHYSICS_MU_MUONFEX_H
#define PHYSICS_MU_MUONFEX_H

#include "TrigmuFast/SequenceBase.h"
#include "TrigmuFast/Datatypes.h"
#include "TrigMuonEvent/MuonFeature.h"
#include <string>

class MuMessageInterface;
class MuServiceInterface;
class MuData;

namespace MuonFexConsts {
const int EPtStrategy_linear = 0;
const int EPtStrategy_pol2   = 1;
}

class MuonFex: public SequenceBase {

public:
  MuonFex(MuMessageInterface& msg,
          MuServiceInterface& svc,
	  bool useTiltedBField,
          const TriggerData& trig_data,
          const TrackFit& track_fit,
          LayerDigits (*digits)[MAXLAYER]); // output
    virtual ~MuonFex(void);

public:
    virtual MUFAST::ErrorCode start(MuonFeatureDetails* det=0);
    const SpointData* super_points(void) const { return m_super_points; }
    const MuFeature& feature(void) const;
    MuonFeature* muonFeatures(void) const;

    void setEPtStrategy(int);
    int  getEPtStrategy(void) const { return m_EPtStrategy; }

public:
    virtual std::string type(void) const;
    virtual std::string name(void) const;

protected:                      // services
    MuMessageInterface* m_msg;
    MuServiceInterface* m_svc;

protected:                      // input
    bool m_useTiltedBField;
    const TriggerData& m_trigger_data;
    const TrackFit& m_track_fit;
    LayerDigits (*m_digits)[MAXLAYER];

protected:                      // output
    MuFeature m_feature;

protected:
    SpointData m_super_points[MAXSTATION];

protected:


protected:
    static const std::string s_type;
    static const std::string s_name;

private:
    int m_EPtStrategy;
};


inline const MuFeature&
MuonFex::feature(void) const
{
    return m_feature;
}

inline std::string
MuonFex::type(void) const
{
    return s_type;
}

inline std::string
MuonFex::name(void) const
{
    return s_name;
}

/*
inline MuonFeature*
MuonFex::muonFeatures(void) const
{
    if(m_feature.ok) 
    {
        if(m_svc->rpcgeo()->isFull())	    
	{    
	    return new MuonFeature(m_feature.Saddress,
                                   m_feature.Pt,
                                   m_feature.Spoint[0][0],
                                   m_feature.EtaMap,
                                   m_feature.PhiMap,
                                   m_feature.dir_phi,
                                   m_feature.Spoint[0][1],
				   m_feature.Spoint[0][2]);
	} else  return new MuonFeature(m_feature.Saddress,
			               m_feature.Sagitta,
	    	                       m_feature.Spoint[0][0],
				       m_feature.EtaMap,
				       m_feature.PhiMap,
				       m_feature.dir_phi,
				       m_feature.Spoint[0][1],
				       m_feature.Spoint[0][2]);
    }	
    else return new MuonFeature();
}
*/

#endif // PHYSICS_MU_MUONFEX_H

