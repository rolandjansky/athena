// emacs: this is -*- c++ -*-
//
//   @file    FTK_RegionSelectorTable.h
//                    
//  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
 
#ifndef InDetRegionSelector_FTK_RegionSelectorTable_h
#define InDetRegionSelector_FTK_RegionSelectorTable_h

#include "RegSelLUT/IRegionIDLUT_Creator.h"

#include "AthenaBaseComps/AthAlgTool.h"
#include "GaudiKernel/ToolHandle.h"
#include "GaudiKernel/ServiceHandle.h"

// class StoreGateSvc;
class RegSelSiLUT;

#include <string>
using std::string;

/////////////////////////////////////////////////////////////////////////////

class FTK_RegionSelectorTable : public AthAlgTool, virtual public IRegionIDLUT_Creator
{

public:
  FTK_RegionSelectorTable (const std::string&, 
			 const std::string&,
			 const IInterface*);
  virtual ~FTK_RegionSelectorTable();
  StatusCode initialize();
  StatusCode finalize();
  
  virtual RegSelSiLUT* getLUT() const;

private:
  
  StatusCode createTable();
  
  RegSelSiLUT*   m_regionLUT;

  // Algorithm properties
  std::string m_managerName;

  std::string m_roiFileName;
  bool m_printHashId;
  bool m_printTable;

  // cablings

};

#endif // InDetRegionSelector_FTK_RegionSelectorTable_h
