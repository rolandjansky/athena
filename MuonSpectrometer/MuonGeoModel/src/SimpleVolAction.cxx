/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

/***************************************************************************
 a GeoVolumeAction
 -----------------------------------------
 ***************************************************************************/

#include "MuonGeoModel/SimpleVolAction.h"

#include "AthenaKernel/getMessageSvc.h"
#include "GaudiKernel/MsgStream.h"

#include <GaudiKernel/IMessageSvc.h>
#include <GeoModelKernel/GeoLogVol.h>
#include <GeoModelKernel/GeoMaterial.h>
#include <GeoModelKernel/GeoVPhysVol.h>
#include <GeoModelKernel/GeoVolumeAction.h>
#include <string>

namespace MuonGM {

    SimpleVolAction::SimpleVolAction() : GeoVolumeAction(GeoVolumeAction::TOP_DOWN) {}

    SimpleVolAction::~SimpleVolAction() {}

    void SimpleVolAction::handleVPhysVol(const GeoVPhysVol *pv) {
        MsgStream log(Athena::getMessageSvc(), "MuGM::SimpleVolAction");

        //   std::cout<<" MY VOL Action is handlying a GeoVPhysVol "<<std::endl;
        const GeoLogVol *lv = pv->getLogVol();
        std::string name = lv->getName();
        const GeoMaterial *mat = lv->getMaterial();
        unsigned int nchild = pv->getNChildVols();

        log << MSG::INFO << " *** Tree Navigation *** in " << name << " made of " << mat->getName() << " N.of Children " << nchild << endmsg;

        for (unsigned int i = 0; i < nchild; ++i) {
            log << MSG::INFO << " ... child n. " << i << " named " << pv->getNameOfChildVol(i) << endmsg;
        }
    }

} // namespace MuonGM
