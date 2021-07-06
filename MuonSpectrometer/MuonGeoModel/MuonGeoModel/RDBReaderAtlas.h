/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#ifndef RDBReaderAtlas_H
#define RDBReaderAtlas_H

#include "MuonGMdbObjects/DblQ00IAcsc.h"
#include "MuonGMdbObjects/DblQ00Xtomo.h"
#include "MuonGeoModel/DBReader.h"

#include <fstream>
#include <vector>

class IMessageSvc;
class IRDBAccessSvc;

namespace MuonGM {

    class RDBReaderAtlas : public DBReader {
      public:
        RDBReaderAtlas(StoreGateSvc *pDetStore, IRDBAccessSvc *m_pRDBAccess, std::string geoTag, std::string geoNode, bool dumpAlines, bool dumpICSCAlines, bool useICSCAlines,
                       const std::map<std::string, std::string> *asciiFileDBMap = 0);
        ~RDBReaderAtlas();
        StatusCode ProcessDB();

        void setControlCscIntAlines(int x) { m_controlCscIntAlines = x; }

        void ProcessTGCreadout();
        void ProcessCscInternalAlignments();
        std::string TGCreadoutName(int ichtyp);
        void ProcessMdtAsBuiltParams();

      private:
        void ProcessTechnologies();

        int m_controlCscIntAlines;

        DblQ00Dbam *m_dhdbam;
        const DblQ00Dbam::DBAM *m_dbam;
        DblQ00Atyp *m_dhatyp;
        const DblQ00Atyp::ATYP *m_atyp;
        DblQ00Asmp *m_dhasmp;
        const DblQ00Asmp::ASMP *m_asmp;
        DblQ00Almn *m_dhalmn;
        const DblQ00Almn::ALMN *m_almn;
        DblQ00Aptp *m_dhaptp;
        const DblQ00Aptp::APTP *m_aptp;
        DblQ00Awln *m_dhwrpc;
        const DblQ00Awln::AWLN *m_wrpc;
        DblQ00Atln *m_dhwtgc;
        const DblQ00Atln::ATLN *m_wtgc;
        DblQ00Acut *m_dhacut;
        const DblQ00Acut::ACUT *m_acut;
        DblQ00Alin *m_dhalin;
        const DblQ00Alin::ALIN *m_alin;
        DblQ00Wmdt *m_dhwmdt;
        const DblQ00Wmdt::WMDT *m_wmdt;
        DblQ00Wcsc *m_dhwcsc;
        const DblQ00Wcsc::WCSC *m_wcsc;
        DblQ00Wrpc *m_dhwrpcall;
        const DblQ00Wrpc::WRPC *m_wrpcall;
        DblQ00Wtgc *m_dhwtgcall;
        const DblQ00Wtgc::WTGC *m_wtgcall;
        DblQ00Wded *m_dhwded;
        const DblQ00Wded::WDED *m_wded;
        DblQ00Wsup *m_dhwsup;
        const DblQ00Wsup::WSUP *m_wsup;
        DblQ00Wspa *m_dhwspa;
        const DblQ00Wspa::WSPA *m_wspa;
        DblQ00Wchv *m_dhwchv;
        const DblQ00Wchv::WCHV *m_wchv;
        DblQ00Wcro *m_dhwcro;
        const DblQ00Wcro::WCRO *m_wcro;
        DblQ00Wcmi *m_dhwcmi;
        const DblQ00Wcmi::WCMI *m_wcmi;
        DblQ00Wlbi *m_dhwlbi;
        const DblQ00Wlbi::WLBI *m_wlbi;
        DblQ00Aszt *m_dhaszt;
        const DblQ00Aszt::ASZT *m_aszt;
        DblQ00IAcsc *m_dhiacsc;
        const DblQ00IAcsc::IACSC *m_iacsc;
        DblQ00Xtomo *m_dhxtomo;
        const DblQ00Xtomo::XTOMO *m_xtomo;

        const std::string m_geoTag;
        const std::string m_geoNode;
        IRDBAccessSvc *m_pRDBAccess;
        std::string m_asciiFileDB;
        IMessageSvc *m_msgSvc;
        bool m_useICSCAlines;

        RDBReaderAtlas &operator=(const RDBReaderAtlas &right);
        RDBReaderAtlas(const RDBReaderAtlas &);
    };

} // namespace MuonGM

#endif
