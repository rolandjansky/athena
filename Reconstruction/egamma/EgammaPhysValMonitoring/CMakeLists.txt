# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( EgammaPhysValMonitoring )

# External dependencies:
find_package( CLHEP )

# Component(s) in the package:
atlas_add_component( EgammaPhysValMonitoring
                     src/*.cxx
                     src/components/*.cxx
                     INCLUDE_DIRS ${CLHEP_INCLUDE_DIRS}
                     LINK_LIBRARIES ${CLHEP_LIBRARIES} GaudiKernel AthenaBaseComps AthenaMonitoringLib StoreGateLib xAODEgamma xAODEventInfo xAODPrimitives xAODTruth MCTruthClassifierLib TrkValHistUtils )
