# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( AnalysisAssociation )

# External dependencies:
find_package( Boost )

# Component(s) in the package:
atlas_add_library( AnalysisAssociation
                   src/*.cxx
                   PUBLIC_HEADERS AnalysisAssociation
                   INCLUDE_DIRS ${Boost_INCLUDE_DIRS}
                   LINK_LIBRARIES ${Boost_LIBRARIES} AthenaKernel EventKernel FourMomUtils GaudiKernel AnalysisUtilsLib )

