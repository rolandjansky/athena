/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
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

    class MuonChamber : public DetectorElement {

      public:
        double width;
        double length;
        double thickness;
        double longWidth; // for trapezoidal layers

        double rotangle[10];

        int stationPhiTGC(std::string stName, int fi, int zi, std::string geometry_version) const;
        inline void setFineClashFixingFlag(int value);

        MuonChamber(Station *s);
        GeoVPhysVol *build(MuonDetectorManager *manager, int ieta, int iphi, bool is_mirrored, bool &isAssembly);
        void print();
        inline void setFPVMAP(FPVMAP *fpvmap);

      private:
        void setCscReadoutGeom(CscReadoutElement *re, const CscComponent *cc, const Position &p, std::string geomVers, std::string statname);
        void setMdtReadoutGeom(MdtReadoutElement *re, const MdtComponent *cc, const Position &p);
        void setRpcReadoutGeom(RpcReadoutElement *re, const RpcComponent *cc, const Position &p, std::string geomVers, MuonDetectorManager *manager);
        void setTgcReadoutGeom(TgcReadoutElement *re, const TgcComponent *cc, const Position &p, std::string geomVers, std::string statname);

        Station *m_station;
        IMessageSvc *m_msgSvc;
        int m_enableFineClashFixing;

        FPVMAP *m_FPVMAP;
    };

    void MuonChamber::setFineClashFixingFlag(int value) { m_enableFineClashFixing = value; }
    void MuonChamber::setFPVMAP(FPVMAP *fpvmap) { m_FPVMAP = fpvmap; }

} // namespace MuonGM

#endif
