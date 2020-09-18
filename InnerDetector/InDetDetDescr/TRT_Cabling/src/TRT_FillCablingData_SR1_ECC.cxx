/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

//
// Implementation file for TRT_FillCablingData_SR1_ECC class
//     P.T. Keener
//

/*
 * Assumptions:
 *    One side of the barrel is readout, namely side A
 *    We need to map between phi sector index used in the offline and
 *       the Source IDs programmed in the hardware.
 *
 * The m_identfier* and m_collID vectors are indexed by ROB source ID - 1.
 * The magic mapping between ROB source IDs and phi sector indices is in 
 * getRobID() and in fillCollID().  Everything else should just fall 
 * through and gets the Right Answer.
 */

//#include <stdio.h>

//#include <iostream>
#include "TRT_FillCablingData_SR1_ECC.h"
#include <fstream>

//#include <functional>
//#include <algorithm>

#include "eformat/SourceIdentifier.h"  // change to new eformat v3
#include "PathResolver/PathResolver.h"



using eformat::helper::SourceIdentifier; 
using namespace std;

static
bool greater_RG_GCM( const GlobalCableMap_t *a, const GlobalCableMap_t *b ),
     greater_Phi_GCM( const GlobalCableMap_t *a, const GlobalCableMap_t *b );

static const InterfaceID IID_ITRT_FillCablingData_SR1_ECC
                            ("TRT_FillCablingData_SR1_ECC", 1, 0);

  // Constructor
TRT_FillCablingData_SR1_ECC::TRT_FillCablingData_SR1_ECC( const std::string& type, const std::string& 
name,const IInterface* parent ):  AthAlgTool(type,name,parent),
				  m_TRTHelper(0)
{
  declareInterface< TRT_FillCablingData_SR1_ECC >( this );   
}

	 
  // Destructor
TRT_FillCablingData_SR1_ECC::~TRT_FillCablingData_SR1_ECC()
{}


const InterfaceID& TRT_FillCablingData_SR1_ECC::interfaceID( )
{ return IID_ITRT_FillCablingData_SR1_ECC; }


  // Initialisation
StatusCode TRT_FillCablingData_SR1_ECC::initialize( )
{
  ATH_MSG_INFO( "TRT_FillCablingData_SR1_ECC::initialize" ); 


  // Get the TRT Helper
  if (detStore()->retrieve(m_TRTHelper, "TRT_ID").isFailure()) 
  {
    ATH_MSG_FATAL( "Could not get TRT ID helper" );
    return StatusCode::FAILURE;
  }
  m_cntx = m_TRTHelper->straw_layer_context();


  m_cabling = new TRT_CablingData;
  

  defineParameters();
  defineTables();
  defineCollID();
  
  ATH_MSG_INFO( "TRT_FillCablingData_SR1_ECC::initializiation finished" );

  return StatusCode::SUCCESS;
}


StatusCode TRT_FillCablingData_SR1_ECC::finalize()
{
  delete m_cabling;

  StatusCode sc = AlgTool::finalize(); 
  return sc;
}


TRT_CablingData* TRT_FillCablingData_SR1_ECC::fillData()
{
  return m_cabling;
}  


void TRT_FillCablingData_SR1_ECC::defineParameters()
{
  m_numberOfRings = 3;

  m_numberOfLayersA = 19;
  m_numberOfLayersB = 24;
  m_numberOfLayersC = 30;

  //  m_numberOfStrawsInBarrelROD = 1642;
  m_numberOfIdentifierSectors = 3;

  // Mapping from phi sector index to source id
  //m_phi_to_source.push_back( 2 );        // Phi 0 is Source ID 0x3102
  //m_phi_to_source.push_back( 3 );        // Phi 1 is Source ID 0x3103
  //  m_phi_to_source.push_back( 3 );        // Phi 0 is Source ID 0x3103
  //  m_phi_to_source.push_back( 2 );        // Phi 1 is Source ID 0x3102
  // NB: Source ID 0x3101 is the 3S boards on BOTH Phi 0 and Phi 1
     
   
  int tmpArray[] = {15, 16, 16, 16, 16, 17, 17, 17, 17, 17, 18,           // Module Type 1
		    18, 18, 18, 18, 19, 19, 19, 18,                       // Module Type 1
		    19, 20, 20, 20, 20, 20, 21, 21, 21, 21, 21,           // Module Type 2
		    22, 22, 22, 22, 22, 23, 23, 23, 23, 23, 24, 24, 23,   // Module Type 2
		    23, 24, 24, 24, 24, 25, 25, 25, 25, 25, 26,           // Module Type 3
		    26, 26, 26, 26, 27, 27, 27, 27, 27, 28, 28, 28, 28,   // Module Type 3
		    28, 29, 29, 29, 29, 28, -1};                          // Module Type 3

  int i=0;
  int RunningCount=0;
  while( -1 != tmpArray[i] )
  {
     RunningCount += tmpArray[i];
     m_numberOfStrawsInLayers.push_back( RunningCount );

     i++;
  }
  m_numberOfStrawsInLayers.push_back( -1 );

  return;
}


  // Fill Tables with IDs for all straws
