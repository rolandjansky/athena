# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( TRT_ConditionsServices )

# Possible extra dependencies:
set( extra_lib )
if( NOT SIMULATIONBASE )
   set( extra_lib InDetByteStreamErrors )
endif()

# External dependencies:
find_package( ROOT COMPONENTS RIO Core Tree MathCore Hist pthread )

# Component(s) in the package:
atlas_add_library( TRT_ConditionsServicesLib
                   src/*.cxx
                   PUBLIC_HEADERS TRT_ConditionsServices
                   PRIVATE_INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
                   LINK_LIBRARIES LINK_LIBRARIES AthenaKernel AthenaPoolUtilities DetDescrConditions EventPrimitives GaudiKernel GeoPrimitives InDetConditionsSummaryService InDetIdentifier TRT_ConditionsData
                   PRIVATE_LINK_LIBRARIES ${ROOT_LIBRARIES} AthenaBaseComps CxxUtils GeoModelInterfaces GeoModelUtilities Identifier xAODEventInfo InDetReadoutGeometry TRT_ReadoutGeometry PathResolver RegistrationServicesLib RDBAccessSvcLib ${extra_lib} )

atlas_add_component( TRT_ConditionsServices
                     src/components/*.cxx
                     LINK_LIBRARIES TRT_ConditionsServicesLib )

# Install files from the package:
atlas_install_python_modules( python/*.py POST_BUILD_CMD ${ATLAS_FLAKE8} )
