// emacs: this is -*- c++ -*-
/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/
//
//   @file    TGC_RegionSelectorTable.h        
//
//                   
// 
//
//   $Id: TGC_RegionSelectorTable.h, v0.0   Tue  4 Aug 2009 16:34:52 BST sutt $


#ifndef __TGC_REGIONSELECTORTABLE_H
#define __TGC_REGIONSELECTORTABLE_H



#include "RegSelLUT/IRegionIDLUT_Creator.h"

// #include "GaudiKernel/AlgTool.h"
#include "AthenaBaseComps/AthAlgTool.h"
#include "GaudiKernel/ToolHandle.h"
#include "GaudiKernel/ServiceHandle.h"

#include "GeoPrimitives/GeoPrimitives.h"

class RegSelSiLUT;
class MuonTGC_CablingSvc;

#include <iostream>
#include <string>




class TGC_RegionSelectorTable : public AthAlgTool, virtual public IRegionIDLUT_Creator {

public:

  TGC_RegionSelectorTable (const std::string&, 
			   const std::string&,
			   const IInterface*);
  virtual ~TGC_RegionSelectorTable();

  StatusCode initialize();
  StatusCode finalize();
  
  virtual RegSelSiLUT* getLUT();

private:
  
  StatusCode createTable();
  
  // Cabling (new)
  MuonTGC_CablingSvc* m_tgcCabling; 

  RegSelSiLUT*   m_regionLUT;
};


inline std::ostream& operator<<( std::ostream& s, const TGC_RegionSelectorTable& /*t*/ ) { 
  return s;
}


#endif  // __TGC_REGIONSELECTORTABLE_H 










