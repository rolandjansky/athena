/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#include "MuonIdHelpers/CscIdHelper.h"

#include <mutex>

#include "AthenaKernel/getMessageSvc.h"

CscIdHelper::CscIdHelper() :
    MuonIdHelper("CscIdHelper"),
    m_CHAMBERLAYER_INDEX(0),
    m_WIRELAYER_INDEX(0),
    m_MEASURESPHI_INDEX(0),
    m_stripMaxPhi(UINT_MAX),
    m_stripMaxEta(UINT_MAX),
    m_hasChamLay1(false) {}

/// Initialize dictionary
int CscIdHelper::initialize_from_dictionary(const IdDictMgr& dict_mgr) {
    int status = 0;

    MsgStream log(m_msgSvc, m_logName);

    // Check whether this helper should be reinitialized
    if (!reinitialize(dict_mgr)) {
        if (m_msgSvc) { log << MSG::INFO << "Request to reinitialize not satisfied - tags have not changed" << endmsg; }
        return (0);
    } else {
        if (m_msgSvc) { log << MSG::DEBUG << "(Re)initialize" << endmsg; }
    }

    /// init base object
    AtlasDetectorID::setMessageSvc(Athena::getMessageSvc());
    if (AtlasDetectorID::initialize_from_dictionary(dict_mgr)) return (1);

    // Register version of the MuonSpectrometer dictionary
    if (register_dict_tag(dict_mgr, "MuonSpectrometer")) return (1);

    m_dict = dict_mgr.find_dictionary("MuonSpectrometer");
    if (!m_dict) {
        if (m_msgSvc) { log << MSG::ERROR << " initialize_from_dict - cannot access MuonSpectrometer dictionary " << endmsg; }
        return 1;
    }

    /// Initialize some of the field indices

    if (initLevelsFromDict()) return (1);

    int index = technologyIndex("CSC");
    if (index == -1) {
        if (m_msgSvc) { log << MSG::DEBUG << "initLevelsFromDict - there are no CSC entries in the dictionary! " << endmsg; }
        return 0;
    }

    IdDictField* field = m_dict->find_field("chamberLayer");
    if (field) {
        m_CHAMBERLAYER_INDEX = field->m_index;
    } else {
        if (m_msgSvc) { log << MSG::ERROR << "initLevelsFromDict - unable to find 'chamberLayer' field " << endmsg; }
        status = 1;
    }

    field = m_dict->find_field("wireLayer");
    if (field) {
        m_WIRELAYER_INDEX = field->m_index;
    } else {
        if (m_msgSvc) { log << MSG::ERROR << "initLevelsFromDict - unable to find 'wireLayer' field " << endmsg; }
        status = 1;
    }

    field = m_dict->find_field("cscMeasuresPhi");
    if (field) {
        m_MEASURESPHI_INDEX = field->m_index;
    } else {
        if (m_msgSvc) { log << MSG::ERROR << "initLevelsFromDict - unable to find 'cscMeasuresPhi' field " << endmsg; }
        status = 1;
    }

    field = m_dict->find_field("cscStrip");
    if (field) {
        m_CHANNEL_INDEX = field->m_index;
    } else {
        if (m_msgSvc) { log << MSG::ERROR << "initLevelsFromDict - unable to find 'cscStrip' field " << endmsg; }
        status = 1;
    }

    // initialize the multilayer index
    m_DETECTORELEMENT_INDEX = m_CHAMBERLAYER_INDEX;

    /// save an index to the first region of csc

    IdDictGroup* cscGroup = m_dict->find_group("csc");
    if (!cscGroup) {
        if (m_msgSvc) { log << MSG::ERROR << "Cannot find csc group" << endmsg; }
    } else {
        m_GROUP_INDEX = cscGroup->regions()[0]->m_index;
    }

    const IdDictRegion& region = *m_dict->m_regions[m_GROUP_INDEX];
    m_eta_impl = region.m_implementation[m_ETA_INDEX];
    m_phi_impl = region.m_implementation[m_PHI_INDEX];
    m_tec_impl = region.m_implementation[m_TECHNOLOGY_INDEX];
    m_cla_impl = region.m_implementation[m_CHAMBERLAYER_INDEX];
    m_lay_impl = region.m_implementation[m_WIRELAYER_INDEX];
    m_mea_impl = region.m_implementation[m_MEASURESPHI_INDEX];
    m_str_impl = region.m_implementation[m_CHANNEL_INDEX];

    if (m_msgSvc) {
        log << MSG::DEBUG << " CSC decode index and bit fields for each level: " << endmsg;
        log << MSG::DEBUG << " muon        " << m_muon_impl.show_to_string() << endmsg;
        log << MSG::DEBUG << " station     " << m_sta_impl.show_to_string() << endmsg;
        log << MSG::DEBUG << " eta         " << m_eta_impl.show_to_string() << endmsg;
        log << MSG::DEBUG << " phi         " << m_phi_impl.show_to_string() << endmsg;
        log << MSG::DEBUG << " technology  " << m_tec_impl.show_to_string() << endmsg;
        log << MSG::DEBUG << " cham layer  " << m_cla_impl.show_to_string() << endmsg;
        log << MSG::DEBUG << " layer       " << m_lay_impl.show_to_string() << endmsg;
        log << MSG::DEBUG << " phi         " << m_mea_impl.show_to_string() << endmsg;
        log << MSG::DEBUG << " strip       " << m_str_impl.show_to_string() << endmsg;
    }

    /**
     * Build multirange for the valid set of identifiers
     */

    /// Find value for the field MuonSpectrometer

    int muonField = -1;
    const IdDictDictionary* atlasDict = dict_mgr.find_dictionary("ATLAS");
    if (atlasDict->get_label_value("subdet", "MuonSpectrometer", muonField)) {
        if (m_msgSvc) {
            log << MSG::ERROR << "Could not get value for label 'MuonSpectrometer' of field "
                << "'subdet' in dictionary " << atlasDict->m_name << endmsg;
        }
        return (1);
    }

    /// Build MultiRange down to "technology" for all (muon) regions

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
        if (m_msgSvc) { log << MSG::ERROR << "Muon MultiRange is empty for modules" << endmsg; }
    }

    // Build MultiRange down to "detector element" for all mdt regions

    ExpandedIdentifier detectorElement_region;
    detectorElement_region.add(muonField);
    Range detectorElement_prefix;
    MultiRange muon_detectorElement_range = m_dict->build_multirange(detectorElement_region, detectorElement_prefix, "chamberLayer");
    if (muon_detectorElement_range.size() > 0) {
        if (m_msgSvc) {
            log << MSG::INFO << "MultiRange built successfully to detector element: "
                << "Multilayer MultiRange size is " << muon_detectorElement_range.size() << endmsg;
        }
    } else {
        if (m_msgSvc) { log << MSG::ERROR << "Muon CSC detector element MultiRange is empty" << endmsg; }
    }

    /// Build MultiRange down to "cscStrip" for all CSC regions

    ExpandedIdentifier csc_region;
    csc_region.add(muonField);
    Range csc_prefix;
    MultiRange muon_channel_range = m_dict->build_multirange(csc_region, csc_prefix, "cscStrip");
    if (muon_channel_range.size() > 0) {
        if (m_msgSvc) {
            log << MSG::INFO << "MultiRange built successfully to cscStrip: "
                << "MultiRange size is " << muon_channel_range.size() << endmsg;
        }
    } else {
        if (m_msgSvc) { log << MSG::ERROR << "Muon MultiRange is empty for channels" << endmsg; }
    }

    /**
     * Build CSC module ranges:
     *
     * Find the regions that have a "technology field" that matches the MDT
     * and save them
     */

    int cscField = -1;
    status = m_dict->get_label_value("technology", "CSC", cscField);

    for (int i = 0; i < (int)muon_range.size(); ++i) {
        const Range& range = muon_range[i];
        if (range.fields() > m_TECHNOLOGY_INDEX) {
            const Range::field& field = range[m_TECHNOLOGY_INDEX];
            if (field.match((ExpandedIdentifier::element_type)cscField)) {
                m_full_module_range.add(range);
                if (m_msgSvc) { log << MSG::DEBUG << "field size is " << (int)range.cardinality() << " field index = " << i << endmsg; }
            }
        }
    }

    for (int j = 0; j < (int)muon_detectorElement_range.size(); ++j) {
        const Range& range = muon_detectorElement_range[j];
        if (range.fields() > m_TECHNOLOGY_INDEX) {
            const Range::field& field = range[m_TECHNOLOGY_INDEX];
            if (field.match((ExpandedIdentifier::element_type)cscField)) {
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
            if (field.match((ExpandedIdentifier::element_type)cscField)) {
                m_full_channel_range.add(range);
                if (m_msgSvc) {
                    log << MSG::DEBUG << "channel field size is " << (int)range.cardinality() << " field index = " << j << endmsg;
                }
            }
        }
    }

    /// test to see that the multi range is not empty

    if (m_full_module_range.size() == 0) {
        if (m_msgSvc) { log << MSG::ERROR << "CSC MultiRange ID is empty for modules" << endmsg; }
        status = 1;
    }

    /// test to see that the detector element multi range is not empty

    if (m_full_detectorElement_range.size() == 0) {
        if (m_msgSvc) { log << MSG::ERROR << "CSC MultiRange ID is empty for detector elements" << endmsg; }
        status = 1;
    }

    /// test to see that the multi range is not empty

    if (m_full_channel_range.size() == 0) {
        if (m_msgSvc) { log << MSG::ERROR << "CSC MultiRange ID is empty for channels" << endmsg; }
        status = 1;
    }

    // initialise the arrays of the geometrical module and detectorElement hashes to values !=0
    // otherwise, it will be impossible to distinguish the hash 0 from an uninitialized array element
    unsigned int* modhash = &m_module_hashes[0][0][0];
    const unsigned int* end_modhash = modhash + sizeof(m_module_hashes) / sizeof(unsigned int);
    for (; modhash != end_modhash; ++modhash) { (*modhash) = UINT_MAX; }
    unsigned int* dethash = &m_detectorElement_hashes[0][0][0][0];
    const unsigned int* end_dethash = dethash + sizeof(m_detectorElement_hashes) / sizeof(unsigned int);
    for (; dethash != end_dethash; ++dethash) { (*dethash) = UINT_MAX; }

    /// Setup the hash tables for CSC

    if (m_msgSvc) { log << MSG::INFO << "Initializing CSC hash indices ... " << endmsg; }
    status = init_hashes();
    status = init_detectorElement_hashes();  // for chamber layer - a chamber
    status = init_channel_hashes();
    status = strip_hash_offsets();
    status = init_id_to_hashes();

    /// Setup hash tables for finding neighbors

    if (m_msgSvc) { log << MSG::INFO << "Initializing CSC hash indices for finding neighbors ... " << endmsg; }
    status = init_neighbors();

    // now we have to set the stripMax values (for the stripMax(id) function)
    // this could be also done on an event-by-event basis as for stripMin(id)
    // however, for all existing layouts there are only 2 possible values for stripMax,
    // namely those for layers which measure phi (measuresPhi(id)=true) and the rest.
    // thus, we initialize 2 member variables here to speed up calling the function during runtime
    // loop on the channel Identifiers and check (for consistency!) that really only
    // two maximum numbers of strips (w/o measuresPhi) are around
    ExpandedIdentifier expId;
    IdContext strip_context = channel_context();
    for (const auto& id : m_channel_vec) {
        if (get_expanded_id(id, expId, &strip_context)) {
            if (m_msgSvc) { log << MSG::ERROR << "Failed to retrieve ExpandedIdentifier from Identifier " << id.get_compact() << endmsg; }
            return 1;
        }
        bool measuresPhi = this->measuresPhi(id);
        for (unsigned int i = 0; i < m_full_channel_range.size(); ++i) {
            const Range& range = m_full_channel_range[i];
            if (range.match(expId)) {
                const Range::field& phi_field = range[m_CHANNEL_INDEX];
                if (!phi_field.has_maximum()) {
                    if (m_msgSvc) { log << MSG::ERROR << "Range::field for phi at position " << i << " does not have a maximum" << endmsg; }
                    return 1;
                }
                unsigned int max = phi_field.get_maximum();
                if (measuresPhi) {
                    if (m_stripMaxPhi != UINT_MAX && m_stripMaxPhi != max) {
                        if (m_msgSvc) {
                            log << MSG::ERROR << "Maximum of Range::field for phi (" << max
                                << ") is not equal to m_stripMaxPhi=" << m_stripMaxPhi << endmsg;
                        }
                        return 1;
                    } else
                        m_stripMaxPhi = max;
                } else {
                    if (m_stripMaxEta != UINT_MAX && m_stripMaxEta != max) {
                        if (m_msgSvc) {
                            log << MSG::ERROR << "Maximum of Range::field for phi (" << max
                                << ") is not equal to m_stripMaxEta=" << m_stripMaxEta << endmsg;
                        }
                        return 1;
                    } else
                        m_stripMaxEta = max;
                }
            }
        }
    }

    // check whether the current layout contains chamberLayer 1 Identifiers (pre-Run3) in the vector of module Identifiers
    if (m_module_vec.size() && chamberLayer(m_module_vec.at(0)) == 1) m_hasChamLay1 = true;
    m_init = true;
    return (status);
}

