/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

#include "MuonIdHelpers/MuonIdHelper.h"

#include "GaudiKernel/Bootstrap.h"
#include "GaudiKernel/IMessageSvc.h"
#include "GaudiKernel/ISvcLocator.h"
#include "GaudiKernel/MsgStream.h"

const std::string MuonIdHelper::BAD_NAME = "UNKNOWN";

MuonIdHelper::MuonIdHelper(const std::string& logName):
    AtlasDetectorID(),
    m_logName{logName}{
    if (m_logName.empty()) m_logName = "MuonIdHelper";
    
    ISvcLocator* svcLocator = Gaudi::svcLocator();
    StatusCode sc = svcLocator->service("MessageSvc", m_msgSvc);
    if (sc.isFailure()) std::cout << "Fail to locate Message Service" << std::endl;
}

int MuonIdHelper::initialize_from_dictionary(const IdDictMgr& dict_mgr) {
    // Check whether this helper should be reinitialized
    MsgStream log(m_msgSvc, m_logName);
    if (!reinitialize(dict_mgr)) {
        if (m_msgSvc) log << MSG::INFO << "Request to reinitialize not satisfied - tags have not changed" << endmsg;
        return (0);
    } else {
        if (m_msgSvc) log << MSG::DEBUG << "(Re)initialize" << endmsg;
    }

    // init base object
    if (AtlasDetectorID::initialize_from_dictionary(dict_mgr)) return (1);

    // Register version of the MuonSpectrometer dictionary
    if (register_dict_tag(dict_mgr, "MuonSpectrometer")) return (1);

    m_dict = dict_mgr.find_dictionary("MuonSpectrometer");
    if (!m_dict) {
        if (m_msgSvc) { log << MSG::ERROR << " initialize_from_dict - cannot access MuonSpectrometer dictionary " << endmsg; }
        return 1;
    }

    // Initialize some of the field indices
    if (initLevelsFromDict()) return (1);

    //
    // Build multirange for the valid set of identifiers
    //

    // Find value for the field MuonSpectrometer
    int muonField = -1;
    const IdDictDictionary* atlasDict = dict_mgr.find_dictionary("ATLAS");
    if (atlasDict->get_label_value("subdet", "MuonSpectrometer", muonField)) {
        if (m_msgSvc) {
            log << MSG::ERROR << "Could not get value for label 'MuonSpectrometer' of field 'subdet' in dictionary " << atlasDict->m_name
                << endmsg;
        }
        return (1);
    }

    // Build MultiRange down to "technology" for all (muon) regions
    ExpandedIdentifier region_id;
    region_id.add(muonField);
    Range prefix;
    MultiRange muon_range = m_dict->build_multirange(region_id, prefix, "technology");
    if (muon_range.size() > 0) {
        if (m_msgSvc) {
            log << MSG::INFO << "MultiRange built successfully to Technology "
                << "MultiRange size is " << muon_range.size() << endmsg;
        }
    } else {
        if (m_msgSvc) { log << MSG::ERROR << "Muon MultiRange is empty" << endmsg; }
    }

    return 0;
}

// From hash get Identifier
int MuonIdHelper::get_id(const IdentifierHash& hash_id, Identifier& id, const IdContext* context) const {
    int result = 1;
    id.clear();

    size_t begin = (context) ? context->begin_index() : 0;
    // cannot get hash if end is 0:
    size_t end = (context) ? context->end_index() : 0;
    if (0 == begin) {
        // No hashes yet for ids with prefixes
        if (m_MODULE_INDEX == end) {
            if (hash_id < (unsigned int)(m_module_vec.end() - m_module_vec.begin())) {
                id = m_module_vec[hash_id];
                result = 0;
            }
        } else if (m_DETECTORELEMENT_INDEX == end) {
            if (hash_id < (unsigned int)(m_detectorElement_vec.end() - m_detectorElement_vec.begin())) {
                id = m_detectorElement_vec[hash_id];
                result = 0;
            }
        } else if (m_CHANNEL_INDEX == end) {
            if (hash_id < (unsigned int)(m_channel_vec.end() - m_channel_vec.begin())) {
                id = m_channel_vec[hash_id];
                result = 0;
            }
        }
    }
    return (result);
}

int MuonIdHelper::get_module_hash(const Identifier& id, IdentifierHash& hash_id) const {
    // by binary search - overwritten in the derived classes
    IdContext context = module_context();
    hash_id = UINT_MAX;
    size_t begin = context.begin_index();
    size_t end = context.end_index();

    if (0 == begin) {
        // No hashes yet for ids with prefixes
        if (m_MODULE_INDEX == end) {
            id_vec_it it = std::lower_bound(m_module_vec.begin(), m_module_vec.end(), id);
            if ((it != m_module_vec.end()) && (*it == id)) {
                hash_id = it - m_module_vec.begin();
                return 0;
            }
        }
    }
    if (m_msgSvc) {
        MsgStream log(m_msgSvc, m_logName);
        log << MSG::WARNING << "MuonIdHelper::get_module_hash(): Could not determine hash for identifier " << id.get_compact() << endmsg;
    }
    return 1;
}

