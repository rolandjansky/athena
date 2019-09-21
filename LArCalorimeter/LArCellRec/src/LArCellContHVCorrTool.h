//Dear emacs, this is -*-c++-*-
/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef LARCELLREC_LARCELLCONTHVCORRTOOL_H
#define LARCELLREC_LARCELLCONTHVCORRTOOL_H


#include "CaloInterface/ICaloCellMakerTool.h"
#include "LArElecCalib/ILArHVScaleCorr.h"
#include "LArCabling/LArOnOffIdMapping.h"
#include "StoreGate/ReadCondHandleKey.h"
#include "AthenaBaseComps/AthAlgTool.h"

class CaloCellContainer;


class LArCellContHVCorrTool : public extends<AthAlgTool, ICaloCellMakerTool>  {
public:
  
  LArCellContHVCorrTool (const std::string& type, const std::string& name, 
		   const IInterface* parent);

  ~LArCellContHVCorrTool();
  virtual StatusCode initialize() override final;

  //Implements the ICaloCellMaker interface
  virtual StatusCode process(CaloCellContainer* cellCollection, const EventContext& ctx) const override final;

 private: 
  SG::ReadCondHandleKey<ILArHVScaleCorr> m_offlineHVScaleCorrKey{this, "keyOfflineHVCorr", "LArHVScaleCorrRecomputed","Key for LArHVScaleCorr"};
  SG::ReadCondHandleKey<LArOnOffIdMapping> m_cablingKey{this,"CablingKey","LArOnOffIdMap","SG Key of LArOnOffIdMapping object"};

};

#endif     
