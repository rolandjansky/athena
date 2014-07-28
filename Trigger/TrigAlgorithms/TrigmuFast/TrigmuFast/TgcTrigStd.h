// -*- C++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


#ifndef PHYSICS_MU_TGCTRIGSTD_H
#define PHYSICS_MU_TGCTRIGSTD_H

#include "TrigmuFast/ProcessBase.h"
#include "TrigmuFast/MuMessageInterface.h"

#include "TrigT1Interfaces/RecMuonRoI.h"

#include "TrigmuFast/TgcFit.h"
#include "MuonCablingTools/OutputDef.h"


class TriggerData;
class TGCcablingSvc;
class TgcRdo;
class TGCgeometrySvc;
class MuServiceInterface;

class TgcTrigStd: public ProcessBase
{

public:
    TgcTrigStd(MuMessageInterface& msg,
               __osstream* dis,
               const TgcRdo* tgc_rdo,
               const LVL1::RecMuonRoI* roi,
               TriggerData& out,
               RawVec&     tgc_raw_data,
               LUTDigitVec& tgc_lut_digits,
               const TGCgeometrySvc* pTgcLUTSvc,
               MuServiceInterface* svc);
    virtual ~TgcTrigStd(void);

public:
    virtual bool run(MuonFeatureDetails* det=0) = 0;
    virtual void printout(void) const;
    virtual unsigned short int number(void) const;

public:
    virtual std::string type(void) const;
    virtual std::string name(void) const = 0;

protected:                       // service
    MuMessageInterface* m_msg;
    __osstream*  m_dis;
    const TGCgeometrySvc* m_pTgcLUTSvc;
    MuServiceInterface* m_svc;

protected:                       // input
    const LVL1::RecMuonRoI* m_pMuonRoI;
    const TgcRdo* m_tgc_rdo;

protected:                       // output
    TriggerData& m_tgc_data;


protected:
    RawVec&     m_tgc_raw_data;           // List of digits Raw data
    LUTDigitVec& m_tgc_lut_digits;
    TgcFit::PointArray m_tgcStripMidPoints;  // List of TGC strip middle station points.
    TgcFit::PointArray m_tgcWireMidPoints;   // List of TGC wire middle station points.
    TgcFit::PointArray m_tgcStripInnPoints;  // List of TGC strip inner station points.
    TgcFit::PointArray m_tgcWireInnPoints;   // List of TGC wire inner station points.

private:
    static const std::string s_type;

};

inline unsigned short int
TgcTrigStd::number() const
{
    return 0x8;
}

inline std::string
TgcTrigStd::type(void) const
{
    return s_type;
}

#endif // PHYSICS_MU_TGCTRIGSTD_H