int CscIdHelper::init_id_to_hashes() {
    unsigned int hash_max = this->module_hash_max();
    for (unsigned int i = 0; i < hash_max; ++i) {
        Identifier id = m_module_vec[i];
        int station = this->stationName(id);
        int eta = this->stationEta(id) + 2;  // for negative etas
        int phi = this->stationPhi(id);
        m_module_hashes[station][eta - 1][phi - 1] = i;
    }

    hash_max = this->detectorElement_hash_max();
    for (unsigned int i = 0; i < hash_max; ++i) {
        Identifier id = m_detectorElement_vec[i];
        int station = this->stationName(id);
        int eta = this->stationEta(id) + 2;  // for negative eta
        int phi = this->stationPhi(id);
        int multilayer = this->chamberLayer(id);
        m_detectorElement_hashes[station][eta - 1][phi - 1][multilayer - 1] = i;
    }

    return 0;
}

int CscIdHelper::get_module_hash(const Identifier& id, IdentifierHash& hash_id) const {
    // if the current layout contains Identifiers for CSC chamberLayer 1, need to call the
    // get_module_hash function with an Identifier which is actually from chamberLayer 1
    if (m_hasChamLay1) return MuonIdHelper::get_module_hash(parentID(id), hash_id);
    // otherwise just use the default implementation
    return MuonIdHelper::get_module_hash(id, hash_id);
}
int CscIdHelper::get_detectorElement_hash(const Identifier& id, IdentifierHash& hash_id) const {
    // if the current layout contains Identifiers for CSC chamberLayer 1, need to call the
    // get_detectorElement_hash function with an Identifier which is actually from chamberLayer 1
    if (m_hasChamLay1) return MuonIdHelper::get_detectorElement_hash(parentID(id), hash_id);
    // otherwise just use the default implementation
    return MuonIdHelper::get_detectorElement_hash(id, hash_id);
}