int MuonIdHelper::get_detectorElement_hash(const Identifier& id, IdentifierHash& hash_id) const {
    // by binary search - overwritten in the derived classes
    hash_id = UINT_MAX;
    IdContext context = detectorElement_context();
    size_t begin = context.begin_index();
    size_t end = context.end_index();
    if (0 == begin) {
        if (m_DETECTORELEMENT_INDEX == end) {
            id_vec_it it = std::lower_bound(m_detectorElement_vec.begin(), m_detectorElement_vec.end(), id);
            if ((it != m_detectorElement_vec.end()) && (*it == id)) {
                hash_id = it - m_detectorElement_vec.begin();
                return 0;
            }
        }
    }
    if (m_msgSvc) {
        MsgStream log(m_msgSvc, m_logName);
        log << MSG::WARNING << "MuonIdHelper::get_detectorElement_hash(): Could not determine hash for identifier " << id.get_compact()
            << endmsg;
    }
    return 1;
}

int MuonIdHelper::get_channel_hash(const Identifier& id, IdentifierHash& hash_id) const {
    IdContext context = channel_context();
    return get_hash(id, hash_id, &context);
}

int MuonIdHelper::get_hash(const Identifier& id, IdentifierHash& hash_id, const IdContext* context) const {
    // Get the hash code from either a vec. We convert to compact
    // and call get_hash again. For the latter, we calculate the hash from the
    // Identifier.

    int result = 1;
    hash_id = 0;
    size_t begin = (context) ? context->begin_index() : 0;
    //    size_t end   = (context) ? context->end_index()  : 0;
    if (0 == begin) {
        // No hashes yet for ids with prefixes
        result = get_hash_calc(id, hash_id, context);
    }
    return (result);
}

int MuonIdHelper::get_expanded_id(const Identifier& id, ExpandedIdentifier& exp_id, const IdContext* context) const {
    return (get_expanded_id_calc(id, exp_id, context));
}

int MuonIdHelper::get_id(const ExpandedIdentifier& old_id, Identifier& new_id) const {
    // Copy old_id into new_id and get compact if possible
    int result = 0;
    new_id.clear();

    ExpandedIdentifier dummy_id;
    IdContext context(dummy_id, 0, old_id.fields() - 1);
    Identifier compact_id;
    if (!get_compact_id(old_id, compact_id, &context)) { new_id = compact_id; }
    return (result);
}

// From compact, get ExpandedIdentifier
int MuonIdHelper::get_expanded_id_calc(const Identifier& compact_id, ExpandedIdentifier& id, const IdContext* context) const {
    int result = 1;
    id.clear();
    if (m_dict) {
        // some preconditions in the case that the dictionary existd
        size_t begin = (context) ? context->begin_index() : 0;
        size_t end = (context) ? context->end_index() : m_CHANNEL_INDEX;
        assert(end <= m_CHANNEL_INDEX);

        if (0 == end) {
            result = 0;
        } else if (0 == begin) {
            ExpandedIdentifier empty;
            result = m_dict->unpack(compact_id, empty, end, id);
        } else {
            // Non-zero prefix - we assume that the prefix contains
            // the IdDet level
            result = m_dict->unpack(compact_id, context->prefix_id(), end, id);
        }
    }
    return result;
}

int MuonIdHelper::get_compact_id(const ExpandedIdentifier& id, Identifier& compact_id, const IdContext* context) const {
    // Get compact for all fields provided
    int result = 1;
    compact_id = (Identifier::value_type)0;
    int exp_id[10];

    for (size_t i = 0; i < id.fields(); i++) exp_id[i] = id[i];

    if (m_dict && id.fields() > 0) {
        size_t begin = (context) ? context->begin_index() : 0;
        size_t end = (context) ? context->end_index() : id.fields() - 1;
        result = m_dict->pack32(exp_id, begin, end, m_GROUP_INDEX, compact_id);
        // 	result = m_dict->pack32(id, begin, end, compact_id);
    }
    return (result);
}

int MuonIdHelper::get_hash_calc(const Identifier& compact_id, IdentifierHash& hash_id, const IdContext* context) const {
    // Get the hash code from vec (for wafers only).
    hash_id = UINT_MAX;
    size_t begin = (context) ? context->begin_index() : 0;
    size_t end = (context) ? context->end_index() : 0;

    if (0 == begin) {
        // No hashes yet for ids with prefixes
        if (m_MODULE_INDEX == end) {
            return get_module_hash(compact_id, hash_id);
        } else if (m_DETECTORELEMENT_INDEX == end) {
            return get_detectorElement_hash(compact_id, hash_id);
        } else if (m_CHANNEL_INDEX == end) {
            id_vec_it it = std::lower_bound(m_channel_vec.begin(), m_channel_vec.end(), compact_id);
            if ((it != m_channel_vec.end()) && (*it == compact_id)) {
                hash_id = it - m_channel_vec.begin();
                return 0;
            }
        }
    }
    if (m_msgSvc) {
        MsgStream log(m_msgSvc, m_logName);
        log << MSG::WARNING << "MuonIdHelper::get_hash_calc(): Could not determine hash for identifier " << compact_id.get_compact()
            << endmsg;
    }
    return 1;
}

