/** emacs: this is -*- c++ -*- **/
/**
 **   @file    MM_RegSelCondAlg.h        
 **                   
 **   @author  sutt
 **   @date    Tue  4 Feb 2020 15:25:00 CET
 **
 **   Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
 **/
 
#ifndef MM_RegSelCondAlg_h
#define MM_RegSelCondAlg_h


#include "MuonRegSelCondAlg.h"


class MM_RegSelCondAlg : public MuonRegSelCondAlg {

public:

  MM_RegSelCondAlg( const std::string& name, ISvcLocator* pSvcLocator );

  std::unique_ptr<RegSelSiLUT> createTable( const EventContext& ctx, EventIDRange& id_range ) const override;

  virtual StatusCode initialize() override;

  SG::ReadCondHandleKey<MuonMDT_CablingMap> m_mdtCablingKey
    { this, "Cabling", "MuonMDT_CablingMap", "Key of output MDT cabling map" };
  
};

#endif // CSC_RegSelCondAlg_h
