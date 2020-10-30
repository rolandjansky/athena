# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( xAODMetaDataCnv )

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
