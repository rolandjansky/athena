################################################################################
# Package: TrigConfBase
################################################################################

# Declare the package name:
atlas_subdir( TrigConfBase )

# External dependencies:
find_package( Boost COMPONENTS regex thread )

# Component(s) in the package:
atlas_add_library( TrigConfBase
   TrigConfBase/*.h src/*.cxx Root/*.cxx
   PUBLIC_HEADERS TrigConfBase
   INCLUDE_DIRS ${Boost_INCLUDE_DIRS}
   LINK_LIBRARIES ${Boost_LIBRARIES} )

atlas_install_joboptions( share/*.py )

# Tests:
atlas_add_component( TrigConfBaseTest
   test/TrigConfMsgAlg.cxx
   LINK_LIBRARIES TrigConfBase AthenaBaseComps )

atlas_add_test( trigconf_msg_standalone_test
   SOURCES test/trigconf_msgsvc_test.cxx
   LINK_LIBRARIES TrigConfBase )

atlas_add_test( trigconf_msg_athena_test
   SCRIPT athena.py TrigConfBase/test_TrigConfMsgAlg.py )
