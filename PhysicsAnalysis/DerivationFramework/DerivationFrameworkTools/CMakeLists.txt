# Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
add_definitions(-g -O2)

# Declare the package name:
atlas_subdir( DerivationFrameworkTools )

# Component(s) in the package:
atlas_add_component( DerivationFrameworkTools
                     src/*.cxx src/components/*.cxx
                     LINK_LIBRARIES AthenaBaseComps DerivationFrameworkInterfaces ExpressionEvaluationLib GaudiKernel PATCoreLib StoreGateLib TrigDecisionToolLib xAODBase xAODEventInfo )
atlas_install_python_modules( python/*.py )
