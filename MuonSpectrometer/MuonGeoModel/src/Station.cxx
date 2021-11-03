/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#include "MuonGeoModel/Station.h"

#include "AthenaKernel/getMessageSvc.h"
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/SystemOfUnits.h"
#include "MuonGeoModel/MDT_Technology.h"
#include "MuonGeoModel/MYSQL.h"
#include "MuonGeoModel/StandardComponent.h"
#include "MuonGeoModel/SupComponent.h"
#include "MuonGeoModel/TgcComponent.h"

#include <algorithm>
#include <cassert>
#include <iostream>
#include <limits>
#include <utility>

namespace MuonGM {

    Station::Station(MYSQL& mysql, std::string s) :
      m_amdbOrigine_along_length (0),
      m_amdbOrigine_along_thickness (0),
      m_name(std::move(s)),
      m_hasMdts (false)
    {
        mysql.StoreStation(this);
    }

    Station::Station() :
      m_amdbOrigine_along_length (0),
      m_amdbOrigine_along_thickness (0),
      m_name("unknown"),
      m_hasMdts (false)
    {
    }

    Station::~Station() {
        for (unsigned int i = 0; i < m_components.size(); i++) {
            delete m_components[i];
        }
        for (unsigned int i = 0; i < m_cutouts.size(); i++) {
            delete m_cutouts[i];
        }
    }

    void Station::SetAlignPos(const AlignPos &p) {
        if (FindAlignPos(p.zindex, p.phiindex) != m_alignpositions.end() && p.jobindex == 0) {
            MsgStream log(Athena::getMessageSvc(), "MuonGeoModel.Station");
            log << MSG::WARNING << " this alignposition already exists !!!" << endmsg;
            log << MSG::WARNING << " for station named " << m_name << " setting alignposition at z,phi, key " << p.zindex << " " << p.phiindex << " " << p.zindex * 100 + p.phiindex
                << " and jobIndex = 0" << endmsg;
            assert(0);
        }

        int key = p.zindex * 100 + p.phiindex;
        m_alignpositions.insert(std::pair<int, AlignPos>(key, p));
    }

    AlignPosIterator Station::getFirstAlignPosInRange(int iz, int iphi, AlignPosIterator &lastAlignPosInRange) const {
        int key = iz * 100 + iphi;
        std::pair<AlignPosIterator, AlignPosIterator> ppp = m_alignpositions.equal_range(key);
        lastAlignPosInRange = ppp.second;
        return ppp.first;
    }

    AlignPosIterator Station::FindAlignPos(int iz, int iphi) const {
        // imt - probably needs to be different key for align pos
        int key = iz * 100 + iphi;
        // std::cout<<" looking for align pos. with key "<<key<<std::endl;
        return m_alignpositions.find(key);
    }

    int Station::CountAlignPos(int iz, int iphi) const {
        // imt - probably needs to be different key for align pos
        int key = iz * 100 + iphi;
        // std::cout<<" looking for align pos. with key "<<key<<std::endl;
        return m_alignpositions.count(key);
    }

    AlignPosIterator Station::abegin() const { return m_alignpositions.begin(); }
    AlignPosIterator Station::aend() const { return m_alignpositions.end(); }

    void Station::SetComponent(Component *c) { m_components.push_back(c); }

    void Station::SetCutout(Cutout *c) { m_cutouts.push_back(c); }

    Component *Station::GetComponent(int i) const { return m_components[i]; }

    Cutout *Station::GetCutout(int i) const { return m_cutouts[i]; }

    void Station::SetPosition(Position p) {
        if (FindPosition(p.zindex, p.phiindex) != end()) {
            MsgStream log(Athena::getMessageSvc(), "MuonGeoModel.Station");
            log << MSG::WARNING << " this position already exists !!!" << endmsg;
            log << MSG::WARNING << " for station named " << m_name << " setting position at z,phi, key " << p.zindex << " " << p.phiindex << " " << p.zindex * 100 + p.phiindex
                << endmsg;
            assert(0);
        } else {
            p.isAssigned = true;
            m_positions[p.zindex * 100 + p.phiindex] = p;
        }
    }

    PositionIterator Station::FindPosition(int iz, int iphi) const {
        int key = iz * 100 + iphi;
        return m_positions.find(key);
    }

