/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#include "MuonIdHelpers/TgcIdHelper.h"

#include "AthenaKernel/getMessageSvc.h"

TgcIdHelper::TgcIdHelper() : MuonIdHelper("TgcIdHelper"), m_GASGAP_INDEX(0), m_ISSTRIP_INDEX(0) {}

// Initialize dictionary
int TgcIdHelper::initialize_from_dictionary(const IdDictMgr& dict_mgr) {
    int status = 0;

    MsgStream log(m_msgSvc, m_logName);

    // Check whether this helper should be reinitialized
    if (!reinitialize(dict_mgr)) {
        if (m_msgSvc) { log << MSG::INFO << "Request to reinitialize not satisfied - tags have not changed" << endmsg; }
        return (0);
    } else {
        if (m_msgSvc) { log << MSG::DEBUG << "(Re)initialize" << endmsg; }
    }

    // init base object
    AtlasDetectorID::setMessageSvc(Athena::getMessageSvc());
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

    IdDictField* field = m_dict->find_field("tgcGasGap");
    if (field) {
        m_GASGAP_INDEX = field->m_index;
    } else {
        if (m_msgSvc) { log << MSG::ERROR << "initLevelsFromDict - unable to find 'tgcGasGap' field " << endmsg; }
        status = 1;
    }

    field = m_dict->find_field("isStrip");
    if (field) {
        m_ISSTRIP_INDEX = field->m_index;
    } else {
        if (m_msgSvc) { log << MSG::ERROR << "initLevelsFromDict - unable to find 'isStrip' field " << endmsg; }
        status = 1;
    }

    field = m_dict->find_field("channel");
    if (field) {
        m_CHANNEL_INDEX = field->m_index;
    } else {
        if (m_msgSvc) { log << MSG::ERROR << "initLevelsFromDict - unable to find channel' field " << endmsg; }
        status = 1;
    }

    // reinitialize the module ndex
    m_DETECTORELEMENT_INDEX = m_MODULE_INDEX;

    // save an index to the first region of tgc
    IdDictGroup* tgcGroup = m_dict->find_group("tgc");
    if (!tgcGroup) {
        if (m_msgSvc) { log << MSG::ERROR << "Cannot find tgc group" << endmsg; }
    } else {
        m_GROUP_INDEX = tgcGroup->regions()[0]->m_index;
    }

    const IdDictRegion& region = *m_dict->m_regions[m_GROUP_INDEX];
    m_eta_impl = region.m_implementation[m_ETA_INDEX];
    m_phi_impl = region.m_implementation[m_PHI_INDEX];
    m_tec_impl = region.m_implementation[m_TECHNOLOGY_INDEX];
    m_gap_impl = region.m_implementation[m_GASGAP_INDEX];
    m_ist_impl = region.m_implementation[m_ISSTRIP_INDEX];
    m_cha_impl = region.m_implementation[m_CHANNEL_INDEX];

    if (m_msgSvc) {
        log << MSG::DEBUG << " TGC decode index and bit fields for each level: " << endmsg;
        log << MSG::DEBUG << " muon        " << m_muon_impl.show_to_string() << endmsg;
        log << MSG::DEBUG << " station     " << m_sta_impl.show_to_string() << endmsg;
        log << MSG::DEBUG << " eta         " << m_eta_impl.show_to_string() << endmsg;
        log << MSG::DEBUG << " phi         " << m_phi_impl.show_to_string() << endmsg;
        log << MSG::DEBUG << " technology  " << m_tec_impl.show_to_string() << endmsg;
        log << MSG::DEBUG << " gas gap     " << m_gap_impl.show_to_string() << endmsg;
        log << MSG::DEBUG << " is strip    " << m_ist_impl.show_to_string() << endmsg;
        log << MSG::DEBUG << " channel     " << m_cha_impl.show_to_string() << endmsg;
    }

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
            log << MSG::INFO << "MultiRange built successfully to Technology: "
                << "MultiRange size is " << muon_range.size() << endmsg;
        }
    } else {
        if (m_msgSvc) { log << MSG::ERROR << "Muon MultiRange is empty" << endmsg; }
    }

    // Build MultiRange down to "detector element" for all mdt regions

    ExpandedIdentifier detectorElement_region;
    detectorElement_region.add(muonField);
    Range detectorElement_prefix;
    MultiRange muon_detectorElement_range = m_dict->build_multirange(detectorElement_region, detectorElement_prefix, "technology");
    if (muon_detectorElement_range.size() > 0) {
        if (m_msgSvc) {
            log << MSG::INFO << "MultiRange built successfully to detector element: "
                << "Multilayer MultiRange size is " << muon_detectorElement_range.size() << endmsg;
        }
    } else {
        if (m_msgSvc) { log << MSG::ERROR << "Muon TGC detector element MultiRange is empty" << endmsg; }
    }

    // Build MultiRange down to "channel" for all TGC regions
    ExpandedIdentifier tgc_region;
    tgc_region.add(muonField);
    Range tgc_prefix;
    MultiRange muon_channel_range = m_dict->build_multirange(tgc_region, tgc_prefix, "channel");
    if (muon_channel_range.size() > 0) {
        if (m_msgSvc) {
            log << MSG::INFO << "MultiRange built successfully to channel: "
                << "MultiRange size is " << muon_channel_range.size() << endmsg;
        }
    } else {
        if (m_msgSvc) { log << MSG::ERROR << "Muon MultiRange is empty for channels" << endmsg; }
    }

    // build TGC module ranges
    // Find the regions that have a "technology field" that matches the TGC and save them
    int tgcField = -1;
    status = m_dict->get_label_value("technology", "TGC", tgcField);

    for (int i = 0; i < (int)muon_range.size(); ++i) {
        const Range& range = muon_range[i];
        if (range.fields() > m_TECHNOLOGY_INDEX) {
            const Range::field& field = range[m_TECHNOLOGY_INDEX];
            if (field.match((ExpandedIdentifier::element_type)tgcField)) {
                m_full_module_range.add(range);
                if (m_msgSvc) { log << MSG::DEBUG << "field size is " << (int)range.cardinality() << " field index = " << i << endmsg; }
            }
        }
    }

    for (int j = 0; j < (int)muon_detectorElement_range.size(); ++j) {
        const Range& range = muon_detectorElement_range[j];
        if (range.fields() > m_TECHNOLOGY_INDEX) {
            const Range::field& field = range[m_TECHNOLOGY_INDEX];
            if (field.match((ExpandedIdentifier::element_type)tgcField)) {
                m_full_detectorElement_range.add(range);
                if (m_msgSvc) {
                    log << MSG::DEBUG << "detector element field size is " << (int)range.cardinality() << " field index = " << j << endmsg;
                }
            }
        }
    }

    for (int j = 0; j < (int)muon_channel_range.size(); ++j) {
        const Range& range = muon_channel_range[j];
        if (range.fields() > m_TECHNOLOGY_INDEX) {
            const Range::field& field = range[m_TECHNOLOGY_INDEX];
            if (field.match((ExpandedIdentifier::element_type)tgcField)) {
                m_full_channel_range.add(range);
                if (m_msgSvc) {
                    log << MSG::DEBUG << "channel field size is " << (int)range.cardinality() << " field index = " << j << endmsg;
                }
            }
        }
    }

    // test to see that the multi range is not empty
    if (m_full_module_range.size() == 0) {
        if (m_msgSvc) { log << MSG::ERROR << "TGC MultiRange ID is empty for modules" << endmsg; }
        status = 1;
    }

    /// test to see that the detector element multi range is not empty

    if (m_full_detectorElement_range.size() == 0) {
        if (m_msgSvc) { log << MSG::ERROR << "TGC MultiRange ID is empty for detector elements" << endmsg; }
        status = 1;
    }

    // test to see that the multi range is not empty
    if (m_full_channel_range.size() == 0) {
        if (m_msgSvc) { log << MSG::ERROR << "TGC MultiRange ID is empty for channels" << endmsg; }
        status = 1;
    }

    // Setup the hash tables for TGC
    if (m_msgSvc) { log << MSG::INFO << "Initializing TGC hash indices ... " << endmsg; }
    status = init_hashes();
    status = init_detectorElement_hashes();  // same as module hash
    status = init_id_to_hashes();

    // Setup hash tables for finding neighbors
    if (m_msgSvc) { log << MSG::INFO << "Initializing TGC hash indices for finding neighbors ... " << endmsg; }
    status = init_neighbors();

    m_init = true;
    return (status);
}

