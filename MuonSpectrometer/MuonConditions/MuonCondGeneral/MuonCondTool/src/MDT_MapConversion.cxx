/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#include "MDT_MapConversion.h"

#include <cstdio>
#include <fstream>
#include <map>
#include <string>

#include "AthenaPoolUtilities/AthenaAttributeList.h"
#include "AthenaPoolUtilities/CondAttrListCollection.h"
#include "CoralBase/Attribute.h"
#include "CoralBase/AttributeListSpecification.h"
#include "GaudiKernel/MsgStream.h"
#include "GeoModelInterfaces/IGeoModelSvc.h"
#include "Identifier/Identifier.h"
#include "Identifier/IdentifierHash.h"
#include "MuonCondSvc/MdtStringUtils.h"
#include "MuonIdHelpers/MdtIdHelper.h"
#include "PathResolver/PathResolver.h"
#include "RDBAccessSvc/IRDBAccessSvc.h"
#include "RDBAccessSvc/IRDBRecord.h"
#include "RDBAccessSvc/IRDBRecordset.h"
#include "SGTools/TransientAddress.h"

//**********************************************************
//*
//* Tool to retrieve the MDT Map Info from ORACLE DB
//* retrieving mapping conversion tables from DB
//* convert method online name into offline ID
//*********************************************************

MDT_MapConversion::MDT_MapConversion(const std::string& type, const std::string& name, const IInterface* parent) :
    AthAlgTool(type, name, parent), m_mdtIdHelper(nullptr) {
    declareInterface<IMDT_MapConversion>(this);
}

StatusCode MDT_MapConversion::initialize() {
    ATH_CHECK(detStore()->retrieve(m_mdtIdHelper, "MDTIDHELPER"));

    IGeoModelSvc* geoModel{nullptr};
    ATH_CHECK(service("GeoModelSvc", geoModel));

    std::string AtlasVersion = geoModel->atlasVersion();
    std::string MuonVersion = geoModel->muonVersionOverride();
    std::string detectorKey = MuonVersion.empty() ? AtlasVersion : MuonVersion;
    std::string detectorNode = MuonVersion.empty() ? "ATLAS" : "MuonSpectrometer";

    IRDBAccessSvc* accessSvc{nullptr};
    ATH_CHECK(service("RDBAccessSvc", accessSvc));

    IRDBRecordset_ptr switchSet = accessSvc->getRecordsetPtr("HwSwIdMapping", detectorKey, detectorNode);

    if ((*switchSet).size() == 0) {
        ATH_MSG_WARNING("Old Atlas Version : " << AtlasVersion << " Only Online Identifier. Falling back to HwSwIdMapping-00 tag");
        switchSet = accessSvc->getRecordsetPtr("HwSwIdMapping", "HwSwIdMapping-00");
    }

    for (unsigned int irow = 0; irow < (*switchSet).size(); ++irow) {
        const IRDBRecord* switches = (*switchSet)[irow];
        std::string hardwareName = switches->getString("HARDNAME");
        std::string stName = switches->getString("SOFTNAME");
        int stPhi = switches->getInt("SOFTOCTANT");
        int stEta = switches->getInt("SOFTIZ");
        Identifier ChamberId = m_mdtIdHelper->elementID(stName, stEta, stPhi);

        m_Chamber_Map[hardwareName] = ChamberId;
    }

    ATH_MSG_INFO("#### Chamber Map SIZE" << m_Chamber_Map.size());
    return StatusCode::SUCCESS;
}

const Identifier& MDT_MapConversion::ConvertToOffline(std::string_view OnlineId, bool quiet) const {
    if (m_Chamber_Map.empty()) return m_Online_empty;
    std::string why_view{OnlineId.data()};
    /// Check if the 3 character is either X or Y coming from BIX / BIY
    if (why_view[2] == 'Y' || why_view[2] == 'X') why_view[2] = 'S';
    for (const auto& mappair : m_Chamber_Map) {
        if (why_view.find(mappair.first) == std::string::npos) continue;        
        const Identifier& OfflineName = mappair.second;
        return OfflineName;
    }
    if (!quiet) { ATH_MSG_ERROR("Lookup of ID " << why_view << " in MDT_MapConversion::ConvertToOffline failed"); }
    return m_Online_empty;  // not quite right but should never get here
}
