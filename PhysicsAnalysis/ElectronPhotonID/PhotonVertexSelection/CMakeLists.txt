################################################################################
# Package: PhotonVertexSelection
################################################################################

# Declare the package name:
atlas_subdir( PhotonVertexSelection )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          Control/AthToolSupport/AsgTools
                          Event/xAOD/xAODEgamma
                          Event/xAOD/xAODEventInfo
                          Event/xAOD/xAODTracking
                          PRIVATE
                          Control/AthContainers
                          Control/AthenaBaseComps
                          GaudiKernel
                          PhysicsAnalysis/ElectronPhotonID/IsolationCorrections
                          Tools/PathResolver )

# External dependencies:
find_package( ROOT COMPONENTS Core Tree MathCore Hist RIO pthread TMVA )

# Component(s) in the package:
atlas_add_library( PhotonVertexSelectionLib
                   src/*.cxx
                   Root/*.cxx
                   PUBLIC_HEADERS PhotonVertexSelection
                   INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
                   LINK_LIBRARIES ${ROOT_LIBRARIES} AsgTools xAODEgamma xAODEventInfo xAODTracking IsolationCorrectionsLib
                   PRIVATE_LINK_LIBRARIES AthContainers AthenaBaseComps GaudiKernel PathResolver )

atlas_add_component( PhotonVertexSelection
                     src/components/*.cxx
                     INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
                     LINK_LIBRARIES ${ROOT_LIBRARIES} AsgTools xAODEgamma xAODEventInfo xAODTracking AthContainers AthenaBaseComps GaudiKernel IsolationCorrectionsLib PathResolver PhotonVertexSelectionLib )

atlas_add_dictionary( PhotonVertexSelectionDict
                      PhotonVertexSelection/PhotonVertexSelectionDict.h
                      PhotonVertexSelection/selection.xml
                      INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
                      LINK_LIBRARIES ${ROOT_LIBRARIES} AsgTools xAODEgamma xAODEventInfo xAODTracking AthContainers AthenaBaseComps GaudiKernel IsolationCorrectionsLib PathResolver PhotonVertexSelectionLib )

