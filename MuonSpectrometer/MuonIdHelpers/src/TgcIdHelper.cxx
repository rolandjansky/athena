/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

#include "MuonIdHelpers/TgcIdHelper.h"

TgcIdHelper::TgcIdHelper() : MuonIdHelper("TgcIdHelper") {}

// Initialize dictionary
int TgcIdHelper::initialize_from_dictionary(const IdDictMgr& dict_mgr) {
    int status = 0;

    // Check whether this helper should be reinitialized
    if (!reinitialize(dict_mgr)) {
        ATH_MSG_INFO("Request to reinitialize not satisfied - tags have not changed");
        return 0;
    } else {
        ATH_MSG_DEBUG("(Re)initialize");
    }

    // init base object
    if (AtlasDetectorID::initialize_from_dictionary(dict_mgr)) return (1);

    // Register version of the MuonSpectrometer dictionary
    if (register_dict_tag(dict_mgr, "MuonSpectrometer")) return (1);

    m_dict = dict_mgr.find_dictionary("MuonSpectrometer");
    if (!m_dict) {
        ATH_MSG_ERROR(" initialize_from_dict - cannot access MuonSpectrometer dictionary ");
        return 1;
    }

    // Initialize some of the field indices
    if (initLevelsFromDict()) return (1);

    IdDictField* field = m_dict->find_field("tgcGasGap");
    if (field) {
        m_GASGAP_INDEX = field->m_index;
    } else {
        ATH_MSG_ERROR("initLevelsFromDict - unable to find 'tgcGasGap' field ");
        status = 1;
    }

    field = m_dict->find_field("isStrip");
    if (field) {
        m_ISSTRIP_INDEX = field->m_index;
    } else {
        ATH_MSG_ERROR("initLevelsFromDict - unable to find 'isStrip' field ");
        status = 1;
    }

    field = m_dict->find_field("channel");
    if (field) {
        m_CHANNEL_INDEX = field->m_index;
    } else {
        ATH_MSG_ERROR("initLevelsFromDict - unable to find channel' field ");
        status = 1;
    }

    // reinitialize the module ndex
    m_DETECTORELEMENT_INDEX = m_MODULE_INDEX;

    // save an index to the first region of tgc
    IdDictGroup* tgcGroup = m_dict->find_group("tgc");
    if (!tgcGroup) {
        ATH_MSG_ERROR("Cannot find tgc group");
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

    ATH_MSG_DEBUG(" TGC decode index and bit fields for each level: " << std::endl
                                                                      << " muon        " << m_muon_impl.show_to_string() << std::endl
                                                                      << " station     " << m_sta_impl.show_to_string() << std::endl
                                                                      << " eta         " << m_eta_impl.show_to_string() << std::endl
                                                                      << " phi         " << m_phi_impl.show_to_string() << std::endl
                                                                      << " technology  " << m_tec_impl.show_to_string() << std::endl
                                                                      << " gas gap     " << m_gap_impl.show_to_string() << std::endl
                                                                      << " is strip    " << m_ist_impl.show_to_string() << std::endl
                                                                      << " channel     " << m_cha_impl.show_to_string());
    // Build multirange for the valid set of identifiers
    //

    // Find value for the field MuonSpectrometer
    int muonField = -1;
    const IdDictDictionary* atlasDict = dict_mgr.find_dictionary("ATLAS");
    if (atlasDict->get_label_value("subdet", "MuonSpectrometer", muonField)) {
        ATH_MSG_ERROR("Could not get value for label 'MuonSpectrometer' of field 'subdet' in dictionary " << atlasDict->m_name);
        return (1);
    }

    // Build MultiRange down to "technology" for all (muon) regions
    ExpandedIdentifier region_id;
    region_id.add(muonField);
    Range prefix;
    MultiRange muon_range = m_dict->build_multirange(region_id, prefix, "technology");
    if (muon_range.size() > 0) {
        ATH_MSG_INFO("MultiRange built successfully to Technology: "
                     << "MultiRange size is " << muon_range.size());
    } else {
        ATH_MSG_ERROR("Muon MultiRange is empty");
    }

    // Build MultiRange down to "detector element" for all mdt regions

    ExpandedIdentifier detectorElement_region;
    detectorElement_region.add(muonField);
    Range detectorElement_prefix;
    MultiRange muon_detectorElement_range = m_dict->build_multirange(detectorElement_region, detectorElement_prefix, "technology");
    if (muon_detectorElement_range.size() > 0) {
        ATH_MSG_INFO("MultiRange built successfully to detector element: "
                     << "Multilayer MultiRange size is " << muon_detectorElement_range.size());
    } else {
        ATH_MSG_ERROR("Muon TGC detector element MultiRange is empty");
    }

    // Build MultiRange down to "channel" for all TGC regions
    ExpandedIdentifier tgc_region;
    tgc_region.add(muonField);
    Range tgc_prefix;
    MultiRange muon_channel_range = m_dict->build_multirange(tgc_region, tgc_prefix, "channel");
    if (muon_channel_range.size() > 0) {
        ATH_MSG_INFO("MultiRange built successfully to channel: "
                     << "MultiRange size is " << muon_channel_range.size());
    } else {
        ATH_MSG_ERROR("Muon MultiRange is empty for channels");
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
                ATH_MSG_DEBUG("field size is " << (int)range.cardinality() << " field index = " << i);
            }
        }
    }

    for (int j = 0; j < (int)muon_detectorElement_range.size(); ++j) {
        const Range& range = muon_detectorElement_range[j];
        if (range.fields() > m_TECHNOLOGY_INDEX) {
            const Range::field& field = range[m_TECHNOLOGY_INDEX];
            if (field.match((ExpandedIdentifier::element_type)tgcField)) {
                m_full_detectorElement_range.add(range);
                ATH_MSG_DEBUG("detector element field size is " << (int)range.cardinality() << " field index = " << j);
            }
        }
    }

    for (int j = 0; j < (int)muon_channel_range.size(); ++j) {
        const Range& range = muon_channel_range[j];
        if (range.fields() > m_TECHNOLOGY_INDEX) {
            const Range::field& field = range[m_TECHNOLOGY_INDEX];
            if (field.match((ExpandedIdentifier::element_type)tgcField)) {
                m_full_channel_range.add(range);
                ATH_MSG_DEBUG("channel field size is " << (int)range.cardinality() << " field index = " << j);
            }
        }
    }

    // test to see that the multi range is not empty
    if (m_full_module_range.size() == 0) {
        ATH_MSG_ERROR("TGC MultiRange ID is empty for modules");
        status = 1;
    }

    /// test to see that the detector element multi range is not empty

    if (m_full_detectorElement_range.size() == 0) {
        ATH_MSG_ERROR("TGC MultiRange ID is empty for detector elements");
        status = 1;
    }

    // test to see that the multi range is not empty
    if (m_full_channel_range.size() == 0) {
        ATH_MSG_ERROR("TGC MultiRange ID is empty for channels");
        status = 1;
    }

    // Setup the hash tables for TGC
    ATH_MSG_INFO("Initializing TGC hash indices ... ");
    status = init_hashes();
    status = init_detectorElement_hashes();  // same as module hash
    status = init_id_to_hashes();

    // Setup hash tables for finding neighbors
    ATH_MSG_INFO("Initializing TGC hash indices for finding neighbors ... ");
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
        ATH_MSG_DEBUG("Invalid gasGap=" << gasG << " gasGapMin=" << gasGapMin(id) << " gasGapMax=" << gasGapMax(tripletChamber(station)));
        return false;
    }

    int isstrip = isStrip(id);
    if (isstrip < isStripMin(id) || isstrip > isStripMax(id)) {
        ATH_MSG_DEBUG("Invalid isStrip=" << isstrip << " isStripMin=" << isStripMin(id) << " isStripMax=" << isStripMax(id));
        return false;
    }

    int element = channel(id);
    if (element < channelMin(id) || element > channelMax(id)) {
        ATH_MSG_DEBUG("Invalid channel=" << element << " channelMin=" << channelMin(id) << " channelMax=" << channelMax(id));
        return false;
    }
    return true;
}
bool TgcIdHelper::isStNameInTech(const std::string& name) const { return 'T' == name[0]; }
bool TgcIdHelper::validElement(const Identifier& id) const {
    int station = stationName(id);
    if (!validStation(station)) {
        ATH_MSG_DEBUG("Invalid stationName=" << stationNameString(station));
        return false;
    }

    int eta = stationEta(id);
    if (eta < stationEtaMin(id) || eta > stationEtaMax(id)) {
        ATH_MSG_DEBUG("Invalid stationEta=" << eta << " for stationName=" << stationNameString(station) << " stationIndex=" << station
                                            << " stationEtaMin=" << stationEtaMin(id) << " stationEtaMax=" << stationEtaMax(id));
        return false;
    }

    int phi = stationPhi(id);
    if (phi < stationPhiMin(id) || phi > stationPhiMax(id)) {
        ATH_MSG_DEBUG("Invalid stationPhi=" << phi << " for stationName=" << stationNameString(station) << " stationIndex=" << station
                                            << " stationPhiMin=" << stationPhiMin(id) << " stationPhiMax=" << stationPhiMax(id));
        return false;
    }
    return true;
}

