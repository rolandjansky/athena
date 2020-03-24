/**
 **   @file   RegSelCaloTool.cxx         
 **            
 **           Implmentation of a local regionselector tool            
 **            
 **   @author sutt
 **   @date   Sun 22 Sep 2019 10:21:50 BST
 **
 **
 **   Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
 **/



#include "RegSelCaloTool.h"
#include "RegSelLUT/RegSelSiLUT.h"


//! Constructor
RegSelCaloTool::RegSelCaloTool( const std::string& type, const std::string& name, const IInterface*  parent )
  :  base_class( type, name, parent ),
     m_initialised(false),
     m_dumpTable(false)
{
  //! Declare properties
  declareProperty( "WriteTable",  m_dumpTable,          "write out maps to files for debugging" );
  declareProperty( "Initialised", m_initialised=false,  "flag to determine whether the corresponding subsystem is initilised" );
}


//! Standard destructor
RegSelCaloTool::~RegSelCaloTool() { }


const RegSelectorHashMap* RegSelCaloTool::lookup() const {
  if ( !m_initialised ) return nullptr; 
  SG::ReadCondHandle< RegSelCondData<RegSelectorHashMap> > table_handle( m_tableKey ); 
  return (*table_handle)->payload();;    
}



StatusCode RegSelCaloTool::initialize() {
  ATH_CHECK( m_tableKey.initialize() );
  ATH_MSG_DEBUG( "Initialising RegSelCaloTool " << name() << "\ttable: " << m_tableKey );
  if ( !m_initialised ) { 
    ATH_MSG_WARNING( "Lookup table will not be initialised " << name() << "\tkey " << m_tableKey );
  } 
  return StatusCode::SUCCESS;
}



StatusCode RegSelCaloTool::finalize() {
  ATH_MSG_INFO( "Finalizing " << name() );
  return StatusCode::SUCCESS;
}







///////////////////////////////////////////////////////////////////////////////////

/// hash id access methods

/// standard roi

void RegSelCaloTool::HashIDList( const IRoiDescriptor& roi, std::vector<IdentifierHash>& idlist ) const {

  if ( !m_initialised ) return; 

  if ( roi.composite() ) {
    idlist.clear();
    for ( unsigned iroi=roi.size() ; iroi-- ;  )  HashIDList( *(roi.at(iroi)), idlist );
    if ( roi.size()>1 ) RegSelSiLUT::removeDuplicates( idlist );
    return;
  }

  if ( roi.isFullscan() ) return HashIDList( idlist );

  const RegSelectorHashMap* lookuptable = lookup();

  if ( lookuptable ) lookuptable->regionSelector( roi.etaMinus(), roi.etaPlus(), roi.phiMinus(), roi.phiPlus(), idlist);

}



/// standard roi for specific layer

void RegSelCaloTool::HashIDList( long layer, const IRoiDescriptor& roi, std::vector<IdentifierHash>& idlist ) const {

  if ( !m_initialised ) return; 

  if ( roi.composite() ) { 
    idlist.clear();
    for ( unsigned iroi=roi.size() ; iroi-- ;  )  HashIDList( layer, *(roi.at(iroi)), idlist );
    if ( roi.size()>1 ) RegSelSiLUT::removeDuplicates( idlist );
    return;
  }

  if ( roi.isFullscan() ) return HashIDList( layer, idlist );

  RegSelRoI roitmp( roi.zedMinus(), roi.zedPlus(), roi.phiMinus(), roi.phiPlus(), roi.etaMinus(), roi.etaPlus() );
  const RegSelectorHashMap* lookuptable = lookup();

  if ( lookuptable ) lookuptable->regionSelector( layer, roi.etaMinus(), roi.etaPlus(), roi.phiMinus(), roi.phiPlus(), idlist);
  
}






///////////////////////////////////////////////////////////////////////////////////

/// ROB id access methods

/// standard roi

void RegSelCaloTool::ROBIDList( const IRoiDescriptor& roi, std::vector<uint32_t>& roblist ) const {

  if ( !m_initialised ) return; 

  if ( roi.composite() ) { 
    roblist.clear();
    for ( unsigned iroi=roi.size() ; iroi-- ;  )  ROBIDList( *(roi.at(iroi)), roblist );
    RegSelSiLUT::removeDuplicates( roblist );
    return;
  }

  if ( roi.isFullscan() ) return ROBIDList( roblist );

  const RegSelectorHashMap* lookuptable = lookup();

  if ( lookuptable ) lookuptable->regionSelectorRobIdUint( roi.etaMinus(), roi.etaPlus(), roi.phiMinus(), roi.phiPlus(), roblist);

}



/// standard roi for specific layer

void RegSelCaloTool::ROBIDList( long layer, const IRoiDescriptor& roi, std::vector<uint32_t>& roblist ) const {

  if ( !m_initialised ) return; 

  if ( roi.composite() ) { 
    roblist.clear();
    for ( unsigned iroi=roi.size() ; iroi-- ;  )  ROBIDList( layer, *(roi.at(iroi)), roblist );
    RegSelSiLUT::removeDuplicates( roblist );
    return;
  }

  if ( roi.isFullscan() ) return ROBIDList( layer, roblist );

  const RegSelectorHashMap* lookuptable = lookup();

  if ( lookuptable ) lookuptable->regionSelectorRobIdUint( layer, roi.etaMinus(), roi.etaPlus(), roi.phiMinus(), roi.phiPlus(), roblist);
}



///////////////////////////////////////////////////////////////////////////////////

/// protected fullscan access methods

/// NB: sadly the calorimeter tables don't have any nice internal checks to 
///     just return the full list without bothering to check the eta and phi 
///     ranges, so we have to give it actual limits so these are hardcoded
///     exactly as in the old RegSelSvc
///     At some point, these calorimeter tables really should be completely 
///     replaced by new table classes - additional functional changes to the 
///     RegSelSiLUT might be possible, then we could do away with the specific 
///     calorimeter versions of the tools completely   

/// full scan hashid 

void RegSelCaloTool::HashIDList( std::vector<IdentifierHash>& idlist ) const {
  const RegSelectorHashMap* lookuptable = lookup();
  if ( lookuptable ) lookuptable->regionSelector( -4.8, 4.8, -M_PI, M_PI, idlist);
}


/// fullscan hashid for specific layer 

void RegSelCaloTool::HashIDList( long layer, std::vector<IdentifierHash>& idlist ) const {
  const RegSelectorHashMap* lookuptable = lookup();
  if ( lookuptable ) lookuptable->regionSelector( layer, -4.8, 4.8, -M_PI, M_PI, idlist);
}

/// full scan robid

void RegSelCaloTool::ROBIDList( std::vector<uint32_t>& roblist ) const {
  const RegSelectorHashMap* lookuptable = lookup();
  if ( lookuptable ) lookuptable->regionSelectorRobIdUint( -4.8, 4.8, -M_PI, M_PI, roblist);
}

/// fullscan robid for specific layer 

void RegSelCaloTool::ROBIDList( long layer, std::vector<uint32_t>& roblist ) const {
  const RegSelectorHashMap* lookuptable = lookup();
  if ( lookuptable ) lookuptable->regionSelectorRobIdUint( layer, -4.8, 4.8, -M_PI, M_PI, roblist);
}




