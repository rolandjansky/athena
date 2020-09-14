/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

//////////////////////////////////////////////////
///
/// TRT_COOLCORALClient.cpp
///
///////////////////////////////////////////////////

#include "InDetCoolCoralClientUtils/TRT_COOLCORALClient.h"

#include <stdlib.h>                     // for atoi, NULL
#include <sys/time.h>                   // for timeval, gettimeofday
#include <iostream>                     // for operator<<, endl, etc

// CORAL API
#include "CoralKernel/Context.h"
#include "CoralBase/Attribute.h"
#include "CoralBase/AttributeList.h"
#include "CoralBase/Date.h"

#include "RelationalAccess/IConnectionService.h"
#include "RelationalAccess/IConnectionServiceConfiguration.h"
#include "RelationalAccess/AccessMode.h"
#include "RelationalAccess/ISessionProxy.h"
#include "RelationalAccess/ISchema.h"
#include "RelationalAccess/ITransaction.h"

#include "RelationalAccess/ITable.h"
#include "RelationalAccess/ITableDescription.h"
#include "RelationalAccess/IColumn.h"
#include "RelationalAccess/ICursor.h"
#include "RelationalAccess/IQuery.h"
#include "RelationalAccess/TableDescription.h"
#include "RelationalAccess/ITableDataEditor.h"
#include "RelationalAccess/IBulkOperation.h"

#include "InDetCoolCoralClientUtils/CORAL-utils.h"

TRT_COOLCORALClient::TRT_COOLCORALClient(const std::string& id1 ) : COOLCORALClient( id1 ) {}
TRT_COOLCORALClient::~TRT_COOLCORALClient() {}

//------------------------------------------------------
/// Get list of disabled RODs from DAQ DB
void TRT_COOLCORALClient::get_EnabledRODs( int runNumber, std::vector<int>& RODIDvec ) {
  m_verbose = false;

  m_session->transaction().start(true);

  // Query the RUN_HISTORY table for the entries for this run number
  coral::IQuery* query0 = m_session->nominalSchema().newQuery();
  query0->addToTableList( "ONLTRT_RUNRODID" );
  query0->addToOutputList( "ONLTRT_RUNRODID.RODID" );
  std::string condStr0 = ( "ONLTRT_RUNRODID.RunNumber=:runNum" );
  coral::AttributeList condVals0 ATLAS_THREAD_SAFE; // not shared so ok
  condVals0.extend<int>( "runNum" );
  condVals0[0].data<int>() = runNumber; // BARREL HV PADS
  query0->setCondition( condStr0, condVals0 );
  int nRows0 = 0;
  coral::ICursor& cursor0 = query0->execute();
  while ( cursor0.next() ) {
    ++nRows0;
    const coral::AttributeList& row0 = cursor0.currentRow();
    int RODID = row0[0].data<int>();
    RODIDvec.push_back( RODID );
  }

}

//------------------------------------------------------
/// Get Run Tag and Start Timestamp from DAQ DB
void TRT_COOLCORALClient::get_RunTagAndTime( int runNumber, std::pair<std::string,int>& runPair  ) {
  m_verbose = false;

  std::string runTag = "";
  int runTime = 0;

  m_session->transaction().start(true);

  // Query the RUN_HISTORY table for the entries for this run number
  coral::IQuery* query0 = m_session->nominalSchema().newQuery();
  query0->addToTableList( "ONLTRT_RUN_HISTORY" );
  query0->addToOutputList( "ONLTRT_RUN_HISTORY.TAG" );
  query0->addToOutputList( "ONLTRT_RUN_HISTORY.Time" );
  std::string condStr0 = ( "ONLTRT_RUN_HISTORY.RUNNumber=:runNum" );
  coral::AttributeList condVals0 ATLAS_THREAD_SAFE; // not shared so ok
  condVals0.extend<int>( "runNum" );
  condVals0[0].data<int>() = runNumber; // BARREL HV PADS
  query0->setCondition( condStr0, condVals0 );
  int nRows0 = 0;
  coral::ICursor& cursor0 = query0->execute();
  while ( cursor0.next() ) {
    ++nRows0;
    const coral::AttributeList& row0 = cursor0.currentRow();
    runTag = row0[0].data<std::string>();
    runTime = (int)row0[1].data<long long>();
  }

  runPair.first  = runTag;
  runPair.second = runTime;
}

