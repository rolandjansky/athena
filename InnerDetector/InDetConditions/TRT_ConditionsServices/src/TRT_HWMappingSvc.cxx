/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

/*---------------------------------------------------------
 * @file TRT_HWMappingSvc.cxx
 * @Service to provide offline -> hardware mapping
 * @author Denver Whittington <Denver.Whittington@cern.ch>
 * updated March 2019 Peter Hansen <phansen@nbi.dk>
 *///------------------------------------------------------

// Header Includes
#include "TRT_HWMappingSvc.h"
#include "TRT_ConditionsServices/ITRT_StrawNeighbourSvc.h"

#include "InDetIdentifier/TRT_ID.h"
#include "TRT_ReadoutGeometry/TRT_DetectorManager.h"
#include "StoreGate/ReadCondHandle.h"

#include "TFile.h"
#include "TGraph.h"

#include <sstream>
#include <fstream>

//////////
/// Constructor
/////
TRT_HWMappingSvc::TRT_HWMappingSvc( const std::string& name,
				    ISvcLocator* pSvcLocator ) :
  AthService( name, pSvcLocator ),
  m_detStore("DetectorStore",name),
  m_TRT_ID_Helper(nullptr),
  m_TRTStrawNeighbourSvc("TRT_StrawNeighbourSvc",name)
{
  // Get properties from job options
  declareProperty( "Barrel_HV_COOLFolderName", m_Barrel_HV_COOLFolderName = "/TRT/DCS/HV/BARREL" );
  declareProperty( "EndcapA_HV_COOLFolderName", m_EndcapA_HV_COOLFolderName = "/TRT/DCS/HV/ENDCAPA" );
  declareProperty( "EndcapC_HV_COOLFolderName", m_EndcapC_HV_COOLFolderName = "/TRT/DCS/HV/ENDCAPC" );
  declareProperty( "TRTStrawNeighbourSvc", m_TRTStrawNeighbourSvc );
  declareProperty( "DetectorStore", m_detStore );
}

//////////
/// Destructor
/////
TRT_HWMappingSvc::~TRT_HWMappingSvc() {}

//////////
/// Initialize
/////
StatusCode TRT_HWMappingSvc::initialize() {
  ATH_MSG_INFO("TRT_HWMappingSvc::initialize.");

  StatusCode sc(StatusCode::SUCCESS);

  // Retrieve the DetectorStore
  sc = m_detStore.retrieve();
  if ( sc.isFailure() ) {
    msg(MSG::ERROR) << "Unable to retrieve " << m_detStore << endmsg;
    return sc;
  }

  // Get the TRT Identifier Helper.
  sc = m_detStore->retrieve( m_TRT_ID_Helper, "TRT_ID" );
  if ( sc.isFailure() ) {
    ATH_MSG_ERROR( "Unable to retrieve pointer to TRT ID Helper." );
    return sc;
  }

  // Get the TRTStrawNeighbourSvc
  sc = m_TRTStrawNeighbourSvc.retrieve();
  if ( sc.isFailure() ) {
    msg(MSG::ERROR) << "Couldn't get " << m_TRTStrawNeighbourSvc << endmsg;
    return sc;
  }

  ATH_CHECK( m_HWMapReadKey.initialize() );

  return sc;
}

