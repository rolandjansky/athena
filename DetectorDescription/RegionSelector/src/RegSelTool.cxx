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


#include "RegSelTool.h"
#include "RegSelLUT/RegSelRoI.h"
#include "RegSelLUT/RegSelSiLUT.h"
// #include "RegSelLUT/RegSelModule.h"

#include "GaudiKernel/ToolHandle.h"

// ???
#include "RegionSelector/StoreGateRS_ClassDEF.h"
#include "RegSelLUT/StoreGateIDRS_ClassDEF.h"



//! Constructor
RegSelTool::RegSelTool( const std::string& type, const std::string& name, const IInterface*  parent )
  :  base_class( type, name, parent ),
     m_dumpTable(false),
     m_lookuptable{nullptr}
{
  //! Declare properties
  declareProperty( "WriteTables", m_dumpTable,    "write out maps to files for debugging" );
}


//! Standard destructor
RegSelTool::~RegSelTool() { }


StatusCode RegSelTool::initialize() {
  ATH_MSG_INFO( "Initializing " << name() );
  return StatusCode::SUCCESS;
}


StatusCode RegSelTool::finalize() {
  ATH_MSG_INFO( "Finalizing " << name() );
  return StatusCode::SUCCESS;
}


bool RegSelTool::handle() { 
  return m_initialised = false;
}



// new RegionSelector interface for the Innner Detector 

void RegSelTool::getRoIData( const IRoiDescriptor& roi, std::vector<const RegSelModule*>& modules ) const {
  modules.clear();
  RegSelRoI roitmp( roi.zedMinus(), roi.zedPlus(), roi.phiMinus(), roi.phiPlus(), roi.etaMinus(), roi.etaPlus() );
  if ( m_lookuptable ) m_lookuptable->getRoIData( roitmp, modules );
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
  if ( m_lookuptable ) m_lookuptable->getHashList( roitmp, idlist ); 
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
  if ( m_lookuptable ) m_lookuptable->getHashList( roitmp, layer, idlist ); 
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
  if ( m_lookuptable ) m_lookuptable->getRobList( roitmp, roblist ); 
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
  if ( m_lookuptable ) m_lookuptable->getRobList( roitmp, layer, roblist ); ///  m_duplicateRemoval ); ??? 
}



///////////////////////////////////////////////////////////////////////////////////

/// protected fullscan access methods

/// full scan hashid 

void RegSelTool::HashIDList( std::vector<IdentifierHash>& idlist ) const {
  if ( m_lookuptable ) m_lookuptable->getHashList( idlist ); 
}

/// fullscan hashid for specific layer 

void RegSelTool::HashIDList( long layer, std::vector<IdentifierHash>& idlist ) const {
  if ( m_lookuptable ) m_lookuptable->getHashList( layer, idlist ); 
}

/// full scan robid

void RegSelTool::ROBIDList( std::vector<uint32_t>& roblist ) const {
  if ( m_lookuptable ) m_lookuptable->getRobList( roblist ); 
}

/// fullscan robid for specific layer 

void RegSelTool::ROBIDList( long layer, std::vector<uint32_t>& roblist ) const {
  if ( m_lookuptable ) m_lookuptable->getRobList( layer, roblist ); 
}

