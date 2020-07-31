# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( TrigRootAnalysis )

# External dependencies:
find_package( ROOT COMPONENTS RIO Cint Core Tree MathCore Hist pthread )

# Component(s) in the package:
atlas_add_root_dictionary( TrigRootAnalysis
                           TrigRootAnalysisDictSource
                           ROOT_HEADERS TrigRootAnalysis/IDataAccess.h TrigRootAnalysis/IConfigAccess.h TrigRootAnalysis/IITrigConfigSvcD3PD.h TrigRootAnalysis/IITrigDecisionToolD3PD.h TrigRootAnalysis/TrigConfigSvcD3PD.h TrigRootAnalysis/DataAccess.h TrigRootAnalysis/ConfigAccess.h TrigRootAnalysis/ChainGroupHandling.h TrigRootAnalysis/TrigDecisionFunctions.h TrigRootAnalysis/TrigDecisionToolD3PD.h TrigRootAnalysis/PyTrigDecisionToolD3PD.h TrigRootAnalysis/ChainGroup.h Root/LinkDef.h
                           EXTERNAL_PACKAGES ROOT )

atlas_add_library( TrigRootAnalysis
                   Root/*.cxx
                   src/*.cxx
                   ${TrigRootAnalysisDictSource}
                   PUBLIC_HEADERS TrigRootAnalysis
                   INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
                   LINK_LIBRARIES ${ROOT_LIBRARIES} GaudiKernel
                   PRIVATE_LINK_LIBRARIES AthenaBaseComps AthenaKernel StoreGateLib )
