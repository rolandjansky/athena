/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

/**
 * @file   AFPProtonAuxContainer_v1.cxx
 * @author Krzysztof Cieśla <krzysztof.marcin.ciesla@cern.ch>
 * @date   2018-04-05
 * 
 * @brief  Implementation file for class AFPProtonAuxContainer_v1
 * 
 */


// Local include(s):
#include <xAODForward/versions/AFPProtonAuxContainer_v1.h>

namespace xAOD {

  AFPProtonAuxContainer_v1::AFPProtonAuxContainer_v1() : AuxContainerBase() {
    AUX_VARIABLE(px);
    AUX_VARIABLE(py);
    AUX_VARIABLE(pz);
    AUX_VARIABLE(e);
    AUX_VARIABLE(side);
    AUX_VARIABLE(chi2);
    AUX_VARIABLE(methodID);
    AUX_VARIABLE(afpTrackLinks);
  }

} // namespace xAOD
