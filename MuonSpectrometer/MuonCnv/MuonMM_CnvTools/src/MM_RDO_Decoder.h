/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MUONBYTESTREAMMMRDODECODER_H
#define MUONBYTESTREAMMMRDODECODER_H

#include "AthenaBaseComps/AthAlgTool.h"

#include "MuonRDO/MM_RawData.h"
#include "MuonDigitContainer/MmDigit.h"
#include "MuonIdHelpers/MmIdHelper.h"

#include "MuonMM_CnvTools/IMM_RDO_Decoder.h"
#include "NSWCalibTools/INSWCalibTool.h"

#include <inttypes.h>
#include <vector>

class NswCalibDbTimeChargeData;

namespace Muon {
  // Decoder class for conversion from MM RDOs to MM digits
  // Adapted for MMs from Alexandre Laurier - June 2019
  
  class MM_RDO_Decoder: public AthAlgTool, virtual public IMM_RDO_Decoder {
    
  public:
    
    MM_RDO_Decoder( const std::string& type, const std::string& name,
        const IInterface* parent ) ;

    virtual StatusCode initialize();
    
    MmDigit * getDigit(const Muon::MM_RawData * data) const;
    
  private:
    
    const MmIdHelper* m_mmIdHelper{nullptr};
    ToolHandle<Muon::INSWCalibTool> m_calibTool{this,"CalibrationTool", ""};
    
  };
  
}

inline MmDigit * Muon::MM_RDO_Decoder::getDigit(const Muon::MM_RawData* data) const
{

  // set up pointer to conditions object
  const EventContext& ctx = Gaudi::Hive::currentContext();

  // unit conversion
  const Identifier& Id    = data->identify();
  int channel             = data->channel();
  int tdo                 = data->time();
  int pdo                 = data->charge();
  uint16_t relBcid        = data->relBcid();
  // MM_RawData has time and charge in counts, need physical units
  double charge{0}, time{0.};
  m_calibTool->tdoToTime  (ctx, data->timeAndChargeInCounts(), tdo, Id, time  , relBcid); 
  m_calibTool->pdoToCharge(ctx, data->timeAndChargeInCounts(), pdo, Id, charge         ); 
  // Fill vectors
  std::vector<float> Time;
  std::vector<int> pos;
  std::vector<float> Charge;
  std::vector<uint16_t> RelBcid;
  Time.push_back(time);
  pos.push_back(channel);
  Charge.push_back(charge);
  RelBcid.push_back(relBcid);  
  // needs to be used once time calibration is available. pscholer Sept 2021

  // MM_RawData is built using only the first 4 values. The others are now simply filled proper objects. 
  MmDigit* mmDigit = new MmDigit(Id,Time,pos,Charge,Time,pos,Charge,Time,pos,Charge,pos,pos);
  return mmDigit;
}

#endif
