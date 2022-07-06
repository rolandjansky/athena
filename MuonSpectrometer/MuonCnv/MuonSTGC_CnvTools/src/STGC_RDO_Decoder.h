/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MUONBYTESTREAMSTGCRDODECODER_H
#define MUONBYTESTREAMSTGCRDODECODER_H

#include "AthenaBaseComps/AthAlgTool.h"

#include "MuonRDO/STGC_RawData.h"
#include "MuonDigitContainer/sTgcDigit.h"
#include "MuonIdHelpers/sTgcIdHelper.h"

#include "MuonSTGC_CnvTools/ISTGC_RDO_Decoder.h"
#include "NSWCalibTools/INSWCalibTool.h"

#include <inttypes.h>
#include <vector>

class NswCalibDbTimeChargeData;

namespace Muon {
  // Decoder class for conversion from STGC RDOs to STGC digits
  // Stefano Rosati - CERN Jan 2004
  // Adapted for STGCs from Alexandre Laurier - June 2019
  
  class STGC_RDO_Decoder: public AthAlgTool, virtual public ISTGC_RDO_Decoder {
    
  public:
    
    STGC_RDO_Decoder( const std::string& type, const std::string& name,
        const IInterface* parent ) ;

    virtual StatusCode initialize() override;
    
    sTgcDigit * getDigit(const Muon::STGC_RawData * data) const override;
    
  private:
    
    const sTgcIdHelper * m_stgcIdHelper;
    ToolHandle<Muon::INSWCalibTool> m_calibTool{this,"CalibrationTool", ""};
    
  };
  
}

inline sTgcDigit * Muon::STGC_RDO_Decoder::getDigit(const Muon::STGC_RawData* data) const
{

  const EventContext& ctx = Gaudi::Hive::currentContext();
  // unit conversion
  Identifier Id    = data->identify();
  int tdo          = data->tdo();
  int pdo          = data->charge();
  uint16_t bcTag   = data->bcTag();
  bool isDead      = data->isDead();
  // MM_RawData has time and charge in counts, need physical units
  double charge{0.},  time{0.};
  m_calibTool->tdoToTime  (ctx, data->timeAndChargeInCounts(), tdo, Id, time  , bcTag); 
  m_calibTool->pdoToCharge(ctx, data->timeAndChargeInCounts(), pdo, Id, charge       ); 

  sTgcDigit* stgcDigit = new sTgcDigit(Id,bcTag,time,charge,isDead,true);
  
  return stgcDigit;
}

#endif
