# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( AthenaPoolServices )

# External dependencies:
find_package( ROOT COMPONENTS Core )

# Component(s) in the package:
atlas_add_library( AthenaPoolServicesLib
   AthenaPoolServices/*.h
   INTERFACE
   PUBLIC_HEADERS AthenaPoolServices
   LINK_LIBRARIES GaudiKernel )

atlas_add_component( AthenaRootStreamerSvc
   src/*.h src/*.cxx src/components/*.cxx
   INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
   LINK_LIBRARIES ${ROOT_LIBRARIES} AthenaBaseComps AthenaPoolServicesLib DataModelRoot StorageSvc )

# Install files from the package:
atlas_install_joboptions( share/*.py )
