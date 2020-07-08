/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

/*
 * Implementation file for TRT_FillCablingData_DC3 class.
 *
 * NB: This is the case of full ATLAS, without the C wheels, for the ROD05.
 *
 *     P.T. Keener
 */

/*
 * Assumptions:
 *    - The ROD Source IDs give the mapping to the dector:
 *      o Barrel is 3100XX and 3200XX, where XX is the phi sector [1-32]
 *      o EC is 33XX0Y, which XX is the phi sector [1-32] and Y is 1
 *        for the low |Z| wheels and 2 for the high |Z| wheels
 *      + NB: the offline phi sectors are numbered from 0
 *    - We are in the "expanded" mode of ROD05
 * 
 *
 * Notes:
 *    - barrel or ec ids
 *      o 310000 is  1 is Barrel A
 *      o 320000 is -1 is Barrel C
 *      o 330000 is  2 is EC A
 *      o 340000 is -2 is EC C
 *
 * The m_identfier* and m_collID vectors are indexed by ROB source ID - 1.
 * The magic mapping between ROB source IDs and phi sector indices is in 
 * getRobID() and in fillCollID().  Everything else should just fall 
 * through and gets the Right Answer.
 */

#undef ENDCAP_DEFINED

#include <stdio.h>

#include <iostream>
#include "TRT_FillCablingData_DC3.h"
#include <fstream>

#include <functional>
#include <algorithm>

#include "eformat/SourceIdentifier.h"  // change to new eformat v3
#include "PathResolver/PathResolver.h"

#include "CxxUtils/checker_macros.h"

using eformat::helper::SourceIdentifier; 
using namespace std;

static const InterfaceID IID_ITRT_FillCablingData_DC3
                            ("TRT_FillCablingData_DC3", 1, 0);

  // Constructor
TRT_FillCablingData_DC3::TRT_FillCablingData_DC3( const std::string& type, const std::string& 
name,const IInterface* parent ):  AthAlgTool(type,name,parent),
    m_TRTHelper(0), 
    m_PartialDetector(false)
{
  declareInterface< TRT_FillCablingData_DC3 >( this );  

  declareProperty( "Filename", m_inputFilename="" );
  declareProperty( "RealData", m_RealData=false );

}

	 
  // Destructor
TRT_FillCablingData_DC3::~TRT_FillCablingData_DC3()
{}


const InterfaceID& TRT_FillCablingData_DC3::interfaceID( )
{ return IID_ITRT_FillCablingData_DC3; }


  // Initialisation
StatusCode TRT_FillCablingData_DC3::initialize ATLAS_NOT_THREAD_SAFE ( ) // thread unsafe TRT_FillCablingData_DC3::defineTables is used.
{
  ATH_MSG_INFO( "TRT_FillCablingData_DC3::initialize" ); 


  // Get the TRT Helper
  if (detStore()->retrieve(m_TRTHelper, "TRT_ID").isFailure()) 
  {
    ATH_MSG_FATAL( "Could not get TRT ID helper" );
    return StatusCode::FAILURE;
  }
  m_cntx = m_TRTHelper->straw_layer_context();


  m_cabling = new TRT_CablingData;
  
  /*
   * If we aren't reading real data, use the broken EC A mapping
   * used for FDR1
   */
  m_broken_FDR_Cabling = ! m_RealData;


  defineParameters();

  if ( m_broken_FDR_Cabling )
  {
    defineTables_broken_FDR();
    defineCollID_broken_FDR();
  }
  else
  {
    defineTables();
    defineCollID();
  }
  
  ATH_MSG_INFO( "TRT_FillCablingData_DC3::initializiation finished" );

  //  return sc;
  return StatusCode::SUCCESS;
}


StatusCode TRT_FillCablingData_DC3::finalize()
{

  delete m_cabling;

  StatusCode sc = AlgTool::finalize(); 
  return sc;
}


TRT_CablingData* TRT_FillCablingData_DC3::fillData()
{
  return m_cabling;
}  


void TRT_FillCablingData_DC3::defineParameters()
{
   m_numberOfRings = 3;

   m_numberOfLayers[0] = 19;
   m_numberOfLayers[1] = 24;
   m_numberOfLayers[2] = 30;

   
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
      m_StrawsPerLayer.push_back( tmpArray[i] );

      RunningCount += tmpArray[i];
      m_numberOfStrawsInLayers.push_back( RunningCount );

      i++;
   }
   m_numberOfStrawsInLayers.push_back( -1 );

   return;
}


  // Fill Tables with IDs for all straws
