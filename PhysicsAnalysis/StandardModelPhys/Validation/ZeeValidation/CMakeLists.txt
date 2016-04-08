################################################################################
# Package: ZeeValidation
################################################################################

# Declare the package name:
atlas_subdir( ZeeValidation )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          Control/AthenaMonitoring
                          Event/xAOD/xAODBase
                          Event/xAOD/xAODEgamma
                          Event/xAOD/xAODEventInfo
                          Event/xAOD/xAODTracking
                          Event/xAOD/xAODTruth
                          GaudiKernel
                          Tracking/TrkValidation/TrkValHistUtils
                          PRIVATE
                          Control/AthenaBaseComps
                          Event/xAOD/xAODPrimitives )

# External dependencies:
find_package( CLHEP )
find_package( ROOT COMPONENTS Core Tree MathCore Hist RIO pthread )

# Component(s) in the package:
atlas_add_component( ZeeValidation
                     src/*.cxx
                     src/components/*.cxx
                     INCLUDE_DIRS ${ROOT_INCLUDE_DIRS} ${CLHEP_INCLUDE_DIRS}
                     LINK_LIBRARIES ${ROOT_LIBRARIES} ${CLHEP_LIBRARIES} AthenaMonitoringLib xAODBase xAODEgamma xAODEventInfo xAODTracking xAODTruth GaudiKernel TrkValHistUtils AthenaBaseComps xAODPrimitives )

# Install files from the package:
atlas_install_headers( ZeeValidation )