//------------------------------------------------------
/// Get COOL Channel Numbers for Barrel from COOL DCS tables
void TRT_COOLCORALClient::get_COOLChanMap( const std::string& folderName, std::map<int,std::string>& theMap ) {
  m_verbose = false;

  m_session->transaction().start(true);

  // Get the name of the folder with the channel names
  std::string channelTableName = "";
  coral::IQuery* query0 = m_session->nominalSchema().newQuery();
  query0->addToTableList( "COMP200_NODES" );
  query0->addToOutputList( "COMP200_NODES.FOLDER_CHANNELTABLENAME" );
  std::string condStr0 = "COMP200_NODES.NODE_FULLPATH='"+folderName+"'";
  coral::AttributeList empty ATLAS_THREAD_SAFE; // Not shared, so ok
  query0->setCondition( condStr0, empty );
  int nRows0 = 0;
  coral::ICursor& cursor0 = query0->execute();
  while( cursor0.next() ) {
    ++nRows0;
    const coral::AttributeList& row0 = cursor0.currentRow();
    channelTableName = row0[0].data<std::string>();
  }

  // Create the channelNumber/channelName map
  coral::IQuery* query1 = m_session->nominalSchema().newQuery();
  query1->addToTableList( channelTableName );
  query1->addToOutputList( channelTableName+".CHANNEL_ID" );
  query1->addToOutputList( channelTableName+".CHANNEL_NAME" );
  std::string condStr1 = ""; // all rows
  coral::AttributeList empty2 ATLAS_THREAD_SAFE; // Not shared, so ok
  query1->setCondition( condStr1, empty2 );
  int nRows1 = 0;
  coral::ICursor& cursor1 = query1->execute();
  while( cursor1.next() ) {
    ++nRows1;
    const coral::AttributeList& row1 = cursor1.currentRow();
    int chanID = row1[0].data<int>();
    std::string chanName = row1[1].data<std::string>();
    theMap.insert( make_pair( chanID, chanName ) );
  }

}

//------------------------------------------------------
/// Get HV-Line/Pad Map for Barrel (raw pad and line names direct from Mapping DB)
void TRT_COOLCORALClient::get_BarrelHVLinePadMap(std::map<std::string,std::string>& theMap) {
  m_verbose = false;

  int padLevelID = 86; // LEVEL_ID for Barrel in TREE_NODE table

  m_session->transaction().start(true);

  coral::IQuery* query0 = m_session->nominalSchema().newQuery();
  query0->addToOutputList( "Result01.padName" );
  query0->addToOutputList( "Result02.lineName" );

  coral::IQueryDefinition& subQuery01 = query0->defineSubQuery( "Result01" );
  subQuery01.addToOutputList( "CORR_MAP.node_id_st", "corrmap_nodeidst" );
  subQuery01.addToOutputList( "TREE_NODE.path_to", "padName" );
  subQuery01.addToTableList( "TREE_NODE" );
  subQuery01.addToTableList( "CORR_MAP" );
  std::string condStr01 = "TREE_NODE.level_id = :lvlid";
  condStr01 += " AND CORR_MAP.node_id_det = TREE_NODE.parent_id";
  coral::AttributeList condVals01 ATLAS_THREAD_SAFE; // Not shared, so ok.
  condVals01.extend<float>( "lvlid" );
  condVals01[0].data<float>() = float( padLevelID ); // BARREL HV PADS
  subQuery01.setCondition( condStr01, condVals01 );
  query0->addToTableList( "Result01" );

  coral::IQueryDefinition& subQuery02 = query0->defineSubQuery( "Result02" );
  subQuery02.addToOutputList( "TREE_NODE.path_to", "lineName" );
  subQuery02.addToOutputList( "TREE_NODE.node_id", "treenode_nodeid" );
  subQuery02.addToTableList( "TREE_NODE" );
  std::string condStr02 = "TREE_NODE.level_id = :lvlid2";
  coral::AttributeList condVals02 ATLAS_THREAD_SAFE; // Not shared, so ok.
  condVals02.extend<float>( "lvlid2" );
  condVals02[0].data<float>() = float( 64 ); // HVLINE LOGICAL NAMES
  subQuery02.setCondition( condStr02, condVals02 );
  query0->addToTableList( "Result02" );

  coral::AttributeList empty ATLAS_THREAD_SAFE; // Not shared, so ok.
  query0->setCondition( "Result01.corrmap_nodeidst = Result02.treenode_nodeid", empty );

  int nRows = 0;
  coral::ICursor& cursor0 = query0->execute();
  while( cursor0.next() ) {
    ++nRows;
    const coral::AttributeList& row0 = cursor0.currentRow();
    std::string padName  = row0[0].data<std::string>();
    std::string lineName = row0[1].data<std::string>();
    theMap.insert( make_pair( padName, lineName ) );
  }
}