// Private validation of levels

bool TgcIdHelper::validElement(const Identifier& id, int stationName, int stationEta, int stationPhi) const {
    if (!validStation(stationName)) {
        ATH_MSG_DEBUG("Invalid stationName=" << stationNameString(stationName));
        return false;
    }
    if (stationEta < stationEtaMin(id) || stationEta > stationEtaMax(id)) {
        ATH_MSG_DEBUG("Invalid stationEta=" << stationEta << " for stationName=" << stationNameString(stationName)
                                            << " stationIndex=" << stationName << " stationEtaMin=" << stationEtaMin(id)
                                            << " stationEtaMax=" << stationEtaMax(id));
        return false;
    }
    if (stationPhi < stationPhiMin(id) || stationPhi > stationPhiMax(id)) {
        ATH_MSG_DEBUG("Invalid stationPhi=" << stationPhi << " for stationName=" << stationNameString(stationName)
                                            << " stationIndex=" << stationName << " stationPhiMin=" << stationPhiMin(id)
                                            << " stationPhiMax=" << stationPhiMax(id));
        return false;
    }
    return true;
}

// Check values down to readout channel level

bool TgcIdHelper::validChannel(const Identifier& id, int stationName, int stationEta, int stationPhi, int gasGap, int isStrip,
                               int channel) const {
    if (!validElement(id, stationName, stationEta, stationPhi)) return false;

    if (gasGap < gasGapMin() || gasGap > gasGapMax(tripletChamber(stationName))) {
        ATH_MSG_DEBUG("Invalid gasGap=" << gasGap << " gasGapMin=" << gasGapMin(id)
                                        << " gasGapMax=" << gasGapMax(tripletChamber(stationName)));
        return false;
    }
    if (isStrip < isStripMin(id) || isStrip > isStripMax(id)) {
        ATH_MSG_DEBUG("Invalid isStrip=" << isStrip << " isStripMin=" << isStripMin(id) << " isStripMax=" << isStripMax(id));
        return false;
    }
    if (channel < channelMin(id) || channel > channelMax(id)) {
        ATH_MSG_DEBUG("Invalid channel=" << channel << " channelMin=" << channelMin(id) << " channelMax=" << channelMax(id));
        return false;
    }
    return true;
}

