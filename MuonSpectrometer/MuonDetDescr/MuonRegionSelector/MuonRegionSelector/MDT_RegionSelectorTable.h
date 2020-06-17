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

class RegSelSiLUT;
class MuonMDT_CablingSvc;
#include <iostream>
#include <string>




class MDT_RegionSelectorTable : public AthAlgTool, virtual public IRegionIDLUT_Creator {

public:

  MDT_RegionSelectorTable (const std::string&, 
			   const std::string&,
			   const IInterface*);
  virtual ~MDT_RegionSelectorTable();

  StatusCode initialize();
  StatusCode finalize();
  
  virtual RegSelSiLUT* getLUT();

private:
  
  StatusCode createTable();
  
  RegSelSiLUT*   m_regionLUT;
  ServiceHandle<MuonMDT_CablingSvc> m_mdtCabling;
};


inline std::ostream& operator<<( std::ostream& s, const MDT_RegionSelectorTable& /*r*/ ) { 
  return s;
}


#endif  // __MDT_REGIONSELECTORTABLE_H 