//------------------------------------------------------
/// Create hashed integer from pad "identifier"
int TRT_COOLCORALClient::HashThisPad( const DetectorType& detector, int stack,
				      int module, int pad ) {

  // Please call with mapping-database numbering scheme for stack and module

  /**
   * Ranges:  ( uses TRT_ID helper counting scheme )
   *
   * Detector         Phi Slice              Layer
   *  BarrelA =  1     Barrel = { 0, 31 }     Barrel Module = { 0,  2 }
   *  BarrelC = -1     Endcap = { 0, 17 }     Endcap Wheel  = { 0, 31 }
   *  EndcapA =  2
   *  EndcapC = -2
   *
   * Pad
   *  Barrel = { 1, 100 } (depending on module type)
   *  Endcap = NOT YET IMPLEMENTED
   */

  int hashedInt = 0;
  int num_barrel_ec = 3;
  int num_phi_slice = 32;
  int num_module_or_wheel = 32;
  //int num_pad = 100;

  // Renumber for TRT_ID helper scheme
  int barrel_ec = 0;
  int phi_slice = stack - 1;
  int module_or_wheel = module - 1;
  switch (detector) {
  case Barrel:
    barrel_ec = 0; break;
  case EndcapA:
    barrel_ec = 1; break;
  case EndcapC:
    barrel_ec = 2; break;
  default:
    barrel_ec = 0; break;
  }

  // Create hashed integer
  hashedInt += barrel_ec;
  hashedInt += num_barrel_ec * phi_slice;
  hashedInt += num_barrel_ec * num_phi_slice * module_or_wheel;
  hashedInt += num_barrel_ec * num_phi_slice * num_module_or_wheel * (pad-1);

  return hashedInt;
}

