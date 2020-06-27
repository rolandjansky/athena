/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef InDetRegionSelector_TRT_RegionSelectorTable_h
#define InDetRegionSelector_TRT_RegionSelectorTable_h

#include "RegSelLUT/IRegionIDLUT_Creator.h"



#include "AthenaBaseComps/AthAlgTool.h"

#include <string>

class RegSelSiLUT;
class TRT_IdentifierConversionTool;
class ITRT_CablingSvc;

/////////////////////////////////////////////////////////////////////////////

class TRT_RegionSelectorTable : public AthAlgTool, virtual public IRegionIDLUT_Creator
{
public:
  
  TRT_RegionSelectorTable (const std::string&, 
			   const std::string&,
			   const IInterface*);
  virtual ~TRT_RegionSelectorTable();
  StatusCode initialize();
  StatusCode execute();
  StatusCode finalize();
  virtual RegSelSiLUT* getLUT();

  
private:
 
  StatusCode createTable();

  ServiceHandle<ITRT_CablingSvc>    m_TRT_IdMapping;  // Identifier mapping Tool
  //TRT_IdentifierConversionTool* m_TRT_IdMappingOld;  // Identifier conversion Tool. 
                                                     // Here temporarily until ATLAS mapping 
                                                     // is put into InDetCabling

  RegSelSiLUT*  m_regionLUT;

  // Algorithm properties
  std::string m_managerName;
  std::string m_roiFileName;
  bool m_printHashId;
  bool m_printTable;
};

#endif // InDetRegionSelector_TRT_RegionSelectorTable_h
