/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MuonStation_H
#define MuonStation_H

#include "GeoPrimitives/CLHEPtoEigenConverter.h"
#include "GeoPrimitives/GeoPrimitivesHelpers.h"
/// Ensure that the ATLAS eigen extenstions are loaded properly

#include <map>
#include <memory>

#include "GeoModelKernel/GeoAlignableTransform.h"
#include "MuonReadoutGeometry/GlobalUtilities.h"

class BLinePar;
class MdtAsBuiltPar;

namespace MuonGM {

    /**
       A MuonStation corresponds to a physical set of detectors in the
       MuonSpectrometer denoted as STATION.  In AMDB the structure of a
       station is described in the corresponding D-lines; there are as
       many MuonStations as replicas in the MS of such set of subdetectors.
       Therefore, it is identified by StationType (BML) ZI location
       (a la amdb) FI location (a la amdb) and it holds an alignable
       transform (from P line, plus A line). Notice that it is not
       possible to identify a MuonStation via StationType, StationEta and
       StationPhi, because in the case of TGC these fields of the offline
       identifiers specify a subdetector belonging to a TGC MuonStation.

       The MuonDetectorManager provides access to MuonStations via
       StationType (BML) ZI location (a la amdb) FI location (a la amdb)
       [the map access key is built by the MuonDetectorManager].

       MuonStations are the readout-geometry elements in a 1-1
       correspondence with the MuonChambers. Every MuonChamber instatiates
       the corresponding MuonStation and records it in the DetectorManager.
       Any MuonReadoutElement will hold a pointer to the parent MuonStation and
       the MuonStation will hold a map of the ReadoutElements with key = the integer
       index (a la amdb) called Job [number of order of the component in the station
       definition]
    */

    class MuonReadoutElement;

    class MuonStation {
    public:
        MuonStation(std::string_view stName, double m_Ssize, double m_Rsize, double m_Zsize, double m_LongSsize, double m_LongRsize,
                    double m_LongZsize, int zi, int fi, bool m_descratzneg);
        ~MuonStation();

        inline int getPhiIndex() const;  //!< a la AMDB

        inline int getEtaIndex() const;  //!< a la AMDB

        inline std::string getKey() const;
        inline std::string getStationType() const;  //!< like BMS, T1F, CSL

        inline std::string getStationName() const;  //!< like BMS5, T1F1, CSL1

        inline void setEtaPhiIndices(int eta, int phi);  //!< a la AMDB

        inline double Rsize() const;
        inline double Ssize() const;
        inline double Zsize() const;
        inline double LongRsize() const;
        inline double LongSsize() const;
        inline double LongZsize() const;
        double RsizeMdtStation() const;
        double ZsizeMdtStation() const;
        bool endcap() const;
        bool barrel() const;

        inline double xAmdbCRO() const;
        inline void setxAmdbCRO(double xpos);

        inline void setTransform(GeoAlignableTransform* xf);
        void setBlineFixedPointInAmdbLRS(double s0, double z0, double t0);
        HepGeom::Point3D<double> getBlineFixedPointInAmdbLRS() const;
        HepGeom::Point3D<double> getUpdatedBlineFixedPointInAmdbLRS() const;
        void updateBlineFixedPointInAmdbLRS();
        inline void setNativeToAmdbLRS(HepGeom::Transform3D xf);
        void setNominalAmdbLRSToGlobal(HepGeom::Transform3D xf);
        void setDeltaAmdbLRS(HepGeom::Transform3D xf);
        //!< set the delta transform in the amdb frame and update the geoModel Delta

        void setDelta_fromAline_forComp(int, double, double, double, double, double, double);
        void setDelta_fromAline(double, double, double, double, double, double);
        //!< set the delta transform in the amdb frame and update the geoModel Delta

        // inline void addMuonReadoutElement(const MuonReadoutElement* a);
        // inline void addMuonReadoutElement(const MuonReadoutElement* a, int jobIndex);
        void addMuonReadoutElementWithAlTransf(MuonReadoutElement* a, GeoAlignableTransform* ptrsf, int jobIndex);
        const MuonReadoutElement* getMuonReadoutElement(int jobIndex) const;
        MuonReadoutElement* getMuonReadoutElement(int jobIndex);
        GeoAlignableTransform* getComponentAlTransf(int jobIndex) const;
        inline int nMuonReadoutElements() const;
        void clearCache();
        void refreshCache();
        void fillCache();
        void clearBLineCache();
        void fillBLineCache();
        void setBline(const BLinePar* bline);
        inline GeoAlignableTransform* getGeoTransform() const;
        inline HepGeom::Transform3D getTransform() const;
        inline const HepGeom::Transform3D* getNativeToAmdbLRS() const;
        inline HepGeom::Transform3D getAmdbLRSToGlobal() const;
        inline const HepGeom::Transform3D* getNominalAmdbLRSToGlobal() const;
        inline double getALine_tras() const;
        inline double getALine_traz() const;
        inline double getALine_trat() const;
        inline double getALine_rots() const;
        inline double getALine_rotz() const;
        inline double getALine_rott() const;
        inline bool hasALines() const;
        inline bool hasBLines() const;
        inline bool hasMdtAsBuiltParams() const;
        const MdtAsBuiltPar* getMdtAsBuiltParams() const;
        void setMdtAsBuiltParams(const MdtAsBuiltPar* xtomo);

