/** emacs: this is -*- c++ -*- **/
/**
 **   @file    MDT_RegSelCondAlg.h        
 **                   
 **   @author  sutt
 **   @date    Tue  4 Feb 2020 15:25:00 CET
 **
 **   Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
 **/
 
#ifndef MDT_RegSelCondAlg_h
#define MDT_RegSelCondAlg_h

#include "MuonRegSelCondAlg.h"


class MDT_RegSelCondAlg : public MuonRegSelCondAlg { 

public:

  MDT_RegSelCondAlg( const std::string& name, ISvcLocator* pSvcLocator );

  std::unique_ptr<RegSelSiLUT> createTable( const MuonMDT_CablingMap* cabling ) const override;   

};

#endif // MDT_RegSelCondAlg_h
