################################################################################
# Package: xAODEventFormat
################################################################################

# Declare the package name:
atlas_subdir( xAODEventFormat )

# Declare the package's dependencies:
atlas_depends_on_subdirs(
   PUBLIC
   Event/xAOD/xAODCore )

# The main library:
atlas_add_library( xAODEventFormat
   xAODEventFormat/*.h xAODEventFormat/versions/*.h Root/*.cxx
   PUBLIC_HEADERS xAODEventFormat
   LINK_LIBRARIES xAODCore )

# The dictionary:
atlas_add_dictionary( xAODEventFormatDict
   xAODEventFormat/xAODEventFormatDict.h
   xAODEventFormat/selection.xml
   LINK_LIBRARIES xAODEventFormat )

# The unit test:
atlas_add_test( ut_xaodeventformat_eventformat_test
   SOURCES test/ut_xaodeventformat_eventformat_test.cxx
   LINK_LIBRARIES xAODEventFormat )
