/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

#include "TRTHWMapCondAlg.h"
#include "CoralBase/AttributeListSpecification.h"
#include <memory>

TRTHWMapCondAlg::TRTHWMapCondAlg(const std::string& name
				 , ISvcLocator* pSvcLocator )
  : ::AthAlgorithm(name,pSvcLocator)
{}
TRTHWMapCondAlg::~TRTHWMapCondAlg(){}

StatusCode TRTHWMapCondAlg::initialize()
{
  // Read key
  ATH_CHECK( m_BarrelReadKey.initialize() );
  ATH_CHECK( m_EndAReadKey.initialize() );
  ATH_CHECK( m_EndCReadKey.initialize() );

  // Register write handle
  ATH_CHECK( m_WriteKey.initialize() );

  return StatusCode::SUCCESS;
}

StatusCode TRTHWMapCondAlg::execute() 
{
  ATH_MSG_DEBUG("execute " << name());

  // ____________ Construct Write Cond Handle and check its validity ____________

  SG::WriteCondHandle<TRTCond::HWMap> writeHandle{m_WriteKey};

  // Do we have a valid Write Cond Handle for current time?
  if(writeHandle.isValid()) {
    ATH_MSG_DEBUG("CondHandle " << writeHandle.fullKey() << " is already valid."
                  << ". In theory this should not be called, but may happen"
                  << " if multiple concurrent events are being processed out of order.");

    return StatusCode::SUCCESS; 
  }



  //  // ____________ Construct new Write Cond Object  ____________

  std::unique_ptr<TRTCond::HWMap> writeCdo=std::make_unique<TRTCond::HWMap>();

  // ____________ Compute the Write Cond Object (HWMap)  ____________

  EventIDRange rangeBarrel, rangeEndcap;

  if(StatusCode::SUCCESS != build_BarrelHVLinePadMaps(rangeBarrel, writeCdo.get() ) ) {
     ATH_MSG_ERROR ("Problem filling barrel map.");
     return StatusCode::FAILURE;     
  }
  if(StatusCode::SUCCESS != build_EndcapHVLinePadMaps(rangeEndcap, writeCdo.get() ) ) {
     ATH_MSG_ERROR ("Problem filling endcap map.");
     return StatusCode::FAILURE;     
  }

  EventIDRange rangeW=EventIDRange::intersect(rangeBarrel,rangeEndcap);

  // Record  CDO
  if(writeHandle.record(rangeW,std::move(writeCdo)).isFailure()) {
    ATH_MSG_ERROR("Could not record " << writeHandle.key() 
		  << " with EventRange " << rangeW
		  << " into Conditions Store");
    return StatusCode::FAILURE;
  }

  return StatusCode::SUCCESS;
}
//////////
/// Hash an HV pad by sector/module/pad#
/////
int TRTHWMapCondAlg::hashThisBarrelPad( int sector, int module, int padNum ) const{

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
  const int nPadsPerSector = 42+65+100;

  int padOffset = -1;
  switch( module ) {
  case 0: padOffset = 0;  break;
  case 1: padOffset = 42;  break;
  case 2: padOffset = 42+65; break;
  default:
    ATH_MSG_ERROR("Couldn't hash this pad: "
		  << sector << "," << module << "," << padNum);
    return -1;
  }

  hashedPad = 0;
  hashedPad += sector*nPadsPerSector;
  hashedPad += padNum + padOffset;

  return hashedPad;
}

//////////
/// Hash an endcap HV cell by sector/wheel/layer/cell#
/////
int TRTHWMapCondAlg::hashThisEndcapCell( int sector, int wheel, int layer, int cellNum ) const{

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
    ATH_MSG_ERROR("Invalid wheel number.");
    return -1;
  }

  const int nCellsPerSector = (6*4+8*2)*3; // 6 A-wheels + 8 B-wheels
  int fourPlaneWheelNum;
  if ( wheelType == 0 ) {
    fourPlaneWheelNum = layer + 4*wheel; // 0-23
  } else {
    fourPlaneWheelNum = layer + 24 + 2*(wheel-6); // 24-39
  }

  hashedCell = cellNum + 3*fourPlaneWheelNum + sector*nCellsPerSector;

  return hashedCell;
}



StatusCode TRTHWMapCondAlg::finalize()
{
  ATH_MSG_DEBUG("finalize " << name());
  return StatusCode::SUCCESS;
}

