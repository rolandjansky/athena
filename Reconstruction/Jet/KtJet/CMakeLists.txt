# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( KtJet )

# External dependencies:
find_package( CLHEP )

# Component(s) in the package:
atlas_add_library( KtJet
                   src/KtDistance.cxx
                   src/KtEvent.cxx
                   src/KtJetTable.cxx
                   src/KtLorentzVector.cxx
                   src/KtRecom.cxx
                   src/KtUtil.cxx
                   PUBLIC_HEADERS KtJet
                   INCLUDE_DIRS ${CLHEP_INCLUDE_DIRS}
                   DEFINITIONS ${CLHEP_DEFINITIONS}
                   LINK_LIBRARIES ${CLHEP_LIBRARIES} )

