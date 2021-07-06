/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#include "MuonGeoModel/SpacerBeam.h"

#include "GeoModelKernel/GeoBox.h"
#include "GeoModelKernel/GeoDefinitions.h"
#include "GeoModelKernel/GeoLogVol.h"
#include "GeoModelKernel/GeoMaterial.h"
#include "GeoModelKernel/GeoPhysVol.h"
#include "GeoModelKernel/GeoShapeShift.h"
#include "GeoModelKernel/GeoShapeSubtraction.h"
#include "GeoModelKernel/GeoVPhysVol.h"
#include "MuonGeoModel/CHV_Technology.h"
#include "MuonGeoModel/CMI_Technology.h"
#include "MuonGeoModel/CRO_Technology.h"
#include "MuonGeoModel/CbmComponent.h"
#include "MuonGeoModel/LBI_Technology.h"
#include "MuonGeoModel/LbiComponent.h"
#include "MuonGeoModel/MYSQL.h"
#include "MuonGeoModel/StandardComponent.h"
#include "MuonGeoModel/Technology.h"

namespace MuonGM {

    SpacerBeam::SpacerBeam(Component *ss) : DetectorElement(ss->name), m_hole_pos1(0), m_hole_pos2(0), m_lb_height(0), m_lb_width(0), m_cross_excent(0) {
        StandardComponent *s = (StandardComponent *)ss;
        std::string componentType = (s->name).substr(0, 3);

        double tol = 1.e-4;

        width = s->dx1;
        longWidth = s->dx2;
        length = s->dy - tol;
        excent = s->excent;

        m_cy = s->posy;
        if (componentType == "CRO" || componentType == "CMI" || componentType == "CHV") {
            CbmComponent *ccbm = (CbmComponent *)s;
            m_hole_pos1 = ccbm->hole_pos1 - length / 2. - m_cy - tol / 2.;
            m_hole_pos2 = ccbm->hole_pos2 - length / 2. - m_cy - tol / 2.;
            m_lb_height = ccbm->lb_height;
            m_lb_width = ccbm->lb_width;
        }

        lowerThickness = 0.;

        MYSQL *mysql = MYSQL::GetPointer();
        if (componentType == "CHV") {
            CHV *ch = (CHV *)mysql->GetTechnology(s->name);
            thickness = ch->thickness;
            largeness = ch->largeness;
            height = ch->height - tol;

        } else if (componentType == "CRO") {
            CRO *cr = (CRO *)mysql->GetTechnology(s->name);
            thickness = cr->thickness;
            largeness = cr->largeness;
            height = cr->height - tol;

        } else if (componentType == "CMI") {
            CMI *cn = (CMI *)mysql->GetTechnology(s->name);
            thickness = cn->thickness;
            largeness = cn->largeness;
            height = cn->height - tol;

        } else if (componentType.substr(0, 2) == "LB") {
            LBI *lb = (LBI *)mysql->GetTechnology(s->name);
            thickness = lb->thickness;
            lowerThickness = lb->lowerThickness;
            largeness = thickness;
            height = lb->height;
            m_cross_excent = 0; // Place holder
        }
    }

    GeoVPhysVol *SpacerBeam::build(bool is_barrel) {
        int cutoutson = 0;
        return build(cutoutson, is_barrel);
    }

    GeoVPhysVol *SpacerBeam::build(int /*cutoutson*/, bool is_barrel) {
        GeoPhysVol *pvol = 0;
        GeoLogVol *lvol = 0;
        const GeoMaterial *mat = getMaterialManager()->getMaterial("std::Aluminium");
        if (name.substr(0, 3) == "CHV" || name.substr(0, 3) == "CRO" || name.substr(0, 3) == "CMI") {
            double sinexc = 0.;
            double cosexc = 1.;
            double volumelargeness = largeness;
            if ((name.substr(0, 3) == "CHV" || name.substr(0, 3) == "CRO") && !is_barrel) {
                double ltemp = std::sqrt(length * length + excent * excent);
                sinexc = std::abs(excent) / ltemp;
                cosexc = length / ltemp;
                length = ltemp - largeness * std::abs(excent) / length;
            }

            if (thickness > 0.) {
                // I-beam shape
                volumelargeness = largeness;
                const GeoShape *IBeamShape = new GeoBox(height / 2, volumelargeness / 2, length / 2);
                GeoBox *sideBox = new GeoBox(height / 2. - thickness, volumelargeness / 4., length / 2 + 1.);
                double yshift = volumelargeness / 4. + thickness / 2.;
                IBeamShape = &(IBeamShape->subtract((*sideBox) << GeoTrf::TranslateY3D(yshift)));
                IBeamShape = &(IBeamShape->subtract((*sideBox) << GeoTrf::TranslateY3D(-yshift)));

                // Cut holes for LB
                GeoBox *holeBox = new GeoBox(m_lb_height / 2. + 1., thickness / 2. + 1., m_lb_width / cosexc / 2. + thickness * sinexc / cosexc + 6.);
                IBeamShape = &(IBeamShape->subtract((*holeBox) << GeoTrf::TranslateZ3D(m_hole_pos1 / cosexc)));
                IBeamShape = &(IBeamShape->subtract((*holeBox) << GeoTrf::TranslateZ3D(m_hole_pos2 / cosexc)));
                lvol = new GeoLogVol(name, IBeamShape, mat);
                pvol = new GeoPhysVol(lvol);

            } else {
                // Box shape

                volumelargeness = width;
                const GeoBox *Cbox = new GeoBox(height / 2, volumelargeness / 2, length / 2);
                lvol = new GeoLogVol(name, Cbox, mat);
                pvol = new GeoPhysVol(lvol);
            }
            return pvol;

        } else if (name.substr(0, 2) == "LB") {
            const GeoShape *LBbox = new GeoBox(height / 2., (width - length / 4.) / 2., length / 2.);
            // (width - length/4) is temporary until excent parameter is put into LbiComponent
            GeoBox *innerBox = new GeoBox(height / 2. - thickness, width / 2. + 1., length / 2. - lowerThickness);
            LBbox = &(LBbox->subtract((*innerBox)));
            lvol = new GeoLogVol(name, LBbox, mat);
            pvol = new GeoPhysVol(lvol);
            return pvol;

        } else {
            return NULL;
        }
    }

    void SpacerBeam::print() {
        MsgStream log(Athena::getMessageSvc(), "MuGM::SpacerBeam");
        log << MSG::INFO << " SpacerBeam " << name << " :" << endmsg;
    }

} // namespace MuonGM