int MuonIdHelper::initLevelsFromDict() {
    if (!m_dict) {
        if (m_msgSvc) {
            MsgStream log(m_msgSvc, m_logName);
            log << MSG::ERROR << " initLevelsFromDict - dictionary NOT initialized " << endmsg;
        }
        return (1);
    }

    // Find out which identifier field corresponds to each level. Use
    // names to find each field/leve.

    m_MUON_INDEX = 999;
    m_NAME_INDEX = 999;
    m_ETA_INDEX = 999;
    m_PHI_INDEX = 999;
    m_CHANNEL_INDEX = 999;

    // Save index to stations for unpacking
    ExpandedIdentifier id(muon_exp());
    if (m_dict->find_region(id, m_station_region_index)) {
        if (m_msgSvc) {
            MsgStream log(m_msgSvc, m_logName);
            log << MSG::ERROR << "initLevelsFromDict - unable to find a muon station index: id, reg"
                << " " << (std::string)id << " " << m_station_region_index << endmsg;
        }
        return (1);
    }

    // Find a Muon region
    IdDictField* field = m_dict->find_field("subdet");
    if (field) {
        m_MUON_INDEX = field->m_index;
    } else {
        if (m_msgSvc) {
            MsgStream log(m_msgSvc, m_logName);
            log << MSG::ERROR << "initLevelsFromDict - unable to find 'subdet' field " << endmsg;
        }
        return (1);
    }
    field = m_dict->find_field("stationName");
    if (field) {
        m_NAME_INDEX = field->m_index;

        if (m_stationIdxToNameMap.empty()) {
            // we only need to fill the vectors and sets once
            for (size_t i = 0; i < field->get_label_number(); i++) {
                std::string name = field->get_label(i);
                int index = (int)field->get_label_value(name);
                m_stationIndexMax = std::max(m_stationIndexMax, index);
                m_stationNameToIdxMap[name] = index;
                m_stationIdxToNameMap[index] = name;
                if (isStNameInTech(name)) m_stationInTech.insert(index);
                // all chambers starting with B are in the barrel except the BEE chambers
                if ('B' == name[0]) {
                    if (name[1] != 'E')
                        m_isBarrel.insert(index);
                    else
                        m_isSmall.insert(index);  // BEE is in the small sector
                    if ('G' == name[2] || 'F' == name[2]) { m_isSmall.insert(index); }
                } else if ('F' == name[2]) {
                    m_isForward.insert(index);
                }
                if ('S' == name[2]) { m_isSmall.insert(index); }
            }
        }

    } else {
        if (m_msgSvc) {
            MsgStream log(m_msgSvc, m_logName);
            log << MSG::ERROR << "initLevelsFromDict - unable to find 'stationName' field " << endmsg;
        }
        return (1);
    }
    field = m_dict->find_field("stationEta");
    if (field) {
        m_ETA_INDEX = field->m_index;
    } else {
        if (m_msgSvc) {
            MsgStream log(m_msgSvc, m_logName);
            log << MSG::ERROR << "initLevelsFromDict - unable to find 'stationEta' field " << endmsg;
        }
        return (1);
    }
    field = m_dict->find_field("stationPhi");
    if (field) {
        m_PHI_INDEX = field->m_index;
    } else {
        if (m_msgSvc) {
            MsgStream log(m_msgSvc, m_logName);
            log << MSG::ERROR << "initLevelsFromDict - unable to find 'stationPhi' field " << endmsg;
        }
        return (1);
    }
    field = m_dict->find_field("technology");
    if (field) {
        m_TECHNOLOGY_INDEX = field->m_index;

        if (m_technologyNameToIdxMap.empty()) {
            for (size_t i = 0; i < field->get_label_number(); ++i) {
                std::string name = field->get_label(i);
                int index = (int)field->get_label_value(name);
                m_technologyIndexMax = std::max(m_technologyIndexMax, index);
                m_technologyNameToIdxMap[name] = index;
                m_technologyIdxToNameMap[index] = name;               
            }
        }

    } else {
        if (m_msgSvc) {
            MsgStream log(m_msgSvc, m_logName);
            log << MSG::ERROR << "initLevelsFromDict - unable to find 'technology' field " << endmsg;
        }
        return (1);
    }
    m_MODULE_INDEX = m_TECHNOLOGY_INDEX;

    // Set the field implementations down to the technology
    const IdDictRegion& region = *m_dict->m_regions[m_station_region_index];
    m_muon_impl = region.m_implementation[m_MUON_INDEX];
    m_sta_impl = region.m_implementation[m_NAME_INDEX];

    // m_stationNameField = m_dict->find_field ("stationName"); // Philipp
    // m_technologyField  = m_dict->find_field ("technology"); // Philipp
    return (0);
}