//////////
/// Get HV-line Logical Name in COOL channel format
/////
std::string TRT_HWMappingSvc::get_HV_CoolChanName( const Identifier ident ) {

  std::string chanName = "";

  // Decode the identifier
  int barrel_ec      = m_TRT_ID_Helper->barrel_ec( ident );
  int phi_module     = m_TRT_ID_Helper->phi_module( ident );
  int layer_or_wheel = m_TRT_ID_Helper->layer_or_wheel( ident );
  //int straw_layer    = m_TRT_ID_Helper->straw_layer( ident );
  //int straw          = m_TRT_ID_Helper->straw( ident );

  // get the data from the Write condition object
  SG::ReadCondHandle<TRTCond::HWMap> readHandle(m_HWMapReadKey);
  const TRTCond::HWMap* theMap{*readHandle};
  if ( theMap == nullptr ) {
    ATH_MSG_WARNING("Couldn't retrieve DCS HV names. Is TRTHWMapCondAlg added to condSeq?");
    return chanName;
  }


  if ( abs(barrel_ec) == 1 ) {
    // Barrel identifier
    int padNum = get_HV_BarrelPadNum( ident );
    int hashedPad = hashThisBarrelPad( phi_module, layer_or_wheel, padNum );

    if ( hashedPad >= (int)theMap->get_Barrel_HV_Names()->size() || hashedPad<0) {
	ATH_MSG_WARNING("channel request for invalid barrel HV pad.");
	return "";
      } else chanName = theMap->get_Barrel_HV_Names()->at(hashedPad);

  } else if ( barrel_ec == 2 ) {
    // EndcapA identifier
    int fourPlaneNum = get_HV_Endcap4PlaneNum( ident );
    int cellNum = get_HV_EndcapCellNum( ident );
    int hashedCell = hashThisEndcapCell( phi_module, layer_or_wheel, fourPlaneNum, cellNum );

    if ( hashedCell >= (int)theMap->get_EndcapA_HV_Names()->size() || hashedCell<0 ) {
	ATH_MSG_WARNING("channel request for invalid endcap A HV pad.");
	return "";
      } else chanName = theMap->get_EndcapA_HV_Names()->at(hashedCell);

  } else if ( barrel_ec == -2 ) {
    // EndcapC identifier
    int fourPlaneNum = get_HV_Endcap4PlaneNum( ident );
    int cellNum = get_HV_EndcapCellNum( ident );
    int hashedCell = hashThisEndcapCell( phi_module, layer_or_wheel, fourPlaneNum, cellNum );

    if ( hashedCell >= (int)theMap->get_EndcapC_HV_Names()->size() || hashedCell<0) {
	ATH_MSG_WARNING("channel request for invalid endcap C HV pad.");
	return "";
    } else chanName = theMap->get_EndcapC_HV_Names()->at(hashedCell);

  } else {
    ATH_MSG_ERROR("Unknown Identifier (not barrel or endcap)!");
    return "";
  }

  return chanName;
}

//////////
/// Get COOL channel number for an identifier
/////
int TRT_HWMappingSvc::get_HV_CoolChanNum( const Identifier ident ) {

  int chanNum = -1;

  // Decode the identifier
  int barrel_ec      = m_TRT_ID_Helper->barrel_ec( ident );
  int phi_module     = m_TRT_ID_Helper->phi_module( ident );
  int layer_or_wheel = m_TRT_ID_Helper->layer_or_wheel( ident );
  //int straw_layer    = m_TRT_ID_Helper->straw_layer( ident );
  //int straw          = m_TRT_ID_Helper->straw( ident );

  // get the data from the Write condition object
  SG::ReadCondHandle<TRTCond::HWMap> readHandle(m_HWMapReadKey);
  const TRTCond::HWMap* theMap{*readHandle};
  if ( theMap == nullptr ) {
    ATH_MSG_WARNING("Couldn't retrieve DCS HV numbers. Is TRTHWMapCondAlg added to condSeq?");
    return chanNum;
  }


  if ( abs(barrel_ec) == 1 ) {
    // Barrel identifier
    int padNum = get_HV_BarrelPadNum( ident );
    int hashedPad = hashThisBarrelPad( phi_module, layer_or_wheel, padNum );


    if ( hashedPad >= (int)theMap->get_Barrel_HV_Nums()->size() || hashedPad<0) {
	ATH_MSG_WARNING("channel request for invalid barrel HV pad.");
	return -1;
    } else chanNum = theMap->get_Barrel_HV_Nums()->at(hashedPad);

  } else if ( barrel_ec == 2 ) {
    // EndcapA identifier
    int fourPlaneNum = get_HV_Endcap4PlaneNum( ident );
    int cellNum = get_HV_EndcapCellNum( ident );
    int hashedPad = hashThisEndcapCell( phi_module, layer_or_wheel, fourPlaneNum, cellNum );

    if ( hashedPad >= (int)theMap->get_EndcapA_HV_Nums()->size() || hashedPad<0) {
	ATH_MSG_WARNING("channel request for invalid EndcapA HV pad.");
	return -1;
    } else chanNum = theMap->get_EndcapA_HV_Nums()->at(hashedPad);


  } else if ( barrel_ec == -2 ) {
    // EndcapC identifier
    int fourPlaneNum = get_HV_Endcap4PlaneNum( ident );
    int cellNum = get_HV_EndcapCellNum( ident );
    int hashedPad = hashThisEndcapCell( phi_module, layer_or_wheel, fourPlaneNum, cellNum );

    if ( hashedPad >= (int)theMap->get_EndcapC_HV_Nums()->size() || hashedPad<0) {
	ATH_MSG_WARNING("channel request for invalid EndcapC HV pad.");
	return -1;
    } else chanNum = theMap->get_EndcapC_HV_Nums()->at(hashedPad);


  } else {
    ATH_MSG_ERROR("Unknown Identifier (not barrel or endcap)!");
    return -1;
  }

  return chanNum;
}

