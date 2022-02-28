/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MuonDetectorFactory001_H
#define MuonDetectorFactory001_H

#include "MuonReadoutGeometry/MuonDetectorManager.h"
///
#include "GeoModelKernel/GeoVDetectorFactory.h"
#include <string>

class GeoPhysVol;
class StoreGateSvc;
class IRDBAccessSvc;

namespace MuonGM {

    class MuonSystemDescription;

    class MuonDetectorFactory001 : public GeoVDetectorFactory {

        typedef std::map<std::string, std::string> AltAsciiDBMap;

      public:
        // Constructor:
        MuonDetectorFactory001(StoreGateSvc *pDetStore);

        // Destructor:
        ~MuonDetectorFactory001();

        // Creates the raw geometry tree: required,
        virtual void create(GeoPhysVol *world) override;

        virtual const MuonDetectorManager *getDetectorManager() const override;
        MuonDetectorManager *getDetectorManager();

        inline void setDBAtlasVersion(const std::string&v);
        inline void setDBMuonVersion(const std::string&v);
        inline void setDBkey(const std::string&v);
        inline void setDBnode(const std::string&v);

        inline void setAmdcDb(bool value);
        inline void setLayout(const std::string& str);
        inline void setCutoutsFlag(int);
        inline void setCutoutsBogFlag(int);
        inline void setCtbBisFlag(int);
        inline void setRDBAccess(IRDBAccessSvc *access);
        inline void setAltAsciiDBMap(const AltAsciiDBMap& asciidbmap);
        inline void setUseRDB(int rdb);
        inline void setControlAlines(int cA);
        inline void setMinimalGeoFlag(int minimalGeo);
        inline void setControlCscIntAlines(int cA);
        inline void setDumpAlines(bool cA);
        inline void setDumpCscIntAlines(bool cA);
        inline void setUseCscIntAlinesFromGM(bool cA);
        inline void setSelection(const std::vector<std::string>&,
                                 const std::vector<int>&,
                                 const std::vector<int>&);
        inline void setCachingFlag(int value);
        inline void setDumpMemoryBreakDown(bool value);
        inline void setCacheFillingFlag(int value);
        inline void setMdtDeformationFlag(int mdtDeformationFlag);
        inline void setMdtAsBuiltParaFlag(int mdtAsBuiltParaFlag);
        inline void setFineClashFixingFlag(int value);
        inline void hasCSC(bool value);
        inline void hasSTgc(bool value);
        inline void hasMM(bool value);
        inline void setABLinesAsciiSideA(const std::string &);
        inline void setABLinesAsciiSideC(const std::string &);
        inline void setMMAsBuiltJsonPath(const std::string &);

      private:

    
        std::string m_DBAtlasVersion{};
        std::string m_DBMuonVersion{};
        std::string m_DBkey{};
        std::string m_DBnode{};

        bool m_isAmdcDb{false};
        std::string m_layout{};
        int m_includeCutouts{0};
        int m_includeCutoutsBog{0};
        int m_includeCtbBis{0};
        int m_rdb{1};
        int m_controlAlines{0};
        int m_minimalGeoFlag{0};
        int m_controlCscIntAlines{0};
        bool m_dumpAlines{false};
        bool m_dumpCscIntAlines{false};
        bool m_useCscIntAlinesFromGM{true};

    
    

        int m_caching{0};
        int m_cacheFillingFlag{0};
        int m_mdtDeformationFlag{0};
        int m_mdtAsBuiltParaFlag = 0;
        bool m_dumpMemoryBreakDown{false};
        int m_enableFineClashFixing{0};
        bool m_hasCSC{true};
        bool m_hasSTgc{true};
        bool m_hasMM{true};

        std::string m_NSWABLinesSideA{};
        std::string m_NSWABLinesSideC{};
        std::string m_MMAsBuiltJsonPath{};

        std::vector<std::string> m_selectedStations;
        std::vector<int> m_selectedStEta;
        std::vector<int> m_selectedStPhi;

