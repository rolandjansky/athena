/*
   Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
 */

/***************************************************************************
   Inner Detector identifier package
   -------------------------------------------
***************************************************************************/


#include "GaudiKernel/MsgStream.h"

#include "InDetIdentifier/SCT_ID.h"
#include "Identifier/IdentifierHash.h"
#include "IdDict/IdDictDefs.h"
#include <set>
#include <algorithm>
#include <iostream>

namespace{
  const IdentifierHash invalidHash;
  const std::array<IdentifierHash, 5> invalidHashes{invalidHash, invalidHash, invalidHash,
  invalidHash, invalidHash};
  const std::function< IdentifierHash(const IdentifierHash &)>
  invalidHashFunc = ([](const IdentifierHash &){return IdentifierHash{};});
}



SCT_ID::SCT_ID(void)
  :
  m_neighboursByEta{invalidHashFunc, invalidHashFunc, invalidHashFunc , invalidHashFunc, invalidHashFunc},
  m_dict(nullptr),
  m_wafer_hash_max(0),
  m_strip_hash_max(0),
  m_barrel_field(0),
  m_hasRows(false) {
  
}

void
SCT_ID::wafer_id_checks(int barrel_ec,
                        int layer_disk,
                        int phi_module,
                        int eta_module,
                        int side) const {
  // Check that id is within allowed range

  // Fill expanded id
  ExpandedIdentifier id;

  id << indet_field_value() << sct_field_value()
     << barrel_ec << layer_disk << phi_module << eta_module << side;

  if (!m_full_wafer_range.match(id)) {  // module range check is sufficient
    std::string errMsg = " result is NOT ok. ID, range " + std::string(id)+std::string(m_full_wafer_range);
    localMessage(errMsg, __func__,  MSG::ERROR);
  }
}

void
SCT_ID::strip_id_checks(int barrel_ec,
                        int layer_disk,
                        int phi_module,
                        int eta_module,
                        int side,
                        int strip) const {
  // Check that id is within allowed range

  // Fill expanded id
  ExpandedIdentifier id;

  id << indet_field_value() << sct_field_value()
     << barrel_ec << layer_disk << phi_module << eta_module << side << strip;

  if (!m_full_strip_range.match(id)) {
    std::string errMsg = " result is NOT ok. ID, range " + std::string(id)+std::string(m_full_strip_range);
    localMessage(errMsg, __func__,  MSG::ERROR);
  }
}

void
SCT_ID::strip_id_checks( const ExpandedIdentifier & expId) const {
  if (not m_hasRows){
    strip_id_checks(expId[m_indices[BARREL_EC]],
                      expId[m_indices[LAYER_DISK]],
                      expId[m_indices[PHI]],
                      expId[m_indices[ETA]],
                      expId[m_indices[SIDE]],
                      expId[m_indices[STRIP]]);
  } else {
    strip_id_checks(expId[m_indices[BARREL_EC]],
                      expId[m_indices[LAYER_DISK]],
                      expId[m_indices[PHI]],
                      expId[m_indices[ETA]],
                      expId[m_indices[SIDE]],
                      expId[m_indices[ROW]],
                      expId[m_indices[STRIP]]);
  }
}

void
SCT_ID::strip_id_checks(int barrel_ec,
                        int layer_disk,
                        int phi_module,
                        int eta_module,
                        int side,
                        int row,
                        int strip) const {
  // Check that id is within allowed range

  // Fill expanded id
  ExpandedIdentifier id;

  id << indet_field_value() << sct_field_value()
     << barrel_ec << layer_disk << phi_module << eta_module << side << row << strip;


  if (!m_full_strip_range.match(id)) {
    std::string errMsg = " result is NOT ok. ID, range " + std::string(id)+std::string(m_full_strip_range);
    localMessage(errMsg, __func__,  MSG::ERROR);
  }
}

