/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef XAODTRIGL1CALO_JETELEMENT_H
#define XAODTRIGL1CALO_JETELEMENT_H

// Local include(s):
#include "xAODTrigL1Calo/versions/JetElement_v1.h"
#include "xAODTrigL1Calo/versions/JetElement_v2.h"
#include <map>

/// Namespace holding all the xAOD EDM classes
namespace xAOD {
   /// Define the latest version of the JetElement class
   typedef JetElement_v2 JetElement;

   typedef std::map<int, JetElement*> JetElementMap_t;
}

#include "xAODCore/CLASS_DEF.h"
CLASS_DEF( xAOD::JetElement , 165900946 , 1 )

#endif // XAODTRIGL1CALO_JETELEMENT_H
