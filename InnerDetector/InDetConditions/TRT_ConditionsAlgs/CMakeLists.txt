# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( TRT_ConditionsAlgs )

# External dependencies:
find_package( CORAL COMPONENTS CoralBase CoralKernel RelationalAccess )
find_package( COOL COMPONENTS CoolKernel CoolApplication )

# Component(s) in the package:
atlas_add_component( TRT_ConditionsAlgs
                     src/*.cxx
                     src/components/*.cxx
<<<<<<< HEAD
                     INCLUDE_DIRS ${CORAL_INCLUDE_DIRS} ${COOL_INCLUDE_DIRS}
                     LINK_LIBRARIES LINK_LIBRARIES ${CORAL_LIBRARIES} ${COOL_LIBRARIES} AthenaBaseComps AthenaKernel AthenaPoolUtilities DetDescrConditions GaudiKernel GeoModelUtilities GeoPrimitives InDetIdentifier StoreGateLib TRT_ConditionsData TRT_ConditionsServicesLib TRT_ReadoutGeometry )
=======
                     INCLUDE_DIRS ${CORAL_INCLUDE_DIRS} ${COOL_INCLUDE_DIRS}
                     LINK_LIBRARIES ${COOL_LIBRARIES} AthenaBaseComps GaudiKernel TRT_ConditionsServicesLib TRT_ConditionsData InDetIdentifier
                                               InDetReadoutGeometry TRT_ReadoutGeometry AthenaPoolUtilities DetDescrConditions GeoModelUtilities )

>>>>>>> Added functionality to access NN configuration

# Install files from the package:
atlas_install_joboptions( share/*.py )
atlas_install_runtime( share/*.txt share/*.py )

