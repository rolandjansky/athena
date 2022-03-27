# Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration

#
# author: Riccardo Maria BIANCHI <riccardo.maria.bianchi@cern.ch>, 2022 Jan
#

# Declare the package name:
atlas_subdir( VP1UtilsCoinSoQt )

# External dependencies:
find_package( Coin3D )
find_package( SoQt )

# Component(s) in the package:
atlas_add_library( VP1UtilsCoinSoQt
    VP1UtilsCoinSoQt/*.h src/*.cxx
    PUBLIC_HEADERS VP1UtilsCoinSoQt
    PRIVATE_INCLUDE_DIRS ${COIN3D_INCLUDE_DIRS} ${SOQT_INCLUDE_DIRS}
    PRIVATE_LINK_LIBRARIES ${COIN3D_LIBRARIES} ${SOQT_LIBRARIES} )

