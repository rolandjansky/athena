// emacs: this is -*- c++ -*-
/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/
//
//   @file    CSC_RegionSelectorTable.h        
//
//                   
// 
//
//   $Id: CSC_RegionSelectorTable.h, v0.0   Tue  4 Aug 2009 16:34:52 BST sutt $


#ifndef __CSC_REGIONSELECTORTABLE_H
#define __CSC_REGIONSELECTORTABLE_H



#include "RegSelLUT/IRegionIDLUT_Creator.h"

// #include "GaudiKernel/AlgTool.h"
#include "AthenaBaseComps/AthAlgTool.h"
#include "GaudiKernel/ToolHandle.h"
#include "GaudiKernel/ServiceHandle.h"

#include "GeoPrimitives/GeoPrimitives.h"

class RegSelSiLUT;

#include <iostream>
#include <string>




class CSC_RegionSelectorTable : public AthAlgTool, virtual public IRegionIDLUT_Creator {

public:

  CSC_RegionSelectorTable (const std::string&, 
			   const std::string&,
			   const IInterface*);
  virtual ~CSC_RegionSelectorTable();

  StatusCode initialize();
  StatusCode finalize();
  
  virtual RegSelSiLUT* getLUT();

private:
  
  StatusCode createTable();
  
  RegSelSiLUT*   m_regionLUT;

};


inline std::ostream& operator<<( std::ostream& s, const CSC_RegionSelectorTable& /*r*/ ) { 
  return s;
}


#endif  // __CSC_REGIONSELECTORTABLE_H 










