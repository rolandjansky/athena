/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef ATHENAKERNEL_ATHDSOUTILS_H
#define ATHENAKERNEL_ATHDSOUTILS_H

#include "DataModelRoot/RootType.h"

namespace Athena {
  namespace DsoUtils {
    bool inDso( const RootDataMember& mem,
                const std::string& dsoname );
  }

}

#endif