//////////
/// Get HV pad for a barrel identifier
/////
int TRT_HWMappingSvc::get_HV_BarrelPadNum( const Identifier ident ) {

  int padNum = -1;
  m_TRTStrawNeighbourSvc->getPad( ident, padNum );
  return padNum;
}

//////////
/// Hash an HV pad by sector/module/pad#
/////
int TRT_HWMappingSvc::hashThisBarrelPad( int sector, int module, int padNum ) {

  int hashedPad = -1;

  /*---------------------------------
   * Pad counting details:
   * 32 sectors ( expect sector in range(0,31) )
   * 3  modules ( expect module in range(0,2)  )
   * 42  pads in type1 module ( expect padNum in range(1,42)  )
   * 65  pads in type2 module ( expect padNum in range(1,65)  )
   * 100 pads in type3 module ( expect padNum in range(1,100) )
   */
  padNum -= 1; // shift range down to (0,n-1)
  int nPadsPerSector = 42+65+100;

  int padOffset = -1;
  switch( module ) {
  case 0: padOffset = 0;  break;
  case 1: padOffset = 42;  break;
  case 2: padOffset = 42+65; break;
  default:
    msg(MSG::ERROR) << "Couldn't hash this pad: "
	 << sector << "," << module << "," << padNum << endmsg;
    return -1;
  }

  hashedPad = 0;
  hashedPad += sector*nPadsPerSector;
  hashedPad += padNum + padOffset;

  return hashedPad;
}

//////////
/// Get HV cell for an endcap identifier
/////
int TRT_HWMappingSvc::get_HV_EndcapCellNum( const Identifier ident ) {

  int cellNum = -1;

  // Decode the identifier
  //int barrel_ec      = m_TRT_ID_Helper->barrel_ec( ident );
  //int phi_module     = m_TRT_ID_Helper->phi_module( ident );
  //int layer_or_wheel = m_TRT_ID_Helper->layer_or_wheel( ident );
  //int straw_layer    = m_TRT_ID_Helper->straw_layer( ident );
  int straw          = m_TRT_ID_Helper->straw( ident );

  /* Simple: Expect straw in range(0,23)
   * First  8 are Cell 0
   * Second 8 are Cell 1
   * Third  8 are Cell 2
   */

  if      ( straw >= 0 && straw < 8  ) cellNum = 0;
  else if ( straw >= 8 && straw < 16 ) cellNum = 1;
  else if ( straw >=16 && straw < 24 ) cellNum = 2;
  else {
    msg(MSG::WARNING) << "Straw number out of range for Endcap!" << endmsg;
    cellNum = -1;
  }

  return cellNum;
}

//////////
/// Get 4-plane wheel number in 16- or 8-plane wheel for an endcap identifier
/////
int TRT_HWMappingSvc::get_HV_Endcap4PlaneNum( const Identifier ident ) {

  int fourPlaneWheelNum = -1;

  // Decode the identifier
  //int barrel_ec      = m_TRT_ID_Helper->barrel_ec( ident );
  //int phi_module     = m_TRT_ID_Helper->phi_module( ident );
  //int layer_or_wheel = m_TRT_ID_Helper->layer_or_wheel( ident );
  int straw_layer    = m_TRT_ID_Helper->straw_layer( ident );
  //int straw          = m_TRT_ID_Helper->straw( ident );

  /* Simple: Expect straw_layer in range(0,15) or range(0,7)
   * First  4 are 4-plane wheel 0
   * Second 4 are 4-plane wheel 1
   * Third  4 are 4-plane wheel 2
   * Fourth 4 are 4-plane wheel 3
   */

  if      ( straw_layer >=  0 && straw_layer <  4 ) fourPlaneWheelNum = 0;
  else if ( straw_layer >=  4 && straw_layer <  8 ) fourPlaneWheelNum = 1;
  else if ( straw_layer >=  8 && straw_layer < 12 ) fourPlaneWheelNum = 2;
  else if ( straw_layer >= 12 && straw_layer < 16 ) fourPlaneWheelNum = 3;
  else {
    msg(MSG::WARNING) << "Straw layer number out of range for Endcap!" << endmsg;
    fourPlaneWheelNum = -1;
  }

  return fourPlaneWheelNum;
}

