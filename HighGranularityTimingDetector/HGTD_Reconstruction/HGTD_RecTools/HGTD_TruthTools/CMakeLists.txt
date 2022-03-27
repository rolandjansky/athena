# Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( HGTD_TruthTools )

find_package( ROOT COMPONENTS Core Tree MathCore Hist RIO pthread )

atlas_add_component( HGTD_TruthTools
                    src/*.cxx
                    src/components/*.cxx
                    LINK_LIBRARIES AthenaBaseComps GaudiKernel
                    HGTD_RecToolInterfaces GeneratorObjects ${ROOT_LIBRARIES}
                    HGTD_PrepRawData InDetSimData xAODTruth)
