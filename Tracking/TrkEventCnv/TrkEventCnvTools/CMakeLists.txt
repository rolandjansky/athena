################################################################################
# Package: TrkEventCnvTools
################################################################################

# Declare the package name:
atlas_subdir( TrkEventCnvTools )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          Control/AthenaBaseComps
                          Control/AthLinks
                          Control/StoreGate
                          Database/AthenaPOOL/AthenaPoolUtilities
                          DetectorDescription/GeoPrimitives
                          DetectorDescription/Identifier
                          GaudiKernel
                          Tracking/TrkDetDescr/TrkDetDescrUtils
                          Tracking/TrkDetDescr/TrkSurfaces
                          Tracking/TrkEvent/TrkParametersBase
                          Tracking/TrkEvent/TrkPrepRawData
                          Tracking/TrkEvent/TrkRIO_OnTrack
                          PRIVATE
                          DetectorDescription/AtlasDetDescr
                          Tracking/TrkEvent/TrkMeasurementBase )

# External dependencies:
find_package( Eigen )

atlas_add_library( TrkEventCnvToolsLib
                   TrkEventCnvTools/*.h
                   INTERFACE
                   PUBLIC_HEADERS TrkEventCnvTools
                   LINK_LIBRARIES TrkSurfaces TrkDetDescrUtils TrkParametersBase GeoPrimitives GaudiKernel AthenaBaseComps AthenaPoolUtilities AthLinks StoreGateLib Identifier TrkPrepRawData TrkRIO_OnTrack )

# Component(s) in the package:
atlas_add_component( TrkEventCnvTools
                     src/*.cxx
                     src/components/*.cxx
                     INCLUDE_DIRS ${EIGEN_INCLUDE_DIRS}
                     LINK_LIBRARIES ${EIGEN_LIBRARIES} AthenaBaseComps AthLinks StoreGateLib SGtests AthenaPoolUtilities GeoPrimitives Identifier GaudiKernel TrkDetDescrUtils TrkSurfaces TrkParametersBase TrkPrepRawData TrkRIO_OnTrack AtlasDetDescr TrkMeasurementBase TrkEventCnvToolsLib )

# Install files from the package:
atlas_install_python_modules( python/*.py )