void TRT_FillCablingData_DC3::defineTables ATLAS_NOT_THREAD_SAFE () // thread unsafe std::exit is used.
{
   ATH_MSG_INFO( "In defineTables" ); 

   int moduleId, strawLayerId, strawInLayerId;
   Identifier strawID;
   IdentifierHash hashId;

   uint32_t bufferOffset;
   string thisLine;

   std::string dataFile;
   std::string file;



  /**************************************************************/
  /*                       Barrel Tables                        */
  /**************************************************************/

  /*
   * Barrel A side sectors
   */
   {
   ATH_MSG_INFO( "defineTables: Barrel A " ); 

   dataFile = "TRT_ROD05_Barrel_A_Map.dat";
  file = PathResolver::find_file (dataFile, "DATAPATH");
  std::ifstream inputFile ( file.c_str() );

  if (!inputFile.is_open())
  {
     ATH_MSG_FATAL( "TRT_FillCablingData_DC3: Could not open file " << dataFile );
  }

  for ( int ii=0; ii<(120*16); ii++ )
    m_barrel_a_map[ii] = -1;

  while ( getline( inputFile, thisLine ) )
  {
     int RodChannel, DtmChannel, strawnumber, HWaddr, GolNumber;
     string Board;

     string::size_type loc = thisLine.find( "#" );
     if ( loc != string::npos )
     {
	thisLine.replace( loc, thisLine.length(), 1, ' ' );
      
	loc = thisLine.find_first_not_of( " \t" );
	if ( loc == string::npos )
	   continue;
     }

     istringstream parse( thisLine );

     parse >> RodChannel >> DtmChannel >> strawnumber >> HWaddr >> Board >> GolNumber;


     bufferOffset = (120 * DtmChannel) + RodChannel;

     m_barrel_a_map[bufferOffset] = strawnumber;

     //     cout << "Fill, Read BA, " << bufferOffset << " " << strawnumber << endl;
  }

  inputFile.close();
   }


  /*
   * Barrel C side sectors
   */
   {
  ATH_MSG_INFO( "defineTables: Barrel C " ); 

  dataFile = "TRT_ROD05_Barrel_C_Map.dat";
  file = PathResolver::find_file (dataFile, "DATAPATH");
  std::ifstream inputFile ( file.c_str() );

  if (!inputFile.is_open())
  {
     ATH_MSG_FATAL( "TRT_FillCablingData_DC3: Could not open file " << dataFile );
  }

  for ( int ii=0; ii<(120*16); ii++ )
    m_barrel_c_map[ii] = -1;

  while ( getline( inputFile, thisLine ) )
  {
     int RodChannel, DtmChannel, strawnumber, HWaddr, GolNumber;
     string Board;

     string::size_type loc = thisLine.find( "#" );
     if ( loc != string::npos )
     {
	thisLine.replace( loc, thisLine.length(), 1, ' ' );
      
	loc = thisLine.find_first_not_of( " \t" );
	if ( loc == string::npos )
	   continue;
     }

     istringstream parse( thisLine );

     parse >> RodChannel >> DtmChannel >> strawnumber >> HWaddr >> Board >> GolNumber;


     bufferOffset = (120 * DtmChannel) + RodChannel;

     m_barrel_c_map[bufferOffset] = strawnumber;
  }

  inputFile.close();
   }




  /**************************************************************/
  /*                       Endcap Tables                        */
  /**************************************************************/

   /*                   ***********                             */
   /*                   *   ECA   *                             */
   /*                   ***********                             */

  {
  /*
   * Left sectors
   */
  ATH_MSG_INFO( "defineTables: ECA Left " ); 

  dataFile = "TRT_ROD05_ECA_LEFT.dat";
  file = PathResolver::find_file (dataFile, "DATAPATH");
  std::ifstream inputFile( file.c_str() );

  if (!inputFile.is_open())
  {
     ATH_MSG_FATAL( "TRT_FillCablingData_DC3: Could not open file " << dataFile );
  }

  for ( int ii=0; ii<(16*120); ii++ )
  {
    m_eca_l0_map[ii] = -1;
    m_eca_l1_map[ii] = -1;
  }

  while ( getline( inputFile, thisLine ) )
  {
    int wheel, dtmaddr, dtmchannel, zlayer, phistraw, rodhalf, strawNumber;


    string::size_type loc = thisLine.find( "#" );
    if ( loc != string::npos )
    {
      thisLine.replace( loc, thisLine.length(), 1, ' ' );
      
      loc = thisLine.find_first_not_of( " \t" );
      if ( loc == string::npos )
	continue;
    }

    istringstream parse( thisLine );

    parse >> wheel >> dtmaddr >> dtmchannel >> zlayer >> phistraw 
	  >> rodhalf >> bufferOffset;

     /*
      * HACK!  We do not have a good straw numbering scheme for the endcap 
      * and we do not yet have enough info to produce a real offline
      * identifier.  So we make something up!
      */

     strawNumber = (zlayer & 0xff) << 8 | (phistraw & 0xff);

     if ( (rodhalf < 0) || (rodhalf > 1) )
     {
	ATH_MSG_WARNING( "Invalid rod half: " << rodhalf );
	exit( -1 );
     }

     if ( 0 == rodhalf )
       m_eca_l0_map[bufferOffset] = strawNumber;
     else if ( 1 == rodhalf )
       m_eca_l1_map[bufferOffset] = strawNumber;

     //     ATH_MSG_INFO( "Fill LEFT " << rodhalf << " " << bufferOffset << " " << strawNumber );
  }

  inputFile.close();
  }



  {
  /*
   * Right sectors
   */
  ATH_MSG_INFO( "defineTables: ECA Right " ); 

  dataFile = "TRT_ROD05_ECA_RIGHT.dat";
  file = PathResolver::find_file (dataFile, "DATAPATH");
  std::ifstream inputFile( file.c_str() );

  if (!inputFile.is_open())
  {
     ATH_MSG_FATAL( "TRT_FillCablingData_DC3: Could not open file " << dataFile );
  }

  for ( int ii=0; ii<(16*120); ii++ )
  {
    m_eca_r0_map[ii] = -1;
    m_eca_r1_map[ii] = -1;
  }

  while ( getline( inputFile, thisLine ) )
  {
    int wheel, dtmaddr, dtmchannel, zlayer, phistraw, rodhalf, strawNumber;


    string::size_type loc = thisLine.find( "#" );
    if ( loc != string::npos )
    {
      thisLine.replace( loc, thisLine.length(), 1, ' ' );
      
      loc = thisLine.find_first_not_of( " \t" );
      if ( loc == string::npos )
	continue;
    }

    istringstream parse( thisLine );

    parse >> wheel >> dtmaddr >> dtmchannel >> zlayer >> phistraw 
	  >> rodhalf >> bufferOffset;

     /*
      * HACK!  We do not have a good straw numbering scheme for the endcap 
      * and we do not yet have enough info to produce a real offline
      * identifier.  So we make something up!
      */

     strawNumber = (zlayer & 0xff) << 8 | (phistraw & 0xff);

     if ( 0 == rodhalf )
       m_eca_r0_map[bufferOffset] = strawNumber;
     else if ( 1 == rodhalf )
       m_eca_r1_map[bufferOffset] = strawNumber;

     //     ATH_MSG_INFO( "Fill RIGHT " << rodhalf << " " << bufferOffset << " " << strawNumber );
  }

  inputFile.close();
  }

   /*                   ***********                             */
   /*                   *   ECC   *                             */
   /*                   ***********                             */

  {
  /*
   * Left sectors
   */
  ATH_MSG_INFO( "defineTables: ECC Left " ); 

  dataFile = "TRT_ROD05_ECC_LEFT.dat";
  file = PathResolver::find_file (dataFile, "DATAPATH");
  std::ifstream inputFile( file.c_str() );

  if (!inputFile.is_open())
  {
     ATH_MSG_FATAL( "TRT_FillCablingData_DC3: Could not open file " << dataFile );
  }

  for ( int ii=0; ii<(16*120); ii++ )
  {
    m_ecc_l0_map[ii] = -1;
    m_ecc_l1_map[ii] = -1;
  }

  while ( getline( inputFile, thisLine ) )
  {
    int wheel, dtmaddr, dtmchannel, zlayer, phistraw, rodhalf, strawNumber;


    string::size_type loc = thisLine.find( "#" );
    if ( loc != string::npos )
    {
      thisLine.replace( loc, thisLine.length(), 1, ' ' );
      
      loc = thisLine.find_first_not_of( " \t" );
      if ( loc == string::npos )
	continue;
    }

    istringstream parse( thisLine );

    parse >> wheel >> dtmaddr >> dtmchannel >> zlayer >> phistraw 
	  >> rodhalf >> bufferOffset;

     /*
      * HACK!  We do not have a good straw numbering scheme for the endcap 
      * and we do not yet have enough info to produce a real offline
      * identifier.  So we make something up!
      */

     strawNumber = (zlayer & 0xff) << 8 | (phistraw & 0xff);

     if ( (rodhalf < 0) || (rodhalf > 1) )
     {
	ATH_MSG_WARNING( "Invalid rod half: " << rodhalf );
	exit( -1 );
     }

     if ( 0 == rodhalf )
       m_ecc_l0_map[bufferOffset] = strawNumber;
     else if ( 1 == rodhalf )
       m_ecc_l1_map[bufferOffset] = strawNumber;

     //     ATH_MSG_INFO( "Fill LEFT " << rodhalf << " " << bufferOffset << " " << strawNumber );
  }

  inputFile.close();
  }



  {
  /*
   * Right sectors
   */
  ATH_MSG_INFO( "defineTables: ECC Right " ); 

  dataFile = "TRT_ROD05_ECC_RIGHT.dat";
  file = PathResolver::find_file (dataFile, "DATAPATH");
  std::ifstream inputFile( file.c_str() );

  if (!inputFile.is_open())
  {
     ATH_MSG_FATAL( "TRT_FillCablingData_DC3: Could not open file " << dataFile );
  }

  for ( int ii=0; ii<(16*120); ii++ )
  {
    m_ecc_r0_map[ii] = -1;
    m_ecc_r1_map[ii] = -1;
  }

  while ( getline( inputFile, thisLine ) )
  {
    int wheel, dtmaddr, dtmchannel, zlayer, phistraw, rodhalf, strawNumber;


    string::size_type loc = thisLine.find( "#" );
    if ( loc != string::npos )
    {
      thisLine.replace( loc, thisLine.length(), 1, ' ' );
      
      loc = thisLine.find_first_not_of( " \t" );
      if ( loc == string::npos )
	continue;
    }

    istringstream parse( thisLine );

    parse >> wheel >> dtmaddr >> dtmchannel >> zlayer >> phistraw 
	  >> rodhalf >> bufferOffset;

     /*
      * HACK!  We do not have a good straw numbering scheme for the endcap 
      * and we do not yet have enough info to produce a real offline
      * identifier.  So we make something up!
      */

     strawNumber = (zlayer & 0xff) << 8 | (phistraw & 0xff);

     if ( 0 == rodhalf )
       m_ecc_r0_map[bufferOffset] = strawNumber;
     else if ( 1 == rodhalf )
       m_ecc_r1_map[bufferOffset] = strawNumber;

     //     ATH_MSG_INFO( "Fill RIGHT " << rodhalf << " " << bufferOffset << " " << strawNumber );
  }

  inputFile.close();
  }




  /**************************************************************/
  /*               Global Cabling Specification                 */
  /**************************************************************/

  ATH_MSG_INFO( "defineTables: Global Spec " ); 

  if ( string(m_inputFilename.value()) != "" )
  {
     ATH_MSG_INFO( "User defined SourceID list: " << m_inputFilename.value() );

     m_PartialDetector = true;

     dataFile = string(m_inputFilename.value());
     //     dataFile = "TRT_Global_Cabling.dat";

     file = PathResolver::find_file (dataFile, "DATAPATH");

     std::ifstream globalInputFile ( file.c_str() );

     if (!globalInputFile.is_open())
     {
	ATH_MSG_FATAL( "TRT_FillCablingData_DC3: Could not open file " << dataFile );
     }


     while ( !globalInputFile.eof() && !globalInputFile.fail() )
     {
	std::getline( globalInputFile, thisLine );

	if ( !thisLine.empty() )
        {
	   uint32_t SourceId;

	   string::size_type loc = thisLine.find( "#" );
	   if ( loc != string::npos )
           {
	      thisLine.replace( loc, thisLine.length(), 1, ' ' );
      
	      loc = thisLine.find_first_not_of( " \t" );
	      if ( loc == string::npos )
		 continue;
	   }

	   istringstream parse( thisLine );

	   parse >> std::hex >> SourceId;

	   ATH_MSG_INFO( "Including Source ID: " << hex << SourceId << dec );

	   m_validSourceIds.insert( SourceId );
	}
     }

     globalInputFile.close();

  }
  //  else
  //  {
  //     ATH_MSG_INFO( "Default (complete) SourceID list" );
  //  }


  Identifier NULLstrawID(0);
  IdentifierHash NULLhashId(0);

  int i_barrel;
  int SubDet;
  int SourceBase;

  for ( i_barrel=0; i_barrel<2; i_barrel++ )   // Loop over A,C sides of barrel
  {
     std::map<int, int> con_map;

     if ( 0 == i_barrel )
     {
	con_map = m_barrel_a_map;
	SubDet = 1;
	SourceBase = 0x310000;
     }
     else
     {
	con_map = m_barrel_c_map;
	SubDet = -1;
	SourceBase = 0x320000;
     }

     int Phi;
     for ( Phi=0; Phi<32; Phi++ )
     {
       int rodId = SourceBase | ((Phi+1) << 8);
       int ii;
       int strawNumber;

       for ( ii=0; ii<(120*16); ii++ )
       {
	 strawNumber = con_map[ii];

	 if ( strawNumber < 0 )
	 {
	    m_cabling->set_identfierForAllStraws( rodId, ii, NULLstrawID );
	    m_cabling->set_identfierHashForAllStraws( rodId, ii, NULLhashId );
	    continue;
	 }

	 /*
	  * Calculate strawLayerId
	  */
	 strawLayerId = 0;
	 while ( (m_numberOfStrawsInLayers[strawLayerId] <= strawNumber) &&
		 (m_numberOfStrawsInLayers[strawLayerId] != -1) )
	 {
	   strawLayerId++;
	 }

	 if ( m_numberOfStrawsInLayers[strawLayerId] == -1 )
	 {
	   ATH_MSG_WARNING( "Invalid Straw in table (" << Phi << ", " << ii << "): " << strawNumber ); 
	   continue;
	 }

	 strawInLayerId = m_numberOfStrawsInLayers[strawLayerId] - strawNumber - 1;


	 moduleId = -1;
	 if ( strawLayerId < m_numberOfLayers[0] )
	   moduleId = 0;
	 else if ( strawLayerId < (m_numberOfLayers[0] + m_numberOfLayers[1]) )
	 {
	   moduleId = 1;
	   strawLayerId = strawLayerId - m_numberOfLayers[0];
	 }
	 else if ( strawLayerId < 
		   (m_numberOfLayers[0] + m_numberOfLayers[1] + m_numberOfLayers[2]) )
	 {
	   moduleId = 2;
	   strawLayerId = strawLayerId - (m_numberOfLayers[0] + m_numberOfLayers[1]);
	 }
	 else
	   ATH_MSG_ERROR( "Bad calculated module!" );

	   
	 strawID = m_TRTHelper->straw_id( SubDet, Phi, moduleId, strawLayerId, strawInLayerId );


// 	 cout << "Fill -> " << hex << rodId << " " << dec << ii << " " 
// 	      << strawNumber << " " 
// 	      << SubDet << " " << Phi << " " << moduleId << " " 
// 	      << strawLayerId << " " << strawInLayerId << " " << std::hex 
// 	      << strawID << std::dec << endl;

	 m_cabling->set_identfierForAllStraws( rodId, ii, strawID );


	 Identifier Lid = m_TRTHelper->layer_id( SubDet, Phi, moduleId, strawLayerId );

	 int result = m_TRTHelper->get_hash( Lid, hashId, &m_cntx );
	 if ( result != 0 )
	 {
	   ATH_MSG_WARNING( ">>>>>>>>>>> Get Hash FAILED! <<<<<<<<<<" );
	   ATH_MSG_WARNING( \
		    " array    = " << m_numberOfStrawsInLayers[strawLayerId] \
		 << " SubDet    = " << SubDet \
		 << " Phi      = " << Phi \
		 << " Module   = " << moduleId \
		 << " Layer    = " << strawLayerId \
		 << " StrInLay = " << strawInLayerId );
	 }  


	 m_cabling->set_identfierHashForAllStraws( rodId, ii, hashId );

       }

     }  // Loop over phi
  }     // Loop over barrel A,C


  int i_ec;
  int Phi;

  for ( i_ec=0; i_ec<2; i_ec++ )   // Loop over A,C sides of EndCap
  {

     if ( 0 == i_ec )     // A side
     {
       //con_map = m_barrel_a_map;
	SubDet = 2;
	SourceBase = 0x330000;
     }
     else                // C side
     {
       //con_map = m_barrel_c_map;
	SubDet = -2;
	SourceBase = 0x340000;
     }

     for ( Phi=0; Phi<32; Phi++ )
     {
       int j;
       for ( j=0; j<2; j++ )              // Inner vs outer Z RODs
       {
	 int my_Phi;
	 std::map< int, int > con_map;

	 if ( 2 == SubDet )                // A side
	 {
	    //	   my_Phi = (32 + 15 - Phi) % 32;
	    my_Phi = Phi;

	    if ( m_broken_FDR_Cabling )
	    {
	       if ( 0 == j )
	       {
		  if ( my_Phi < 8 )
		     con_map = m_ecc_r0_map;
		  else if ( my_Phi < 16 )
		     con_map = m_ecc_l0_map;
		  else if ( my_Phi < 24 )
		     con_map = m_ecc_r0_map;
		  else if ( my_Phi < 32 )
		     con_map = m_ecc_l0_map;
	       }
	       else if ( 1 == j )
	       {
		  if ( my_Phi < 8 )
		     con_map = m_ecc_r1_map;
		  else if ( my_Phi < 16 )
		     con_map = m_ecc_l1_map;
		  else if ( my_Phi < 24 )
		     con_map = m_ecc_r1_map;
		  else if ( my_Phi < 32 )
		     con_map = m_ecc_l1_map;

	       }
	    }
	    else                     // Correct ECA cabling
	    {

	       if ( 0 == j )
	       {
		  if ( my_Phi < 8 )
		     con_map = m_eca_r0_map;
		  else if ( my_Phi < 16 )
		     con_map = m_eca_l0_map;
		  else if ( my_Phi < 24 )
		     con_map = m_eca_r0_map;
		  else if ( my_Phi < 32 )
		     con_map = m_eca_l0_map;
	       }
	       else if ( 1 == j )
	       {
		  if ( my_Phi < 8 )
		     con_map = m_eca_r1_map;
		  else if ( my_Phi < 16 )
		     con_map = m_eca_l1_map;
		  else if ( my_Phi < 24 )
		     con_map = m_eca_r1_map;
		  else if ( my_Phi < 32 )
		     con_map = m_eca_l1_map;
	       }

	   }
	 }
	 else                             // C side
	 {
	     my_Phi = Phi;

	     if ( 0 == j )
	     {
	       if ( my_Phi < 8 )
		 con_map = m_ecc_l0_map;
	       else if ( my_Phi < 16 )
		 con_map = m_ecc_r0_map;
	       else if ( my_Phi < 24 )
		 con_map = m_ecc_l0_map;
	       else if ( my_Phi < 32 )
		 con_map = m_ecc_r0_map;
	     }
	     else if ( 1 == j )
	     {
	       if ( my_Phi < 8 )
		 con_map = m_ecc_l1_map;
	       else if ( my_Phi < 16 )
		 con_map = m_ecc_r1_map;
	       else if ( my_Phi < 24 )
		 con_map = m_ecc_l1_map;
	       else if ( my_Phi < 32 )
		 con_map = m_ecc_r1_map;
	     }
	 }

	 int rodId = SourceBase | ((my_Phi+1) << 8) | (j+1);

	 int i;
	 for ( i=0; i<(120*16); i++ )      // loop over buffer offset
	 {

	   int BufferLocation = i;
	   int StrawNumber = con_map[i];

	   strawLayerId = (StrawNumber >> 8) & 0xff;
	   strawInLayerId = StrawNumber & 0xff;

	   if ( m_broken_FDR_Cabling && 2 == SubDet )           // A side
	   {
	      strawInLayerId = 23 - strawInLayerId;
	   }


	   if ( (strawInLayerId < 0) || (strawInLayerId > 23 ) )
	   {
	     ATH_MSG_WARNING( i << " " << j << " " << my_Phi << " " << hex << StrawNumber );

	     exit( -1 );
	   }


	    m_StrawLayerToRod[strawLayerId] = j+1;


	    moduleId = strawLayerId / 8;
	    if ( moduleId < 12 )           // A wheel?
	    {
	      moduleId = moduleId / 2;
	      strawLayerId = strawLayerId % 16;

	      if ( (-2 == SubDet) && ! m_broken_FDR_Cabling )          // ECC
	      {
		/* Flip local Z */
		if ( strawLayerId < 8 )             
		  strawLayerId = 7 - strawLayerId;
		else
		  strawLayerId = 15 - (strawLayerId - 8);

		/* Flip local Phi */
		strawInLayerId = 23 - strawInLayerId;
	      }
	    }
	    else                           // B wheel
	    {
	      moduleId = moduleId - 6;
	      strawLayerId = strawLayerId % 8;
	      if ( (-2 == SubDet) && ! m_broken_FDR_Cabling )          // ECC
	      {
		strawLayerId = 7 - strawLayerId;	 // Flip local Z
		strawInLayerId = 23 - strawInLayerId;    // Flip local Phi
	      }
	    }



	    strawID = m_TRTHelper->straw_id( SubDet, my_Phi, 
					     moduleId, strawLayerId,
					     strawInLayerId );


	    m_cabling->set_identfierForAllStraws( rodId, BufferLocation,
						  strawID );


	    //	   IdLayer = m_TRTHelper->layer_id( strawID );
	    
	    //	   int result = m_TRTHelper->get_hash( strawID, hashId, &m_cntx );

	    Identifier Lid = m_TRTHelper->layer_id( SubDet,
						    my_Phi, moduleId,
						    strawLayerId );

	    int result = m_TRTHelper->get_hash( Lid, hashId, &m_cntx );
	    if ( result != 0 )
	    {
	      ATH_MSG_WARNING( ">>>>>>>>>>> Get Hash FAILED! <<<<<<<<<<" );

              ATH_MSG_WARNING ( \
                       " SubDet    = " << SubDet \
                    << " Phi      = "  << my_Phi \
                    << " Module   = "  << moduleId \
                    << " Layer    = "  << strawLayerId \
                    << " StrInLay = "  << strawInLayerId );
	    }


	    m_cabling->set_identfierHashForAllStraws( rodId,
						      BufferLocation,
						      hashId );
	 } // loop over bufferOffsets
       }   // loop over inner, outer Z RODs
     }   // Endcap
  }      // loop over GCM


  ATH_MSG_INFO( "Out of defineTables" ); 

  return;
}

