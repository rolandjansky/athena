/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#include "MuonGeoModel/MYSQL.h"

#include "MuonGeoModel/Technology.h"
#include "MuonReadoutGeometry/GlobalUtilities.h"
#include "MuonReadoutGeometry/TgcReadoutParams.h"

#include <cassert>
#include <iostream>
#include <sstream>

namespace MuonGM {

    MYSQL *MYSQL::s_thePointer = 0;

    MYSQL::MYSQL() : m_includeCutoutsBog(0), m_includeCtbBis(0), m_controlAlines(0) {
        m_geometry_version = "unknown";
        m_layout_name = "unknown";
        m_amdb_version = 0;
        m_nova_version = 0;
        m_amdb_from_rdb = false;
        for (unsigned int i = 0; i < NTgcReadouts; i++) {
            m_tgcReadout[i] = NULL;
        }
    }

    MYSQL::~MYSQL() {
        // delete stations
        std::map<std::string, Station *>::const_iterator it;
        for (it = m_stations.begin(); it != m_stations.end(); it++) {
            delete (*it).second;
        }

        // delete m_technologies
        std::map<std::string, Technology *>::const_iterator it1;
        for (it1 = m_technologies.begin(); it1 != m_technologies.end(); it1++) {
            delete (*it1).second;
        }

        // reset the pointer so that at next initialize the MYSQL object will be re-created
        s_thePointer = 0;
    }

    MYSQL *MYSQL::GetPointer() {
        if (!s_thePointer) {
            s_thePointer = new MYSQL;
        }
        return s_thePointer;
    }

    Station *MYSQL::GetStation(std::string name) {
        MsgStream log(Athena::getMessageSvc(), "MuonGeoModel.MYSQL");
        if (log.level() <= MSG::VERBOSE) {
            log << MSG::VERBOSE << " looking for station " << name << endmsg;
        }
        std::map<std::string, Station *>::const_iterator it = m_stations.find(name);
        if (it != m_stations.end()) {
            if (log.level() <= MSG::VERBOSE) {
                log << MSG::VERBOSE << "found the station" << endmsg;
            }
            return it->second;
        } else {
            return 0;
        }
    }

    Position MYSQL::GetStationPosition(std::string nameType, int fi, int zi) {
        Position p;
        MsgStream log(Athena::getMessageSvc(), "MuonGeoModel.MYSQL");
        if (log.level() <= MSG::VERBOSE) {
            log << MSG::VERBOSE << " MYSQL::GetStationPosition for " << nameType << " fi/zi " << fi << " " << zi << endmsg;
        }
        int subtype = allocPosFindSubtype(nameType, fi, zi);
        std::string stname = nameType + MuonGM::buildString(subtype, 0);
        Station *st = GetStation(stname);
        if (st != NULL) {
            if (log.level() <= MSG::VERBOSE) {
                log << MSG::VERBOSE << " found in Station " << st->GetName();
            }
            p = (*(st->FindPosition(zi, fi))).second;
            if (log.level() <= MSG::VERBOSE) {
                log << MSG::VERBOSE << " at p.fi,zi " << p.phiindex << " " << p.zindex << " shift/z " << p.shift << " " << p.z << endmsg;
            }
        } else {
            log << MSG::WARNING << "::GetStationPosition nothing found for " << nameType << " at fi/zi " << fi << " " << zi << endmsg;
        }
        return p;
    }

    TgcReadoutParams *MYSQL::GetTgcRPars(std::string name) {
        MsgStream log(Athena::getMessageSvc(), "MuonGeoModel.MYSQL");
        if (log.level() <= MSG::VERBOSE) {
            log << MSG::VERBOSE << "MYSQL::GetTgcRPars looking for a TgcRPars named <" << name << ">" << endmsg;
        }

        std::map<std::string, TgcReadoutParams *>::const_iterator it = m_tgcReadouts.find(name);
        if (it != m_tgcReadouts.end()) {
            return it->second;
        } else
            return NULL;
    }

    TgcReadoutParams *MYSQL::GetTgcRPars(int jsta) {
        if (jsta - 1 < 0 || jsta >= NTgcReadouts) {
            MsgStream log(Athena::getMessageSvc(), "MuonGeoModel.MYSQL");
            log << MSG::ERROR << "MYSQL::GetTgcRPars jsta = " << jsta << " out of range (0," << NTgcReadouts - 1 << ")" << endmsg;
            return NULL;
        }
        return m_tgcReadout[jsta - 1];
    }

    Technology *MYSQL::GetTechnology(std::string name) {
        std::map<std::string, Technology *>::const_iterator it = m_technologies.find(name);
#ifndef NDEBUG
        MsgStream log(Athena::getMessageSvc(), "MuonGeoModel.MYSQL");
#endif
        if (it != m_technologies.end()) {
#ifndef NDEBUG
            if (log.level() <= MSG::VERBOSE) {
                log << MSG::VERBOSE << "found the station technology name " << name << endmsg;
            }
#endif
            return it->second;
        } else {
#ifndef NDEBUG
            if (log.level() <= MSG::VERBOSE) {
                log << MSG::VERBOSE << "MYSQL:: Technology " << name << "+++++++++ not found!" << endmsg;
            }
#endif
            return nullptr;
        }
    }