//////////
/// Get HV fuse for an endcap identifier
/////
int TRT_HWMappingSvc::get_HV_EndcapFuseNum( const Identifier ident ) {

  int fuseNum = -1;

  // Decode the identifier
  //int barrel_ec      = m_TRT_ID_Helper->barrel_ec( ident );
  //int phi_module     = m_TRT_ID_Helper->phi_module( ident );
  //int layer_or_wheel = m_TRT_ID_Helper->layer_or_wheel( ident );
  //int straw_layer    = m_TRT_ID_Helper->straw_layer( ident );
  int straw          = m_TRT_ID_Helper->straw( ident );

  /* Simple: Expect straw in range(0,23)
   *  straw# 0,1,  8, 9, 16,17 are Fuse 0
   *  straw# 2,3, 10,11, 18,19 are Fuse 1
   *  straw# 4,5, 12,13, 20,21 are Fuse 2
   *  straw# 6,7, 14,15, 22,23 are Fuse 3
   */

  straw = straw%8; // (0,8,16 -> 0)
  if ( straw == 0 or straw == 1 ) fuseNum = 0;
  if ( straw == 2 or straw == 3 ) fuseNum = 1;
  if ( straw == 4 or straw == 5 ) fuseNum = 2;
  if ( straw == 6 or straw == 7 ) fuseNum = 3;

  return fuseNum;
}

//////////
/// Hash an endcap HV cell by sector/wheel/layer/cell#
/////
int TRT_HWMappingSvc::hashThisEndcapCell( int sector, int wheel, int layer, int cellNum ) {

  int hashedCell = -1;

  /*---------------------------------
   * Cell counting details:
   * 32 sectors ( expect sector in range(0,31) )
   * 14 wheels ( expect wheel in range(0,13)  )
   *    typeA: (0,5)  wheelType = 0
   *    typeB: (6,13) wheelType = 1
   * 4 4-plane layers in typeA ( expect layer in range(0,3) )
   * 2 4-plane layers in typeB ( expect layer in range(0,1) )
   * 3 cells per sector in one 4-plane wheel ( expect cell in range(0,2) )
   */

  // Trick for counting
  int wheelType = -1;
  if ( wheel >= 0 && wheel < 6  ) wheelType = 0; // A wheel
  if ( wheel >= 6 && wheel < 14 ) wheelType = 1; // B wheel
  if ( wheelType == -1 ) {
    msg(MSG::ERROR) << "Invalid wheel number." << endmsg;
    return -1;
  }

  int nCellsPerSector = (6*4+8*2)*3; // 6 A-wheels + 8 B-wheels
  int fourPlaneWheelNum;
  if ( wheelType == 0 ) {
    fourPlaneWheelNum = layer + 4*wheel; // 0-23
  } else {
    fourPlaneWheelNum = layer + 24 + 2*(wheel-6); // 24-39
  }

  hashedCell = cellNum + 3*fourPlaneWheelNum + sector*nCellsPerSector;

  return hashedCell;
}


//////////
/// Finalize
/////
StatusCode TRT_HWMappingSvc::finalize() {

  return StatusCode::SUCCESS;
}

