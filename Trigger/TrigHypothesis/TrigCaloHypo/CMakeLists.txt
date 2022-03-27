# Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( TrigCaloHypo )

find_package( tdaq-common COMPONENTS hltinterface )


# Component(s) in the package:
atlas_add_library( TrigCaloHypoLib
                   TrigCaloHypo/*.h
                   INTERFACE
                   PUBLIC_HEADERS TrigCaloHypo
                   INCLUDE_DIRS ${TDAQ-COMMON_INCLUDE_DIRS}
                   LINK_LIBRARIES xAODTrigCalo CaloEvent DecisionHandlingLib GaudiKernel Identifier TrigSteeringEvent ${TDAQ-COMMON_LIBRARIES} )

atlas_add_component( TrigCaloHypo
                     src/*.cxx src/components/*.cxx
                     LINK_LIBRARIES AthenaBaseComps AthenaMonitoringKernelLib AthViews xAODTrigCalo CaloEvent CaloInterfaceLib DecisionHandlingLib GaudiKernel Identifier LArRecConditions LArRecEvent StoreGateLib TrigCaloHypoLib TrigCompositeUtilsLib TrigSteeringEvent )

# Install files from the package:
atlas_install_python_modules( python/*.py POST_BUILD_CMD ${ATLAS_FLAKE8} --extend-select=ATL900,ATL901 )

# Install IS schema files:
atlas_install_generic( schema/Larg.LArNoiseBurstCandidates.is.schema.xml
   DESTINATION share/schema
)
