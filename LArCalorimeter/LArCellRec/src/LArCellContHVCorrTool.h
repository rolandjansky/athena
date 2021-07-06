//Dear emacs, this is -*-c++-*-
/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef LARCELLREC_LARCELLCONTHVCORRTOOL_H
#define LARCELLREC_LARCELLCONTHVCORRTOOL_H


#include "CaloInterface/ICaloCellMakerTool.h"
#include "LArRecConditions/LArHVCorr.h"
#include "StoreGate/ReadCondHandleKey.h"
#include "AthenaBaseComps/AthAlgTool.h"

class CaloCellContainer;


class LArCellContHVCorrTool : public extends<AthAlgTool, ICaloCellMakerTool>  {
public:
  using base_class::base_class;
  //LArCellContHVCorrTool (const std::string& type, const std::string& name, 
  //		   const IInterface* parent);

  ~LArCellContHVCorrTool() = default;
  virtual StatusCode initialize() override final;

  //Implements the ICaloCellMaker interface
  virtual StatusCode process(CaloCellContainer* cellCollection, const EventContext& ctx) const override final;

 private: 
  SG::ReadCondHandleKey<LArHVCorr> m_offlineHVScaleCorrKey{this, "keyOfflineHVCorr", "LArHVScaleCorrRecomputed","Key for LArHVScaleCorr"};

};

#endif     