void TRT_FillCablingData_SR1_ECC::defineTables()
{
  ATH_MSG_INFO( "In defineTables" ); 



  // Reading mapping   
  int moduleId, strawLayerId, strawInLayerId;
  Identifier strawID;
  IdentifierHash hashId;

  uint32_t strawNumber, bufferOffset;
  string thisLine;

  std::string dataFile;
  std::string file;

  std::ifstream *inputFile;

    /**************************************************************/
    /*                  Endcap Connector Tables                   */
    /**************************************************************/

    /*
     * We assume the data look like:

     group 3
     group 2
     group 1
     group 0

     and within each group (444 words):

     <DTMROC head 25> <DTMROC head 24> ... <DTMROC head 13>
     <start word>
     <DTMROC 25, chan 15> <DTMROC 24, chan 16> ... <DTMROC 13, chan 16>
     <DTMROC 25, chan 14> <DTMROC 24, chan 14> ... <DTMROC 13, chan 14>
     ...                 ...                       ...
     <DTMROC 25, chan 0> <DTMROC 24, chan 0> ... <DTMROC 13, chan 0>

     <DTMROC head 12> <DTMROC head 11> ... <DTMROC head 0>
     <start word>
     <DTMROC 12, chan 15> <DTMROC 11, chan 16> ... <DTMROC 0, chan 16>
     <DTMROC 12, chan 14> <DTMROC 11, chan 14> ... <DTMROC 0, chan 14>
     ...                 ...                       ...
     <DTMROC 12, chan 0> <DTMROC 11, chan 0> ... <DTMROC 0, chan 0>

    */


    /*
     * Left sectors, layers 0-63
     */
    ATH_MSG_INFO( "defineTables: rod1l " ); 

    dataFile = "TRT_SR1_ec_rod1l.dat";
    file = PathResolver::find_file (dataFile, "DATAPATH");
    inputFile = new std::ifstream( file.c_str() );

    if (!inputFile->is_open())
      {
	ATH_MSG_FATAL( "TRT_FillCablingData_SR1_ECC: Could not open file " << dataFile );
      }

    while ( getline( *inputFile, thisLine ) )
      {
	int rodgroup, rodline, dtmchannel, strawlayer, strawnumber;

	string::size_type loc = thisLine.find( "#" );
	if ( loc != string::npos )
	  {
	    thisLine.replace( loc, thisLine.length(), 1, ' ' );
      
	    loc = thisLine.find_first_not_of( " \t" );
	    if ( loc == string::npos )
	      continue;
	  }

	istringstream parse( thisLine );

	parse >> rodgroup >> rodline >> dtmchannel >> strawlayer >> strawnumber;
	//    parse >> strawNumber >> bufferOffset;

	/*
	 * HACK!  We do not have a good straw numbering scheme for the endcap 
	 * and we do not yet have enough info to produce a real offline
	 * identifier.  So we make something up!
	 */

	strawNumber = (strawlayer & 0xff) << 8 | (strawnumber & 0xff);

	bufferOffset = ((3 - rodgroup) * 444) + 14;
	if ( rodline > 12 )
	  {
	    bufferOffset += (25 - rodline) + ((dtmchannel) * 13);
	  }
	else
	  {
	    bufferOffset += 222 + (12 - rodline) + ((dtmchannel) * 13);
	  }

	m_ec_1l_map[bufferOffset] = strawNumber;
      }

    inputFile->close();


    /*
     * Left sectors, layers 64-95
     */
    ATH_MSG_INFO( "defineTables: rod2l " ); 

    dataFile = "TRT_SR1_ec_rod2l.dat";
    file = PathResolver::find_file (dataFile, "DATAPATH");
    inputFile = new std::ifstream( file.c_str() );

    if (!inputFile->is_open())
      {
	ATH_MSG_FATAL( "TRT_FillCablingData_SR1_ECC: Could not open file " << dataFile );
      }

    while ( getline( *inputFile, thisLine ) )
      {
	int rodgroup, rodline, dtmchannel, strawlayer, strawnumber;

	string::size_type loc = thisLine.find( "#" );
	if ( loc != string::npos )
	  {
	    thisLine.replace( loc, thisLine.length(), 1, ' ' );
      
	    loc = thisLine.find_first_not_of( " \t" );
	    if ( loc == string::npos )
	      continue;
	  }

	istringstream parse( thisLine );

	parse >> rodgroup >> rodline >> dtmchannel >> strawlayer >> strawnumber;
	//    parse >> strawNumber >> bufferOffset;

	/*
	 * HACK!  We do not have a good straw numbering scheme for the endcap 
	 * and we do not yet have enough info to produce a real offline
	 * identifier.  So we make something up!
	 */

	strawNumber = (strawlayer & 0xff) << 8 | (strawnumber & 0xff);

	bufferOffset = ((1 - rodgroup) * 444) + 14;
	if ( rodline > 12 )
	  {
	    bufferOffset += (25 - rodline) + ((dtmchannel) * 13);
	  }
	else
	  {
	    bufferOffset += 222 + (12 - rodline) + ((dtmchannel) * 13);
	  }

	m_ec_2l_map[bufferOffset] = strawNumber;
      }

    inputFile->close();


    /*
     * Left sectors, layers 96-159
     */
    ATH_MSG_INFO( "defineTables: rod3l " ); 

    dataFile = "TRT_SR1_ec_rod3l.dat";
    file = PathResolver::find_file (dataFile, "DATAPATH");
    inputFile = new std::ifstream( file.c_str() );

    if (!inputFile->is_open())
      {
	ATH_MSG_FATAL( "TRT_FillCablingData_SR1_ECC: Could not open file " << dataFile );
      }

    while ( getline( *inputFile, thisLine ) )
      {
	int rodgroup, rodline, dtmchannel, strawlayer, strawnumber;

	string::size_type loc = thisLine.find( "#" );
	if ( loc != string::npos )
	  {
	    thisLine.replace( loc, thisLine.length(), 1, ' ' );
      
	    loc = thisLine.find_first_not_of( " \t" );
	    if ( loc == string::npos )
	      continue;
	  }

	istringstream parse( thisLine );

	parse >> rodgroup >> rodline >> dtmchannel >> strawlayer >> strawnumber;
	//    parse >> strawNumber >> bufferOffset;

	/*
	 * HACK!  We do not have a good straw numbering scheme for the endcap 
	 * and we do not yet have enough info to produce a real offline
	 * identifier.  So we make something up!
	 */

	strawNumber = (strawlayer & 0xff) << 8 | (strawnumber & 0xff);

	bufferOffset = ((3 - rodgroup) * 444) + 14;
	if ( rodline > 12 )
	  {
	    bufferOffset += (25 - rodline) + ((dtmchannel) * 13);
	  }
	else
	  {
	    bufferOffset += 222 + (12 - rodline) + ((dtmchannel) * 13);
	  }

	m_ec_3l_map[bufferOffset] = strawNumber;
      }

    inputFile->close();


    /*
     * Right sectors, layers 0-63
     */
    ATH_MSG_INFO( "defineTables: rod1r " ); 

    dataFile = "TRT_SR1_ec_rod1r.dat";
    file = PathResolver::find_file (dataFile, "DATAPATH");
    inputFile = new std::ifstream( file.c_str() );

    if (!inputFile->is_open())
      {
	ATH_MSG_FATAL( "TRT_FillCablingData_SR1_ECC: Could not open file " << dataFile );
      }

    while ( getline( *inputFile, thisLine ) )
      {
	int rodgroup, rodline, dtmchannel, strawlayer, strawnumber;

	string::size_type loc = thisLine.find( "#" );
	if ( loc != string::npos )
	  {
	    thisLine.replace( loc, thisLine.length(), 1, ' ' );
      
	    loc = thisLine.find_first_not_of( " \t" );
	    if ( loc == string::npos )
	      continue;
	  }

	istringstream parse( thisLine );

	parse >> rodgroup >> rodline >> dtmchannel >> strawlayer >> strawnumber;
	//    parse >> strawNumber >> bufferOffset;

	/*
	 * HACK!  We do not have a good straw numbering scheme for the endcap 
	 * and we do not yet have enough info to produce a real offline
	 * identifier.  So we make something up!
	 */

	strawNumber = (strawlayer & 0xff) << 8 | (strawnumber & 0xff);

	bufferOffset = ((3 - rodgroup) * 444) + 14;
	if ( rodline > 12 )
	  {
	    bufferOffset += (25 - rodline) + ((dtmchannel) * 13);
	  }
	else
	  {
	    bufferOffset += 222 + (12 - rodline) + ((dtmchannel) * 13);
	  }

	m_ec_1r_map[bufferOffset] = strawNumber;
      }

    inputFile->close();


    /*
     * Right sectors, layers 64-95
     */
    ATH_MSG_INFO( "defineTables: rod2r " ); 

    dataFile = "TRT_SR1_ec_rod2r.dat";
    file = PathResolver::find_file (dataFile, "DATAPATH");
    inputFile = new std::ifstream( file.c_str() );

    if (!inputFile->is_open())
      {
	ATH_MSG_FATAL( "TRT_FillCablingData_SR1_ECC: Could not open file " << dataFile );
      }

    while ( getline( *inputFile, thisLine ) )
      {
	int rodgroup, rodline, dtmchannel, strawlayer, strawnumber;

	string::size_type loc = thisLine.find( "#" );
	if ( loc != string::npos )
	  {
	    thisLine.replace( loc, thisLine.length(), 1, ' ' );
      
	    loc = thisLine.find_first_not_of( " \t" );
	    if ( loc == string::npos )
	      continue;
	  }

	istringstream parse( thisLine );

	parse >> rodgroup >> rodline >> dtmchannel >> strawlayer >> strawnumber;
	//    parse >> strawNumber >> bufferOffset;

	/*
	 * HACK!  We do not have a good straw numbering scheme for the endcap 
	 * and we do not yet have enough info to produce a real offline
	 * identifier.  So we make something up!
	 */

	strawNumber = (strawlayer & 0xff) << 8 | (strawnumber & 0xff);

	bufferOffset = ((1 - rodgroup) * 444) + 14;
	if ( rodline > 12 )
	  {
	    bufferOffset += (25 - rodline) + ((dtmchannel) * 13);
	  }
	else
	  {
	    bufferOffset += 222 + (12 - rodline) + ((dtmchannel) * 13);
	  }

	m_ec_2r_map[bufferOffset] = strawNumber;
      }

    inputFile->close();


    /*
     * Right sectors, layers 96-159
     */
    ATH_MSG_INFO( "defineTables: rod3r " ); 

    dataFile = "TRT_SR1_ec_rod3r.dat";
    file = PathResolver::find_file (dataFile, "DATAPATH");
    inputFile = new std::ifstream( file.c_str() );

    if (!inputFile->is_open())
      {
	ATH_MSG_FATAL( "TRT_FillCablingData_SR1_ECC: Could not open file " << dataFile );
      }

    while ( getline( *inputFile, thisLine ) )
      {
	int rodgroup, rodline, dtmchannel, strawlayer, strawnumber;

	string::size_type loc = thisLine.find( "#" );
	if ( loc != string::npos )
	  {
	    thisLine.replace( loc, thisLine.length(), 1, ' ' );
      
	    loc = thisLine.find_first_not_of( " \t" );
	    if ( loc == string::npos )
	      continue;
	  }

	istringstream parse( thisLine );

	parse >> rodgroup >> rodline >> dtmchannel >> strawlayer >> strawnumber;
	//    parse >> strawNumber >> bufferOffset;

	/*
	 * HACK!  We do not have a good straw numbering scheme for the endcap 
	 * and we do not yet have enough info to produce a real offline
	 * identifier.  So we make something up!
	 */

	strawNumber = (strawlayer & 0xff) << 8 | (strawnumber & 0xff);

	bufferOffset = ((3 - rodgroup) * 444) + 14;
	if ( rodline > 12 )
	  {
	    bufferOffset += (25 - rodline) + ((dtmchannel) * 13);
	  }
	else
	  {
	    bufferOffset += 222 + (12 - rodline) + ((dtmchannel) * 13);
	  }

	m_ec_3r_map[bufferOffset] = strawNumber;
      }

    inputFile->close();

  /**************************************************************/
  /*               Global Cabling Specification                 */
  /**************************************************************/

  ATH_MSG_INFO( "defineTables: Global Spec " ); 

  dataFile = "TRT_SR1_EndcapC_Cable_map.dat";


  file = PathResolver::find_file (dataFile, "DATAPATH");
  inputFile = new std::ifstream( file.c_str() );

  if (!inputFile->is_open())
  {
     ATH_MSG_FATAL( "TRT_FillCablingData_SR1_ECC: Could not open file " << dataFile );
  }

  uint32_t srcId;
  std::string feId;

  while ( getline( *inputFile, thisLine ) )
  {
    if ( thisLine.length() == 0 )
      continue;

    string::size_type loc = thisLine.find( "#" );
    if ( loc != string::npos )
    {
      thisLine.replace( loc, thisLine.length(), 1, ' ' );
      
      loc = thisLine.find_first_not_of( " \t" );
      if ( loc == string::npos )
	continue;
    }


     istringstream parse( thisLine );

     GlobalCableMap_t *my_map = new GlobalCableMap_t;

     parse >> (hex) >> srcId >> (dec) >> my_map->Phi >> my_map->RODGroup >> my_map->FEid;


     //     int this_phi;    // set but not used

     if ( (srcId & 0xFF0000) == 0x310000 )
     {
	my_map->SubDet = 1;
	//	this_phi = my_map->Phi;    // set but not used
     }
     else if ( (srcId & 0xFF0000) == 0x320000 )
     {
	my_map->SubDet = -1;
	//	this_phi = -my_map->Phi;    // set but not used
     }
     else if ( (srcId & 0xFF0000) == 0x330000 )
     {
	my_map->SubDet = 2;
	//	this_phi = my_map->Phi;    // set but not used
     }
     else if ( (srcId & 0xFF0000) == 0x340000 )
     {
	my_map->SubDet = -2;
	//	this_phi = -my_map->Phi;    // set but not used
     }
     else
	ATH_MSG_FATAL( "TRT_FillCablingData_SR1_ECC: Invalid source id " << (hex) << srcId << (dec) );


     /*
      * my_phi is 
                    32 -  63 for +barrel
		    64 -  95 for +end cap
		   -32 - -63 for -barrel
		   -64 - -95 for -end cap
     */


     //     int my_phi = my_map->Phi * my_map->SubDet;
     int my_phi = my_map->SubDet * 32 + my_map->Phi;


     std::vector<uint32_t>::iterator it = (m_phi_to_source[my_phi]).begin();
     while ( (it != (m_phi_to_source[my_phi]).end()) && (srcId != *it) )
	++it;

     if ( it == m_phi_to_source[my_phi].end() )
     {
	(m_phi_to_source[my_phi]).push_back( srcId );
     }


     (m_CableMap[srcId]).push_back( my_map );
  }

  inputFile->close();

#ifdef NOTDEF
  int ptk_i;
  for ( ptk_i=1; ptk_i<=444; ptk_i++ )
  {
     std::map<int, int>::iterator map_it = m_1BS_1BL_map.find( ptk_i );
     //     if ( map_it != m_1BS_1BL_map.end() )
     //	cout << "Offset " << map_it->first << " is Straw " << map_it->second << endl;
  }
#endif // NOTDEF

  GlobalCableMap::iterator pos;
  for ( pos=m_CableMap.begin(); pos != m_CableMap.end(); ++pos )
  {
     std::vector< GlobalCableMap_t *> GCM;

     GCM = pos->second;

     sort( GCM.begin(), GCM.end(), greater_RG_GCM );

     int ConnectorCount=-1;
     std::vector< GlobalCableMap_t *>::iterator it;
     for ( it=GCM.begin(); it != GCM.end(); ++it )
     {
	ConnectorCount++;

	GlobalCableMap_t *my_map = *it;
	std::map<int, int> con_map;


	if ( my_map->FEid == "1BS_1BL" )
	   con_map = m_1BS_1BL_map;
	else if ( my_map->FEid == "2BS_2BL1" )
	   con_map = m_2BS_2BL1_map;
	else if ( my_map->FEid == "2BL2_3BL1" )
	   con_map = m_2BL2_3BL1_map;
	else if ( my_map->FEid == "3BL2" )
	   con_map = m_3BL2_map;
	else if ( my_map->FEid == "3BS" )
	   con_map = m_3BS_map;
	else if ( my_map->FEid == "1FS_1FL" )
	   con_map = m_1FS_1FL_map;
	else if ( my_map->FEid == "2FS_2FL1" )
	   con_map = m_2FS_2FL1_map;
	else if ( my_map->FEid == "2FL2_3FL1" )
	   con_map = m_2FL2_3FL1_map;
	else if ( my_map->FEid == "3FL2" )
	   con_map = m_3FL2_map;
	else if ( my_map->FEid == "3FS" )
	   con_map = m_3FS_map;
	else if ( my_map->FEid == "ec_rod1l" )
	   con_map = m_ec_1l_map;
	else if ( my_map->FEid == "ec_rod2l" )
	   con_map = m_ec_2l_map;
	else if ( my_map->FEid == "ec_rod3l" )
	   con_map = m_ec_3l_map;
	else if ( my_map->FEid == "ec_rod1r" )
	   con_map = m_ec_1r_map;
	else if ( my_map->FEid == "ec_rod2r" )
	   con_map = m_ec_2r_map;
	else if ( my_map->FEid == "ec_rod3r" )
	   con_map = m_ec_3r_map;


	int i;
	Identifier NULLstrawID(0);

	if ( abs( my_map->SubDet ) == 1 )   // barrel
	{
	  for ( i=1; i<=444; i++ )      // loop over buffer offset
	  {
	    std::map<int, int>::iterator map_it = con_map.find( i );

	    if ( map_it == con_map.end() )
	    {
	      m_cabling->set_identfierForAllStraws( pos->first, 
						    i + ConnectorCount*444,
						    NULLstrawID );
	      continue;
	    }

	    int BufferLocation = map_it->first + ConnectorCount*444;
	    int Straw = map_it->second - 1;   // Offline starts at 0

	    strawLayerId = 0;
	    while ( (m_numberOfStrawsInLayers[strawLayerId] <= Straw) &&
		    (m_numberOfStrawsInLayers[strawLayerId] != -1) )
	    {
	      strawLayerId++;
	    }

	    if ( m_numberOfStrawsInLayers[strawLayerId] == -1)
	    {
	      ATH_MSG_WARNING( "Invalid Straw in table: " << Straw ); 
	      continue;
	    }


	    strawInLayerId = m_numberOfStrawsInLayers[strawLayerId] - Straw - 1;

	    moduleId = -1;
	    if ( strawLayerId < m_numberOfLayersA )
	      moduleId = 0;
	    else if ( strawLayerId < (m_numberOfLayersA + m_numberOfLayersB) )
	    {
	      moduleId = 1;
	      strawLayerId = strawLayerId - m_numberOfLayersA;
	    }
	    else if ( strawLayerId < (m_numberOfLayersA + m_numberOfLayersB + m_numberOfLayersC) )
	    {
	      moduleId = 2;
	      strawLayerId = strawLayerId - (m_numberOfLayersA + m_numberOfLayersB);
	    }



	    strawID = m_TRTHelper->straw_id( my_map->SubDet, my_map->Phi, 
					     moduleId, strawLayerId, 
					     strawInLayerId );


	    m_cabling->set_identfierForAllStraws( pos->first, BufferLocation,
						  strawID );


	    //	   IdLayer = m_TRTHelper->layer_id( strawID );
	    
	    //	   int result = m_TRTHelper->get_hash( strawID, hashId, &m_cntx );

	    Identifier Lid = m_TRTHelper->layer_id( my_map->SubDet,
						    my_map->Phi, moduleId,
						    strawLayerId );

	    int result = m_TRTHelper->get_hash( Lid, hashId, &m_cntx );
	    if ( result != 0 )
	    {
	      ATH_MSG_WARNING( ">>>>>>>>>>> Get Hash FAILED! <<<<<<<<<<" );
	      ATH_MSG_WARNING( "Straw     = " << Straw \
		    << " array    = " << m_numberOfStrawsInLayers[strawLayerId] \
		    << " SubDet    = " << my_map->SubDet \
		    << " Phi      = " << my_map->Phi \
		    << " Module   = " << moduleId \
		    << " Layer    = " << strawLayerId \
		    << " StrInLay = " << strawInLayerId );
	    }  


	    m_cabling->set_identfierHashForAllStraws( pos->first,
						      BufferLocation,
						      hashId );
	  }
	}
	else       // Endcap
	{
	  for ( i=0; i<1776; i++ )      // loop over buffer offset
	  {
	    std::map<int, int>::iterator map_it = con_map.find( i );

	    if ( map_it == con_map.end() )
	    {
	      m_cabling->set_identfierForAllStraws( pos->first, i,
						    NULLstrawID );
	      continue;
	    }

	    int BufferLocation = i;
	    int Straw = map_it->second;   // Offline starts at 0

	    strawLayerId = (Straw >> 8) & 0xff;
	    strawInLayerId = Straw & 0xff;

	    moduleId = strawLayerId / 8;
	    if ( moduleId < 12 )           // A wheel?
	    {
	      moduleId = moduleId / 2;
	      strawLayerId = strawLayerId % 16;
	    }
	    else                           // B wheel
	    {
	      moduleId = moduleId - 6;
	      strawLayerId = strawLayerId % 8;
	    }


	    strawID = m_TRTHelper->straw_id( my_map->SubDet, my_map->Phi, 
					     moduleId, strawLayerId, 
					     strawInLayerId );


	    // Decode the identifier
	    //	ExpandedIdentifier strawExpId;
	    // m_TRTHelper->get_expanded_id (strawID, strawExpId);
	    // ATH_MSG_INFO( "defineTables() - Identifier: " 
	    //	      << std::left << std::setw(20) << std::string(strawExpId)  << std::setw(0)
	    //	      << " robId: " << std::hex << pos->first << std::dec << " bufferOffset: " << BufferLocation );

	    //	ATH_MSG_INFO( m_TRTHelper->print_to_string( strawID ) << " bufferOffset: " << BufferLocation );
	    
	    m_cabling->set_identfierForAllStraws( pos->first, BufferLocation,
						  strawID );


	    //	   IdLayer = m_TRTHelper->layer_id( strawID );
	    
	    //	   int result = m_TRTHelper->get_hash( strawID, hashId, &m_cntx );

	    Identifier Lid = m_TRTHelper->layer_id( my_map->SubDet,
						    my_map->Phi, moduleId,
						    strawLayerId );

	    int result = m_TRTHelper->get_hash( Lid, hashId, &m_cntx );
	    if ( result != 0 )
	    {
	      ATH_MSG_WARNING( ">>>>>>>>>>> Get Hash FAILED! <<<<<<<<<<" );

              ATH_MSG_WARNING( "Straw     = " << Straw \
                    << " SubDet    = " << my_map->SubDet \
                    << " Phi      = " << my_map->Phi \
                    << " Module   = " << moduleId \
                    << " Layer    = " << strawLayerId \
                    << " StrInLay = " << strawInLayerId );
	    }


	    m_cabling->set_identfierHashForAllStraws( pos->first,
						      BufferLocation,
						      hashId );
	  } // loop over bufferOffsets
	}   // Endcap
     }      // loop over GCM
  }         // loop over CableMap



  ATH_MSG_INFO( "Out of defineTables" ); 

  return;
}

