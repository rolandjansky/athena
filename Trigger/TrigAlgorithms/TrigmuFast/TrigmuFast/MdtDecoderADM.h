// -*- C++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


#ifndef PHYSICS_MU_MDTDECODERADM_H
#define PHYSICS_MU_MDTDECODERADM_H

#include "TrigmuFast/MdtDecoderStd.h"
#include <string>

class TrackFit;
class LayerDigits;
class DigitData;
//class DataTDC;

class MdtDecoderADM: public MdtDecoderStd {

public:
    MdtDecoderADM(MuMessageInterface& msg,
                  const RoadRegions& muon_regions,
                  const MuData& data,
                  LayerDigits (*digits)[MAXLAYER],
		  LayerDigits (*overlap)[MAXLAYER],
                  const MDTGeometry* geometry,
		  const MuRoads* roads);
    virtual ~MdtDecoderADM(void);

public:
    virtual bool run(MuonFeatureDetails* det=0);

public:
    virtual std::string name(void) const;

private:
    float residual(float aw,float bw,float x,float y);

    bool decode_csm(const MdtCsm* csm, int tr, MuonFeatureDetails* det=0);
    DigitData* create_digit(unsigned int name,int StationEta,int StationPhi,
    int MultiLayer,int TubeLayer,int Tube,float R,float Z,float Phi,float InCo,
    float Res,unsigned short int time,uint32_t onlineId,
    uint16_t adc,uint16_t coarseTime,uint16_t fineTime) const;

private:
    static const std::string s_name;

};

inline std::string
MdtDecoderADM::name(void) const
{
    return s_name;
}

#endif // PHYSICS_MU_MDTDECODERADM_H
