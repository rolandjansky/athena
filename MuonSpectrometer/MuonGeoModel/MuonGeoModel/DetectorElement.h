/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#ifndef DetectorElement_H
#define DetectorElement_H

#include "AthenaKernel/getMessageSvc.h"
#include "GaudiKernel/MsgStream.h"
#include "GeoModelInterfaces/StoredMaterialManager.h"

#include <GaudiKernel/IMessageSvc.h>
#include <string>

namespace MuonGM {

    class DetectorElement {
      public:
        std::string name;
        std::string logVolName;

        DetectorElement(const std::string& n) : name(n) {  }

        void setLogVolName(const std::string& str) { logVolName = str; }

        virtual void print() {
            MsgStream log(Athena::getMessageSvc(), "MuonGM::DetectorElement");
            log << MSG::INFO << "generic detector element" << endmsg;
        }

        virtual ~DetectorElement() = default;

    }; // class DetectorElement

} // namespace MuonGM

#endif
