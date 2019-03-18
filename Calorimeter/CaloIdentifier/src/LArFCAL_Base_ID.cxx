/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#include "CaloIdentifier/LArFCAL_Base_ID.h"
#include "IdDict/IdDictDefs.h"
#include "PathResolver/PathResolver.h"

#include "CxxUtils/StrFormat.h"
#include <iostream>
#include <fstream>

using CxxUtils::strformat;

#define MAX_BUFFER_LEN 1024


LArFCAL_Base_ID::LArFCAL_Base_ID(const std::string& name, bool supercell) 
  : CaloIDHelper (name),
    m_slar (supercell ? 1 : 0)
{
  m_fcal_region_index = 0;
  m_LAR_INDEX = 999;
  m_SLAR_INDEX = 999;
  m_FCAL_INDEX = 999;
  m_POSNEG_INDEX = 999;
  m_MODULE_INDEX = 999; 
  m_ETA_INDEX = 999;
  m_PHI_INDEX = 999;
  m_two_sym_sides = 1;
}


bool LArFCAL_Base_ID::is_supercell (const Identifier id)const
{
  return(m_slar_impl.unpack(id)) != 0;
}

int LArFCAL_Base_ID::eta_min(const Identifier modId) const
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

int LArFCAL_Base_ID::eta_max(const Identifier modId) const
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

int LArFCAL_Base_ID::phi_min_init(const Identifier modId) const
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

int LArFCAL_Base_ID::phi_max(const Identifier modId) const
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

IdContext	
LArFCAL_Base_ID::module_context 		(void) const
{
  return region_context();
}

int  LArFCAL_Base_ID::initialize_base_from_dictionary (const IdDictMgr& dict_mgr,
						       const std::string& group_name)
