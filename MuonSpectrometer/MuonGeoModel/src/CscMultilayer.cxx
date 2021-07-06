/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#include "MuonGeoModel/CscMultiLayer.h"

#include "AthenaKernel/getMessageSvc.h"
#include "GeoModelInterfaces/StoredMaterialManager.h"
#include "GeoModelKernel/GeoDefinitions.h"
#include "GeoModelKernel/GeoIdentifierTag.h"
#include "GeoModelKernel/GeoNameTag.h"
#include "GeoModelKernel/GeoPhysVol.h"
#include "GeoModelKernel/GeoShapeShift.h"
#include "GeoModelKernel/GeoShapeUnion.h"
#include "GeoModelKernel/GeoTransform.h"
#include "GeoModelKernel/GeoTrd.h"
#include "MuonGeoModel/CSC_Technology.h"
#include "MuonGeoModel/DetectorElement.h"
#include "MuonGeoModel/MYSQL.h"

#include <GaudiKernel/IMessageSvc.h>
#include <GaudiKernel/MsgStream.h>
#include <GeoModelKernel/GeoLogVol.h>
#include <GeoModelKernel/GeoShape.h>
#include <math.h>
#include <stddef.h>
#include <string>
#include <vector>

class GeoMaterial;
class GeoVPhysVol;
namespace MuonGM {
    class Cutout;
}

namespace MuonGM {

    CscMultiLayer::CscMultiLayer(std::string n) : DetectorElement(n), width(0.), longWidth(0.), upWidth(0.), excent(0.), length(0.), physicalLength(0.), maxwLength(0.) {
        MYSQL *amdb = MYSQL::GetPointer();
        CSC *md = (CSC *)amdb->GetTechnology(name);
        nrOfLayers = md->numOfLayers;
        cscthickness = md->totalThickness;
        thickness = cscthickness;
    }

    GeoVPhysVol *CscMultiLayer::build() {
        std::vector<Cutout *> vcutdef;
        int cutoutson = 0;
        return build(cutoutson, vcutdef);
    }