Identifier TgcIdHelper::elementID(int stationName, int stationEta, int stationPhi) const {
    // pack fields independently
    Identifier result((Identifier::value_type)0);
    m_muon_impl.pack(muon_field_value(), result);
    m_sta_impl.pack(stationName, result);
    m_eta_impl.pack(stationEta, result);
    m_phi_impl.pack(stationPhi, result);
    m_tec_impl.pack(tgc_field_value(), result);
    return result;
}
Identifier TgcIdHelper::elementID(int stationName, int stationEta, int stationPhi, bool& isValid) const {
    try {
        const Identifier result = elementID(stationName, stationEta, stationPhi);
        isValid = validElement(result, stationName, stationEta, stationPhi);
        return result;
    } catch (const std::out_of_range&) { isValid = false; }
    return Identifier{0};
}

Identifier TgcIdHelper::elementID(const std::string& stationNameStr, int stationEta, int stationPhi) const {
    return elementID(stationNameIndex(stationNameStr), stationEta, stationPhi);
}
Identifier TgcIdHelper::elementID(const std::string& stationNameStr, int stationEta, int stationPhi, bool& isValid) const {
    return elementID(stationNameIndex(stationNameStr), stationEta, stationPhi, isValid);
}
Identifier TgcIdHelper::elementID(const Identifier& id) const { return parentID(id); }

