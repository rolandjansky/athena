/** emacs: this is -*- c++ -*- **/
/**
 **   @file    SiRegSelCondAlg.h        
 **                   
 **   @author  sutt
 **   @date    Tue  4 Feb 2020 15:25:00 CET
 **
 **   Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
 **/
 
#ifndef SiRegSelCondAlg_h
#define SiRegSelCondAlg_h

#include "GaudiKernel/ISvcLocator.h"

#include "AthenaBaseComps/AthReentrantAlgorithm.h"
#include "AthenaBaseComps/AthAlgTool.h"
#include "SCT_Cabling/ISCT_CablingTool.h"

#include "GaudiKernel/ToolHandle.h"


#include "PixelConditionsData/PixelCablingCondData.h"
#include "SCT_Cabling/SCT_CablingData.h"
#include "StoreGate/ReadCondHandleKey.h"

#include "StoreGate/WriteCondHandleKey.h"

#include "IRegionSelector/RegSelLUTCondData.h"

#include <string>


/////////////////////////////////////////////////////////////////////////////

class SiRegSelCondAlg : public AthReentrantAlgorithm {

public:

  SiRegSelCondAlg( const std::string& name, ISvcLocator* pSvcLocator );

  virtual StatusCode  initialize() override;
  virtual StatusCode  execute (const EventContext& ctx) const override;

 private:

  std::string m_managerName;
  bool        m_printTable;

  /// Sadly the PIxel and SCT cabling are different classes so need both, 
  /// even if only one is to be used
 
  ToolHandle<ISCT_CablingTool>  m_sctCablingTool; // This class accesses SCT cabling during table creation

  SG::ReadCondHandleKey<SCT_CablingData> m_sctCablingKey
    {this, "SCT_CablingData", "SCT_CablingData", "SCT cabling key"};

  SG::ReadCondHandleKey<PixelCablingCondData> m_pixCablingKey
    {this, "PixelCablingCondData", "PixelCablingCondData", "Pixel cabling key"};



  /// Output conditions object
  SG::WriteCondHandleKey<RegSelLUTCondData> m_tableKey  
    { this, "RegSelLUT", "RegSelLUTCondData", "Region Selector lookup table" };

};

#endif // SiRegSelCondAlg_h
