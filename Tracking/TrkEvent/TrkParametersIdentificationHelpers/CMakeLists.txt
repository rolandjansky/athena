# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( TrkParametersIdentificationHelpers )

# Component(s) in the package:
atlas_add_library( TrkParametersIdentificationHelpers
                   src/*.cxx
                   PUBLIC_HEADERS TrkParametersIdentificationHelpers
                   LINK_LIBRARIES CaloGeoHelpers AtlasDetDescr TrkParameters )

atlas_add_executable( TestTrkParametersIdentificationHelpers
                      exe/TestTrkParametersIdentificationHelpers.cxx
                      LINK_LIBRARIES CaloGeoHelpers AtlasDetDescr TrkParameters TrkParametersIdentificationHelpers )