    int Station::Npositions() const { return m_positions.size(); }

    PositionIterator Station::begin() const { return m_positions.begin(); }

    PositionIterator Station::end() const { return m_positions.end(); }

    std::string Station::GetName() const { return m_name; }

    double Station::GetThickness(const MYSQL& mysql) const {
        double thick = 0;
        if (m_name[0] == 'T') {
            for (unsigned int i = 0; i < m_components.size(); i++) {
                TgcComponent *t = (TgcComponent *)m_components[i];
                thick = thick > t->GetThickness(mysql) + t->posz ? thick : t->GetThickness(mysql) + t->posz;
            }
        } else {
            MsgStream log(Athena::getMessageSvc(), "MuonGeoModel.Station");
            double zstart = std::numeric_limits<double>::max();

            for (unsigned int i = 0; i < m_components.size(); i++) {
                StandardComponent *s = (StandardComponent *)m_components[i];
                thick = thick > s->GetThickness(mysql) + s->posz ? thick : s->GetThickness(mysql) + s->posz;
                if (i == 0 || s->posz < zstart)
                    zstart = s->posz;

                if (log.level() <= MSG::VERBOSE) {
                    log << MSG::VERBOSE << "Station " << m_name << " calculating  Thinkness = " << thick << " and zstart = " << zstart << endmsg;
                }
            }

            if (fabs(zstart) > 0.001) {
                thick = thick - zstart;
                m_amdbOrigine_along_thickness = -zstart;
                if (log.level() <= MSG::VERBOSE) {
                    log << MSG::VERBOSE << "Station " << m_name << " redefining Thinkness = " << thick << " because zstart = " << zstart
                        << "; then amdbOrigine_along_thickness = " << m_amdbOrigine_along_thickness << endmsg;
                }
            }
        }

        return thick;
    }

    double Station::GetExtraTopThickness() const {
        if (m_name[0] != 'B')
            return 0.;

        return 0.;
        /* COMMENTING IS JUST TO MAKE COVERITY HAPPY
           WHY THIS CODE WAS NEVER ENTERED HAS TO BE FIGURED OUT
            double xupsup = 0.;
            double deltaup = 0.;
            int nsup=0;
            for (unsigned int i=0;i<m_components.size();i++)
            {
                std::string n=m_components[i]->m_name.substr(0,3);
                if (n=="SUP")
                {
                    ++nsup;
                    SupComponent *s=(SupComponent *)m_components[i];
                    deltaup = s->posz + s->topsizewrtAMDB0();
        //             log << MSG::DEBUG<<" deltaup: s->posz, s->topsizewrtAMDB0 "
        //                      <<s->posz<<" "
        //                      <<s->topsizewrtAMDB0()<<endmsg;
                    double pos = -m_thickness/2. + deltaup;
        //            log << MSG::DEBUG<<" pos, m_thickness/2., deltaup "<<pos<<" "
        //	    <<-m_thickness/2.<<" "<<deltaup<<endmsg;
                    //xupsup > pos ? xupsup : pos;
                    if (xupsup <= pos) xupsup=pos;
        //             log << MSG::DEBUG<<" SUP component named "<<s->m_name
        //                       <<" xupsup, m_thickness/2. ="<<xupsup<<" "<<m_thickness/2.<<endmsg;
                }
            }
            double dtop=xupsup - m_thickness/2.;
            //log << MSG::DEBUG<<" dtop = "<<dtop<<endmsg;
            if (dtop > 0.) {
              if (log.level()<=MSG::VERBOSE) log << MSG::VERBOSE<<" GetExtraTopThickness for stat = "
            <<m_name<<" with "<<nsup<<" SUPs is "<<dtop<<endmsg;
                return dtop;
            }

            return 0.;
        */
    }