int TgcIdHelper::init_id_to_hashes() {
    unsigned int hash_max = this->module_hash_max();
    for (unsigned int i = 0; i < hash_max; ++i) {
        Identifier id = m_module_vec[i];
        int station = this->stationName(id);
        int eta = this->stationEta(id) + 10;  // for negative etas
        int phi = this->stationPhi(id);
        m_module_hashes[station][eta - 1][phi - 1] = i;
    }
    return 0;
}

int TgcIdHelper::get_module_hash(const Identifier& id, IdentifierHash& hash_id) const {
    // Identifier moduleId = elementID(id);
    // IdContext context = module_context();
    // return get_hash(moduleId,hash_id,&context);
    int station = this->stationName(id);
    int eta = this->stationEta(id) + 10;  // for negative etas
    int phi = this->stationPhi(id);
    hash_id = m_module_hashes[station][eta - 1][phi - 1];
    return 0;
}

int TgcIdHelper::get_detectorElement_hash(const Identifier& id, IdentifierHash& hash_id) const {
    return this->get_module_hash(id, hash_id);
}

void TgcIdHelper::idChannels(const Identifier& id, std::vector<Identifier>& vect) const {
    vect.clear();
    Identifier parent = parentID(id);
    for (unsigned int i = 0; i < m_full_channel_range.size(); ++i) {
        const Range& range = m_full_channel_range[i];
        Range::const_identifier_factory first = range.factory_begin();
        Range::const_identifier_factory last = range.factory_end();
        for (; first != last; ++first) {
            Identifier child;
            get_id((*first), child);
            if (parentID(child) == parent) vect.push_back(child);
        }
    }
}

