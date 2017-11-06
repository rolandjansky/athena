/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TileRegionSelector_TileRegionSelectorTable_h
#define TileRegionSelector_TileRegionSelectorTable_h

#include "AthenaBaseComps/AthAlgTool.h"
#include <map> 
#include <vector> 
#include <string>

//#include "RegionSelector/IRegionLUT_Creator.h"
#include "IRegionLUT_Creator.h"
#include "TileByteStream/TileHid2RESrcID.h"
#include "TileIdentifier/TileHWID.h"
#include "TileCalibBlobObjs/TileCalibUtils.h"
#include "CaloIdentifier/TileID.h"
#include "TileDetDescr/TileDetDescrManager.h"
#include "TileConditions/TileCablingService.h"
#include "CaloDetDescr/CaloDetDescrElement.h"
#include "TileByteStream/TileROD_Decoder.h"

class RegionSelectorLUT;

/////////////////////////////////////////////////////////////////////////////

/**
 @class TileRegionSelectorTable
 @brief To create TileCal tables for Region selector
 @author Ambreesh Gupta


 This package contains code to generate the tables of element extents
 and ROBids used by the region selector. <br>
 
 Currently an Algorithm. To be changed to a Service at some point. <br>

 Authors: <br>
 Initial implementation (May 2004) by C. Bourdarios, 
 making use of code fragments from LArRawUtils (H. Ma) <br>
 Major update (Feb. 2006) by F. Ledroit <br>
 TileCal maintainer Ambreesh Gupta <br>
*/


class TileRegionSelectorTable : public AthAlgTool, virtual public IRegionLUT_Creator {
public:

  //TileRegionSelectorTable (const std::string& name, ISvcLocator* pSvcLocator);
  TileRegionSelectorTable (const std::string&,
			   const std::string&,
			   const IInterface*);
  
  StatusCode initialize();
  StatusCode execute();
  StatusCode finalize();
  
  virtual const RegionSelectorLUT * getLUT(std::string subSyst) const;

private:

  void fillMaps(); 

  StatusCode recordMap(RegionSelectorLUT* RSlut, std::string key); 

  StatusCode  testMaps(); 

  RegionSelectorLUT * m_tileLUT;

  // Algorithm properties
  bool m_printTable;
  bool m_testTable;
  bool m_2017RODs;

  // Names for ascii files
  std::string m_roiFileNameTile;
};

#endif // TileRegionSelector_TileRegionSelectorTable_h
