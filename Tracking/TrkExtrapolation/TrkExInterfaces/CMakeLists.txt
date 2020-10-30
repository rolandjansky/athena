# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( TrkExInterfaces )

# Component(s) in the package:
atlas_add_library( TrkExInterfaces
                   PUBLIC_HEADERS TrkExInterfaces
                   LINK_LIBRARIES GeoPrimitives xAODTracking GaudiKernel TrkDetDescrUtils TrkSurfaces TrkVolumes TrkEventPrimitives TrkMaterialOnTrack TrkNeutralParameters TrkParameters TrkParametersBase TrkExUtils )
