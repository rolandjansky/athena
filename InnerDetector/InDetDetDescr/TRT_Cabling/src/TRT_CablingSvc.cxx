/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

//
// Implementation file for TRT_CablingSvc class
//

//#include <iostream>
#include "TRT_CablingSvc.h"
//#include <fstream>

#include "eformat/SourceIdentifier.h"  // change to new eformat v3
#include "PathResolver/PathResolver.h"
  // Tool 
#include "GaudiKernel/IToolSvc.h"


using eformat::helper::SourceIdentifier; 


  // Constructor
TRT_CablingSvc::TRT_CablingSvc( const std::string& name, 
			      ISvcLocator * pSvcLocator)
   : AthService( name, pSvcLocator ),
     m_manager                    ( nullptr ), 
     m_idHelper                   ( nullptr ),
     m_cabling                    ( nullptr ),
     m_cablingTool_SR1_ECC        ( nullptr ),
     m_cablingTool_SR1            ( nullptr ),
     m_cablingTool_TB             ( nullptr ),
     m_cablingTool_DC1            ( nullptr ),
     m_cablingTool_DC2            ( nullptr ),
     m_cablingTool_DC3            ( nullptr ),
     m_TRTLayout                  ( 0 ),
     m_numberOfStrawsInROD        ( 0 ),
     m_numberOfStrawsInBarrelROD  ( 0 ),
     m_shiftForLeftEndCapStraws   ( 0 ),
     m_shiftForLeftBarrelStraws   ( 0 ),
     m_shiftForRightBarrelStraws  ( 0 ),
     m_shiftForRightEndCapStraws  ( 0 )
{
  //  declareInterface< ITRT_CablingSvc >( this );   
}
	 
  // Destructor
TRT_CablingSvc::~TRT_CablingSvc()
{}



  // Initialisation
StatusCode TRT_CablingSvc::initialize( )
{
  StatusCode sc;
  ATH_MSG_INFO( "TRT_CablingSvc::initialize" );

  // Retrieve Detector Store
  ServiceHandle<StoreGateSvc> detStore("DetectorStore",name());
  if (detStore.retrieve().isFailure()) 
  {
    ATH_MSG_FATAL( "Detector service  not found !" );
    return StatusCode::FAILURE;
  }


  // Retrieve detector manager
  sc = detStore->retrieve(m_manager,"TRT"); 
  if (sc.isFailure()) {
    ATH_MSG_FATAL( "Cannot retrieve TRT_DetectorManager!" );
    return StatusCode::FAILURE;
  } 

  // Get the TRT Helper
  if (detStore->retrieve(m_idHelper, "TRT_ID").isFailure()) 
  {
    ATH_MSG_FATAL( "Could not get TRT ID helper" );
    return StatusCode::FAILURE;
  }
  m_cntx = m_idHelper->straw_layer_context();

  // Get ToolSvc
  IToolSvc* toolSvc;
  sc = service("ToolSvc",toolSvc);
  if (sc.isFailure()) 
  {
    ATH_MSG_ERROR( "Can't get ToolSvc!" );
    return StatusCode::FAILURE;
  }
  
  // Get tool for filling of cabling data
  std::string toolType;
  if (m_manager->getLayout()=="TestBeam")
  {
    m_TRTLayout = 1;
    toolType = "TRT_FillCablingData_TB04";
    ATH_MSG_INFO( "TRT TB04 Cabling" ); 
    if(StatusCode::SUCCESS !=toolSvc->retrieveTool(toolType,m_cablingTool_TB))
    {
      ATH_MSG_ERROR( " Can't get TRT_FillCablingData_TB04 tool " );
      return StatusCode::FAILURE;
    }
    m_cabling = m_cablingTool_TB->fillData();
  }
    // DC1
  else if ((m_manager->getLayout() == "Initial") || 
	   (m_manager->getLayout() == "Final") )
  {
    m_TRTLayout = 6;
    toolType = "TRT_FillCablingData_DC3";
    ATH_MSG_INFO( "TRT DC3 Cabling" ); 
    if( StatusCode::SUCCESS !=
	toolSvc->retrieveTool(toolType,m_cablingTool_DC3) )
    {
      ATH_MSG_ERROR( " Can't get TRT_FillCablingData_DC3 tool " );
      return StatusCode::FAILURE;
    }

    m_cabling = m_cablingTool_DC3->fillData();

  }
  else if ( m_manager->getLayout()=="SR1" )      // SR1 Barrel cosmics layout
  {
    m_TRTLayout = 4;
    toolType = "TRT_FillCablingData_SR1";
    ATH_MSG_INFO( "TRT SR1 Cabling" ); 
    if(StatusCode::SUCCESS !=toolSvc->retrieveTool(toolType,m_cablingTool_SR1))
    {
      ATH_MSG_ERROR( " Can't get TRT_FillCablingData_SR1 tool " );
      return StatusCode::FAILURE;
    }

    m_cabling = m_cablingTool_SR1->fillData();
  }
  else if ( m_manager->getLayout()=="SR1-EndcapC" )  // SR1 EC C cosmics layout
  {
    m_TRTLayout = 5;
    toolType = "TRT_FillCablingData_SR1_ECC";
    ATH_MSG_INFO( "TRT SR1 Cabling" ); 
    if( StatusCode::SUCCESS !=
	toolSvc->retrieveTool(toolType,m_cablingTool_SR1_ECC) )
    {
      ATH_MSG_ERROR( " Can't get TRT_FillCablingData_SR1_ECC tool " );
      return StatusCode::FAILURE;
    }

    m_cabling = m_cablingTool_SR1_ECC->fillData();
  }
  else
  {
     ATH_MSG_FATAL( " Layout is not defined for TRT_FillCablingData (" << m_manager->getLayout() << ")! " );
    return StatusCode::FAILURE;
  }

  ATH_MSG_INFO( "TRT_CablingSvc::initializiation finished" );
  
  return sc;
}


