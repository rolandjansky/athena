# Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( InDetBeamSpotFinder )

# External dependencies:
find_package( CLHEP )
find_package( ROOT COMPONENTS Core Tree MathCore Hist Matrix Minuit RooFitCore RooFit )

# Component(s) in the package:
atlas_add_library( InDetBeamSpotFinderLib
                   src/*.cxx
                   PUBLIC_HEADERS InDetBeamSpotFinder
                   INCLUDE_DIRS ${CLHEP_INCLUDE_DIRS}
                   PRIVATE_INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
                   DEFINITIONS ${CLHEP_DEFINITIONS}
                   LINK_LIBRARIES ${CLHEP_LIBRARIES} BeamSpotConditionsData LumiBlockData GaudiKernel TrkFitterUtils xAODTracking
                   PRIVATE_LINK_LIBRARIES ${ROOT_LIBRARIES} AthenaBaseComps CxxUtils EventInfo xAODEventInfo TrkEventPrimitives VxVertex AtlasDetDescr Identifier StoreGateLib TrkFitterInterfaces TrkParameters TrkPseudoMeasurementOnTrack TrkRIO_OnTrack TrkTrack TrkVertexFitterInterfaces )

atlas_add_component( InDetBeamSpotFinder
                     src/components/*.cxx
                     LINK_LIBRARIES InDetBeamSpotFinderLib )

# Install files from the package:
atlas_install_joboptions( share/*.py )