/*
 * Define map of collection IDs for all ROBs
 */
void TRT_FillCablingData_DC3::defineCollID()
{
   int Phi;
   int i_barrel;
   int i_ec;
   int SourceBase;

   ATH_MSG_INFO( "In defineCollID()" );



   for ( i_barrel=0; i_barrel<2; i_barrel++ )   // Loop over A,C sides of barrel
   {
      if ( 0 == i_barrel )
      {
	 SourceBase = 0x310000;
      }
      else
      {
	 SourceBase = 0x320000;
      }

      for ( Phi=0; Phi<32; Phi++ )
      {
	int rodId = SourceBase | ((Phi+1) << 8);

	m_cabling->add_allRobs( rodId );

	 std::vector<IdentifierHash> * vectID = new std::vector<IdentifierHash>();
	 fillCollID( rodId, *vectID);

	 ATH_MSG_DEBUG( "Adding Collid for ROB " << std::hex << rodId << std::dec );

	 m_cabling->add_collID( rodId, vectID);
      }
   }


   for ( i_ec=0; i_ec<2; i_ec++ )   // Loop over A,C sides of barrel
   {
      if ( 0 == i_ec )
      {
	 SourceBase = 0x330000;
      }
      else
      {
	 SourceBase = 0x340000;
      }

      for ( Phi=0; Phi<32; Phi++ )
      {
	 int z;
	 for ( z=1; z<3; z++ )
	 {
	    int rodId = SourceBase | ((Phi+1) << 8) | z;

	    m_cabling->add_allRobs( rodId );

	    std::vector<IdentifierHash> * vectID = new std::vector<IdentifierHash>();
	    fillCollID( rodId, *vectID);

	    ATH_MSG_DEBUG( "Adding Collid for ROB " << std::hex << rodId << std::dec );

	    m_cabling->add_collID( rodId, vectID);
	 }
      }
   }



   return;
}


