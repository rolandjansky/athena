// emacs: this is -*- c++ -*-
//
//   @file    RPC_RegionSelectorTable.h        
//
//                   
// 
//   Copyright (C) 2007 M.Sutton (sutt@cern.ch)    
//
//   $Id: RPC_RegionSelectorTable.h, v0.0   Tue  4 Aug 2009 16:34:52 BST sutt $


#ifndef __RPC_REGIONSELECTORTABLE_H
#define __RPC_REGIONSELECTORTABLE_H



#include "RegSelLUT/IRegionIDLUT_Creator.h"

// #include "GaudiKernel/AlgTool.h"
#include "AthenaBaseComps/AthAlgTool.h"
#include "GaudiKernel/ToolHandle.h"
#include "GaudiKernel/ServiceHandle.h"

#include "GeoPrimitives/GeoPrimitives.h"

// class StoreGateSvc;
class RegSelSiLUT;

#include <iostream>
#include <string>




class RPC_RegionSelectorTable : public AthAlgTool, virtual public IRegionIDLUT_Creator {

public:

  RPC_RegionSelectorTable (const std::string&, 
			   const std::string&,
			   const IInterface*);
  virtual ~RPC_RegionSelectorTable();

  StatusCode initialize();
  StatusCode finalize();
  
  virtual RegSelSiLUT* getLUT() const;

private:
  
  StatusCode createTable();
  
  //  StoreGateSvc*  m_detStore;
  RegSelSiLUT*   m_regionLUT;

};


inline std::ostream& operator<<( std::ostream& s, const RPC_RegionSelectorTable& /*r*/ ) { 
  return s;
}


#endif  // __RPC_REGIONSELECTORTABLE_H 










