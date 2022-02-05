/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

/***************************************************************************
 A Muon GeoVDetectorElement
 -----------------------------------------
***************************************************************************/

#include "MuonReadoutGeometry/MuonReadoutElement.h"

#include <TString.h>  // for Form

#include "GeoModelKernel/GeoDefinitions.h"
#include "GeoModelKernel/GeoPhysVol.h"
#include "GeoModelKernel/GeoTransform.h"
#include "GeoPrimitives/GeoPrimitives.h"
#include "MuonReadoutGeometry/MuonDetectorManager.h"
#include "MuonReadoutGeometry/MuonStation.h"
#include "TrkSurfaces/CylinderBounds.h"
#include "TrkSurfaces/StraightLineSurface.h"

namespace MuonGM {

    MuonReadoutElement::MuonReadoutElement(GeoVFullPhysVol* pv, int zi, int fi, bool is_mirrored, MuonDetectorManager* mgr) :
        TrkDetElementBase(pv), m_zi(zi), m_fi(fi), m_mirrored(is_mirrored), m_muon_mgr(mgr) {}

    MuonReadoutElement::~MuonReadoutElement() {}

    const Amg::Vector3D MuonReadoutElement::globalPosition() const {
        Amg::Transform3D xf = absTransform();
        return xf * Amg::Vector3D(0., 0., 0.);
    }

    bool MuonReadoutElement::barrel() const {
        if (m_statname[0] == 'B')
            return true;
        else
            return false;
    }

    bool MuonReadoutElement::endcap() const { return !barrel(); }

    bool MuonReadoutElement::largeSector() const {
        // this doesn't apply to TGC
        if(m_statname.size() >= 3){
        char c = m_statname[2];
        if (c == 'L')
            return true;
        else if (c == 'S')
            return false;
        else {
            if (c == 'E' || c == 'F' || c == 'G') return false;
            if (c == 'M' || c == 'R') return true;
        }
        }
        throw std::runtime_error(
            Form("File: %s, Line: %d\nMuonReadoutElement::largeSector() - is this Station in a largeSector ???? - DEFAULT answer is NO",
                 __FILE__, __LINE__));
        return false;
    }

    bool MuonReadoutElement::smallSector() const {
        // this doesn't apply to TGC
        return (!largeSector());
    }

    void MuonReadoutElement::setParentStationPV(const PVConstLink& x) {
        m_parentStationPV = x;
        setIndexOfREinMuonStation();
    }

    void MuonReadoutElement::setParentStationPV() {
        if (m_parentStationPV) return;

        std::string::size_type npos;
        PVConstLink pStat = PVConstLink(nullptr);
        PVConstLink myphysvol(getMaterialGeom());

        std::string name = (myphysvol->getLogVol())->getName();
        if ((npos = name.find("Station")) != std::string::npos) {
            pStat = myphysvol;
        } else
            for (unsigned int k = 0; k < 10; k++) {
                pStat = myphysvol->getParent();
                if (pStat == PVConstLink(nullptr)) break;
                name = (pStat->getLogVol())->getName();
                if ((npos = name.find("Station")) != std::string::npos) { break; }
                myphysvol = pStat;
            }

        m_parentStationPV = pStat;
        setIndexOfREinMuonStation();
    }

    PVConstLink MuonReadoutElement::parentStationPV() const { return m_parentStationPV; }

    int MuonReadoutElement::getIndexOfREinMuonStation() const { return m_indexOfREinMuonStation; }

    void MuonReadoutElement::setIndexOfREinMuonStation() {
        PVConstLink par = parentStationPV();
        if (par == PVConstLink(nullptr)) {
            throw std::runtime_error(
                Form("File: %s, Line: %d\nMuonReadoutElement::setIndexOfREinMuonStation() - parent station not found", __FILE__, __LINE__));
        }
        Query<unsigned int> c = par->indexOf(getMaterialGeom());
        if (c.isValid()) {
            m_indexOfREinMuonStation = (int)c;
        } else
            m_indexOfREinMuonStation = -999;
    }

    Amg::Transform3D MuonReadoutElement::toParentStation() const {
        PVConstLink par = parentStationPV();
        if (par == PVConstLink(nullptr)) {
            throw std::runtime_error(
                Form("File: %s, Line: %d\nMuonReadoutElement::toParentStation() - parent not found", __FILE__, __LINE__));
        }

        GeoTrf::Transform3D par_to_child = GeoTrf::Transform3D::Identity();

        if (m_indexOfREinMuonStation >= 0) par_to_child = par->getXToChildVol((unsigned int)m_indexOfREinMuonStation);
        return par_to_child;
    }