/*
 * Input: ROB Source ID
 * Output: List of Hash IDs, one for each of 73 layers
 */
void TRT_FillCablingData_DC3::fillCollID(uint32_t rob_id,  
std::vector<IdentifierHash> & ids)
{
   int made_collections = 0;
   IdentifierHash idHash;
  
   int phi;
   int barrelId;

   phi = ((rob_id >> 8) & 0xff) - 1;

   if ( (rob_id & 0xff0000) == 0x310000 )
   {
      barrelId = 1;
   }
   else if ( (rob_id & 0xff0000) == 0x320000 )
   {
      barrelId = -1;
   }
   else if ( (rob_id & 0xff0000) == 0x330000 )
   {
      barrelId = 2;
   }
   else if ( (rob_id & 0xff0000) == 0x340000 )
   {
      barrelId = -2;
   }
   else
   {
      ATH_MSG_FATAL( "fillCollID: Bad rob_id " <<
	 std::hex << rob_id << std::dec );
      return;
   }


   if ( abs( barrelId ) == 1 )   // barrel?
   {
      for (int ring = 0; ring < m_numberOfRings; ++ring)
      {
	 for (int layer = 0; layer < m_numberOfLayers[ring]; ++layer) 
	 {
	    Identifier id = m_TRTHelper->layer_id(barrelId, phi, ring, layer);

	    if (!m_TRTHelper->get_hash(id, idHash, &m_cntx)) 
	    {
	       ids.push_back(idHash);
	       made_collections++;
	    }
	    else
	       ATH_MSG_FATAL( " Unable to get hash for id "
		     << m_TRTHelper->print_to_string(id) << " -- " << std::hex << rob_id << std::dec << 
		 " " << phi << " " << ring << " " << layer );
	 } // Loop over layers
      }    // Loop over module types
   }       // Loop over barrel A,C
   else   // End cap
   {
       int Z = rob_id & 0xf;


       int Layer;
       int wheel;
       int LayerInWheel;

       for ( Layer=0; Layer<160; Layer++ )          // 160 Layers in the EC
       {
	 if ( m_StrawLayerToRod[Layer] != Z )
	   continue;

	 
	 wheel = Layer / 8;
	 if ( wheel < 12 )           // A wheel?
	 {
	   wheel = wheel / 2;
	   LayerInWheel = Layer % 16;
	 }
	 else                           // B wheel
	 {
	   wheel = wheel - 6;
	   LayerInWheel = Layer % 8;
	 }

	 Identifier id = m_TRTHelper->layer_id( barrelId, phi, wheel, LayerInWheel );

	 if (!m_TRTHelper->get_hash(id, idHash, &m_cntx)) 
	 {
	   ids.push_back(idHash);
	   made_collections++;
	 }
	 else
	   ATH_MSG_FATAL( " EC: Unable to get hash for id "
		 << m_TRTHelper->show_to_string(id) << " " << barrelId << " " << phi << " "
		 << wheel << " " << LayerInWheel );
       }  // loop over layer


   }      // endcap

   return;
}



  // Fill Tables with IDs for all straws