//////////
/// Build HV-line/pad map for Barrel
/////
StatusCode TRTHWMapCondAlg::build_BarrelHVLinePadMaps(EventIDRange& range, TRTCond::HWMap* writeCdo) const{
  StatusCode sc(StatusCode::SUCCESS);

    std::map< std::string, std::vector<int> > fuseBoxPadMapEven;
    std::map< std::string, std::vector<int> > fuseBoxPadMapOdd;
    std::vector<int> theVector;
    // Even stacks (in HW numbering): Fuseboxes A & B
    const int pads_1A1[ 9] = { 5, 7,13,21,23,30,38,39,41};
    theVector = std::vector<int>( pads_1A1, pads_1A1+9 );
    fuseBoxPadMapEven.insert( make_pair("1_A1", theVector) );
    const int pads_1A2[ 6] = { 6, 8,15,16,22,24};
    theVector = std::vector<int>( pads_1A2, pads_1A2+6 );
    fuseBoxPadMapEven.insert( make_pair("1_A2", theVector) );
    const int pads_1A3[ 6] = {14,29,31,32,40,42};
    theVector = std::vector<int>( pads_1A3, pads_1A3+6 );
    fuseBoxPadMapEven.insert( make_pair("1_A3", theVector) );
    const int pads_1B1[ 9] = { 2, 4,11,12,19,34,35,36,37};
    theVector = std::vector<int>( pads_1B1, pads_1B1+9 );
    fuseBoxPadMapEven.insert( make_pair("1_B1", theVector) );
    const int pads_1B2[ 6] = {17,25,26,27,28,33};
    theVector = std::vector<int>( pads_1B2, pads_1B2+6 );
    fuseBoxPadMapEven.insert( make_pair("1_B2", theVector) );
    const int pads_1B3[ 6] = { 1, 3, 9,10,18,20};
    theVector = std::vector<int>( pads_1B3, pads_1B3+6 );
    fuseBoxPadMapEven.insert( make_pair("1_B3", theVector) );
    const int pads_2A1[12] = { 6, 8,10,15,18,20,49,50,51,61,63,65};
    theVector = std::vector<int>( pads_2A1, pads_2A1+12);
    fuseBoxPadMapEven.insert( make_pair("2_A1", theVector) );
    const int pads_2A2[11] = { 7, 9,16,17,19,27,52,53,60,62,64};
    theVector = std::vector<int>( pads_2A2, pads_2A2+11);
    fuseBoxPadMapEven.insert( make_pair("2_A2", theVector) );
    const int pads_2A3[10] = {26,28,29,30,37,38,39,40,41,42};
    theVector = std::vector<int>( pads_2A3, pads_2A3+10);
    fuseBoxPadMapEven.insert( make_pair("2_A3", theVector) );
    const int pads_2B1[10] = { 2, 4,12,13,24,46,55,57,58,59};
    theVector = std::vector<int>( pads_2B1, pads_2B1+10);
    fuseBoxPadMapEven.insert( make_pair("2_B1", theVector) );
    const int pads_2B2[12] = { 1, 3, 5,11,14,21,43,44,45,47,54,56};
    theVector = std::vector<int>( pads_2B2, pads_2B2+12);
    fuseBoxPadMapEven.insert( make_pair("2_B2", theVector) );
    const int pads_2B3[10] = {22,23,25,31,32,33,34,35,36,48};
    theVector = std::vector<int>( pads_2B3, pads_2B3+10);
    fuseBoxPadMapEven.insert( make_pair("2_B3", theVector) );
    const int pads_3A1[10] = { 9,10,20,23,24,32,33,34,46,54};
    theVector = std::vector<int>( pads_3A1, pads_3A1+10);
    fuseBoxPadMapEven.insert( make_pair("3_A1", theVector) );
    const int pads_3A2[10] = {70,72,73,82,84,86,92,93,96,99};
    theVector = std::vector<int>( pads_3A2, pads_3A2+10);
    fuseBoxPadMapEven.insert( make_pair("3_A2", theVector) );
    const int pads_3A3[10] = { 8,11,12,21,22,31,35,36,43,44};
    theVector = std::vector<int>( pads_3A3, pads_3A3+10);
    fuseBoxPadMapEven.insert( make_pair("3_A3", theVector) );
    const int pads_3A4[10] = {59,71,81,83,85,94,95,97,98,100};
    theVector = std::vector<int>( pads_3A4, pads_3A4+10);
    fuseBoxPadMapEven.insert( make_pair("3_A4", theVector) );
    const int pads_3A5[10] = {45,47,48,55,56,57,58,60,68,69};
    theVector = std::vector<int>( pads_3A5, pads_3A5+10);
    fuseBoxPadMapEven.insert( make_pair("3_A5", theVector) );
    const int pads_3B1[10] = {51,63,64,66,74,76,78,80,89,90};
    theVector = std::vector<int>( pads_3B1, pads_3B1+10);
    fuseBoxPadMapEven.insert( make_pair("3_B1", theVector) );
    const int pads_3B2[10] = { 1, 4, 5, 6,14,15,18,25,27,28};
    theVector = std::vector<int>( pads_3B2, pads_3B2+10);
    fuseBoxPadMapEven.insert( make_pair("3_B2", theVector) );
    const int pads_3B3[10] = {61,62,65,67,75,77,79,87,88,91};
    theVector = std::vector<int>( pads_3B3, pads_3B3+10);
    fuseBoxPadMapEven.insert( make_pair("3_B3", theVector) );
    const int pads_3B4[10] = { 2, 3, 7,13,16,17,19,26,29,30};
    theVector = std::vector<int>( pads_3B4, pads_3B4+10);
    fuseBoxPadMapEven.insert( make_pair("3_B4", theVector) );
    const int pads_3B5[10] = {37,38,39,40,41,42,49,50,52,53};
    theVector = std::vector<int>( pads_3B5, pads_3B5+10);
    fuseBoxPadMapEven.insert( make_pair("3_B5", theVector) );
    // Odd stacks (in HW numbering): Fuseboxes C & D
    const int* pads_1C1 = pads_1A1;
    theVector = std::vector<int>( pads_1C1, pads_1C1+9 );
    fuseBoxPadMapOdd.insert( make_pair("1_A1", theVector) );
    const int* pads_1C2 = pads_1A3;
    theVector = std::vector<int>( pads_1C2, pads_1C2+6 );
    fuseBoxPadMapOdd.insert( make_pair("1_A2", theVector) );
    const int* pads_1C3 = pads_1A2;
    theVector = std::vector<int>( pads_1C3, pads_1C3+6 );
    fuseBoxPadMapOdd.insert( make_pair("1_A3", theVector) );
    const int* pads_1D1 = pads_1B1;
    theVector = std::vector<int>( pads_1D1, pads_1D1+ 9);
    fuseBoxPadMapOdd.insert( make_pair("1_B1", theVector) );
    const int* pads_1D2 = pads_1B3;
    theVector = std::vector<int>( pads_1D2, pads_1D2+ 6);
    fuseBoxPadMapOdd.insert( make_pair("1_B2", theVector) );
    const int* pads_1D3 = pads_1B2;
    theVector = std::vector<int>( pads_1D3, pads_1D3+ 6);
    fuseBoxPadMapOdd.insert( make_pair("1_B3", theVector) );
    const int* pads_2C1 = pads_2A1;
    theVector = std::vector<int>( pads_2C1, pads_2C1+12);
    fuseBoxPadMapOdd.insert( make_pair("2_A1", theVector) );
    const int* pads_2C2 = pads_2A2;
    theVector = std::vector<int>( pads_2C2, pads_2C2+11);
    fuseBoxPadMapOdd.insert( make_pair("2_A2", theVector) );
    const int* pads_2C3 = pads_2A3;
    theVector = std::vector<int>( pads_2C3, pads_2C3+10);
    fuseBoxPadMapOdd.insert( make_pair("2_A3", theVector) );
    const int* pads_2D1 = pads_2B1;
    theVector = std::vector<int>( pads_2D1, pads_2D1+10);
    fuseBoxPadMapOdd.insert( make_pair("2_B1", theVector) );
    const int* pads_2D2 = pads_2B2;
    theVector = std::vector<int>( pads_2D2, pads_2D2+12);
    fuseBoxPadMapOdd.insert( make_pair("2_B2", theVector) );
    const int* pads_2D3 = pads_2B3;
    theVector = std::vector<int>( pads_2D3, pads_2D3+10);
    fuseBoxPadMapOdd.insert( make_pair("2_B3", theVector) );
    const int* pads_3C1 = pads_3A2;
    theVector = std::vector<int>( pads_3C1, pads_3C1+10);
    fuseBoxPadMapOdd.insert( make_pair("3_A1", theVector) );
    const int* pads_3C2 = pads_3A1;
    theVector = std::vector<int>( pads_3C2, pads_3C2+10);
    fuseBoxPadMapOdd.insert( make_pair("3_A2", theVector) );
    const int* pads_3C3 = pads_3A4;
    theVector = std::vector<int>( pads_3C3, pads_3C3+10);
    fuseBoxPadMapOdd.insert( make_pair("3_A3", theVector) );
    const int* pads_3C4 = pads_3A3;
    theVector = std::vector<int>( pads_3C4, pads_3C4+10);
    fuseBoxPadMapOdd.insert( make_pair("3_A4", theVector) );
    const int* pads_3C5 = pads_3A5;
    theVector = std::vector<int>( pads_3C5, pads_3C5+10);
    fuseBoxPadMapOdd.insert( make_pair("3_A5", theVector) );
    const int* pads_3D1 = pads_3B2;
    theVector = std::vector<int>( pads_3D1, pads_3D1+10);
    fuseBoxPadMapOdd.insert( make_pair("3_B1", theVector) );
    const int* pads_3D2 = pads_3B1;
    theVector = std::vector<int>( pads_3D2, pads_3D2+10);
    fuseBoxPadMapOdd.insert( make_pair("3_B2", theVector) );
    const int* pads_3D3 = pads_3B4;
    theVector = std::vector<int>( pads_3D3, pads_3D3+10);
    fuseBoxPadMapOdd.insert( make_pair("3_B3", theVector) );
    const int* pads_3D4 = pads_3B3;
    theVector = std::vector<int>( pads_3D4, pads_3D4+10);
    fuseBoxPadMapOdd.insert( make_pair("3_B4", theVector) );
    const int* pads_3D5 = pads_3B5;
    theVector = std::vector<int>( pads_3D5, pads_3D5+10 );
    fuseBoxPadMapOdd.insert( make_pair("3_B5", theVector) );
    std::string chanName;
    chanName.reserve(40);
    // Loop through sectors
    for ( int sector = 0; sector < 32; ++sector ) {
      // Odd Stacks = Side A (1) Fusebox C & D, Even = Side C (0) Fusebox A & B
      int side = (sector+1)%2;
      // Pick the appropriate Fusebox/Pad map
      const auto & fuseBoxPadMap = (side == 1)? fuseBoxPadMapOdd : fuseBoxPadMapEven;
      // Loop through all fusebox lines in this stack
      for (const auto& [fuseBoxName, padVec] : fuseBoxPadMap){
        int module = fuseBoxName.front() - '0' - 1;
        // Loop through pads
        for ( const auto & pad: padVec ) {
          int hashedPad = hashThisBarrelPad( sector, module, pad );
          if ( hashedPad >= (int)writeCdo->get_Barrel_HV_Names()->size() || hashedPad<0) {
            ATH_MSG_INFO("channel request for invalid barrel HV pad.");
          } else {
            chanName = "HVB_S" + std::to_string(sector+1) + "_M" + fuseBoxName + "_OutputVoltage";
            writeCdo->setBarrelName(hashedPad,chanName);
          }
        }
      }  
    }


    ATH_MSG_INFO("TRT Barrel HV-line/pad map successfully built - "
		 << writeCdo->get_Barrel_HV_Names()->size() << " channels.");

  SG::ReadCondHandle<CondAttrListCollection> readBarrelHandle{m_BarrelReadKey};
  if(!readBarrelHandle.range(range)) {
    ATH_MSG_ERROR("Failed to retrieve validity range for " << readBarrelHandle.key());
    return StatusCode::FAILURE;
  }

  const CondAttrListCollection* DCSCondFolder{*readBarrelHandle};
  if ( DCSCondFolder == nullptr ) {
    ATH_MSG_WARNING("Couldn't retrieve folder /TRT/DCS/HV/BARREL. Has it been loaded into IOVDbSvc?");
    return StatusCode::FAILURE;
  }
  if ( DCSCondFolder->name_size() == 0 ) {
    ATH_MSG_WARNING("CondAttrListCollection for folder /TRT/DCS/HV/BARREL is missing ");
    return StatusCode::FAILURE;
  }


  // Loop through the channel names.
  CondAttrListCollection::name_const_iterator nameItr;
  for ( nameItr = DCSCondFolder->name_begin(); nameItr != DCSCondFolder->name_end(); ++nameItr ) {
    int chanNum = (*nameItr).first;
    std::string chanName = (*nameItr).second;
    for ( int itr = 0; itr < (int)writeCdo->get_Barrel_HV_Names()->size(); ++itr ) {
      if ( writeCdo->get_Barrel_HV_Names()->at(itr) == chanName ) writeCdo->setBarrelNum(itr , chanNum );
    }
  }
  

  return sc;
}