int CscIdHelper::get_geo_module_hash(const Identifier& id, IdentifierHash& hash_id) const {
    int station = this->stationName(id);
    int eta = this->stationEta(id) + 2;  // for negative etas
    int phi = this->stationPhi(id);
    hash_id = m_module_hashes[station][eta - 1][phi - 1];
    return 0;
}

int CscIdHelper::get_geo_detectorElement_hash(const Identifier& id, IdentifierHash& hash_id) const {
    // Identifier multilayerId = multilayerID(id);
    // IdContext context = multilayer_context();
    // return get_hash(multilayerId,hash_id,&context);
    int station = this->stationName(id);
    int eta = this->stationEta(id) + 2;  // for negative eta
    int phi = this->stationPhi(id);
    int multilayer = this->chamberLayer(id);
    hash_id = m_detectorElement_hashes[station][eta - 1][phi - 1][multilayer - 1];
    return 0;
}

int CscIdHelper::get_geo_channel_hash(const Identifier& id, IdentifierHash& hash_id) const {
    const IdContext context = this->channel_context();
    return get_geo_hash_calc(id, hash_id, &context);
}
int CscIdHelper::get_hash_fromGeoHash(const IdentifierHash& geoHash, IdentifierHash& realHash, const IdContext* context) const {
    int result = 1;
    size_t begin = (context) ? context->begin_index() : 0;
    // cannot get hash if end is 0:
    size_t end = (context) ? context->end_index() : 0;
    if (0 == begin) {
        Identifier id;
        // No hashes yet for ids with prefixes
        if (m_MODULE_INDEX == end) {
            // need to compute module identifier from geoHash (inversion of get_geo_module_hash())
            // m_module_hashes[station][eta-1][phi-1];
            // array size: m_module_hashes[60][3][8]
            for (unsigned int station = 50; station < 52; ++station) {  // only check stations 50/51 since those are CSCs
                for (unsigned int eta = 0; eta < 3; ++eta) {            // stationEtaMax() ranges from 0 to 1
                    for (unsigned int phi = 0; phi < (unsigned int)stationPhiMax(); ++phi) {
                        if (geoHash == m_module_hashes[station][eta][phi]) {
                            id = this->elementID(station, eta - 1, phi + 1);
                            break;
                        }
                    }
                }
            }
            result = MuonIdHelper::get_module_hash(id, realHash);
        } else if (m_DETECTORELEMENT_INDEX == end) {
            // need to compute detector-element identifier from geoHash (inversion of get_geo_detectorElement_hash())
            // hash_id = m_detectorElement_hashes[station][eta-1][phi-1][multilayer-1];
            // array size: m_detectorElement_hashes[60][3][8][2]
            for (unsigned int station = 50; station < 52; ++station) {  // only check stations 50/51 since those are CSCs
                for (unsigned int eta = 0; eta < 3; ++eta) {            // stationEtaMax() ranges from 0 to 1
                    for (unsigned int phi = 0; phi < (unsigned int)stationPhiMax(); ++phi) {
                        for (unsigned int ml = 0; ml < (unsigned int)chamberLayerMax(); ++ml) {
                            if (geoHash == m_detectorElement_hashes[station][eta][phi][ml]) {
                                // take wireLayer=strip=1, measuresPhi=0
                                id = this->channelID(station, eta - 1, phi + 1, ml + 1, 1, 0, 1);
                                break;
                            }
                        }
                    }
                }
            }
            result = MuonIdHelper::get_detectorElement_hash(id, realHash);
        } else if (m_CHANNEL_INDEX == end) {
            // need to compute channel identifier from geoHash (inversion of get_geo_hash_calc())
            // hash_id = offset +
            // (stripNumber-1)+maxStrip*(wireLayer-1)+4*maxStrip*(chamberLayer-1)+8*maxStrip*(phi-1)+64*maxStrip*(etaIndex-1); first, find
            // offset:
            unsigned int offset = 0;
            int chamberType(0), orientation(0);
            if (geoHash < (unsigned int)m_hashOffset[0][1]) {
                offset = 0;
                chamberType = 0;
                orientation = 0;
            } else if (geoHash < (unsigned int)m_hashOffset[1][0]) {
                offset = m_hashOffset[0][1];
                chamberType = 0;
                orientation = 1;
            } else if (geoHash < (unsigned int)m_hashOffset[1][1]) {
                offset = m_hashOffset[1][0];
                chamberType = 1;
                orientation = 0;
            } else {
                offset = m_hashOffset[1][1];
                chamberType = 1;
                orientation = 1;
            }
            unsigned int geoHashNoOff = geoHash - offset;
            // orientation equals measuresPhi
            // second, get maxStrip by just using the member variables set in initialize()
            int maxStrip = (orientation) ? m_stripMaxPhi : m_stripMaxEta;
            // next, get the station (cf. get_geo_hash_calc()):
            // if chamberType is 0, it is a small station, i.e. station 50, else 51
            // (cf. DetectorDescription/IdDictParser/data/IdDictMuonSpectrometer_R.03.xml)
            int station = (chamberType) ? 51 : 50;
            // now, invert get_geo_hash_calc()
            int etaIndex = (int)(geoHashNoOff) / (64 * maxStrip);
            geoHashNoOff -= etaIndex * 64 * maxStrip;
            int phiIndex = (int)(geoHashNoOff) / (8 * maxStrip);
            geoHashNoOff -= phiIndex * 8 * maxStrip;
            int chamberLayerIndex = (int)(geoHashNoOff) / (4 * maxStrip);
            geoHashNoOff -= chamberLayerIndex * 4 * maxStrip;
            int wireLayerIndex = (int)(geoHashNoOff) / (maxStrip);
            geoHashNoOff -= wireLayerIndex * maxStrip;
            int eta = (etaIndex == 0) ? -1 : 1;
            id = this->channelID(station, eta, phiIndex + 1, chamberLayerIndex + 1, wireLayerIndex + 1, orientation, geoHashNoOff + 1);
            result = MuonIdHelper::get_hash(id, realHash, context);
        }
    } else
        realHash = UINT_MAX;
    return result;
}

