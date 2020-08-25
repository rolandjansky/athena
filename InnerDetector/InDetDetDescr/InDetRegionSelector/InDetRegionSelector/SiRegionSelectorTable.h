/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/
 
#ifndef InDetRegionSelector_SiRegionSelectorTable_h
#define InDetRegionSelector_SiRegionSelectorTable_h

#include "RegSelLUT/IRegionIDLUT_Creator.h"

#include "AthenaBaseComps/AthAlgTool.h"
#include "SCT_Cabling/ISCT_CablingTool.h"

#include "GaudiKernel/ToolHandle.h"

#include "PixelConditionsData/PixelCablingCondData.h"
#include "StoreGate/ReadCondHandleKey.h"

#include <string>

class RegSelSiLUT;

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
  
  virtual RegSelSiLUT* getLUT();

private:
  
  StatusCode createTable (const EventContext& ctx);
  
  RegSelSiLUT*   m_regionLUT;

  // Algorithm properties
  std::string m_managerName;
  std::string m_roiFileName;
  bool m_printHashId;
  bool m_printTable;
  bool m_noDBM;

  // cablings
  PublicToolHandle<ISCT_CablingTool>  m_sctCablingToolInc // This class accesses SCT cabling during initialization.
    {this, "SCT_CablingTool", "SCT_CablingToolInc", "Tool to retrieve SCT Cabling"};

  SG::ReadCondHandleKey<PixelCablingCondData> m_condCablingKey
    {this, "PixelCablingCondData", "PixelCablingCondData", "Pixel cabling key"};

};

#endif // InDetRegionSelector_SiRegionSelectorTable_h