int
SCT_ID::getMaxField(const Identifier & id, const ExpandedIdIndices &fieldIndx) const{
  // get max from dictionary
  ExpandedIdentifier expId;
  int result(-999);
  const IdContext context = (fieldIndx == LAYER_DISK) ? wafer_context():IdContext(expId, 0, m_indices[LAYER_DISK]) ;
  get_expanded_id(id, expId, &context);
  const auto & useRange = (fieldIndx == STRIP)? m_full_strip_range : m_full_wafer_range;
  for (unsigned int i = 0; i != useRange.size(); ++i) {
    const Range& range = useRange[i];
    if (range.match(expId)) {
      const Range::field& thisField = range[m_indices[fieldIndx]];
      if (thisField.has_maximum()) {
        auto thisMax= thisField.get_maximum();
        if (fieldIndx == ETA or fieldIndx == STRIP){
          result = std::max(result, thisMax);
        } else {
          return thisMax;
        }
      }
    }
  }
  return result;  // default
}

int
SCT_ID::layer_disk_max(const Identifier& id) const {
  return getMaxField(id, LAYER_DISK);
}

int
SCT_ID::eta_module_min(const Identifier& id) const {
  // get min from dictionary - note that eta modules skip 0 for
  // sct, so we must search for absolute min
  ExpandedIdentifier expId;
  IdContext layer_context(expId, 0, m_indices[LAYER_DISK]);
  get_expanded_id(id, expId, &layer_context);
  int result = -999;
  for (unsigned int i = 0; i < m_full_wafer_range.size(); ++i) {
    const Range& range = m_full_wafer_range[i];
    if (range.match(expId)) {
      const Range::field& eta_field = range[m_indices[ETA]];
      if (eta_field.has_minimum()) {
        int etamin = eta_field.get_minimum();
        if (-999 == result) {
          result = etamin;
        } else {
          if (etamin < result) result = etamin;
        }
      }
    }
  }
  return(result);
}

int
SCT_ID::eta_module_max(const Identifier& id) const {
  return getMaxField(id, ETA);
}

int
SCT_ID::strip_max(const Identifier& id) const {
 return getMaxField(id, STRIP);
}

bool
SCT_ID::is_eta_module_min(const Identifier& id) const {
  // get min from dictionary
  return(eta_module(id) == eta_module_min(id));
}

bool
SCT_ID::is_eta_module_max(const Identifier& id)  const {
  // get max from dictionary
  return(eta_module(id) == eta_module_max(id));
}

int
SCT_ID::phi_module_max(const Identifier& id) const {
  return getMaxField(id, PHI);
}

bool
SCT_ID::is_phi_module_max(const Identifier& id) const {
  // get max from dictionary
  return(phi_module(id) == phi_module_max(id));
}