StatusCode TRT_CablingSvc::finalize()
{

  return StatusCode::SUCCESS;
}


StatusCode
TRT_CablingSvc::queryInterface( const InterfaceID & riid, 
				void** ppvInterface )
{
  if ( IIncidentListener::interfaceID().versionMatch(riid) )
  {
    *ppvInterface = dynamic_cast<IIncidentListener*>(this);
  } else if ( ITRT_CablingSvc::interfaceID().versionMatch(riid) ) 
         {
	   *ppvInterface = dynamic_cast<ITRT_CablingSvc*>(this);
	 } else {
    // Interface is not directly available : try out a base class
    return AthService::queryInterface(riid, ppvInterface);
  }
  addRef();
  return StatusCode::SUCCESS;
}


std::vector<uint32_t> TRT_CablingSvc::getRobID(Identifier& id) const
{

    // TB Case
  if (m_TRTLayout == 1)
  {
    return m_cablingTool_TB->getRobID(id);
  }
    // DC1 Case
  else if (m_TRTLayout == 2)
  {
    return m_cablingTool_DC1->getRobID(id);
  }
    // DC2
  else if ( m_TRTLayout == 3 )
  {
     return m_cablingTool_DC2->getRobID(id);
  }
  // SR1 Barrel
  else if ( m_TRTLayout == 4 )
  {
     return m_cablingTool_SR1->getRobID( id );
  }
  // SR1 EC C
  else if ( m_TRTLayout == 5 )
  {
     return m_cablingTool_SR1_ECC->getRobID( id );
  }
    // DC3
  else if ( m_TRTLayout == 6 )
  {
     return m_cablingTool_DC3->getRobID(id);
  }
  else
  {
    //     std::vector<uint32_t> *vint = new std::vector<uint32_t>;
    //     return *vint;

    std::vector<uint32_t> v;
    return v;
  }
}

Identifier TRT_CablingSvc::getIdentifier(const eformat::SubDetector& 
  subdetector,const unsigned& rod, const int& bufferOffset, 
  IdentifierHash& hashId) const
{
  int intRod = (int) rod;
  int shift;

    // TB04, SR1 or DC3 Case 
  if ( m_TRTLayout == 1 || m_TRTLayout == 4 || m_TRTLayout == 5 || m_TRTLayout == 6 )
  {
    hashId = m_cabling->get_identifierHashForAllStraws(intRod, bufferOffset);
    return m_cabling->get_identifierForAllStraws(intRod, bufferOffset);
  }

    // DC1, DC2 Case
  int straw = bufferOffset;
  if (subdetector == eformat::TRT_ENDCAP_A_SIDE)
  {
    shift = m_shiftForLeftEndCapStraws +
      intRod * m_numberOfStrawsInROD + straw; 
    hashId = m_cabling->get_identifierHashForAllStraws(shift);
    return m_cabling->get_identifierForAllStraws(shift);
  }

  if (subdetector == eformat::TRT_BARREL_A_SIDE)
  {
    shift = m_shiftForLeftBarrelStraws + 
     intRod * m_numberOfStrawsInBarrelROD + straw; 
    hashId = m_cabling->get_identifierHashForAllStraws(shift);
    return m_cabling->get_identifierForAllStraws(shift);
  }

  if (subdetector == eformat::TRT_BARREL_C_SIDE)
  {
    shift = m_shiftForRightBarrelStraws +
      intRod * m_numberOfStrawsInBarrelROD + straw;
    hashId = m_cabling->get_identifierHashForAllStraws(shift);
    return m_cabling->get_identifierForAllStraws(shift);
  }

  shift = m_shiftForRightEndCapStraws +
    intRod * m_numberOfStrawsInROD + straw;
  hashId = m_cabling->get_identifierHashForAllStraws(shift);
  return m_cabling->get_identifierForAllStraws(shift);

}



