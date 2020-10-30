# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( TrkiPatFitterUtils )

# External dependencies:
find_package( CLHEP )

# Component(s) in the package:
atlas_add_library( TrkiPatFitterUtils
                   src/ExtrapolationType.cxx
                   src/FitMatrices.cxx
                   src/FitMeasurement.cxx
                   src/FitParameters.cxx
                   src/FitProcedure.cxx
                   src/FitProcedureQuality.cxx
                   src/MeasurementProcessor.cxx
                   src/MeasurementType.cxx
                   src/MessageHelper.cxx
                   src/ParameterType.cxx
                   PUBLIC_HEADERS TrkiPatFitterUtils
                   PRIVATE_INCLUDE_DIRS ${CLHEP_INCLUDE_DIRS}
                   PRIVATE_DEFINITIONS ${CLHEP_DEFINITIONS}
                   LINK_LIBRARIES AthenaBaseComps AthContainers GeoPrimitives EventPrimitives GaudiKernel TrkEventPrimitives TrkGeometry TrkParameters
                   PRIVATE_LINK_LIBRARIES ${CLHEP_LIBRARIES} TrkSurfaces TrkMaterialOnTrack TrkMeasurementBase TrkTrack TrkExInterfaces TrkExUtils )
