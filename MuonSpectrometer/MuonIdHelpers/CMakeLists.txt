# $Id: CMakeLists.txt 761516 2016-07-13 06:43:14Z krasznaa $
################################################################################
# Package: MuonIdHelpers
################################################################################

# Declare the package name:
atlas_subdir( MuonIdHelpers )

# Declare the package's dependencies. In standalone mode this package doesn't
# depend on anything.
if( NOT XAOD_STANDALONE )
   atlas_depends_on_subdirs(
      PUBLIC
      Control/AthenaBaseComps
      Control/CLIDSvc
      Control/SGTools
      DetectorDescription/AtlasDetDescr
      DetectorDescription/IdDict
      DetectorDescription/Identifier
      PRIVATE
      DetectorDescription/IdDictParser
      GaudiKernel )
endif()

# Decide on the sources/libraries to use based on the build environment:
if( NOT XAOD_STANDALONE )
   set( extra_sources src/*.cxx )
   set( extra_libraries
      LINK_LIBRARIES AthenaBaseComps SGTools AtlasDetDescr IdDict Identifier
      PRIVATE_LINK_LIBRARIES IdDictParser GaudiKernel )
endif()

# The main library of the package:
atlas_add_library( MuonIdHelpersLib
   MuonIdHelpers/*.h Root/*.cxx ${extra_sources}
   PUBLIC_HEADERS MuonIdHelpers
   ${extra_libraries} )

# These are only needed with Athena present:
if( NOT XAOD_STANDALONE )
   atlas_add_component( MuonIdHelpers
      src/components/*.cxx
      LINK_LIBRARIES MuonIdHelpersLib )

   atlas_add_dictionary( MuonIdHelpersDict
      MuonIdHelpers/MuonIdHelpersDict.h
      MuonIdHelpers/selection.xml
      LINK_LIBRARIES MuonIdHelpersLib )

   atlas_add_test( muon_id_test
                SOURCES
                test/muon_id_test.cxx
                LINK_LIBRARIES Identifier IdDictParser GaudiKernel MuonIdHelpersLib )
endif()
