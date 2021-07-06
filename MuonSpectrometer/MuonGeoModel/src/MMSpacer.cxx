/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#include "MuonGeoModel/MMSpacer.h"

#include "AthenaKernel/getMessageSvc.h"
#include "GeoModelInterfaces/StoredMaterialManager.h"
#include "GeoModelKernel/GeoDefinitions.h"
#include "GeoModelKernel/GeoLogVol.h"
#include "GeoModelKernel/GeoPhysVol.h"
#include "GeoModelKernel/GeoShapeShift.h"
#include "GeoModelKernel/GeoShapeSubtraction.h"
#include "GeoModelKernel/GeoTrd.h"
#include "MuonGeoModel/Component.h"
#include "MuonGeoModel/MMSpacerComponent.h"
#include "MuonGeoModel/MMSpacer_Technology.h"
#include "MuonGeoModel/MYSQL.h"

#include <GaudiKernel/IMessageSvc.h>
#include <GaudiKernel/MsgStream.h>
#include <GeoModelKernel/GeoShape.h>
#include <string>

class GeoMaterial;

namespace MuonGM {

    MMSpacer::MMSpacer(Component *ss) : DetectorElement(ss->name) {
        MMSpacerComponent *s = (MMSpacerComponent *)ss;
        m_component = s;
        width = s->dx1;
        longWidth = s->dx2;
        length = s->dy;
        name = s->name;
        thickness = s->GetThickness();
        index = s->index;
    }

    GeoPhysVol *MMSpacer::build(int minimalgeo) {
        std::vector<Cutout *> vcutdef;
        int cutoutson = 0;
        return build(minimalgeo, cutoutson, vcutdef);
    }

    GeoPhysVol *MMSpacer::build(int minimalgeo, int, std::vector<Cutout *>) {
        MYSQL *mysql = MYSQL::GetPointer();

        MMSpacer_Technology *t = (MMSpacer_Technology *)mysql->GetTechnology(name);
        thickness = t->Thickness();

        // Build Micromegas mother volume out of G10
        // std::cout<<"creating spacer "<<thickness<<" "<<width<<" "<<longWidth<<" "<<length<<std::endl;
        const GeoShape *strd = new GeoTrd(thickness / 2, thickness / 2, width / 2, longWidth / 2, length / 2);

        if (t->lowZCutOuts) {
            double dy = t->lowZCutOutWidth;
            double dz = t->lowZCutOutDZ;
            const GeoShape *sbox = new GeoTrd(thickness, thickness, dy, dy, dz);
            GeoTrf::Translate3D cut1(0.0, -width / 2., -length / 2.);
            GeoTrf::Translate3D cut2(0.0, width / 2., -length / 2.);
            strd = &(strd->subtract((*sbox) << cut1));
            strd = &(strd->subtract((*sbox) << cut2));
        }

        if (t->highZCutOuts) {
            double dy = t->highZCutOutWidth;
            double dz = t->highZCutOutDZ;
            const GeoShape *sbox = new GeoTrd(thickness, thickness, dy, dy, dz);
            GeoTrf::Translate3D cut1(0.0, -longWidth / 2., length / 2.);
            GeoTrf::Translate3D cut2(0.0, longWidth / 2., length / 2.);
            strd = &(strd->subtract((*sbox) << cut1));
            strd = &(strd->subtract((*sbox) << cut2));
        }

        const GeoMaterial *mtrd = getMaterialManager()->getMaterial("muo::Honeycomb");
        GeoLogVol *ltrd = new GeoLogVol(logVolName, strd, mtrd);
        GeoPhysVol *ptrd = new GeoPhysVol(ltrd);

        if (minimalgeo == 1)
            return ptrd;

        return ptrd;
    }

    void MMSpacer::print() {
        MsgStream log(Athena::getMessageSvc(), "MuonGM::MMSpacer");
        log << MSG::INFO << " MMSpacer " << name << " :" << endmsg;
    }

} // namespace MuonGM
