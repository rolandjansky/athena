// emacs: this is -*- c++ -*-
/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/
//
//   @file    MDT_RegionSelectorTable.h        
//
//                   
// 
//
//   $Id: MDT_RegionSelectorTable.h, v0.0   Tue  4 Aug 2009 16:34:52 BST sutt $


#ifndef __MDT_REGIONSELECTORTABLE_H
#define __MDT_REGIONSELECTORTABLE_H



#include "RegSelLUT/IRegionIDLUT_Creator.h"

// #include "GaudiKernel/AlgTool.h"
#include "AthenaBaseComps/AthAlgTool.h"
#include "GaudiKernel/ToolHandle.h"
#include "GaudiKernel/ServiceHandle.h"

#include "GeoPrimitives/GeoPrimitives.h"
#include "StoreGate/ReadCondHandleKey.h"
#include "MuonCablingData/MuonMDT_CablingMap.h"
class RegSelSiLUT;
#include <iostream>
#include <string>

namespace MuonGM{
    class MuonDetectorManager;
}


class MDT_RegionSelectorTable : public AthAlgTool, virtual public IRegionIDLUT_Creator {

public:

  MDT_RegionSelectorTable (const std::string&, 
			   const std::string&,
			   const IInterface*);
  virtual ~MDT_RegionSelectorTable();

  StatusCode initialize() override; 
  StatusCode finalize() override;
  virtual RegSelSiLUT* getLUT() override;

private:
  
  void createTable();
  const MuonGM::MuonDetectorManager* m_detMgr{nullptr};
  std::unique_ptr<RegSelSiLUT>   m_regionLUT;
  SG::ReadCondHandleKey<MuonMDT_CablingMap> m_cablingKey{this, "MDTCablingKey", "MuonMDT_CablingMap", "Key of output MDT cabling map"};
};


inline std::ostream& operator<<( std::ostream& s, const MDT_RegionSelectorTable& /*r*/ ) { 
  return s;
}


#endif  // __MDT_REGIONSELECTORTABLE_H 










