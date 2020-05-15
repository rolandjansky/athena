################################################################################
# Package: TrkDistributedKalmanFilter
################################################################################

# Declare the package name:
atlas_subdir( TrkDistributedKalmanFilter )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          Control/AthenaBaseComps
                          GaudiKernel
                          MagneticField/MagFieldConditions
                          MagneticField/MagFieldElements
                          Tracking/TrkEvent/TrkEventPrimitives
                          Tracking/TrkFitter/TrkFitterInterfaces
                          Tracking/TrkFitter/TrkFitterUtils
                          PRIVATE
                          Control/StoreGate
                          DetectorDescription/AtlasDetDescr
                          Tracking/TrkDetDescr/TrkDetElementBase
                          Tracking/TrkDetDescr/TrkSurfaces
                          Tracking/TrkEvent/TrkEventUtils
                          Tracking/TrkEvent/TrkMeasurementBase
                          Tracking/TrkEvent/TrkParameters
                          Tracking/TrkEvent/TrkPrepRawData
                          Tracking/TrkEvent/TrkRIO_OnTrack
                          Tracking/TrkEvent/TrkTrack
                          Tracking/TrkExtrapolation/TrkExInterfaces
                          Tracking/TrkTools/TrkToolInterfaces )

# Component(s) in the package:
atlas_add_library( TrkDistributedKalmanFilterLib
                   src/*.cxx
                   PUBLIC_HEADERS TrkDistributedKalmanFilter
                   LINK_LIBRARIES AthenaBaseComps GaudiKernel MagFieldElements MagFieldConditions TrkEventPrimitives TrkFitterInterfaces TrkFitterUtils StoreGateLib SGtests
                   PRIVATE_LINK_LIBRARIES AtlasDetDescr TrkDetElementBase TrkSurfaces TrkEventUtils TrkMeasurementBase TrkParameters TrkPrepRawData TrkRIO_OnTrack TrkTrack TrkExInterfaces TrkToolInterfaces )

atlas_add_component( TrkDistributedKalmanFilter
                     src/components/*.cxx
                     LINK_LIBRARIES AthenaBaseComps GaudiKernel MagFieldElements MagFieldConditions TrkEventPrimitives TrkFitterInterfaces TrkFitterUtils StoreGateLib SGtests AtlasDetDescr TrkDetElementBase TrkSurfaces TrkEventUtils TrkMeasurementBase TrkParameters TrkPrepRawData TrkRIO_OnTrack TrkTrack TrkExInterfaces TrkToolInterfaces TrkDistributedKalmanFilterLib )

# Install files from the package:
atlas_install_python_modules( python/*.py )

