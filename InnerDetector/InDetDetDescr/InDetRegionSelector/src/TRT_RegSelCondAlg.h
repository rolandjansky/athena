/** emacs: this is -*- c++ -*- **/
/**
 **   @file    TRT_RegSelCondAlg.h        
 **                   
 **   @author  sutt
 **   @date    Tue  4 Feb 2020 15:25:00 CET
 **
 **   Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
 **/
 
#ifndef TRT_RegSelCondAlg_h
#define TRT_RegSelCondAlg_h

#include "AthenaBaseComps/AthReentrantAlgorithm.h"
#include "AthenaBaseComps/AthAlgTool.h"

#include "GaudiKernel/ISvcLocator.h"

#include "GaudiKernel/EventIDRange.h"

#include "TRT_Cabling/ITRT_CablingSvc.h"

#include "PixelConditionsData/PixelCablingCondData.h"

#include "StoreGate/ReadCondHandleKey.h"
#include "StoreGate/WriteCondHandleKey.h"

#include "IRegionSelector/IRegSelLUTCondData.h"

#include <string>

#include "TRT_RegSelCondAlg.h"



/////////////////////////////////////////////////////////////////////////////

class TRT_RegSelCondAlg : public AthReentrantAlgorithm {

public:

  TRT_RegSelCondAlg( const std::string& name, ISvcLocator* pSvcLocator );

  virtual StatusCode  initialize() override;
  virtual StatusCode  execute (const EventContext& ctx) const override;

 private:

  std::string m_managerName;
  bool        m_printTable;

  /// Sadly still load the pixel cabling service to get the EventIDRange

  SG::ReadCondHandleKey<PixelCablingCondData> m_condCablingKey
    {this, "PixelCablingCondData", "PixelCablingCondData", "Pixel cabling key"};

  /// Output conditions object
  SG::WriteCondHandleKey<IRegSelLUTCondData> m_tableKey  
    { this, "RegSelLUT", "RegSelLUTCondData", "Region Selector lookup table" };

};

#endif // TRT_RegSelCondAlg_h