int TgcIdHelper::stationEtaMin(const Identifier& id) const {
    ExpandedIdentifier expId;
    IdContext eta_context(expId, 0, m_ETA_INDEX);
    if (!get_expanded_id(id, expId, &eta_context)) {
        int result = -999;
        for (unsigned int i = 0; i < m_full_module_range.size(); ++i) {
            const Range& range = m_full_module_range[i];
            if (range.match(expId)) {
                const Range::field& eta_field = range[m_ETA_INDEX];
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
        return (result);
    }
    return (999);  // default
}

int TgcIdHelper::stationEtaMax(const Identifier& id) const {
    ExpandedIdentifier expId;
    IdContext eta_context(expId, 0, m_ETA_INDEX);
    if (!get_expanded_id(id, expId, &eta_context)) {
        int result = -999;
        for (unsigned int i = 0; i < m_full_module_range.size(); ++i) {
            const Range& range = m_full_module_range[i];
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
    return (-999);
}

int TgcIdHelper::stationPhiMin(const Identifier& id) const {
    ExpandedIdentifier expId;
    IdContext phi_context(expId, 0, m_PHI_INDEX);
    if (!get_expanded_id(id, expId, &phi_context)) {
        for (unsigned int i = 0; i < m_full_module_range.size(); ++i) {
            const Range& range = m_full_module_range[i];
            if (range.match(expId)) {
                const Range::field& phi_field = range[m_PHI_INDEX];
                if (phi_field.has_minimum()) { return (phi_field.get_minimum()); }
            }
        }
    }
    // Failed to find the min
    return (999);
}

int TgcIdHelper::stationPhiMax(const Identifier& id) const {
    ExpandedIdentifier expId;
    IdContext phi_context(expId, 0, m_PHI_INDEX);
    if (!get_expanded_id(id, expId, &phi_context)) {
        for (unsigned int i = 0; i < m_full_module_range.size(); ++i) {
            const Range& range = m_full_module_range[i];
            if (range.match(expId)) {
                const Range::field& phi_field = range[m_PHI_INDEX];
                if (phi_field.has_maximum()) { return (phi_field.get_maximum()); }
            }
        }
    }
    // Failed to find the max
    return (-999);
}

int TgcIdHelper::gasGapMin(const Identifier& id) const {
    ExpandedIdentifier expId;
    IdContext gasgap_context(expId, 0, m_GASGAP_INDEX);
    if (!get_expanded_id(id, expId, &gasgap_context)) {
        int result = -999;
        for (unsigned int i = 0; i < m_full_channel_range.size(); ++i) {
            const Range& range = m_full_channel_range[i];
            if (range.match(expId)) {
                const Range::field& gasgap_field = range[m_GASGAP_INDEX];
                if (gasgap_field.has_minimum()) {
                    int gasgapmin = gasgap_field.get_minimum();
                    if (-999 == result) {
                        result = gasgapmin;
                    } else {
                        if (gasgapmin < result) result = gasgapmin;
                    }
                }
            }
        }
        return (result);
    }
    return (999);
}

int TgcIdHelper::gasGapMax(const Identifier& id) const {
    ExpandedIdentifier expId;
    IdContext gasgap_context(expId, 0, m_GASGAP_INDEX);
    if (!get_expanded_id(id, expId, &gasgap_context)) {
        for (unsigned int i = 0; i < m_full_channel_range.size(); ++i) {
            const Range& range = m_full_channel_range[i];
            if (range.match(expId)) {
                const Range::field& gasgap_field = range[m_GASGAP_INDEX];
                if (gasgap_field.has_maximum()) { return (gasgap_field.get_maximum()); }
            }
        }
    }
    // Failed to find the max
    return (-999);
}

int TgcIdHelper::isStripMin(const Identifier& id) const {
    ExpandedIdentifier expId;
    IdContext isstrip_context(expId, 0, m_ISSTRIP_INDEX);
    if (!get_expanded_id(id, expId, &isstrip_context)) {
        int result = -999;
        for (unsigned int i = 0; i < m_full_channel_range.size(); ++i) {
            const Range& range = m_full_channel_range[i];
            if (range.match(expId)) {
                const Range::field& isstrip_field = range[m_ISSTRIP_INDEX];
                if (isstrip_field.has_minimum()) {
                    int isstripmin = isstrip_field.get_minimum();
                    if (-999 == result) {
                        result = isstripmin;
                    } else {
                        if (isstripmin < result) result = isstripmin;
                    }
                }
            }
        }
        return (result);
    }
    return (999);
}

int TgcIdHelper::isStripMax(const Identifier& id) const {
    ExpandedIdentifier expId;
    IdContext isstrip_context(expId, 0, m_ISSTRIP_INDEX);
    if (!get_expanded_id(id, expId, &isstrip_context)) {
        int result = -999;
        for (unsigned int i = 0; i < m_full_channel_range.size(); ++i) {
            const Range& range = m_full_channel_range[i];
            if (range.match(expId)) {
                const Range::field& isstrip_field = range[m_ISSTRIP_INDEX];
                if (isstrip_field.has_maximum()) {
                    int isstripmax = isstrip_field.get_maximum();
                    if (result < isstripmax) result = isstripmax;
                }
            }
        }
        return (result);
    }
    return (-999);
}

int TgcIdHelper::channelMin(const Identifier& id) const {
    ExpandedIdentifier expId;
    IdContext channel_context(expId, 0, m_CHANNEL_INDEX);
    if (!get_expanded_id(id, expId, &channel_context)) {
        int result = -999;
        for (unsigned int i = 0; i < m_full_channel_range.size(); ++i) {
            const Range& range = m_full_channel_range[i];
            if (range.match(expId)) {
                const Range::field& channel_field = range[m_CHANNEL_INDEX];
                if (channel_field.has_minimum()) {
                    int channelmin = channel_field.get_minimum();
                    if (-999 == result) {
                        result = channelmin;
                    } else {
                        if (channelmin < result) result = channelmin;
                    }
                }
            }
        }
        return (result);
    }
    return (999);
}

int TgcIdHelper::channelMax(const Identifier& id) const {
    ExpandedIdentifier expId;
    IdContext channel_context(expId, 0, m_CHANNEL_INDEX);
    if (!get_expanded_id(id, expId, &channel_context)) {
        int result = -999;
        for (unsigned int i = 0; i < m_full_channel_range.size(); ++i) {
            const Range& range = m_full_channel_range[i];
            if (range.match(expId)) {
                const Range::field& channel_field = range[m_CHANNEL_INDEX];
                if (channel_field.has_maximum()) {
                    int channelmax = channel_field.get_maximum();
                    if (result < channelmax) result = channelmax;
                }
            }
        }
        return (result);
    }
    return (-999);
}

// validation of levels

bool TgcIdHelper::valid(const Identifier& id) const {
    if (!validElement(id)) return false;

    int station = stationName(id);
    int gasG = gasGap(id);
    if (gasG < gasGapMin() || gasG > gasGapMax(tripletChamber(station))) {
        if (m_msgSvc) {
            MsgStream log(m_msgSvc, m_logName);
            log << MSG::WARNING << "Invalid gasGap=" << gasG << " gasGapMin=" << gasGapMin(id)
                << " gasGapMax=" << gasGapMax(tripletChamber(station)) << endmsg;
        }
        return false;
    }

    int isstrip = isStrip(id);
    if (isstrip < isStripMin(id) || isstrip > isStripMax(id)) {
        if (m_msgSvc) {
            MsgStream log(m_msgSvc, m_logName);
            log << MSG::WARNING << "Invalid isStrip=" << isstrip << " isStripMin=" << isStripMin(id) << " isStripMax=" << isStripMax(id)
                << endmsg;
        }
        return false;
    }

    int element = channel(id);
    if (element < channelMin(id) || element > channelMax(id)) {
        if (m_msgSvc) {
            MsgStream log(m_msgSvc, m_logName);
            log << MSG::WARNING << "Invalid channel=" << element << " channelMin=" << channelMin(id) << " channelMax=" << channelMax(id)
                << endmsg;
        }
        return false;
    }
    return true;
}

bool TgcIdHelper::validElement(const Identifier& id) const {
    int station = stationName(id);
    std::string name = stationNameString(station);

    if ('T' != name[0]) {
        if (m_msgSvc) {
            MsgStream log(m_msgSvc, m_logName);
            log << MSG::WARNING << "Invalid stationName=" << name << endmsg;
        }
        return false;
    }

    int eta = stationEta(id);
    if (eta < stationEtaMin(id) || eta > stationEtaMax(id)) {
        if (m_msgSvc) {
            MsgStream log(m_msgSvc, m_logName);
            log << MSG::WARNING << "Invalid stationEta=" << eta << " for stationName=" << name << " stationIndex=" << station
                << " stationEtaMin=" << stationEtaMin(id) << " stationEtaMax=" << stationEtaMax(id) << endmsg;
        }
        return false;
    }

    int phi = stationPhi(id);
    if (phi < stationPhiMin(id) || phi > stationPhiMax(id)) {
        if (m_msgSvc) {
            MsgStream log(m_msgSvc, m_logName);
            log << MSG::WARNING << "Invalid stationPhi=" << phi << " for stationName=" << name << " stationIndex=" << station
                << " stationPhiMin=" << stationPhiMin(id) << " stationPhiMax=" << stationPhiMax(id) << endmsg;
        }
        return false;
    }
    return true;
}

// Private validation of levels

bool TgcIdHelper::validElement(const Identifier& id, int stationName, int stationEta, int stationPhi) const {
    std::string name = stationNameString(stationName);

    if ('T' != name[0]) {
        if (m_msgSvc) {
            MsgStream log(m_msgSvc, m_logName);
            log << MSG::WARNING << "Invalid stationName=" << name << endmsg;
        }
        return false;
    }
    if (stationEta < stationEtaMin(id) || stationEta > stationEtaMax(id)) {
        if (m_msgSvc) {
            MsgStream log(m_msgSvc, m_logName);
            log << MSG::WARNING << "Invalid stationEta=" << stationEta << " for stationName=" << name << " stationIndex=" << stationName
                << " stationEtaMin=" << stationEtaMin(id) << " stationEtaMax=" << stationEtaMax(id) << endmsg;
        }
        return false;
    }
    if (stationPhi < stationPhiMin(id) || stationPhi > stationPhiMax(id)) {
        if (m_msgSvc) {
            MsgStream log(m_msgSvc, m_logName);
            log << MSG::WARNING << "Invalid stationPhi=" << stationPhi << " for stationName=" << name << " stationIndex=" << stationName
                << " stationPhiMin=" << stationPhiMin(id) << " stationPhiMax=" << stationPhiMax(id) << endmsg;
        }
        return false;
    }
    return true;
}

// Check values down to readout channel level

bool TgcIdHelper::validChannel(const Identifier& id, int stationName, int stationEta, int stationPhi, int gasGap, int isStrip,
                               int channel) const {
    if (!validElement(id, stationName, stationEta, stationPhi)) return false;

    if (gasGap < gasGapMin() || gasGap > gasGapMax(tripletChamber(stationName))) {
        if (m_msgSvc) {
            MsgStream log(m_msgSvc, m_logName);
            log << MSG::WARNING << "Invalid gasGap=" << gasGap << " gasGapMin=" << gasGapMin(id)
                << " gasGapMax=" << gasGapMax(tripletChamber(stationName)) << endmsg;
        }
        return false;
    }
    if (isStrip < isStripMin(id) || isStrip > isStripMax(id)) {
        if (m_msgSvc) {
            MsgStream log(m_msgSvc, m_logName);
            log << MSG::WARNING << "Invalid isStrip=" << isStrip << " isStripMin=" << isStripMin(id) << " isStripMax=" << isStripMax(id)
                << endmsg;
        }
        return false;
    }
    if (channel < channelMin(id) || channel > channelMax(id)) {
        if (m_msgSvc) {
            MsgStream log(m_msgSvc, m_logName);
            log << MSG::WARNING << "Invalid channel=" << channel << " channelMin=" << channelMin(id) << " channelMax=" << channelMax(id)
                << endmsg;
        }
        return false;
    }
    return true;
}

Identifier TgcIdHelper::elementID(int stationName, int stationEta, int stationPhi, bool check, bool* isValid) const {
    // pack fields independently
    Identifier result((Identifier::value_type)0);
    bool val = false;
    m_muon_impl.pack(muon_field_value(), result);
    m_sta_impl.pack(stationName, result);
    m_eta_impl.pack(stationEta, result);
    m_phi_impl.pack(stationPhi, result);
    m_tec_impl.pack(tgc_field_value(), result);
    if (check) {
        val = this->validElement(result, stationName, stationEta, stationPhi);
        if (isValid) *isValid = val;
    }
    return result;
}

Identifier TgcIdHelper::elementID( std::string_view stationNameStr, int stationEta, int stationPhi, bool check, bool* isValid) const {
    Identifier id;
    int stationName = stationNameIndex(stationNameStr);
    id = elementID(stationName, stationEta, stationPhi, check, isValid);
    return id;
}

Identifier TgcIdHelper::elementID(const Identifier& id) const { return parentID(id); }

Identifier TgcIdHelper::channelID(int stationName, int stationEta, int stationPhi, int gasGap, int isStrip, int channel, bool check,
                                  bool* isValid) const {
    // pack fields independently
    Identifier result((Identifier::value_type)0);
    bool val = false;
    m_muon_impl.pack(muon_field_value(), result);
    m_sta_impl.pack(stationName, result);
    m_eta_impl.pack(stationEta, result);
    m_phi_impl.pack(stationPhi, result);
    m_tec_impl.pack(tgc_field_value(), result);
    m_gap_impl.pack(gasGap, result);
    m_ist_impl.pack(isStrip, result);
    m_cha_impl.pack(channel, result);
    if (check) {
        val = validChannel(result, stationName, stationEta, stationPhi, gasGap, isStrip, channel);
        if (isValid) *isValid = val;
    }
    return result;
}

Identifier TgcIdHelper::channelID(std::string_view stationNameStr, int stationEta, int stationPhi, int gasGap, int isStrip, int channel,
                                  bool check, bool* isValid) const {
    Identifier id;
    int stationName = stationNameIndex(stationNameStr);
    id = channelID(stationName, stationEta, stationPhi, gasGap, isStrip, channel, check, isValid);
    return id;
}

Identifier TgcIdHelper::channelID(const Identifier& id, int gasGap, int isStrip, int channel, bool check, bool* isValid) const {
    Identifier result(id);
    bool val = false;
    m_gap_impl.pack(gasGap, result);
    m_ist_impl.pack(isStrip, result);
    m_cha_impl.pack(channel, result);
    if (check) {
        val = this->valid(result);
        if (isValid) *isValid = val;
    }
    return result;
}

// get parent id from strip or gang identifier
Identifier TgcIdHelper::parentID(const Identifier& id) const {
    assert(is_tgc(id));
    Identifier result(id);
    m_gap_impl.reset(result);
    m_ist_impl.reset(result);
    m_cha_impl.reset(result);
    return result;
}

// Access to components of the ID

int TgcIdHelper::gasGap(const Identifier& id) const { return m_gap_impl.unpack(id); }

/** returns measuresPhi */
int TgcIdHelper::isStrip(const Identifier& id) const { return m_ist_impl.unpack(id); }

bool TgcIdHelper::measuresPhi(const Identifier& id) const { return isStrip(id); }

int TgcIdHelper::channel(const Identifier& id) const { return m_cha_impl.unpack(id); }

// Access to min and max of level ranges

int TgcIdHelper::stationEtaMin() const { return StationEtaMin; }

int TgcIdHelper::stationEtaMax() const { return StationEtaMax; }

int TgcIdHelper::stationPhiMin(bool endcap) const {
    if (endcap) {
        return StationPhiEndcapMin;
    } else {
        return StationPhiForwardMin;
    }
}

int TgcIdHelper::stationPhiMax(bool endcap) const {
    if (endcap) {
        return StationPhiEndcapMax;
    } else {
        return StationPhiForwardMax;
    }
}

int TgcIdHelper::gasGapMin() const { return GasGapMin; }

int TgcIdHelper::gasGapMax(bool triplet) const {
    if (triplet) {
        return GasGapTripletMax;
    } else {
        return GasGapDoubletMax;
    }
}

int TgcIdHelper::isStripMin() const { return IsStripMin; }

int TgcIdHelper::isStripMax() const { return IsStripMax; }

int TgcIdHelper::channelMin() const { return ChannelMin; }

int TgcIdHelper::channelMax() const { return ChannelMax; }
/// Utility methods

int TgcIdHelper::tgcTechnology() const {
    int tgcField = technologyIndex("TGC");
    if (m_dict) { tgcField = tgc_field_value(); }
    return tgcField;
}

bool TgcIdHelper::endcapChamber(int stationName) const {
    std::string name = stationNameString(stationName);
    return ('E' == stationNameString(stationName)[2]);
}

bool TgcIdHelper::tripletChamber(int stationName) const {
    std::string name = stationNameString(stationName);
    return ('1' == name[1]);
}

int TgcIdHelper::chamberType(std::string stationName, int stationEta) const {
    if ('1' == stationName[1]) {
        if ('F' == stationName[2])
            return 1;
        else
            return (abs(stationEta) + 1);
    } else if ('2' == stationName[1]) {
        if ('F' == stationName[2])
            return 6;
        else
            return (abs(stationEta) + 6);
    } else if ('3' == stationName[1]) {
        if ('F' == stationName[2])
            return 12;
        else
            return (abs(stationEta) + 12);
    } else if ('4' == stationName[1]) {
        if ('F' == stationName[2])
            return 18;
        else
            return (abs(stationEta) + 18);
    }
    assert(0);
    return -1;
}

int TgcIdHelper::chamberType(int stationName, int stationEta) const {
    std::string name = stationNameString(stationName);
    return chamberType(name, stationEta);
}
