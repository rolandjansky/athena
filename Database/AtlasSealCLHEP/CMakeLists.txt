# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( AtlasSealCLHEP )

# External dependencies:
find_package( CLHEP )

# Component(s) in the package:
atlas_add_library( AtlasSealCLHEP
                   src/OldCLHEPStreamers.cxx
                   PUBLIC_HEADERS AtlasSealCLHEP
                   INCLUDE_DIRS ${CLHEP_INCLUDE_DIRS}
                   DEFINITIONS ${CLHEP_DEFINITIONS}
                   LINK_LIBRARIES ${CLHEP_LIBRARIES} AthenaPoolServicesLib )

atlas_add_dictionary( AtlasSealCLHEPDict
                      AtlasSealCLHEP/AtlasSealCLHEPDict.h
                      AtlasSealCLHEP/selection.xml
                      LINK_LIBRARIES AtlasSealCLHEP )

# Install files from the package:
atlas_install_python_modules( python/*.py )
atlas_install_joboptions( share/*.py )

atlas_add_test( OldCLHEPStreamers_test
                SOURCES
                test/OldCLHEPStreamers_test.cxx
                LINK_LIBRARIES AtlasSealCLHEP )
