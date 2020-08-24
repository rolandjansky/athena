/*
Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/
// emacs: this is -*- c++ -*-
//
//   @file    TGC_RegionSelectorTable.h        
//
//                   
// 
//   $Id: MM_RegionSelectorTable.h, v0.0   Tue  4 Aug 2009 16:34:52 BST sutt $


#ifndef MM_REGIONSELECTORTABLE_H
#define MM_REGIONSELECTORTABLE_H



#include "RegSelLUT/IRegionIDLUT_Creator.h"

// #include "GaudiKernel/AlgTool.h"
#include "AthenaBaseComps/AthAlgTool.h"
#include "GaudiKernel/ToolHandle.h"
#include "GaudiKernel/ServiceHandle.h"

#include "GeoPrimitives/GeoPrimitives.h"

class RegSelSiLUT;

/// calbliong not implemented yet
/// class MuonMM_CablingSvc;

#include <iostream>
#include <string>




class MM_RegionSelectorTable : public AthAlgTool, virtual public IRegionIDLUT_Creator {

public:

  MM_RegionSelectorTable (const std::string&, 
			  const std::string&,
			  const IInterface*);

  virtual ~MM_RegionSelectorTable();

  StatusCode initialize();
  StatusCode finalize();
  
  virtual RegSelSiLUT* getLUT();

private:
  
  StatusCode createTable();
  
  // Cabling (new) - not implemented yet
  // MuonMM_CablingSvc* m_tgcCabling; 

  RegSelSiLUT*   m_regionLUT;
};


inline std::ostream& operator<<( std::ostream& s, const MM_RegionSelectorTable&  ) { 
  return s;
}


#endif  // MM_REGIONSELECTORTABLE_H 