Identifier TgcIdHelper::channelID(int stationName, int stationEta, int stationPhi, int gasGap, int isStrip, int channel) const {
    // pack fields independently
    Identifier result((Identifier::value_type)0);
    m_muon_impl.pack(muon_field_value(), result);
    m_sta_impl.pack(stationName, result);
    m_eta_impl.pack(stationEta, result);
    m_phi_impl.pack(stationPhi, result);
    m_tec_impl.pack(tgc_field_value(), result);
    m_gap_impl.pack(gasGap, result);
    m_ist_impl.pack(isStrip, result);
    m_cha_impl.pack(channel, result);
    return result;
}
Identifier TgcIdHelper::channelID(int stationName, int stationEta, int stationPhi, int gasGap, int isStrip, int channel,
                                  bool& isValid) const {
    try {
        const Identifier result = channelID(stationName, stationEta, stationPhi, gasGap, isStrip, channel);
        isValid = validChannel(result, stationName, stationEta, stationPhi, gasGap, isStrip, channel);
        return result;
    } catch (const std::out_of_range&) { isValid = false; }
    return Identifier{0};
}
Identifier TgcIdHelper::channelID(const std::string& stationNameStr, int stationEta, int stationPhi, int gasGap, int isStrip,
                                  int channel) const {
    return channelID(stationNameIndex(stationNameStr), stationEta, stationPhi, gasGap, isStrip, channel);
}
Identifier TgcIdHelper::channelID(const std::string& stationNameStr, int stationEta, int stationPhi, int gasGap, int isStrip, int channel,
                                  bool& isValid) const {
    return channelID(stationNameIndex(stationNameStr), stationEta, stationPhi, gasGap, isStrip, channel, isValid);
}

Identifier TgcIdHelper::channelID(const Identifier& id, int gasGap, int isStrip, int channel) const {
    Identifier result(id);
    m_gap_impl.pack(gasGap, result);
    m_ist_impl.pack(isStrip, result);
    m_cha_impl.pack(channel, result);
    return result;
}
Identifier TgcIdHelper::channelID(const Identifier& id, int gasGap, int isStrip, int channel, bool& isValid) const {
    try {
        const Identifier result = channelID(id, gasGap, isStrip, channel);
        isValid = valid(result);
        return result;
    } catch (const std::out_of_range&) { isValid = false; }
    return Identifier{0};
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

int TgcIdHelper::stationPhiMin(bool endcap) const { return endcap ? StationPhiEndcapMin : StationPhiForwardMin; }

int TgcIdHelper::stationPhiMax(bool endcap) const { return endcap ? StationPhiEndcapMax : StationPhiForwardMax; }

int TgcIdHelper::gasGapMin() const { return GasGapMin; }

int TgcIdHelper::gasGapMax(bool triplet) const { return triplet ? GasGapTripletMax : GasGapDoubletMax; }

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
    const std::string& name = stationNameString(stationName);
    return ('E' == name[2]);
}

bool TgcIdHelper::tripletChamber(int stationName) const {
    const std::string& name = stationNameString(stationName);
    return ('1' == name[1]);
}

int TgcIdHelper::chamberType(std::string stationName, int stationEta) const {
    if ('1' == stationName[1]) {
        if ('F' == stationName[2])
            return 1;
        else
            return (std::abs(stationEta) + 1);
    } else if ('2' == stationName[1]) {
        if ('F' == stationName[2])
            return 6;
        else
            return (std::abs(stationEta) + 6);
    } else if ('3' == stationName[1]) {
        if ('F' == stationName[2])
            return 12;
        else
            return (std::abs(stationEta) + 12);
    } else if ('4' == stationName[1]) {
        if ('F' == stationName[2])
            return 18;
        else
            return (std::abs(stationEta) + 18);
    }
    assert(0);
    return -1;
}

int TgcIdHelper::chamberType(int stationName, int stationEta) const {
    const std::string& name = stationNameString(stationName);
    return chamberType(name, stationEta);
}
