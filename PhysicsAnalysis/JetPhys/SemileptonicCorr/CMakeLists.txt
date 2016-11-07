# $Id: CMakeLists.txt 782727 2016-11-07 13:00:55Z krasznaa $

# The name of the package:
atlas_subdir( SemileptonicCorr )

# External dependencies of the package:
find_package( ROOT COMPONENTS Core Hist RIO Physics )

# Component(s) in the package:
atlas_add_root_dictionary( SemileptonicCorr SemileptonicCorrCintDict
   ROOT_HEADERS SemileptonicCorr/semilCorr.h SemileptonicCorr/LinkDef.h
   EXTERNAL_PACKAGES ROOT )

atlas_add_library( SemileptonicCorr
   SemileptonicCorr/*.h Root/*.cxx ${SemileptonicCorrCintDict}
   PUBLIC_HEADERS SemileptonicCorr
   INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
   LINK_LIBRARIES ${ROOT_LIBRARIES} )

# Install files from the package:
atlas_install_data( data/*.root )
