/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#include "MuonGeoModel/Ded.h"

#include "GeoModelKernel/GeoCutVolAction.h"
#include "GeoModelKernel/GeoDefinitions.h"
#include "GeoModelKernel/GeoLogVol.h"
#include "GeoModelKernel/GeoMaterial.h"
#include "GeoModelKernel/GeoPhysVol.h"
#include "GeoModelKernel/GeoShapeIntersection.h"
#include "GeoModelKernel/GeoShapeSubtraction.h"
#include "GeoModelKernel/GeoTrd.h"
#include "GeoModelKernel/GeoVPhysVol.h"
#include "MuonGeoModel/Cutout.h"
#include "MuonGeoModel/DED_Technology.h"
#include "MuonGeoModel/MYSQL.h"

#define skip_ded false

namespace MuonGM {

    Ded::Ded(Component *ss) : DetectorElement(ss->name) {
        double tol = 1.e-4;
        DedComponent *s = (DedComponent *)ss;
        width = s->dx1;
        longWidth = s->dx2;
        thickness = s->GetThickness() - tol;
        length = s->dy - tol;
        m_component = s;
    }

    GeoVPhysVol *Ded::build() {
        std::vector<Cutout *> vcutdef;
        int cutoutson = 0;
        return build(cutoutson, vcutdef);
    }

    GeoVPhysVol *Ded::build(int cutoutson, std::vector<Cutout *> vcutdef) {
        MYSQL *mysql = MYSQL::GetPointer();
        DED *r = (DED *)mysql->GetTechnology(name);

        double tckaluminum = r->AlThickness;
        double honthickness = r->HoneyCombThickness;
        const GeoShape *sded = new GeoTrd(thickness / 2, thickness / 2, width / 2, longWidth / 2, length / 2);
        const GeoShape *shon = new GeoTrd(honthickness / 2, honthickness / 2, width / 2. - tckaluminum, longWidth / 2. - tckaluminum, length / 2. - tckaluminum);

        const GeoMaterial *mded = getMaterialManager()->getMaterial("std::Aluminium");
        GeoLogVol *lded = new GeoLogVol("DedModuleSkin", sded, mded);
        GeoPhysVol *pded = new GeoPhysVol(lded);

        // Place honeycomb inside aluminum to make aluminum skin
        const GeoMaterial *mhon = getMaterialManager()->getMaterial("muo::RpcPapHonC");
        GeoLogVol *lhon = new GeoLogVol("DedModulehoneycomb", shon, mhon);
        GeoPhysVol *phon = new GeoPhysVol(lhon);
        if (!skip_ded)
            pded->add(phon);

        // Apply cutouts
        if (cutoutson && vcutdef.size() > 0) {
            GeoPhysVol *tempPhys = 0;
            Cutout *cut = 0;
            GeoShape *cutoutShape = 0;
            GeoTrf::Transform3D cutTrans{GeoTrf::Transform3D::Identity()};
            for (unsigned i = 0; i < vcutdef.size(); i++) {
                cut = vcutdef[i];
                cutoutShape = new GeoTrd(thickness / 2. + 1., thickness / 2. + 1., cut->widthXs / 2., cut->widthXl / 2., cut->lengthY / 2.);
                cutTrans = GeoTrf::Translate3D(0.0, cut->dx, -length / 2 + cut->dy + cut->lengthY / 2.);

                GeoCutVolAction cutAction(*cutoutShape, cutTrans);
                pded->apply(&cutAction);
                tempPhys = cutAction.getPV();
                pded->ref();
                pded->unref();
                pded = tempPhys;
            }
        }

        return pded;
    }

    void Ded::print() {
        MsgStream log(Athena::getMessageSvc(), "MuonGM::Ded");
        log << MSG::INFO << "Ded " << name.c_str() << " :" << endmsg;
    }

} // namespace MuonGM
