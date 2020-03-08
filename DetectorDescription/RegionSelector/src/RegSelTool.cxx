/**
 **   @file   RegSelTool.cxx         
 **            
 **           Implmentation of a local regionselector tool            
 **            
 **   @author sutt
 **   @date   Sun 22 Sep 2019 10:21:50 BST
 **
 **
 **   Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
 **/


#include "RegSelLUT/RegSelRoI.h"
#include "RegSelLUT/RegSelSiLUT.h"

#include "GaudiKernel/ToolHandle.h"

#include "RegSelTool.h"


#include "RegionSelector/StoreGateRS_ClassDEF.h"
#include "RegSelLUT/StoreGateIDRS_ClassDEF.h"



//! Constructor
RegSelTool::RegSelTool( const std::string& type, const std::string& name, const IInterface*  parent )
  :  base_class( type, name, parent ),
     m_dumpTable(false)
{
  //! Declare properties
  declareProperty( "WriteTable", m_dumpTable,    "write out maps to files for debugging" );
}


//! Standard destructor
RegSelTool::~RegSelTool() { }


const RegSelSiLUT* RegSelTool::lookup() const {
  SG::ReadCondHandle< RegSelCondData<RegSelSiLUT> > table_handle( m_tableKey ); 
  //  ATH_CHECK( table_handle.isSuccess() );
  const RegSelSiLUT* lookup_table = (*table_handle)->payload();
  return lookup_table;
}




StatusCode RegSelTool::initialize() {
  ATH_CHECK( m_tableKey.initialize() );
  ATH_MSG_INFO( "Initialising " << name() << "\tkey " << m_tableKey );
  return StatusCode::SUCCESS;
}



StatusCode RegSelTool::finalize() {
  ATH_MSG_INFO( "Finalizing " << name() );
  return StatusCode::SUCCESS;
}


bool RegSelTool::handle() { 
  return m_initialised = true;
}



// new RegionSelector interface for the Innner Detector 

void RegSelTool::getRoIData( const IRoiDescriptor& roi, std::vector<const RegSelModule*>& modules ) const {
  modules.clear();
  RegSelRoI roitmp( roi.zedMinus(), roi.zedPlus(), roi.phiMinus(), roi.phiPlus(), roi.etaMinus(), roi.etaPlus() );
  const RegSelSiLUT* lookuptable = lookup();
  if ( lookuptable ) lookuptable->getRoIData( roitmp, modules );
}




///////////////////////////////////////////////////////////////////////////////////

/// hash id access methods

/// standard roi

void RegSelTool::HashIDList( const IRoiDescriptor& roi, std::vector<IdentifierHash>& idlist ) const {

  if ( roi.composite() ) {
    idlist.clear();
    for ( unsigned iroi=roi.size() ; iroi-- ;  )  HashIDList( *(roi.at(iroi)), idlist );
    if ( roi.size()>1 ) RegSelSiLUT::removeDuplicates( idlist );
    return;
  }

  if ( roi.isFullscan() ) return HashIDList( idlist );

  RegSelRoI roitmp( roi.zedMinus(), roi.zedPlus(), roi.phiMinus(), roi.phiPlus(), roi.etaMinus(), roi.etaPlus() );
  const RegSelSiLUT* lookuptable = lookup();
  if ( lookuptable ) lookuptable->getHashList( roitmp, idlist ); 

}



/// standard roi for specific layer

void RegSelTool::HashIDList( long layer, const IRoiDescriptor& roi, std::vector<IdentifierHash>& idlist ) const {

  if ( roi.composite() ) { 
    idlist.clear();
    for ( unsigned iroi=roi.size() ; iroi-- ;  )  HashIDList( layer, *(roi.at(iroi)), idlist );
    if ( roi.size()>1 ) RegSelSiLUT::removeDuplicates( idlist );
    return;
  }

  if ( roi.isFullscan() ) return HashIDList( layer, idlist );

  RegSelRoI roitmp( roi.zedMinus(), roi.zedPlus(), roi.phiMinus(), roi.phiPlus(), roi.etaMinus(), roi.etaPlus() );
  const RegSelSiLUT* lookuptable = lookup();
  if ( lookuptable ) lookuptable->getHashList( roitmp, layer, idlist ); 

}






///////////////////////////////////////////////////////////////////////////////////

/// ROB id access methods

/// standard roi

void RegSelTool::ROBIDList( const IRoiDescriptor& roi, std::vector<uint32_t>& roblist ) const {

  if ( roi.composite() ) { 
    roblist.clear();
    for ( unsigned iroi=roi.size() ; iroi-- ;  )  ROBIDList( *(roi.at(iroi)), roblist );
    RegSelSiLUT::removeDuplicates( roblist );
    return;
  }

  if ( roi.isFullscan() ) return ROBIDList( roblist );

  RegSelRoI roitmp( roi.zedMinus(), roi.zedPlus(), roi.phiMinus(), roi.phiPlus(), roi.etaMinus(), roi.etaPlus() );

  const RegSelSiLUT* lookuptable = lookup();
  if ( lookuptable ) lookuptable->getRobList( roitmp, roblist ); 
}



/// standard roi for specific layer

void RegSelTool::ROBIDList( long layer, const IRoiDescriptor& roi, std::vector<uint32_t>& roblist ) const {

  if ( roi.composite() ) { 
    roblist.clear();
    for ( unsigned iroi=roi.size() ; iroi-- ;  )  ROBIDList( layer, *(roi.at(iroi)), roblist );
    RegSelSiLUT::removeDuplicates( roblist );
    return;
  }

  if ( roi.isFullscan() ) return ROBIDList( layer, roblist );

  RegSelRoI roitmp( roi.zedMinus(), roi.zedPlus(), roi.phiMinus(), roi.phiPlus(), roi.etaMinus(), roi.etaPlus() );

  const RegSelSiLUT* lookuptable = lookup();
  if ( lookuptable ) lookuptable->getRobList( roitmp, layer, roblist ); ///  m_duplicateRemoval ); ??? 
}



///////////////////////////////////////////////////////////////////////////////////

/// protected fullscan access methods

/// full scan hashid 

void RegSelTool::HashIDList( std::vector<IdentifierHash>& idlist ) const {
  const RegSelSiLUT* lookuptable = lookup();
  if ( lookuptable ) lookuptable->getHashList( idlist ); 
}

/// fullscan hashid for specific layer 

void RegSelTool::HashIDList( long layer, std::vector<IdentifierHash>& idlist ) const {
  const RegSelSiLUT* lookuptable = lookup();
  if ( lookuptable ) lookuptable->getHashList( layer, idlist ); 
}

/// full scan robid

void RegSelTool::ROBIDList( std::vector<uint32_t>& roblist ) const {
  const RegSelSiLUT* lookuptable = lookup();
  if ( lookuptable ) lookuptable->getRobList( roblist ); 
}

/// fullscan robid for specific layer 

void RegSelTool::ROBIDList( long layer, std::vector<uint32_t>& roblist ) const {
  const RegSelSiLUT* lookuptable = lookup();
  if ( lookuptable ) lookuptable->getRobList( layer, roblist ); 
}