/*=================================================================*/
{
  MsgStream log(m_msgSvc, "LArFCAL_Base_ID" );

  std::string strg =  "initialize_from_dictionary";
  log << MSG::DEBUG << strg << endmsg;

  // Check whether this helper should be reinitialized
  if (!reinitialize(dict_mgr)) {
    if(m_msgSvc)log << MSG::DEBUG << "Request to reinitialize not satisfied - tags have not changed" << endmsg;
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

  // Initialize the field indices
  if (initLevelsFromDict(group_name)) return (1);

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


  // Find value for the field LArFCAL
    int larFcalField   = -1;
    if (dict()->get_label_value("part", "LArFCAL", larFcalField)) {
      std::stringstream strm ;
      strm <<  atlasDict->m_name ;
      strg = "Could not get value for label 'LArFCAL' of field 'part' in dictionary " 
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

    ExpandedIdentifier region_id;
    region_id.add(larField);
    region_id.add(larFcalField);
    Range prefix;
    m_full_channel_range = dict()->build_multirange(region_id, group_name, prefix);
    m_full_module_range = dict()->build_multirange(region_id, group_name, prefix, "module");

    if (!m_quiet) {
      std::string strg0 = " initialize_from_dict : " ;
      std::string strg1 = " channel range -> " + (std::string)m_full_channel_range;
      std::string strg2 = " module range -> "  + (std::string)m_full_module_range;
      if(m_msgSvc) {
        log << MSG::DEBUG << strg0 << endmsg;
        log << MSG::DEBUG << strg1 << endmsg;
        log << MSG::DEBUG << strg2 << endmsg;
      }
      else {
        std::cout << strg0 << std::endl;
        std::cout << strg1 << std::endl;
        std::cout << strg2 << std::endl;
      }
    }

    // Setup the hash tables
    if(init_hashes()) return (1);

    // initilize m_two_sym_sides
    m_two_sym_sides = ( dictionaryVersion() == "fullAtlas" );

    if (fill_vec_of_dict_regions (group_name)) return 1;

    // added nov 2005 to speed up CaloNoiseTool initialisation
    m_vecOfPhiMin.resize(regions().hash_max());
    for (unsigned int i = 1; i < regions().hash_max(); ++i) {
      Identifier modId = module_id(i); 
      m_vecOfPhiMin[i] = phi_min_init(modId);
    }
        
  // Setup hash tables for finding neighbours
  if(m_do_neighbours) {
    if(init_neighbours(dict_mgr)) return (1);     
  }
  
  return 0;
    
}

void LArFCAL_Base_ID::module_id_checks ( int pos_neg, int module ) const
{
	
  // Check that id is within allowed range
  // Fill expanded id
  ExpandedIdentifier expId(lar_fcal_exp());
  expId << pos_neg << module;

  if (!m_full_module_range.match(expId)) { 
    std::string errorMessage = "LArFCAL_Base_ID::module_id() result is not OK: ID, range = "
      + std::string(expId) + " , " + (std::string)m_full_module_range;
    throw LArID_Exception(errorMessage , 6);
  }
}

void LArFCAL_Base_ID::channel_id_checks ( int pos_neg, int module, int eta, int phi) const
{
	
  // Check that id is within allowed range
  // Fill expanded id
  ExpandedIdentifier expId(lar_fcal_exp());
  expId << pos_neg << module << eta << phi << m_slar ;

  if (!m_full_channel_range.match(expId)) { 
    std::string errorMessage = "LArFCAL_Base_ID::channel_id() result is not OK: ID, range = "
      + std::string(expId) + " , " + (std::string)m_full_channel_range;
    throw LArID_Exception(errorMessage , 10);
  }
}

void   LArFCAL_Base_ID::channel_id_checks   (const Identifier moduleId,
				        int eta, int phi) const
{
  // Check that id is within allowed range
  // Fill expanded id
  ExpandedIdentifier expId; 

  IdContext context = module_context();
  if (get_expanded_id(moduleId, expId, &context)) {
    std::string errorMessage = "LArFCAL_Base_ID::channel_id(modId) result is not OK: ID = "
      + show_to_string(moduleId) ;
    throw LArID_Exception(errorMessage , 10);
  }

  expId << eta << phi << m_slar ;

  if (!m_full_channel_range.match(expId)) { 
    std::string errorMessage = "LArFCAL_Base_ID::channel_id(modId) result is not OK: ID, range = "
      + std::string(expId) + " , " + (std::string)m_full_channel_range;
    throw LArID_Exception(errorMessage , 10);
  }
}

int  LArFCAL_Base_ID::get_expanded_id  (const Identifier& id, ExpandedIdentifier& exp_id, const IdContext* context) const
{
    // We assume that the context is >= region
    exp_id.clear();
    exp_id << lar_field_value()
      	   << lar_fcal_field_value()
	   << pos_neg(id)
	   << module(id);
    if(context && context->end_index() >= m_ETA_INDEX) {
	exp_id << eta(id);
	if(context->end_index() >= m_PHI_INDEX) {
	    exp_id << phi(id);
	    if ( context->end_index() >= m_SLAR_INDEX) {
              exp_id << (unsigned)is_supercell(id);
            }
	}
    }
    return (0);
}

int         LArFCAL_Base_ID::initLevelsFromDict(const std::string& /*group_name*/)
{
  MsgStream log(m_msgSvc, "LArFCAL_Base_ID" );

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
  m_LAR_INDEX        = 999 ;
  m_FCAL_INDEX       = 999 ;
  m_POSNEG_INDEX     = 999 ;
  m_MODULE_INDEX     = 999 ;
  m_ETA_INDEX        = 999 ;
  m_PHI_INDEX        = 999 ;
  m_SLAR_INDEX       = 999 ;

  // Save index to a FCAL region for unpacking
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
    m_FCAL_INDEX = field->m_index ;
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
    if(dictionaryVersion() != "H8TestBeam" ) {
      if(m_msgSvc) {
	log << MSG::ERROR << strg << endmsg;
      }
      else {
	std::cout << strg << std::endl;
      }
    }
    return (1);
  }

  field = dict()->find_field("eta-fcal") ;
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
  
  field = dict()->find_field("phi-fcal") ;
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
  
  field = dict()->find_field("is-slar-fcal") ;
  if (field) {
    m_SLAR_INDEX = field->m_index ;
  }
  else {
    if(m_msgSvc) {
      log << MSG::ERROR << "initLevelsFromDict - unable to find 'is-slar-fcal' field "
          << endmsg;
    }
    else {
      std::cout << "LArFCAL_Base_ID::initLevelsFromDict - unable to find 'is-slar-fcal' field "
                << std::endl ;
    }
    return (1);
  }

  // Set the field implementations

  const IdDictRegion& region = *dict()->m_regions[m_fcal_region_index];

  /*
  std::cout << "LArFCAL_Base_ID::initLevelsFromDict - found levels " << std::endl ;
  std::cout << "part           " << m_FCAL_INDEX     << std::endl ;
  std::cout << "pos-neg        " << m_POSNEG_INDEX   << std::endl ;
  std::cout << "module         " << m_MODULE_INDEX   << std::endl ;
  std::cout << "eta            " << m_ETA_INDEX      << std::endl ;
  std::cout << "phi            " << m_PHI_INDEX      << std::endl ;
  */

  m_lar_impl      = region.m_implementation[m_LAR_INDEX]; 
  m_fcal_impl      = region.m_implementation[m_FCAL_INDEX]; 
  m_pn_impl       = region.m_implementation[m_POSNEG_INDEX]; 
  m_module_impl   = region.m_implementation[m_MODULE_INDEX]; 
  m_eta_impl      = region.m_implementation[m_ETA_INDEX]; 
  m_phi_impl      = region.m_implementation[m_PHI_INDEX]; 
  m_slar_impl     = region.m_implementation[m_SLAR_INDEX]; 

  if (!m_quiet) {
    if(m_msgSvc) {
      log << MSG::DEBUG << "decode index and bit fields for each level: " << endmsg;
      log << MSG::DEBUG << "lar  "  << m_lar_impl.show_to_string() << endmsg;
      log << MSG::DEBUG << "fcal "  << m_fcal_impl.show_to_string() << endmsg;
      log << MSG::DEBUG << "pn   "  << m_pn_impl.show_to_string() << endmsg;
      log << MSG::DEBUG << "mod  "  << m_module_impl.show_to_string() << endmsg;
      log << MSG::DEBUG << "eta  "  << m_eta_impl.show_to_string() << endmsg;
      log << MSG::DEBUG << "phi  "  << m_phi_impl.show_to_string() << endmsg;
      log << MSG::DEBUG << "is-slar  "  << m_slar_impl.show_to_string() << endmsg;
    }
    else {
      std::cout << "decode index and bit fields for each level: " << std::endl;
      std::cout << "lar  "  << m_lar_impl.show_to_string() << std::endl;
      std::cout << "fcal "  << m_fcal_impl.show_to_string() << std::endl;
      std::cout << "pn   "  << m_pn_impl.show_to_string() << std::endl;
      std::cout << "mod  "  << m_module_impl.show_to_string() << std::endl;
      std::cout << "eta  "  << m_eta_impl.show_to_string() << std::endl;
      std::cout << "phi  "  << m_phi_impl.show_to_string() << std::endl;
      std::cout << "is-slar  "  << m_slar_impl.show_to_string() << std::endl;
    }
  }


  return(0) ;
}

int         LArFCAL_Base_ID::init_hashes(void) 
{
  if (channels().init (*this, "channels",
                       m_full_channel_range,
                       &LArFCAL_Base_ID::channel_id,
                       m_SLAR_INDEX))
    return 1;
  if (regions().init (*this, "regions",
                      m_full_module_range,
                      &LArFCAL_Base_ID::module_id,
                      m_MODULE_INDEX))
    return 1;

  return (0);
}

int   LArFCAL_Base_ID::get_neighbours(const IdentifierHash id, const LArNeighbours::neighbourOption& option, 
				 std::vector<IdentifierHash>& neighbourList) const
{
  int result = 1; 

  neighbourList.clear();

  if(!m_do_neighbours) {
    if(m_msgSvc) {
      MsgStream log(m_msgSvc, "LArFCAL_Base_ID" );
      log << MSG::WARNING << "neighbours not initialized !!! returning empty list" << endmsg;
    }
    else {
      std::cout << " neighbours not initialized !!! returning empty list " << std::endl;
    }
    return result;
  }

  if(id>=channel_hash_max()) {
    if(m_msgSvc) {
      MsgStream log(m_msgSvc, "LArFCAL_Base_ID" );
      log << MSG::WARNING << "neighbours requested for  non-existing channel -- id/max " << id << "/"
          << channel_hash_max() << endmsg;
    }
    else {
      std::cout << " neighbours requested for non-existing channel -- id/max " << id << "/"
                << channel_hash_max() << std::endl;
    }
    return result;
  }

  if( dictionaryVersion() == "fullAtlas" || dictionaryVersion() == "H6TestBeam" ) {

    if ( (option & LArNeighbours::all2D) == LArNeighbours::all2D ){
      if ( m_neighbours_2d_vec[(unsigned int)id].size() > 0 ) { 
	neighbourList.insert(neighbourList.end(),
			     m_neighbours_2d_vec[(unsigned int)id].begin(),
			     m_neighbours_2d_vec[(unsigned int)id].end());
      }
    }
    
    if ( (option & LArNeighbours::prevInSamp) ){
      if ( m_neighbours_3d_prev_vec[(unsigned int)id].size() > 0 ) { 
	neighbourList.insert(neighbourList.end(),
			     m_neighbours_3d_prev_vec[(unsigned int)id].begin(),
			     m_neighbours_3d_prev_vec[(unsigned int)id].end());
      }
    }
    
    if ( (option & LArNeighbours::nextInSamp) ){
      if ( m_neighbours_3d_next_vec[(unsigned int)id].size() > 0 ) { 
	neighbourList.insert(neighbourList.end(),
			     m_neighbours_3d_next_vec[(unsigned int)id].begin(),
			     m_neighbours_3d_next_vec[(unsigned int)id].end());
      }
    }
    
  } else {
    if(m_msgSvc) {
      MsgStream log(m_msgSvc, "LArFCAL_Base_ID" );
      log << MSG::WARNING << " NO FCAL neighbours (yet) in the context of " << dictionaryVersion() << endmsg;
    }
    else {
      std::cout << " NO FCAL neighbours (yet) in the context of " << dictionaryVersion() << std::endl;
    }
  }
  return result;
}

int         LArFCAL_Base_ID::init_neighbours_from_file(const std::string& filename, std::vector<std::set<IdentifierHash> > & vec)
{
  MsgStream log(m_msgSvc, "LArFCAL_Base_ID" );

  log << MSG::DEBUG << "init_neighbours_from_file" << endmsg;
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
      std::cout << "LarFCal_Base_ID::Could not find input file " << filename <<  std::endl;
    }
    return 1;
  }
  if (fin.bad()) {
    if(m_msgSvc) {
      log << MSG::ERROR << "Could not open file " << file << endmsg;
    }
    else {
      std::cout << "LarFCal_Base_ID::Could not open file " << file << std::endl;
    }
    return 1;
  }

  //
  // Parse the input file
  //

  vec.resize(channel_hash_max());
  
  char aLine[MAX_BUFFER_LEN];
  std::string sLine;
  bool isComment = true;
  char AorC,dot;
  int isamp,iphi,ieta;

  while( isComment ) {
    sLine.resize( 0 );
    do {  
      fin.getline(aLine,sizeof(aLine)-1);
      sLine = std::string(aLine);
    } while (sLine == "" && !fin.eof());
    isComment = ( sLine.find("#") != std::string::npos );
  }
  do {
    unsigned int ic, inext;
    Identifier thisCell,nextCell;
    while ( sLine == "" && !fin.eof()) {
      fin.getline(aLine,sizeof(aLine)-1);
      sLine = std::string(aLine);
    }
    std::istringstream es( sLine.c_str() );

    if ( es >> AorC >> isamp >> dot >> iphi >> dot >> ieta ) {
      thisCell = channel_id((AorC=='A'||AorC=='S'?2:-2),isamp,ieta,iphi);
      ic = channel_hash(thisCell);
      while ( es >> AorC >> isamp >> dot >> iphi >> dot >> ieta ) {
	nextCell = channel_id((AorC=='A'||AorC=='S'?2:-2),isamp,ieta,iphi);
	inext = channel_hash(nextCell);
	vec[ic].insert(vec[ic].end(),inext);
      }
      sLine.resize(0);
    } 
  }while (!fin.eof()) ;
  fin.close();

  return 0;
}

