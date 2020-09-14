# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( EventCommonAlgs )

# External dependencies:
find_package( Boost )

# Component(s) in the package:
atlas_add_library( EventCommonAlgsLib
                   EventCommonAlgs/*.h
                   INTERFACE
                   PUBLIC_HEADERS EventCommonAlgs
                   LINK_LIBRARIES AthenaBaseComps GaudiKernel )

atlas_add_component( EventCommonAlgs
                     src/*.cxx
                     src/components/*.cxx
                     INCLUDE_DIRS ${Boost_INCLUDE_DIRS}
                     LINK_LIBRARIES ${Boost_LIBRARIES} EventCommonAlgsLib FourMomUtils NavFourMom )

# Install files from the package:
atlas_install_joboptions( share/*.py )
