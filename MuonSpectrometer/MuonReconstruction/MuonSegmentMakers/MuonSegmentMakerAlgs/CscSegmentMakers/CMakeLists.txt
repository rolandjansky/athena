################################################################################
# Package: CscSegmentMakers
################################################################################

# Declare the package name:
atlas_subdir( CscSegmentMakers )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          DetectorDescription/GeoPrimitives
                          GaudiKernel
                          MuonSpectrometer/MuonReconstruction/MuonRecEvent/MuonPrepRawData
                          MuonSpectrometer/MuonReconstruction/MuonRecEvent/MuonSegment
                          PRIVATE
                          Control/AthenaBaseComps
                          Control/StoreGate
                          DetectorDescription/Identifier
                          Event/EventPrimitives
                          Event/xAOD/xAODEventInfo
                          MuonSpectrometer/MuonDetDescr/MuonReadoutGeometry
                          MuonSpectrometer/MuonIdHelpers
                          MuonSpectrometer/MuonReconstruction/MuonDataPrep/CscClusterization
                          MuonSpectrometer/MuonReconstruction/MuonRecEvent/MuonRIO_OnTrack
                          MuonSpectrometer/MuonReconstruction/MuonRecTools/MuonRecHelperTools
                          MuonSpectrometer/MuonReconstruction/MuonRecTools/MuonRecToolInterfaces
                          MuonSpectrometer/MuonConditions/MuonCondGeneral/MuonCondData
                          MuonSpectrometer/MuonConditions/MuonCondGeneral/MuonCondInterface
                          Tracking/TrkDetDescr/TrkSurfaces
                          Tracking/TrkEvent/TrkEventPrimitives
                          Tracking/TrkEvent/TrkRoad
                          Tracking/TrkEvent/TrkSegment )

# External dependencies:
find_package( Eigen )
find_package( ROOT COMPONENTS Core Tree MathCore Hist RIO pthread )

atlas_add_library( CscSegmentMakersLib
                   CscSegmentMakers/*.h
                   INTERFACE
                   PUBLIC_HEADERS CscSegmentMakers
                   LINK_LIBRARIES GaudiKernel MuonRecToolInterfaces MuonSegment MuonPrepRawData CscSegmentMakersLib GeoPrimitives )

# Component(s) in the package:
atlas_add_component( CscSegmentMakers
                     src/*.cxx
                     src/components/*.cxx
                     INCLUDE_DIRS ${ROOT_INCLUDE_DIRS} ${EIGEN_INCLUDE_DIRS}
                     LINK_LIBRARIES ${ROOT_LIBRARIES} ${EIGEN_LIBRARIES} CscSegmentMakersLib AthenaBaseComps StoreGateLib SGtests Identifier EventPrimitives xAODEventInfo MuonReadoutGeometry MuonIdHelpersLib CscClusterizationLib MuonRIO_OnTrack MuonRecHelperToolsLib MuonCondData TrkSurfaces TrkEventPrimitives TrkRoad TrkSegment  )


