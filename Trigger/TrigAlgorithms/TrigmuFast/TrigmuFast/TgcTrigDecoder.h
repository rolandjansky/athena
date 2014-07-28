// -*- C++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


#ifndef PHYSICS_MU_TGCTRIGDECODING_H
#define PHYSICS_MU_TGCTRIGDECODING_H

#include "TrigmuFast/TgcTrigStd.h"


class TriggerData;
class TgcLUT;

class TgcTrigDecoder: public TgcTrigStd
{

public:
    TgcTrigDecoder(MuMessageInterface& msg,
                   __osstream* display,
                   const TgcRdo* tgc_rdo,
                   const LVL1::RecMuonRoI* roi,
                   TriggerData& out,
                   RawVec& tgc_raw_data,
                   LUTDigitVec& tgc_lut_digits,
                   const TGCgeometrySvc* pTgcLUTSvc,
                   MuServiceInterface* svc);
    virtual ~TgcTrigDecoder(void);

public:
    virtual bool run(MuonFeatureDetails* det=0);
    virtual bool prepareTgcPoints(void);
    void postprocessTgcPoints();

public:
    virtual std::string name(void) const;


protected:
    static const std::string s_name;

    bool decodeTgcRDO(void);

private:
    TgcLUT* m_pLUT;
    
    void fillTgcPointsIntoESD(MuonFeatureDetails* det=0);
};

inline std::string
TgcTrigDecoder::name(void) const
{
    return s_name;
}


#endif // PHYSICS_MU_TGCTRIGDECODER_H
