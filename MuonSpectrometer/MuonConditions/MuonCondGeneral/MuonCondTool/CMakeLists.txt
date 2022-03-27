# Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( MuonCondTool )

# External dependencies:
find_package( CORAL COMPONENTS CoralBase )

# Component(s) in the package:
atlas_add_component( MuonCondTool
                     src/*.cxx
                     src/components/*.cxx
                     INCLUDE_DIRS ${CORAL_INCLUDE_DIRS}
                     LINK_LIBRARIES ${CORAL_LIBRARIES} AthenaBaseComps AthenaPoolUtilities GaudiKernel GeoModelInterfaces Identifier MuonCondInterface MuonCondSvcLib MuonIdHelpersLib PathResolver RDBAccessSvcLib SGTools )

# Install files from the package:
atlas_install_joboptions( share/*.py )

