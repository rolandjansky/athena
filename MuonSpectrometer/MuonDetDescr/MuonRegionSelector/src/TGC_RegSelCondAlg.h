/** emacs: this is -*- c++ -*- **/
/**
 **   @file    TGC_RegSelCondAlg.h        
 **                   
 **   @author  sutt
 **   @date    Tue  4 Feb 2020 15:25:00 CET
 **
 **   Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
 **/
 
#ifndef TGC_RegSelCondAlg_h
#define TGC_RegSelCondAlg_h

#include "MuonRegSelCondAlg.h"


class TGC_RegSelCondAlg : public MuonRegSelCondAlg {

public:

  TGC_RegSelCondAlg( const std::string& name, ISvcLocator* pSvcLocator );

  std::unique_ptr<RegSelSiLUT> createTable( const MuonMDT_CablingMap* mdtCabling ) const;

};

#endif // TGC_RegSelCondAlg_h