    double Station::GetExtraBottomThickness() const {
        if (m_name[0] != 'B')
            return 0.;

        return 0.;
        /* COMMENTING IS JUST TO MAKE COVERITY HAPPY
           WHY THIS CODE WAS NEVER ENTERED HAS TO BE FIGURED OUT
            double xdownsup = 0.;
            double deltadown = 0.;
            int nsup =0;
            for (unsigned int i=0;i<m_components.size();i++)
            {
                std::string n=m_components[i]->m_name.substr(0,3);
                if (n=="SUP")
                {
                    ++nsup;
                    SupComponent *s=(SupComponent *)m_components[i];
                    deltadown = s->posz - s->bottomsizewrtAMDB0();
        //             log << MSG::DEBUG<<" deltadown: s->posz, s->bottomsizewrtAMDB0 "
        //                      <<s->posz<<" "
        //                      <<s->bottomsizewrtAMDB0()<<endmsg;
                    double pos = -m_thickness/2. + deltadown;
                    // log << MSG::DEBUG<<" pos, m_thickness/2., deltadown "<<pos
              //	    <<" "<<-m_thickness/2.<<" "<<deltadown<<endmsg;
                    //xdownsup < pos ? xdownsup : pos;
                    if (xdownsup >=pos ) xdownsup=pos;
        //             log << MSG::DEBUG<<" SUP component named "<<s->m_name
        //                      <<" xdownsup, -m_thickness/2. ="<<xdownsup<<" "<<-m_thickness/2.<<endmsg;
                }
            }
            double dbottom = - xdownsup - m_thickness/2.;
            if (dbottom > 0.) {
              if (log.level()<=MSG::VERBOSE) log << MSG::VERBOSE <<" GetExtraBottomThickness for stat = "
            <<m_name<<" with "<<nsup<<" SUPs is "<<dbottom<<endmsg;
                return dbottom;
            }
            return 0.;
        */
    }

    double Station::GetLength() const {
        double len = 0;
        if (m_name[0] == 'T') {
            double innerrad = std::numeric_limits<double>::max();
            double outerrad = 0.;

            for (unsigned int i = 0; i < m_components.size(); i++) {
                TgcComponent *tg = (TgcComponent *)m_components[i];

                if (tg->posy < innerrad) {
                    innerrad = tg->posy;
                }

                if (tg->posy + tg->dy > outerrad) {
                    outerrad = tg->posy + tg->dy;
                }
            }

            len = outerrad - innerrad;
        } else {
            double ystart = 999999.;

            MsgStream log(Athena::getMessageSvc(), "MuonGeoModel.Station");
            for (unsigned int i = 0; i < m_components.size(); i++) {
                StandardComponent *sc = (StandardComponent *)m_components[i];
                if (log.level() <= MSG::VERBOSE) {
                    log << MSG::VERBOSE << "Station " << m_name << " *** comp " << i << " named " << sc->name << " posy " << sc->posy << " dy " << sc->dy << " len " << len
                        << " ystart " << ystart << endmsg;
                }
                if ((sc->dy + sc->posy) > len)
                    len = sc->dy + sc->posy;
                if (i == 0 || sc->posy < ystart)
                    ystart = sc->posy;
                if (log.level() <= MSG::VERBOSE) {
                    log << MSG::VERBOSE << " now len = " << len << " ystart = " << ystart << endmsg;
                }
            }

            if (fabs(ystart) > 0.001) {
                len = len - ystart;
                m_amdbOrigine_along_length = -ystart;

                if (log.level() <= MSG::VERBOSE) {
                    log << MSG::VERBOSE << "Station " << m_name << " redefining len = " << len << " because ystart = " << ystart << endmsg;
                }
            }
        }

        return len;
    }

    double Station::getYMin() const {
        if (m_name[0] != 'T') {
            double ystart = std::numeric_limits<double>::max();

            for (unsigned int i = 0; i < m_components.size(); i++) {
                StandardComponent *sc = (StandardComponent *)m_components[i];
                if (i == 0 || sc->posy < ystart)
                    ystart = sc->posy;
            }
            return ystart;
        }
        return 0.;
    }

