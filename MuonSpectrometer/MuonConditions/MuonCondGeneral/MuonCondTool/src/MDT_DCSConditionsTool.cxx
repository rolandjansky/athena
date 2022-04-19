/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

#include "MDT_DCSConditionsTool.h"

#include <algorithm>
#include <cstdio>
#include <fstream>
#include <map>
#include <string>

#include "AthenaPoolUtilities/AthenaAttributeList.h"
#include "AthenaPoolUtilities/CondAttrListCollection.h"
#include "CoralBase/Attribute.h"
#include "CoralBase/AttributeListSpecification.h"
#include "GaudiKernel/MsgStream.h"
#include "Identifier/Identifier.h"
#include "Identifier/IdentifierHash.h"
#include "MDT_MapConversion.h"
#include "MuonCondSvc/MdtStringUtils.h"
#include "MuonIdHelpers/MdtIdHelper.h"
#include "PathResolver/PathResolver.h"
#include "SGTools/TransientAddress.h"

//*
//* Tool to retrieve the MDT DCS Info from COOL DB
//* retrieving of tables from DB
//*********************************************************

MDT_DCSConditionsTool::MDT_DCSConditionsTool(const std::string& type, const std::string& name, const IInterface* parent) :
    AthAlgTool(type, name, parent), m_IOVSvc(nullptr), m_mdtIdHelper(nullptr), m_chronoSvc(nullptr), m_condMapTool("MDT_MapConversion") {
    declareInterface<IMDT_DCSConditionsTool>(this);

    declareProperty("DropChamberFolder", m_dropchamberFolder = "/MDT/DCS/DROPPEDCH");
    declareProperty("LVFolder", m_lvFolder = "/MDT/DCS/PSLVCHSTATE");
    declareProperty("HVFolder", m_hvFolder = "/MDT/DCS/PSHVMLSTATE");
    declareProperty("JTAGFolder", m_jtagFolder = "/MDT/DCS/JTAGCHSTATE");
    declareProperty("SetPointsV0Folder", m_setPointsV0Folder = "/MDT/DCS/PSV0SETPOINTS");
    declareProperty("SetPointsV1Folder", m_setPointsV1Folder = "/MDT/DCS/PSV1SETPOINTS");
    declareProperty("Check_on_setPoint", m_check_on_setPoint = false);
    declareProperty("Simulation_Setup", m_simulation_Setup = false);
    declareProperty("MDT_MapConversion", m_condMapTool);

    m_MDTChamDrop.str("EMPTY");
    m_MDTLV.str("EMPTY");
    m_MDTHV.str("EMPTY");
    m_MDTJTAG.str("EMPTY");
}

StatusCode MDT_DCSConditionsTool::updateAddress(StoreID::type /*storeID*/, SG::TransientAddress* /*tad*/, const EventContext& /*ctx*/) {
    return StatusCode::FAILURE;
}

StatusCode MDT_DCSConditionsTool::initialize() {
    ATH_MSG_INFO("Initializing - folders names are: ChamberDropped " << m_dropchamberFolder << " / LV " << m_lvFolder << " / HV "
                                                                     << m_hvFolder);

    ATH_CHECK(detStore()->retrieve(m_mdtIdHelper, "MDTIDHELPER"));

    // Get interface to IOVSvc
    m_IOVSvc = nullptr;
    bool CREATEIF(true);
    ATH_CHECK(service("IOVSvc", m_IOVSvc, CREATEIF));

    // initialize the chrono service
    ATH_CHECK(service("ChronoStatSvc", m_chronoSvc));
    // tool for the conversion online -> offline ID

    ATH_CHECK(m_condMapTool.retrieve());
    return StatusCode::SUCCESS;
}