int MuonIdHelper::init_hashes(void) {
    //
    // create a vector(s) to retrieve the hashes for compact ids. For
    // the moment, we implement a hash for modules
    //

    MsgStream log(m_msgSvc, m_logName);

    // module hash
    m_module_hash_max = m_full_module_range.cardinality();
    if (m_msgSvc) { log << MSG::INFO << "The element hash max is " << (int)m_module_hash_max << endmsg; }
    m_module_vec.resize(m_module_hash_max);
    IdContext context = module_context();
    unsigned int nids = 0;
    std::set<Identifier> ids;
    for (unsigned int i = 0; i < m_full_module_range.size(); ++i) {
        const Range& range = m_full_module_range[i];
        Range::const_identifier_factory first = range.factory_begin();
        Range::const_identifier_factory last = range.factory_end();
        for (; first != last; ++first) {
            Identifier id;
            get_id((*first), id);
            if (!(ids.insert(id)).second) {
                if (m_msgSvc) {
                    log << MSG::ERROR << "init_hashes "
                        << " Error: duplicated id for module id. nid " << (int)nids << " compact id " << id << " id " << endmsg;
                }
                return (1);
            }
            nids++;
        }
    }
    if (ids.size() != m_module_hash_max) {
        if (m_msgSvc) {
            log << MSG::ERROR << "init_hashes "
                << " Error: set size NOT EQUAL to element hash max. size " << ids.size() << " hash max " << m_module_hash_max << endmsg;
        }
        return (1);
    }

    nids = 0;
    std::set<Identifier>::const_iterator first = ids.begin();
    std::set<Identifier>::const_iterator last = ids.end();
    for (; first != last && nids < m_module_vec.size(); ++first) {
        m_module_vec[nids] = (*first);
        nids++;
    }
    // sort the vector of identifiers to be able to use std::lower_bound to find hashes
    std::sort(m_module_vec.begin(), m_module_vec.end());
    return (0);
}

int MuonIdHelper::init_detectorElement_hashes(void) {
    //
    // create a vector(s) to retrieve the hashes for compact ids. For
    // the moment, we implement a hash for readout channels
    //

    MsgStream log(m_msgSvc, m_logName);

    // detector element hash
    m_detectorElement_hash_max = m_full_detectorElement_range.cardinality();
    if (m_msgSvc) { log << MSG::INFO << "The detector element hash max is " << (int)m_detectorElement_hash_max << endmsg; }
    m_detectorElement_vec.resize(m_detectorElement_hash_max);
    IdContext context = detectorElement_context();
    unsigned int nids = 0;
    std::set<Identifier> ids;
    for (unsigned int i = 0; i < m_full_detectorElement_range.size(); ++i) {
        const Range& range = m_full_detectorElement_range[i];
        Range::const_identifier_factory first = range.factory_begin();
        Range::const_identifier_factory last = range.factory_end();
        for (; first != last; ++first) {
            Identifier id;
            get_id((*first), id);
            if (!(ids.insert(id)).second) {
                if (m_msgSvc) {
                    log << MSG::ERROR << "init_detectorElement_hashes "
                        << " Error: duplicated id for channel id. nid " << (int)nids << " compact id " << id << " id " << endmsg;
                }
                return (1);
            }
            nids++;
        }
    }
    if (ids.size() != m_detectorElement_hash_max) {
        if (m_msgSvc) {
            log << MSG::ERROR << "init_hashes "
                << " Error: set size NOT EQUAL to hash max. size " << ids.size() << " hash max " << m_detectorElement_hash_max << endmsg;
        }
        return (1);
    }

    nids = 0;
    std::set<Identifier>::const_iterator first = ids.begin();
    std::set<Identifier>::const_iterator last = ids.end();
    for (; first != last && nids < m_detectorElement_vec.size(); ++first) {
        m_detectorElement_vec[nids] = (*first);
        nids++;
    }
    // sort the vector of identifiers to be able to use std::lower_bound to find hashes
    std::sort(m_detectorElement_vec.begin(), m_detectorElement_vec.end());
    return (0);
}

int MuonIdHelper::init_channel_hashes(void) {
    //
    // create a vector(s) to retrieve the hashes for compact ids. For
    // the moment, we implement a hash for readout channels
    //

    MsgStream log(m_msgSvc, m_logName);

    // readout channel hash
    m_channel_hash_max = m_full_channel_range.cardinality();
    if (m_msgSvc) { log << MSG::INFO << "The channel hash max is " << (int)m_channel_hash_max << endmsg; }
    m_channel_vec.resize(m_channel_hash_max);
    IdContext context = channel_context();
    unsigned int nids = 0;
    std::set<Identifier> ids;
    for (unsigned int i = 0; i < m_full_channel_range.size(); ++i) {
        const Range& range = m_full_channel_range[i];
        Range::const_identifier_factory first = range.factory_begin();
        Range::const_identifier_factory last = range.factory_end();
        for (; first != last; ++first) {
            Identifier id;
            get_id((*first), id);

            if (!(ids.insert(id)).second) {
                if (m_msgSvc) {
                    log << MSG::ERROR << "init_channel_hashes "
                        << " Error: duplicated id for channel id. nid " << (int)nids << " compact id " << id << " id " << endmsg;
                }
                return (1);
            }
            m_channel_vec[nids] = id;
            nids++;
        }
    }
    if (ids.size() != m_channel_hash_max) {
        if (m_msgSvc) {
            log << MSG::ERROR << "init_hashes "
                << " Error: set size NOT EQUAL to hash max. size " << ids.size() << " hash max " << m_channel_hash_max << endmsg;
        }
        return (1);
    }
    // sort the vector of identifiers to be able to use std::lower_bound to find hashes
    std::sort(m_channel_vec.begin(), m_channel_vec.end());
    return (0);
}

