/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#include "MuonGeoModel/FPVMAP.h"

#include "AthenaKernel/getMessageSvc.h"
#include "GaudiKernel/MsgStream.h"
#include "MuonGeoModel/Technology.h"

#include <GaudiKernel/IMessageSvc.h>
#include <iostream>
#include <utility>

namespace MuonGM {

    FPVMAP::FPVMAP() { m_nreused = 0; }

    GeoVPhysVol *FPVMAP::GetDetector(const std::string& name) {
        if (m_Detectors.find(name) != m_Detectors.end()) {
            m_nreused++;
            // std::cout<<"FPVMAP:: the pointer to "<<name
            //          <<" is already stored; saving memory "<<m_nreused<<std::endl;
            return m_Detectors[name];
        } else
            return nullptr;
    }

    void FPVMAP::StoreDetector(GeoVPhysVol *s, const std::string& name) {
        // std::cout<<"FPVMAP:: store the pointer to "<<name<<std::endl;
        m_Detectors[name] = s;
    }

    void FPVMAP::PrintAllDetectors() {
        MsgStream log(Athena::getMessageSvc(), "MuonGM::FPVMAP");

        for (const auto& p : m_Detectors) {
            log << MSG::INFO << "---> A PhysVol corresponds to  " << p.first << endmsg;
        }
    }

} // namespace MuonGM