StatusCode MDT_DCSConditionsTool::loadParameters(IOVSVC_CALLBACK_ARGS_P(I, keys)) {
    bool isHVfolder = false, isV0folder = false, isV1folder = false;
    std::list<std::string>::const_iterator itr;
    for (itr = keys.begin(); itr != keys.end(); ++itr) {
        ATH_MSG_INFO("LoadParameters " << *itr << " I=" << I << " ");
        if (*itr == m_dropchamberFolder) {
            ATH_CHECK(loadDropChamber(I, keys));

        } else if (*itr == m_lvFolder) {
            ATH_CHECK(loadLV(I, keys));

        } else if (*itr == m_jtagFolder && !m_simulation_Setup) {
            ATH_CHECK(loadJTAG(I, keys));

        }
        // array of folders for the hv : hv, setpointsV0, setpointsV1

        else if (*itr == m_hvFolder && !m_simulation_Setup)
            isHVfolder = true;
        else if (*itr == m_setPointsV0Folder && !m_simulation_Setup)
            isV0folder = true;
        else if (*itr == m_setPointsV1Folder && !m_simulation_Setup)
            isV1folder = true;
    }

    if (isHVfolder && isV0folder && isV1folder && !m_simulation_Setup) { ATH_CHECK(loadHV(I, keys)); }

    return StatusCode::SUCCESS;
}

StatusCode MDT_DCSConditionsTool::loadDropChamber(IOVSVC_CALLBACK_ARGS_P(I, keys)) {
    ATH_MSG_INFO("Load Dropped Chamber from DCS DB");

    // Print out callback information
    if (msgLvl(MSG::DEBUG)) {
        std::stringstream sstr;
        for (const std::string& ston_key : keys) sstr << ston_key << " ";
        ATH_MSG_DEBUG("Level " << I << " Keys: " << sstr.str());
    }

    const CondAttrListCollection* atrc;
    ATH_MSG_INFO("Try to read from folder <" << m_dropchamberFolder << ">");

    ATH_CHECK(detStore()->retrieve(atrc, m_dropchamberFolder));

    CondAttrListCollection::const_iterator itr;

    for (itr = atrc->begin(); itr != atrc->end(); ++itr) {
        const coral::AttributeList& atr = itr->second;

        std::string chamber_dropped;

        chamber_dropped = *(static_cast<const std::string*>((atr["Chambers_disabled"]).addressOfData()));

        constexpr char delimiter = ' ';
        auto tokens = MuonCalib::MdtStringUtils::tokenize(chamber_dropped, delimiter);
        for (unsigned int i = 0; i < tokens.size(); i++) {
            if (tokens[i] != "0") {
                m_cachedDeadStations.push_back(std::string(tokens[i]));
                std::string_view chamber_name = tokens[i];

                Identifier ChamberId = m_condMapTool->ConvertToOffline(chamber_name);
                m_cachedDeadStationsId.push_back(ChamberId);
            }
        }
    }

    return StatusCode::SUCCESS;
}