/*
 * Define map of collection IDs for all ROBs
 */
void TRT_FillCablingData_SR1_ECC::defineCollID()
{

   ATH_MSG_INFO( "In defineCollID()" );

   GlobalCableMap::iterator pos;
   for ( pos=m_CableMap.begin(); pos != m_CableMap.end(); ++pos )
   {   
      std::vector<IdentifierHash> * vectID = new std::vector<IdentifierHash>();
      fillCollID( pos->first, *vectID);

      ATH_MSG_INFO( "Adding Collid for ROB " << std::hex << pos->first << std::dec );

      m_cabling->add_collID( pos->first, vectID);
   }

   return;
}


/*
 * Input: ROB Source ID
 * Output: List of Hash IDs, one for each of 73 layers
 */
void TRT_FillCablingData_SR1_ECC::fillCollID(uint32_t rob_id,  
std::vector<IdentifierHash> & ids)
{

  IdentifierHash idHash;
  
  //  eformat::SubDetector det_id;    // set but not used
  //  int module;    // set but not used

  eformat::helper::SourceIdentifier id (rob_id);
  //  det_id = id.subdetector_id();    // set but not used
  //  module = (int) id.module_id();    // set but not used


  std::map<int, std::vector<GlobalCableMap_t *> >::iterator map_it = m_CableMap.find( rob_id );
  if ( map_it == m_CableMap.end() )
  {     
     ATH_MSG_FATAL( "fillCollID: Unable to get CableMap for rob_id " << std::hex << rob_id << std::dec );
     return;
  }

  std::vector< GlobalCableMap_t *> GCM = map_it->second;   

  sort( GCM.begin(), GCM.end(), greater_Phi_GCM );

  std::vector<GlobalCableMap_t *>::iterator it;


  it=GCM.begin();
  while ( it != GCM.end() )
  {
     GlobalCableMap_t *my_map = *it;
     int rings = 0;
     int phi = my_map->Phi;
     int barrelId = my_map->SubDet;

     if ( abs( barrelId ) == 1 )   // barrel?
     {

       bool done = !((my_map->SubDet == barrelId) && (my_map->Phi == phi));
       while ( !done && (it != GCM.end()) )
       {


	 if ( (my_map->FEid == "1BS_1BL") || (my_map->FEid == "1FS_1FL") )
	   rings |= 1;
	 else if ( (my_map->FEid == "2BS_2BL1") || (my_map->FEid == "2FS_2FL1") )
	   rings |= 2;
	 else if ( (my_map->FEid == "2BL2_3BL1") || (my_map->FEid == "2FL2_3FL1") )
	   rings |= 3;
	 else if ( (my_map->FEid == "3BL2") || (my_map->FEid == "3BS") ||
		   (my_map->FEid == "3FL2") || (my_map->FEid == "3FS") )
	   rings |= 4;

	 ++it;
	
	 if ( it != GCM.end() )
	 {
	   my_map = *it;
	   done = !((my_map->SubDet == barrelId) && (my_map->Phi == phi));
	 }

       }


       int phiModule = phi;
       int made_collections = 0;

       for (int ring = 0; ring < m_numberOfRings; ++ring)
       {
	 if ( (ring == 0) && ((1<<ring) & rings) )  // Module Type 1
	 {
	   for (int layer = 0; layer < m_numberOfLayersA; ++layer) 
	   {
	      Identifier id = m_TRTHelper->layer_id(barrelId, phiModule, 
						    ring, layer);

	      if (!m_TRTHelper->get_hash(id, idHash, &m_cntx)) 
	      {
		 ids.push_back(idHash);
		 made_collections++;
	      }
	      else
		 ATH_MSG_FATAL( " Unable to get hash for id " << m_TRTHelper->show_to_string(id) );
	   }
	 }
	 else if ( (ring == 1) && ((1<<ring) & rings) )   // Module Type 2
	 {
	   for (int layer = 0; layer < m_numberOfLayersB; ++layer) 
	   {
	      Identifier id = m_TRTHelper->layer_id(barrelId, phiModule,
						    ring, layer);

	      if (!m_TRTHelper->get_hash(id, idHash, &m_cntx))
	      {
		 ids.push_back(idHash);
		 made_collections++;
	      }
	      else  
		 ATH_MSG_FATAL( " Unable to get hash for id " << m_TRTHelper->show_to_string(id) );
	   }
	 }
	 else if ( (ring == 2) && ((1<<ring) & rings) )   //  Module Type 3
	 {
	   for (int layer = 0; layer < m_numberOfLayersC; ++layer) 
	   {
	      Identifier id = m_TRTHelper->layer_id(barrelId, phiModule,
						    ring, layer);

	      if (!m_TRTHelper->get_hash(id, idHash, &m_cntx)) 
	      {
		 ids.push_back(idHash);
		 made_collections++;
	      }
	      else 
		 ATH_MSG_FATAL( " Unable to get hash for id " << m_TRTHelper->show_to_string(id) );

	   }
	 }
       }
     }
     else   // End cap
     {
       int StartWheel=0;
       int EndWheel=0;
       int StartLayer=0;
       int EndLayer=0;

       int phiModule = phi;
       int made_collections = 0;

       bool done = !((my_map->SubDet == barrelId) && (my_map->Phi == phi));
       while ( !done && (it != GCM.end()) )
       {

	 if ( (my_map->FEid == "ec_rod1l") || 
	      (my_map->FEid == "ec_rod1r") )
	 {
	   StartWheel = 0;
	   EndWheel   = 4;
	   StartLayer = 0;
	   EndLayer   = 16;
	 }
	 else if ( (my_map->FEid == "ec_rod2l") || 
		   (my_map->FEid == "ec_rod2r") )
	 {
	   StartWheel = 4;
	   EndWheel   = 6;
	   StartLayer = 0;
	   EndLayer   = 16;
	 }
	 else	 if ( (my_map->FEid == "ec_rod3l") || 
	      (my_map->FEid == "ec_rod3r") )
	 {
	   StartWheel = 6;
	   EndWheel   = 14;
	   StartLayer = 0;
	   EndLayer   = 8;
	 }
	 else
	 {
	   ATH_MSG_ERROR( "Invalid my_map->FEid.  This should not happen!" );
	   return;
	 }

	 ++it;
	
	 if ( it != GCM.end() )
	 {
	   my_map = *it;
	   done = !((my_map->SubDet == barrelId) && (my_map->Phi == phi));
	 }

       }

       
       for (int wheel = StartWheel; wheel < EndWheel; wheel++ ) 
       {
	 for (int layer = StartLayer; layer < EndLayer; layer++ ) 
	 {
	   Identifier id = m_TRTHelper->layer_id( barrelId, phiModule, 
						  wheel, layer);

	   if (!m_TRTHelper->get_hash(id, idHash, &m_cntx)) 
	   {
	     ids.push_back(idHash);
	     made_collections++;
	   }
	   else
	     ATH_MSG_FATAL( " Unable to get hash for id " << m_TRTHelper->show_to_string(id) );
	 }  // loop over layer
       }    // loop over wheel
     }      // endcap
  }

  return;
}

