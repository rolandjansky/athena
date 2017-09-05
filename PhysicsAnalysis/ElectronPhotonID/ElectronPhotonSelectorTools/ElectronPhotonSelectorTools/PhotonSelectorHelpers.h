/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef __PHOTONSELECTORHELPERS__
#define __PHOTONSELECTORHELPERS__

#pragma message "This code  is deprecated, consider moving to EgammaAnalysisHelpers/PhotonHelpers.h instead"

// Atlas includes
#include "xAODEgamma/Photon.h"
#include <cstddef>

namespace PhotonSelectorHelpers{
  bool passOQquality(const xAOD::Photon *ph){
    static const asg::AsgMessaging msg("PhotonSelectorHelpers");
    msg.msg(MSG::WARNING) << "This function is  deprecated, move to EgammaAnalysisHelpers/PhotonHelpers.h " << endmsg;
    return 0;
  }

  bool passOQqualitydelayed(const xAOD::Photon *ph){
    static const asg::AsgMessaging msg("PhotonSelectorHelpers");
    msg.msg(MSG::WARNING) << "This function is  deprecated, moveto EgammaAnalysisHelpers/PhotonHelpers.h instead" << endmsg;
    return 0; 
  }
}

#endif