void TRT_FillCablingData_DC3::defineTables_broken_FDR()
{
   ATH_MSG_INFO( "In defineTables_broken_FDR" ); 

   int moduleId, strawLayerId, strawInLayerId;
   Identifier strawID;
   IdentifierHash hashId;

   uint32_t bufferOffset;
   string thisLine;

   std::string dataFile;
   std::string file;



  /**************************************************************/
  /*                       Barrel Tables                        */
  /**************************************************************/

  /*
   * Barrel A side sectors
   */
   {
   ATH_MSG_INFO( "defineTables: Barrel A " ); 

   dataFile = "TRT_ROD05_Barrel_A_Map.dat";
  file = PathResolver::find_file (dataFile, "DATAPATH");
  std::ifstream inputFile ( file.c_str() );

  if (!inputFile.is_open())
  {
     ATH_MSG_FATAL( "TRT_FillCablingData_DC3: Could not open file " << dataFile );
  }

  while ( getline( inputFile, thisLine ) )
  {
     int RodChannel, DtmChannel, strawnumber, HWaddr, GolNumber;
     string Board;

     string::size_type loc = thisLine.find( "#" );
     if ( loc != string::npos )
     {
	thisLine.replace( loc, thisLine.length(), 1, ' ' );
      
	loc = thisLine.find_first_not_of( " \t" );
	if ( loc == string::npos )
	   continue;
     }

     istringstream parse( thisLine );

     parse >> RodChannel >> DtmChannel >> strawnumber >> HWaddr >> Board >> GolNumber;


     bufferOffset = (120 * DtmChannel) + RodChannel;

     m_barrel_a_map[bufferOffset] = strawnumber;

     //     cout << "Fill, Read BA, " << bufferOffset << " " << strawnumber << endl;
  }

  inputFile.close();
   }


  /*
   * Barrel C side sectors
   */
   {
  ATH_MSG_INFO( "defineTables: Barrel C " ); 

  dataFile = "TRT_ROD05_Barrel_C_Map.dat";
  file = PathResolver::find_file (dataFile, "DATAPATH");
  std::ifstream inputFile ( file.c_str() );

  if (!inputFile.is_open())
  {
     ATH_MSG_FATAL( "TRT_FillCablingData_DC3: Could not open file " << dataFile );
  }

  while ( getline( inputFile, thisLine ) )
  {
     int RodChannel, DtmChannel, strawnumber, HWaddr, GolNumber;
     string Board;

     string::size_type loc = thisLine.find( "#" );
     if ( loc != string::npos )
     {
	thisLine.replace( loc, thisLine.length(), 1, ' ' );
      
	loc = thisLine.find_first_not_of( " \t" );
	if ( loc == string::npos )
	   continue;
     }

     istringstream parse( thisLine );

     parse >> RodChannel >> DtmChannel >> strawnumber >> HWaddr >> Board >> GolNumber;


     bufferOffset = (120 * DtmChannel) + RodChannel;

     m_barrel_c_map[bufferOffset] = strawnumber;
  }

  inputFile.close();
   }




  /**************************************************************/
  /*                       Endcap Tables                        */
  /**************************************************************/

#ifdef ENDCAP_TABLES_DEFINED

  /*
   * Left sectors, layers 0-79
   */
  ATH_MSG_INFO( "defineTables: EC Left low |Z| " ); 

  dataFile = "TRT_ROD05_EC_LEFT_LOWZ.dat";
  file = PathResolver::find_file (dataFile, "DATAPATH");
  inputFile.open( file.c_str() );

  if (!inputFile.is_open())
  {
     ATH_MSG_FATAL( "TRT_FillCablingData_DC3: Could not open file " << dataFile );
  }

  while ( getline( inputFile, thisLine ) )
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

     /*
      * HACK!  We do not have a good straw numbering scheme for the endcap 
      * and we do not yet have enough info to produce a real offline
      * identifier.  So we make something up!
      */

     strawNumber = (strawlayer & 0xff) << 8 | (strawnumber & 0xff);

     bufferOffset = (120 * DtmChannel) + RodChannel;

     m_ec_l1_map[bufferOffset] = strawNumber;
  }

  inputFile.close();



  /*
   * Left sectors, layers 80-159
   */
  ATH_MSG_INFO( "defineTables: EC Left high |Z| " ); 

  dataFile = "TRT_ROD05_EC_LEFT_HIGHZ.dat";
  file = PathResolver::find_file (dataFile, "DATAPATH");
  inputFile.open( file.c_str() );

  if (!inputFile.is_open())
  {
     ATH_MSG_FATAL( "TRT_FillCablingData_DC3: Could not open file " << dataFile );
  }

  while ( getline( inputFile, thisLine ) )
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

     /*
      * HACK!  We do not have a good straw numbering scheme for the endcap 
      * and we do not yet have enough info to produce a real offline
      * identifier.  So we make something up!
      */

     strawNumber = (strawlayer & 0xff) << 8 | (strawnumber & 0xff);

     bufferOffset = (120 * DtmChannel) + RodChannel;

     m_ec_l2_map[bufferOffset] = strawNumber;
  }

  inputFile.close();


  /*
   * Right sectors, layers 0-79
   */
  ATH_MSG_INFO( "defineTables: EC Right low |Z| " ); 

  dataFile = "TRT_ROD05_EC_Right_LOWZ.dat";
  file = PathResolver::find_file (dataFile, "DATAPATH");
  inputFile.open( file.c_str() );

  if (!inputFile.is_open())
  {
     ATH_MSG_FATAL( "TRT_FillCablingData_DC3: Could not open file " << dataFile );
  }

  while ( getline( inputFile, thisLine ) )
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

     /*
      * HACK!  We do not have a good straw numbering scheme for the endcap 
      * and we do not yet have enough info to produce a real offline
      * identifier.  So we make something up!
      */

     strawNumber = (strawlayer & 0xff) << 8 | (strawnumber & 0xff);

     bufferOffset = (120 * DtmChannel) + RodChannel;

     m_ec_r1_map[bufferOffset] = strawNumber;
  }

  inputFile.close();



  /*
   * Right sectors, layers 80-159
   */
  ATH_MSG_INFO( "defineTables: EC Right high |Z| " ); 

  dataFile = "TRT_ROD05_EC_RIGHT_HIGHZ.dat";
  file = PathResolver::find_file (dataFile, "DATAPATH");
  inputFile.open( file.c_str() );

  if (!inputFile.is_open())
  {
     ATH_MSG_FATAL( "TRT_FillCablingData_DC3: Could not open file " << dataFile );
  }

  while ( getline( inputFile, thisLine ) )
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

     /*
      * HACK!  We do not have a good straw numbering scheme for the endcap 
      * and we do not yet have enough info to produce a real offline
      * identifier.  So we make something up!
      */

     strawNumber = (strawlayer & 0xff) << 8 | (strawnumber & 0xff);

     bufferOffset = (120 * DtmChannel) + RodChannel;

     m_ec_r2_map[bufferOffset] = strawNumber;
  }

  inputFile.close();

