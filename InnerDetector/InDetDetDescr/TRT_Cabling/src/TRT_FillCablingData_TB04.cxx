/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//
// Implementation file for TRT_FillCablingData_TB04 class
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


//#include <iostream>
#include "GaudiKernel/ToolFactory.h"
#include "TRT_FillCablingData_TB04.h"
#include <fstream>

#include "eformat/SourceIdentifier.h"  // change to new eformat v3
#include "PathResolver/PathResolver.h"


using eformat::helper::SourceIdentifier; 

static const InterfaceID IID_ITRT_FillCablingData_TB04
                            ("TRT_FillCablingData_TB04", 1, 0);

  // Constructor
TRT_FillCablingData_TB04::TRT_FillCablingData_TB04( const std::string& type, const std::string& 
name,const IInterface* parent ):  AthAlgTool(type,name,parent),
				  m_TRTHelper(0)
{
  declareInterface< TRT_FillCablingData_TB04 >( this );   
}

	 
  // Destructor
TRT_FillCablingData_TB04::~TRT_FillCablingData_TB04()
{}


const InterfaceID& TRT_FillCablingData_TB04::interfaceID( )
{ return IID_ITRT_FillCablingData_TB04; }


  // Initialisation
StatusCode TRT_FillCablingData_TB04::initialize( )
{
  ATH_MSG_INFO( "TRT_FillCablingData_TB04::initialize" ); 


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
  
  ATH_MSG_INFO( "TRT_FillCablingData_TB04::initializiation finished" );

  return StatusCode::SUCCESS;
}


StatusCode TRT_FillCablingData_TB04::finalize()
{
  delete m_cabling;

  StatusCode sc = AlgTool::finalize(); 
  return sc;
}


TRT_CablingData* TRT_FillCablingData_TB04::fillData()
{
  return m_cabling;
}  


void TRT_FillCablingData_TB04::defineParameters()
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
  m_phi_to_source.push_back( 0x310003 );        // Phi 0 is Source ID 0x3103
  m_phi_to_source.push_back( 0x310002 );        // Phi 1 is Source ID 0x3102
  // NB: Source ID 0x3101 is the 3S boards on BOTH Phi 0 and Phi 1
     
   
  int numberOfStrawsInLayersA[] = {15, 16, 16, 16, 16, 17, 17, 17, 17, 17, 18,
    18, 18, 18, 18, 19, 19, 19, 18};

  int numberOfStrawsInLayersB[] = {19, 20, 20, 20, 20, 20, 21, 21, 21, 21, 21,
    22, 22, 22, 22, 22, 23, 23, 23, 23, 23, 24, 24, 23};

  int numberOfStrawsInLayersC[] = {23, 24, 24, 24, 24, 25, 25, 25, 25, 25, 26,
    26, 26, 26, 26, 27, 27, 27, 27, 27, 28, 28, 28, 28, 28, 29, 29, 29, 29, 28};
   
  std::vector<int> ncol0 (numberOfStrawsInLayersA, 
    numberOfStrawsInLayersA + sizeof(numberOfStrawsInLayersA) / sizeof(int));
  std::vector<int> ncol1 (numberOfStrawsInLayersB, 
    numberOfStrawsInLayersB + sizeof(numberOfStrawsInLayersB) / sizeof(int));
  std::vector<int> ncol2 (numberOfStrawsInLayersC, 
    numberOfStrawsInLayersC + sizeof(numberOfStrawsInLayersC) / sizeof(int));  

  m_ncol.push_back(ncol0);
  m_ncol.push_back(ncol1);
  m_ncol.push_back(ncol2);

  m_StrawsByModule[0] = 329;
  m_StrawsByModule[1] = 520;
  m_StrawsByModule[2] = 793;

}


  // Fill Tables with IDs for all straws