int
SCT_ID::initialize_from_dictionary(const IdDictMgr& dict_mgr) {
  localMessage("", __func__,  MSG::INFO);

  // Check whether this helper should be reinitialized
  if (!reinitialize(dict_mgr)) {
    localMessage("Request to reinitialize not satisfied - tags have not changed", __func__,  MSG::INFO);
    return(0);
  } else {
    localMessage("(Re)initialize", __func__,  MSG::DEBUG);
  }

  // init base object
  if (AtlasDetectorID::initialize_from_dictionary(dict_mgr)) return(2);

  // Register version of InnerDetector dictionary
  if (register_dict_tag(dict_mgr, "InnerDetector")) return(1);

  m_dict = dict_mgr.find_dictionary("InnerDetector");
  if (!m_dict) {
    localMessage( " - cannot access InnerDetector dictionary ",__func__, MSG::ERROR);
    return 1;
  }

  // Initialize the field indices
  if (initLevelsFromDict()) return(1);

  //
  // Set barrel field for testing is_barrel
  //
  int barrel_value;
  if (m_dict->get_label_value("barrel_endcap", "barrel", barrel_value)) {
    const std::string errMsg = "Could not get value for label 'barrel' of field 'barrel_endcap' in dictionary " + m_dict->m_name;
    localMessage(errMsg, __func__, MSG::ERROR);
    return(1);
  }
  m_barrel_field.clear();
  m_barrel_field.add_value(barrel_value);
  std::string dbgMsg = "Set barrel field values: " + std::string(m_barrel_field);
  localMessage(dbgMsg, __func__, MSG::DEBUG);
  


  //
  // Build multirange for the valid set of identifiers
  //


  // Find value for the field InnerDetector
  const IdDictDictionary* atlasDict = dict_mgr.find_dictionary("ATLAS");
  int inDetField = -1;
  if (atlasDict->get_label_value("subdet", "InnerDetector", inDetField)) {
    const std::string errMsg = "Could not get value for label 'InnerDetector' of field 'subdet' in dictionary " + atlasDict->m_name;
    localMessage(errMsg, __func__, MSG::ERROR);
    return(1);
  }

  // Find value for the field SCT
  int sctField = -1;
  if (m_dict->get_label_value("part", "SCT", sctField)) {
    const std::string errMsg= "Could not get value for label 'SCT' of field 'part' in dictionary " + m_dict->m_name;
    localMessage(errMsg, __func__, MSG::ERROR);
    return(1);
  }
  dbgMsg = "Found field values: SCT " + std::to_string(sctField);
  localMessage(dbgMsg, __func__, MSG::DEBUG);

  // Set up id for region and range prefix
  ExpandedIdentifier region_id;
  region_id.add(inDetField);
  region_id.add(sctField);
  Range prefix;
  m_full_wafer_range = m_dict->build_multirange(region_id, prefix, "side");
  m_full_strip_range = m_dict->build_multirange(region_id, prefix);

  // Setup the hash tables
  if (init_hashes()) return(1);

  // Setup hash tables for finding neighbors
  if (init_neighbors()) return(1);
  dbgMsg = "Wafer range -> " + std::string( m_full_wafer_range) + "\n";
  dbgMsg += "Strip range -> " + std::string(m_full_strip_range);
  localMessage(dbgMsg, __func__, MSG::DEBUG);
  ///insert now valid calls to the m_neighboursByEta
  m_neighboursByEta = {
    [this](const IdentifierHash & id){return this->get_other_side(id);},
    [this](const IdentifierHash & id){return this->get_prev_in_eta(id);},
    [this](const IdentifierHash & id){return this->get_next_in_eta(id);},
    [this](const IdentifierHash & id){return this->get_prev_in_phi(id);},
    [this](const IdentifierHash & id){return this->get_next_in_phi(id);}
  };
  return 0;
}

int
SCT_ID::init_hashes(void) {
  //
  // create a vector(s) to retrieve the hashes for compact ids. For
  // the moment, we implement a hash for wafers but NOT for strips
  // (too many)
  //

  // wafer hash
  m_wafer_hash_max = m_full_wafer_range.cardinality();
  m_wafer_vec.resize(m_wafer_hash_max);
  unsigned int nids = 0;
  std::set<Identifier> ids;
  for (unsigned int i = 0; i < m_full_wafer_range.size(); ++i) {
    const Range& range = m_full_wafer_range[i];
    Range::const_identifier_factory first = range.factory_begin();
    Range::const_identifier_factory last = range.factory_end();
    for (; first != last; ++first) {
      const ExpandedIdentifier& exp_id = (*first);
      Identifier id = wafer_id(exp_id);
      if (!(ids.insert(id)).second) {
        const std::string errMsg = "Error: duplicated id for wafer id. nid " + std::to_string(nids) + " compact id " + id.getString() + " id " + std::string(exp_id);
        localMessage(errMsg, __func__, MSG::ERROR);
        return(1);
      }
      nids++;
    }
  }
  if (ids.size() != m_wafer_hash_max) {
    const std::string errMsg = "Error: set size NOT EQUAL to hash max. size  " + std::to_string(ids.size()) + " hash max " +std::to_string( m_wafer_hash_max);
    localMessage(errMsg, __func__, MSG::ERROR);
    return(1);
  }

  nids = 0;
  std::set<Identifier>::const_iterator first = ids.begin();
  std::set<Identifier>::const_iterator last = ids.end();
  for (; first != last && nids < m_wafer_vec.size(); ++first) {
    m_wafer_vec[nids] = (*first);
    nids++;
  }

  // strip hash - we do not keep a vec for the strips - too large
  m_strip_hash_max = m_full_strip_range.cardinality();

  return(0);
}

