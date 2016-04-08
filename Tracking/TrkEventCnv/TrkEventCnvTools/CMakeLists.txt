################################################################################
# Package: TrkEventCnvTools
################################################################################

# Declare the package name:
atlas_subdir( TrkEventCnvTools )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          Control/AthenaBaseComps
                          Control/DataModel
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

# Component(s) in the package:
atlas_add_component( TrkEventCnvTools
                     src/*.cxx
                     src/components/*.cxx
                     INCLUDE_DIRS ${EIGEN_INCLUDE_DIRS}
                     LINK_LIBRARIES ${EIGEN_LIBRARIES} AthenaBaseComps DataModel StoreGateLib SGtests AthenaPoolUtilities GeoPrimitives Identifier GaudiKernel TrkDetDescrUtils TrkSurfaces TrkParametersBase TrkPrepRawData TrkRIO_OnTrack AtlasDetDescr TrkMeasurementBase )

# Install files from the package:
atlas_install_headers( TrkEventCnvTools )
atlas_install_python_modules( python/*.py )

