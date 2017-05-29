// emacs: this is -*- c++ -*-
//
//   SiRegionSelectorTable.h        
//
//   Create the new RegSelSiLUT for the Pixel detector and SCT
// 
//   Copyright (C) 2007 M.Sutton (sutt@hep.ucl.ac.uk)    
//
//   $Id: SiRegionSelectorTable.h, v0.0   Mon Apr 14 17:00:14 BST 2008 sutt

 
#ifndef InDetRegionSelector_SiRegionSelectorTable_h
#define InDetRegionSelector_SiRegionSelectorTable_h

#include "RegSelLUT/IRegionIDLUT_Creator.h"

// #include "GaudiKernel/AlgTool.h"
#include "AthenaBaseComps/AthAlgTool.h"
#include "GaudiKernel/ToolHandle.h"
#include "GaudiKernel/ServiceHandle.h"
// #include "PixelCabling/PixelIdMapping.h"
#include "PixelCabling/IPixelCablingSvc.h"
#include "SCT_Cabling/ISCT_CablingSvc.h"

// class StoreGateSvc;
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
  
  //  StoreGateSvc*  m_detStore;
  RegSelSiLUT*   m_regionLUT;

  // Algorithm properties
  std::string m_managerName;
  double m_deltaZ;
  std::string m_roiFileName;
  bool m_printHashId;
  bool m_printTable;
  bool m_noDBM;

  // cablings
  //  ToolHandle<PixelIdMapping>     m_pixIdMapping;
  ServiceHandle<IPixelCablingSvc> m_pixIdMapping;
  ServiceHandle<ISCT_CablingSvc>  m_sctCablingSvc;

};

#endif // InDetRegionSelector_SiRegionSelectorTable_h
