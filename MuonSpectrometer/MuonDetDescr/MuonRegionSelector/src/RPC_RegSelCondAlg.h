/** emacs: this is -*- c++ -*- **/
/**
 **   @file    RPC_RegSelCondAlg.h        
 **                   
 **   @author  sutt
 **   @date    Tue  4 Feb 2020 15:25:00 CET
 **
 **   Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
 **/
 
#ifndef RPC_RegSelCondAlg_h
#define RPC_RegSelCondAlg_h

#include "MuonRegSelCondAlg.h"


class RPC_RegSelCondAlg : public MuonRegSelCondAlg {

public:

  RPC_RegSelCondAlg( const std::string& name, ISvcLocator* pSvcLocator );

  std::unique_ptr<RegSelSiLUT> createTable( const MuonMDT_CablingMap* mdtCabling ) const override;

};

#endif // RPC_RegSelCondAlg_h