//------------------------------------------------------
/// Create HV-Line/Pad Map
void TRT_COOLCORALClient::CreateHVLinePadMap( const DetectorType& detector, 
                                              std::map<int,std::string>& theMap) {
  m_verbose = false;


  int padLevelID = 0;
  switch( detector ) {
  case Barrel:
    padLevelID = 86;
    break;
  case EndcapA:
    std::cout << "I can't do Endcaps yet!" << std::endl;
    return;
    break;
  case EndcapC:
    std::cout << "I can't do Endcaps yet!" << std::endl;
    return;
    break;
  default:
    std::cout << "Invalid Detector specified." << std::endl;
    return;
    break;
  }

  m_session->transaction().start(true);

//   coral::IQuery* query0 = m_session->nominalSchema().newQuery();
//   query0->addToOutputList( "Result01.padName" );
//   query0->addToOutputList( "Result02.lineName" );

//   coral::IQueryDefinition& subQuery01 = query0->defineSubQuery( "Result01" );
//   subQuery01.addToOutputList( "CORR_MAP.node_id_st", "corrmap_nodeidst" );
//   subQuery01.addToOutputList( "TREE_NODE.path_to", "padName" );
//   subQuery01.addToTableList( "TREE_NODE" );
//   subQuery01.addToTableList( "CORR_MAP" );
//   std::string condStr01 = "TREE_NODE.level_id = :lvlid";
//   condStr01 += " AND CORR_MAP.node_id_det = TREE_NODE.parent_id";
//   coral::AttributeList condVals01;
//   condVals01.extend<float>( "lvlid" );
//   condVals01[0].data<float>() = float( padLevelID ); // BARREL HV PADS
//   subQuery01.setCondition( condStr01, condVals01 );
//   query0->addToTableList( "Result01" );

//   coral::IQueryDefinition& subQuery02 = query0->defineSubQuery( "Result02" );
//   subQuery02.addToOutputList( "TREE_NODE_MAP.node_id_out", "treenodemap_nodeidout" );
//   subQuery02.addToOutputList( "TREE_NODE.path_to", "lineName" );
//   subQuery02.addToTableList( "TREE_NODE" );
//   subQuery02.addToTableList( "TREE_NODE_MAP" );
//   std::string condStr02 = "TREE_NODE.level_id = :lvlid2";
//   condStr02 += " AND TREE_NODE_MAP.node_id_in = TREE_NODE.node_id";
//   coral::AttributeList condVals02;
//   condVals02.extend<float>( "lvlid2" );
//   condVals02[0].data<float>() = float( 64 ); // HVLINE LOGICAL NAMES
//   subQuery02.setCondition( condStr02, condVals02 );
//   query0->addToTableList( "Result02" );

//   query0->setCondition( "Result01.corrmap_nodeidst = Result02.treenodemap_nodeidout", coral::AttributeList() );

  coral::IQuery* query0 = m_session->nominalSchema().newQuery();
  query0->addToOutputList( "Result01.padName" );
  query0->addToOutputList( "Result02.lineName" );

  coral::IQueryDefinition& subQuery01 = query0->defineSubQuery( "Result01" );
  subQuery01.addToOutputList( "CORR_MAP.node_id_st", "corrmap_nodeidst" );
  subQuery01.addToOutputList( "TREE_NODE.path_to", "padName" );
  subQuery01.addToTableList( "TREE_NODE" );
  subQuery01.addToTableList( "CORR_MAP" );
  std::string condStr01 = "TREE_NODE.level_id = :lvlid";
  condStr01 += " AND CORR_MAP.node_id_det = TREE_NODE.parent_id";
  coral::AttributeList condVals01 ATLAS_THREAD_SAFE; // Not shared, so ok.
  condVals01.extend<float>( "lvlid" );
  condVals01[0].data<float>() = float( padLevelID ); // BARREL HV PADS
  subQuery01.setCondition( condStr01, condVals01 );
  query0->addToTableList( "Result01" );

  coral::IQueryDefinition& subQuery02 = query0->defineSubQuery( "Result02" );
  subQuery02.addToOutputList( "TREE_NODE.path_to", "lineName" );
  subQuery02.addToOutputList( "TREE_NODE.node_id", "treenode_nodeid" );
  subQuery02.addToTableList( "TREE_NODE" );
  std::string condStr02 = "TREE_NODE.level_id = :lvlid2";
  coral::AttributeList condVals02 ATLAS_THREAD_SAFE; // not shared, so ok
  condVals02.extend<float>( "lvlid2" );
  condVals02[0].data<float>() = float( 64 ); // HVLINE LOGICAL NAMES
  subQuery02.setCondition( condStr02, condVals02 );
  query0->addToTableList( "Result02" );

  coral::AttributeList empty ATLAS_THREAD_SAFE; // Not shared, so ok
  query0->setCondition( "Result01.corrmap_nodeidst = Result02.treenode_nodeid", empty );

  int nRows = 0;
  coral::ICursor& cursor0 = query0->execute();
  while ( cursor0.next() ) {
    ++nRows;
    const coral::AttributeList& row0 = cursor0.currentRow();

    // Parse the padName;
    std::string padName = row0[0].data<std::string>();
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
    int stackNum = atoi(stackNumAsString.c_str());
    int moduleNum = atoi(moduleNumAsString.c_str());
    int padNum = atoi(padNumAsString.c_str());
    int hashedPad = HashThisPad( detector, stackNum, moduleNum, padNum );

    // Parse the logical HV line name into COOL channel name
    std::string lineName = row0[1].data<std::string>();
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

    // Add it to the map!
    theMap.insert( make_pair( hashedPad, chanName ) );

//     std::cout << "HashedPad = " << hashedPad
// 	      << "\nPadNum = " << stackNumAsString
// 	      << "/" << moduleNumAsString << "/" << padNumAsString
// 	      << "\nLine = " << lineName << std::endl;

  }

}

