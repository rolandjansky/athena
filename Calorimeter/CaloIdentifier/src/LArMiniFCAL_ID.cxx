/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#include "CaloIdentifier/LArMiniFCAL_ID.h"
#include "AtlasDetDescr/AtlasDetectorID.h"
#include "Identifier/IdentifierHash.h"
#include "IdDict/IdDictDefs.h"
#include "PathResolver/PathResolver.h"

#include "GaudiKernel/MsgStream.h"

#include <string>
#include <set>
#include <iostream>
#include <fstream>
#include <sstream>
#include <math.h>
#include <stdexcept>

#include "CxxUtils/StrFormat.h"

using CxxUtils::strformat;


LArMiniFCAL_ID::LArMiniFCAL_ID(void) 
        : 
        CaloIDHelper ("LArMiniFCAL_ID"),
        m_is_initialized(false),
        m_fcal_region_index(0),
        m_minifcal_region_index(0),
        m_LAR_INDEX(999),
        m_MiniFCAL_INDEX(999),
        m_POSNEG_INDEX(999),
        m_MODULE_INDEX(999),
        m_DEPTH_INDEX(999),
        m_ETA_INDEX(999),
        m_PHI_INDEX(999),
        m_two_sym_sides(1)
{}

LArMiniFCAL_ID::~LArMiniFCAL_ID(void) 
{}

int LArMiniFCAL_ID::eta_min(const Identifier modId) const
{
  ExpandedIdentifier expId;
  IdContext module_cntxt = module_context();
  if(!get_expanded_id(modId, expId, &module_cntxt)) {
    int result = -999;
    for (unsigned int i = 0; i < m_full_channel_range.size(); ++i) {
      const Range& range = m_full_channel_range[i];
      if (range.match(expId)) {
	const Range::field& eta_field = range[m_ETA_INDEX];
	if (eta_field.has_minimum()) {
	  int etamin = eta_field.get_minimum();
	  if (-999 == result) {
	    result = etamin;
	  }
	  else {
	    if (etamin < result) result = etamin;
	  }
	}
      }
    }
    return (result);
  }
  return (-999);
}

int LArMiniFCAL_ID::eta_max(const Identifier modId) const
{
  ExpandedIdentifier expId;
  IdContext module_cntxt = module_context();
  if(!get_expanded_id(modId, expId, &module_cntxt)) {
    int result = -999;
    for (unsigned int i = 0; i < m_full_channel_range.size(); ++i) {
      const Range& range = m_full_channel_range[i];
      if (range.match(expId)) {
	const Range::field& eta_field = range[m_ETA_INDEX];
	if (eta_field.has_maximum()) {
	  int etamax = eta_field.get_maximum();
	  if (result < etamax) result = etamax;
	}
      }
    }
    return (result);
  }
  return (-999);  // default
}

int LArMiniFCAL_ID::phi_min_init(const Identifier modId) const
{
  ExpandedIdentifier expId;
  IdContext module_cntxt = module_context();
  if(!get_expanded_id(modId, expId, &module_cntxt)) {
    int result = -999;
    for (unsigned int i = 0; i < m_full_channel_range.size(); ++i) {
      const Range& range = m_full_channel_range[i];
      if (range.match(expId)) {
	const Range::field& phi_field = range[m_PHI_INDEX];
	if (phi_field.has_minimum()) {
	  int phimin = phi_field.get_minimum();
	  if (-999 == result) {
	    result = phimin;
	  }
	  else {
	    if (phimin < result) result = phimin;
	  }
	}
      }
    }
    return (result);
  }
  return (-999);  // default
}