    void MuonReadoutElement::setParentMuonStation(const MuonStation* mstat) { m_parentMuonStation = mstat; }

    const MuonStation* MuonReadoutElement::parentMuonStation() const { return m_parentMuonStation; }

    double MuonReadoutElement::parentStationRsize() const { return parentMuonStation()->Rsize(); }
    double MuonReadoutElement::parentStationSsize() const { return parentMuonStation()->Ssize(); }
    double MuonReadoutElement::parentStationZsize() const { return parentMuonStation()->Zsize(); }
    double MuonReadoutElement::parentStationLongRsize() const { return parentMuonStation()->LongRsize(); }
    double MuonReadoutElement::parentStationLongSsize() const { return parentMuonStation()->LongSsize(); }
    double MuonReadoutElement::parentStationLongZsize() const { return parentMuonStation()->LongZsize(); }

    double MuonReadoutElement::parentStation_s_amdb() const {
        Amg::Vector3D scentre = parentMuonStationPos();
        double phi = (getStationPhi() - 1) * M_PI / 4.;
        if (smallSector()) phi = phi + M_PI / 8.;
        Amg::Vector3D Saxis = Amg::Vector3D(-sin(phi), cos(phi), 0.);
        Amg::Vector3D scVec = Amg::Vector3D(scentre.x(), scentre.y(), 0.);
        double s = scVec.x() * Saxis.x() + scVec.y() * Saxis.y();
        return s;
    }

    Amg::Vector3D MuonReadoutElement::parentMuonStationPos() const {
        Amg::Transform3D tr = Amg::CLHEPTransformToEigen(parentMuonStation()->getTransform());
        Amg::Vector3D scentre = tr * Amg::Vector3D(0., 0., 0.);
        return scentre;
    }

    const Amg::Vector3D MuonReadoutElement::AmdbLRSToGlobalCoords(Amg::Vector3D x) const {
        HepGeom::Point3D<double> p(x[0], x[1], x[2]);
        HepGeom::Transform3D msToGlobal = parentMuonStation()->getTransform();             // native_MuonStation to global
        const HepGeom::Transform3D* msToAmdb = parentMuonStation()->getNativeToAmdbLRS();  // native_MuonStation to Amdb local (szt)
        HepGeom::Point3D<double> p2 = msToGlobal * (msToAmdb->inverse()) * p;
        return Amg::Vector3D(p2.x(), p2.y(), p2.z());
    }

    const Amg::Transform3D MuonReadoutElement::AmdbLRSToGlobalTransform() const {
        HepGeom::Transform3D msToGlobal = parentMuonStation()->getTransform();             // native_MuonStation to global
        const HepGeom::Transform3D* msToAmdb = parentMuonStation()->getNativeToAmdbLRS();  // native_MuonStation to Amdb local (szt)
        return Amg::CLHEPTransformToEigen(msToGlobal * (msToAmdb->inverse()));
    }

    const Amg::Vector3D MuonReadoutElement::GlobalToAmdbLRSCoords(Amg::Vector3D x) const {
        HepGeom::Point3D<double> p(x[0], x[1], x[2]);
        HepGeom::Transform3D msToGlobal = parentMuonStation()->getTransform();             // native_MuonStation to global
        const HepGeom::Transform3D* msToAmdb = parentMuonStation()->getNativeToAmdbLRS();  // native_MuonStation to Amdb local (szt)
        HepGeom::Point3D<double> p2 = (*msToAmdb) * (msToGlobal.inverse()) * p;
        return Amg::Vector3D(p2.x(), p2.y(), p2.z());
    }

    const Amg::Transform3D MuonReadoutElement::GlobalToAmdbLRSTransform() const {
        HepGeom::Transform3D msToGlobal = parentMuonStation()->getTransform();             // native_MuonStation to global
        const HepGeom::Transform3D* msToAmdb = parentMuonStation()->getNativeToAmdbLRS();  // native_MuonStation to Amdb local (szt)
        return Amg::CLHEPTransformToEigen((*msToAmdb) * (msToGlobal.inverse()));
    }
}  // namespace MuonGM