/*
 * Get ROBIDs for each Detector Element (Straw Layer)
 *
 * Input : Straw ID
 * Output: list of ROB Source IDs
 */
std::vector<uint32_t> TRT_FillCablingData_SR1_ECC::getRobID(Identifier id) 
{
  std::vector<uint32_t> v;


  int id_barrel_ec = m_TRTHelper->barrel_ec(id);
  int id_phi_module = m_TRTHelper->phi_module(id);
  //int id_layer = m_TRTHelper->layer_or_wheel(id);

  /*
   * It seems to be assumed that the ROD/ROB source IDs are aligned to
   * the phi sector numbering.  This is NOT the case for SR1, so we
   * need to fix it up.
   * In addition, for Module type 3 (ie, layer 2), we need to read out
   * the ROB with the 3S boards (number 1), as well as the ROB with
   * the rest of the sector.
   * We assume only one side here.
   */

  id_phi_module = id_phi_module + 32 * id_barrel_ec;

  std::vector<u_int32_t> SourceList = m_phi_to_source[id_phi_module];

  vector<uint32_t>::iterator it1 = SourceList.begin();
  vector<uint32_t>::iterator it2 = SourceList.end();

  while( it1 != it2 )
  {
     eformat::helper::SourceIdentifier sid( *it1 );
     v.push_back(sid.code());

     ++it1;
  }

  return v;
}



/*
 * Sort Global Cable Map by ROD Group
 */
static
bool greater_RG_GCM( const GlobalCableMap_t *a, const GlobalCableMap_t *b )
{
   return ( a->RODGroup > b->RODGroup );
}

/*
 * Sort Global Cable Map by phi
 */
static
bool greater_Phi_GCM( const GlobalCableMap_t *a, const GlobalCableMap_t *b )
{
   return ( a->Phi > b->Phi );
}
