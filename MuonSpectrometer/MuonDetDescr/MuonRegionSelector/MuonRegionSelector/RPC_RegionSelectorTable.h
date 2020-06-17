// emacs: this is -*- C++ -*-
/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/
//
//   @file    RPC_RegionSelectorTable.h        
//
//                   
// 
//
//   $Id: RPC_RegionSelectorTable.h, v0.0   Tue  4 Aug 2009 16:34:52 BST sutt $


#ifndef __RPC_REGIONSELECTORTABLE_H
#define __RPC_REGIONSELECTORTABLE_H


#include "RegSelLUT/IRegionIDLUT_Creator.h"

#include "AthenaBaseComps/AthAlgTool.h"

#include <string>


class RegSelSiLUT;
class IRPCcablingSvc;


class RPC_RegionSelectorTable : public AthAlgTool, virtual public IRegionIDLUT_Creator {

public:

  RPC_RegionSelectorTable (const std::string&, 
			   const std::string&,
			   const IInterface*);
  virtual ~RPC_RegionSelectorTable();

  StatusCode initialize();
  StatusCode finalize();
  
  virtual RegSelSiLUT* getLUT();

private:
  
  StatusCode createTable();
  
  RegSelSiLUT*   m_regionLUT;
  
  const IRPCcablingSvc* m_rpcCabling;

};


inline std::ostream& operator<<( std::ostream& s, const RPC_RegionSelectorTable& /*r*/ ) { 
  return s;
}


#endif  // __RPC_REGIONSELECTORTABLE_H 