    GeoVPhysVol *CscMultiLayer::build(int /*cutoutson*/, std::vector<Cutout *> /*vcutdef*/) {
        MYSQL *amdb = MYSQL::GetPointer();
        CSC *md = (CSC *)amdb->GetTechnology(name);

        // define the chamber volumes --- for CSS and CSL

        //-------------internal CSC layer structure-------------4 layers ------

        // The dimensions of the panel
        double hon_thick = md->honeycombthick;
        double gasThickness = 2.0 * md->anocathodist;
        double newpos = cscthickness / 2.;
        double g10thi = (2. * md->g10thick + md->honeycombthick);

        // define the volumes
        // g10, honeycomb, and the gas
        // gas mixture: 80% Ar + 20% CO2:
        // at 1 atmosphere, 20 deg. Celcius:
        // density Ar = 1.699 g cm-3
        // density CO2 = 1.902 g cm-3
        // density of mixture = 0.8 d1 + 0.2 d2 = 1.740 gcm-3

        // Create CSC layer envelope made of air
        const GeoShape *sml = new GeoTrd(thickness / 2., thickness / 2., width / 2., longWidth / 2., maxwLength / 2.);
        if (excent != length) {
            const GeoShape *smlt = new GeoTrd(thickness / 2., thickness / 2., longWidth / 2., upWidth / 2., (physicalLength - maxwLength) / 2.);
            sml = &((sml->add((*smlt) << GeoTrf::TranslateZ3D(physicalLength / 2.))) << GeoTrf::TranslateZ3D((maxwLength - physicalLength) / 2.));
        }

        const GeoMaterial *mhon = getMaterialManager()->getMaterial("muo::Honeycomb");
        const GeoLogVol *lml = new GeoLogVol("CscMultilayer", sml, mhon);
        GeoPhysVol *pml = new GeoPhysVol(lml);

        // Create G10 and honeycomb volumes
        const GeoShape *shon = new GeoTrd(hon_thick / 2., hon_thick / 2., width / 2., longWidth / 2., maxwLength / 2.);
        const GeoShape *sg10hon = new GeoTrd(g10thi / 2., g10thi / 2., width / 2., longWidth / 2., maxwLength / 2.);
        if (excent != length) {
            const GeoShape *shont = new GeoTrd(hon_thick / 2., hon_thick / 2., longWidth / 2., upWidth / 2., (physicalLength - maxwLength) / 2.);
            shon = &((shon->add((*shont) << GeoTrf::TranslateZ3D(physicalLength / 2.))) << GeoTrf::TranslateZ3D((maxwLength - physicalLength) / 2.));
            const GeoShape *sg10hont = new GeoTrd(g10thi / 2., g10thi / 2., longWidth / 2., upWidth / 2., (physicalLength - maxwLength) / 2.);
            sg10hon = &((sg10hon->add((*sg10hont) << GeoTrf::TranslateZ3D(physicalLength / 2.))) << GeoTrf::TranslateZ3D((maxwLength - physicalLength) / 2.));
        }

        const GeoLogVol *lhon = new GeoLogVol("Honeycomb", shon, mhon);
        GeoPhysVol *phon = new GeoPhysVol(lhon);

        const GeoMaterial *mg10 = getMaterialManager()->getMaterial("std::G10");
        const GeoLogVol *lg10hon = new GeoLogVol("G10", sg10hon, mg10);
        GeoPhysVol *pg10hon = new GeoPhysVol(lg10hon);
        // Put honeycomb inside G10
        pg10hon->add(phon);

        // Create gas volume
        const GeoShape *sgas = NULL;
        double beta = atan((longWidth - width) / (2. * maxwLength));
        double gShortWidth = width - 2 * md->fullwirefixbarwidth * (1 - sin(beta)) / cos(beta);
        double gLongWidth = longWidth - 2 * md->fullwirefixbarwidth * (1 + sin(beta)) / cos(beta);
        if (excent == length) {
            double gLength = maxwLength - 2 * md->fullwirefixbarwidth;
            sgas = new GeoTrd(gasThickness / 2., gasThickness / 2., gShortWidth / 2., gLongWidth / 2., gLength / 2.);
        } else {
            double alpha = atan((excent - maxwLength) / (longWidth / 2.));
            double gmaxwLength = maxwLength - md->fullwirefixbarwidth;
            double gLength = length - 2. * md->fullwirefixbarwidth;
            double gupWidth = upWidth - 2 * md->fullwirefixbarwidth * (1 - cos(alpha)) / sin(alpha);

            sgas = new GeoTrd(gasThickness / 2., gasThickness / 2., gShortWidth / 2., gLongWidth / 2., gmaxwLength / 2.);
            const GeoShape *sgast = new GeoTrd(gasThickness / 2., gasThickness / 2., gLongWidth / 2., gupWidth / 2., (gLength - gmaxwLength) / 2.);
            sgas = &((sgas->add((*sgast) << GeoTrf::TranslateZ3D(gLength / 2.))) << GeoTrf::TranslateZ3D((gmaxwLength - gLength) / 2.));
        }
        const GeoMaterial *mgas = getMaterialManager()->getMaterial("muo::CscArCO2");
        const GeoLogVol *lgas = nullptr;

        // Place G10/honeycomb and gas volumes in CSC envelop, starting at top
        //   G10 - gas - G10 - gas - G10 - gas - G10 - gas - G10
        for (int i = 0; i < nrOfLayers + 1; i++) {
            GeoNameTag *np = new GeoNameTag("panel");
            GeoTransform *xp = new GeoTransform(GeoTrf::TranslateX3D(newpos - g10thi / 2.));
            pml->add(new GeoIdentifierTag(i));
            pml->add(np);
            pml->add(xp);
            pml->add(pg10hon);
            newpos -= g10thi;

            if (i < nrOfLayers) {
                lgas = new GeoLogVol("CscArCO2", sgas, mgas);
                GeoPhysVol *pgas = new GeoPhysVol(lgas);
                // the gas Gap
                // correct the position within the chamber
                GeoNameTag *ng = new GeoNameTag("CscArCO2");
                GeoTransform *xg = new GeoTransform(GeoTrf::TranslateX3D(newpos - gasThickness / 2.));
                pml->add(new GeoIdentifierTag(i));
                pml->add(ng);
                pml->add(xg);
                pml->add(pgas);
                newpos -= gasThickness;
            }
        }

        return pml;
    }

    void CscMultiLayer::print() {
        MsgStream log(Athena::getMessageSvc(), "MuonGM::CscMultiLayer");
        log << MSG::INFO << "CscMulti Layer:: CscMulti Layer " << name << " :" << endmsg;
    }

} // namespace MuonGM
