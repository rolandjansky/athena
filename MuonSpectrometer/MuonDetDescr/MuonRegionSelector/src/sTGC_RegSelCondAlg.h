/** emacs: this is -*- c++ -*- **/
/**
 **   @file    sTGC_RegSelCondAlg.h        
 **                   
 **   @author  sutt
 **   @date    Tue  4 Feb 2020 15:25:00 CET
 **
 **   Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
 **/
 
#ifndef sTGC_RegSelCondAlg_h
#define sTGC_RegSelCondAlg_h


#include "MuonRegSelCondAlg.h"


/// NB: this sTGC class follows the naming convention from the 
///     sTGC detector code base hence the non-standard naming 
///     convention sTGC rather than STGC etc

class sTGC_RegSelCondAlg : public MuonRegSelCondAlg {

public:

  sTGC_RegSelCondAlg( const std::string& name, ISvcLocator* pSvcLocator );

  std::unique_ptr<RegSelSiLUT> createTable(  const EventContext& ctx, EventIDRange& id_range ) const override;

  virtual StatusCode initialize() override;

  SG::ReadCondHandleKey<MuonMDT_CablingMap> m_mdtCablingKey
     { this, "Cabling", "MuonMDT_CablingMap", "Key of output MDT cabling map" };
  
};

#endif // sTGC_RegSelCondAlg_h