#endif /* ENDCAP_TABLES_DEFINED */


  /**************************************************************/
  /*               Global Cabling Specification                 */
  /**************************************************************/

  ATH_MSG_INFO( "defineTables: Global Spec " ); 

  if ( string(m_inputFilename.value()) != "" )
  {
     ATH_MSG_INFO( "User defined SourceID list: " << m_inputFilename.value() );

     m_PartialDetector = true;

     dataFile = string(m_inputFilename.value());

     file = PathResolver::find_file (dataFile, "DATAPATH");

     std::ifstream globalInputFile ( file.c_str() );

     if (!globalInputFile.is_open())
     {
	ATH_MSG_FATAL( "TRT_FillCablingData_DC3: Could not open file " << dataFile );
     }


     while ( !globalInputFile.eof() && !globalInputFile.fail() )
     {
	std::getline( globalInputFile, thisLine );

	if ( !thisLine.empty() )
        {
	   uint32_t SourceId;

	   string::size_type loc = thisLine.find( "#" );
	   if ( loc != string::npos )
           {
	      thisLine.replace( loc, thisLine.length(), 1, ' ' );
      
	      loc = thisLine.find_first_not_of( " \t" );
	      if ( loc == string::npos )
		 continue;
	   }

	   istringstream parse( thisLine );

	   parse >> std::hex >> SourceId;

	   ATH_MSG_INFO( "Including Source ID: " << hex << SourceId << dec );

	   m_validSourceIds.insert( SourceId );
	}
     }

     globalInputFile.close();

  }
  else
  {
     ATH_MSG_INFO( "Default (complete) SourceID list" );
  }


  Identifier NULLstrawID(0);
  IdentifierHash NULLhashId(0);

  int i_barrel;
  int SubDet;
  int SourceBase;

  for ( i_barrel=0; i_barrel<2; i_barrel++ )   // Loop over A,C sides of barrel
  {
     std::map<int, int> con_map;

     if ( 0 == i_barrel )
     {
	con_map = m_barrel_a_map;
	SubDet = 1;
	SourceBase = 0x310000;
     }
     else
     {
	con_map = m_barrel_c_map;
	SubDet = -1;
	SourceBase = 0x320000;
     }

     int Phi;
     for ( Phi=0; Phi<32; Phi++ )
     {
       int rodId = SourceBase | ((Phi+1) << 8);
       int ii;
       int strawNumber;

       for ( ii=0; ii<(120*16); ii++ )
       {
	 strawNumber = con_map[ii];

	 if ( strawNumber < 0 )
	 {
	    m_cabling->set_identfierForAllStraws( rodId, ii, NULLstrawID );
	    m_cabling->set_identfierHashForAllStraws( rodId, ii, NULLhashId );
	    continue;
	 }

	 /*
	  * Calculate strawLayerId
	  */
	 strawLayerId = 0;
	 while ( (m_numberOfStrawsInLayers[strawLayerId] <= strawNumber) &&
		 (m_numberOfStrawsInLayers[strawLayerId] != -1) )
	 {
	   strawLayerId++;
	 }

	 if ( m_numberOfStrawsInLayers[strawLayerId] == -1 )
	 {
	   ATH_MSG_WARNING( "Invalid Straw in table: " << strawNumber ); 
	   continue;
	 }

	 strawInLayerId = m_numberOfStrawsInLayers[strawLayerId] - strawNumber - 1;


	 moduleId = -1;
	 if ( strawLayerId < m_numberOfLayers[0] )
	   moduleId = 0;
	 else if ( strawLayerId < (m_numberOfLayers[0] + m_numberOfLayers[1]) )
	 {
	   moduleId = 1;
	   strawLayerId = strawLayerId - m_numberOfLayers[0];
	 }
	 else if ( strawLayerId < 
		   (m_numberOfLayers[0] + m_numberOfLayers[1] + m_numberOfLayers[2]) )
	 {
	   moduleId = 2;
	   strawLayerId = strawLayerId - (m_numberOfLayers[0] + m_numberOfLayers[1]);
	 }
	 else
	   ATH_MSG_ERROR( "Bad calculated module!" );

	   
	 strawID = m_TRTHelper->straw_id( SubDet, Phi, moduleId, strawLayerId, strawInLayerId );


// 	 cout << "Fill -> " << hex << rodId << " " << dec << ii << " " 
// 	      << strawNumber << " " 
// 	      << SubDet << " " << Phi << " " << moduleId << " " 
// 	      << strawLayerId << " " << strawInLayerId << " " << std::hex 
// 	      << strawID << std::dec << endl;

	 m_cabling->set_identfierForAllStraws( rodId, ii, strawID );


	 Identifier Lid = m_TRTHelper->layer_id( SubDet, Phi, moduleId, strawLayerId );

	 int result = m_TRTHelper->get_hash( Lid, hashId, &m_cntx );
	 if ( result != 0 )
	 {
	   ATH_MSG_WARNING( ">>>>>>>>>>> Get Hash FAILED! <<<<<<<<<<" );
	   ATH_MSG_WARNING( \
		    " array    = " << m_numberOfStrawsInLayers[strawLayerId] \
		 << " SubDet    = " << SubDet \
		 << " Phi      = " << Phi \
		 << " Module   = " << moduleId \
		 << " Layer    = " << strawLayerId \
		 << " StrInLay = " << strawInLayerId );
	 }  


	 m_cabling->set_identfierHashForAllStraws( rodId, ii, hashId );

       }

     }  // Loop over phi
  }     // Loop over barrel A,C


  int i_ec;
  int Phi;

  for ( i_ec=0; i_ec<2; i_ec++ )   // Loop over A,C sides of EndCap
  {


     /*
      *  XXX  XXX  XXX  XXX  XXX  XXX  XXX  XXX
      *
      * This is a complete hack to get *some* mapping for the EndCap.  It will be
      * replaced by the real mapping once it is figured out.
      *
      *  XXX  XXX  XXX  XXX  XXX  XXX  XXX  XXX
      */
     if ( 0 == i_ec )     // A side
     {
       //con_map = m_barrel_a_map;
	SubDet = 2;
	SourceBase = 0x330000;
     }
     else                // C side
     {
       //con_map = m_barrel_c_map;
	SubDet = -2;
	SourceBase = 0x340000;
     }

     for ( Phi=0; Phi<32; Phi++ )
     {
       int j;
       for ( j=0; j<2; j++ )              // Inner vs outer Z RODs
       {

	 int rodId = SourceBase | ((Phi+1) << 8) | (j+1);

	 int i;
	 for ( i=0; i<(120*16); i++ )      // loop over buffer offset
	 {

	   int BufferLocation = i;
	   int Straw = i + (120*16) * j;        // XXX - Hack.  This is FAKE FAKE FAKE!

	   strawLayerId = Straw / 24;      // 24 straws per layer
	   strawInLayerId = Straw % 24;

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


	    strawID = m_TRTHelper->straw_id( SubDet, Phi, 
					     moduleId, strawLayerId, 
					     strawInLayerId );


	    m_cabling->set_identfierForAllStraws( rodId, BufferLocation,
						  strawID );


	    //	   IdLayer = m_TRTHelper->layer_id( strawID );
	    
	    //	   int result = m_TRTHelper->get_hash( strawID, hashId, &m_cntx );

	    Identifier Lid = m_TRTHelper->layer_id( SubDet,
						    Phi, moduleId,
						    strawLayerId );

	    int result = m_TRTHelper->get_hash( Lid, hashId, &m_cntx );
	    if ( result != 0 )
	    {
	      ATH_MSG_WARNING( ">>>>>>>>>>> Get Hash FAILED! <<<<<<<<<<" );

              ATH_MSG_WARNING( "Straw     = " << Straw \
                    << " SubDet    = " << SubDet \
                    << " Phi      = "  << Phi \
                    << " Module   = "  << moduleId \
                    << " Layer    = "  << strawLayerId \
                    << " StrInLay = "  << strawInLayerId );
	    }


	    m_cabling->set_identfierHashForAllStraws( rodId,
						      BufferLocation,
						      hashId );
	 } // loop over bufferOffsets
       }   // loop over inner, outer Z RODs
     }   // Endcap
  }      // loop over GCM


  ATH_MSG_INFO( "Out of defineTables_broken_FDR" ); 

  return;
}

