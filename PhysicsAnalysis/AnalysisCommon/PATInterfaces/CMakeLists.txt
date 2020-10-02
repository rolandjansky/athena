# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( PATInterfaces )

# External dependencies:
find_package( Boost COMPONENTS regex )
find_package( ROOT COMPONENTS Core MathCore )
find_package( GTest )

# Generate a dictionary:
atlas_add_root_dictionary( PATInterfaces _dictionary
   ROOT_HEADERS PATInterfaces/SystematicVariation.h
   PATInterfaces/SystematicSet.h PATInterfaces/SystematicRegistry.h
   PATInterfaces/SystematicsUtil.h PATInterfaces/StreamTestWrapper.h
   PATInterfaces/CorrectionCode.h Root/LinkDef.h
   EXTERNAL_PACKAGES ROOT )

# Component(s) in the package:
atlas_add_library( PATInterfaces
   PATInterfaces/*.h PATInterfaces/*.icc Root/*.cxx ${_dictionary}
   PUBLIC_HEADERS PATInterfaces
   INCLUDE_DIRS ${ROOT_INCLUDE_DIRS} ${Boost_INCLUDE_DIRS}
   LINK_LIBRARIES ${ROOT_LIBRARIES} ${Boost_LIBRARIES} AsgTools
   PRIVATE_LINK_LIBRARIES RootCoreUtils )

# Helper macro for declaring the "simple" tests:
macro( _add_test name )
   atlas_add_test( ${name}
      SOURCES test/${name}.cxx
      LINK_LIBRARIES PATInterfaces RootCoreUtils )
endmacro( _add_test )

# Test(s) in the package:
atlas_add_test( gt_SystematicSet
   SOURCES test/gt_SystematicSet.cxx
   INCLUDE_DIRS ${GTEST_INCLUDE_DIRS}
   LINK_LIBRARIES ${GTEST_LIBRARIES} PATInterfaces RootCoreUtils )

_add_test( ut_SystematicSet_unordered_map )
_add_test( ut_SystematicVariation )
_add_test( ut_make_systematic_vector )
_add_test( ut_stream_systematics )

atlas_add_test( ut_syst_constructors
   SCRIPT test/ut_syst_constructors.py )

atlas_add_test( ut_syst_dict
   SCRIPT test/ut_syst_dict.py )

atlas_add_test( ut_syst_equality
   SCRIPT test/ut_syst_equality.py )

# Clean up:
unset( _add_test )
