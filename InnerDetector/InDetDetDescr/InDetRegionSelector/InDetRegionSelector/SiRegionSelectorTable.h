//   emacs: this is -*- c++ -*-
//
//   SiRegionSelectorTable.h        
//
//   Create the new RegSelSiLUT for the Pixel detector and SCT
// 
//   $Id: SiRegionSelectorTable.h, v0.0   Mon Apr 14 17:00:14 BST 2008 sutt (sutt@cern.ch) $
//
//   Copyright (C) 2002-2017 CERN being for the benefit of the ATLAS collaboration

 
#ifndef InDetRegionSelector_SiRegionSelectorTable_h
#define InDetRegionSelector_SiRegionSelectorTable_h

#include "RegSelLUT/IRegionIDLUT_Creator.h"

#include "AthenaBaseComps/AthAlgTool.h"
#include "GaudiKernel/ToolHandle.h"
#include "GaudiKernel/ServiceHandle.h"
#include "PixelCabling/IPixelCablingSvc.h"
#include "SCT_Cabling/ISCT_CablingSvc.h"

class RegSelSiLUT;

#include <string>
using std::string;

/////////////////////////////////////////////////////////////////////////////

class SiRegionSelectorTable : public AthAlgTool, virtual public IRegionIDLUT_Creator
{

public:
  SiRegionSelectorTable (const std::string&, 
			 const std::string&,
			 const IInterface*);
  virtual ~SiRegionSelectorTable();
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
  bool m_noDBM;

  // cablings
  ServiceHandle<IPixelCablingSvc> m_pixIdMapping;
  ServiceHandle<ISCT_CablingSvc>  m_sctCablingSvc;

};

#endif // InDetRegionSelector_SiRegionSelectorTable_h
