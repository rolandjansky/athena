# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( InDetTrackHoleSearch )

# Component(s) in the package:
atlas_add_component( InDetTrackHoleSearch
                     src/*.cxx
                     src/components/*.cxx
                     LINK_LIBRARIES AthenaBaseComps GaudiKernel GeoModelInterfaces TrkEventPrimitives TrkParameters TrkToolInterfaces AtlasDetDescr Identifier InDetReadoutGeometry InDetRecToolInterfaces TrkDetDescrUtils TrkGeometry TrkVolumes TrkEventUtils TrkMeasurementBase TrkTrack TrkTrackSummary TrkExInterfaces )
