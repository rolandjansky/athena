/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MYSQL_H
#define MYSQL_H

#include "AthenaKernel/getMessageSvc.h"
#include "GaudiKernel/MsgStream.h"
#include "MuonGeoModel/Position.h"
#include "MuonGeoModel/Station.h"
#include "CxxUtils/LockedPointer.h"

#include <map>
#include <string>
#include <mutex>

/*
  This class holds an std::map of stations* (key = stationName, i.e. BMS5),
  an std::map of Technologies* (key = RPC20), and an std::map of TgcReadoutParams*.
  Stations and Technologies are used only to build the geometry (can be deleted
  after that).
  TgcReadoutParams are used by the TgcReadoutElements -> must leave forever
  (they belong to the readout geometry).
  MYSQL is used only to build the geometry - can be deleted as soon as the job is
  done (in the Factory).
  It is responsible for releasing the memory allocated by these objects.

  This is not thread safe!
*/

namespace MuonGM {

    class Technology;
    class TgcReadoutParams;

    typedef std::map<std::string, Station *>::const_iterator StationIterator;
    typedef std::map<std::string, TgcReadoutParams *>::const_iterator TgcReadParsIterator;
    typedef std::map<int, std::string>::const_iterator AllocposIterator;
    typedef std::map<std::string, int>::const_iterator allocPosIterator;

    class MYSQL {
      public:
        enum TgcReadoutRange { NTgcReadouts = 30 };

        using LockedMYSQL = CxxUtils::LockedPointer<MYSQL>;

        ~MYSQL();
        inline bool amdb_from_RDB() const;
        inline void set_amdb_from_RDB(bool);
        inline void set_DBMuonVersion(const std::string&);
        inline std::string get_DBMuonVersion();
        inline void setGeometryVersion(const std::string& s);
        inline std::string getGeometryVersion() const;
        inline void setCtbBisFlag(int i);
        inline int getCtbBisFlag() const;
        inline void setLayoutName(const std::string& s);
        inline std::string getLayoutName() const;
        inline void setCutoutsBogFlag(int i);
        inline int getCutoutsBogFlag() const;
        inline void setNovaVersion(int i);
        inline int getNovaVersion() const;
        inline void setNovaReadVersion(int i);
        inline int getNovaReadVersion() const;
        inline void setControlAlines(int);
        inline int controlAlines() const;

        inline StationIterator StationBegin() const;
        inline StationIterator StationEnd() const;
        inline TgcReadParsIterator TgcRParsBegin() const;
        inline TgcReadParsIterator TgcRParsend() const;
        inline AllocposIterator AllocposBegin();
        inline AllocposIterator AllocposEnd() const;
        inline AllocposIterator AllocposFind(int);
        inline std::string AllocposFindName(int);
        inline void addAllocpos(int i, const std::string& str);
        // the new ones
        std::string allocPosBuildKey(const std::string& statType, int fi, int zi) const;
        inline int allocPosBuildValue(int subtype, int cutout);
        inline allocPosIterator allocPosBegin();
        inline allocPosIterator allocPosEnd() const;
        inline allocPosIterator allocPosFind(const std::string& key);
        allocPosIterator allocPosFind(const std::string& statType, int fi, int zi);
        int allocPosFindSubtype(const std::string& statType, int fi, int zi) const;
        inline int allocPosFindSubtype(const std::string& key) const;
        inline int allocPosFindSubtype(allocPosIterator it) const;
        int allocPosFindCutout(const std::string& statType, int fi, int zi) const;
        inline int allocPosFindCutout(const std::string& key) const;
        inline int allocPosFindCutout(allocPosIterator it) const;
        inline void addallocPos(const std::string& key, int value);
        void addallocPos(const std::string& statType, int fi, int zi, int subtyp, int cutout);
        inline void addallocPos(const std::string& key, int subtype, int cutout);

        inline int NStations() const;
        inline int NTgcReadTypes() const;