/*
 * getBufferOffset( strawId ) -
 * get Offset into ROD buffer given straw Identifier StrawId.
 *
 */
uint32_t TRT_CablingSvc::getBufferOffset( const Identifier &StrawId )
{
 
   // DC3 Case
   if ( m_TRTLayout == 6 )
   {
      return m_cabling->get_BufferOffset( StrawId );
   }
   else
   {
     ATH_MSG_FATAL( "TRT_CablingSvc::getBufferOffset called in invalid case !" );
     assert(0);
   }

   return 0;
}


const std::vector<uint32_t>& TRT_CablingSvc::getAllRods() const
{
  return m_cabling->get_allRods();
}



#ifdef DEPRECATED
uint32_t TRT_CablingSvc::getRodID(Identifier& id) const
{
    // DC1 Case
  if (m_TRTLayout == 2)
  {
    return m_cablingTool_DC1->getRodID(id);
  }
    // DC2 Case
  else
    return m_cablingTool_DC2->getRodID(id);
}

void TRT_CablingSvc::getStrawRodID(Identifier& id, uint32_t& rod_id, 
  uint32_t& straw_number) const
{
    // DC1 Case
  if (m_TRTLayout == 2)
  {
    m_cablingTool_DC1->getStrawRodID(id, rod_id, straw_number);
  }
   // DC2 Case
  else
    m_cablingTool_DC2->getStrawRodID(id, rod_id, straw_number);
}



const std::vector<IdentifierHash>& 
  TRT_CablingSvc::getCollID(uint32_t rob_id)
{
   return m_cabling->get_collID( rob_id );
}

int TRT_CablingSvc::getNumStrawInROD(uint32_t /*rod_id*/) const //remove unused variable to avoid compiler warning
{
  return m_numberOfStrawsInROD;
}


void TRT_CablingSvc::set_parameters()
{
  int numberOfStrawsInROD;
  int numberOfStrawsInBarrelROD;
  int shiftForLeftEndCapStraws;
  int shiftForLeftBarrelStraws;
  int shiftForRightBarrelStraws;
  int shiftForRightEndCapStraws;
  int numberOfEndCapPhiSectors;
  int numberOfIdentifierSectors;

    // DC1 Case
  if (m_TRTLayout == 2)
  {  
    m_cablingTool_DC1->get_parameters(numberOfStrawsInROD,
      numberOfStrawsInBarrelROD,
      shiftForLeftEndCapStraws,
      shiftForLeftBarrelStraws,
      shiftForRightBarrelStraws,
      shiftForRightEndCapStraws,
      numberOfEndCapPhiSectors,
      numberOfIdentifierSectors);
  }
    // DC2 Case
  else
  {
    m_cablingTool_DC2->get_parameters(numberOfStrawsInROD,
      numberOfStrawsInBarrelROD,
      shiftForLeftEndCapStraws,
      shiftForLeftBarrelStraws,
      shiftForRightBarrelStraws,
      shiftForRightEndCapStraws,
      numberOfEndCapPhiSectors,
      numberOfIdentifierSectors);
  }

  m_numberOfStrawsInROD = numberOfStrawsInROD;
  m_numberOfStrawsInBarrelROD = numberOfStrawsInBarrelROD;
  m_shiftForLeftEndCapStraws = shiftForLeftEndCapStraws;
  m_shiftForLeftBarrelStraws = shiftForLeftBarrelStraws;
  m_shiftForRightBarrelStraws = shiftForRightBarrelStraws;
  m_shiftForRightEndCapStraws = shiftForRightEndCapStraws;
  m_numberOfEndCapPhiSectors = numberOfEndCapPhiSectors;
  m_numberOfIdentifierSectors = numberOfIdentifierSectors;

}
#endif // DEPRACATED
