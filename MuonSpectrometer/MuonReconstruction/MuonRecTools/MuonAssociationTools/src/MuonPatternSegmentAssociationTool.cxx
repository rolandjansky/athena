/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// MuonPatternSegmentAssociationTool.cxx, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

#include "MuonAssociationTools/MuonPatternSegmentAssociationTool.h"
#include "MuonSegment/MuonSegmentCombination.h" // Not sure these are actually needed. EJWM
#include "MuonPattern/MuonPatternCombination.h"
#include <stdexcept>

//================ Constructor =================================================

Muon::MuonPatternSegmentAssociationTool::MuonPatternSegmentAssociationTool(const std::string& t,
              const std::string& n,
              const IInterface*  p )
  :
  AthAlgTool(t,n,p),
  m_key("MuonPatternSegmentAssociationMap")
{
  declareInterface<IMuonPatternSegmentAssociationTool>(this);

  //  template for property decalration
  //declareProperty("PropertyName", m_propertyName);
}

//================ Destructor =================================================

Muon::MuonPatternSegmentAssociationTool::~MuonPatternSegmentAssociationTool()
{}


//================ Initialisation =================================================

StatusCode Muon::MuonPatternSegmentAssociationTool::initialize()
{
  
  ATH_CHECK( AthAlgTool::initialize() );
  declareProperty( "AssociationMapKey",     m_key);

  ATH_MSG_INFO("initialize() successful in " << name() );
  return StatusCode::SUCCESS;
}

//================ Finalisation =================================================

StatusCode Muon::MuonPatternSegmentAssociationTool::finalize()
{
  ATH_CHECK( AthAlgTool::finalize() );
  return StatusCode::SUCCESS;
}

//============================================================================================

inline 
const Muon::IMuonPatternSegmentAssociationTool::AssociationMap& 
Muon::MuonPatternSegmentAssociationTool::map() const {
   
   IMuonPatternSegmentAssociationTool::AssociationMap* assocmap = evtStore()->tryRetrieve<IMuonPatternSegmentAssociationTool::AssociationMap>(m_key);
   if (!assocmap) {
   	 assocmap = new IMuonPatternSegmentAssociationTool::AssociationMap;
	 if (evtStore()->record(assocmap, m_key).isFailure() ){
		 ATH_MSG_ERROR("Could not record association map at "+m_key);
		 throw std::runtime_error("Unable to record AssociationMap in SG.");
	 }
   }
   return nonconst_map();
}

inline 
Muon::IMuonPatternSegmentAssociationTool::AssociationMap& 
Muon::MuonPatternSegmentAssociationTool::nonconst_map() const {
   
   IMuonPatternSegmentAssociationTool::AssociationMap* assocmap = evtStore()->tryRetrieve<IMuonPatternSegmentAssociationTool::AssociationMap>(m_key);
   if (!assocmap) {
   	 assocmap = new IMuonPatternSegmentAssociationTool::AssociationMap;
	 if (evtStore()->record(assocmap, m_key).isFailure() ){
		 ATH_MSG_ERROR("Could not record association map at "+m_key);
		 throw std::runtime_error("Unable to record AssociationMap in SG.");
	 }
   }
   return *assocmap;
}

inline 
void 
Muon::MuonPatternSegmentAssociationTool::insert( const Muon::MuonSegmentCombination* segmentCombi, 
                                                 const Muon::MuonPatternCombination* associatedPatternCombi) const
{
    //std::pair<IMuonPatternSegmentAssociationTool::AssociationMap::iterator, bool> retVal;
    nonconst_map().insert(std::make_pair(segmentCombi, associatedPatternCombi) );
    return; // true if successfully inserted into map
}



inline 
Muon::IMuonPatternSegmentAssociationTool::AssociationMapRange 
Muon::MuonPatternSegmentAssociationTool::find(const Muon::MuonSegmentCombination* segmentCombi) const
{
    return map().equal_range(segmentCombi);
}

inline 
unsigned int 
Muon::MuonPatternSegmentAssociationTool::count(const MuonSegmentCombination* segmentCombi) const
{
    return map().count(segmentCombi);    
}


inline 
void
Muon::MuonPatternSegmentAssociationTool::reset() const {
	// EJWM - do we need this now?
   nonconst_map().clear();
}