        static LockedMYSQL GetPointer();
        const Station *GetStation(const std::string& name) const;
        Station *GetStation(const std::string& name);
        Position GetStationPosition(const std::string& nameType, int fi, int zi) const;
        TgcReadoutParams *GetTgcRPars(const std::string& name) const;
        TgcReadoutParams *GetTgcRPars(int i) const;
        void StoreStation(Station *s);
        void PrintAllStations();
        void StoreTechnology(Technology *t);
        void StoreTgcRPars(TgcReadoutParams *t);
        Technology *GetTechnology(const std::string& name);
        const Technology *GetTechnology(const std::string& name) const;
        const Technology *GetATechnology(const std::string& name) const;
        void PrintTechnologies();

        // singleton
      private:
        // Protects s_thePointer.
        struct MYSQLPtr
        {
          std::recursive_mutex m_mutex;
          MYSQL* m_ptr;
        };
        static MYSQLPtr& GetMYSQLPtr();

        MYSQL();
        std::map<int, std::string> m_allocatedpos;
        std::map<std::string, int> m_allocPos;
        std::map<std::string, Station *> m_stations;
        std::map<std::string, Technology *> m_technologies;
        std::map<std::string, TgcReadoutParams *> m_tgcReadouts;
        TgcReadoutParams *m_tgcReadout[NTgcReadouts]{};
        std::string m_geometry_version; // from our job-option
        std::string m_layout_name;      // from nova
        std::string m_DBMuonVersion;    // name of the MuonVersion table-collection in Oracle
        int m_nova_version;
        int m_amdb_version;
        bool m_amdb_from_rdb;
        int m_includeCutoutsBog;
        int m_includeCtbBis;
        int m_controlAlines;
    };

    void MYSQL::addAllocpos(int i, const std::string& str) { m_allocatedpos[i] = str; }

    AllocposIterator MYSQL::AllocposEnd() const { return m_allocatedpos.end(); }

    AllocposIterator MYSQL::AllocposBegin() { return m_allocatedpos.begin(); }

    AllocposIterator MYSQL::AllocposFind(int i) { return m_allocatedpos.find(i); }

    std::string MYSQL::AllocposFindName(int i) {
        AllocposIterator it = m_allocatedpos.find(i);
        // imt fix in case key is wrong:
        if (it != m_allocatedpos.end()) {
            return (*it).second;
        } else {
            return "ERROR: bad key!";
        }
    }

    StationIterator MYSQL::StationBegin() const { return m_stations.begin(); }

    StationIterator MYSQL::StationEnd() const { return m_stations.end(); }

    TgcReadParsIterator MYSQL::TgcRParsBegin() const { return m_tgcReadouts.begin(); }

    TgcReadParsIterator MYSQL::TgcRParsend() const { return m_tgcReadouts.end(); }

    int MYSQL::NStations() const { return m_stations.size(); }

    int MYSQL::NTgcReadTypes() const { return m_tgcReadouts.size(); }

    int MYSQL::allocPosBuildValue(int subtype, int cutout) { return 100 * subtype + cutout; }

    allocPosIterator MYSQL::allocPosBegin() { return m_allocPos.begin(); }

    allocPosIterator MYSQL::allocPosEnd() const { return m_allocPos.end(); }

    allocPosIterator MYSQL::allocPosFind(const std::string& key) { return m_allocPos.find(key); }

    int MYSQL::allocPosFindSubtype(allocPosIterator it) const {
        int value = it->second;
        int subtype = int(value / 100);
        return subtype;
    }

    int MYSQL::allocPosFindCutout(allocPosIterator it) const {
        int value = (*it).second;
        int cutout = int(value % 100);
        return cutout;
    }

    void MYSQL::addallocPos(const std::string& key, int value) { m_allocPos[key] = value; }

    void MYSQL::addallocPos(const std::string& key, int subtype, int cutout) { m_allocPos[key] = allocPosBuildValue(subtype, cutout); }

    std::string MYSQL::getGeometryVersion() const { return m_geometry_version; }