//////////
/// Dump TRT Maps
/////
void TRT_HWMappingSvc::DumpMaps() {

  // get the data from the Write condition object
  SG::ReadCondHandle<TRTCond::HWMap> readHandle(m_HWMapReadKey);
  const TRTCond::HWMap* theMap{*readHandle};
  if ( theMap == nullptr ) {
    ATH_MSG_WARNING("Couldn't retrieve DCS HV. Is TRTHWMapCondAlg added to condSeq?");
    return;
  }

  ATH_MSG_INFO("Dumping TRT Barrel HV-line/pad map...");
  for ( int mapItr = 0; mapItr < (int)theMap->get_Barrel_HV_Names()->size(); ++mapItr ) {
    ATH_MSG_INFO( mapItr << " " << theMap->get_Barrel_HV_Names()->at(mapItr));
  }

  ATH_MSG_INFO("Dumping TRT EndcapA HV-line/pad map...");
  for ( int mapItr = 0; mapItr < (int)theMap->get_EndcapA_HV_Names()->size(); ++mapItr ) {
    ATH_MSG_INFO( mapItr << " " << theMap->get_EndcapA_HV_Names()->at(mapItr));
  }

  ATH_MSG_INFO("Dumping TRT EndcapA HV-line/pad map...");
  for ( int mapItr = 0; mapItr < (int)theMap->get_EndcapC_HV_Names()->size(); ++mapItr ) {
    ATH_MSG_INFO( mapItr << " " << theMap->get_EndcapC_HV_Names()->at(mapItr));
  }

  ATH_MSG_INFO("Dumping TRT Barrel HV-line/pad channel values...");
  for ( int mapItr = 0; mapItr < (int)theMap->get_Barrel_HV_Nums()->size(); ++mapItr ) {
    ATH_MSG_INFO( mapItr << " " << theMap->get_Barrel_HV_Nums()->at(mapItr));
  }

  ATH_MSG_INFO("Dumping TRT EndcapA HV-line/pad channel values...");
  for ( int mapItr = 0; mapItr < (int)theMap->get_EndcapA_HV_Nums()->size(); ++mapItr ) {
    ATH_MSG_INFO( mapItr << " " << theMap->get_EndcapA_HV_Nums()->at(mapItr));
  }

  ATH_MSG_INFO("Dumping TRT EndcapA HV-line/pad channel values...");
  for ( int mapItr = 0; mapItr < (int)theMap->get_EndcapC_HV_Nums()->size(); ++mapItr ) {
    ATH_MSG_INFO( mapItr << " " << theMap->get_EndcapC_HV_Nums()->at(mapItr));
  }



  // Create txt file of HV Line name and x,y / phi,z of straws on line
  const InDetDD::TRT_DetectorManager* detMan = nullptr;
  StatusCode sc = m_detStore->retrieve(detMan);
  if ( sc.isFailure() ) {
    ATH_MSG_ERROR("Couldn't get TRT Detector Manager. Can't ouput text file for monitoring.");
    return;
  }
  std::map< std::string, std::vector<Identifier> > chanNameStrawMap;
  TRT_ID::const_expanded_id_iterator strawItr;
  int nStraws = 0;
  for ( strawItr = m_TRT_ID_Helper->straw_begin();
	strawItr != m_TRT_ID_Helper->straw_end(); ++strawItr ) {
    nStraws++;
    Identifier strawID = m_TRT_ID_Helper->straw_id(*strawItr);
    std::string HVchanName = get_HV_CoolChanName(strawID);
    std::map< std::string, std::vector<Identifier> >::iterator mapItr;
    mapItr = chanNameStrawMap.find(HVchanName);
    if ( mapItr == chanNameStrawMap.end() ) {
      // Channel name not yet recorded.  Insert it!
      std::vector<Identifier> vec;
      vec.push_back(strawID);
      chanNameStrawMap.insert( std::make_pair(HVchanName,vec) );
    } else {
      // Channel name is there.  Add identifier to its vector.
      (*mapItr).second.push_back(strawID);
    }
  }
  ATH_MSG_INFO( nStraws << " straws from TRT_ID." );
  std::fstream outFile( "TRT_HVmap.txt", std::ios::out );
  std::map< std::string, std::vector<Identifier> >::iterator mapItr;
  for ( mapItr = chanNameStrawMap.begin(); mapItr != chanNameStrawMap.end(); ++mapItr ) {
    std::vector<Identifier>::const_iterator idItr;
    ATH_MSG_INFO( "Channel " << (*mapItr).first << " - " );
    for ( idItr = (*mapItr).second.begin(); idItr != (*mapItr).second.end(); ++idItr ) {
      int det = m_TRT_ID_Helper->barrel_ec(*idItr);
      int phi = m_TRT_ID_Helper->phi_module(*idItr);
      int lay = m_TRT_ID_Helper->layer_or_wheel(*idItr);
      int sLay = m_TRT_ID_Helper->straw_layer(*idItr);
      int strawInLay = m_TRT_ID_Helper->straw(*idItr);
      ATH_MSG_INFO( det << " " << phi << " " << lay << " " << sLay << " " << strawInLay );
      // Get the detector element for this straw (layer)
      const InDetDD::TRT_BaseElement* detElem = detMan->getElement(*idItr);
      // 2D coordinates of straws on line
      if ( abs(det) == 1 ) outFile << det << " " << (*mapItr).first << " " << detElem->strawCenter(strawInLay)[0] << " " << detElem->strawCenter(strawInLay)[1] << "\n";
      if ( abs(det) == 2 ) outFile << det << " " << (*mapItr).first << " " << detElem->strawCenter(strawInLay).phi() << " " << detElem->strawCenter(strawInLay)[2] << "\n";
    }
  }

  return;
}
