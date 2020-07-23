# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( TrigT1Muctpi )

# External dependencies:
find_package( XercesC )

# Component(s) in the package:
atlas_add_component( TrigT1Muctpi
                     src/Algorithms/*.cxx
                     src/Common/*.cxx
                     src/Logging/*.cxx
                     src/Mibak/*.cxx
                     src/Mictp/*.cxx
                     src/Mioct/*.cxx
                     src/Mirod/*.cxx
                     src/components/*.cxx
                     INCLUDE_DIRS ${XERCESC_INCLUDE_DIRS}
                     LINK_LIBRARIES ${XERCESC_LIBRARIES} AnalysisTriggerEvent AthenaBaseComps AthenaKernel CxxUtils GaudiKernel PathResolver StoreGateLib TrigConfInterfaces TrigConfL1Data TrigConfMuctpi TrigT1Interfaces TrigT1Result )

# Install files from the package:
atlas_install_python_modules( python/*.py POST_BUILD_CMD ${ATLAS_FLAKE8} )
