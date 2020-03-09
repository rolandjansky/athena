/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGCONFINTERFACES_IJOBOPTIONSSVC_H
#define TRIGCONFINTERFACES_IJOBOPTIONSSVC_H

#include "GaudiKernel/IInterface.h"
#include <string>

namespace TrigConf {

  /**
   * Trigger-specific JobOptionsSvc interface
   */
  class IJobOptionsSvc : virtual public IInterface {
  public:
    DeclareInterfaceID(TrigConf::IJobOptionsSvc, 1, 0);

    virtual const std::string& server() const = 0; ///< DB connection alias
    virtual int superMasterKey() const = 0;        ///< Return SMK (-1 if not set)
    virtual int l1PrescaleKey() const = 0;         ///< Return L1PSK (-1 if not set)
    virtual int hltPrescaleKey() const = 0;        ///< Return HLTPSK (-1 if not set)
  };

} // namespace TrigConf

#endif
