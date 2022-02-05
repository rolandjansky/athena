/** emacs: this is -*- c++ -*- **/
/**
 **   @file    RegSelCondAlg_LAr.h        
 **                   
 **   @author  sutt
 **   @date    Tue  4 Feb 2020 15:25:00 CET
 **
 **   Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
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
#include "LArRecConditions/LArRoIMap.h"
#include "CaloDetDescr/CaloDetDescrManager.h"

#include <string>




class RegSelCondAlg_LAr : public AthReentrantAlgorithm {
  
public:
  
  RegSelCondAlg_LAr( const std::string& name, ISvcLocator* pSvcLocator );

  virtual StatusCode  initialize() override;
  virtual StatusCode  execute (const EventContext& ctx) const override;

private:
  std::unique_ptr<RegSelectorHashMap> createTable (const LArRoIMap& roiMap) const; 

  std::string m_managerName;
  bool        m_printTable;

   
  SG::ReadCondHandleKey<LArRoIMap> m_roiMapKey
    {this, "LArRoIMapKey", "LArRoIMap", "LAr RoI map key"};

  SG::ReadCondHandleKey<CaloDetDescrManager> m_caloMgrKey
    {this, "CaloDetDescrManager", "CaloDetDescrManager", "SG Key for CaloDetDescrManager in the Condition Store"};

  /// Output conditions object
  SG::WriteCondHandleKey<IRegSelLUTCondData> m_tableKey  
    { this, "RegSelLUT", "RegSelLUTCondData", "Region Selector lookup table" };
};

#endif // RegSelCondAlg_LAr_h
