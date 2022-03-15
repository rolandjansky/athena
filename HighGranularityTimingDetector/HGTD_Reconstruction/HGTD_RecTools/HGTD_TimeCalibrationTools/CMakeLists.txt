# Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( HGTD_TimeCalibrationTools )

# Component(s) in the package:
atlas_add_component( HGTD_TimeCalibrationTools
                    src/*.cxx
                    src/components/*.cxx
                    LINK_LIBRARIES AthenaBaseComps GaudiKernel
                    HGTD_RecToolInterfaces HGTD_ReadoutGeometry TrkTrack
                    HGTD_PrepRawData)

