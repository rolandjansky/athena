// -*- C++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


#ifndef PHYSICS_MU_MDTDECODERTB_H
#define PHYSICS_MU_MDTDECODERTB_H

#include "TrigmuFast/MdtDecoderStd.h"
#include <string>

class TrackFit;
class LayerDigits;
class DigitData;
//class DataTDC;

class MdtDecoderTB: public MdtDecoderStd {

public:
    MdtDecoderTB(MuMessageInterface& msg,
                  const RoadRegions& muon_regions,
                  const MuData& data,
                  LayerDigits (*digits)[MAXLAYER],
		  LayerDigits (*overlap)[MAXLAYER],
                  const MDTGeometry* geometry,
		  const MuRoads* roads,
		  unsigned int& Ttime);
    virtual ~MdtDecoderTB(void);

public:
    virtual bool run(MuonFeatureDetails* det=0);

public:
    virtual std::string name(void) const;

private:
    unsigned int& m_Ttime;

    float residual(float aw,float bw,float x,float y);

    DigitData* create_digit(unsigned int name,int StationEta,int StationPhi,
    int MultiLayer,int TubeLayer,int Tube,float R,float Z,float Phi,float InCo,
    float Res,unsigned short int time,unsigned short int adc) const;

private:
    static const std::string s_name;

};

inline std::string
MdtDecoderTB::name(void) const
{
    return s_name;
}

#endif // PHYSICS_MU_MDTDECODERTB_H