void TRT_FillCablingData_TB04::defineTables()
{
   // Reading mapping   
   // Important number for converter!!! Pay attention!!!
   // Another side has Id = -1
  int barrelId = -1;
  int phiModuleId, moduleId, strawLayerId, strawInLayerId,
     strawNumberInModule,BufferLocation;
  Identifier strawID, IdLayer;
  IdentifierHash hashId;

  // Loop over two RODs (not PhiModules!)
  for (int rod = 0; rod < m_numberOfIdentifierSectors; rod++ )
    {
     std::vector<Identifier> tempbuff;
     std::vector<IdentifierHash> tempbuff2;
     for (int i = 0; i < 8*13*16; i++){
       Identifier id(0);
       tempbuff.push_back(id);
       tempbuff2.push_back(0);
     }
     m_cabling->zero_identfierForAllStraws(rod, tempbuff);
     m_cabling->zero_identfierHashForAllStraws(rod, tempbuff2);
     
     std::ostringstream ssFile;
     ssFile << rod;
     std::string dataFile = "TRT_TB04_IdMapping_ROD" + ssFile.str() + ".dat";

     std::string file = PathResolver::find_file (dataFile, "DATAPATH");

     std::ifstream inputFile (file.c_str());
     if (!inputFile)
	ATH_MSG_FATAL( "TRT_FillCablingData_TB04: Could not open file " << dataFile );

      /*
       *       We now loop over all buffer locations 
       *       Read-out order is:
       *       ROD 0: 3S1, 3S2 (Assumed: UB3S and DB3S)
       *       ROD 1+2: 3L2, 3L1, 2L2, 2S, 2L1, 1S, 1L
       *       Data-files have been assembled in that order
       *       The individual file is ordered first after board and then
       *       after buffer location
      */
    

      while(!inputFile.eof()){
	/*
	 *     Need to add moduleId to data file, as the absolute strawnumber
	 *       is relative to the module layer
	 *       Also add phiModuleId. This is the easiest.
	 */
	inputFile >> phiModuleId >> moduleId >> strawNumberInModule
		  >> BufferLocation;

	/*
	 * Hack, because we do not detect EOF properly
	 */
	if ( -1 == phiModuleId && -1 == moduleId && 
	     -1 == strawNumberInModule && -1 == BufferLocation )
	{
	   continue;
	}

	if ( !(moduleId == 0 || moduleId == 1 || moduleId ==2) )
	   continue;

	if ( strawNumberInModule > m_StrawsByModule[moduleId] )
	   continue;

         // Swap of phi sectors for ROD 0: 3S1, 3S2
        if (rod == 0)
        {
          if (phiModuleId == 0)
            phiModuleId = 1;
          else
            phiModuleId = 0;
        }        


	int rodSourceId=0;
	if ( rod == 0 )
	   rodSourceId = 1;
	else if ( rod == 1 )
	   rodSourceId = 2;
	else if ( rod == 2 )
	   rodSourceId = 3;
	else 
	{
	   ATH_MSG_ERROR( " Bad rod index! (" << rod << ")" );
	   return;
	}

	/*
	 * DAQ numbers (seems to) go from 1, whereas offline Identifiers
	 * go from 0, so we'll just fix this:
	 */
	strawNumberInModule--;

	//Need to retrieve strawLayerId & strawInLayerId from ncols...
	strawLayerId=0;
	strawInLayerId=strawNumberInModule;
	
	while(strawInLayerId>=0){
	  //std::cout << "moduleID, strawLyerID" << moduleId << strawLayerId << std::endl;
	  strawInLayerId -= m_ncol[moduleId][strawLayerId];
	  strawLayerId++;
	}
	// Taking a step back
	strawLayerId--;
	strawInLayerId += m_ncol[moduleId][strawLayerId];

   	  //Apparently this is needed to skip to the next line
	inputFile.ignore(256,'\n');

	//std::cout<<"phiModuleId "<<phiModuleId  <<" moduleId "<< moduleId  <<" strawNumberInModule "<<strawNumberInModule<<std::endl;
	//std::cout<<"strawLayerId "<<strawLayerId  <<" strawInLayerId "<<strawInLayerId<<std::endl;
	// I NEED TO COUNT THE ENTRIES  [ptk: ???]

	//std::cout << "strawID " << barrelId << " " << phiModuleId << " " <<  moduleId << " " << strawLayerId << " " << strawInLayerId << std::endl;

	  // Construct an Identifier and put it in the list
        strawID = m_TRTHelper->straw_id(barrelId, phiModuleId, moduleId, 
          strawLayerId, strawInLayerId);
	m_cabling->set_identfierForAllStraws(0x310000 + rodSourceId, BufferLocation, strawID);
	//std::cout << "Filling m_identfierForAllStraws " << rodSourceId << " " << BufferLocation << " " << m_TRTHelper->show_to_string(strawID)  << std::endl;

       	  // Construct Hash and put it in the list
        IdLayer = m_TRTHelper->layer_id(strawID);
        if ( !m_TRTHelper->get_hash(IdLayer, hashId, &m_cntx) )
	   m_cabling->set_identfierHashForAllStraws(0x310000 + rodSourceId,
						    BufferLocation, hashId);
	else
	   ATH_MSG_DEBUG( "defineTables: unable to get hash for IdLayer " << IdLayer );

//	std::cout << "Filling  m_identfierHashForAllStraws " << rodSourceId << " " << BufferLocation << " " << hashId << std::endl;
      }   // end of loop over straw in ROD
      
      inputFile.close();  //
  }       // end of loop over RODs

  return;
}

