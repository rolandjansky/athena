/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MuonChamber_H
#define MuonChamber_H

#include "GeoModelKernel/GeoVFullPhysVol.h"
#include "MuonGeoModel/DetectorElement.h"
#include "MuonGeoModel/Station.h"

class IMessageSvc;

namespace MuonGM {
    class MuonDetectorManager;
    class CscReadoutElement;
    class MdtReadoutElement;
    class RpcReadoutElement;
    class TgcReadoutElement;
    class CscComponent;
    class MdtComponent;
    class RpcComponent;
    class TgcComponent;
    class Position;
    class FPVMAP;
    class MYSQL;

    class MuonChamber : public DetectorElement {

      public:
        double width;
        double length;
        double thickness;
        double longWidth; // for trapezoidal layers

        double rotangle[10]{};

        int stationPhiTGC(std::string_view stName, int fi, int zi, std::string_view geometry_version) const;
        inline void setFineClashFixingFlag(int value);

        MuonChamber(const MYSQL& mysql, Station *s);
        GeoVPhysVol *build(const StoredMaterialManager& matManager,
                           const MYSQL& mysql,
                           MuonDetectorManager *manager, int ieta, int iphi, bool is_mirrored, bool &isAssembly);
        virtual void print() override;
        inline void setFPVMAP(FPVMAP *fpvmap);

      private:
        void setCscReadoutGeom(const MYSQL& mysql,
                               CscReadoutElement *re, const CscComponent *cc, const Position &p);
        void setMdtReadoutGeom(const MYSQL& mysql,
                               MdtReadoutElement *re, const MdtComponent *cc, const Position &p);
        void setRpcReadoutGeom(const MYSQL& mysql,
                               RpcReadoutElement *re, const RpcComponent *cc, const Position &p, const std::string& geomVers, MuonDetectorManager *manager);
        void setTgcReadoutGeom(const MYSQL& mysql,
                               TgcReadoutElement *re, const TgcComponent *cc, const Position &p, const std::string& statname);

        Station *m_station;
        IMessageSvc *m_msgSvc;
        int m_enableFineClashFixing;

        FPVMAP *m_FPVMAP = nullptr;
    };

    void MuonChamber::setFineClashFixingFlag(int value) { m_enableFineClashFixing = value; }
    void MuonChamber::setFPVMAP(FPVMAP *fpvmap) { m_FPVMAP = fpvmap; }

} // namespace MuonGM

#endif
