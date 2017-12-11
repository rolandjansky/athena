///////////////////////// -*- C++ -*- /////////////////////////////

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// PayLoad.h 
// Header file for class SgTests::PayLoad
// Author: S.Binet<binet@cern.ch>
/////////////////////////////////////////////////////////////////// 
#ifndef STOREGATETESTS_PAYLOAD_H
#define STOREGATETESTS_PAYLOAD_H

/** @class SgTests::PayLoad
 *  is a very simple class to exercize the basic
 *  features of the @c StoreGateSvc class: publishing and retrieving data
 *  between various algorithm. The main goal is to monitor the performances of
 *  these key functionalities.
 */

// STL
#include <vector>

#include "AthContainers/DataVector.h"
#include "CLIDSvc/CLASS_DEF.h"

namespace SgTests {
  struct PayLoad {
    std::vector<int> m_data;
  };
}

namespace SgTests {
  struct PayLoadDv : DataVector<SgTests::PayLoad> {};
}

CLASS_DEF( SgTests::PayLoad,   115335033, 1 )
CLASS_DEF( SgTests::PayLoadDv,  54422553, 1 )

#endif // STOREGATETESTS_PAYLOAD_H