//------------------------------------------------------
/// Read Mapping stuff
void TRT_COOLCORALClient::GetHVLineFromPad( const DetectorType& detector,
                                            int stack, int module, int pad,
                                            std::vector<std::string>& outputList ) {
  m_verbose = false;

  struct timeval start_time;
  struct timeval end_time;
  unsigned long long total_usecs;

  gettimeofday(&start_time, NULL);

  m_session->transaction().start(true);

  coral::IQuery* query0 = m_session->nominalSchema().tableHandle("TREE_NODE").newQuery();

  query0->addToOutputList("TREE_NODE.node_id" );
  //query0->addToOutputList("TREE_NODE.node_name" );

  std::string lvldet;
  int fbox_map_id;
  int pad_map_id;

  if (detector == 81) {
    lvldet = "ATLTRTBHVB:Barrel";
    fbox_map_id = 83;
    pad_map_id = 86;
  }
  else
    {
      std::cout << "Ooops: you wanted the end-cap, sorry" << std::endl;
      return;
    }

  lvldet += "/Stack(";
  lvldet += ToString(stack);
  lvldet += ")/Module_Type_";
  lvldet += ToString(module);

  std::string corr_cond = "TREE_NODE.tree_id = :treeid";
  corr_cond += " AND TREE_NODE.level_id = :lvlid";
  corr_cond += " AND TREE_NODE.path_to = :lvldet";


  coral::AttributeList corr_condData ATLAS_THREAD_SAFE; // Not shared, so ok.
  corr_condData.extend<int>( "treeid" );
  corr_condData.extend<int>( "lvlid" );
  corr_condData.extend<std::string>( "lvldet" );

  query0->setCondition( corr_cond, corr_condData);
  corr_condData[0].data<int>() = detector;
  corr_condData[1].data<int>() = fbox_map_id;  // going to get the fusebox
  corr_condData[2].data<std::string>() = lvldet;


  coral::ICursor& cursor0 = query0->execute();

  int nlines = 0;

  std::vector<float> PadID;
  std::vector<std::string> PadName;

  std::map<float,std::string> PadMap;

  while ( cursor0.next() ) {
    const coral::AttributeList &row0 = cursor0.currentRow();
    //    std::cout << "Module ID: " << row0[0].data<float>() << std::endl;
    coral::IQuery* query1 = m_session->nominalSchema().tableHandle("TREE_NODE").newQuery();
    query1->addToOutputList("TREE_NODE.node_id" );
    query1->addToOutputList("TREE_NODE.node_name" );
    std::string corr_cond1 = "TREE_NODE.parent_id = :treeid";
    coral::AttributeList corr_condData1 ATLAS_THREAD_SAFE; // Not shared, so ok.
    corr_condData1.extend<float>( "treeid" );
    query1->setCondition( corr_cond1, corr_condData1);
    corr_condData1[0].data<float>() = row0[0].data<float>();
    coral::ICursor& cursor1 = query1->execute();
    while ( cursor1.next() ) {
      const coral::AttributeList &row1 = cursor1.currentRow();
      coral::IQuery* query2 = m_session->nominalSchema().tableHandle("TREE_NODE").newQuery();
      query2->addToOutputList("TREE_NODE.node_id" );
      query2->addToOutputList("TREE_NODE.node_name" );
      std::string corr_cond2 = "TREE_NODE.parent_id = :treeid";
      coral::AttributeList corr_condData2 ATLAS_THREAD_SAFE; // Not shared, so ok.
      corr_condData2.extend<float>( "treeid" );
      query2->setCondition( corr_cond2, corr_condData2);
      corr_condData2[0].data<float>() = row1[0].data<float>();
      coral::ICursor& cursor2 = query2->execute();
     while ( cursor2.next() ) {
        const coral::AttributeList &row2 = cursor2.currentRow();
        coral::IQuery* query3 = m_session->nominalSchema().tableHandle("TREE_NODE").newQuery();
        query3->addToOutputList("TREE_NODE.node_id" );
        query3->addToOutputList("TREE_NODE.node_name" );
        std::string corr_cond3 = "TREE_NODE.parent_id = :treeid";
	coral::AttributeList corr_condData3 ATLAS_THREAD_SAFE; // Not shared, so ok.
        corr_condData3.extend<float>( "treeid" );
        query3->setCondition( corr_cond3, corr_condData3);
        corr_condData3[0].data<float>() = row2[0].data<float>();
        coral::ICursor& cursor3 = query3->execute();
	while ( cursor3.next() ) {
          const coral::AttributeList &row3 = cursor3.currentRow();
          PadID.push_back(row3[0].data<float>());
          PadName.push_back(row3[1].data<std::string>());
          PadMap.insert(make_pair(row3[0].data<float>(),row3[1].data<std::string>()));
	}
        delete query3;
      }
      delete query2;
    }
    delete query1;
    nlines++;
  }
  delete query0;

  float pad_node_id = 0.;

  std::map<float,std::string>::iterator PadMap_iter;

  for (PadMap_iter=PadMap.begin(); PadMap_iter != PadMap.end(); PadMap_iter++)
    {
      std::string theString = (*PadMap_iter).second;
      int bg = theString.find_first_of("d");
      int ed = theString.find_first_of("_");
      int start = bg+1;
      int length = ed - bg - 1;
      std::string PadNumAsString( theString, start, length );
      int padnr;
      FromString(padnr, PadNumAsString);
      if (padnr == pad) {
        pad_node_id = (*PadMap_iter).first;
        int bg2 = theString.find_first_of("(");
        int ed2 = theString.find_first_of(")");
        int start2 = bg2+1;
        int length2 = ed2 - bg2 - 1;
        std::string theStraws( theString, start2, length2 );
	// std::cout << std::endl;
	// std::cout << "Detector/Stack/Module: " << lvldet << std::endl;
	// std::cout << "Straws involved: " << theStraws << std::endl;
      }
    }


  coral::IQuery* query00 = m_session->nominalSchema().newQuery();

  query00->addToOutputList("TREE_NODE_MAP.node_id_in" );

  query00->addToTableList("CORR_MAP");
  query00->addToTableList("TREE_NODE");
  query00->addToTableList("TREE_NODE_MAP");

  std::string corr_cond00 = "TREE_NODE.tree_id = :treeid AND TREE_NODE.level_id = :lvlid AND TREE_NODE.node_id= :nodeid";
  corr_cond00 += " AND TREE_NODE.parent_id=CORR_MAP.node_id_det";
  corr_cond00 += " AND CORR_MAP.node_id_st=TREE_NODE_MAP.node_id_out";


  coral::AttributeList corr_condData00 ATLAS_THREAD_SAFE; // Not shared, so ok.
  corr_condData00.extend<int>( "treeid" );
  corr_condData00.extend<int>( "lvlid" );
  corr_condData00.extend<float>( "nodeid" );

  query00->setCondition( corr_cond00, corr_condData00);
  corr_condData00[0].data<int>() = detector;
  corr_condData00[1].data<int>() = pad_map_id;
  corr_condData00[2].data<float>() = pad_node_id;


  coral::ICursor& cursor00 = query00->execute();

  int nlines00 = 0;

  while ( cursor00.next() ) {
   const coral::AttributeList &row00 = cursor00.currentRow();

    coral::IQuery* query01 = m_session->nominalSchema().tableHandle("TREE_NODE").newQuery();
    query01->addToOutputList("TREE_NODE.path_to" );

    std::string corr_cond01 = "TREE_NODE.node_id = :treefinal";
    coral::AttributeList corr_condData01 ATLAS_THREAD_SAFE; // Not shared, so ok.
    corr_condData01.extend<float>( "treefinal" );
    query01->setCondition( corr_cond01, corr_condData01);

    corr_condData01[0].data<float>() = row00[0].data<float>();
    coral::ICursor& cursor01 = query01->execute();

    while ( cursor01.next() ) {
      const coral::AttributeList &row01 = cursor01.currentRow();
      std::string thePathTo = row01[0].data<std::string>();
      outputList.push_back(thePathTo);
      //      std::cout << "TREE_NODE.PATH_TO: " << thePathTo << std::endl;
      //      std::cout << "TREE_NODE_MAP.node_id_in: " << row00[0].data<float>() << std::endl;
      nlines00++;
    }
    delete query01;
  }
  delete query00;

  //  std::cout << "Number of lines: " << nlines00 << std::endl;



  m_session->transaction().commit();

  // measuring elapsed time
  gettimeofday(&end_time, NULL);
  total_usecs = (end_time.tv_sec-start_time.tv_sec) * 1000000 +
    (end_time.tv_usec-start_time.tv_usec);
  if(m_verbose) std::cout << "Total time was " << total_usecs << " usec" << std::endl;


}
