/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MYSQL_H
#define MYSQL_H

#include "AthenaKernel/getMessageSvc.h"
#include "GaudiKernel/MsgStream.h"
#include "MuonGeoModel/Position.h"
#include "MuonGeoModel/Station.h"

#include <map>
#include <string>

/*
  This class holds an std::map of stations* (key = stationName, i.e. BMS5),
  an std::map of Technolgies* (key = RPC20), and an std::map of TgcReadoutParams*.
  Stations and Technolgies are used only to build the geometry (can be deleted
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

        ~MYSQL();
        inline bool amdb_from_RDB() const;
        inline void set_amdb_from_RDB(bool);
        inline void set_DBMuonVersion(std::string);
        inline std::string get_DBMuonVersion();
        inline void setGeometryVersion(std::string s);
        inline std::string getGeometryVersion() const;
        inline void setCtbBisFlag(int i);
        inline int getCtbBisFlag() const;
        inline void setLayoutName(std::string s);
        inline std::string getLayoutName() const;
        inline void setCutoutsBogFlag(int i);
        inline int getCutoutsBogFlag() const;
        inline void setNovaVersion(int i);
        inline int getNovaVersion() const;
        inline void setNovaReadVersion(int i);
        inline int getNovaReadVersion() const;
        inline void setControlAlines(int);
        inline int controlAlines() const;

        inline StationIterator StationBegin();
        inline StationIterator StationEnd();
        inline TgcReadParsIterator TgcRParsBegin();
        inline TgcReadParsIterator TgcRParsend();
        inline AllocposIterator AllocposBegin();
        inline AllocposIterator AllocposEnd();
        inline AllocposIterator AllocposFind(int);
        inline std::string AllocposFindName(int);
        inline void addAllocpos(int i, std::string str);
        // the new ones
        std::string allocPosBuildKey(std::string statType, int fi, int zi);
        inline int allocPosBuildValue(int subtype, int cutout);
        inline allocPosIterator allocPosBegin();
        inline allocPosIterator allocPosEnd();
        inline allocPosIterator allocPosFind(std::string key);
        allocPosIterator allocPosFind(std::string statType, int fi, int zi);
        int allocPosFindSubtype(std::string statType, int fi, int zi);
        inline int allocPosFindSubtype(std::string key);
        inline int allocPosFindSubtype(allocPosIterator it);
        int allocPosFindCutout(std::string statType, int fi, int zi);
        inline int allocPosFindCutout(std::string key);
        inline int allocPosFindCutout(allocPosIterator it);
        inline void addallocPos(std::string key, int value);
        void addallocPos(std::string statType, int fi, int zi, int subtyp, int cutout);
        inline void addallocPos(std::string key, int subtype, int cutout);

        inline int NStations();
        inline int NTgcReadTypes();

        static MYSQL *GetPointer();
        Station *GetStation(std::string name);
        Position GetStationPosition(std::string nameType, int fi, int zi);
        TgcReadoutParams *GetTgcRPars(std::string name);
        TgcReadoutParams *GetTgcRPars(int i);
        void StoreStation(Station *s);
        void PrintAllStations();
        void StoreTechnology(Technology *t);
        void StoreTgcRPars(TgcReadoutParams *t);
        Technology *GetTechnology(std::string name);
        Technology *GetATechnology(std::string name);
        void PrintTechnologies();

        // singleton
      private:
        // This is not thread safe. It is neccessary to suppress warning.
        static MYSQL *s_thePointer ATLAS_THREAD_SAFE;

        MYSQL();
        std::map<int, std::string> m_allocatedpos;
        std::map<std::string, int> m_allocPos;
        std::map<std::string, Station *> m_stations;
        std::map<std::string, Technology *> m_technologies;
        std::map<std::string, TgcReadoutParams *> m_tgcReadouts;
        TgcReadoutParams *m_tgcReadout[NTgcReadouts];
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

    void MYSQL::addAllocpos(int i, std::string str) { m_allocatedpos[i] = str; }

    AllocposIterator MYSQL::AllocposEnd() { return m_allocatedpos.end(); }

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

    StationIterator MYSQL::StationBegin() { return m_stations.begin(); }

    StationIterator MYSQL::StationEnd() { return m_stations.end(); }

    TgcReadParsIterator MYSQL::TgcRParsBegin() { return m_tgcReadouts.begin(); }

    TgcReadParsIterator MYSQL::TgcRParsend() { return m_tgcReadouts.end(); }

    int MYSQL::NStations() { return m_stations.size(); }

    int MYSQL::NTgcReadTypes() { return m_tgcReadouts.size(); }

    int MYSQL::allocPosBuildValue(int subtype, int cutout) { return 100 * subtype + cutout; }

    allocPosIterator MYSQL::allocPosBegin() { return m_allocPos.begin(); }

    allocPosIterator MYSQL::allocPosEnd() { return m_allocPos.end(); }

    allocPosIterator MYSQL::allocPosFind(std::string key) { return m_allocPos.find(key); }

    int MYSQL::allocPosFindSubtype(allocPosIterator it) {
        int value = it->second;
        int subtype = int(value / 100);
        return subtype;
    }

    int MYSQL::allocPosFindCutout(allocPosIterator it) {
        int value = (*it).second;
        int cutout = int(value % 100);
        return cutout;
    }

    void MYSQL::addallocPos(std::string key, int value) { m_allocPos[key] = value; }

    void MYSQL::addallocPos(std::string key, int subtype, int cutout) { m_allocPos[key] = allocPosBuildValue(subtype, cutout); }

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

    void MYSQL::set_DBMuonVersion(std::string a) { m_DBMuonVersion = a; }

    std::string MYSQL::get_DBMuonVersion() { return m_DBMuonVersion; }

    void MYSQL::setControlAlines(int cA) { m_controlAlines = cA; }

    int MYSQL::controlAlines() const { return m_controlAlines; }

    void MYSQL::setGeometryVersion(std::string s) {
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

    void MYSQL::setLayoutName(std::string s) {
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

    int MYSQL::allocPosFindCutout(std::string key) {
        int cutout = 0;
        allocPosIterator it = m_allocPos.find(key);
        if (it != allocPosEnd()) {
            return allocPosFindCutout(it);
        }

        MsgStream log(Athena::getMessageSvc(), "MuonGeoModel.MYSQL");
        log << MSG::ERROR << "MYSQL::allocPosFindCutout for key  " << key << " no element found" << endmsg;
        return cutout;
    }

    int MYSQL::allocPosFindSubtype(std::string key) {
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