IdentifierHash 
SCT_ID::get_prev_in_phi(const IdentifierHash& id) const{
  return nextInSequence(id, m_prev_phi_wafer_vec);
}
/// Next wafer hash in phi 
IdentifierHash 
SCT_ID::get_next_in_phi(const IdentifierHash& id) const{
  return nextInSequence(id, m_next_phi_wafer_vec);
}
/// Previous wafer hash in eta 
IdentifierHash 
SCT_ID::get_prev_in_eta(const IdentifierHash& id) const{
  return nextInSequence(id, m_prev_eta_wafer_vec);
}
/// Next wafer hash in eta 
IdentifierHash 
SCT_ID::get_next_in_eta(const IdentifierHash& id) const{
  return nextInSequence(id, m_next_eta_wafer_vec);
}
/// Wafer hash on other side
IdentifierHash 
SCT_ID::get_other_side(const IdentifierHash& hashId) const{
  if (m_dict) {
    // get max from dictionary
    Identifier id;
    const IdContext & wafer_context1 = wafer_context();
    if (!get_id(hashId, id, &wafer_context1)) {
      return( side(id) ? hashId - 1 : hashId + 1);
    }
  }
  return IdentifierHash{};
}
//
std::array<std::function< IdentifierHash(const IdentifierHash &)>, 5 >
SCT_ID::neighbour_calls_by_eta() const{
  return m_neighboursByEta;
}


int
SCT_ID::get_prev_in_phi(const IdentifierHash& id, IdentifierHash& prev) const {
  const auto result = nextInSequence(id, m_prev_phi_wafer_vec);
  if (result.is_valid()) {
    prev = result;
    return 0;
  }
  return 1;
}

int
SCT_ID::get_next_in_phi(const IdentifierHash& id, IdentifierHash& next) const {
  const auto result = nextInSequence(id, m_next_phi_wafer_vec);
  if (result.is_valid()) {
    next = result;
    return 0;
  }
  return 1;
}

int
SCT_ID::get_prev_in_eta(const IdentifierHash& id, IdentifierHash& prev) const {
  const auto result = nextInSequence(id, m_prev_eta_wafer_vec);
  if (result.is_valid()) {
    prev = result;
    return 0;
  }
  return 1;
}

int
SCT_ID::get_next_in_eta(const IdentifierHash& id, IdentifierHash& next) const {
  const auto result = nextInSequence(id, m_next_eta_wafer_vec);
  if (result.is_valid()) {
    next = result;
    return 0;
  }
  return 1;
}

int
SCT_ID::get_other_side(const IdentifierHash& hashId, IdentifierHash& other) const {
  if (m_dict) {
    // get max from dictionary
    Identifier id;
    IdContext wafer_context1 = wafer_context();
    if (!get_id(hashId, id, &wafer_context1)) {
      other = side(id) ? hashId - 1 : hashId + 1;
      return(0);
    }
  }
  return(1);
}

 Identifier 
 SCT_ID::wafer_id(const ExpandedIdentifier & expId) const{
   return wafer_id(expId[m_indices[BARREL_EC]],
                   expId[m_indices[LAYER_DISK]],
                   expId[m_indices[PHI]],
                   expId[m_indices[ETA]],
                   expId[m_indices[SIDE]]);
 }