    double Station::GetWidth1() const {
        double maxdxmin = std::numeric_limits<double>::lowest();
        double ymin = getYMin();
        double w = 0;
        for (unsigned int i = 0; i < m_components.size(); i++) {
            std::string_view n = std::string_view(m_components[i]->name).substr(0, 3);
            if (n == "TGC") {
                double dw = 20.;
                std::string_view typetgc = std::string_view(m_components[i]->name).substr(3, 2);
                // in case of station containing one module
                if (typetgc == "01" || typetgc == "06" || typetgc == "12" || typetgc == "18" || typetgc == "19" || typetgc == "20" || typetgc == "21") {
                    dw = 0.;
                }

                if (w == 0) {
                    w = m_components[i]->dx1 + dw;
                } else {
                    if (w > m_components[i]->dx1 + dw) {
                        w = m_components[i]->dx1 + dw;
                    }
                }
            } else {
                double dxmin = 0.;
                if (fabs(m_components[i]->dy) < 1.e-10) {
                    dxmin = m_components[i]->dx1;
                } else {
                    double num = (m_components[i]->dx2 - m_components[i]->dx1) / 2.;
                    double tantheta = num != 0 ? num / m_components[i]->dy : 0;
                    double y = ((StandardComponent *)m_components[i])->posy;
                    dxmin = m_components[i]->dx1 + 2. * tantheta * (ymin - y);
                }

                if (maxdxmin < dxmin && (n.substr(0, 2) != "LB" || m_name[0] == 'B'))
                    maxdxmin = dxmin;
            }
        }

        if (m_name.substr(0, 1) == "T")
            return w;
        else
            return maxdxmin;
    }

    double Station::GetWidth2() const {
        // double ymin= -getAmdbOrigine_along_length();
        double ymax = getYMin() + GetLength();
        double maxdxmax = std::numeric_limits<double>::lowest();
        double w = 0;

        for (unsigned int i = 0; i < m_components.size(); i++) {
            if (w < m_components[i]->dx2) {
                w = m_components[i]->dx2;
            }

            std::string_view n = std::string_view(m_components[i]->name).substr(0, 3);
            if (n == "TGC") {
                double dw = 20.;
                std::string_view typetgc = std::string_view(m_components[i]->name).substr(3, 2);
                // in case of one station containing one module
                if (typetgc == "01" || typetgc == "06" || typetgc == "12" || typetgc == "18" || typetgc == "19" || typetgc == "20" || typetgc == "21") {
                    dw = 0.;
                }
                w += dw;
            } else {
                double dxmax = 0.;
                if (fabs(m_components[i]->dy) < 1.e-10)
                    dxmax = m_components[i]->dx2;
                else {
                    double num = (m_components[i]->dx2 - m_components[i]->dx1) / 2.;
                    double tantheta = num != 0 ? num / m_components[i]->dy : 0;
                    double y = ((StandardComponent *)m_components[i])->posy;
                    dxmax = m_components[i]->dx1 + 2. * tantheta * (ymax - y);
                }

                if (maxdxmax < dxmax)
                    maxdxmax = dxmax;
            }
        }

        if (m_name.compare(0, 1, "T") == 0)
            return w;
        else
            return maxdxmax;
    }

    int Station::GetNrOfComponents() const { return m_components.size(); }

    int Station::GetNrOfCutouts() const { return m_cutouts.size(); }

    std::ostream &operator<<(std::ostream &os, const Station &s) {
        os << "Station m_name: " << s.m_name << " " << s.m_components.size() << std::endl;
        for (unsigned int i = 0; i < s.m_components.size(); i++)
            os << "\t" << s.m_components[i] << std::endl;

        PositionIterator k;
        for (k = s.begin(); k != s.end(); k++)
            os << "\t\t" << (*k).second << std::endl;

        AlignPosIterator ak;
        for (ak = s.abegin(); ak != s.aend(); ak++)
            os << "\t\t" << (*ak).second << std::endl;

        os << "--------------------------------------------------" << std::endl;
        return os;
    }

    double Station::mdtHalfPitch(const MYSQL& mysql) const {
        const MDT *mdtobj = dynamic_cast<const MDT*>(mysql.GetATechnology("MDT0"));
        double mdthalfpitch = 0.5 * (mdtobj->pitch);

        if (hasMdts()) {
            MsgStream log(Athena::getMessageSvc(), "MuonGeoModel.Station");

            for (int icomp = 0; icomp < GetNrOfComponents(); ++icomp) {
                const Component *c = GetComponent(icomp);
                if (c->name.compare(0, 3, "MDT") != 0)
                    continue;
                const MDT *mdtobj = dynamic_cast<const MDT*>(mysql.GetATechnology(c->name));
                if (!mdtobj) {
                    if (log.level() <= MSG::ERROR) {
                        log << MSG::ERROR << "Cannot find MDT definition for component " << c->name << endmsg;
                    }
                    continue;
                }
                mdthalfpitch = 0.5 * (mdtobj->pitch);
                if (log.level() <= MSG::DEBUG) {
                    log << MSG::DEBUG << "Setting halfpitch " << mdthalfpitch << " for station " << m_name << endmsg;
                }
                break;
            }
        }
        return mdthalfpitch;
    }

