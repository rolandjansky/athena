# Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration

# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( SiSpacePointsSeedTool_xk )

# Component(s) in the package:
atlas_add_component( SiSpacePointsSeedTool_xk
                     src/*.cxx
                     src/components/*.cxx
                     LINK_LIBRARIES AthenaBaseComps BeamSpotConditionsData GaudiKernel InDetPrepRawData InDetReadoutGeometry InDetRecToolInterfaces MagFieldConditions MagFieldElements SiSPSeededTrackFinderData TrkEventUtils TrkSpacePoint CxxUtils )
