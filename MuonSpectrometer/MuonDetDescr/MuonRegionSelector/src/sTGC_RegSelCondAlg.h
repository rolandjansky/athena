/** emacs: this is -*- c++ -*- **/
/**
 **   @file    sTGC_RegSelCondAlg.h        
 **                   
 **   @author  sutt
 **   @date    Tue  4 Feb 2020 15:25:00 CET
 **
 **   Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
 **/
 
#ifndef sTGC_RegSelCondAlg_h
#define sTGC_RegSelCondAlg_h


#include "MuonRegSelCondAlg.h"
#include "MuonReadoutGeometry/MuonDetectorManager.h"

/// NB: this sTGC class follows the naming convention from the 
///     sTGC detector code base hence the non-standard naming 
///     convention sTGC rather than STGC etc

class sTGC_RegSelCondAlg : public MuonRegSelCondAlg {

public:

  sTGC_RegSelCondAlg( const std::string& name, ISvcLocator* pSvcLocator );

  std::unique_ptr<RegSelSiLUT> createTable(  const EventContext& ctx, EventIDRange& id_range ) const override;

  virtual StatusCode initialize() override;

  SG::ReadCondHandleKey<MuonGM::MuonDetectorManager> m_DetectorManagerKey{this, "DetectorManagerKey", "MuonDetectorManager",
                                                                          "Key of input MuonDetectorManager condition data"};
};

#endif // sTGC_RegSelCondAlg_h
