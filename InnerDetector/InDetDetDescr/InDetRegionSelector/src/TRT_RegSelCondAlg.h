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

#include "TRT_ReadoutGeometry/TRT_DetElementContainer.h"

#include "StoreGate/ReadCondHandleKey.h"
#include "StoreGate/WriteCondHandleKey.h"

#include "IRegionSelector/IRegSelLUTCondData.h"

#include <string>




/////////////////////////////////////////////////////////////////////////////

class TRT_RegSelCondAlg : public AthReentrantAlgorithm {

public:

  TRT_RegSelCondAlg( const std::string& name, ISvcLocator* pSvcLocator );

  virtual StatusCode  initialize() override;
  virtual StatusCode  execute (const EventContext& ctx) const override;

 private:

  bool        m_printTable;

  SG::ReadCondHandleKey<InDetDD::TRT_DetElementContainer> m_trtDetEleContKey
    {this, "TRTDetEleContKey", "TRT_DetElementContainer", "Key of TRT_DetElementContainer"};

  /// Output conditions object
  SG::WriteCondHandleKey<IRegSelLUTCondData> m_tableKey  
    { this, "RegSelLUT", "RegSelLUTCondData", "Region Selector lookup table" };

};

#endif // TRT_RegSelCondAlg_h
