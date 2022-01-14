/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
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

    SpacerBeam::SpacerBeam(const MYSQL& mysql,
                           Component *ss) : DetectorElement(ss->name), m_hole_pos1(0), m_hole_pos2(0), m_lb_height(0), m_lb_width(0), m_cross_excent(0) {
        StandardComponent *s = (StandardComponent *)ss;
        std::string_view componentType = std::string_view(s->name).substr(0, 3);

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

        if (componentType == "CHV") {
            const CHV *ch = dynamic_cast<const CHV*>(mysql.GetTechnology(s->name));
            thickness = ch->thickness;
            largeness = ch->largeness;
            height = ch->height - tol;

        } else if (componentType == "CRO") {
            const CRO *cr = dynamic_cast<const CRO*>(mysql.GetTechnology(s->name));
            thickness = cr->thickness;
            largeness = cr->largeness;
            height = cr->height - tol;

        } else if (componentType == "CMI") {
            const CMI *cn = dynamic_cast<const CMI*>(mysql.GetTechnology(s->name));
            thickness = cn->thickness;
            largeness = cn->largeness;
            height = cn->height - tol;

        } else if (componentType.substr(0, 2) == "LB") {
            const LBI *lb = dynamic_cast<const LBI*>(mysql.GetTechnology(s->name));
            thickness = lb->thickness;
            lowerThickness = lb->lowerThickness;
            largeness = thickness;
            height = lb->height;
            m_cross_excent = 0; // Place holder
        }
    }

    GeoVPhysVol *SpacerBeam::build(const StoredMaterialManager& matManager,
                                   bool is_barrel) {
        int cutoutson = 0;
        return build(matManager, cutoutson, is_barrel);
    }

    GeoVPhysVol *SpacerBeam::build(const StoredMaterialManager& matManager,
                                   int /*cutoutson*/, bool is_barrel) {
        GeoPhysVol *pvol = nullptr;
        GeoLogVol *lvol = nullptr;
        const GeoMaterial *mat = matManager.getMaterial("std::Aluminium");
        if (name.compare(0, 3, "CHV") == 0 || name.compare(0, 3, "CRO") == 0 || name.compare(0, 3, "CMI") == 0) {
            double sinexc = 0.;
            double cosexc = 1.;
            double volumelargeness = largeness;
            if ((name.compare(0, 3, "CHV") == 0 || name.compare(0, 3, "CRO") == 0) && !is_barrel) {
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

        } else if (name.compare(0, 2, "LB") == 0) {
            const GeoShape *LBbox = new GeoBox(height / 2., (width - length / 4.) / 2., length / 2.);
            // (width - length/4) is temporary until excent parameter is put into LbiComponent
            GeoBox *innerBox = new GeoBox(height / 2. - thickness, width / 2. + 1., length / 2. - lowerThickness);
            LBbox = &(LBbox->subtract((*innerBox)));
            lvol = new GeoLogVol(name, LBbox, mat);
            pvol = new GeoPhysVol(lvol);
            return pvol;

        } else {
            return nullptr;
        }
    }

    void SpacerBeam::print() {
        MsgStream log(Athena::getMessageSvc(), "MuGM::SpacerBeam");
        log << MSG::INFO << " SpacerBeam " << name << " :" << endmsg;
    }

} // namespace MuonGM