    void MYSQL::setCtbBisFlag(int i) { m_includeCtbBis = i; }

    int MYSQL::getCtbBisFlag() const { return m_includeCtbBis; }

    int MYSQL::getNovaReadVersion() const { return m_amdb_version; }

    std::string MYSQL::getLayoutName() const { return m_layout_name; }

    void MYSQL::setCutoutsBogFlag(int i) { m_includeCutoutsBog = i; }

    int MYSQL::getCutoutsBogFlag() const { return m_includeCutoutsBog; }

    int MYSQL::getNovaVersion() const { return m_nova_version; }

    bool MYSQL::amdb_from_RDB() const { return m_amdb_from_rdb; }

    void MYSQL::set_amdb_from_RDB(bool val) { m_amdb_from_rdb = val; }

    void MYSQL::set_DBMuonVersion(const std::string& a) { m_DBMuonVersion = a; }

    std::string MYSQL::get_DBMuonVersion() { return m_DBMuonVersion; }

    void MYSQL::setControlAlines(int cA) { m_controlAlines = cA; }

    int MYSQL::controlAlines() const { return m_controlAlines; }

    void MYSQL::setGeometryVersion(const std::string& s) {
        MsgStream log(Athena::getMessageSvc(), "MuonGeoModel.MYSQL");

        if (m_geometry_version != "unknown") {
            if (s == m_geometry_version)
                return;
            log << MSG::WARNING << "GeometryVersion already set to  <" << m_geometry_version << ">"
                << " resetting to <" << s << ">" << endmsg;
        }
        m_geometry_version = s;
        log << MSG::INFO << "GeometryVersion set to <" << m_geometry_version << ">" << endmsg;
    }

    void MYSQL::setNovaReadVersion(int i) {
        m_amdb_version = i;
        MsgStream log(Athena::getMessageSvc(), "MuonGeoModel.MYSQL");
        if (log.level() <= MSG::VERBOSE)
            log << MSG::VERBOSE << "setNovaReadVersion to " << m_amdb_version << endmsg;
    }

    void MYSQL::setLayoutName(const std::string& s) {
        MsgStream log(Athena::getMessageSvc(), "MuonGeoModel.MYSQL");
        if (m_layout_name != "unknown") {
            if (s == m_layout_name)
                return;
            log << MSG::WARNING << "LayoutName already set to  <" << m_layout_name << ">"
                << " resetting to <" << s << ">" << endmsg;
        }
        m_layout_name = s;
        log << MSG::INFO << "LayoutName (from DBAM) set to <" << m_layout_name << ">  -- relevant for CTB2004" << endmsg;
    }

    void MYSQL::setNovaVersion(int i) {
        m_nova_version = i;
        MsgStream log(Athena::getMessageSvc(), "MuonGeoModel.MYSQL");
        if (log.level() <= MSG::VERBOSE)
            log << MSG::VERBOSE << "setNovaVersion to " << m_nova_version << endmsg;
    }

    int MYSQL::allocPosFindCutout(const std::string& key) const {
        int cutout = 0;
        allocPosIterator it = m_allocPos.find(key);
        if (it != allocPosEnd()) {
            return allocPosFindCutout(it);
        }

        MsgStream log(Athena::getMessageSvc(), "MuonGeoModel.MYSQL");
        log << MSG::ERROR << "MYSQL::allocPosFindCutout for key  " << key << " no element found" << endmsg;
        return cutout;
    }

    int MYSQL::allocPosFindSubtype(const std::string& key) const {
        int subtype = 0;
        allocPosIterator it = m_allocPos.find(key);
        if (it != allocPosEnd()) {
            return allocPosFindSubtype(it);
        }
        MsgStream log(Athena::getMessageSvc(), "MuonGeoModel.MYSQL");
        log << MSG::ERROR << "MYSQL::allocPosFindSubtype for key  " << key << " no element found" << endmsg;
        return subtype;
    }

} // namespace MuonGM

#endif
