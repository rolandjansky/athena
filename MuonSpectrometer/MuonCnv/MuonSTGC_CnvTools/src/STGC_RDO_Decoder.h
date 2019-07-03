/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MUONBYTESTREAMSTGCRDODECODER_H
#define MUONBYTESTREAMSTGCRDODECODER_H

#include "AthenaBaseComps/AthAlgTool.h"

#include "MuonRDO/STGC_RawData.h"
#include "MuonDigitContainer/sTgcDigit.h"
#include "MuonIdHelpers/sTgcIdHelper.h"
#include "StoreGate/ReadCondHandleKey.h"

#include "MuonSTGC_CnvTools/ISTGC_RDO_Decoder.h"

#include <inttypes.h>
#include <vector>


namespace Muon {
  // Decoder class for conversion from STGC RDOs to STGC digits
  // Stefano Rosati - CERN Jan 2004
  // Adapted for STGCs from Alexandre Laurier - June 2019
  
  class STGC_RDO_Decoder: public AthAlgTool, virtual public ISTGC_RDO_Decoder {
    
  public:
    
    STGC_RDO_Decoder( const std::string& type, const std::string& name,
        const IInterface* parent ) ;

    virtual StatusCode initialize();
    
    sTgcDigit * getDigit(const Muon::STGC_RawData * data) const;
    
  private:
    
    const sTgcIdHelper * m_stgcIdHelper;
    
  };
  
}

inline sTgcDigit * Muon::STGC_RDO_Decoder::getDigit(const Muon::STGC_RawData* data) const
{
  Identifier Id    = data->identify();
  float time       = data->time();
  uint16_t charge  = data->charge();
  uint16_t bcTag   = data->bcTag();
  bool isDead      = data->isDead();
  
  sTgcDigit* stgcDigit = new sTgcDigit(Id,bcTag,time,charge,isDead,true);
  
  return stgcDigit;
}

#endif