int LArMiniFCAL_ID::phi_max(const Identifier modId) const
{
  ExpandedIdentifier expId;
  IdContext module_cntxt = module_context();
  if(!get_expanded_id(modId, expId, &module_cntxt)) {
    int result = -999;
    for (unsigned int i = 0; i < m_full_channel_range.size(); ++i) {
      const Range& range = m_full_channel_range[i];
      if (range.match(expId)) {
	const Range::field& phi_field = range[m_PHI_INDEX];
	if (phi_field.has_maximum()) {
	  int phimax = phi_field.get_maximum();
	  if (result < phimax) result = phimax;
	}
      }
    }
    return (result);
  }
  return (-999);  // default
}

int   LArMiniFCAL_ID::get_neighbours(const IdentifierHash id, const LArNeighbours::neighbourOption& option, std::vector<IdentifierHash>& neighbourList) const
{
  int result = 1; 

  neighbourList.clear();

  if(!m_do_neighbours) {
    if(m_msgSvc) {
      MsgStream log(m_msgSvc, "LArMiniFCAL_ID" );
      log << MSG::WARNING << "neighbours not initialized !!! returning empty list" << endmsg;
    }
    else {
      std::cout << " neighbours not initialized !!! returning empty list " << std::endl;
    }
    return result;
  }

  if(id>=channels().hash_max()) {
    if(m_msgSvc) {
      MsgStream log(m_msgSvc, "LArMiniFCAL_ID" );
      log << MSG::WARNING << "neighbours requested for  non-existing channel -- id/max " << id << "/"
          << channel_hash_max() << endmsg;
    }
    else {
      std::cout << " neighbours requested for non-existing channel -- id/max " << id << "/"
                << channel_hash_max() << std::endl;
    }
    return result;
  }

  if( dictionaryVersion() == "fullAtlas" ) {

    if ( (option & LArNeighbours::all2D) == LArNeighbours::all2D ){
      if ( m_neighbors_2d_vec[(unsigned int)id].size() > 0 ) { 
	neighbourList.insert(neighbourList.end(),
			     m_neighbors_2d_vec[(unsigned int)id].begin(),
			     m_neighbors_2d_vec[(unsigned int)id].end());
      }
    }
    
    if ( (option & LArNeighbours::prevInSamp) ){
      if ( m_neighbors_3d_prev_vec[(unsigned int)id].size() > 0 ) { 
	neighbourList.insert(neighbourList.end(),
			     m_neighbors_3d_prev_vec[(unsigned int)id].begin(),
			     m_neighbors_3d_prev_vec[(unsigned int)id].end());
      }
    }
    
    if ( (option & LArNeighbours::nextInSamp) ){
      if ( m_neighbors_3d_next_vec[(unsigned int)id].size() > 0 ) { 
	neighbourList.insert(neighbourList.end(),
			     m_neighbors_3d_next_vec[(unsigned int)id].begin(),
			     m_neighbors_3d_next_vec[(unsigned int)id].end());
      }
    }
    
  } else {
    if(m_msgSvc) {
      MsgStream log(m_msgSvc, "LArMiniFCAL_ID" );
      log << MSG::WARNING << " NO MiniFCAL neighbours (yet) in the context of " << dictionaryVersion() << endmsg;
    }
    else {
      std::cout << " NO MiniFCAL neighbours (yet) in the context of " << dictionaryVersion() << std::endl;
    }
  }
  return result;
}

IdContext	
LArMiniFCAL_ID::module_context 		(void) const
{
  return region_context();
}