int
LArFCAL_Base_ID::init_neighbours(const IdDictMgr& dict_mgr) 
{
    
    MsgStream log(m_msgSvc, "LArFCAL_Base_ID" );
    if(m_msgSvc) {
        log << MSG::DEBUG << "init_neighbours" << endmsg;
    }
    else {
        std::cout << "LarFCal_Base_ID::init_neighbours " << std::endl;
    }

    int status;
    std::string f2d,f3dnext,f3dprev;

    if ( m_slar ) {
      f2d     = "FCalSuperCells2DNeighborsNew-April2014.txt";
      f3dnext = "FCalSuperCells3DNeighborsNextNew-April2014.txt"; 
      f3dprev = "FCalSuperCells3DNeighborsPrevNew-April2014.txt"; 
    }
    else {
      f2d     = dict_mgr.find_metadata("FCAL2DNEIGHBORS");
      f3dnext = dict_mgr.find_metadata("FCAL3DNEIGHBORSNEXT");
      f3dprev = dict_mgr.find_metadata("FCAL3DNEIGHBORSPREV");
    }
    if (!f2d.size() || !f3dnext.size() || !f3dprev.size()) {
        if(m_msgSvc) {
            log << MSG::ERROR << "init_neighbours: cannot find neighbours files: " 
                << " f2d: " << f2d << " f3dnext: " << f3dnext << " f3dprev: " << f3dprev
                << endmsg;
        }
        else {
            std::cout << "LarFCal_Base_ID::init_neighbours cannot find neighbours files: " 
                      << " f2d: " << f2d << " f3dnext: " << f3dnext << " f3dprev: " << f3dprev
                      << std::endl;
        }
        throw std::runtime_error("LArFCAL_Base_ID::init_neighbours: Cannot find the FCAL Neighbour file names");
    }
    
    status = init_neighbours_2d(f2d);
    if ( status == 0 ) 
        status = init_neighbours_3d_next(f3dnext);
    if ( status == 0 ) 
        status = init_neighbours_3d_prev(f3dprev);

    if(m_msgSvc) {
        log << MSG::DEBUG << "init_neighbours status: " << status << endmsg;
    }
    else {
        std::cout << "LarFCal_Base_ID::init_neighbours status: " << status << std::endl;
    }
    
    if ( status == 0 ) 
        return (0);
    else 
        return (1);
}

