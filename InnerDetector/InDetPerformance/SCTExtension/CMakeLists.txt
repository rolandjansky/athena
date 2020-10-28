# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( SCTExtension )

# Component(s) in the package:
atlas_add_component( SCTExtension
                     src/*.cxx
                     src/components/*.cxx
                     LINK_LIBRARIES AthenaBaseComps xAODTracking InDetPrepRawData InDetRIO_OnTrack StoreGateLib TrkMeasurementBase TrkTrack )

# Install files from the package:
atlas_install_joboptions( share/*.py )