int MuonIdHelper::get_prev_in_phi(const IdentifierHash& id, IdentifierHash& prev) const {
    unsigned short index = id;
    if (index < m_prev_phi_module_vec.size()) {
        if (m_prev_phi_module_vec[index] == NOT_VALID_HASH) return (1);
        prev = m_prev_phi_module_vec[index];
        return (0);
    }
    return (1);
}

int MuonIdHelper::get_next_in_phi(const IdentifierHash& id, IdentifierHash& next) const {
    unsigned short index = id;
    if (index < m_next_phi_module_vec.size()) {
        if (m_next_phi_module_vec[index] == NOT_VALID_HASH) return (1);
        next = m_next_phi_module_vec[index];
        return (0);
    }
    return (1);
}

int MuonIdHelper::get_prev_in_eta(const IdentifierHash& id, IdentifierHash& prev) const {
    unsigned short index = id;
    if (index < m_prev_eta_module_vec.size()) {
        if (m_prev_eta_module_vec[index] == NOT_VALID_HASH) return (1);
        prev = m_prev_eta_module_vec[index];
        return (0);
    }
    return (1);
}

int MuonIdHelper::get_next_in_eta(const IdentifierHash& id, IdentifierHash& next) const {
    unsigned short index = id;
    if (index < m_next_eta_module_vec.size()) {
        if (m_next_eta_module_vec[index] == NOT_VALID_HASH) return (1);
        next = m_next_eta_module_vec[index];
        return (0);
    }
    return (1);
}