    private:
        // Declaring private message stream member.
        bool m_firstRequestBlineFixedP{true};

        std::string m_statname{};
        double m_Ssize{0.};
        double m_Rsize{0.};
        double m_Zsize{0.};
        double m_LongSsize{0.};
        double m_LongRsize{0.};
        double m_LongZsize{0.};
        double m_xAmdbCRO{0.};
        bool m_descratzneg;
        int m_statPhiIndex{0};
        int m_statEtaIndex{0};
        std::string m_key;
        GeoAlignableTransform* m_transform{nullptr};

        std::unique_ptr<HepGeom::Transform3D> m_delta_amdb_frame;
        std::unique_ptr<HepGeom::Transform3D> m_native_to_amdbl;
        std::unique_ptr<HepGeom::Transform3D> m_amdbl_to_global;  // nominal
        double m_rots{0.};
        double m_rotz{0.};
        double m_rott{0.};
        bool m_hasALines{false};
        bool m_hasBLines{false};
        HepGeom::Point3D<double> m_BlineFixedPointInAmdbLRS{0., 0., 0.};
        const MdtAsBuiltPar* m_XTomoData{nullptr};

        typedef std::pair<MuonReadoutElement*, GeoAlignableTransform*> pairRE_AlignTransf;
        std::map<int, pairRE_AlignTransf> m_REwithAlTransfInStation;  //!< keep track of the REs in this station

        MuonStation& operator=(const MuonStation& right) = delete;
        MuonStation(const MuonStation&) = delete;
    };

    int MuonStation::getPhiIndex() const { return m_statPhiIndex; }
    int MuonStation::getEtaIndex() const { return m_statEtaIndex; }
    std::string MuonStation::getStationType() const { return m_statname.substr(0, 3); }
    std::string MuonStation::getStationName() const { return m_statname; }
    void MuonStation::setEtaPhiIndices(int eta, int phi) {
        m_statEtaIndex = eta;
        m_statPhiIndex = phi;
    }

    void MuonStation::setTransform(GeoAlignableTransform* xf) { m_transform = xf; }

    GeoAlignableTransform* MuonStation::getGeoTransform() const { return m_transform; }

    HepGeom::Transform3D MuonStation::getTransform() const { return Amg::EigenTransformToCLHEP(m_transform->getTransform()); }

    std::string MuonStation::getKey() const { return m_key; }

    double MuonStation::Rsize() const { return m_Rsize; }
    double MuonStation::Ssize() const { return m_Ssize; }
    double MuonStation::Zsize() const { return m_Zsize; }
    double MuonStation::LongRsize() const { return m_LongRsize; }
    double MuonStation::LongSsize() const { return m_LongSsize; }
    double MuonStation::LongZsize() const { return m_LongZsize; }

    double MuonStation::xAmdbCRO() const { return m_xAmdbCRO; }

    void MuonStation::setxAmdbCRO(double xpos) { m_xAmdbCRO = xpos; }

    void MuonStation::setNativeToAmdbLRS(HepGeom::Transform3D xf) {
        if (!m_native_to_amdbl)
            m_native_to_amdbl = std::make_unique<HepGeom::Transform3D>(xf);
        else
            *m_native_to_amdbl = xf;
    }

    const HepGeom::Transform3D* MuonStation::getNativeToAmdbLRS() const { return m_native_to_amdbl.get(); }

    const HepGeom::Transform3D* MuonStation::getNominalAmdbLRSToGlobal() const { return m_amdbl_to_global.get(); }

    HepGeom::Transform3D MuonStation::getAmdbLRSToGlobal() const { return (*m_amdbl_to_global) * (*m_delta_amdb_frame); }

    int MuonStation::nMuonReadoutElements() const { return m_REwithAlTransfInStation.size(); }

    double MuonStation::getALine_tras() const { return (*m_delta_amdb_frame)[0][3]; }
    double MuonStation::getALine_traz() const { return (*m_delta_amdb_frame)[1][3]; }
    double MuonStation::getALine_trat() const { return (*m_delta_amdb_frame)[2][3]; }
    double MuonStation::getALine_rots() const { return m_rots; /*return (*_delta_amdb_frame)[2][1];*/ }
    double MuonStation::getALine_rotz() const { return m_rotz; /*return (*_delta_amdb_frame)[0][2];*/ }
    double MuonStation::getALine_rott() const { return m_rott; /*return (*_delta_amdb_frame)[1][0];*/ }
    bool MuonStation::hasALines() const { return m_hasALines; }
    bool MuonStation::hasBLines() const { return m_hasBLines; }
    bool MuonStation::hasMdtAsBuiltParams() const {
        if (m_XTomoData == NULL) return false;
        return true;
    }

}  // namespace MuonGM

#endif