void CscIdHelper::idChannels(const Identifier& id, std::vector<Identifier>& vect) const {
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

int CscIdHelper::stationEtaMin(const Identifier& id) const {
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
    return (999);  /// default
}

int CscIdHelper::stationEtaMax(const Identifier& id) const {
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

int CscIdHelper::stationPhiMin(const Identifier& id) const {
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
    /// Failed to find the min
    return (999);
}

int CscIdHelper::stationPhiMax(const Identifier& id) const {
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
    /// Failed to find the max
    return (-999);
}

int CscIdHelper::chamberLayerMin(const Identifier& id) const {
    ExpandedIdentifier expId;
    IdContext chamberLayer_context(expId, 0, m_CHAMBERLAYER_INDEX);
    if (!get_expanded_id(id, expId, &chamberLayer_context)) {
        for (unsigned int i = 0; i < m_full_channel_range.size(); ++i) {
            const Range& range = m_full_channel_range[i];
            if (range.match(expId)) {
                const Range::field& phi_field = range[m_CHAMBERLAYER_INDEX];
                if (phi_field.has_minimum()) { return (phi_field.get_minimum()); }
            }
        }
    }
    /// Failed to find the min
    return (999);
}

int CscIdHelper::chamberLayerMax(const Identifier& id) const {
    ExpandedIdentifier expId;
    IdContext chamberLayer_context(expId, 0, m_CHAMBERLAYER_INDEX);
    if (!get_expanded_id(id, expId, &chamberLayer_context)) {
        for (unsigned int i = 0; i < m_full_channel_range.size(); ++i) {
            const Range& range = m_full_channel_range[i];
            if (range.match(expId)) {
                const Range::field& phi_field = range[m_CHAMBERLAYER_INDEX];
                if (phi_field.has_maximum()) { return (phi_field.get_maximum()); }
            }
        }
    }
    /// Failed to find the max
    return (-999);
}

int CscIdHelper::wireLayerMin(const Identifier& id) const {
    ExpandedIdentifier expId;
    IdContext wireLayer_context(expId, 0, m_WIRELAYER_INDEX);
    if (!get_expanded_id(id, expId, &wireLayer_context)) {
        for (unsigned int i = 0; i < m_full_channel_range.size(); ++i) {
            const Range& range = m_full_channel_range[i];
            if (range.match(expId)) {
                const Range::field& phi_field = range[m_WIRELAYER_INDEX];
                if (phi_field.has_minimum()) { return (phi_field.get_minimum()); }
            }
        }
    }
    /// Failed to find the min
    return (999);
}

int CscIdHelper::wireLayerMax(const Identifier& id) const {
    ExpandedIdentifier expId;
    IdContext chamberLayer_context(expId, 0, m_WIRELAYER_INDEX);
    if (!get_expanded_id(id, expId, &chamberLayer_context)) {
        for (unsigned int i = 0; i < m_full_channel_range.size(); ++i) {
            const Range& range = m_full_channel_range[i];
            if (range.match(expId)) {
                const Range::field& phi_field = range[m_WIRELAYER_INDEX];
                if (phi_field.has_maximum()) { return (phi_field.get_maximum()); }
            }
        }
    }
    /// Failed to find the max
    return (-999);
}

int CscIdHelper::measuresPhiMin(const Identifier& id) const {
    ExpandedIdentifier expId;
    IdContext measuresPhi_context(expId, 0, m_MEASURESPHI_INDEX);
    if (!get_expanded_id(id, expId, &measuresPhi_context)) {
        for (unsigned int i = 0; i < m_full_channel_range.size(); ++i) {
            const Range& range = m_full_channel_range[i];
            if (range.match(expId)) {
                const Range::field& phi_field = range[m_MEASURESPHI_INDEX];
                if (phi_field.has_minimum()) { return (phi_field.get_minimum()); }
            }
        }
    }
    /// Failed to find the min
    return (999);
}

int CscIdHelper::measuresPhiMax(const Identifier& id) const {
    ExpandedIdentifier expId;
    IdContext measuresPhi_context(expId, 0, m_MEASURESPHI_INDEX);
    if (!get_expanded_id(id, expId, &measuresPhi_context)) {
        for (unsigned int i = 0; i < m_full_channel_range.size(); ++i) {
            const Range& range = m_full_channel_range[i];
            if (range.match(expId)) {
                const Range::field& phi_field = range[m_MEASURESPHI_INDEX];
                if (phi_field.has_maximum()) { return (phi_field.get_maximum()); }
            }
        }
    }
    /// Failed to find the max
    return (-999);
}

int CscIdHelper::stripMin(const Identifier& id) const {
    ExpandedIdentifier expId;
    IdContext strip_context = channel_context();
    if (!get_expanded_id(id, expId, &strip_context)) {
        for (unsigned int i = 0; i < m_full_channel_range.size(); ++i) {
            const Range& range = m_full_channel_range[i];
            if (range.match(expId)) {
                const Range::field& phi_field = range[m_CHANNEL_INDEX];
                if (phi_field.has_minimum()) { return (phi_field.get_minimum()); }
            }
        }
    }
    /// Failed to find the min
    return (999);
}

int CscIdHelper::stripMax(const Identifier& id) const {
    if (measuresPhi(id))
        return m_stripMaxPhi;
    else
        return m_stripMaxEta;
}

/// Public validation of levels

bool CscIdHelper::valid(const Identifier& id) const {
    if (!validElement(id)) return false;

    int cLayer = chamberLayer(id);
    if ((cLayer < chamberLayerMin(id)) || (cLayer > chamberLayerMax(id))) {
        if (m_msgSvc) {
            MsgStream log(m_msgSvc, m_logName);
            log << MSG::WARNING << "Invalid chamberLayer=" << cLayer << " chamberLayerMin=" << chamberLayerMin(id)
                << " chamberLayerMax=" << chamberLayerMax(id) << endmsg;
        }
        return false;
    }

    int wLayer = wireLayer(id);
    if ((wLayer < wireLayerMin(id)) || (wLayer > wireLayerMax(id))) {
        if (m_msgSvc) {
            MsgStream log(m_msgSvc, m_logName);
            log << MSG::WARNING << "Invalid wireLayer=" << wLayer << " wireLayerMin=" << wireLayerMin(id)
                << " wireLayerMax=" << wireLayerMax(id) << endmsg;
        }
        return false;
    }

    int mPhi = measuresPhi(id);
    if ((mPhi < measuresPhiMin(id)) || (mPhi > measuresPhiMax(id))) {
        if (m_msgSvc) {
            MsgStream log(m_msgSvc, m_logName);
            log << MSG::WARNING << "Invalid measuresPhi=" << mPhi << " measuresPhiMin=" << measuresPhiMin(id)
                << " measuresPhiMax=" << measuresPhiMax(id) << endmsg;
        }
        return false;
    }

    int channel = strip(id);
    if ((channel > stripMax(id)) || (channel < stripMin(id))) {
        if (m_msgSvc) {
            MsgStream log(m_msgSvc, m_logName);
            log << MSG::WARNING << "Invalid strip=" << channel << " stripMin=" << stripMin(id) << " stripMax=" << stripMax(id) << endmsg;
        }
        return false;
    }
    return true;
}

bool CscIdHelper::validElement(const Identifier& id) const {
    int station = stationName(id);
    std::string name = stationNameString(station);
    if ('C' != name[0]) {
        if (m_msgSvc) {
            MsgStream log(m_msgSvc, m_logName);
            log << MSG::WARNING << "Invalid stationName=" << name << endmsg;
        }
        return false;
    }

    int eta = stationEta(id);
    if ((eta < stationEtaMin(id)) || (eta > stationEtaMax(id)) || (0 == eta)) {
        if (m_msgSvc) {
            MsgStream log(m_msgSvc, m_logName);
            log << MSG::WARNING << "Invalid stationEta=" << eta << " for stationName=" << name << " stationEtaMin=" << stationEtaMin(id)
                << " stationEtaMax=" << stationEtaMax(id) << endmsg;
        }
        return false;
    }

    int phi = stationPhi(id);
    if ((phi < stationPhiMin(id)) || (phi > stationPhiMax(id))) {
        if (m_msgSvc) {
            MsgStream log(m_msgSvc, m_logName);
            log << MSG::WARNING << "Invalid stationPhi=" << phi << " for stationName=" << name << " stationPhiMin=" << stationPhiMin(id)
                << " stationPhiMax=" << stationPhiMax(id) << endmsg;
        }
        return false;
    }
    return true;
}

/// Private validation of levels

bool CscIdHelper::validElement(const Identifier& id, int stationName, int stationEta, int stationPhi) const {
    std::string name = stationNameString(stationName);

    if ('C' != name[0]) {
        if (m_msgSvc) {
            MsgStream log(m_msgSvc, m_logName);
            log << MSG::WARNING << "Invalid stationName=" << name << endmsg;
        }
        return false;
    }
    if ((stationEta < stationEtaMin(id)) || (stationEta > stationEtaMax(id)) || (0 == stationEta)) {
        static std::once_flag flag;
        std::call_once(flag, [&]() {
            if (m_msgSvc) {
                MsgStream log(m_msgSvc, m_logName);
                log << MSG::WARNING << "Invalid stationEta=" << stationEta << " for stationName=" << name
                    << " stationEtaMin=" << stationEtaMin(id) << " stationEtaMax=" << stationEtaMax(id) << endmsg;
            }
        });
        return false;
    }
    if ((stationPhi < stationPhiMin(id)) || (stationPhi > stationPhiMax(id))) {
        if (m_msgSvc) {
            MsgStream log(m_msgSvc, m_logName);
            log << MSG::WARNING << "Invalid stationPhi=" << stationPhi << " for stationName=" << name
                << " stationPhiMin=" << stationPhiMin(id) << " stationPhiMax=" << stationPhiMax(id) << endmsg;
        }
        return false;
    }
    return true;
}

bool CscIdHelper::validChannel(const Identifier& id, int stationName, int stationEta, int stationPhi, int chamberLayer, int wireLayer,
                               int measuresPhi, int strip) const {
    if (!validElement(id, stationName, stationEta, stationPhi)) return false;

    if ((chamberLayer < chamberLayerMin(id)) || (chamberLayer > chamberLayerMax(id))) {
        if (m_msgSvc) {
            MsgStream log(m_msgSvc, m_logName);
            log << MSG::WARNING << "Invalid chamberLayer=" << chamberLayer << " chamberLayerMin=" << chamberLayerMin(id)
                << " chamberLayerMax=" << chamberLayerMax(id) << endmsg;
        }
        return false;
    }
    if ((wireLayer < wireLayerMin(id)) || (wireLayer > wireLayerMax(id))) {
        if (m_msgSvc) {
            MsgStream log(m_msgSvc, m_logName);
            log << MSG::WARNING << "Invalid wireLayer=" << wireLayer << " wireLayerMin=" << wireLayerMin(id)
                << " wireLayerMax=" << wireLayerMax(id) << endmsg;
        }
        return false;
    }
    if ((measuresPhi < measuresPhiMin(id)) || (measuresPhi > measuresPhiMax(id))) {
        if (m_msgSvc) {
            MsgStream log(m_msgSvc, m_logName);
            log << MSG::WARNING << "Invalid measuresPhi=" << measuresPhi << " measuresPhiMin=" << measuresPhiMin(id)
                << " measuresPhiMax=" << measuresPhiMax(id) << endmsg;
        }
        return false;
    }
    if ((strip > stripMax(id)) || (strip < stripMin(id))) {
        if (m_msgSvc) {
            MsgStream log(m_msgSvc, m_logName);
            log << MSG::WARNING << "Invalid strip=" << strip << " stripMin=" << stripMin(id) << " stripMax=" << stripMax(id) << endmsg;
        }
        return false;
    }
    return true;
}

// Create hash from compact
int CscIdHelper::get_geo_hash_calc(const Identifier& compact_id, IdentifierHash& hash_id, const IdContext* context) const {
    // Get the hash code from vec (for wafers only).
    int result = 1;
    hash_id = 0;
    size_t begin = (context) ? context->begin_index() : 0;
    size_t end = (context) ? context->end_index() : 0;

    if (0 == begin) {
        // No hashes yet for ids with prefixes
        if (m_MODULE_INDEX == end) {
            result = get_geo_module_hash(compact_id, hash_id);
        } else if (m_DETECTORELEMENT_INDEX == end) {
            result = get_geo_detectorElement_hash(compact_id, hash_id);
        } else if (m_CHANNEL_INDEX == end) {
            int stationIndex = stationName(compact_id);
            int eta = this->stationEta(compact_id);
            int phi = this->stationPhi(compact_id);
            int chamberLayer = this->chamberLayer(compact_id);
            int wireLayer = this->wireLayer(compact_id);
            int orientation = this->measuresPhi(compact_id);
            int stripNumber = this->strip(compact_id);
            int etaIndex = (eta == -1) ? 1 : 2;
            int chamberType = -1;
            isSmall(stationIndex) ? chamberType = 0 : chamberType = 1;
            int maxStrip = this->stripMax(compact_id);
            int offset = m_hashOffset[chamberType][orientation];

            hash_id = offset + (stripNumber - 1) + maxStrip * (wireLayer - 1) + 4 * maxStrip * (chamberLayer - 1) +
                      8 * maxStrip * (phi - 1) + 64 * maxStrip * (etaIndex - 1);

            result = 0;
        }
    }
    return result;
}

// calculate the hash offset
int CscIdHelper::strip_hash_offsets() {
    m_hashOffset[0][0] = 0;
    std::string version = m_dict->m_version;

    if (version == "H8 2004") {
        m_hashOffset[0][1] = 1536;
        m_hashOffset[1][0] = m_hashOffset[0][0];
        m_hashOffset[1][1] = m_hashOffset[0][1];
    } else if (version == "CSC Cosmic") {
        m_hashOffset[0][1] = 3072;
        m_hashOffset[1][0] = m_hashOffset[0][0];
        m_hashOffset[1][1] = m_hashOffset[0][1];
    } else if (version == "P.03" || version == "H8 2003" || version == "H8 2002" || version == "M2.8") {
        m_hashOffset[0][1] = 27392;
        m_hashOffset[1][0] = m_hashOffset[0][1] + 3584;
        m_hashOffset[1][1] = m_hashOffset[0][1] + m_hashOffset[1][0];
    } else {
        m_hashOffset[0][1] = 24576;
        m_hashOffset[1][0] = m_hashOffset[0][1] + 6144;
        m_hashOffset[1][1] = m_hashOffset[0][1] + m_hashOffset[1][0];
    }
    return 0;
}
Identifier CscIdHelper::elementID(int stationName, int stationEta, int stationPhi, bool check, bool* isValid) const {
    // pack fields independently
    Identifier result((Identifier::value_type)0);
    bool val = false;
    m_muon_impl.pack(muon_field_value(), result);
    m_sta_impl.pack(stationName, result);
    m_eta_impl.pack(stationEta, result);
    m_phi_impl.pack(stationPhi, result);
    m_tec_impl.pack(csc_field_value(), result);
    if (check) {
        val = this->validElement(result, stationName, stationEta, stationPhi);
        if (isValid) *isValid = val;
    }
    return result;
}

Identifier CscIdHelper::elementID(std::string_view stationNameStr, int stationEta, int stationPhi, bool check, bool* isValid) const {
    Identifier id;
    int stationName = stationNameIndex(stationNameStr);
    id = elementID(stationName, stationEta, stationPhi, check, isValid);
    return id;
}

Identifier CscIdHelper::elementID(const Identifier& id) const { return parentID(id); }

Identifier CscIdHelper::channelID(int stationName, int stationEta, int stationPhi, int chamberLayer, int wireLayer, int measuresPhi,
                                  int strip, bool check, bool* isValid) const {
    // pack fields independently
    Identifier result((Identifier::value_type)0);
    bool val = false;
    m_muon_impl.pack(muon_field_value(), result);
    m_sta_impl.pack(stationName, result);
    m_eta_impl.pack(stationEta, result);
    m_phi_impl.pack(stationPhi, result);
    m_tec_impl.pack(csc_field_value(), result);
    m_cla_impl.pack(chamberLayer, result);
    m_lay_impl.pack(wireLayer, result);
    m_mea_impl.pack(measuresPhi, result);
    m_str_impl.pack(strip, result);
    if (check) {
        val = this->validChannel(result, stationName, stationEta, stationPhi, chamberLayer, wireLayer, measuresPhi, strip);
        if (isValid) *isValid = val;
    }
    return result;
}

Identifier CscIdHelper::channelID(const std::string& stationNameStr, int stationEta, int stationPhi, int chamberLayer, int wireLayer,
                                  int measuresPhi, int strip, bool check, bool* isValid) const {
    Identifier id;
    int stationName = stationNameIndex(stationNameStr);
    id = channelID(stationName, stationEta, stationPhi, chamberLayer, wireLayer, measuresPhi, strip, check, isValid);
    return id;
}

Identifier CscIdHelper::channelID(const Identifier& id, int chamberLayer, int wireLayer, int measuresPhi, int strip, bool check,
                                  bool* isValid) const {
    Identifier result(id);
    bool val = false;
    m_cla_impl.pack(chamberLayer, result);
    m_lay_impl.pack(wireLayer, result);
    m_mea_impl.pack(measuresPhi, result);
    m_str_impl.pack(strip, result);
    if (check) {
        val = this->valid(result);
        if (isValid) *isValid = val;
    }
    return result;
}

/// get parent id from channel id

Identifier CscIdHelper::parentID(const Identifier& id) const {
    assert(is_csc(id));
    Identifier result(id);
    m_cla_impl.reset(result);
    m_lay_impl.reset(result);
    m_mea_impl.reset(result);
    m_str_impl.reset(result);
    return result;
}

// Access to components of the ID

int CscIdHelper::chamberLayer(const Identifier& id) const { return m_cla_impl.unpack(id); }

int CscIdHelper::wireLayer(const Identifier& id) const { return m_lay_impl.unpack(id); }

bool CscIdHelper::measuresPhi(const Identifier& id) const { return m_mea_impl.unpack(id); }

int CscIdHelper::strip(const Identifier& id) const { return m_str_impl.unpack(id); }

int CscIdHelper::channel(const Identifier& id) const { return strip(id); }

/// Access to min and max of level ranges

int CscIdHelper::stationEtaMin() const { return StationEtaMin; }

int CscIdHelper::stationEtaMax() const { return StationEtaMax; }

int CscIdHelper::stationPhiMin() const { return StationPhiMin; }

int CscIdHelper::stationPhiMax() const { return StationPhiMax; }

int CscIdHelper::chamberLayerMin() const { return ChamberLayerMin; }

int CscIdHelper::chamberLayerMax() const { return ChamberLayerMax; }

int CscIdHelper::wireLayerMin() const { return WireLayerMin; }

int CscIdHelper::wireLayerMax() const { return WireLayerMax; }

int CscIdHelper::measuresPhiMin() const { return MeasuresPhiMin; }

int CscIdHelper::measuresPhiMax() const { return MeasuresPhiMax; }

int CscIdHelper::stripMin() const { return StripMin; }

int CscIdHelper::stripMax() const { return StripMax; }

/// Utility methods

int CscIdHelper::cscTechnology() const {
    int cscField = technologyIndex("CSC");
    if (m_dict) { cscField = csc_field_value(); }
    return cscField;
}

int CscIdHelper::sector(const Identifier& id) const { return stationEta(id) * (2 * stationPhi(id) - (stationName(id) - 49) + 1); }

int CscIdHelper::gasGap(const Identifier& id) const { return chamberLayer(id); }
