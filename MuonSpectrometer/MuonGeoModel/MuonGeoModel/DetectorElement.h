/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef DetectorElement_H
#define DetectorElement_H

#include "CxxUtils/checker_macros.h"
#include "GeoModelInterfaces/AbsMaterialManager.h"
#include "GaudiKernel/MsgStream.h"
#include "AthenaKernel/getMessageSvc.h"

#include <iostream>
#include <mutex>
#include <string>
#include <vector>

namespace MuonGM {

  class DetectorElement {
  public:
    std::string name;
    std::string logVolName;

    DetectorElement(std::string n): name(n)
    {
      logVolName = "";
    }

    void setLogVolName(std::string str)
    {
      logVolName = str;
    }

    virtual void print() {
      MsgStream log(Athena::getMessageSvc(), "MuonGM::DetectorElement");
      log << MSG::INFO << "generic detector element" << endmsg;
    }

    virtual ~DetectorElement() {}

    // Static
    static void setMaterialManager(const AbsMaterialManager & matMan) {
      std::lock_guard<std::mutex> lock(s_mutex);
      s_matManager = &matMan;
    }

    static const AbsMaterialManager * getMaterialManager() {
      std::lock_guard<std::mutex> lock(s_mutex);
      return s_matManager;
    }

  private:
    static const AbsMaterialManager *s_matManager ATLAS_THREAD_SAFE;
    static std::mutex s_mutex;

  }; // class DetectorElement

} // namespace MuonGM

#endif
