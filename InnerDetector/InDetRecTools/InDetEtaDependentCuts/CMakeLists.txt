# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( InDetEtaDependentCuts )

# External dependencies:
find_package( ROOT COMPONENTS Core )
find_package( Boost COMPONENTS unit_test_framework )

# Component(s) in the package:
atlas_add_component( InDetEtaDependentCuts
                     src/*.cxx
                     src/components/*.cxx
                     LINK_LIBRARIES AthenaBaseComps GaudiKernel InDetRecToolInterfaces )

# Install files from the package:
atlas_install_joboptions( share/*.txt )

# Test(s) in the package:
atlas_add_test( InDetEtaDependentCutsSvc_test                
   SOURCES test/InDetEtaDependentCutsSvc_test.cxx 
   INCLUDE_DIRS ${Boost_INCLUDE_DIRS} ${ROOT_INCLUDE_DIRS}
   LINK_LIBRARIES ${Boost_LIBRARIES} ${ROOT_LIBRARIES} AthenaBaseComps CxxUtils GaudiKernel InDetRecToolInterfaces TestTools
   POST_EXEC_SCRIPT "nopost.sh" )