int  LArMiniFCAL_ID::initialize_from_dictionary (const IdDictMgr& dict_mgr)
/*=================================================================*/
{
  MsgStream log(m_msgSvc, "LArMiniFCAL_ID" );

  std::string strg =  "initialize_from_dictionary";
  log << MSG::DEBUG << strg << endmsg;

  // Check whether this helper should be reinitialized
  if (!reinitialize(dict_mgr)) {
    log << MSG::DEBUG << "Request to reinitialize not satisfied - tags have not changed" << endmsg;
    return (0);
  }
  else {
    if(m_msgSvc)log << MSG::DEBUG << "(Re)initialize" << endmsg;
  }

  // init base object
  if(CaloIDHelper::initialize_base_from_dictionary(dict_mgr,
                                                   "LArCalorimeter"))
    return (1);
  
  // initialize dictionary version
  AtlasDetectorID::setDictVersion(dict_mgr, "LArCalorimeter");

  // Check whether dictionary has miniFCAL
  if (!dict()->find_group("lar_mini_fcal")) {
    strg =  " initialize_from_dict - LArCalorimeter dictionary does NOT contain miniFCAL description. Unable to initialize LArMiniFCAL_ID.";
    log << MSG::INFO << strg << endmsg;
    return 0; // return OK
  }
      

  // Initialize the field indices
  if(initLevelsFromDict()) {
    strg =  " initialize_from_dict - cannot initialize MiniFCAL part of LArCalorimeter dictionary ";
    if(m_msgSvc) {
      log << MSG::WARNING << strg << endmsg;
    }
    else {
      std::cout << strg << std::endl;
    }
    return (1);   
  }
  else {

    // Find value for the field LAr Calorimeter   
    const IdDictDictionary* atlasDict = dict_mgr.find_dictionary ("ATLAS"); 
    int larField   = -1;
    if (atlasDict->get_label_value("subdet", "LArCalorimeter", larField)) {
      std::stringstream strm ;
      strm <<  atlasDict->m_name ;
      strg = "Could not get value for label 'LArCalorimeter' of field 'subdet' in dictionary " 
      + strm.str();
      if(m_msgSvc) {
	log << MSG::ERROR << strg << endmsg;
      }
      else {
	std::cout << strg << std::endl;
      }
      return (1);
    }


    // Find value for the field LArMiniFCAL  - here the "part" has to be LArFCAL
    int larFcalField   = -1;
    if (dict()->get_label_value("part", "LArFCAL", larFcalField)) {
      std::stringstream strm ;
      strm <<  atlasDict->m_name ;
      strg = "Could not get value for label 'LArMiniFCAL' of field 'part' in dictionary " 
      + strm.str();
      if(m_msgSvc) {
	log << MSG::ERROR << strg << endmsg;
      }
      else {
	std::cout << strg << std::endl;
      }
      return (1);
    }

    // Set up id for region and range prefix

    // RDS: need to find the range for MiniFCAL module 0, which includes
    // both pos/neg endcaps
    ExpandedIdentifier region_id;
    region_id.add(larField);
    region_id.add(larFcalField);
    Range prefix;
    std::string str = "lar_mini_fcal";
    m_full_channel_range = dict()->build_multirange(region_id, str, prefix);
    m_full_module_range = dict()->build_multirange(region_id, str, prefix, "module");
    m_full_depth_range = dict()->build_multirange(region_id, str, prefix, "depth-mfcal");

    if (!m_quiet) {
      std::string strg0 = "initialize_from_dict : " ;
      std::string strg1 = " channel range -> " + (std::string)m_full_channel_range;
      std::string strg2 = " module range -> "  + (std::string)m_full_module_range;
      std::string strg2a= " depth range -> "   + (std::string)m_full_depth_range;
      if(m_msgSvc) {
        log << MSG::DEBUG << strg0 << endmsg;
        log << MSG::DEBUG << strg1 << endmsg;
        log << MSG::DEBUG << strg2 << endmsg;
        log << MSG::DEBUG << strg2a<< endmsg;
      }
      else {
        std::cout << strg0 << std::endl;
        std::cout << strg1 << std::endl;
        std::cout << strg2 << std::endl;
        std::cout << strg2a<< std::endl;
      }
    }

    // Setup the hash tables
    if(init_hashes()) return (1);

    // initilize m_two_sym_sides
    m_two_sym_sides = ( dictionaryVersion() == "fullAtlas" );

    // collect the regions to later access deta/dphi etc.
    if (fill_vec_of_dict_regions()) return 1;

    // added nov 2005 to speed up CaloNoiseTool initialisation
    m_vecOfPhiMin.resize(regions().hash_max());
    for (unsigned int i = 1; i < regions().hash_max(); ++i) {
      Identifier modId = module_id(i); 
      m_vecOfPhiMin[i] = phi_min_init(modId);
    }

    // Setup hash tables for finding neighbors
    if(m_do_neighbours) {
      if(init_neighbors(dict_mgr)) return (1);     
    }

  }

  m_is_initialized = true;
  return 0;

}

