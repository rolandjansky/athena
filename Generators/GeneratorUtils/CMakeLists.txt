# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( GeneratorUtils )

# Component(s) in the package:
atlas_add_library( GeneratorUtilsLib
                   GeneratorUtils/*.h
                   INTERFACE
                   PUBLIC_HEADERS GeneratorUtils
                   INCLUDE_DIRS ${Boost_INCLUDE_DIRS}
                   LINK_LIBRARIES ${Boost_LIBRARIES} )

# Test(s) in the package:
atlas_add_test( StringParse_test
                SOURCES
                test/StringParse_test.cxx
                LINK_LIBRARIES GeneratorUtilsLib )

# Install files from the package:
atlas_install_joboptions( share/*.py )
