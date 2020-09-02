# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( SubDetectorEnvelopes )

# External dependencies:
find_package( CLHEP )

# Component(s) in the package:
atlas_add_library( SubDetectorEnvelopesLib
                   INTERFACE
                   PUBLIC_HEADERS SubDetectorEnvelopes
                   LINK_LIBRARIES GaudiKernel AtlasDetDescr )

atlas_add_component( SubDetectorEnvelopes
                     src/*.cxx
                     src/components/*.cxx
                     INCLUDE_DIRS ${CLHEP_INCLUDE_DIRS}
                     LINK_LIBRARIES ${CLHEP_LIBRARIES} AthenaBaseComps GeoModelInterfaces RDBAccessSvcLib SubDetectorEnvelopesLib )

# Install files from the package:
atlas_install_python_modules( python/*.py POST_BUILD_CMD ${ATLAS_FLAKE8} )
