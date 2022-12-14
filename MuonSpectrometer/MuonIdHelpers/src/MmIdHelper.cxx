/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

#include "MuonIdHelpers/MmIdHelper.h"

#include "AthenaKernel/getMessageSvc.h"

/*******************************************************************************/
// Constructor/Destructor
MmIdHelper::MmIdHelper() : MuonIdHelper("MmIdHelper") {}
/*******************************************************************************/
// Initialize dictionary
int MmIdHelper::initialize_from_dictionary(const IdDictMgr& dict_mgr) {
    int status = 0;

    MsgStream log(m_msgSvc, m_logName);

    // Check whether this helper should be reinitialized
    if (!reinitialize(dict_mgr)) {
        if (m_msgSvc) { log << MSG::INFO << "Request to reinitialize not satisfied - tags have not changed" << endmsg; }
        return (0);
    } else {
        if (m_msgSvc) { log << MSG::DEBUG << "(Re)initialize " << endmsg; }
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

    int index = technologyIndex("MM");
    if (index == -1) {
        if (m_msgSvc) { log << MSG::DEBUG << "initLevelsFromDict - there are no MM entries in the dictionary! " << endmsg; }
        return 0;
    }

    IdDictField* field = m_dict->find_field("mmMultilayer");
    if (field) {
        m_DETECTORELEMENT_INDEX = field->m_index;
    } else {
        if (m_msgSvc) { log << MSG::ERROR << "initLevelsFromDict - unable to find 'mmMultilayer' field " << endmsg; }
        status = 1;
    }

    field = m_dict->find_field("mmGasGap");
    if (field) {
        m_GASGAP_INDEX = field->m_index;
    } else {
        if (m_msgSvc) { log << MSG::ERROR << "initLevelsFromDict - unable to find 'mmGasGap' field " << endmsg; }
        status = 1;
    }

    field = m_dict->find_field("mmChannel");
    if (field) {
        m_CHANNEL_INDEX = field->m_index;
    } else {
        if (m_msgSvc) { log << MSG::ERROR << "initLevelsFromDict - unable to find mmChannel' field " << endmsg; }
        status = 1;
    }

    // reinitialize the module ndex
    //// m_DETECTORELEMENT_INDEX = m_MODULE_INDEX;

    // save an index to the first region of MM
    IdDictGroup* mmGroup = m_dict->find_group("mm");
    if (!mmGroup) {
        if (m_msgSvc) { log << MSG::ERROR << "Cannot find mm group" << endmsg; }
    } else {
        m_GROUP_INDEX = mmGroup->regions()[0]->m_index;
    }

    const IdDictRegion& region = *m_dict->m_regions[m_GROUP_INDEX];
    m_eta_impl = region.m_implementation[m_ETA_INDEX];
    m_phi_impl = region.m_implementation[m_PHI_INDEX];
    m_tec_impl = region.m_implementation[m_TECHNOLOGY_INDEX];
    m_mplet_impl = region.m_implementation[m_DETECTORELEMENT_INDEX];
    m_gap_impl = region.m_implementation[m_GASGAP_INDEX];
    m_cha_impl = region.m_implementation[m_CHANNEL_INDEX];

    if (m_msgSvc) {
        log << MSG::DEBUG << " MicroMegas decode index and bit fields for each level: " << endmsg;
        log << MSG::DEBUG << " muon        " << m_muon_impl.show_to_string() << endmsg;
        log << MSG::DEBUG << " station     " << m_sta_impl.show_to_string() << endmsg;
        log << MSG::DEBUG << " eta         " << m_eta_impl.show_to_string() << endmsg;
        log << MSG::DEBUG << " phi         " << m_phi_impl.show_to_string() << endmsg;
        log << MSG::DEBUG << " technology  " << m_tec_impl.show_to_string() << endmsg;
        log << MSG::DEBUG << " multilayer   " << m_mplet_impl.show_to_string() << endmsg;
        log << MSG::DEBUG << " gasgap      " << m_gap_impl.show_to_string() << endmsg;
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
    MultiRange muon_detectorElement_range = m_dict->build_multirange(detectorElement_region, detectorElement_prefix, "mmMultilayer");
    if (muon_detectorElement_range.size() > 0) {
        if (m_msgSvc) {
            log << MSG::INFO << "MultiRange built successfully to detector element: "
                << "Multilayer MultiRange size is " << muon_detectorElement_range.size() << endmsg;
        }
    } else {
        if (m_msgSvc) { log << MSG::ERROR << "Muon MicroMegas detector element MultiRange is empty" << endmsg; }
    }

    // Build MultiRange down to "channel" for all MM regions
    ExpandedIdentifier mm_region;
    mm_region.add(muonField);
    Range mm_prefix;
    MultiRange muon_channel_range = m_dict->build_multirange(mm_region, mm_prefix, "mmChannel");
    if (muon_channel_range.size() > 0) {
        if (m_msgSvc) {
            log << MSG::INFO << "MultiRange built successfully to channel: "
                << "MultiRange size is " << muon_channel_range.size() << endmsg;
        }
    } else {
        if (m_msgSvc) { log << MSG::ERROR << "Muon MultiRange is empty for channels" << endmsg; }
    }

    // build MicroMegas module ranges
    // Find the regions that have a "technology field" that matches the MM and save them
    int mmField = -1;
    status = m_dict->get_label_value("technology", "MM", mmField);

    for (int i = 0; i < (int)muon_range.size(); ++i) {
        const Range& range = muon_range[i];
        if (range.fields() > m_TECHNOLOGY_INDEX) {
            const Range::field& field = range[m_TECHNOLOGY_INDEX];
            if (field.match((ExpandedIdentifier::element_type)mmField)) {
                m_full_module_range.add(range);
                if (m_msgSvc) { log << MSG::DEBUG << "field size is " << (int)range.cardinality() << " field index = " << i << endmsg; }
            }
        }
    }

    for (int j = 0; j < (int)muon_detectorElement_range.size(); ++j) {
        const Range& range = muon_detectorElement_range[j];
        if (range.fields() > m_TECHNOLOGY_INDEX) {
            const Range::field& field = range[m_TECHNOLOGY_INDEX];
            if (field.match((ExpandedIdentifier::element_type)mmField)) {
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
            if (field.match((ExpandedIdentifier::element_type)mmField)) {
                m_full_channel_range.add(range);
                if (m_msgSvc) {
                    log << MSG::DEBUG << "channel field size is " << (int)range.cardinality() << " field index = " << j << endmsg;
                }
            }
        }
    }

    // test to see that the multi range is not empty
    if (m_full_module_range.size() == 0) {
        if (m_msgSvc) { log << MSG::ERROR << "MicroMegas MultiRange ID is empty for modules" << endmsg; }
    }

    // test to see that the detector element multi range is not empty
    if (m_full_detectorElement_range.size() == 0) {
        if (m_msgSvc) { log << MSG::ERROR << "MicroMegas MultiRange ID is empty for detector elements" << endmsg; }
    }

    // test to see that the multi range is not empty
    if (m_full_channel_range.size() == 0) {
        if (m_msgSvc) { log << MSG::ERROR << "MicroMegas MultiRange ID is empty for channels" << endmsg; }
    }

    // Setup the hash tables for MicroMegas
    if (m_msgSvc) { log << MSG::INFO << "Initializing MicroMegas hash indices ... " << endmsg; }
    init_hashes();
    init_detectorElement_hashes();  // same as module hash
    init_id_to_hashes();

    /*
    //comment out this bit to test the identifiers

    status = init_channel_hashes();
    std::cout << " looping over identifiers " << std::endl;
    const_id_iterator it = channel_begin();
    const_id_iterator it_end =  channel_end();
    for( ;it!=it_end;++it ){
    if( !is_mm(*it) )  (*m_Log) << MSG::DEBUG << "BadStgc: not MM    " <<  print_to_string(*it) << endmsg;
    if( !valid(*it) )  (*m_Log) << MSG::DEBUG << "BadStgc: not valid " <<  print_to_string(*it) << endmsg;
    }
    */

    // Setup hash tables for finding neighbors
    if (m_msgSvc) { log << MSG::INFO << "Initializing MicroMegas hash indices for finding neighbors ... " << endmsg; }
    status = init_neighbors();

    m_init = true;
    return (status);
}  // end MmIdHelper::initialize_from_dictionary
/*******************************************************************************/
int MmIdHelper::init_id_to_hashes() {
    unsigned int hash_max = this->module_hash_max();
    for (unsigned int i = 0; i < hash_max; ++i) {
        Identifier id = m_module_vec[i];
        int station = this->stationName(id);
        int eta = this->stationEta(id) + 10;  // for negative etas
        int phi = this->stationPhi(id);
        m_module_hashes[station][eta - 1][phi - 1] = i;
    }

    hash_max = this->detectorElement_hash_max();
    for (unsigned int i = 0; i < hash_max; ++i) {
        Identifier id = m_detectorElement_vec[i];
        int station = this->stationName(id);
        int eta = this->stationEta(id) + 10;  // for negative eta
        int phi = this->stationPhi(id);
        int multilayer = this->multilayer(id);
        m_detectorElement_hashes[station][eta - 1][phi - 1][multilayer - 1] = i;
    }

    return 0;
}  // end MmIdHelper::init_id_to_hashes()
/*******************************************************************************/
int MmIdHelper::get_module_hash(const Identifier& id, IdentifierHash& hash_id) const {
    // Identifier moduleId = elementID(id);
    // IdContext context = module_context();
    // return get_hash(moduleId,hash_id,&context);
    int station = this->stationName(id);
    int eta = this->stationEta(id) + 10;  // for negative etas
    int phi = this->stationPhi(id);
    hash_id = m_module_hashes[station][eta - 1][phi - 1];
    return 0;
}  // end MmIdHelper::get_module_hash
/*******************************************************************************/
int MmIdHelper::get_detectorElement_hash(const Identifier& id, IdentifierHash& hash_id) const {
    int station = this->stationName(id);
    int eta = this->stationEta(id) + 10;  // for negative eta
    int phi = this->stationPhi(id);
    int multilayer = this->multilayer(id);
    hash_id = m_detectorElement_hashes[station][eta - 1][phi - 1][multilayer - 1];
    return 0;
    // return this->get_module_hash(id, hash_id);
}
/*******************************************************************************/
Identifier MmIdHelper::multilayerID(const Identifier& channelID) const {
    assert(is_mm(channelID));
    Identifier result(channelID);
    // m_mplet_impl.reset(result);
    m_gap_impl.reset(result);
    m_cha_impl.reset(result);
    return result;
}
/*******************************************************************************/
Identifier MmIdHelper::multilayerID(const Identifier& moduleID, int multilayer) const {
    Identifier result(moduleID);
    m_mplet_impl.pack(multilayer, result);
    return result;
}
Identifier MmIdHelper::multilayerID(const Identifier& moduleID, int multilayer, bool& isValid) const {
    const Identifier result = multilayerID(moduleID, multilayer);
    isValid = validElement(result);
    return result;
}
/*******************************************************************************/
void MmIdHelper::idChannels(const Identifier& id, std::vector<Identifier>& vect) const {
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
/*******************************************************************************/
int MmIdHelper::stationEtaMin(const Identifier& id) const {
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
}  // end MmIdHelper::stationEtaMin
/*******************************************************************************/
int MmIdHelper::stationEtaMax(const Identifier& id) const {
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
}  // end MmIdHelper::stationEtaMax
/*******************************************************************************/
int MmIdHelper::stationPhiMin(const Identifier& id) const {
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
/*******************************************************************************/
int MmIdHelper::stationPhiMax(const Identifier& id) const {
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
/*******************************************************************************/
int MmIdHelper::numberOfMultilayers(const Identifier& id) const {
    ExpandedIdentifier expId;
    IdContext context = technology_context();
    if (!get_expanded_id(id, expId, &context)) {
        int result = -999;
        for (unsigned int i = 0; i < m_full_channel_range.size(); ++i) {
            const Range& range = m_full_channel_range[i];
            if (range.match(expId)) {
                const Range::field& multilayer_field = range[m_DETECTORELEMENT_INDEX];
                if (multilayer_field.has_maximum()) {
                    int multilayermax = multilayer_field.get_maximum();
                    if (result < multilayermax) result = multilayermax;
                }
            }
        }
        return (result);
    }
    return (-999);
}
/*******************************************************************************/
int MmIdHelper::multilayerMin(const Identifier& id) const {
    ExpandedIdentifier expId;
    IdContext multilayer_context(expId, 0, m_DETECTORELEMENT_INDEX);
    if (!get_expanded_id(id, expId, &multilayer_context)) {
        int result = -999;
        for (unsigned int i = 0; i < m_full_channel_range.size(); ++i) {
            const Range& range = m_full_channel_range[i];
            if (range.match(expId)) {
                const Range::field& multilayer_field = range[m_DETECTORELEMENT_INDEX];
                if (multilayer_field.has_minimum()) {
                    int multilayermin = multilayer_field.get_minimum();
                    if (-999 == result) {
                        result = multilayermin;
                    } else {
                        if (multilayermin < result) result = multilayermin;
                    }
                }
            }
        }
        return (result);
    }
    return (999);  /// default
}
/*******************************************************************************/
int MmIdHelper::multilayerMax(const Identifier& id) const {
    ExpandedIdentifier expId;
    IdContext multilayer_context(expId, 0, m_DETECTORELEMENT_INDEX);
    if (!get_expanded_id(id, expId, &multilayer_context)) {
        int result = -999;
        for (unsigned int i = 0; i < m_full_channel_range.size(); ++i) {
            const Range& range = m_full_channel_range[i];
            if (range.match(expId)) {
                const Range::field& multilayer_field = range[m_DETECTORELEMENT_INDEX];
                if (multilayer_field.has_maximum()) {
                    int multilayermax = multilayer_field.get_maximum();
                    if (result < multilayermax) result = multilayermax;
                }
            }
        }
        return (result);
    }
    return (-999);
}
/*******************************************************************************/
int MmIdHelper::gasGapMin(const Identifier& id) const {
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
/*******************************************************************************/
int MmIdHelper::gasGapMax(const Identifier& id) const {
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
/*******************************************************************************/
int MmIdHelper::channelMin(const Identifier& id) const {
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
/*******************************************************************************/
int MmIdHelper::channelMax(const Identifier& id) const {
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
/*******************************************************************************/
// validation of levels
bool MmIdHelper::valid(const Identifier& id) const {
    if (!validElement(id)) return false;

    int mplet = multilayer(id);
    if ((mplet < multilayerMin(id)) || (mplet > multilayerMax(id))) {
        if (m_msgSvc) {
            MsgStream log(m_msgSvc, m_logName);
            log << MSG::WARNING << "Invalid multilayer=" << mplet << " multilayerMin=" << multilayerMin(id)
                << " multilayerMax=" << multilayerMax(id) << endmsg;
        }
        return false;
    }

    int gasG = gasGap(id);
    if (gasG < gasGapMin(id) || gasG > gasGapMax(id)) {
        if (m_msgSvc) {
            MsgStream log(m_msgSvc, m_logName);
            log << MSG::WARNING << "Invalid gasGap=" << gasG << " gasGapMin=" << gasGapMin(id) << " gasGapMax=" << gasGapMax(id) << endmsg;
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
}  // end MmIdHelper::valid
/*******************************************************************************/
bool MmIdHelper::isStNameInTech(const std::string& stationName) const {
    return stationName[0] == 'M';
}
bool MmIdHelper::validElement(const Identifier& id) const {
    int station = stationName(id);
    if (!validStation(station)) {
        if (m_msgSvc) {
            MsgStream log(m_msgSvc, m_logName);
            log << MSG::WARNING << "Invalid stationName=" << stationNameString(station) << endmsg;
        }
        return false;
    }

    int eta = stationEta(id);
    if (eta < stationEtaMin(id) || eta > stationEtaMax(id)) {
        if (m_msgSvc) {
            MsgStream log(m_msgSvc, m_logName);
            log << MSG::WARNING << "Invalid stationEta=" << eta << " for stationName=" << stationNameString(station) << " stationIndex=" << station
                << " stationEtaMin=" << stationEtaMin(id) << " stationEtaMax=" << stationEtaMax(id) << endmsg;
        }
        return false;
    }

    int phi = stationPhi(id);
    if (phi < stationPhiMin(id) || phi > stationPhiMax(id)) {
        if (m_msgSvc) {
            MsgStream log(m_msgSvc, m_logName);
            log << MSG::WARNING << "Invalid stationPhi=" << phi << " for stationName=" << stationNameString(station) << " stationIndex=" << station
                << " stationPhiMin=" << stationPhiMin(id) << " stationPhiMax=" << stationPhiMax(id) << endmsg;
        }
        return false;
    }
    return true;

}  // end MmIdHelper::validElement
/*******************************************************************************/
// Private validation of levels
bool MmIdHelper::validElement(const Identifier& id, int stationName, int stationEta, int stationPhi) const {
    if (!validStation(stationName)) {
        if (m_msgSvc) {
            MsgStream log(m_msgSvc, m_logName);
            log << MSG::WARNING << "Invalid stationName=" << stationNameString(stationName) << endmsg;
        }
        return false;
    }
    if (stationEta < stationEtaMin(id) || stationEta > stationEtaMax(id)) {
        if (m_msgSvc) {
            MsgStream log(m_msgSvc, m_logName);
            log << MSG::WARNING << "Invalid stationEta=" << stationEta << " for stationName=" << stationNameString(stationName) << " stationIndex=" << stationName
                << " stationEtaMin=" << stationEtaMin(id) << " stationEtaMax=" << stationEtaMax(id) << endmsg;
        }
        return false;
    }
    if (stationPhi < stationPhiMin(id) || stationPhi > stationPhiMax(id)) {
        if (m_msgSvc) {
            MsgStream log(m_msgSvc, m_logName);
            log << MSG::WARNING << "Invalid stationPhi=" << stationPhi << " for stationName=" << stationNameString(stationName) << " stationIndex=" << stationName
                << " stationPhiMin=" << stationPhiMin(id) << " stationPhiMax=" << stationPhiMax(id) << endmsg;
        }
        return false;
    }
    return true;
}  // end MmIdHelper::validElement
/*******************************************************************************/
// Check values down to readout channel level
bool MmIdHelper::validChannel(const Identifier& id, int stationName, int stationEta, int stationPhi, int multilayer, int gasGap,
                              int channel) const {
    if (!validElement(id, stationName, stationEta, stationPhi)) return false;

    if ((multilayer < multilayerMin(id)) || (multilayer > multilayerMax(id))) {
        if (m_msgSvc) {
            MsgStream log(m_msgSvc, m_logName);
            log << MSG::WARNING << "Invalid multilayer=" << multilayer << " multilayerMin=" << multilayerMin(id)
                << " multilayerMax=" << multilayerMax(id) << endmsg;
        }
        return false;
    }

    if (gasGap < gasGapMin(id) || gasGap > gasGapMax(id)) {
        if (m_msgSvc) {
            MsgStream log(m_msgSvc, m_logName);
            log << MSG::WARNING << "Invalid gasGap=" << gasGap << " gasGapMin=" << gasGapMin(id) << " gasGapMax=" << gasGapMax(id)
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
}  // end MmIdHelper::validChannel
   /*******************************************************************************/
   // Construct ID from components

Identifier MmIdHelper::elementID(int stationName, int stationEta, int stationPhi) const {
    // pack fields independently
    Identifier result((Identifier::value_type)0);
    m_muon_impl.pack(muon_field_value(), result);
    m_sta_impl.pack(stationName, result);
    m_eta_impl.pack(stationEta, result);
    m_phi_impl.pack(stationPhi, result);
    m_tec_impl.pack(mm_field_value(), result);
    return result;
}
Identifier MmIdHelper::elementID(int stationName, int stationEta, int stationPhi, bool& isValid) const  {
    const Identifier result = elementID(stationName, stationEta, stationPhi);
    isValid = validElement(result, stationName, stationEta, stationPhi);
    return result;
}

/*******************************************************************************/
Identifier MmIdHelper::elementID(const std::string& stationNameStr, int stationEta, int stationPhi) const {
    return  elementID(stationNameIndex(stationNameStr), stationEta, stationPhi);
}
Identifier MmIdHelper::elementID(const std::string& stationNameStr, int stationEta, int stationPhi, bool& isValid) const {
    return  elementID(stationNameIndex(stationNameStr), stationEta, stationPhi, isValid);
}

/*******************************************************************************/
Identifier MmIdHelper::elementID(const Identifier& id) const { return parentID(id); }
/*******************************************************************************/
Identifier MmIdHelper::channelID(int stationName, int stationEta, int stationPhi, int multilayer, int gasGap, int channel) const {
    // pack fields independently
    Identifier result((Identifier::value_type)0);
    m_muon_impl.pack(muon_field_value(), result);
    m_sta_impl.pack(stationName, result);
    m_eta_impl.pack(stationEta, result);
    m_phi_impl.pack(stationPhi, result);
    m_tec_impl.pack(mm_field_value(), result);
    m_mplet_impl.pack(multilayer, result);
    m_gap_impl.pack(gasGap, result);
    m_cha_impl.pack(channel, result);
    return result;
}
Identifier MmIdHelper::channelID(int stationName, int stationEta, int stationPhi, int multilayer, int gasGap, int channel, bool& isValid) const {
    const Identifier result = channelID(stationName, stationEta, stationPhi, multilayer, gasGap, channel);
    isValid = validChannel(result, stationName, stationEta, stationPhi, multilayer, gasGap, channel);
    return result;
}
/*******************************************************************************/
Identifier MmIdHelper::channelID(const std::string& stationNameStr, int stationEta, int stationPhi, int multilayer, int gasGap, int channel,
                                 bool& isValid) const {
    return channelID(stationNameIndex(stationNameStr), stationEta, stationPhi, multilayer, gasGap, channel, isValid);  
}
Identifier MmIdHelper::channelID(const std::string& stationNameStr, int stationEta, int stationPhi, int multilayer, int gasGap, int channel) const {
    return channelID(stationNameIndex(stationNameStr), stationEta, stationPhi, multilayer, gasGap, channel);  
}

/*******************************************************************************/
Identifier MmIdHelper::channelID(const Identifier& id, int multilayer, int gasGap, int channel) const {
    Identifier result(id);
    m_mplet_impl.pack(multilayer, result);
    m_gap_impl.pack(gasGap, result);
    m_cha_impl.pack(channel, result);
  
    return result;
}
Identifier MmIdHelper::channelID(const Identifier& id, int multilayer, int gasGap, int channel, bool& isValid) const{
  const Identifier result = channelID(id, multilayer,gasGap, channel);
  isValid = valid(result);
  return result;
}
/*******************************************************************************/
// get parent id from strip or gang identifier
Identifier MmIdHelper::parentID(const Identifier& id) const {
    assert(is_mm(id));
    Identifier result(id);
    m_mplet_impl.reset(result);
    m_gap_impl.reset(result);
    m_cha_impl.reset(result);
    return result;
}
/*******************************************************************************/
// Access to components of the ID
int MmIdHelper::multilayer(const Identifier& id) const { return m_mplet_impl.unpack(id); }
/*******************************************************************************/
int MmIdHelper::gasGap(const Identifier& id) const { return m_gap_impl.unpack(id); }
/*******************************************************************************/
int MmIdHelper::channel(const Identifier& id) const { return m_cha_impl.unpack(id); }
/*******************************************************************************/
// Access to min and max of level ranges
int MmIdHelper::stationEtaMin() const { return StationEtaMin; }
/*******************************************************************************/
int MmIdHelper::stationEtaMax() const { return StationEtaMax; }
/*******************************************************************************/
int MmIdHelper::stationPhiMin() const { return StationPhiMin; }
/*******************************************************************************/
int MmIdHelper::stationPhiMax() const { return StationPhiMax; }
/*******************************************************************************/
int MmIdHelper::multilayerMin() const { return MultilayerMin; }
/*******************************************************************************/
int MmIdHelper::multilayerMax() const { return MultilayerMax; }
/*******************************************************************************/
int MmIdHelper::gasGapMin() const { return GasGapMin; }
/*******************************************************************************/
bool MmIdHelper::isStereo(const Identifier& id) const {
    bool isStereo = false;
    int ml = multilayer(id);
    int gg = gasGap(id);
    if ((ml == 1 && gg > 2) || (ml == 2 && gg < 3)) isStereo = true;
    return isStereo;
}
/*******************************************************************************/
bool MmIdHelper::measuresPhi(const Identifier& /*id*/) const { return false; }
/*******************************************************************************/
int MmIdHelper::gasGapMax() const { return GasGapMax; }
/*******************************************************************************/
int MmIdHelper::channelMin() const { return ChannelMin; }
/*******************************************************************************/
int MmIdHelper::channelMax() const { return ChannelMax; }
/*******************************************************************************/
/// Utility methods
int MmIdHelper::mmTechnology() const {
    int mmField = technologyIndex("MM");
    if (m_dict) { mmField = mm_field_value(); }
    return mmField;
}
/*******************************************************************************/
bool MmIdHelper::LargeSector(int stationName) const { return ('L' == stationNameString(stationName)[2]); }
/*******************************************************************************/
bool MmIdHelper::SmallSector(int stationName) const { return ('S' == stationNameString(stationName)[2]); }
/*******************************************************************************/
// Nektar: Modified for MicroMegas, but is almost certainly wrong
int MmIdHelper::sectorType(std::string stationName, int stationEta) const {
    if ('L' == stationName[2]) {
        return (abs(stationEta) + 1);
    } else if ('S' == stationName[2]) {
        return (abs(stationEta) + 12);
    }
    assert(0);
    return -1;
}
/*******************************************************************************/
int MmIdHelper::sectorType(int stationName, int stationEta) const {
    std::string name = stationNameString(stationName);
    return sectorType(name, stationEta);
}