int MuonIdHelper::init_neighbors(void) {
    //
    // create a vector(s) to retrieve the hashes for compact ids for
    // module neighbors.
    //

    MsgStream log(m_msgSvc, m_logName);

    if (m_msgSvc) { log << MSG::VERBOSE << "MuonIdHelper::init_neighbors " << endmsg; }

    m_prev_phi_module_vec.resize(m_module_hash_max, NOT_VALID_HASH);
    m_next_phi_module_vec.resize(m_module_hash_max, NOT_VALID_HASH);
    m_prev_eta_module_vec.resize(m_module_hash_max, NOT_VALID_HASH);
    m_next_eta_module_vec.resize(m_module_hash_max, NOT_VALID_HASH);

    for (unsigned int i = 0; i < m_full_module_range.size(); ++i) {
        const Range& range = m_full_module_range[i];
        const Range::field& phi_field = range[m_PHI_INDEX];
        const Range::field& eta_field = range[m_ETA_INDEX];
        Range::const_identifier_factory first = range.factory_begin();
        Range::const_identifier_factory last = range.factory_end();
        for (; first != last; ++first) {
            const ExpandedIdentifier& id = (*first);
            ExpandedIdentifier::element_type previous_phi;
            ExpandedIdentifier::element_type next_phi;
            ExpandedIdentifier::element_type previous_eta;
            ExpandedIdentifier::element_type next_eta;
            bool pphi = phi_field.get_previous(id[m_PHI_INDEX], previous_phi);
            bool nphi = phi_field.get_next(id[m_PHI_INDEX], next_phi);
            bool peta = eta_field.get_previous(id[m_ETA_INDEX], previous_eta);
            bool neta = eta_field.get_next(id[m_ETA_INDEX], next_eta);

            Identifier compact_id;
            IdContext wcontext = module_context();

            // First get primary hash id
            IdentifierHash hash_id;
            if (!get_compact_id(id, compact_id, &wcontext)) {
                // forward to compact -> hash
                if (get_hash(compact_id, hash_id, &wcontext)) {
                    if (m_msgSvc) {
                        log << MSG::WARNING << " MuonIdHelper::init_neighbors - unable to get hash, exp/compact ";
                        //		    id.show();
                        log << " " << std::hex << compact_id << std::dec << endmsg;
                    }
                    return (1);
                }
            } else {
                if (m_msgSvc) { log << MSG::WARNING << " MuonIdHelper::init_neighbors - unable to get compact, exp/compact " << endmsg; }
                //		id.show();

                return (1);
            }

            // index for the subsequent arrays
            unsigned short index = hash_id;
            assert(hash_id < m_prev_phi_module_vec.size());
            assert(hash_id < m_next_phi_module_vec.size());
            assert(hash_id < m_prev_eta_module_vec.size());
            assert(hash_id < m_next_eta_module_vec.size());

            if (pphi) {
                // Get previous phi hash id
                ExpandedIdentifier expId = id;
                expId[m_PHI_INDEX] = previous_phi;
                if (!get_compact_id(expId, compact_id, &wcontext)) {
                    // forward to compact -> hash
                    if (get_hash(compact_id, hash_id, &wcontext)) {
                        if (m_msgSvc) {
                            log << MSG::WARNING << " MuonIdHelper::init_neighbors - unable to get previous phi hash, exp/compact ";
                            //			id.show();
                            log << " " << std::hex << compact_id << std::dec << endmsg;
                        }
                        return (1);
                    }
                } else {
                    if (m_msgSvc) {
                        log << MSG::WARNING << " MuonIdHelper::init_neighbors - unable to get previous phi compact, exp/compact " << endmsg;
                    }
                    return (1);
                }
                m_prev_phi_module_vec[index] = hash_id;
            }

            if (nphi) {
                // Get next phi hash id
                ExpandedIdentifier expId = id;
                expId[m_PHI_INDEX] = next_phi;
                if (!get_compact_id(expId, compact_id, &wcontext)) {
                    // forward to compact -> hash
                    if (get_hash(compact_id, hash_id, &wcontext)) {
                        if (m_msgSvc) {
                            log << MSG::WARNING << " MuonIdHelper::init_neighbors - unable to get next phi hash, exp/compact ";
                            //		id.show();
                            log << " " << std::hex << compact_id << std::dec << endmsg;
                        }
                        return (1);
                    }
                } else {
                    if (m_msgSvc) {
                        log << MSG::WARNING << " MuonIdHelper::init_neighbors - unable to get next phi compact, exp/compact " << endmsg;
                    }
                    //		    id.show();
                    //		    std::cout << std::endl;
                    return (1);
                }
                m_next_phi_module_vec[index] = hash_id;
            }

            if (peta) {
                // Get previous eta hash id
                ExpandedIdentifier expId = id;
                expId[m_ETA_INDEX] = previous_eta;
                if (!get_compact_id(expId, compact_id, &wcontext)) {
                    // forward to compact -> hash
                    if (get_hash(compact_id, hash_id, &wcontext)) {
                        if (m_msgSvc) {
                            log << MSG::WARNING << " MuonIdHelper::init_neighbors - unable to get previous eta hash, exp/compact ";
                            log << " " << std::hex << compact_id << std::dec << endmsg;
                        }
                        return (1);
                    }
                } else {
                    if (m_msgSvc) {
                        log << MSG::WARNING << " MuonIdHelper::init_neighbors - unable to get previous eta compact, exp/compact " << endmsg;
                    }
                    return (1);
                }
                m_prev_eta_module_vec[index] = hash_id;
            }

            if (neta) {
                // Get next eta hash id
                ExpandedIdentifier expId = id;
                expId[m_ETA_INDEX] = next_eta;
                if (!get_compact_id(expId, compact_id, &wcontext)) {
                    // forward to compact -> hash
                    if (get_hash(compact_id, hash_id, &wcontext)) {
                        if (m_msgSvc) {
                            log << MSG::WARNING << " MuonIdHelper::init_neighbors - unable to get next eta hash, exp/compact ";
                            // id.show();
                            log << " " << std::hex << compact_id << std::dec << endmsg;
                        }
                        return (1);
                    }
                } else {
                    if (m_msgSvc) {
                        log << MSG::WARNING << " MuonIdHelper::init_neighbors - unable to get next eta compact, exp/compact " << endmsg;
                    }
                    return (1);
                }
                m_next_eta_module_vec[index] = hash_id;
            }
        }
    }
    // std::cout << "Finish init_neighors() ..." << std::endl;
    return (0);
}

void MuonIdHelper::test_module_packing(void) const {
    MsgStream log(m_msgSvc, m_logName);

    if (m_dict) {
        int nids = 0;
        IdContext context = module_context();
        const_id_iterator first = m_module_vec.begin();
        const_id_iterator last = m_module_vec.end();
        for (; first != last; ++first, ++nids) {
            Identifier compact = (*first);
            ExpandedIdentifier id;
            if (get_expanded_id(compact, id, &context)) {
                if (m_msgSvc) { log << MSG::ERROR << "test_module_packing: Unable to get expanded id. Compact id " << compact << endmsg; }
                continue;
            }
            Identifier new_compact;
            if (get_compact_id(id, new_compact, &context)) {
                if (m_msgSvc) {
                    log << MSG::ERROR << "test_module_packing: Unable to get compact id. Expanded id " << (std::string)(id) << endmsg;
                }
                continue;
            }
            if (compact != new_compact) {
                if (m_msgSvc) {
                    log << MSG::ERROR << "test_module_packing: new and old compacts not equal "
                        << "New/old/expanded ids " << new_compact << " " << compact << " " << (std::string)(id) << endmsg;
                }
                continue;
            }
        }

        if (m_msgSvc) { log << MSG::INFO << "test_module_packing: Successful tested " << nids << " ids. " << endmsg; }
    } else {
        if (m_msgSvc) { log << MSG::INFO << "Unable to test module is packing - no dictionary has been defined. " << endmsg; }
    }
}