    // this is really needed
    GeoTrf::Transform3D Station::native_to_tsz_frame(const MYSQL& mysql, const Position &p) const {
        MsgStream log(Athena::getMessageSvc(), "MuonGeoModel.Station");
        int amdbVersion = mysql.getNovaReadVersion();

        if (amdbVersion > 0 && amdbVersion < 7 && m_name[0] != 'B') {
            log << MSG::DEBUG << "For AMDB version " << amdbVersion << " a left-handed chamber coordinate system was used "
                << " for endcap side A so be very careful." << endmsg;
        }

        // first apply here the mirror symmetry: (we, in fact, apply a rotation)
        GeoTrf::Transform3D mirrsym = GeoTrf::Transform3D::Identity();
        if (p.isMirrored) {
            if (m_name[0] == 'B') {
                mirrsym = GeoTrf::RotateX3D(180. * Gaudi::Units::deg);
            }
        }

        // define the translation to position the chamber in the tzs frame
        GeoTrf::Translate3D AMDBorgTranslation(0, 0, 0);
        if ((m_name[0] == 'B' || p.isBarrelLike) && p.zindex < 0 && (!p.isMirrored) && hasMdts()) {
            double halfpitch = mdtHalfPitch(mysql);
            AMDBorgTranslation = GeoTrf::Translate3D(GetThickness(mysql) / 2. - getAmdbOrigine_along_thickness(mysql), 0., GetLength() / 2. - (getAmdbOrigine_along_length() + halfpitch));

            log << MSG::VERBOSE << " GetThickness / getAmdbO_thick / GetLength() / getAmdbO_length " << GetThickness(mysql) << " " << getAmdbOrigine_along_thickness(mysql) << " "
                << GetLength() << " " << getAmdbOrigine_along_length() + halfpitch << endmsg;
        } else {
            if (m_name[0] == 'T') {
                AMDBorgTranslation = GeoTrf::Translate3D(GetThickness(mysql) / 2. - getAmdbOrigine_along_thickness(mysql), 0.,
                                                         GetLength() / 2. - getAmdbOrigine_along_length() + ((TgcComponent *)GetComponent(0))->posy);
            } else {
                AMDBorgTranslation = GeoTrf::Translate3D(GetThickness(mysql) / 2. - getAmdbOrigine_along_thickness(mysql), 0., GetLength() / 2. - getAmdbOrigine_along_length());
            }

            log << MSG::VERBOSE << " GetThickness / getAmdbO_thick / GetLength() / getAmdbO_length " << GetThickness(mysql) << " " << getAmdbOrigine_along_thickness(mysql) << " "
                << GetLength() << " " << getAmdbOrigine_along_length() << endmsg;
        }

        // // define the rotations by alpha, beta, gamma
        // GeoTrf::Rotate3D ralpha = GeoTrf::RotateX3D(p.alpha*Gaudi::Units::deg);
        // GeoTrf::Rotate3D rbeta  = GeoTrf::RotateZ3D(p.beta*Gaudi::Units::deg);
        // GeoTrf::Rotate3D rgamma;
        // rgamma = GeoTrf::RotateY3D(p.gamma*Gaudi::Units::deg);
        // log<<MSG::VERBOSE<<" gamma is not changing sign - original "<<p.gamma<<" new one "<<p.gamma<<endmsg;
        // log<<MSG::VERBOSE<<" alpha / beta "<<p.alpha<<" "<<p.beta<<endmsg;

        // // apply all transform in sequence
        // //    GeoTrf::Transform3D to_tsz = rgamma*rbeta*ralpha*AMDBorgTranslation*mirrsym;  // works for barrel and barrel-like
        // // imt: tested for CTB2004, seems to work for all amdb versions...
        // GeoTrf::Transform3D to_tsz = rgamma*rbeta*ralpha*AMDBorgTranslation*mirrsym;
        GeoTrf::Transform3D to_tsz = AMDBorgTranslation * mirrsym;

        return to_tsz;
    }