StatusCode MDT_DCSConditionsTool::loadHV(IOVSVC_CALLBACK_ARGS_P(I, keys)) {
    using namespace MuonCalib;

    ATH_MSG_INFO("Load HV from DCS DB");
    const CondAttrListCollection* atrc;
    ATH_MSG_INFO("Try to read from folder <" << m_hvFolder << ">");

    // Print out callback information
    if (msgLvl(MSG::DEBUG)) {
        std::stringstream sstr;
        for (const std::string& ston_key : keys) sstr << ston_key << " ";
        ATH_MSG_DEBUG("Level " << I << " Keys: " << sstr.str());
    }
    ATH_CHECK(detStore()->retrieve(atrc, m_hvFolder));

    // setpointsV0

    const CondAttrListCollection* atrc_v0;
    ATH_MSG_INFO("Try to read from folder <" << m_setPointsV0Folder << ">");

    ATH_CHECK(detStore()->retrieve(atrc_v0, m_setPointsV0Folder));

    // setpointsV1

    const CondAttrListCollection* atrc_v1;
    ATH_MSG_INFO("Try to read from folder <" << m_setPointsV1Folder << ">");

    ATH_CHECK(detStore()->retrieve(atrc_v1, m_setPointsV1Folder));

    //////////////////////////////

    CondAttrListCollection::const_iterator itr;

    unsigned int chan_index = 0;

    for (itr = atrc->begin(); itr != atrc->end(); ++itr) {
        unsigned int chanNum = atrc->chanNum(chan_index);
        std::string hv_name;
        const std::string& hv_payload = atrc->chanName(chanNum);

        itr = atrc->chanAttrListPair(chanNum);
        const coral::AttributeList& atr = itr->second;

        if (atr.size() == 1) {
            hv_name = *(static_cast<const std::string*>((atr["fsm_currentState"]).addressOfData()));
            constexpr char delimiter = ' ';
            auto tokens = MuonCalib::MdtStringUtils::tokenize(hv_name, delimiter);

            constexpr char delimiter2 = '_';
            auto tokens2 = MuonCalib::MdtStringUtils::tokenize(hv_payload, delimiter2);

            if (tokens[0] != "ON" && tokens[0] != "STANDBY" && tokens[0] != "UNKNOWN") {
                ATH_MSG_VERBOSE("NOT ON and NOT STANDBY HV : " << tokens[0] << " ChamberName : " << tokens2[2] << "multilayer"
                                                               << tokens2[3]);
                m_cachedDeadMultiLayers.push_back(std::string(tokens2[2]));
                int multilayer = MdtStringUtils::atoi(tokens2[3]);
                std::string_view chamber_name = tokens2[2];
                Identifier ChamberId = m_condMapTool->ConvertToOffline(chamber_name);
                Identifier MultiLayerId = m_mdtIdHelper->channelID(ChamberId, multilayer, 1, 1);
                m_cachedDeadMultiLayersId.push_back(MultiLayerId);
            }
            if (tokens[0] == "STANDBY") {
                ATH_MSG_VERBOSE("STANDBY HV : " << tokens[0] << " ChamberName : " << tokens2[2] << "multilayer" << tokens2[3]);

                int multilayer = MdtStringUtils::atoi(tokens2[3]);
                std::string_view chamber_name = tokens2[2];
                Identifier ChamberId = m_condMapTool->ConvertToOffline(chamber_name);
                Identifier MultiLayerId = m_mdtIdHelper->channelID(ChamberId, multilayer, 1, 1);
                m_cachedDeadMultiLayersId_standby.push_back(MultiLayerId);

                // m_Chamber_Naming_standby.insert(std::make_pair(int(chanNum),MultiLayerId));
                m_Chamber_Naming_standby[int(chanNum)] = MultiLayerId;
            }
        }
        chan_index++;
    }

    if (m_check_on_setPoint) {
        // check on Setpoints V0
        CondAttrListCollection::const_iterator itr_v0;

        unsigned int chan_index_v0 = 0;

        for (itr_v0 = atrc_v0->begin(); itr_v0 != atrc_v0->end(); ++itr_v0) {
            unsigned int chanNum = atrc_v0->chanNum(chan_index_v0);

            float setPointsV0_name;
            const std::string& setPointsV0_payload = atrc_v0->chanName(chanNum);

            itr_v0 = atrc_v0->chanAttrListPair(chanNum);
            const coral::AttributeList& atr_v0 = itr_v0->second;

            if (atr_v0.size() == 1) {
                setPointsV0_name = *(static_cast<const float*>((atr_v0["readBackSettings_v0"]).addressOfData()));
                constexpr char delimiter2 = '_';
                const auto tokens2 = MuonCalib::MdtStringUtils::tokenize(setPointsV0_payload, delimiter2);
                m_ChamberML_V0_chanum[int(chanNum)] = float(setPointsV0_name);

                int multilayer = MdtStringUtils::atoi(tokens2[3]);
                const std::string_view& chamber_name = tokens2[2];

                Identifier ChamberId = m_condMapTool->ConvertToOffline(chamber_name);
                Identifier MultiLayerId = m_mdtIdHelper->channelID(ChamberId, multilayer, 1, 1);
                m_ChamberML_V0[MultiLayerId] = setPointsV0_name;
            }
            chan_index_v0++;
        }

        // check on Setpoints V1

        CondAttrListCollection::const_iterator itr_v1;

        unsigned int chan_index_v1 = 0;

        for (itr_v1 = atrc_v1->begin(); itr_v1 != atrc_v1->end(); ++itr_v1) {
            unsigned int chanNum = atrc_v1->chanNum(chan_index_v1);
            float setPointsV1_name;
            const std::string& setPointsV1_payload = atrc_v1->chanName(chanNum);

            itr_v1 = atrc_v1->chanAttrListPair(chanNum);
            const coral::AttributeList& atr_v1 = itr_v1->second;

            if (atr_v1.size() == 1) {
                setPointsV1_name = *(static_cast<const float*>((atr_v1["readBackSettings_v1"]).addressOfData()));

                constexpr char delimiter2 = '_';
                auto tokens2 = MuonCalib::MdtStringUtils::tokenize(setPointsV1_payload, delimiter2);
                m_ChamberML_V1_chanum[int(chanNum)] = float(setPointsV1_name);

                int multilayer = MuonCalib::MdtStringUtils::atoi(tokens2[3]);
                std::string_view chamber_name = tokens2[2];
                Identifier ChamberId = m_condMapTool->ConvertToOffline(chamber_name);
                Identifier MultiLayerId = m_mdtIdHelper->channelID(ChamberId, multilayer, 1, 1);
                m_ChamberML_V1[MultiLayerId] = setPointsV1_name;
            }
            chan_index_v1++;
        }

        // check  for chamber standby the correct value of Setpoint V0 vs SetpointV1
        // for chamber StandBy --> V0==V1 to be on

        for (unsigned int vect = 0; vect < m_cachedDeadMultiLayersId_standby.size(); vect++) {
            Identifier MultilayerId_ch = m_cachedDeadMultiLayersId_standby[vect];

            if (m_ChamberML_V1.find(MultilayerId_ch)->second == m_ChamberML_V0.find(MultilayerId_ch)->second) {
                ATH_MSG_DEBUG(" Chamber has  correct Voltage V1 = " << m_ChamberML_V1.find(MultilayerId_ch)->second
                                                                    << " V0=   " << m_ChamberML_V0.find(MultilayerId_ch)->second);
            } else {
                ATH_MSG_DEBUG(" Chamber has  wrong correct Voltage V1 = " << m_ChamberML_V1.find(MultilayerId_ch)->second
                                                                          << " V0=   " << m_ChamberML_V0.find(MultilayerId_ch)->second);
                ATH_MSG_DEBUG(" Has to be masked!!! ");
                m_cachedDeadMultiLayersId.push_back(MultilayerId_ch);
            }
        }
    }
    return StatusCode::SUCCESS;
}
StatusCode MDT_DCSConditionsTool::loadLV(IOVSVC_CALLBACK_ARGS_P(I, keys)) {
    ATH_MSG_INFO("Load LV from DCS DB");
    const CondAttrListCollection* atrc;
    ATH_MSG_INFO("Try to read from folder <" << m_lvFolder << ">");

    // Print out callback information
    if (msgLvl(MSG::DEBUG)) {
        std::stringstream sstr;
        for (const std::string& st_key : keys) sstr << st_key << " ";
        ATH_MSG_DEBUG("Level " << I << " Keys: " << sstr.str());
    }

    ATH_CHECK(detStore()->retrieve(atrc, m_lvFolder));

    CondAttrListCollection::const_iterator itr;

    unsigned int chan_index = 0;

    for (itr = atrc->begin(); itr != atrc->end(); ++itr) {
        unsigned int chanNum = atrc->chanNum(chan_index);
        std::string hv_name;
        const std::string& hv_payload = atrc->chanName(chanNum);

        itr = atrc->chanAttrListPair(chanNum);
        const coral::AttributeList& atr = itr->second;

        if (atr.size() == 1) {
            hv_name = *(static_cast<const std::string*>((atr["fsm_currentState"]).addressOfData()));
            constexpr char delimiter = ' ';
            const auto tokens = MuonCalib::MdtStringUtils::tokenize(hv_name, delimiter);

            constexpr char delimiter2 = '_';
            auto tokens2 = MuonCalib::MdtStringUtils::tokenize(hv_payload, delimiter2);

            if (tokens[0] != "ON") {
                ATH_MSG_VERBOSE("NOT ON LV: " << tokens[0] << " ChamberName : " << tokens2[2]);
                m_cachedDeadLVStations.push_back(std::string(tokens2[2]));
                std::string_view chamber_name = tokens2[2];
                Identifier ChamberId = m_condMapTool->ConvertToOffline(chamber_name);
                m_cachedDeadLVStationsId.push_back(ChamberId);
                ATH_MSG_VERBOSE("Chamber off from LV Chamber !=ON " << tokens2[2]);
            }
        }
        chan_index++;
    }

    // merge deadStations with deadLVStations, then sort the vector elements and
    // finally remove duplicates
    ATH_MSG_VERBOSE("Now merging the  DeadStations with DeadLVStations");
    m_cachedDeadStations.insert(m_cachedDeadStations.end(), m_cachedDeadLVStations.begin(), m_cachedDeadLVStations.end());

    std::sort(m_cachedDeadStations.begin(), m_cachedDeadStations.end());
    std::vector<std::string>::const_iterator it;
    it = std::unique(m_cachedDeadStations.begin(), m_cachedDeadStations.end());
    m_cachedDeadStations.resize(it - m_cachedDeadStations.begin());

    // merge deadStationsId with deadLVStationsId, then sort the vector elements and
    // finally remove duplicates

    ATH_MSG_VERBOSE("Now merging the  DeadStationsId with DeadLVStationsId");
    m_cachedDeadStationsId.insert(m_cachedDeadStationsId.end(), m_cachedDeadLVStationsId.begin(), m_cachedDeadLVStationsId.end());
    std::sort(m_cachedDeadStationsId.begin(), m_cachedDeadStationsId.end(), compareId);
    std::vector<Identifier>::const_iterator itId;
    itId = std::unique(m_cachedDeadStationsId.begin(), m_cachedDeadStationsId.end());
    m_cachedDeadStationsId.resize(itId - m_cachedDeadStationsId.begin());

    return StatusCode::SUCCESS;
}