    void MYSQL::StoreTechnology(Technology *t) {
        MsgStream log(Athena::getMessageSvc(), "MuonGeoModel.MYSQL");
        if (log.level() <= MSG::VERBOSE) {
            log << MSG::VERBOSE << "MYSQL::StoreTechnology /// techn. named " << t->GetName() << endmsg;
        }

        std::map<std::string, Technology *>::const_iterator it = m_technologies.find(t->GetName());
        if (it != m_technologies.end()) {
            log << MSG::ERROR << "MYSQL::StoreTechnology ERROR /// This place is already taken !!! for " << t->GetName() << endmsg;
            assert(0);
        } else {
            m_technologies[t->GetName()] = t;
        }
    }

    void MYSQL::StoreStation(Station *s) {
        MsgStream log(Athena::getMessageSvc(), "MuonGeoModel.MYSQL");
        if (log.level() <= MSG::VERBOSE) {
            log << MSG::VERBOSE << "name " << s->GetName() << endmsg;
        }
        m_stations[s->GetName()] = s;
    }

    void MYSQL::StoreTgcRPars(TgcReadoutParams *s) {
        MsgStream log(Athena::getMessageSvc(), "MuonGeoModel.MYSQL");
        if (log.level() <= MSG::VERBOSE) {
            log << MSG::VERBOSE << "MYSQL::StoreTgcRPars named " << s->GetName() << " located @ " << s << " jsta = " << s->chamberType() << endmsg;
        }
        if (s->chamberType() >= NTgcReadouts) {
            log << MSG::ERROR << "MYSQL::StoreTgcRPars ChamberType(JSTA) " << s->chamberType() << " > NTgcReadouts=" << NTgcReadouts << endmsg;
            return;
        }
        m_tgcReadout[s->chamberType() - 1] = s;
    }

    void MYSQL::PrintAllStations() {
        MsgStream log(Athena::getMessageSvc(), "MuonGM::MYSQL");

        std::map<std::string, Station *>::const_iterator it;
        for (it = m_stations.begin(); it != m_stations.end(); it++) {
            std::string key = (*it).first;
            log << MSG::INFO << "---> Station  " << key << endmsg;
        }
    }

    void MYSQL::PrintTechnologies() {
        MsgStream log(Athena::getMessageSvc(), "MuonGM::MYSQL");

        std::map<std::string, Technology *>::const_iterator it;
        for (it = m_technologies.begin(); it != m_technologies.end(); it++) {
            std::string key = (*it).first;
            log << MSG::INFO << "---> Technology " << key << endmsg;
        }
    }

    Technology *MYSQL::GetATechnology(std::string name) {
        std::map<std::string, Technology *>::const_iterator it = m_technologies.find(name);
        MsgStream log(Athena::getMessageSvc(), "MuonGeoModel.MYSQL");

        if (it != m_technologies.end()) {
            if (log.level() <= MSG::VERBOSE) {
                log << MSG::VERBOSE << "found the station technology name " << name << endmsg;
            }
            return it->second;
        } else {
            if (log.level() <= MSG::VERBOSE) {
                log << MSG::VERBOSE << "MYSQL:: Technology " << name << "+++++++++ not found!" << endmsg;
            }
            for (unsigned int i = 1; i <= 20; i++) {
                char chindex[3];
                sprintf(chindex, "%u", i);
                // std::string newname = name.substr(0,3)+chindex;
                std::string newname = name.substr(0, 3) + MuonGM::buildString(i, 2);
                it = m_technologies.find(newname);
                if (it != m_technologies.end()) {
                    if (log.level() <= MSG::VERBOSE) {
                        log << MSG::VERBOSE << " Selecting a technology called <" << newname << ">" << endmsg;
                    }
                    return it->second;
                }
            }
            return nullptr;
        }
    }

    std::string MYSQL::allocPosBuildKey(std::string statType, int fi, int zi) {
        std::ostringstream mystream;
        mystream << statType << "fi" << MuonGM::buildString(fi, 1) << "zi" << MuonGM::buildString(zi, -1);
        MsgStream log(Athena::getMessageSvc(), "MuonGeoModel.MYSQL");
        if (log.level() <= MSG::VERBOSE) {
            log << MSG::VERBOSE << " from " << statType << " fi " << fi << " zi " << zi << " we get as key " << mystream.str() << endmsg;
        }
        return mystream.str();
    }

    allocPosIterator MYSQL::allocPosFind(std::string statType, int fi, int zi) {
        std::string key = allocPosBuildKey(statType, fi, zi);
        return allocPosFind(key);
    }

    int MYSQL::allocPosFindSubtype(std::string statType, int fi, int zi) {
        std::string key = allocPosBuildKey(statType, fi, zi);
        return allocPosFindSubtype(key);
    }

    int MYSQL::allocPosFindCutout(std::string statType, int fi, int zi) {
        std::string key = allocPosBuildKey(statType, fi, zi);
        return allocPosFindCutout(key);
    }

    void MYSQL::addallocPos(std::string statType, int fi, int zi, int subtyp, int cutout) {
        std::string key = allocPosBuildKey(statType, fi, zi);
        addallocPos(key, subtyp, cutout);
    }

} // namespace MuonGM
