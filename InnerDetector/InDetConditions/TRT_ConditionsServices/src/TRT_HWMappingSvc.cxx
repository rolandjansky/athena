/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/*---------------------------------------------------------
 * @file TRT_HWMappingSvc.cxx
 * @Service to provide offline -> hardware mapping
 * @author Denver Whittington <Denver.Whittington@cern.ch>
 *///------------------------------------------------------

// Header Includes
#include "GaudiKernel/IIncidentSvc.h"

#include "TRT_HWMappingSvc.h"
#include "TRT_ConditionsServices/ITRT_StrawNeighbourSvc.h"

#include "InDetCoolCoralClientUtils/TRT_COOLCORALClient.h"
#include "CoolApplication/Application.h"
#include "AthenaPoolUtilities/CondAttrListCollection.h"

#include "InDetIdentifier/TRT_ID.h"
#include "InDetReadoutGeometry/TRT_DetectorManager.h"

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
  m_Barrel_HV_CoolChanNames(0),
  m_EndcapA_HV_CoolChanNames(0),
  m_EndcapC_HV_CoolChanNames(0),
  m_Barrel_HV_CoolChanNums(0),
  m_EndcapA_HV_CoolChanNums(0),
  m_EndcapC_HV_CoolChanNums(0),
  m_TRT_ID_Helper(0),
  m_TRTStrawNeighbourSvc("TRT_StrawNeighbourSvc",name)
{
  // Get properties from job options
  declareProperty( "UseMapDb", m_UseMapDb = false );
  declareProperty( "MapDbConnectionString", m_MapDbConnectionString = "" );
  declareProperty( "Barrel_HV_COOLFolderName", m_Barrel_HV_COOLFolderName = "/TRT/DCS/HV/BARREL" );
  declareProperty( "EndcapA_HV_COOLFolderName", m_EndcapA_HV_COOLFolderName = "/TRT/DCS/HV/ENDCAPA" );
  declareProperty( "EndcapC_HV_COOLFolderName", m_EndcapC_HV_COOLFolderName = "/TRT/DCS/HV/ENDCAPC" );
  declareProperty( "TRTStrawNeighbourSvc", m_TRTStrawNeighbourSvc );
  declareProperty( "DumpMaps", m_DumpMaps = false );
  declareProperty( "BuildChanNumMaps", m_buildChanNumMaps = true );
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
  msg(MSG::INFO) << "TRT_HWMappingSvc::initialize." << endmsg;

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

  // Register a callback to load the HVLinePadMap on "BeginRun"
  IIncidentSvc* incSvc;
  sc = service( "IncidentSvc", incSvc );
  if ( sc.isFailure() ) {
    msg(MSG::ERROR) << "Couldn't get the IncidentSvc." << endmsg;
    return sc;
  }
  incSvc->addListener( this, std::string("BeginRun") );

  // Initialize HV-line/pad maps (don't forget to delete them at finalize!)
  int nBarrelPadsTotal = 32*(42+65+100);
  m_Barrel_HV_CoolChanNames = new std::vector<std::string>(nBarrelPadsTotal,"");
  m_Barrel_HV_CoolChanNums  = new std::vector<int>(nBarrelPadsTotal,-1);
  int nEndcapCellsTotal = 32*3*(6*4+8*2);
  m_EndcapA_HV_CoolChanNames = new std::vector<std::string>(nEndcapCellsTotal,"");
  m_EndcapA_HV_CoolChanNums  = new std::vector<int>(nEndcapCellsTotal,-1);
  m_EndcapC_HV_CoolChanNames = new std::vector<std::string>(nEndcapCellsTotal,"");
  m_EndcapC_HV_CoolChanNums  = new std::vector<int>(nEndcapCellsTotal,-1);

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

  if ( abs(barrel_ec) == 1 ) {
    // Barrel identifier
    int padNum = get_HV_BarrelPadNum( ident );
    int hashedPad = hashThisBarrelPad( phi_module, layer_or_wheel, padNum );
    if ( m_Barrel_HV_CoolChanNames ) {
      if ( hashedPad >= (int)m_Barrel_HV_CoolChanNames->size() || hashedPad<0) {
	msg(MSG::WARNING) << "channel request for invalid barrel HV pad." << endmsg;
	return "";
      } else chanName = m_Barrel_HV_CoolChanNames->at(hashedPad);
    }
  } else if ( barrel_ec == 2 ) {
    // EndcapA identifier
    int fourPlaneNum = get_HV_Endcap4PlaneNum( ident );
    int cellNum = get_HV_EndcapCellNum( ident );
    int hashedCell = hashThisEndcapCell( phi_module, layer_or_wheel, fourPlaneNum, cellNum );
    if ( m_EndcapA_HV_CoolChanNames ) {
      if ( hashedCell >= (int)m_EndcapA_HV_CoolChanNames->size() || hashedCell<0 ) {
	msg(MSG::WARNING) << "channel request for invalid endcap A HV pad." << endmsg;
	return "";
      } else chanName = m_EndcapA_HV_CoolChanNames->at(hashedCell);
    }
  } else if ( barrel_ec == -2 ) {
    // EndcapC identifier
    int fourPlaneNum = get_HV_Endcap4PlaneNum( ident );
    int cellNum = get_HV_EndcapCellNum( ident );
    int hashedCell = hashThisEndcapCell( phi_module, layer_or_wheel, fourPlaneNum, cellNum );
    if ( m_EndcapC_HV_CoolChanNames ) {
      if ( hashedCell >= (int)m_EndcapC_HV_CoolChanNames->size() || hashedCell<0) {
	msg(MSG::WARNING) << "channel request for invalid endcap C HV pad." << endmsg;
	return "";
      } else chanName = m_EndcapC_HV_CoolChanNames->at(hashedCell);
    }
  } else {
    msg(MSG::ERROR) << "Unknown Identifier (not barrel or endcap)!" << endmsg;
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

  if ( abs(barrel_ec) == 1 ) {
    // Barrel identifier
    int padNum = get_HV_BarrelPadNum( ident );
    int hashedPad = hashThisBarrelPad( phi_module, layer_or_wheel, padNum );
    if ( m_Barrel_HV_CoolChanNums ) {
      if ( hashedPad >= (int)m_Barrel_HV_CoolChanNums->size() || hashedPad<0 ) {
	msg(MSG::WARNING) << "channel number request for invalid barrel HV pad." << endmsg;
	return -1;
      } else chanNum = m_Barrel_HV_CoolChanNums->at(hashedPad);
    }
  } else if ( barrel_ec == 2 ) {
    // EndcapA identifier
    int fourPlaneNum = get_HV_Endcap4PlaneNum( ident );
    int cellNum = get_HV_EndcapCellNum( ident );
    int hashedCell = hashThisEndcapCell( phi_module, layer_or_wheel, fourPlaneNum, cellNum );
    if ( m_EndcapA_HV_CoolChanNums ) {
      if ( hashedCell >= (int)m_EndcapA_HV_CoolChanNums->size() || hashedCell<0) {
	msg(MSG::WARNING) << "channel number request for invalid endcap A HV cell." << endmsg;
	return -1;
      } else chanNum = m_EndcapA_HV_CoolChanNums->at(hashedCell);
    }
  } else if ( barrel_ec == -2 ) {
    // EndcapC identifier
    int fourPlaneNum = get_HV_Endcap4PlaneNum( ident );
    int cellNum = get_HV_EndcapCellNum( ident );
    int hashedCell = hashThisEndcapCell( phi_module, layer_or_wheel, fourPlaneNum, cellNum );
    if ( m_EndcapC_HV_CoolChanNums ) {
      if ( hashedCell >= (int)m_EndcapC_HV_CoolChanNums->size() || hashedCell<0) {
	msg(MSG::WARNING) << "channel number request for invalid endcap C HV cell." << endmsg;
	return -1;
      } else chanNum = m_EndcapC_HV_CoolChanNums->at(hashedCell);
    }
  } else {
    msg(MSG::ERROR) << "Unknown Identifier (not barrel or endcap)!" << endmsg;
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
/// Build HV-line/pad map for Barrel
/////
StatusCode TRT_HWMappingSvc::build_BarrelHVLinePadMap() {
  StatusCode sc(StatusCode::SUCCESS);

  if ( m_UseMapDb ) {

    // Get the HV-line/pad map from the mapping database

    TRT_COOLCORALClient* TRTcoralClient;
    TRTcoralClient = new TRT_COOLCORALClient( m_MapDbConnectionString );
    std::map<std::string,std::string> rawMap;
    TRTcoralClient->get_BarrelHVLinePadMap( rawMap );
    if ( rawMap.size() == 0 ) {
      msg(MSG::WARNING) << "Retrieved and empty Barrel HV-line/pad map from database." << endmsg;
      delete TRTcoralClient;
      return StatusCode::FAILURE;
    }

    // Iterate through the raw map to hash each pad number and enter its 
    std::map<std::string,std::string>::const_iterator rawMapItr;
    for ( rawMapItr = rawMap.begin(); rawMapItr != rawMap.end(); ++rawMapItr ) {
      std::string padName  = (*rawMapItr).first;
      std::string lineName = (*rawMapItr).second;

      // Parse the pad name string to get the hashed pad (index in map vector)
      int index1 = 0;
      int index2 = 0;
      index1 = padName.find("/Stack(",index1) + 7;
      index2 = padName.find(")",index1);
      std::string stackNumAsString( padName, index1, index2-index1 );
      index1 = padName.find("/Module_Type_",index1) + 13;
      std::string moduleNumAsString( padName, index1, 1 );
      index1 = padName.find("Pad",index1) + 3;
      index2 = padName.find("_",index1);
      std::string padNumAsString( padName, index1, index2-index1 );
      int stackNum = atoi(stackNumAsString.c_str()) - 1; // shift to range(0,31)
      int moduleNum = atoi(moduleNumAsString.c_str()) - 1; // shift to range(0,2)
      int padNum = atoi(padNumAsString.c_str()); // leave in range(1,n)
      int hashedPad = hashThisBarrelPad( stackNum, moduleNum, padNum );

      // Parse the string into COOL channel format
      int indexStart = lineName.find(":")+1;
      std::string chanName( lineName, indexStart, lineName.size()-indexStart );
      for ( int itr = 0; itr < (int)chanName.size(); ++itr ) {
	if ( chanName.compare( itr, 1, "/" ) == 0 ) {
	  chanName.replace( itr, 1, "_" );
	  if ( chanName.compare( itr+1, 1, "C" ) == 0 ) chanName.replace( itr+1, 1, "A" );
	  if ( chanName.compare( itr+1, 1, "D" ) == 0 ) chanName.replace( itr+1, 1, "B" );
	}
      }
      chanName.append( "_OutputVoltage" );

      // Add this channel into the map vector at the appropriate position
      // (hashed pad gives index in map vector)
      if ( hashedPad >= (int)m_Barrel_HV_CoolChanNames->size() || hashedPad<0) {
	msg(MSG::WARNING) << "channel request for invalid barrel HV pad." << endmsg;
      }else{
        m_Barrel_HV_CoolChanNames->at(hashedPad) = chanName;
      }
    }
    delete TRTcoralClient;

  } else {
    std::map< std::string, std::vector<int> > fuseBoxPadMapEven;
    std::map< std::string, std::vector<int> > fuseBoxPadMapOdd;
    std::vector<int> theVector;
    // Even stacks (in HW numbering): Fuseboxes A & B
    int pads_1A1[ 9] = { 5, 7,13,21,23,30,38,39,41};
    theVector = std::vector<int>( pads_1A1, pads_1A1+9 );
    fuseBoxPadMapEven.insert( make_pair("1_A1", theVector) );
    int pads_1A2[ 6] = { 6, 8,15,16,22,24};
    theVector = std::vector<int>( pads_1A2, pads_1A2+6 );
    fuseBoxPadMapEven.insert( make_pair("1_A2", theVector) );
    int pads_1A3[ 6] = {14,29,31,32,40,42};
    theVector = std::vector<int>( pads_1A3, pads_1A3+6 );
    fuseBoxPadMapEven.insert( make_pair("1_A3", theVector) );
    int pads_1B1[ 9] = { 2, 4,11,12,19,34,35,36,37};
    theVector = std::vector<int>( pads_1B1, pads_1B1+9 );
    fuseBoxPadMapEven.insert( make_pair("1_B1", theVector) );
    int pads_1B2[ 6] = {17,25,26,27,28,33};
    theVector = std::vector<int>( pads_1B2, pads_1B2+6 );
    fuseBoxPadMapEven.insert( make_pair("1_B2", theVector) );
    int pads_1B3[ 6] = { 1, 3, 9,10,18,20};
    theVector = std::vector<int>( pads_1B3, pads_1B3+6 );
    fuseBoxPadMapEven.insert( make_pair("1_B3", theVector) );
    int pads_2A1[12] = { 6, 8,10,15,18,20,49,50,51,61,63,65};
    theVector = std::vector<int>( pads_2A1, pads_2A1+12);
    fuseBoxPadMapEven.insert( make_pair("2_A1", theVector) );
    int pads_2A2[11] = { 7, 9,16,17,19,27,52,53,60,62,64};
    theVector = std::vector<int>( pads_2A2, pads_2A2+11);
    fuseBoxPadMapEven.insert( make_pair("2_A2", theVector) );
    int pads_2A3[10] = {26,28,29,30,37,38,39,40,41,42};
    theVector = std::vector<int>( pads_2A3, pads_2A3+10);
    fuseBoxPadMapEven.insert( make_pair("2_A3", theVector) );
    int pads_2B1[10] = { 2, 4,12,13,24,46,55,57,58,59};
    theVector = std::vector<int>( pads_2B1, pads_2B1+10);
    fuseBoxPadMapEven.insert( make_pair("2_B1", theVector) );
    int pads_2B2[12] = { 1, 3, 5,11,14,21,43,44,45,47,54,56};
    theVector = std::vector<int>( pads_2B2, pads_2B2+12);
    fuseBoxPadMapEven.insert( make_pair("2_B2", theVector) );
    int pads_2B3[10] = {22,23,25,31,32,33,34,35,36,48};
    theVector = std::vector<int>( pads_2B3, pads_2B3+10);
    fuseBoxPadMapEven.insert( make_pair("2_B3", theVector) );
    int pads_3A1[10] = { 9,10,20,23,24,32,33,34,46,54};
    theVector = std::vector<int>( pads_3A1, pads_3A1+10);
    fuseBoxPadMapEven.insert( make_pair("3_A1", theVector) );
    int pads_3A2[10] = {70,72,73,82,84,86,92,93,96,99};
    theVector = std::vector<int>( pads_3A2, pads_3A2+10);
    fuseBoxPadMapEven.insert( make_pair("3_A2", theVector) );
    int pads_3A3[10] = { 8,11,12,21,22,31,35,36,43,44};
    theVector = std::vector<int>( pads_3A3, pads_3A3+10);
    fuseBoxPadMapEven.insert( make_pair("3_A3", theVector) );
    int pads_3A4[10] = {59,71,81,83,85,94,95,97,98,100};
    theVector = std::vector<int>( pads_3A4, pads_3A4+10);
    fuseBoxPadMapEven.insert( make_pair("3_A4", theVector) );
    int pads_3A5[10] = {45,47,48,55,56,57,58,60,68,69};
    theVector = std::vector<int>( pads_3A5, pads_3A5+10);
    fuseBoxPadMapEven.insert( make_pair("3_A5", theVector) );
    int pads_3B1[10] = {51,63,64,66,74,76,78,80,89,90};
    theVector = std::vector<int>( pads_3B1, pads_3B1+10);
    fuseBoxPadMapEven.insert( make_pair("3_B1", theVector) );
    int pads_3B2[10] = { 1, 4, 5, 6,14,15,18,25,27,28};
    theVector = std::vector<int>( pads_3B2, pads_3B2+10);
    fuseBoxPadMapEven.insert( make_pair("3_B2", theVector) );
    int pads_3B3[10] = {61,62,65,67,75,77,79,87,88,91};
    theVector = std::vector<int>( pads_3B3, pads_3B3+10);
    fuseBoxPadMapEven.insert( make_pair("3_B3", theVector) );
    int pads_3B4[10] = { 2, 3, 7,13,16,17,19,26,29,30};
    theVector = std::vector<int>( pads_3B4, pads_3B4+10);
    fuseBoxPadMapEven.insert( make_pair("3_B4", theVector) );
    int pads_3B5[10] = {37,38,39,40,41,42,49,50,52,53};
    theVector = std::vector<int>( pads_3B5, pads_3B5+10);
    fuseBoxPadMapEven.insert( make_pair("3_B5", theVector) );
    // Odd stacks (in HW numbering): Fuseboxes C & D
    int* pads_1C1 = pads_1A1;
    theVector = std::vector<int>( pads_1C1, pads_1C1+9 );
    fuseBoxPadMapOdd.insert( make_pair("1_A1", theVector) );
    int* pads_1C2 = pads_1A3;
    theVector = std::vector<int>( pads_1C2, pads_1C2+6 );
    fuseBoxPadMapOdd.insert( make_pair("1_A2", theVector) );
    int* pads_1C3 = pads_1A2;
    theVector = std::vector<int>( pads_1C3, pads_1C3+6 );
    fuseBoxPadMapOdd.insert( make_pair("1_A3", theVector) );
    int* pads_1D1 = pads_1B1;
    theVector = std::vector<int>( pads_1D1, pads_1D1+ 9);
    fuseBoxPadMapOdd.insert( make_pair("1_B1", theVector) );
    int* pads_1D2 = pads_1B3;
    theVector = std::vector<int>( pads_1D2, pads_1D2+ 6);
    fuseBoxPadMapOdd.insert( make_pair("1_B2", theVector) );
    int* pads_1D3 = pads_1B2;
    theVector = std::vector<int>( pads_1D3, pads_1D3+ 6);
    fuseBoxPadMapOdd.insert( make_pair("1_B3", theVector) );
    int* pads_2C1 = pads_2A1;
    theVector = std::vector<int>( pads_2C1, pads_2C1+12);
    fuseBoxPadMapOdd.insert( make_pair("2_A1", theVector) );
    int* pads_2C2 = pads_2A2;
    theVector = std::vector<int>( pads_2C2, pads_2C2+11);
    fuseBoxPadMapOdd.insert( make_pair("2_A2", theVector) );
    int* pads_2C3 = pads_2A3;
    theVector = std::vector<int>( pads_2C3, pads_2C3+10);
    fuseBoxPadMapOdd.insert( make_pair("2_A3", theVector) );
    int* pads_2D1 = pads_2B1;
    theVector = std::vector<int>( pads_2D1, pads_2D1+10);
    fuseBoxPadMapOdd.insert( make_pair("2_B1", theVector) );
    int* pads_2D2 = pads_2B2;
    theVector = std::vector<int>( pads_2D2, pads_2D2+12);
    fuseBoxPadMapOdd.insert( make_pair("2_B2", theVector) );
    int* pads_2D3 = pads_2B3;
    theVector = std::vector<int>( pads_2D3, pads_2D3+10);
    fuseBoxPadMapOdd.insert( make_pair("2_B3", theVector) );
    int* pads_3C1 = pads_3A2;
    theVector = std::vector<int>( pads_3C1, pads_3C1+10);
    fuseBoxPadMapOdd.insert( make_pair("3_A1", theVector) );
    int* pads_3C2 = pads_3A1;
    theVector = std::vector<int>( pads_3C2, pads_3C2+10);
    fuseBoxPadMapOdd.insert( make_pair("3_A2", theVector) );
    int* pads_3C3 = pads_3A4;
    theVector = std::vector<int>( pads_3C3, pads_3C3+10);
    fuseBoxPadMapOdd.insert( make_pair("3_A3", theVector) );
    int* pads_3C4 = pads_3A3;
    theVector = std::vector<int>( pads_3C4, pads_3C4+10);
    fuseBoxPadMapOdd.insert( make_pair("3_A4", theVector) );
    int* pads_3C5 = pads_3A5;
    theVector = std::vector<int>( pads_3C5, pads_3C5+10);
    fuseBoxPadMapOdd.insert( make_pair("3_A5", theVector) );
    int* pads_3D1 = pads_3B2;
    theVector = std::vector<int>( pads_3D1, pads_3D1+10);
    fuseBoxPadMapOdd.insert( make_pair("3_B1", theVector) );
    int* pads_3D2 = pads_3B1;
    theVector = std::vector<int>( pads_3D2, pads_3D2+10);
    fuseBoxPadMapOdd.insert( make_pair("3_B2", theVector) );
    int* pads_3D3 = pads_3B4;
    theVector = std::vector<int>( pads_3D3, pads_3D3+10);
    fuseBoxPadMapOdd.insert( make_pair("3_B3", theVector) );
    int* pads_3D4 = pads_3B3;
    theVector = std::vector<int>( pads_3D4, pads_3D4+10);
    fuseBoxPadMapOdd.insert( make_pair("3_B4", theVector) );
    int* pads_3D5 = pads_3B5;
    theVector = std::vector<int>( pads_3D5, pads_3D5+10 );
    fuseBoxPadMapOdd.insert( make_pair("3_B5", theVector) );
    // Loop through sectors
    for ( int sector = 0; sector < 32; ++sector ) {
      // Odd Stacks = Side A (1) Fusebox C & D, Even = Side C (0) Fusebox A & B
      int side = (sector+1)%2;
      // Pick the appropriate Fusebox/Pad map
      std::map<std::string,std::vector<int> >* fuseBoxPadMap = 0;
      if ( side == 1 ) fuseBoxPadMap = &fuseBoxPadMapOdd;
      else fuseBoxPadMap = &fuseBoxPadMapEven;
      // Loop through all fusebox lines in this stack
      std::map<std::string,std::vector<int> >::const_iterator mapItr;
      for ( mapItr = fuseBoxPadMap->begin(); mapItr != fuseBoxPadMap->end(); ++mapItr ) {
	std::string fuseBoxName = (*mapItr).first;
	int module = atoi( &(fuseBoxName.at(0)) ) - 1;
	std::stringstream chanName;
	chanName << "HVB_S" << sector+1 << "_M" << fuseBoxName << "_OutputVoltage";
	// Loop through pads
	const std::vector<int>* padVec = &((*mapItr).second);
	std::vector<int>::const_iterator padItr;
	for ( padItr = padVec->begin(); padItr != padVec->end(); ++padItr ) {
	  int hashedPad = hashThisBarrelPad( sector, module, *padItr );
          if ( hashedPad >= (int)m_Barrel_HV_CoolChanNames->size() || hashedPad<0) {
	     msg(MSG::WARNING) << "channel request for invalid barrel HV pad." << endmsg;
          }else{
  	     m_Barrel_HV_CoolChanNames->at(hashedPad) = chanName.str();
          }
	}
      }  
    }
  }

  msg(MSG::INFO) << "TRT Barrel HV-line/pad map successfully built - "
       << m_Barrel_HV_CoolChanNames->size() << " channels." << endmsg;

  if ( m_buildChanNumMaps ) {
    // Get the CondAttrListCollection for the barrel
    const CondAttrListCollection* DCScondFolder = 0;
    sc = m_detStore->retrieve( DCScondFolder, m_Barrel_HV_COOLFolderName );
    if ( sc.isFailure() ) {
      msg(MSG::WARNING) << "Couldn't retrieve folder " << m_Barrel_HV_COOLFolderName
	   << " from DetectorStore.  Has it been loaded into IOVDbSvc?" << endmsg;
      return sc;
    }
    if ( DCScondFolder->name_size() == 0 ) {
      msg(MSG::WARNING) << "CondAttrListCollection for folder " << m_Barrel_HV_COOLFolderName
	   << " has no channel names." << endmsg;
      return StatusCode::FAILURE;
    }
    // Loop through the channel names.
    CondAttrListCollection::name_const_iterator nameItr;
    for ( nameItr = DCScondFolder->name_begin(); nameItr != DCScondFolder->name_end(); ++nameItr ) {
      int chanNum = (*nameItr).first;
      std::string chanName = (*nameItr).second;
      for ( int itr = 0; itr < (int)m_Barrel_HV_CoolChanNames->size(); ++itr ) {
	if ( m_Barrel_HV_CoolChanNames->at(itr) == chanName ) {
	  m_Barrel_HV_CoolChanNums->at(itr) = chanNum;
	}
      }
    }
  }

  return sc;
}

//////////
/// Build HV-line/pad maps for Endcaps
/////
StatusCode TRT_HWMappingSvc::build_EndcapHVLinePadMaps() {
  StatusCode sc(StatusCode::SUCCESS);

  // Loop through all possible pads

  // Loop through phi-sectors
  for ( int sector = 0; sector < 32; ++sector ) {
    // Loop through wheels (type A and B)
    for ( int wheel = 0; wheel < 14; ++wheel ) {
      // Loop through 4-plane layers in each wheel
      int nLayers = -1;
      if ( wheel >= 0 && wheel < 6  ) nLayers = 4;
      if ( wheel >= 6 && wheel < 14 ) nLayers = 2;
      for ( int layer = 0; layer < nLayers; ++layer ) {
	// Construct the HV-line logical name for this cell (COOL style)
	std::stringstream lineName;
	// SectorPair
	int sectorLeft = 0;
	int sectorRight = 0;
	if ( sector%2 == 0 ) {
	  sectorLeft  = sector+1;
	  sectorRight = sector+2;
	} else {
	  sectorLeft  = sector;
	  sectorRight = sector+1;
	}
	lineName << "S" << sectorLeft << "S" << sectorRight << "_";
	if ( sectorLeft%2 == 0 ) {
	  msg(MSG::WARNING) << "Mistake in sector pairing!!!" << endmsg;
	  break;
	}
	// Wheel
	if ( wheel >= 0 && wheel < 6 ) lineName << "WA" << (wheel+1) << "_";
	else lineName << "WB" << (wheel-5) << "_";
	// 4-plane layer
	if ( wheel >= 0 && wheel < 6 ) {
	  if ( layer == 0 ) lineName << "1B";
	  if ( layer == 1 ) lineName << "1T";
	  if ( layer == 2 ) lineName << "2B";
	  if ( layer == 3 ) lineName << "2T";
	} else {
	  if ( layer == 0 ) lineName << "B";
	  if ( layer == 1 ) lineName << "T";
	}
	lineName << "_OutputVoltage";
	// Final names for ECA and ECC
	std::stringstream lineNameA;
	std::stringstream lineNameC;
	lineNameA << "HVA_" << lineName.str();
	lineNameC << "HVC_" << lineName.str();
	// Add them to the maps
	for ( int cellNum = 0; cellNum < 3; ++cellNum ) {
	  int hashedCell = hashThisEndcapCell( sector, wheel, layer, cellNum );
          if ( hashedCell >= (int)m_EndcapA_HV_CoolChanNames->size() || hashedCell<0) {
	    msg(MSG::WARNING) << "channel request for invalid endcap HV pad." << endmsg;
          }else{
	    m_EndcapA_HV_CoolChanNames->at(hashedCell) = lineNameA.str();
	    m_EndcapC_HV_CoolChanNames->at(hashedCell) = lineNameC.str();
	  }
	}
      }
    }
  }

  // Apply corrections to the map
  int hashedCellToFix = hashThisEndcapCell( 5, 12, 0, 2 );
  if ( hashedCellToFix >= (int)m_EndcapA_HV_CoolChanNames->size() || hashedCellToFix<0) {
	    msg(MSG::WARNING) << "channel request for invalid endcap HV pad." << endmsg;
  }else{
  m_EndcapA_HV_CoolChanNames->at(hashedCellToFix) = "HVA_S7S8_WB7_B_OutputVoltage";
  }
  msg(MSG::INFO) << "Endcap HV-line/pad maps successfully built - ECA: "
       << m_EndcapA_HV_CoolChanNames->size() << " channels; ECC: "
       << m_EndcapC_HV_CoolChanNames->size() << " channels." << endmsg;

  if ( m_buildChanNumMaps ) {

    // EndcapA

    // Get the CondAttrListCollection for Endcap A
    const CondAttrListCollection* DCScondFolder = 0;
    sc = m_detStore->retrieve( DCScondFolder, m_EndcapA_HV_COOLFolderName );
    if ( sc.isFailure() ) {
      msg(MSG::WARNING) << "Couldn't retrieve folder " << m_EndcapA_HV_COOLFolderName
	   << " from DetectorStore.  Has it been loaded into IOVDbSvc?" << endmsg;
      return sc;
    }
    if ( DCScondFolder->name_size() == 0 ) {
      msg(MSG::WARNING) << "CondAttrListCollection for folder " << m_EndcapA_HV_COOLFolderName
	   << " has no channel names." << endmsg;
      return StatusCode::FAILURE;
    }
    // Loop through the channel names.
    CondAttrListCollection::name_const_iterator nameItr;
    for ( nameItr = DCScondFolder->name_begin(); nameItr != DCScondFolder->name_end(); ++nameItr ) {
      int chanNum = (*nameItr).first;
      std::string chanName = (*nameItr).second;
      for ( int itr = 0; itr < (int)m_EndcapA_HV_CoolChanNames->size(); ++itr ) {
	if ( m_EndcapA_HV_CoolChanNames->at(itr) == chanName ) {
	  m_EndcapA_HV_CoolChanNums->at(itr) = chanNum;
	}
      }
    }

    // EndcapC

    // Get the CondAttrListCollection for Endcap C
    DCScondFolder = 0;
    sc = m_detStore->retrieve( DCScondFolder, m_EndcapC_HV_COOLFolderName );
    if ( sc.isFailure() ) {
      msg(MSG::WARNING) << "Couldn't retrieve folder " << m_EndcapC_HV_COOLFolderName
	   << " from DetectorStore.  Has it been loaded into IOVDbSvc?" << endmsg;
      return sc;
    }
    if ( DCScondFolder->name_size() == 0 ) {
      msg(MSG::WARNING) << "CondAttrListCollection for folder " << m_EndcapC_HV_COOLFolderName
	   << " has no channel names." << endmsg;
      return StatusCode::FAILURE;
    }
    // Loop through the channel names.
    for ( nameItr = DCScondFolder->name_begin(); nameItr != DCScondFolder->name_end(); ++nameItr ) {
      int chanNum = (*nameItr).first;
      std::string chanName = (*nameItr).second;
      for ( int itr = 0; itr < (int)m_EndcapC_HV_CoolChanNames->size(); ++itr ) {
	if ( m_EndcapC_HV_CoolChanNames->at(itr) == chanName ) {
	  m_EndcapC_HV_CoolChanNums->at(itr) = chanNum;
	}
      }
    }
  }

  return sc;
}

//////////
/// Finalize
/////
StatusCode TRT_HWMappingSvc::finalize() {

  if ( m_Barrel_HV_CoolChanNames )  delete m_Barrel_HV_CoolChanNames;
  if ( m_EndcapA_HV_CoolChanNames ) delete m_EndcapA_HV_CoolChanNames;
  if ( m_EndcapC_HV_CoolChanNames ) delete m_EndcapC_HV_CoolChanNames;
  if ( m_Barrel_HV_CoolChanNums )   delete m_Barrel_HV_CoolChanNums;
  if ( m_EndcapA_HV_CoolChanNums )  delete m_EndcapA_HV_CoolChanNums;
  if ( m_EndcapC_HV_CoolChanNums )  delete m_EndcapC_HV_CoolChanNums;

  return StatusCode::SUCCESS;
}

//////////
/// IncidentSvc incident handler
/////
void TRT_HWMappingSvc::handle( const Incident& inc ) {
  StatusCode sc(StatusCode::SUCCESS);

  // BeginRun handler
  if ( inc.type() == "BeginRun" ) {

    sc = build_BarrelHVLinePadMap();
    if ( sc.isFailure() ) {
      msg(MSG::ERROR) << "Error in building Barrel HV-line/pad map." << endmsg;
    }

    sc = build_EndcapHVLinePadMaps();
    if ( sc.isFailure() ) {
      msg(MSG::ERROR) << "Error in building Endcap HV-line/pad maps." << endmsg;
    }

    if ( m_DumpMaps ) DumpMaps();

  }

  return;
}

//////////
/// Dump TRT Maps
/////
void TRT_HWMappingSvc::DumpMaps() {

  if ( m_Barrel_HV_CoolChanNames ) {
    msg(MSG::INFO) << "Dumping TRT Barrel HV-line/pad map..." << endmsg;
    for ( int mapItr = 0; mapItr < (int)m_Barrel_HV_CoolChanNames->size(); ++mapItr ) {
      msg(MSG::INFO) << mapItr << " " << m_Barrel_HV_CoolChanNames->at(mapItr) << endmsg;
    }
  }
  if ( m_EndcapA_HV_CoolChanNames ) {
    msg(MSG::INFO) << "Dumping TRT EndcapA HV-line/pad map..." << endmsg;
    for ( int mapItr = 0; mapItr < (int)m_EndcapA_HV_CoolChanNames->size(); ++mapItr ) {
      msg(MSG::INFO) << mapItr << " " << m_EndcapA_HV_CoolChanNames->at(mapItr) << endmsg;
    }
  }
  if ( m_EndcapC_HV_CoolChanNames ) {
    msg(MSG::INFO) << "Dumping TRT EndcapC HV-line/pad map..." << endmsg;
    for ( int mapItr = 0; mapItr < (int)m_EndcapC_HV_CoolChanNames->size(); ++mapItr ) {
      msg(MSG::INFO) << mapItr << " " << m_EndcapC_HV_CoolChanNames->at(mapItr) << endmsg;
    }
  }
  if ( m_Barrel_HV_CoolChanNums ) {
    msg(MSG::INFO) << "Dumping TRT Barrel HV-line/pad COOL channel numbers..." << endmsg;
    for ( int mapItr = 0; mapItr < (int)m_Barrel_HV_CoolChanNums->size(); ++mapItr ) {
      msg(MSG::INFO) << mapItr << " " << m_Barrel_HV_CoolChanNums->at(mapItr) << endmsg;
    }
  }
  if ( m_EndcapA_HV_CoolChanNums ) {
    msg(MSG::INFO) << "Dumping TRT EndcapA HV-line/pad COOL channel numbers..." << endmsg;
    for ( int mapItr = 0; mapItr < (int)m_EndcapA_HV_CoolChanNums->size(); ++mapItr ) {
      msg(MSG::INFO) << mapItr << " " << m_EndcapA_HV_CoolChanNums->at(mapItr) << endmsg;
    }
  }
  if ( m_EndcapC_HV_CoolChanNums ) {
    msg(MSG::INFO) << "Dumping TRT EndcapC HV-line/pad COOL channel numbers..." << endmsg;
    for ( int mapItr = 0; mapItr < (int)m_EndcapC_HV_CoolChanNums->size(); ++mapItr ) {
      msg(MSG::INFO) << mapItr << " " << m_EndcapC_HV_CoolChanNums->at(mapItr) << endmsg;
    }
  }

  // Create txt file of HV Line name and x,y / phi,z of straws on line
  const InDetDD::TRT_DetectorManager* detMan;
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