StatusCode MDT_DCSConditionsTool::loadJTAG(IOVSVC_CALLBACK_ARGS_P(I, keys)) {
    ATH_MSG_INFO("Load JTAG from DCS DB");
    const CondAttrListCollection* atrc{nullptr};

    ATH_MSG_INFO("Try to read from folder <" << m_jtagFolder << ">");

    // Print out callback information
    if (msgLvl(MSG::DEBUG)) {
        std::stringstream sstr;
        for (const std::string& sto_key : keys) sstr << sto_key << " ";
        ATH_MSG_DEBUG("Level " << I << " Keys: " << sstr.str());
    }

    ATH_CHECK(detStore()->retrieve(atrc, m_jtagFolder));

    CondAttrListCollection::const_iterator itr;

    unsigned int chan_index = 0;
    for (itr = atrc->begin(); itr != atrc->end(); ++itr) {
        unsigned int chanNum = atrc->chanNum(chan_index);
        std::string hv_name;
        const std::string& hv_payload = atrc->chanName(chanNum);

        itr = atrc->chanAttrListPair(chanNum);
        const coral::AttributeList& atr = itr->second;

        if (atr.size() == 1) {
            hv_name = *(static_cast<const std::string*>((atr["fsm_currentState"]).addressOfData()));
            constexpr char delimiter = ' ';
            auto tokens = MuonCalib::MdtStringUtils::tokenize(hv_name, delimiter);
            for (unsigned int i = 0; i < tokens.size(); i++) {
                if (tokens[i] != "0") {}
            }

            constexpr char delimiter2 = '_';
            auto tokens2 = MuonCalib::MdtStringUtils::tokenize(hv_payload, delimiter2);

            if (tokens[0] != "INITIALIZED") {
                m_cachedDeadJTAGStatus.push_back(std::string(tokens2[2]));
                std::string_view chamber_name = tokens2[2];
                Identifier ChamberId = m_condMapTool->ConvertToOffline(chamber_name);
                m_cachedDeadJTAGStatusId.push_back(ChamberId);
            }
        }

        chan_index++;
    }

    return StatusCode::SUCCESS;
}
