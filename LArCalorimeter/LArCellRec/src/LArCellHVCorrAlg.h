/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef LARCELLREC_LARCELLHVCORRALG_H
#define LARCELLREC_LARCELLHVCORRALG_H


#include "CaloUtils/CaloCellCorrection.h"
#include "GaudiKernel/ToolHandle.h"
#include "LArElecCalib/ILArHVScaleCorr.h"
#include "LArElecCalib/ILArCellHVCorrTool.h"
#include "LArCabling/LArOnOffIdMapping.h"
#include "StoreGate/ReadCondHandleKey.h"  

class CaloCell;
class CaloCellContainer;


class LArCellHVCorrAlg : public CaloCellCorrection {

public:
  
  LArCellHVCorrAlg (const std::string& type, const std::string& name, 
		   const IInterface* parent);

  ~LArCellHVCorrAlg();
  virtual StatusCode initialize() override final;
  virtual StatusCode execute(CaloCellContainer* cellCollection, const EventContext& ctx) const override final;


  //Implements the CaloCellCorrection interface    
  virtual void MakeCorrection (CaloCell* /*theCell*/, const EventContext& /*ctx*/) const override final; 
  void MakeCorrection (CaloCell* theCell,
                               const EventContext& ctx, const ILArHVScaleCorr* ofl=nullptr, const LArOnOffIdMapping* cabling=nullptr) const;



 private: 

  SG::ReadCondHandleKey<ILArHVScaleCorr> m_offlineHVScaleCorrKey{this, "keyOfflineHVCorr", "LArHVScaleCorrRecomputed","Key for LArHVScaleCorr"};
  SG::ReadCondHandleKey<LArOnOffIdMapping> m_cablingKey{this,"CablingKey","LArOnOffIdMap","SG Key of LArOnOffIdMapping object"};

};

#endif     