int
SCT_ID::init_neighbors(void) {
  //
  // create a vector(s) to retrieve the hashes for compact ids for
  // wafer neighbors.
  //

  localMessage("", __func__, MSG::DEBUG);
  const IdentifierHash invalidHash;
  m_prev_phi_wafer_vec.resize(m_wafer_hash_max, invalidHash);
  m_next_phi_wafer_vec.resize(m_wafer_hash_max, invalidHash);
  m_prev_eta_wafer_vec.resize(m_wafer_hash_max, invalidHash);
  m_next_eta_wafer_vec.resize(m_wafer_hash_max, invalidHash);

  for (unsigned int i = 0; i < m_full_wafer_range.size(); ++i) {
    const Range& range = m_full_wafer_range[i];
    const Range::field& phi_field = range[m_indices[PHI]];
    const Range::field& eta_field = range[m_indices[ETA]];
    Range::const_identifier_factory first = range.factory_begin();
    Range::const_identifier_factory last = range.factory_end();
    for (; first != last; ++first) {
      const ExpandedIdentifier& exp_id = (*first);
      ExpandedIdentifier::element_type previous_phi;
      ExpandedIdentifier::element_type next_phi;
      ExpandedIdentifier::element_type previous_eta;
      ExpandedIdentifier::element_type next_eta;
      bool pphi = phi_field.get_previous(exp_id[m_indices[PHI]], previous_phi);
      bool nphi = phi_field.get_next(exp_id[m_indices[PHI]], next_phi);
      bool peta = eta_field.get_previous(exp_id[m_indices[ETA]], previous_eta);
      bool neta = eta_field.get_next(exp_id[m_indices[ETA]], next_eta);

      IdContext wcontext = wafer_context();

      // First get primary hash id
      IdentifierHash hash_id;
      Identifier id = wafer_id(exp_id);
      if (get_hash(id, hash_id, &wcontext)) {
        const std::string errMsg = "- unable to get hash, exp/compact " + show_to_string(id, &wcontext) + std::string(m_full_wafer_range);
        localMessage(errMsg, __func__, MSG::ERROR);
        return(1);
      }

      // index for the subsequent arrays
      unsigned short index = hash_id;
      assert(hash_id < m_prev_phi_wafer_vec.size());
      assert(hash_id < m_next_phi_wafer_vec.size());
      assert(hash_id < m_prev_eta_wafer_vec.size());
      assert(hash_id < m_next_eta_wafer_vec.size());

      if (pphi) {
        // Get previous phi hash id
        ExpandedIdentifier expId = exp_id;
        expId[m_indices[PHI]] = previous_phi;
        Identifier id = wafer_id(expId);
        if (get_hash(id, hash_id, &wcontext)) {        
          const std::string errMsg = "- unable to get previous phi hash, exp/compact " + id.getString();
          localMessage(errMsg, __func__, MSG::ERROR);
          return(1);
        }
        m_prev_phi_wafer_vec[index] = hash_id;
      }

      if (nphi) {
        // Get next phi hash id
        ExpandedIdentifier expId = exp_id;
        expId[m_indices[PHI]] = next_phi;
        Identifier id = wafer_id(expId);
        if (get_hash(id, hash_id, &wcontext)) {
          const std::string errMsg = "- unable to get next phi hash, exp/compact " + id.getString();
          localMessage(errMsg, __func__, MSG::ERROR);
          return(1);
        }
        m_next_phi_wafer_vec[index] = hash_id;
      }

      if (peta) {
        // Get previous eta hash id
        ExpandedIdentifier expId = exp_id;
        expId[m_indices[ETA]] = previous_eta;
        Identifier id = wafer_id(expId);
        if (get_hash(id, hash_id, &wcontext)) {
          const std::string errMsg = "- unable to get previous eta hash, exp/compact " + id.getString();
          localMessage(errMsg, __func__, MSG::ERROR);
          return(1);
        }
        m_prev_eta_wafer_vec[index] = hash_id;
      }

      if (neta) {
        // Get next eta hash id
        ExpandedIdentifier expId = exp_id;
        expId[m_indices[ETA]] = next_eta;
        Identifier id = wafer_id(expId);
        if (get_hash(id, hash_id, &wcontext)) {
          const std::string errMsg = "- unable to get next eta hash, exp/compact " + id.getString();
          localMessage(errMsg, __func__, MSG::ERROR);
          return(1);
        }
        m_next_eta_wafer_vec[index] = hash_id;
      }
    }
  }
  return(0);
}

