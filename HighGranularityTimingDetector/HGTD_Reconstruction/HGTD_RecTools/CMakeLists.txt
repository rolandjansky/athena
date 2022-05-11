# Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( HGTD_RecTools )

find_package( ROOT COMPONENTS Core Tree MathCore Hist RIO pthread )

# Component(s) in the package:
atlas_add_component( HGTD_RecTools
                    src/*.cxx
                    src/components/*.cxx
                    LINK_LIBRARIES AthenaBaseComps GaudiKernel
                    HGTD_RecToolInterfaces HGTD_PrepRawData HGTD_ReadoutGeometry
                    HGTD_Identifier HGTD_RIO_OnTrack TrkExInterfaces
                    TrkToolInterfaces TrkGeometry TrkTrack TrkDetDescrUtils
                    GeneratorObjects ${ROOT_LIBRARIES} InDetSimData xAODTruth )
