# Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( TrigConfMuctpi )

# External dependencies:
find_package( Boost )

# Component(s) in the package:
atlas_add_library( TrigConfMuctpi
                   src/*.cxx
                   PUBLIC_HEADERS TrigConfMuctpi
                   INCLUDE_DIRS ${Boost_INCLUDE_DIRS}
                   LINK_LIBRARIES ${Boost_LIBRARIES} TrigConfBase )

atlas_add_executable( TrigConfReadWriteMuCTPiGeometry
                      src/test/ReadWriteMuCTPiGeometry.cxx
                      LINK_LIBRARIES TrigConfMuctpi )

# Install files from the package:
atlas_install_python_modules( python/*.py POST_BUILD_CMD ${ATLAS_FLAKE8} )
atlas_install_scripts( scripts/*.py POST_BUILD_CMD ${ATLAS_FLAKE8} )
atlas_install_xmls( data/*.dtd )


