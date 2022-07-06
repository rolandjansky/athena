// -*- C++ -*-

/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGJETMONITORING_JTMCONTAINERS_H
#define TRIGJETMONITORING_JTMCONTAINERS_H

#include "xAODTrigger/JetRoIContainer.h"
#include "xAODTrigger/gFexJetRoIContainer.h"
#include "xAODTrigger/jFexLRJetRoIContainer.h"
#include "xAODTrigger/jFexSRJetRoIContainer.h"

// The structs below allow provide a typedef to enable
// the naming of xAOD jet containers.
//
// the xAOD container header files  have typedefs which
// means that the classes produced by intantiating a template
// with an xAOD container with have version numbers in the class name.
//
// If the instantiated class is an AlgTool, the class nemae, containing
// a version number leaks into the python configurations code, which is
// not acceptable. The structuts below prevent this leakage from happening
//
// Code: C++ - see TrigL1FexJetMonitorTool_tpl
//             see also the typedefs in components/TrigJetMonitoring_entries.cxx
//    python - see L1JetMonitoringConfig.py


struct JTM_JetRoIContainer {
  typedef xAOD::JetRoIContainer JetContainer;
};


  
struct JTM_gFexJetRoIContainer {
  typedef xAOD::gFexJetRoIContainer JetContainer;
};

  
struct JTM_jFexLRJetRoIContainer {
  typedef xAOD::jFexLRJetRoIContainer JetContainer;
};

  
struct JTM_jFexSRJetRoIContainer {
  typedef xAOD::jFexSRJetRoIContainer JetContainer;
};


#endif