void MuonIdHelper::test_id(const Identifier& id, const IdContext& context) const {
    Identifier compact = id;
    ExpandedIdentifier new_id;
    if (get_expanded_id(compact, new_id, &context)) {
        if (m_msgSvc) {
            MsgStream log(m_msgSvc, m_logName);
            log << MSG::ERROR << "Unable to get expanded id. Compact id " << compact << endmsg;
        }
        return;
    }
    Identifier new_compact;
    if (get_compact_id(new_id, new_compact, &context)) {
        if (m_msgSvc) {
            MsgStream log(m_msgSvc, m_logName);
            log << MSG::ERROR << "Unable to get compact id. Expanded id " << show_to_string(id) << endmsg;
        }
        return;
    }
    if (compact != new_compact) {
        if (m_msgSvc) {
            MsgStream log(m_msgSvc, m_logName);
            log << MSG::ERROR << "new and old compacts not equal. New/old/expanded ids " << new_compact << " " << compact << " "
                << show_to_string(id) << endmsg;
        }
        return;
    }
}

// Prepend the station ID

void MuonIdHelper::addStationID(Identifier& id, int stationName, int stationEta, int stationPhi, int technology) const {
    ExpandedIdentifier exp_id;
    IdContext muon_context(exp_id, 0, m_MUON_INDEX);
    if (get_expanded_id(id, exp_id, &muon_context)) {
        if (m_msgSvc) {
            MsgStream log(m_msgSvc, m_logName);
            log << MSG::ERROR << " MUON_ID result is NOT ok. MUON id " << show_to_string(id) << " Fields not appended " << endmsg;
        }
    } else {
        exp_id << stationName << stationEta << stationPhi << technology;
        get_id(exp_id, id);  // Fill output
    }
}

int MuonIdHelper::stationRegion(const Identifier& id) const {
    std::string name = stationNameString(stationName(id));

    if ('I' == name[1] || '4' == name[1]) return 0;
    if ('E' == name[1] || '1' == name[1]) return 1;
    if ('M' == name[1] || '2' == name[1]) return 2;
    if ('O' == name[1] || '3' == name[1]) return 3;
    if (name == "CSS" || name == "CSL") return 0;
    if (m_msgSvc) {
        MsgStream log(m_msgSvc, m_logName);
        log << MSG::ERROR << " MuonId::stationRegion / id = " << show_to_string(id) << " stationnamestring = " << name << endmsg;
    }
    std::abort();
    return -1;
}

/*******************************************************************************/
Identifier MuonIdHelper::muon() const { return AtlasDetectorID::muon(); }
/*******************************************************************************/
// Print method
void MuonIdHelper::print(const Identifier& id) const { AtlasDetectorID::print(id); }
/*******************************************************************************/
IdContext MuonIdHelper::technology_context(void) const {
    ExpandedIdentifier id;
    return (IdContext(id, 0, m_TECHNOLOGY_INDEX));
}
/*******************************************************************************/
IdContext MuonIdHelper::module_context(void) const {
    ExpandedIdentifier id;
    return (IdContext(id, 0, m_MODULE_INDEX));
}
/*******************************************************************************/
IdContext MuonIdHelper::detectorElement_context(void) const {
    ExpandedIdentifier id;
    return (IdContext(id, 0, m_DETECTORELEMENT_INDEX));
}
/*******************************************************************************/
IdContext MuonIdHelper::channel_context(void) const {
    ExpandedIdentifier id;
    return (IdContext(id, 0, m_CHANNEL_INDEX));
}
/*******************************************************************************/
MultiRange MuonIdHelper::multiRange(void) const { return m_full_module_range; }
/*******************************************************************************/
MuonIdHelper::size_type MuonIdHelper::module_hash_max(void) const { return m_module_hash_max; }
/*******************************************************************************/
MuonIdHelper::size_type MuonIdHelper::channel_hash_max(void) const { return m_channel_hash_max; }
/*******************************************************************************/
std::vector<Identifier> MuonIdHelper::idVector(void) const { return m_module_vec; }
/*******************************************************************************/
MuonIdHelper::const_id_iterator MuonIdHelper::module_begin(void) const { return (m_module_vec.begin()); }
/*******************************************************************************/
MuonIdHelper::const_id_iterator MuonIdHelper::module_end(void) const { return (m_module_vec.end()); }
/*******************************************************************************/
MuonIdHelper::const_id_iterator MuonIdHelper::detectorElement_begin(void) const { return (m_detectorElement_vec.begin()); }
/*******************************************************************************/
MuonIdHelper::const_id_iterator MuonIdHelper::detectorElement_end(void) const { return (m_detectorElement_vec.end()); }

MuonIdHelper::const_id_iterator MuonIdHelper::channel_begin(void) const { return (m_channel_vec.begin()); }
/*******************************************************************************/
MuonIdHelper::const_id_iterator MuonIdHelper::channel_end(void) const { return (m_channel_vec.end()); }
/*******************************************************************************/
// Check common station fields
bool MuonIdHelper::validStation(int stationName, int technology) const {   
    return validStation(stationName) && validTechnology(technology);
}
bool MuonIdHelper::validStation(int stationName) const{
    return stationName >= 0 && m_stationInTech.count(stationName);
}
bool MuonIdHelper::validTechnology(int technology) const{
    return technology >= 0 && m_technologyIdxToNameMap.find(technology) != m_technologyIdxToNameMap.end();    
}
    