        std::unique_ptr<MuonSystemDescription> m_muon;
        MuonDetectorManager* m_manager{nullptr};
        StoreGateSvc *m_pDetStore{nullptr};
        IRDBAccessSvc *m_pRDBAccess{nullptr};
        AltAsciiDBMap m_altAsciiDBMap{};
    };

    void MuonDetectorFactory001::setDBAtlasVersion(const std::string&v) { m_DBAtlasVersion = v; }
    void MuonDetectorFactory001::setDBMuonVersion(const std::string&v) { m_DBMuonVersion = v; }
    void MuonDetectorFactory001::setDBkey(const std::string&v) { m_DBkey = v; }
    void MuonDetectorFactory001::setDBnode(const std::string&v) { m_DBnode = v; }

    void MuonDetectorFactory001::setAmdcDb(bool value) { m_isAmdcDb = value; }
    void MuonDetectorFactory001::setLayout(const std::string&str) { m_layout = str; }
    void MuonDetectorFactory001::setCutoutsFlag(int flag) { m_includeCutouts = flag; }
    void MuonDetectorFactory001::setCutoutsBogFlag(int flag) { m_includeCutoutsBog = flag; }
    void MuonDetectorFactory001::setCtbBisFlag(int flag) { m_includeCtbBis = flag; }
    void MuonDetectorFactory001::setUseRDB(int rdb) { m_rdb = rdb; }
    void MuonDetectorFactory001::setRDBAccess(IRDBAccessSvc *access) { m_pRDBAccess = access; }
    void MuonDetectorFactory001::setAltAsciiDBMap(const AltAsciiDBMap& asciidbmap) { m_altAsciiDBMap = asciidbmap; }
    void MuonDetectorFactory001::setControlAlines(int cA) { m_controlAlines = cA; }
    void MuonDetectorFactory001::setMinimalGeoFlag(int minimalGeo) { m_minimalGeoFlag = minimalGeo; }
    void MuonDetectorFactory001::setControlCscIntAlines(int cA) { m_controlCscIntAlines = cA; }
    void MuonDetectorFactory001::setDumpAlines(bool dumpAlines) { m_dumpAlines = dumpAlines; }
    void MuonDetectorFactory001::setDumpCscIntAlines(bool dumpAlines) { m_dumpCscIntAlines = dumpAlines; }
    void MuonDetectorFactory001::setUseCscIntAlinesFromGM(bool useIlinesFromGM) { m_useCscIntAlinesFromGM = useIlinesFromGM; }

    void MuonDetectorFactory001::setSelection(const std::vector<std::string>& vst,
                                              const std::vector<int>& veta,
                                              const std::vector<int>& vphi) {
        m_selectedStations = vst;
        m_selectedStEta = veta;
        m_selectedStPhi = vphi;
    }

    void MuonDetectorFactory001::setCachingFlag(int value) { m_caching = value; }
    void MuonDetectorFactory001::setCacheFillingFlag(int value) { m_cacheFillingFlag = value; }
    void MuonDetectorFactory001::setMdtDeformationFlag(int value) { m_mdtDeformationFlag = value; }
    void MuonDetectorFactory001::setMdtAsBuiltParaFlag(int value) { m_mdtAsBuiltParaFlag = value; }
    void MuonDetectorFactory001::setDumpMemoryBreakDown(bool value) { m_dumpMemoryBreakDown = value; }
    void MuonDetectorFactory001::setFineClashFixingFlag(int value) { m_enableFineClashFixing = value; }
    void MuonDetectorFactory001::hasCSC(bool value) { m_hasCSC = value; }
    void MuonDetectorFactory001::hasSTgc(bool value) { m_hasSTgc = value; }
    void MuonDetectorFactory001::hasMM(bool value) { m_hasMM = value; }
    void MuonDetectorFactory001::setABLinesAsciiSideA(const std::string &str) { m_NSWABLinesSideA = str; }
    void MuonDetectorFactory001::setABLinesAsciiSideC(const std::string &str) { m_NSWABLinesSideC = str; }
    void MuonDetectorFactory001::setMMAsBuiltJsonPath(const std::string &str) { m_MMAsBuiltJsonPath = str; }
} // namespace MuonGM

#endif