void LArMiniFCAL_ID::module_id_checks ( int pos_neg, int /*module*/ ) const
{
	
  // Check that id is within allowed range
  // Fill expanded id
  ExpandedIdentifier expId(lar_fcal_exp());
  //expId << pos_neg << module;
  expId << pos_neg << 0;

  if (!m_full_module_range.match(expId)) { 
    std::string errorMessage = "LArMiniFCAL_ID::module_id() result is not OK: ID, range = "
      + std::string(expId) + " , " + (std::string)m_full_module_range;
    throw LArID_Exception(errorMessage , 6);
  }
}

void LArMiniFCAL_ID::channel_id_checks ( int pos_neg, int /*module*/, int depth, int eta, int phi) const
{
	
  // Check that id is within allowed range
  // Fill expanded id
  ExpandedIdentifier expId(lar_fcal_exp());
  //expId << pos_neg << module << depth << eta << phi ;
  expId << pos_neg << 0 << depth << eta << phi ;

  if (!m_full_channel_range.match(expId)) { 
    std::string errorMessage = "LArMiniFCAL_ID::channel_id() result is not OK: ID, range = "
      + std::string(expId) + " , " + (std::string)m_full_channel_range;
    throw LArID_Exception(errorMessage , 10);
  }
}

void   LArMiniFCAL_ID::channel_id_checks   (const Identifier moduleId, int depth,
				        int eta, int phi) const
{
  // Check that id is within allowed range
  // Fill expanded id
  ExpandedIdentifier expId; 

  IdContext context = module_context();
  if (get_expanded_id(moduleId, expId, &context)) {
    std::string errorMessage = "LArMiniFCAL_ID::channel_id(modId) result is not OK: ID = "
      + show_to_string(moduleId) ;
    throw LArID_Exception(errorMessage , 10);
  }

  expId << depth << eta << phi ;

  if (!m_full_channel_range.match(expId)) { 
    std::string errorMessage = "LArMiniFCAL_ID::channel_id(modId) result is not OK: ID, range = "
      + std::string(expId) + " , " + (std::string)m_full_channel_range;
    throw LArID_Exception(errorMessage , 10);
  }
}

int  LArMiniFCAL_ID::get_expanded_id  (const Identifier& id, ExpandedIdentifier& exp_id, const IdContext* context) const
{
    // We assume that the context is >= region
    exp_id.clear();
    exp_id << lar_field_value()
      	   << lar_fcal_field_value()
	   << pos_neg(id)
      	   << module(id);
    if (context && context->end_index() >= m_DEPTH_INDEX) {
      exp_id << depth(id);
      if (context->end_index() >= m_ETA_INDEX) {
        exp_id << eta(id);
        if (context->end_index() >= m_PHI_INDEX) {
          exp_id << phi(id);
        }
      }
    }
    return 0;
}

