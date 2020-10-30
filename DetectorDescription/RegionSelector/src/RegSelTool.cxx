/**
 **   @file   RegSelTool.cxx         
 **            
 **           Implmentation of a local regionselector tool            
 **            
 **   @author sutt
 **   @date   Sun 22 Sep 2019 10:21:50 BST
 **
 **
 **   Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
 **/


#include "RegSelLUT/RegSelSiLUT.h"

#include "RegSelTool.h"



//! Constructor
RegSelTool::RegSelTool( const std::string& type, const std::string& name, const IInterface*  parent )
  :  base_class( type, name, parent ),
     m_initialised(false),
     m_dumpTable(false),
     m_rpcflag(false)
{
  //! Declare properties
  declareProperty( "WriteTable",  m_dumpTable,          "write out maps to files for debugging" );
  declareProperty( "Initialised", m_initialised=false,  "flag to determine whether the corresponding subsystem is initilised" );
}


//! Standard destructor
RegSelTool::~RegSelTool() { }


const IRegSelLUT* RegSelTool::lookup() const {
  if ( !m_initialised ) return nullptr; 
  SG::ReadCondHandle<IRegSelLUTCondData> table_handle( m_tableKey ); 
  return (*table_handle)->payload();
}



StatusCode RegSelTool::initialize() {
  ATH_MSG_DEBUG( "Initialising RegSelTool " << name() << "\ttable: " << m_tableKey );
  if ( !m_initialised ) { 
    ATH_MSG_WARNING( "Lookup table will not be initialised " << name() << "\tkey " << m_tableKey );
  } 
  ATH_CHECK( m_tableKey.initialize(m_initialised) );
  if ( name().find( "RPC") != std::string::npos ) m_rpcflag = true;
  return StatusCode::SUCCESS;
}




void RegSelTool::cleanup( std::vector<IdentifierHash>& idvec ) const {
  for ( size_t i=idvec.size() ; i-- ; ) idvec[i] = IdentifierHash( ((unsigned)idvec[i]) & 0xfff );
  RegSelSiLUT::removeDuplicates( idvec );
}




///////////////////////////////////////////////////////////////////////////////////

/// hash id access methods

/// standard roi

void RegSelTool::HashIDList( const IRoiDescriptor& roi, std::vector<IdentifierHash>& idlist ) const {

  if ( !m_initialised ) return; 

  if ( roi.composite() ) {
    idlist.clear();
    for ( unsigned iroi=roi.size() ; iroi-- ;  )  HashIDList( *(roi.at(iroi)), idlist );
    if ( roi.size()>1 ) RegSelSiLUT::removeDuplicates( idlist );
    return;
  }

  const IRegSelLUT* lookuptable = lookup();
  if ( lookuptable ) lookuptable->HashIDList( roi, idlist ); 
  if ( m_rpcflag ) cleanup( idlist );

}



/// standard roi for specific layer

void RegSelTool::HashIDList( long layer, const IRoiDescriptor& roi, std::vector<IdentifierHash>& idlist ) const {

  if ( !m_initialised ) return; 

  if ( roi.composite() ) { 
    idlist.clear();
    for ( unsigned iroi=roi.size() ; iroi-- ;  )  HashIDList( layer, *(roi.at(iroi)), idlist );
    if ( roi.size()>1 ) RegSelSiLUT::removeDuplicates( idlist );
    return;
  }

  const IRegSelLUT* lookuptable = lookup();
  if ( lookuptable ) lookuptable->HashIDList( layer, roi, idlist ); 
  if ( m_rpcflag ) cleanup( idlist );

}






///////////////////////////////////////////////////////////////////////////////////

/// ROB id access methods

/// standard roi

void RegSelTool::ROBIDList( const IRoiDescriptor& roi, std::vector<uint32_t>& roblist ) const {

  if ( !m_initialised ) return; 

  if ( roi.composite() ) { 
    roblist.clear();
    for ( unsigned iroi=roi.size() ; iroi-- ;  )  ROBIDList( *(roi.at(iroi)), roblist );
    RegSelSiLUT::removeDuplicates( roblist );
    return;
  }

  const IRegSelLUT* lookuptable = lookup();
  if ( lookuptable ) lookuptable->ROBIDList( roi, roblist ); 

}



/// standard roi for specific layer

void RegSelTool::ROBIDList( long layer, const IRoiDescriptor& roi, std::vector<uint32_t>& roblist ) const {

  if ( !m_initialised ) return; 

  if ( roi.composite() ) { 
    roblist.clear();
    for ( unsigned iroi=roi.size() ; iroi-- ;  )  ROBIDList( layer, *(roi.at(iroi)), roblist );
    RegSelSiLUT::removeDuplicates( roblist );
    return;
  }

  const IRegSelLUT* lookuptable = lookup();
  if ( lookuptable ) lookuptable->ROBIDList( layer, roi, roblist );

}



