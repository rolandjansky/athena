/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#ifndef ATHENAKERNEL_TOOLS_ATHENA_PACKAGEINFO_H
#define ATHENAKERNEL_TOOLS_ATHENA_PACKAGEINFO_H 1

#include <string>

namespace Athena {

/**
 * @class PackageInfo
 * @author Paolo Calafiura
 * @brief decodes cmt PACKAGE_VERSION macro (deprecated)
 *
 * As of April 2021 this is a dummy class as the package information is no
 * longer stored in the CLID database (ATEAM-731). It can be deleted once
 * the IClassIDSvc interface has been cleaned up.
 */
  class PackageInfo {
  public:
    PackageInfo(const std::string&) {}
  };
}

#endif
