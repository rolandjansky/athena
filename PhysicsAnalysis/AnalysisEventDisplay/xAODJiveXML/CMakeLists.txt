# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( xAODJiveXML )

# External dependencies:
find_package( CLHEP )

# Component(s) in the package:
atlas_add_component( xAODJiveXML
                     src/*.cxx
                     src/components/*.cxx
                     INCLUDE_DIRS ${CLHEP_INCLUDE_DIRS}
                     LINK_LIBRARIES ${CLHEP_LIBRARIES} AthenaBaseComps AthenaKernel CaloEvent CaloGeoHelpers JiveXMLLib TrkTrack xAODBTagging xAODCaloEvent xAODEgamma xAODJet xAODMissingET xAODMuon xAODTau xAODTracking )

# Install files from the package:
atlas_install_joboptions( share/*.py )