int
SCT_ID::initLevelsFromDict() {
  if (!m_dict) {
    localMessage("- dictionary NOT initialized", __func__, MSG::ERROR);
    return(1);
  }
  // Find out which identifier field corresponds to each level. Use
  // names to find each field/leve.
  m_indices.fill(999);
  m_hasRows = false;
  // Save index to a SCT region for unpacking
  ExpandedIdentifier id;
  id << indet_field_value() << sct_field_value();
  
 
  if (m_dict->find_region(id, m_sct_region_index)) {
    const std::string errMsg  = "- unable to find sct region index: id, reg " + std::string(id) + " " + std::to_string(m_sct_region_index);
    localMessage(errMsg, __func__, MSG::ERROR);
    return(1);
  }
  
  auto findField = [this](const std::string &name, const size_t indx){
    IdDictField* pField = m_dict->find_field(name);
    if (pField) {
      m_indices[indx] = pField->m_index;
      return true;
    } 
    const auto lvl = (indx == ROW) ? MSG::DEBUG : MSG::ERROR;
    localMessage("- unable to find '" + name +"' field ", __func__, lvl);
    return false;
  };
  
  // Find an SCT region
  if (not findField("subdet", INDET)) return 1;
  if (not findField("part", SCT)) return 1;
  if (not findField("barrel_endcap", BARREL_EC)) return 1;
  if (not findField("layer", LAYER_DISK)) return 1;
  if (not findField("phi_module", PHI)) return 1;
  if (not findField("eta_module", ETA)) return 1;
  if (not findField("side", SIDE)) return 1;
  m_hasRows = (findField("row", ROW));
  if (not findField("strip", STRIP)) return 1;

  // Set the field implementations: for bec, lay/disk, eta/phi mod

  const IdDictRegion& region = *m_dict->m_regions[m_sct_region_index];

  m_indet_impl = region.m_implementation[m_indices[INDET]];
  m_sct_impl = region.m_implementation[m_indices[SCT]];
  m_bec_impl = region.m_implementation[m_indices[BARREL_EC]];
  m_lay_disk_impl = region.m_implementation[m_indices[LAYER_DISK]];
  m_phi_mod_impl = region.m_implementation[m_indices[PHI]];
  m_eta_mod_impl = region.m_implementation[m_indices[ETA]];
  m_side_impl = region.m_implementation[m_indices[SIDE]];
  if (m_hasRows) {
    m_row_impl = region.m_implementation[m_indices[ROW]];
  }
  m_strip_impl = region.m_implementation[m_indices[STRIP]];
  localMessage("decode index and bit fields for each level: ", __func__, MSG::DEBUG);
  localMessage("indet    " + m_indet_impl.show_to_string(), __func__, MSG::DEBUG);
  localMessage("sct      " + m_sct_impl.show_to_string(), __func__, MSG::DEBUG);
  localMessage("bec      " + m_bec_impl.show_to_string(), __func__, MSG::DEBUG);
  localMessage("lay_disk " + m_lay_disk_impl.show_to_string(), __func__, MSG::DEBUG);
  localMessage("phi_mod  " + m_phi_mod_impl.show_to_string(), __func__, MSG::DEBUG);
  localMessage("eta_mod  " + m_eta_mod_impl.show_to_string(), __func__, MSG::DEBUG);
  localMessage("side     " + m_side_impl.show_to_string(), __func__, MSG::DEBUG);
  if (m_hasRows) {
    localMessage("row     " + m_row_impl.show_to_string(), __func__, MSG::DEBUG);
  }
  localMessage("strip    " + m_strip_impl.show_to_string(), __func__, MSG::DEBUG);

 

  return(0);
}

SCT_ID::size_type
SCT_ID::wafer_hash_max(void) const {
  return m_wafer_hash_max;
}

SCT_ID::size_type
SCT_ID::strip_hash_max(void) const {
  return m_strip_hash_max;
}

SCT_ID::const_id_iterator SCT_ID::wafer_begin(void) const {
  return(m_wafer_vec.begin());
}

SCT_ID::const_id_iterator SCT_ID::wafer_end(void) const {
  return(m_wafer_vec.end());
}

SCT_ID::const_expanded_id_iterator SCT_ID::strip_begin(void) const {
  return(m_full_strip_range.factory_begin());
}

SCT_ID::const_expanded_id_iterator SCT_ID::strip_end(void) const {
  return(m_full_strip_range.factory_end());
}

