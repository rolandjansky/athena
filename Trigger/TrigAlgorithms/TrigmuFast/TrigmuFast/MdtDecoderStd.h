// -*- C++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


#ifndef PHYSICS_MDTDECODERSTD_H
#define PHYSICS_MDTDECODERSTD_H

#include "TrigmuFast/ProcessBase.h"
#include "TrigmuFast/Datatypes.h"
#include "TrigmuFast/MuMessageInterface.h"

#include <string>

class MDTGeometry;
class MuRoads;
//class RoadRegions;
//class LayerDigits;

class MdtDecoderStd: public ProcessBase {

public:  
    MdtDecoderStd(MuMessageInterface& msg,
                  const RoadRegions& muon_regions,
                  const MuData& mu_data,
                  LayerDigits (*digits)[MAXLAYER],
		  LayerDigits (*overlap)[MAXLAYER],
                  const MDTGeometry* mdt_geometry,
		  const MuRoads* roads);
    virtual ~MdtDecoderStd(void);

public:
    virtual bool run(MuonFeatureDetails* det=0) = 0;
    virtual void printout(void) const;
    virtual unsigned short int number(void) const;

public:
    virtual std::string type(void) const;
    virtual std::string name(void) const = 0;

protected:
    uint32_t decoding_error(MUFAST::CsmErrorCode err, uint32_t hash,
                            unsigned short int mrod,
		   	    unsigned short int subsytem,
		            unsigned short int link,
			    unsigned short int tdc) const;

protected:
    MuMessageInterface* m_msg;
    const RoadRegions& m_regions;
    const MuData::CSMlist& m_csm_data;
    const MuData::CSMlist& m_ovl_data;
    LayerDigits (*m_digits)[MAXLAYER];
    LayerDigits (*m_overlap)[MAXLAYER];
    const MDTGeometry* m_mdtgeo;
    const MuRoads* m_roads;
    

    void get_address(const char name[4],int StationPhi,
                     int& station,int& special,int& type) const;
    
    uint32_t get_system_id (unsigned short int SubsystemId) const;

private:
    static const std::string s_type;

};

inline unsigned short int
MdtDecoderStd::number() const
{
    return 0x3;
}


inline std::string
MdtDecoderStd::type(void) const
{
    return s_type;
}


#endif // PHYSICS_MDTDECODERSTD_H
