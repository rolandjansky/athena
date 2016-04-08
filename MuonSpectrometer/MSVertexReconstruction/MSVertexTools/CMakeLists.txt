################################################################################
# Package: MSVertexTools
################################################################################

# Declare the package name:
atlas_subdir( MSVertexTools )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PRIVATE
                          Control/AthenaBaseComps
                          Control/AthenaKernel
                          Control/DataModel
                          Control/StoreGate
                          DetectorDescription/GeoPrimitives
                          DetectorDescription/Identifier
                          Event/EventPrimitives
                          Event/xAOD/xAODTracking
                          GaudiKernel
                          MuonSpectrometer/MSVertexReconstruction/MSVertexToolInterfaces
                          MuonSpectrometer/MSVertexReconstruction/MSVertexUtils
                          MuonSpectrometer/MuonDetDescr/MuonReadoutGeometry
                          MuonSpectrometer/MuonIdHelpers
                          MuonSpectrometer/MuonReconstruction/MuonRecEvent/MuonPrepRawData
                          Tracking/TrkEvent/TrkParameters
                          Tracking/TrkExtrapolation/TrkExInterfaces )

# External dependencies:
find_package( CLHEP )
find_package( Eigen )

# Component(s) in the package:
atlas_add_component( MSVertexTools
                     src/*.cxx
                     src/components/*.cxx
                     INCLUDE_DIRS ${CLHEP_INCLUDE_DIRS} ${EIGEN_INCLUDE_DIRS}
                     LINK_LIBRARIES ${CLHEP_LIBRARIES} ${EIGEN_LIBRARIES} AthenaBaseComps AthenaKernel DataModel StoreGateLib SGtests GeoPrimitives Identifier EventPrimitives xAODTracking GaudiKernel MSVertexUtils MuonReadoutGeometry MuonIdHelpersLib MuonPrepRawData TrkParameters TrkExInterfaces )

# Install files from the package:
atlas_install_headers( MSVertexTools )