/*
 * Define map of collection IDs for all ROBs
 */
void TRT_FillCablingData_DC3::defineCollID_broken_FDR()
{
   int Phi;
   int i_barrel;
   int i_ec;
   int SourceBase;

   ATH_MSG_INFO( "In defineCollID_broken_FDR()" );



   for ( i_barrel=0; i_barrel<2; i_barrel++ )   // Loop over A,C sides of barrel
   {
      if ( 0 == i_barrel )
      {
	 SourceBase = 0x310000;
      }
      else
      {
	 SourceBase = 0x320000;
      }

      for ( Phi=0; Phi<32; Phi++ )
      {
	int rodId = SourceBase | ((Phi+1) << 8);

	m_cabling->add_allRobs( rodId );

	 std::vector<IdentifierHash> * vectID = new std::vector<IdentifierHash>();
	 fillCollID_broken_FDR( rodId, *vectID);

	 ATH_MSG_DEBUG( "Adding Collid for ROB " << std::hex << rodId << std::dec );

	 m_cabling->add_collID( rodId, vectID);
      }
   }


   for ( i_ec=0; i_ec<2; i_ec++ )   // Loop over A,C sides of barrel
   {
      if ( 0 == i_ec )
      {
	 SourceBase = 0x330000;
      }
      else
      {
	 SourceBase = 0x340000;
      }

      for ( Phi=0; Phi<32; Phi++ )
      {
	 int z;
	 for ( z=1; z<3; z++ )
	 {
	    int rodId = SourceBase | ((Phi+1) << 8) | z;

	    m_cabling->add_allRobs( rodId );

	    std::vector<IdentifierHash> * vectID = new std::vector<IdentifierHash>();
	    fillCollID_broken_FDR( rodId, *vectID);

	    ATH_MSG_DEBUG( "Adding Collid for ROB " << std::hex << rodId << std::dec );

	    m_cabling->add_collID( rodId, vectID);
	 }
      }
   }



   return;
}


/*
 * Input: ROB Source ID
 * Output: List of Hash IDs, one for each of 73 layers
 */
