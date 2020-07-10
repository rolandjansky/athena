################################################################################
# Package: DCMathSegmentMaker
################################################################################

# Declare the package name:
atlas_subdir( DCMathSegmentMaker )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PRIVATE
                          Control/AthenaBaseComps
                          Control/AthenaKernel
                          Control/StoreGate
                          DetectorDescription/GeoPrimitives
                          Event/EventPrimitives
                          GaudiKernel
                          MuonSpectrometer/MuonDetDescr/MuonReadoutGeometry
                          MuonSpectrometer/MuonIdHelpers
                          MuonSpectrometer/MuonReconstruction/MuonRecEvent/MuonCompetingRIOsOnTrack
                          MuonSpectrometer/MuonReconstruction/MuonRecEvent/MuonPrepRawData
                          MuonSpectrometer/MuonReconstruction/MuonRecEvent/MuonRIO_OnTrack
                          MuonSpectrometer/MuonReconstruction/MuonRecEvent/MuonSegment
                          MuonSpectrometer/MuonReconstruction/MuonRecTools/MuonRecHelperTools
                          MuonSpectrometer/MuonReconstruction/MuonRecTools/MuonRecToolInterfaces
                          MuonSpectrometer/MuonReconstruction/MuonRecUtils/MuonStationIntersectSvc
                          MuonSpectrometer/MuonReconstruction/MuonSegmentMakers/MuonSegmentMakerInterfaces
                          MuonSpectrometer/MuonReconstruction/MuonSegmentMakers/MuonSegmentMakerTools/MuonSegmentMakerToolInterfaces
                          Tracking/TrkDetDescr/TrkGeometry
                          Tracking/TrkDetDescr/TrkSurfaces
                          Tracking/TrkEvent/TrkEventPrimitives
                          Tracking/TrkEvent/TrkParameters
                          Tracking/TrkEvent/TrkPseudoMeasurementOnTrack
                          Tracking/TrkEvent/TrkRIO_OnTrack
                          Tracking/TrkEvent/TrkRoad
                          Tracking/TrkEvent/TrkTrack
                          Tracking/TrkExtrapolation/TrkExInterfaces
                          Tracking/TrkFitter/TrkFitterInterfaces
                          Tracking/TrkTools/TrkToolInterfaces
                          Tracking/TrkUtilityPackages/TrkDriftCircleMath
			  MuonSpectrometer/MuonConditions/MuonCondGeneral/MuonCondData )

# External dependencies:
find_package( Eigen )

# Component(s) in the package:
atlas_add_component( DCMathSegmentMaker
                     src/*.cxx
                     src/components/*.cxx
                     INCLUDE_DIRS ${EIGEN_INCLUDE_DIRS}
                     LINK_LIBRARIES ${EIGEN_LIBRARIES} AthenaBaseComps AthenaKernel StoreGateLib SGtests GeoPrimitives EventPrimitives GaudiKernel MuonReadoutGeometry MuonIdHelpersLib MuonCompetingRIOsOnTrack MuonPrepRawData MuonRIO_OnTrack MuonSegment MuonRecHelperToolsLib MuonRecToolInterfaces MuonStationIntersectSvcLib TrkGeometry TrkSurfaces TrkEventPrimitives TrkParameters TrkPseudoMeasurementOnTrack TrkRIO_OnTrack TrkRoad TrkTrack TrkExInterfaces TrkFitterInterfaces TrkToolInterfaces TrkDriftCircleMath MuonCondData MuonSegmentMakerToolInterfaces MuonSegmentMakerInterfacesLib )
