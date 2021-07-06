# Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( TrigHypoCommonTools )

# Component(s) in the package:
atlas_add_component( TrigHypoCommonTools
                     src/*.cxx
                     src/components/*.cxx
                     LINK_LIBRARIES AsgTools AthLinks AthenaBaseComps AthenaMonitoringKernelLib DecisionHandlingLib GaudiKernel L1DecoderLib TrigCompositeUtilsLib TrigT1Result xAODBase )

# Install files from the package:
atlas_install_python_modules( python/*.py POST_BUILD_CMD ${ATLAS_FLAKE8} --extend-extensions=ATL900,ATL901 )
