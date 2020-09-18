################################################################################
# Package: MuonIdHelpers
################################################################################

# Declare the package name:
atlas_subdir( MuonIdHelpers )

# Decide on the sources/libraries to use based on the build environment:
if( NOT XAOD_STANDALONE )
   set( extra_sources src/*.cxx )
   set( extra_libraries
      LINK_LIBRARIES AthenaKernel AthenaBaseComps AtlasDetDescr IdDict Identifier MuonStationIndexLib
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
      LINK_LIBRARIES MuonIdHelpersLib MuonStationIndexLib )

   atlas_add_dictionary( MuonIdHelpersDict
      MuonIdHelpers/MuonIdHelpersDict.h
      MuonIdHelpers/selection.xml
      LINK_LIBRARIES MuonIdHelpersLib MuonStationIndexLib )

   atlas_add_test( muon_id_test
                SOURCES
                test/muon_id_test.cxx
                LINK_LIBRARIES Identifier IdDictParser GaudiKernel MuonIdHelpersLib MuonStationIndexLib )
endif()