//////////
/// Build HV-line/pad maps for Endcaps
/////
StatusCode TRTHWMapCondAlg::build_EndcapHVLinePadMaps(EventIDRange& range, TRTCond::HWMap* writeCdo) const{
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
	  ATH_MSG_WARNING("Mistake in sector pairing!!!");
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
          if ( hashedCell >= (int)writeCdo->get_EndcapA_HV_Names()->size() || hashedCell<0) {
	    ATH_MSG_WARNING("channel request for invalid endcap HV pad.");
          }else{
	    writeCdo->setEndcapAName(hashedCell,lineNameA.str());
	    writeCdo->setEndcapCName(hashedCell,lineNameC.str());
	  }
	}
      }
    }
  }

  // Apply corrections to the map
  int hashedCellToFix = hashThisEndcapCell( 5, 12, 0, 2 );
  if ( hashedCellToFix >= (int)writeCdo->get_EndcapA_HV_Names()->size() || hashedCellToFix<0) {
    ATH_MSG_WARNING("channel request for invalid endcap HV pad.");
  }else{
    writeCdo->setEndcapAName(hashedCellToFix, "HVA_S7S8_WB7_B_OutputVoltage");
  }
  ATH_MSG_INFO("Endcap HV-line/pad maps successfully built - ECA: "
       << writeCdo->get_EndcapA_HV_Names()->size() << " channels; ECC: "
	       << writeCdo->get_EndcapC_HV_Names()->size() << " channels.");
  

  // EndcapA

  // Get the CondAttrListCollection for Endcap A
  EventIDRange rangeA;
  SG::ReadCondHandle<CondAttrListCollection> readEndAHandle{m_EndAReadKey};
  if(!readEndAHandle.range(rangeA)) {
    ATH_MSG_ERROR("Failed to retrieve validity range for " << readEndAHandle.key());
    return StatusCode::FAILURE;
  }

  const CondAttrListCollection* DCSCondFolder{*readEndAHandle};
  if ( DCSCondFolder == nullptr ) {
    ATH_MSG_WARNING("Couldn't retrieve folder /TRT/DCS/HV/ENDCAPA. Has it been loaded into IOVDbSvc?");
    return StatusCode::FAILURE;
  }
  if ( DCSCondFolder->name_size() == 0 ) {
    ATH_MSG_WARNING("CondAttrListCollection for folder /TRT/DCS/HV/ENDCAPA is missing");
    return StatusCode::FAILURE;
  }

  // Loop through the channel names.
  CondAttrListCollection::name_const_iterator nameItr;
  for ( nameItr = DCSCondFolder->name_begin(); nameItr != DCSCondFolder->name_end(); ++nameItr ) {
    int chanNum = (*nameItr).first;
    std::string chanName = (*nameItr).second;
    for ( int itr = 0; itr < (int)writeCdo->get_EndcapA_HV_Names()->size(); ++itr ) {
      if ( writeCdo->get_EndcapA_HV_Names()->at(itr) == chanName ) writeCdo->setEndcapANum(itr , chanNum );
    }
  }

  // EndcapC

  // Get the CondAttrListCollection for Endcap C
  EventIDRange rangeC;
  SG::ReadCondHandle<CondAttrListCollection> readEndCHandle{m_EndCReadKey};
  const CondAttrListCollection* DCSCondFolder1{*readEndCHandle};
  if(!readEndCHandle.range(rangeC)) {
    ATH_MSG_ERROR("Failed to retrieve validity range for " << readEndCHandle.key());
    return StatusCode::FAILURE;
  }
  if ( DCSCondFolder1 == nullptr ) {
    ATH_MSG_WARNING("Couldn't retrieve folder /TRT/DCS/HV/ENDCAPC. Has it been loaded into IOVDbSvc?");
    return StatusCode::FAILURE;
  }
  if ( DCSCondFolder1->name_size() == 0 ) {
    ATH_MSG_WARNING("CondAttrListCollection for folder /TRT/DCS/HV/ENDCAPC is missing");
    return StatusCode::FAILURE;
  }


  // Loop through the channel names.
  for ( nameItr = DCSCondFolder1->name_begin(); nameItr != DCSCondFolder1->name_end(); ++nameItr ) {
    int chanNum = (*nameItr).first;
    std::string chanName = (*nameItr).second;
    for ( int itr = 0; itr < (int)writeCdo->get_EndcapC_HV_Names()->size(); ++itr ) {
      if ( writeCdo->get_EndcapC_HV_Names()->at(itr) == chanName ) writeCdo->setEndcapCNum(itr , chanNum );
    }
  }

  range=EventIDRange::intersect(rangeA,rangeC);
  return sc;
}


