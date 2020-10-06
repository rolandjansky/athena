# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( RegistrationServices )

# External dependencies:
find_package( COOL COMPONENTS CoolKernel )
find_package( CORAL COMPONENTS CoralBase )

atlas_add_library( RegistrationServicesLib
                   INTERFACE
                   PUBLIC_HEADERS RegistrationServices
                   LINK_LIBRARIES GaudiKernel )

# Component(s) in the package:
atlas_add_component( RegistrationServices
                     src/RegistrationStream.cxx
                     src/RegistrationStreamLCGTool.cxx
                     src/RegistrationStreamTagTool.cxx
                     src/IOVRegistrationSvc.cxx
                     src/OutputConditionsAlg.cxx
                     src/components/*.cxx
                     INCLUDE_DIRS ${COOL_INCLUDE_DIRS} ${CORAL_INCLUDE_DIRS}
                     LINK_LIBRARIES ${COOL_LIBRARIES} ${CORAL_LIBRARIES} AthenaBaseComps AthenaKernel AthenaPoolUtilities CollectionBase DBDataModel FileCatalog IOVDbSvcLib PersistentDataModel PoolSvcLib RegistrationServicesLib SGTools StoreGateLib )

# Install files from the package:
atlas_install_python_modules( python/*.py )
atlas_install_joboptions( share/*.py )