/*
 * Define map of collection IDs for all ROBs
 */
void TRT_FillCablingData_TB04::defineCollID()
{
     // Initialize allRobs
   //eformat::ModuleType type = eformat::ROB_TYPE;
   //eformat::SubDetector  det_id = eformat::TRT_BARREL_A_SIDE;

     // Start from 1 because we have no Source ID = 0
   for(int rod = 1; rod <= m_numberOfIdentifierSectors; rod++ )
   {
      //eformat::helper::SourceIdentifier sid(det_id, type, (uint8_t) (rod));
      //      eformat::helper::SourceIdentifier sid(det_id, (uint8_t) (rod));
      uint32_t RODSourceId = 0x310000 + rod;

      std::vector<IdentifierHash> * vectID = new std::vector<IdentifierHash>();
      //      fillCollID(sid.code(), *vectID);
      fillCollID(RODSourceId, *vectID);

//      m_cabling->add_collID(sid.code(), vectID);
      m_cabling->add_collID(RODSourceId, vectID);
   }

   return;
}


/*
 * Input: ROB Source ID
 * Output: List of Hash IDs, one for each of 73 layers
 */
void TRT_FillCablingData_TB04::fillCollID(uint32_t rob_id,  
std::vector<IdentifierHash> & ids)
{

  int barrelId, phiModule=-1;
  IdentifierHash idHash;
  
  //  eformat::SubDetector det_id;   // set but not used
  int module;

  eformat::helper::SourceIdentifier id (rob_id);
  //  det_id = id.subdetector_id();  // set but not used
  module = (int) id.module_id();
  
  barrelId = -1;

    // Define Phi sector depending on ROB number
    // 3102 - Phi 1; 3103 - Phi 0; 
  if (module == 2)
  {
    phiModule = 1;
  }
  else if (module == 3) 
  {
    phiModule = 0;
  }
    
  if ( module == 1 )   // XXX -- ROB with both 3S boards!
  {
     int ring = 2;  // Only Module Type 3!
     for ( int phiModule=0; phiModule<2; phiModule++ )

     {
        for (int layer = 0; layer < m_numberOfLayersC; ++layer) 
	{
	  Identifier id = m_TRTHelper->layer_id(barrelId, phiModule,
            ring, layer);
	  if (!m_TRTHelper->get_hash(id, idHash, &m_cntx)) 
	  {
	    ids.push_back(idHash);
	  }
	  else 
            ATH_MSG_FATAL( " Unable to get hash for id " << m_TRTHelper->show_to_string(id) );
	}
     }

     return;
  }
	

  for (int ring = 0; ring < m_numberOfRings; ++ring)
  {
     if (ring == 0)  // Module Type 1
     {
        for (int layer = 0; layer < m_numberOfLayersA; ++layer) 
	{
	  Identifier id = m_TRTHelper->layer_id(barrelId, phiModule, 
            ring, layer);
	  if (!m_TRTHelper->get_hash(id, idHash, &m_cntx)) 
	    ids.push_back(idHash);
	  else
	    ATH_MSG_FATAL( " Unable to get hash for id " << m_TRTHelper->show_to_string(id) );
	}
     }
     else if (ring == 1)   // Module Type 2
     {
        for (int layer = 0; layer < m_numberOfLayersB; ++layer) 
	{
	  Identifier id = m_TRTHelper->layer_id(barrelId, phiModule,
            ring, layer);
	  if (!m_TRTHelper->get_hash(id, idHash, &m_cntx))
	    ids.push_back(idHash);
	  else  
            ATH_MSG_FATAL( " Unable to get hash for id " << m_TRTHelper->show_to_string(id) );
	}
     }
     else   //  Module Type 3
     {
        for (int layer = 0; layer < m_numberOfLayersC; ++layer) 
	{
	  Identifier id = m_TRTHelper->layer_id(barrelId, phiModule,
            ring, layer);
	  if (!m_TRTHelper->get_hash(id, idHash, &m_cntx)) 
	    ids.push_back(idHash);
	  else 
            ATH_MSG_FATAL( " Unable to get hash for id " << m_TRTHelper->show_to_string(id) );
	}
     }
  }

  return;
}