    GeoTrf::Transform3D Station::tsz_to_native_frame(const MYSQL& mysql,
                                                     const Position &p) const { return (native_to_tsz_frame(mysql, p)).inverse(); }

    // this is really needed
    GeoTrf::Transform3D Station::tsz_to_global_frame(const MYSQL& mysql,
                                                     const Position &p) const {
        GeoTrf::Transform3D nominalTransf = GeoTrf::Transform3D::Identity();

        GeoTrf::Vector3D vec;
        double RAD;

        if (m_name[0] == 'T') {
            RAD = p.radius;
        } else {
            RAD = p.radius;
        }

        vec.x() = RAD * cos(p.phi * Gaudi::Units::deg);
        vec.x() = vec.x() - p.shift * sin((p.phi) * Gaudi::Units::deg);
        vec.y() = RAD * sin(p.phi * Gaudi::Units::deg);
        vec.y() = vec.y() + p.shift * cos((p.phi) * Gaudi::Units::deg);

        if (p.isMirrored) {
            if ((p.isBarrelLike) || (m_name[0] == 'B')) {
                // correct the z location (=-p.z-m_length) for possible m_amdbOrigine_along_length
                vec.z() = p.z + getAmdbOrigine_along_length();
            } else {
                vec.z() = p.z + GetThickness(mysql); // re-establish the amdb z location (with a - sign)
            }
        } else {
            if ((p.isBarrelLike) || (m_name[0] == 'B' && p.zindex < 0 && hasMdts())) {
                double halfpitch = mdtHalfPitch(mysql);
                vec.z() = p.z + halfpitch;
            } else {
                vec.z() = p.z;
            }
        }

        MsgStream log(Athena::getMessageSvc(), "MuonGeoModel.Station");
        log << MSG::VERBOSE << " translation according to " << vec.x() << " " << vec.y() << " " << vec.z() << endmsg;

        // // define the rotations by alpha, beta, gamma
        GeoTrf::RotateX3D ralpha(p.alpha * Gaudi::Units::deg);
        GeoTrf::RotateZ3D rbeta(p.beta * Gaudi::Units::deg);
        GeoTrf::RotateY3D rgamma(p.gamma * Gaudi::Units::deg);

        log << MSG::VERBOSE << " gamma is not changing sign - original " << p.gamma << " new one " << p.gamma << endmsg;
        log << MSG::VERBOSE << " alpha / beta " << p.alpha << " " << p.beta << endmsg;

        // // apply all transform in sequence
        // //    GeoTrf::Transform3D to_tsz = rgamma*rbeta*ralpha*AMDBorgTranslation*mirrsym;
        // works for barrel and barrel-like
        // // imt: tested for CTB2004, seems to work for all amdb versions...
        GeoTrf::Transform3D abgRot = rgamma * rbeta * ralpha;

        if (m_name[0] == 'B' || p.isBarrelLike) {
            // here all Barrel chambers
            nominalTransf = GeoTrf::RotateZ3D(p.phi * Gaudi::Units::deg);
        } else {
            // replace this with the folowing lines 8/06/2006 SS because, EC not mirrored chambers have anyway to be rotated
            // by 180deg around z to mov ecoherently their local reference frame and the tube-layer numbering
            //         if ( p.z>=0 || ( p.z<0 && !(p.isMirrored) ) ){
            //             nominalTransf =  GeoTrf::Transform3D(GeoTrf::RotateY3D(-90*Gaudi::Units::deg)*
            // 					    GeoTrf::RotateX3D(p.phi*Gaudi::Units::deg-180*Gaudi::Units::deg));
            //         }
            //         else if (p.z<0 && p.isMirrored){
            //             nominalTransf =  GeoTrf::Transform3D(GeoTrf::RotateY3D(-90*Gaudi::Units::deg)*
            //                                             GeoTrf::RotateX3D(p.phi*Gaudi::Units::deg-180*Gaudi::Units::deg)*
            //                                             GeoTrf::RotateZ3D(180*Gaudi::Units::deg));
            //         }
            if (p.z >= 0) {
                nominalTransf = GeoTrf::Transform3D(GeoTrf::RotateY3D(-90 * Gaudi::Units::deg) * GeoTrf::RotateX3D(p.phi * Gaudi::Units::deg - 180 * Gaudi::Units::deg));
            } else if (p.z < 0) {
                nominalTransf = GeoTrf::Transform3D(GeoTrf::RotateY3D(-90 * Gaudi::Units::deg) * GeoTrf::RotateX3D(p.phi * Gaudi::Units::deg - 180 * Gaudi::Units::deg) *
                                                    GeoTrf::RotateZ3D(180 * Gaudi::Units::deg));
            } else {
                log << MSG::WARNING << "Problem here p.z, mirrored " << p.z << " " << p.isMirrored << endmsg;
            }
        }

        return GeoTrf::Translate3D(vec.x(), vec.y(), vec.z()) * nominalTransf * abgRot;
    }

