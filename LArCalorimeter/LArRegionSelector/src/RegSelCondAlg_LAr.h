/** emacs: this is -*- c++ -*- **/
/**
 **   @file    RegSelCondAlg_LAr.h        
 **                   
 **   @author  sutt
 **   @date    Tue  4 Feb 2020 15:25:00 CET
 **
 **   Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
 **/
 
#ifndef RegSelCondAlg_LAr_h
#define RegSelCondAlg_LAr_h

#include "GaudiKernel/ISvcLocator.h"

#include "AthenaBaseComps/AthReentrantAlgorithm.h"
#include "AthenaBaseComps/AthAlgTool.h"

#include "GaudiKernel/ToolHandle.h"

#include "StoreGate/ReadCondHandleKey.h"
#include "StoreGate/WriteCondHandleKey.h"

#include "IRegionSelector/IRegSelLUTCondData.h"
#include "RegionSelector/RegSelectorHashMap.h"

#include <string>

#include "LArCabling/LArOnOffIdMapping.h"





class RegSelCondAlg_LAr : public AthReentrantAlgorithm {
  
public:
  
  RegSelCondAlg_LAr( const std::string& name, ISvcLocator* pSvcLocator );

  virtual StatusCode  initialize() override;
  virtual StatusCode  execute (const EventContext& ctx) const override;

  virtual std::unique_ptr<RegSelectorHashMap> createTable() const; 

private:

  std::string m_managerName;
  bool        m_printTable;

   
  /// some cabling cond data to act as a flag to schedule the algorithm
  /// sadly have to use the pixel cabling if the calorimeter doesn't have
  /// it's own conditions data version 
  SG::ReadCondHandleKey<LArOnOffIdMapping> m_cablingKey
    {this, "CablingKey", "LArOnOffIdMap", "LAr cabling key"};

  /// Output conditions object
  SG::WriteCondHandleKey<IRegSelLUTCondData> m_tableKey  
    { this, "RegSelLUT", "RegSelLUTCondData", "Region Selector lookup table" };

};

#endif // RegSelCondAlg_LAr_h