/*
 * Get ROBIDs for each Detector Element (Straw Layer)
 *
 * Input : Straw ID
 * Output: list of ROB Source IDs
 */
std::vector<uint32_t> TRT_FillCablingData_TB04::getRobID(Identifier id) 
{
  std::vector<uint32_t> v;

  //eformat::ModuleType type = eformat::ROB_TYPE;
  //  eformat::SubDetector det_id;  // set but not used

  int id_barrel_ec = m_TRTHelper->barrel_ec(id);
  int id_phi_module = m_TRTHelper->phi_module(id);
  //  int id_layer = m_TRTHelper->layer_or_wheel(id);

  /*
   * It seems to be assumed that the ROD/ROB source IDs are aligned to
   * the phi sector numbering.  This is NOT the case for TB04, so we
   * need to fix it up.
   * In addition, for Module type 3 (ie, layer 2), we need to read out
   * the ROB with the 3S boards (number 1), as well as the ROB with
   * the rest of the sector.
   * We assume only one side here.
   */
  if (id_barrel_ec == -1)
  {
     //    det_id = eformat::TRT_BARREL_A_SIDE;   // set but not used

    //eformat::helper::SourceIdentifier sid(det_id, type, 
    //    eformat::helper::SourceIdentifier sid(det_id,
    //			      (uint8_t) m_phi_to_source[id_phi_module] );
    //    v.push_back(sid.code());
    v.push_back( m_phi_to_source[id_phi_module] );

    

// !!! Artificial inclusion of the ROB XXX (which is in both
// phi-sectors) in list of ROBs for each collection. !!!
// This is done in order to have this ROB data to be read in 
// BS converter

    //eformat::helper::SourceIdentifier sid1(det_id, type, (uint8_t) 1);
    //    eformat::helper::SourceIdentifier sid1(det_id, (uint8_t) 0x310001);
    //    v.push_back(sid1.code());
    v.push_back( 0x310001 );
  }
  else
     ATH_MSG_WARNING ( \
       " TRT_FillCablingData_TB04 --> Couldn't get RobID for given Identifier " \
       << m_TRTHelper->print_to_string(id) << ", cabling differs from dictionary " );

  return v;
}
