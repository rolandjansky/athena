# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( LArRegionSelector )

# External dependencies:
find_package( tdaq-common )

# Component(s) in the package:
atlas_add_component( LArRegionSelector
                     src/*.cxx
                     src/components/*.cxx
                     INCLUDE_DIRS ${TDAQ-COMMON_INCLUDE_DIRS}
                     LINK_LIBRARIES ${TDAQ-COMMON_LIBRARIES} CaloIdentifier AthenaBaseComps Identifier IRegionSelector RegionSelectorLib GaudiKernel CaloDetDescrLib CaloTTDetDescr CaloTriggerToolLib StoreGateLib ByteStreamData LArIdentifier LArRawUtilsLib PixelConditionsData )

# Install files from the package:
atlas_install_joboptions( share/*.py )
