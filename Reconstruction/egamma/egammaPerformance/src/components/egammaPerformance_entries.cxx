/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/


// New Run 3 algorithms
#include "egammaPerformance/MonitorElectronAlgorithm.h"
#include "egammaPerformance/MonitorPhotonAlgorithm.h"
#include "egammaPerformance/MonitorFwdElectronAlgorithm.h"
#include "egammaPerformance/MonitorTnPAlgorithm.h"
// Run 2 algorithms
#include "egammaPerformance/photonMonTool.h"
#include "egammaPerformance/electronMonTool.h"
#include "egammaPerformance/forwardElectronMonTool.h"
#include "egammaPerformance/ZeeTaPMonTool.h"

// Run 2 Algorithms

DECLARE_COMPONENT( photonMonTool )
DECLARE_COMPONENT( electronMonTool )
DECLARE_COMPONENT( forwardElectronMonTool )
DECLARE_COMPONENT( ZeeTaPMonTool )

// RUN 3 algorithms

DECLARE_COMPONENT( MonitorElectronAlgorithm )
DECLARE_COMPONENT( MonitorPhotonAlgorithm )
DECLARE_COMPONENT( MonitorFwdElectronAlgorithm )
DECLARE_COMPONENT( MonitorTnPAlgorithm )