int         LArMiniFCAL_ID::initLevelsFromDict(void) 
{
  MsgStream log(m_msgSvc, "LArMiniFCAL_ID" );

  if(!dict()) {
    std::string strg = "initLevelsFromDict - dictionary NOT initialized ";
    if(m_msgSvc) {
      log << MSG::ERROR << strg << endmsg;
    }
    else {
      std::cout << strg << std::endl;
    }
    return (1);
  }

  // Find out which identifier field corresponds to each level.

  m_fcal_region_index = 999 ;
  m_LAR_INDEX         = 999 ;
  m_MiniFCAL_INDEX    = 999 ;
  m_POSNEG_INDEX      = 999 ;
  m_MODULE_INDEX      = 999 ;
  m_DEPTH_INDEX       = 999 ;
  m_ETA_INDEX         = 999 ;
  m_PHI_INDEX         = 999 ;

  IdDictField* field = dict()->find_field("subdet") ;
  if (field) {
    m_LAR_INDEX = field->m_index ;
  }
  else {
    std::string strg = "initLevelsFromDict - unable to find 'subdet' field ";
    if(m_msgSvc) {
      log << MSG::ERROR << strg << endmsg;
    }
    else {
      std::cout << strg << std::endl;
    }
    return (1);
  }

  field = dict()->find_field("part") ;
  if (field) {
    m_MiniFCAL_INDEX = field->m_index ;
  }
  else {
    std::string strg = "initLevelsFromDict - unable to find 'part' field ";
    if(m_msgSvc) {
      log << MSG::ERROR << strg << endmsg;
    }
    else {
      std::cout << strg << std::endl;
    }
    return (1);
  }

  field = dict()->find_field("barrel-endcap") ;
  if (field) {
    m_POSNEG_INDEX = field->m_index ;
  }
  else {
    std::string strg = "initLevelsFromDict - unable to find 'barrel-endcap' field ";
    if(m_msgSvc) {
      log << MSG::ERROR << strg << endmsg;
    }
    else {
      std::cout << strg << std::endl;
    }
    return (1);
  }
  
  field = dict()->find_field("module") ;
  if (field) {
    m_MODULE_INDEX = field->m_index ;
  }
  else {
    std::string strg = "initLevelsFromDict - unable to find 'module' field ";
    if(m_msgSvc) {
      log << MSG::ERROR << strg << endmsg;
    }
    else {
      std::cout << strg << std::endl;
    }
    return (1);
  }

  
  field = dict()->find_field("depth-mfcal") ;
  if (field) {
    m_DEPTH_INDEX = field->m_index ;
  }
  else {
    std::string strg = "initLevelsFromDict - unable to find 'depth' field ";
    if(m_msgSvc) {
      log << MSG::WARNING << strg << endmsg;
    }
    else {
      std::cout << strg << std::endl;
    }
    return (1);
  }
  
  
  field = dict()->find_field("eta-mfcal") ;
  if (field) {
    m_ETA_INDEX = field->m_index ;
  }
  else {
    std::string strg = "initLevelsFromDict - unable to find 'eta' field ";
    if(m_msgSvc) {
      log << MSG::ERROR << strg << endmsg;
    }
    else {
      std::cout << strg << std::endl;
    }
    return (1);
  }
  
  field = dict()->find_field("phi-mfcal") ;
  if (field) {
    m_PHI_INDEX = field->m_index ;
  }
  else {
    std::string strg = "initLevelsFromDict - unable to find 'phi' field ";
    if(m_msgSvc) {
      log << MSG::ERROR << strg << endmsg;
    }
    else {
      std::cout << strg << std::endl;
    }
    return (1);
  }
  
  /*
  std::cout << "LArMiniFCAL_ID::initLevelsFromDict - found levels " << std::endl ;
  std::cout << "part           " << m_MiniFCAL_INDEX     << std::endl ;
  std::cout << "pos-neg        " << m_POSNEG_INDEX   << std::endl ;
  std::cout << "module         " << m_MODULE_INDEX   << std::endl ;
  std::cout << "eta            " << m_ETA_INDEX      << std::endl ;
  std::cout << "phi            " << m_PHI_INDEX      << std::endl ;
  */

  // Save index to a MiniFCAL region for unpacking

  // Fill expanded identifier with id for module 0 of the MiniFCAL,
  // i.e. add on module == 0 to the id below

  ExpandedIdentifier id(lar_fcal_exp());
  if (dict()->find_region(id,m_fcal_region_index)){  
    std::stringstream strm ;
    strm <<  m_fcal_region_index ;
    std::string strg = "initLevelsFromDict - unable to find fcal region index: id, reg "  
      +  (std::string)id + strm.str();
    if(m_msgSvc) {
      log << MSG::ERROR << strg << endmsg;
    }
    else {
      std::cout << strg << std::endl;
    }
    return (1);
  }

   // Set the field implementations


  IdDictDictionary::regions_const_it rit; 
  for (rit = dict()->m_regions.begin (); rit != dict()->m_regions.end (); ++rit) { 
    const IdDictRegion& region = *(*rit);
    if (region.m_group=="lar_mini_fcal") {m_minifcal_region_index = region.m_index; break;}
  }
  
  const IdDictRegion& region = *dict()->m_regions[m_minifcal_region_index];

  m_lar_impl      = region.m_implementation[m_LAR_INDEX]; 
  m_fcal_impl     = region.m_implementation[m_MiniFCAL_INDEX]; 
  m_pn_impl       = region.m_implementation[m_POSNEG_INDEX]; 
  m_module_impl   = region.m_implementation[m_MODULE_INDEX]; 
  m_depth_impl    = region.m_implementation[m_DEPTH_INDEX]; 
  m_eta_impl      = region.m_implementation[m_ETA_INDEX]; 
  m_phi_impl      = region.m_implementation[m_PHI_INDEX];

  if (!m_quiet) {
    if(m_msgSvc) {
      log << MSG::DEBUG << "decode index and bit fields for each level: " << endmsg;
      log << MSG::DEBUG << "lar   "   << m_lar_impl.show_to_string()    << endmsg;
      log << MSG::DEBUG << "fcal  "   << m_fcal_impl.show_to_string()   << endmsg;
      log << MSG::DEBUG << "pn    "   << m_pn_impl.show_to_string()     << endmsg;
      log << MSG::DEBUG << "mod   "   << m_module_impl.show_to_string() << endmsg;
      log << MSG::DEBUG << "depth "   << m_depth_impl.show_to_string()  << endmsg;
      log << MSG::DEBUG << "eta   "   << m_eta_impl.show_to_string()    << endmsg;
      log << MSG::DEBUG << "phi   "   << m_phi_impl.show_to_string()    << endmsg;
    }
    else {
      std::cout << "decode index and bit fields for each level: " << std::endl;
      std::cout << "lar   "  << m_lar_impl.show_to_string()    << std::endl;
      std::cout << "fcal  "  << m_fcal_impl.show_to_string()   << std::endl;
      std::cout << "pn    "  << m_pn_impl.show_to_string()     << std::endl;
      std::cout << "mod   "  << m_module_impl.show_to_string() << std::endl;
      std::cout << "depth "  << m_depth_impl.show_to_string()  << std::endl;
      std::cout << "eta   "  << m_eta_impl.show_to_string()    << std::endl;
      std::cout << "phi   "  << m_phi_impl.show_to_string()    << std::endl;
    }
  }


  return(0) ;
}