// From hash get Identifier
int
SCT_ID::get_id(const IdentifierHash& hash_id,
               Identifier& id,
               const IdContext* context) const {
  int result = 1;
  id.clear();
  size_t begin = (context) ? context->begin_index() : 0;
  // cannot get hash if end is 0:
  size_t end = (context) ? context->end_index()  : 0;
  if (0 == begin) {
    // No hashes yet for ids with prefixes
    if (m_indices[SIDE] == end) {
      if (hash_id < m_wafer_vec.size()) {
        id = m_wafer_vec[hash_id];
        result = 0;
      }
    } else if (m_indices[STRIP] == end) {
      // Do not know how to calculate strip id from hash yet!!
      localMessage( "Do not know how to calculate strip id from hash yet!!", __func__, MSG::ERROR );
    }
  }
  return(result);
}

void
SCT_ID::get_expanded_id(const Identifier& id,
                        ExpandedIdentifier& exp_id,
                        const IdContext* context) const {
  exp_id.clear();
  exp_id << indet_field_value()
         << sct_field_value()
         << barrel_ec(id)
         << layer_disk(id)
         << phi_module(id)
         << eta_module(id)
         << side(id);
  if (!context || context->end_index() == m_indices[STRIP]) {
    if (m_hasRows) {
      exp_id << row(id) << strip(id);
    } else {
      exp_id << strip(id);
    }
  }
}

int
SCT_ID::get_hash(const Identifier& id,
                 IdentifierHash& hash_id,
                 const IdContext* context) const {
  // Get the hash code from either a vec (for wafers) or calculate
  // it (strips). For the former, we convert to compact and call
  // get_hash again. For the latter, we calculate the hash from the
  // Identifier.

  int result = 1;

  hash_id = 0;
  size_t begin = (context) ? context->begin_index() : 0;
  size_t end = (context) ? context->end_index()  : 0;
  if (0 == begin) {
    // No hashes yet for ids with prefixes
    if (m_indices[SIDE] == end) {
      hash_id = wafer_hash(id);
      if (hash_id.is_valid()) result = 0;
    } else if (context && context->end_index() == m_indices[STRIP]) {
      // Must calculate for strip hash
      ExpandedIdentifier new_id;
      get_expanded_id(id, new_id);
      hash_id = m_full_strip_range.cardinalityUpTo(new_id);
      result = 0;
    }
  }
  return(result);
}
void
SCT_ID::localMessage(const std::string & msgTxt, const std::string &func, const MSG::Level & lvl) const{
  const std::array<std::string, MSG::NUM_LEVELS> prefix={"","VERBOSE ", "DEBUG ", "INFO ", "WARNING ", "ERROR ", "FATAL "," "};
  if (m_msgSvc){
    MsgStream log(m_msgSvc, "SCT_ID");
    log << lvl << msgTxt << endmsg;
  } else {
    #ifdef NDEBUG
    if (lvl > MSG::DEBUG) std::cout<<prefix[lvl]<<"SCT_ID::"<<func<<" "<<msgTxt<<std::endl;
    #else
    std::cout<<prefix[lvl]<<"SCT_ID::"<<func<<" "<<msgTxt<<std::endl;
    #endif
  }
  
}

//all neighbours: opposite and then eta direction first
std::array<IdentifierHash, 5>
SCT_ID::neighbours_by_eta(const IdentifierHash & idh) const{
  if (size_type index = idh; index<m_wafer_hash_max) return std::array<IdentifierHash, 5>{ 
    get_other_side(idh), 
    m_prev_eta_wafer_vec[index], m_next_eta_wafer_vec[index],
    m_prev_phi_wafer_vec[index], m_next_phi_wafer_vec[index]
  };
  else return  invalidHashes;
}
  
//all neighbours: opposite and then phi direction first
std::array<IdentifierHash, 5>
SCT_ID::neighbours_by_phi(const IdentifierHash & idh) const{
  if (size_type index = idh; index<m_wafer_hash_max) return std::array<IdentifierHash, 5>{ 
    get_other_side(idh), 
    m_prev_phi_wafer_vec[index], m_next_phi_wafer_vec[index],
    m_prev_eta_wafer_vec[index], m_next_eta_wafer_vec[index]
  };
  else return  invalidHashes;
}