/*******************************************************************************/
void MuonIdHelper::addStationID(ExpandedIdentifier& id, int stationName, int stationEta, int stationPhi, int technology) const {
    id << stationName << stationEta << stationPhi << technology;
}
/*******************************************************************************/
// Check if ID for muon system
bool MuonIdHelper::is_muon(const Identifier& id) const { return AtlasDetectorID::is_muon(id); }
/*******************************************************************************/
// Check if ID for MDT
bool MuonIdHelper::is_mdt(const Identifier& id) const { return AtlasDetectorID::is_mdt(id); }
/*******************************************************************************/
// Check if ID for CSC
bool MuonIdHelper::is_csc(const Identifier& id) const { return AtlasDetectorID::is_csc(id); }
/*******************************************************************************/
// Check if ID for RPC
bool MuonIdHelper::is_rpc(const Identifier& id) const { return AtlasDetectorID::is_rpc(id); }
/*******************************************************************************/
// Check if ID for TGC
bool MuonIdHelper::is_tgc(const Identifier& id) const { return AtlasDetectorID::is_tgc(id); }
/*******************************************************************************/
// Check if ID for sTGC
bool MuonIdHelper::is_stgc(const Identifier& id) const { return AtlasDetectorID::is_stgc(id); }
/*******************************************************************************/
// Check if ID for MicroMegas
bool MuonIdHelper::is_mm(const Identifier& id) const { return AtlasDetectorID::is_mm(id); }
/*******************************************************************************/
// Access to components of the ID
int MuonIdHelper::stationName(const Identifier& id) const {
    int result = m_sta_impl.unpack(id);
    return result;
}
/*******************************************************************************/
int MuonIdHelper::stationEta(const Identifier& id) const {
    int result = m_eta_impl.unpack(id);
    return result;
}
/*******************************************************************************/
int MuonIdHelper::stationPhi(const Identifier& id) const {
    int result = m_phi_impl.unpack(id);
    return result;
}
/*******************************************************************************/
int MuonIdHelper::technology(const Identifier& id) const {
    int result = m_tec_impl.unpack(id);
    return result;
}
/*******************************************************************************/
int MuonIdHelper::stationNameIndexMax() const { return m_stationIndexMax; }
/*******************************************************************************/
int MuonIdHelper::technologyNameIndexMax() const { return m_technologyIndexMax; }
/*******************************************************************************/
// Methods used by Moore
bool MuonIdHelper::isBarrel(const Identifier& id) const { return isBarrel(stationName(id)); }
/*******************************************************************************/
bool MuonIdHelper::isEndcap(const Identifier& id) const { return isEndcap(stationName(id)); }
/*******************************************************************************/
bool MuonIdHelper::isForward(const Identifier& id) const { return isForward(stationName(id)); }
/*******************************************************************************/
bool MuonIdHelper::isSmall(const Identifier& id) const { return isSmall(stationName(id)); }
/*******************************************************************************/
bool MuonIdHelper::isBarrel(const int& stationNameIndex) const { return (m_isBarrel.count(stationNameIndex) == 1); }
/*******************************************************************************/
bool MuonIdHelper::isEndcap(const int& stationNameIndex) const { return (m_isBarrel.count(stationNameIndex) == 0); }
/*******************************************************************************/
bool MuonIdHelper::isForward(const int& stationNameIndex) const { return (m_isForward.count(stationNameIndex) == 1); }
/*******************************************************************************/
bool MuonIdHelper::isSmall(const int& stationNameIndex) const { return (m_isSmall.count(stationNameIndex) == 1); }
/*******************************************************************************/
// Access to name and technology maps
int MuonIdHelper::stationNameIndex(const std::string& name) const {
    std::map<std::string, int>::const_iterator itr = m_stationNameToIdxMap.find(name);
    if (itr != m_stationNameToIdxMap.end()) return itr->second;
    return -1;
}
/*******************************************************************************/
int MuonIdHelper::technologyIndex(const std::string& name) const {
    std::map<std::string, int>::const_iterator itr = m_technologyNameToIdxMap.find(name);
    if (itr != m_technologyNameToIdxMap.end()) return itr ->second;
    return -1;
}
/*******************************************************************************/
const std::string& MuonIdHelper::stationNameString(const int& index) const {
    assert(index >= 0 && index <= stationNameIndexMax());
    std::map<int, std::string>::const_iterator itr = m_stationIdxToNameMap.find(index);
    if (itr != m_stationIdxToNameMap.end()) return itr->second;
    return BAD_NAME;
}
/*******************************************************************************/
const std::string& MuonIdHelper::technologyString(const int& index) const {
    assert(index >= 0 && index <= technologyNameIndexMax());
    std::map<int, std::string>::const_iterator itr =  m_technologyIdxToNameMap.find(index);
    if (itr != m_technologyIdxToNameMap.end()) return itr->second;
    return BAD_NAME;
}
/*******************************************************************************/
int MuonIdHelper::nStationNames() const { return (int)m_isSmall.size(); }
/*******************************************************************************/
bool MuonIdHelper::isInitialized() const { return m_init; }
/*******************************************************************************/
