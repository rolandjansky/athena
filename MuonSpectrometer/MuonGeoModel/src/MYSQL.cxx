/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#include "MuonGeoModel/MYSQL.h"

#include "MuonGeoModel/Technology.h"
#include "MuonReadoutGeometry/GlobalUtilities.h"
#include "MuonReadoutGeometry/TgcReadoutParams.h"
#include "AthenaKernel/SlotSpecificObj.h"
#include "CxxUtils/checker_macros.h"

#include <cassert>
#include <iostream>
#include <sstream>
#include <utility>

namespace MuonGM {

    MYSQL::MYSQL() :
      m_geometry_version ("unknown"),
      m_layout_name ("unknown"),
      m_nova_version (0),
      m_amdb_version (0),
      m_amdb_from_rdb (false),
      m_includeCutoutsBog(0), m_includeCtbBis(0), m_controlAlines(0)
    {
        for (unsigned int i = 0; i < NTgcReadouts; i++) {
            m_tgcReadout[i] = nullptr;
        }
    }

    MYSQL::~MYSQL() {
        MYSQLPtr& ptr = GetMYSQLPtr();
        std::unique_lock l (ptr.m_mutex);

        // delete stations
        for (auto& p : m_stations) {
            delete p.second;
        }

        // delete m_technologies
        for (auto& p : m_technologies) {
            delete p.second;
        }

        // reset the pointer so that at next initialize the MYSQL object will be re-created
        if (ptr.m_ptr == this)
          ptr.m_ptr = nullptr;
    }

    MYSQL::MYSQLPtr& MYSQL::GetMYSQLPtr() {
        static SG::SlotSpecificObj<MYSQLPtr> ptrs ATLAS_THREAD_SAFE;
        const EventContext& ctx = Gaudi::Hive::currentContext();
        if (ctx.slot() == EventContext::INVALID_CONTEXT_ID) {
          EventContext ctx2 (0, 0);
          return *ptrs.get(ctx2);
        }
        return *ptrs.get(ctx);
    }

    MYSQL::LockedMYSQL MYSQL::GetPointer() {
        MYSQLPtr& ptr = GetMYSQLPtr();
        std::unique_lock l (ptr.m_mutex);
        if (!ptr.m_ptr) {
            ptr.m_ptr = new MYSQL;
        }
        return LockedMYSQL (*ptr.m_ptr, std::move(l));
    }

    const Station *MYSQL::GetStation(const std::string& name) const {
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
            return nullptr;
        }
    }

    Station *MYSQL::GetStation(const std::string& name) {
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
            return nullptr;
        }
    }

    Position MYSQL::GetStationPosition(const std::string& nameType, int fi, int zi) const {
        Position p;
        MsgStream log(Athena::getMessageSvc(), "MuonGeoModel.MYSQL");
        if (log.level() <= MSG::VERBOSE) {
            log << MSG::VERBOSE << " MYSQL::GetStationPosition for " << nameType << " fi/zi " << fi << " " << zi << endmsg;
        }
        int subtype = allocPosFindSubtype(nameType, fi, zi);
        std::string stname = nameType + MuonGM::buildString(subtype, 0);
        const Station *st = GetStation(stname);
        if (st != nullptr) {
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

    TgcReadoutParams *MYSQL::GetTgcRPars(const std::string& name) const {
        MsgStream log(Athena::getMessageSvc(), "MuonGeoModel.MYSQL");
        if (log.level() <= MSG::VERBOSE) {
            log << MSG::VERBOSE << "MYSQL::GetTgcRPars looking for a TgcRPars named <" << name << ">" << endmsg;
        }

        std::map<std::string, TgcReadoutParams *>::const_iterator it = m_tgcReadouts.find(name);
        if (it != m_tgcReadouts.end()) {
            return it->second;
        } else
            return nullptr;
    }

    TgcReadoutParams *MYSQL::GetTgcRPars(int jsta) const {
        if (jsta - 1 < 0 || jsta >= NTgcReadouts) {
            MsgStream log(Athena::getMessageSvc(), "MuonGeoModel.MYSQL");
            log << MSG::ERROR << "MYSQL::GetTgcRPars jsta = " << jsta << " out of range (0," << NTgcReadouts - 1 << ")" << endmsg;
            return nullptr;
        }
        return m_tgcReadout[jsta - 1];
    }

    Technology *MYSQL::GetTechnology(const std::string& name) {
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

    const Technology *MYSQL::GetTechnology(const std::string& name) const {
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

        for (const auto& p : m_stations) {
            log << MSG::INFO << "---> Station  " << p.first << endmsg;
        }
    }

    void MYSQL::PrintTechnologies() {
        MsgStream log(Athena::getMessageSvc(), "MuonGM::MYSQL");

        for (const auto& p : m_technologies) {
            log << MSG::INFO << "---> Technology " << p.first << endmsg;
        }
    }

    const Technology *MYSQL::GetATechnology(const std::string& name) const {
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

    std::string MYSQL::allocPosBuildKey(const std::string& statType, int fi, int zi) const {
        std::ostringstream mystream;
        mystream << statType << "fi" << MuonGM::buildString(fi, 1) << "zi" << MuonGM::buildString(zi, -1);
        MsgStream log(Athena::getMessageSvc(), "MuonGeoModel.MYSQL");
        if (log.level() <= MSG::VERBOSE) {
            log << MSG::VERBOSE << " from " << statType << " fi " << fi << " zi " << zi << " we get as key " << mystream.str() << endmsg;
        }
        return mystream.str();
    }

    allocPosIterator MYSQL::allocPosFind(const std::string& statType, int fi, int zi) {
        std::string key = allocPosBuildKey(statType, fi, zi);
        return allocPosFind(key);
    }

    int MYSQL::allocPosFindSubtype(const std::string& statType, int fi, int zi) const {
        std::string key = allocPosBuildKey(statType, fi, zi);
        return allocPosFindSubtype(key);
    }

    int MYSQL::allocPosFindCutout(const std::string& statType, int fi, int zi) const {
        std::string key = allocPosBuildKey(statType, fi, zi);
        return allocPosFindCutout(key);
    }

    void MYSQL::addallocPos(const std::string& statType, int fi, int zi, int subtyp, int cutout) {
        std::string key = allocPosBuildKey(statType, fi, zi);
        addallocPos(key, subtyp, cutout);
    }

} // namespace MuonGM