int         LArMiniFCAL_ID::init_hashes(void) 
{
  if (channels().init (*this, "channels",
                       m_full_channel_range,
                       &LArMiniFCAL_ID::channel_id,
                       m_PHI_INDEX))
    return 1;
  if (regions().init (*this, "regions",
                      m_full_module_range,
                      &LArMiniFCAL_ID::module_id,
                      m_MODULE_INDEX))
    return 1;

  return (0);
}


int         LArMiniFCAL_ID::init_neighbors_from_file(const std::string& filename, std::vector<std::set<IdentifierHash> > & vec)
{
  MsgStream log(m_msgSvc, "LArMiniFCAL_ID" );

  if(m_msgSvc) {
    log << MSG::DEBUG << "init_neighbors_from_file" << endmsg;
  }
  else {
    std::cout << "LArMiniFCAL_ID::init_neighbors_from_file " << std::endl;
  }
  // Find the full path to filename:
  std::string file = PathResolver::find_file (filename, "DATAPATH");
  log << MSG::INFO << "Reading file " << file << endmsg;

  std::ifstream fin;
  if (file != "") {
    fin.open(file.c_str());
  }
  else {
    if(m_msgSvc) {
      log << MSG::ERROR << "Could not find input file " << filename << endmsg;
    }
    else {
      std::cout << "LArMiniFCAL_ID::Could not find input file " << filename <<  std::endl;
    }
    return 1;
  }
  if (fin.bad()) {
    if(m_msgSvc) {
      log << MSG::ERROR << "Could not open file " << file << endmsg;
    }
    else {
      std::cout << "LArMiniFCAL_ID::Could not open file " << file << std::endl;
    }
    return 1;
  }

  //
  // Parse the input file
  //

  vec.resize(channels().hash_max());
  
  char aLine[1024];
  std::string sLine;
  bool isComment = true;
  char AorC,dot;
  int isamp,idepth, iphi,ieta;

  while( isComment ) {
    sLine.resize( 0 );
    do {  
      fin.getline(aLine,sizeof(aLine)-1);
      sLine = std::string(aLine);
    } while (sLine == "" && !fin.eof());
    isComment = ( sLine.find("#") != std::string::npos );
  }
  do {
    while ( sLine == "" && !fin.eof()) {
      fin.getline(aLine,sizeof(aLine)-1);
      sLine = std::string(aLine);
    }
    std::istringstream es( sLine.c_str() );

    if ( es >> AorC >> isamp >> dot >> idepth >> dot >> iphi >> dot >> ieta ) {
      Identifier thisCell = channel_id((AorC=='A'?2:-2),isamp,idepth,ieta,iphi);
      IdentifierHash ic = channel_hash(thisCell);
      while ( es >> AorC >> isamp >> dot >> idepth >> dot >> iphi >> dot >> ieta ) {
        if (!ic.is_valid()) continue;
	Identifier nextCell = channel_id((AorC=='A'?2:-2),isamp,idepth,ieta,iphi);
	IdentifierHash inext = channel_hash(nextCell);
	vec[ic].insert(vec[ic].end(),inext);
      }
      sLine.resize(0);
    } 
  }while (!fin.eof()) ;
  fin.close();

  return 0;
}

