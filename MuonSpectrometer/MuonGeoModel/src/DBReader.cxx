/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#include "MuonGeoModel/DBReader.h"

#include "StoreGate/StoreGateSvc.h"

namespace MuonGM {

    DBReader::DBReader(StoreGateSvc * /*pDetStore*/) : m_mgr(0) {
        m_SCdbaccess = StatusCode::SUCCESS;
        m_msgSvc = Athena::getMessageSvc();
    }

    void DBReader::setGeometryVersion(std::string geoVersion) { m_version = geoVersion; }

    std::string DBReader::getGeometryVersion() const { return m_version; }

    std::string DBReader::TGCreadoutName(int ichtyp) {
        MsgStream log(Athena::getMessageSvc(), "DBReader::TGCreadoutName");
        if (m_tgcReadoutMapping.size() == 0) {
            // first time fill the vector

            m_tgcReadoutMapping.push_back("T1F1"); // 1

            m_tgcReadoutMapping.push_back("T1E1"); // 2
            m_tgcReadoutMapping.push_back("T1E2");
            m_tgcReadoutMapping.push_back("T1E3");
            m_tgcReadoutMapping.push_back("T1E4");

            m_tgcReadoutMapping.push_back("T2F1"); // 6

            m_tgcReadoutMapping.push_back("T2E1"); // 7
            m_tgcReadoutMapping.push_back("T2E2");
            m_tgcReadoutMapping.push_back("T2E3");
            m_tgcReadoutMapping.push_back("T2E4");
            m_tgcReadoutMapping.push_back("T2E5"); // 11

            m_tgcReadoutMapping.push_back("T3F1"); // 12

            m_tgcReadoutMapping.push_back("T3E1"); // 13
            m_tgcReadoutMapping.push_back("T3E2");
            m_tgcReadoutMapping.push_back("T3E3");
            m_tgcReadoutMapping.push_back("T3E4");
            m_tgcReadoutMapping.push_back("T3E5"); // 17

            m_tgcReadoutMapping.push_back("T4F1"); // 18

            m_tgcReadoutMapping.push_back("T4E1"); // 19
        }

        if (ichtyp < 1 || ichtyp > 19) {
            log << MSG::ERROR << " DBReader::TGCreadoutName  - ichtype " << ichtyp << " out of range 1-19" << endmsg;
            return "XXXY";
        }
        return m_tgcReadoutMapping[ichtyp - 1];
    }

} // namespace MuonGM