    GeoTrf::Transform3D Station::global_to_tsz_frame(const MYSQL& mysql,
                                                     const Position &p) const { return (tsz_to_global_frame(mysql, p)).inverse(); }

    GeoTrf::Transform3D Station::getNominalTransform(const MYSQL& mysql,
                                                     const Position &p) const { return tsz_to_global_frame(mysql, p) * native_to_tsz_frame(mysql, p); }

    GeoTrf::Transform3D Station::getAlignedTransform(const MYSQL& mysql,
                                                     const AlignPos &ap, const Position &p) const {
        return tsz_to_global_frame(mysql, p) * getDeltaTransform_tszFrame(mysql, ap) * native_to_tsz_frame(mysql, p);
    }

    GeoTrf::Transform3D Station::getDeltaTransform_tszFrame(const MYSQL& mysql,
                                                            const AlignPos &ap) const {
        MsgStream log(Athena::getMessageSvc(), "MuonGeoModel.Station");
        if (ap.tras != 0 || ap.trat != 0 || ap.traz != 0 || ap.rots != 0 || ap.rott != 0 || ap.rotz != 0) {
            log << MSG::VERBOSE << "Setting corrections." << endmsg;
            log << MSG::VERBOSE << "For station " << m_name << " corrections sent are " << ap.tras << " " << ap.traz << " " << ap.trat << " " << ap.rots << " " << ap.rotz << " "
                << ap.rott << " isBarrel=" << ap.isBarrel << endmsg;
            log << MSG::VERBOSE << "length=" << GetLength() << " m_thickness=" << GetThickness(mysql) << endmsg;
        }

        GeoTrf::RotateX3D rott(ap.rott);
        GeoTrf::RotateZ3D rotz(ap.rotz);
        GeoTrf::RotateY3D rots(ap.rots);
        GeoTrf::Transform3D trans = GeoTrf::TranslateY3D(ap.tras) * GeoTrf::TranslateZ3D(ap.traz) * GeoTrf::TranslateX3D(ap.trat);

        GeoTrf::Transform3D delta = trans * rots * rotz * rott;

        log << MSG::VERBOSE << " delta transform in the tsz frame --------------" << endmsg << delta(0, 0) << " " << delta(0, 1) << " " << delta(0, 2) << " " << delta(0, 3) << " "
            << endmsg << delta(1, 0) << " " << delta(1, 1) << " " << delta(1, 2) << " " << delta(1, 3) << " " << endmsg << delta(2, 0) << " " << delta(2, 1) << " " << delta(2, 2)
            << " " << delta(2, 3) << " " << endmsg;

        // our delta transform must be applied in the tsz frame:
        return delta;
    }

    GeoTrf::Transform3D Station::getDeltaTransform(const MYSQL& mysql,
                                                   const AlignPos &ap, const Position &p) const {
        // GM applies Delta transform like transform*delta
        GeoTrf::Transform3D deltaGM = tsz_to_native_frame(mysql, p) * getDeltaTransform_tszFrame(mysql, ap) * native_to_tsz_frame(mysql, p);
        return deltaGM;
    }

    double Station::getAmdbOrigine_along_length() const {
        GetLength();
        return m_amdbOrigine_along_length;
    }

    double Station::getAmdbOrigine_along_thickness(const MYSQL& mysql) const {
        GetThickness(mysql);
        return m_amdbOrigine_along_thickness;
    }

} // namespace MuonGM
