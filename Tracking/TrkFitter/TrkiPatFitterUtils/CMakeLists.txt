################################################################################
# Package: TrkiPatFitterUtils
################################################################################

# Declare the package name:
atlas_subdir( TrkiPatFitterUtils )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          Control/AthenaBaseComps
                          Control/DataModel
                          DetectorDescription/GeoPrimitives
                          Event/EventPrimitives
                          GaudiKernel
                          Tracking/TrkEvent/TrkEventPrimitives
                          Tracking/TrkEvent/TrkParameters
			  Tracking/TrkAlignment/TrkAlgebraUtils
                          PRIVATE
                          Tracking/TrkDetDescr/TrkGeometry
                          Tracking/TrkDetDescr/TrkSurfaces
                          Tracking/TrkEvent/TrkRIO_OnTrack
                          MuonSpectrometer/MuonReconstruction/MuonRecEvent/MuonCompetingRIOsOnTrack
                          DetectorDescription/Identifier
                          Tracking/TrkEvent/TrkMaterialOnTrack
                          Tracking/TrkEvent/TrkMeasurementBase
                          Tracking/TrkEvent/TrkTrack
                          Tracking/TrkExtrapolation/TrkExInterfaces
                          Tracking/TrkExtrapolation/TrkExUtils )

# External dependencies:
find_package( CLHEP )
find_package( Eigen )

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
                   INCLUDE_DIRS ${EIGEN_INCLUDE_DIRS}
                   PRIVATE_INCLUDE_DIRS ${CLHEP_INCLUDE_DIRS}
                   PRIVATE_DEFINITIONS ${CLHEP_DEFINITIONS}
                   LINK_LIBRARIES ${EIGEN_LIBRARIES} AthenaBaseComps DataModel GeoPrimitives EventPrimitives GaudiKernel TrkEventPrimitives TrkParameters TrkAlgebraUtils
                   PRIVATE_LINK_LIBRARIES ${CLHEP_LIBRARIES} TrkGeometry TrkSurfaces TrkRIO_OnTrack MuonCompetingRIOsOnTrack Identifier TrkMaterialOnTrack TrkMeasurementBase TrkTrack TrkExInterfaces TrkExUtils )