int         LArMiniFCAL_ID::init_neighbors(const IdDictMgr& dict_mgr) 
{
    MsgStream log(m_msgSvc, "LArMiniFCAL_ID" );

    if(m_msgSvc) {
        log << MSG::DEBUG << "init_neighbors" << endmsg;
    }
    else {
        std::cout << "LArMiniFCAL_ID::init_neighbors " << std::endl;
    }

    int status;
    std::string f2d,f3dnext,f3dprev;

    f2d     = dict_mgr.find_metadata("FCAL2DNEIGHBORS");
    f3dnext = dict_mgr.find_metadata("FCAL3DNEIGHBORSNEXT");
    f3dprev = dict_mgr.find_metadata("FCAL3DNEIGHBORSPREV");
    if (!f2d.size() || !f3dnext.size() || !f3dprev.size()) {
        if(m_msgSvc) {
            log << MSG::ERROR << "init_neighbours: cannot find neighbours files: " 
                << " f2d: " << f2d << " f3dnext: " << f3dnext << " f3dprev: " << f3dprev
                << endmsg;
        }
        else {
            std::cout << "LArMiniFCAL_ID::init_neighbours cannot find neighbours files: " 
                      << " f2d: " << f2d << " f3dnext: " << f3dnext << " f3dprev: " << f3dprev
                      << std::endl;
        }
        throw std::runtime_error("LArMiniFCAL_ID::init_neighbours: Cannot find the FCAL Neighbour file names");
    }
    
    status = init_neighbors_2d(f2d);
    if ( status == 0 ) 
        status = init_neighbors_3d_next(f3dnext);
    if ( status == 0 ) 
        status = init_neighbors_3d_prev(f3dprev);
    
    if ( status == 0 ) 
        return (0);
    else 
        return (1);
}



