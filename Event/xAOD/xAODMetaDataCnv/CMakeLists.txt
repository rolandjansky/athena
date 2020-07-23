# $Id: CMakeLists.txt 744578 2016-05-03 16:26:20Z krasznaa $
################################################################################
# Package: xAODMetaDataCnv
################################################################################

# Declare the package name:
atlas_subdir( xAODMetaDataCnv )

# Extra dependencies, based on the environment that we're in:
if( NOT XAOD_STANDALONE )
   set( extra_deps
      GaudiKernel
      PRIVATE
      Control/AthenaKernel
      Database/AthenaPOOL/AthenaPoolUtilities
      Event/EventInfo )
endif()

# Declare the package's dependencies:
atlas_depends_on_subdirs(
   PUBLIC
   Control/AthToolSupport/AsgTools
   Event/xAOD/xAODMetaData
   ${extra_deps} )

# Build the package differently in standalone and Athena modes:
if( XAOD_STANDALONE )

   # The main library is an installed one in this case:
   atlas_add_library( xAODMetaDataCnv
      xAODMetaDataCnv/*.h Root/*.cxx
      PUBLIC_HEADERS xAODMetaDataCnv
      LINK_LIBRARIES AsgTools xAODMetaData )

   # Plus we also build a dictionary:
   atlas_add_dictionary( xAODMetaDataCnvDict
      xAODMetaDataCnv/xAODMetaDataCnvDict.h
      xAODMetaDataCnv/selection.xml
      LINK_LIBRARIES xAODMetaDataCnv )

else()

   # The main library is a component one in this case:
   atlas_add_component( xAODMetaDataCnv
      xAODMetaDataCnv/*.h src/*.cxx Root/*.cxx src/components/*.cxx
      LINK_LIBRARIES AsgTools xAODMetaData GaudiKernel AthenaKernel
      AthenaPoolUtilities EventInfo )

endif()

# Install files from the package:
atlas_install_joboptions( share/*.py )
