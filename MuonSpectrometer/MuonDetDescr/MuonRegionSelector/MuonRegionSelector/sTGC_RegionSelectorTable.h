/*
Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/
// emacs: this is -*- c++ -*-
//
//   @file    sTGC_RegionSelectorTable.h        
//
//                   
// 
//   $Id: sTGC_RegionSelectorTable.h, v0.0   Tue  4 Aug 2009 16:34:52 BST sutt $


#ifndef STGC_REGIONSELECTORTABLE_H
#define STGC_REGIONSELECTORTABLE_H



#include "RegSelLUT/IRegionIDLUT_Creator.h"

// #include "GaudiKernel/AlgTool.h"
#include "AthenaBaseComps/AthAlgTool.h"
#include "GaudiKernel/ToolHandle.h"
#include "GaudiKernel/ServiceHandle.h"

#include "GeoPrimitives/GeoPrimitives.h"

class RegSelSiLUT;
/// NSW cabling is not implemented yet - leave this in as a reminder
/// class MuonTGC_CablingSvc;

#include <iostream>
#include <string>




class sTGC_RegionSelectorTable : public AthAlgTool, virtual public IRegionIDLUT_Creator {

public:

  sTGC_RegionSelectorTable (const std::string&, 
			    const std::string&,
			    const IInterface*);
  virtual ~sTGC_RegionSelectorTable();

  StatusCode initialize();
  StatusCode finalize();
  
  virtual RegSelSiLUT* getLUT();

private:
  
  StatusCode createTable();
  
  // Cabling (new) - not implemented yet
  // MuonTGC_CablingSvc* m_tgcCabling; 

  RegSelSiLUT*   m_regionLUT;
};


inline std::ostream& operator<<( std::ostream& s, const sTGC_RegionSelectorTable&  ) { 
  return s;
}


#endif  // STGC_REGIONSELECTORTABLE_H 










