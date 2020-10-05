/**
 **   @file   MuonRegSelCondAlg.cxx         
 **            
 **           conditions algorithm to create the Si detector 
 **           lookup tables    
 **            
 **   @author sutt
 **   @date   Sun 22 Sep 2019 10:21:50 BST
 **
 **
 **   Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
 **/


#include "GaudiKernel/EventIDRange.h"
#include "StoreGate/WriteCondHandle.h"
#include "MuonRegSelCondAlg.h"


#include <iostream>
#include <fstream>
#include <string>



MuonRegSelCondAlg::MuonRegSelCondAlg(const std::string& name, ISvcLocator* pSvcLocator):
  AthReentrantAlgorithm( name, pSvcLocator ),
  m_managerName(""),
  m_printTable(false)
{ 
  ATH_MSG_DEBUG( "MuonRegSelCondAlg::MuonRegSelCondAlg() " << name );
  declareProperty( "PrintTable",  m_printTable=false );  
  declareProperty( "ManagerName", m_managerName );  
}




StatusCode MuonRegSelCondAlg::initialize()
{
  ATH_MSG_DEBUG("MuonRegSelCondAlg::initialize() ");
  ATH_CHECK(m_tableKey.initialize());
  ATH_MSG_INFO("MuonRegSelCondAlg::initialize() " << m_tableKey );
  return StatusCode::SUCCESS;
}



StatusCode MuonRegSelCondAlg::execute(const EventContext& ctx )  const
{
  ATH_MSG_DEBUG("MuonRegSelCondAlg::execute() -- enter -- ");
  
  /// do stuff here ...  
  ATH_MSG_DEBUG( "Creating region selector table " << m_tableKey );


  SG::WriteCondHandle<IRegSelLUTCondData> lutCondData( m_tableKey, ctx );
  if (lutCondData.isValid()) {
    /// inpractice, this should never be called, although in serial athena,                                                                          
    /// because the implementation of the conditions behaviour is flawed in                                                                          
    /// the framework, this routine will be called every event (!) regardless                                                                        
    /// of whether it should be called or not so we need this check to                                                                               
    /// prevent unecessary code execution on out our side                                                                                            
    ATH_MSG_DEBUG("CondHandle " << lutCondData.fullKey() << " is already valid." );
    return StatusCode::SUCCESS;
  }

  /// create the new lookup table

  EventIDRange id_range;

  std::unique_ptr<IRegSelLUT> rd = createTable( ctx, id_range );

  if ( !rd ) return StatusCode::FAILURE;

  ATH_MSG_DEBUG( "Initialising new map " );;

  // write out new new LUT to a file if need be
  
  if ( m_printTable ) dynamic_cast<const RegSelSiLUT*>(rd.get())->write( name()+".map" );

  /// create the conditions data for storage 

  IRegSelLUTCondData* rcd = new IRegSelLUTCondData( std::move(rd) );
  
  try {
    /// leave this commented here since this is where it should really be,
    /// but we had to move it up in the code to handle the flawed conditions 
    /// handling in the serial athena use case 
    ///    SG::WriteCondHandle<IRegSelLUTCondData> lutCondData( m_tableKey, ctx );
    if( lutCondData.record( id_range, rcd ).isFailure() ) {
      ATH_MSG_ERROR( "Could not record " << m_tableKey 
		     << " " << lutCondData.key()
		     << " with range " << id_range );  
      return StatusCode::FAILURE;   
    } 
    ATH_MSG_INFO( "RegSelCondAlg LUT recorded: " << m_tableKey);
  }
  catch (...) { 
    ATH_MSG_ERROR("MuonRegSelCondAlg::execute() failed to record table: " << m_tableKey);
    return StatusCode::FAILURE;   
  }   
  
  
  ATH_MSG_DEBUG("MuonRegSelCondAlg::execute() -- exit -- ");

  return StatusCode::SUCCESS;
}




