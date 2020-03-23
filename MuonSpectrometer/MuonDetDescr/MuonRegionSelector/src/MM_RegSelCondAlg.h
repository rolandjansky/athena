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

  std::unique_ptr<RegSelSiLUT> createTable( const MuonMDT_CablingMap* mdtCabling ) const;

};

#endif // CSC_RegSelCondAlg_h