void TRT_FillCablingData_DC3::fillCollID_broken_FDR(uint32_t rob_id,  
std::vector<IdentifierHash> & ids)
{
   int made_collections = 0;
   IdentifierHash idHash;
  
   int phi;
   int barrelId;

   phi = ((rob_id >> 8) & 0xff) - 1;

   if ( (rob_id & 0xff0000) == 0x310000 )
   {
      barrelId = 1;
   }
   else if ( (rob_id & 0xff0000) == 0x320000 )
   {
      barrelId = -1;
   }
   else if ( (rob_id & 0xff0000) == 0x330000 )
   {
      barrelId = 2;
   }
   else if ( (rob_id & 0xff0000) == 0x340000 )
   {
      barrelId = -2;
   }
   else
   {
      ATH_MSG_FATAL( "fillCollID_broken_FDR: Bad rob_id " << std::hex << rob_id << std::dec );
      return;
   }


   if ( abs( barrelId ) == 1 )   // barrel?
   {
      for (int ring = 0; ring < m_numberOfRings; ++ring)
      {
	 for (int layer = 0; layer < m_numberOfLayers[ring]; ++layer) 
	 {
	    Identifier id = m_TRTHelper->layer_id(barrelId, phi, ring, layer);

	    if (!m_TRTHelper->get_hash(id, idHash, &m_cntx)) 
	    {
	       ids.push_back(idHash);
	       made_collections++;
	    }
	    else
	       ATH_MSG_FATAL( " Unable to get hash for id " \
		     << m_TRTHelper->print_to_string(id) << " -- " << std::hex << rob_id << std::dec << \
		 " " << phi << " " << ring << " " << layer );
	 } // Loop over layers
      }    // Loop over module types
   }       // Loop over barrel A,C
   else   // End cap
   {
       int StartWheelA;
       int EndWheelA;
       int StartLayerA;
       int EndLayerA;

       int StartWheelB;
       int EndWheelB;
       int StartLayerB;
       int EndLayerB;

       int Z = rob_id & 0xf;

       if ( 1 == Z )
       {
	  StartWheelA = 0;
	  EndWheelA   = 4;
	  StartLayerA = 0;
	  EndLayerA   = 15;

	  StartWheelB = 0;
	  EndWheelB   = 0;
	  StartLayerB = 0;
	  EndLayerB   = 0;
       }
       else if ( 2 == Z )
       {
	  StartWheelA = 5;
	  EndWheelA   = 5;
	  StartLayerA = 0;
	  EndLayerA   = 15;

	  StartWheelB = 6;
	  EndWheelB   = 13;
	  StartLayerB = 0;
	  EndLayerB   = 7;
       }
       else
       {
	  ATH_MSG_FATAL( "fillCollID_broken_FDR: Bad rob_id " << std::hex << rob_id << std::dec );
	  return;
       }

       /*
	* A wheels
	*/
       int wheel = StartWheelA;
       while ( wheel <= EndWheelA )
       {
	  for (int layer = StartLayerA; layer <= EndLayerA; layer++ ) 
	  {
	     Identifier id = m_TRTHelper->layer_id( barrelId, phi, wheel, layer);

	     if (!m_TRTHelper->get_hash(id, idHash, &m_cntx)) 
	     {
		ids.push_back(idHash);
		made_collections++;
	     }
	     else
		ATH_MSG_FATAL( " A: Unable to get hash for id " \
		      << m_TRTHelper->show_to_string(id) << " " << barrelId << " " << phi << " " \
		      << wheel << " " << layer );
	  }  // loop over layer

	  wheel++;
       }    // loop over wheelA


       /*
	* B wheels
	*/
       wheel = StartWheelB;
       if ( 0 == wheel )
	 return;

       while ( wheel <= EndWheelB )
       {
	  for (int layer = StartLayerB; layer <= EndLayerB; layer++ ) 
	  {
	     Identifier id = m_TRTHelper->layer_id( barrelId, phi, wheel, layer);

	     if (!m_TRTHelper->get_hash(id, idHash, &m_cntx)) 
	     {
		ids.push_back(idHash);
		made_collections++;
	     }
	     else
		ATH_MSG_FATAL( " B: Unable to get hash for id " \
		      << m_TRTHelper->show_to_string(id)  << " " << barrelId << " " << phi << " " \
		      << wheel << " " << layer );
	  }  // loop over layer

	  wheel++;
       }    // loop over wheelB

   }      // endcap

   return;
}






/*
 * Get ROBIDs for each Detector Element (Straw Layer)
 *
 * Input : Straw ID
 * Output: list of ROB Source IDs
 */
std::vector<uint32_t> TRT_FillCablingData_DC3::getRobID(Identifier id) 
{
  std::vector<uint32_t> v;
  uint32_t Source=0;

  int id_barrel_ec = m_TRTHelper->barrel_ec(id);
  int id_phi_module = m_TRTHelper->phi_module(id);
  int id_layer = m_TRTHelper->layer_or_wheel(id);

  if ( !m_broken_FDR_Cabling )
  {
    int strawLayer=0;
    if ( (2 == id_barrel_ec) || (-2 == id_barrel_ec) )
    {
      if ( id_layer < 6 )
	strawLayer = id_layer * 2 * 8 + m_TRTHelper->straw_layer(id);
      else
	strawLayer = 6 * 16 + (id_layer - 6) * 8 + m_TRTHelper->straw_layer(id);
    }


    if ( 1 == id_barrel_ec )
    {
      Source = 0x310000 | ((id_phi_module+1) << 8);
    }
    else if ( -1 == id_barrel_ec )
    {
      Source = 0x320000 | ((id_phi_module+1) << 8);
    }
    else if ( 2 == id_barrel_ec )
    {
      Source = 0x330000 | ((id_phi_module+1) << 8) | m_StrawLayerToRod[strawLayer];
    }
    else if ( -2 == id_barrel_ec )
    {
      Source = 0x340000 | ((id_phi_module+1) << 8) | m_StrawLayerToRod[strawLayer];
    }
  }
  else   // broken_FDR_Cabling
  {
    if ( 1 == id_barrel_ec )
    {
      Source = 0x310000 | ((id_phi_module+1) << 8);
    }
    else if ( -1 == id_barrel_ec )
    {
      Source = 0x320000 | ((id_phi_module+1) << 8);
    }
    else if ( 2 == id_barrel_ec )
    {
      Source = 0x330000 | ((id_phi_module+1) << 8) | ((id_layer < 5)?1:2);
    }
    else if ( -2 == id_barrel_ec )
    {
      Source = 0x340000 | ((id_phi_module+1) << 8) | ((id_layer < 5)?1:2);
    }
  }

  if ( m_PartialDetector )
  {
      //      ATH_MSG_WARNING( "Running in Partial Detector Mode" );

      std::set< uint32_t >::iterator pos;
      pos = m_validSourceIds.find( Source );
      if ( pos == m_validSourceIds.end() )
      {
	 //	 ATH_MSG_WARNING( "Returning 0 instead of " << hex << Source << dec );

	 return v;
      }
      else
      {
	 //	 ATH_MSG_WARNING( "Returning " << hex << Source << dec );
      }

  }


  eformat::helper::SourceIdentifier sid( Source );
  v.push_back(sid.code());

  return v;
}



