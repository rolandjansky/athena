# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( CLIDComps )

# External dependencies:
find_package( Boost COMPONENTS program_options )

# Component(s) in the package:
atlas_add_component( CLIDComps
   src/*.h src/*.cxx src/components/*.cxx
   NOCLIDDB
   INCLUDE_DIRS ${Boost_INCLUDE_DIRS}
   LINK_LIBRARIES ${Boost_LIBRARIES} AthenaKernel CxxUtils GaudiKernel )

# Executable(s) in the package:
atlas_add_executable( genCLIDDB
   util/genCLIDDB.cxx
   INCLUDE_DIRS ${Boost_INCLUDE_DIRS}
   LINK_LIBRARIES ${Boost_LIBRARIES} GaudiKernel AthenaKernel )
if( TARGET genCLIDDB AND TARGET CLIDComps )
   add_dependencies( genCLIDDB CLIDCompsComponentsList )
endif()

# Test(s) in the package:
atlas_add_test( ClassIDSvc_test
   SOURCES test/ClassIDSvc_test.cxx
   LINK_LIBRARIES AthenaKernel GaudiKernel TestTools
   ENVIRONMENT "JOBOPTSEARCHPATH=${CMAKE_CURRENT_SOURCE_DIR}/share" )

atlas_add_test( clid
                SCRIPT test/_clid_unittest.py
                LOG_IGNORE_PATTERN "Ran 3 tests in" )

# Install files from the package:
atlas_install_python_modules( python/*.py POST_BUILD_CMD ${ATLAS_FLAKE8} )
atlas_install_runtime( share/Gaudi_clid.db )
atlas_install_scripts( share/clid test/_clid_unittest.py )
atlas_install_joboptions( share/*.opts )

# Wait with the build of genCLIDDB until the jobOptions are installed:
if( TARGET genCLIDDB AND TARGET CLIDCompsJobOptInstall )
   add_dependencies( genCLIDDB CLIDCompsJobOptInstall )
endif()
