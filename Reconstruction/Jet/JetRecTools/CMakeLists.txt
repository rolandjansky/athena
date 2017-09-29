################################################################################
# Package: JetRecTools
################################################################################

# Declare the package name:
atlas_subdir( JetRecTools )

set( extra_deps )
if( NOT XAOD_STANDALONE )
   set( extra_deps GaudiKernel )
endif()

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          Control/AthContainers
                          Control/AthToolSupport/AsgTools
                          Event/xAOD/xAODPFlow
                          Event/xAOD/xAODTracking
                          Event/xAOD/xAODCore
                          Event/xAOD/xAODBase
                          Event/xAOD/xAODCaloEvent
                          Event/xAOD/xAODTruth
                          Event/xAOD/xAODEgamma
                          Event/xAOD/xAODJet
                          InnerDetector/InDetRecTools/InDetTrackSelectionTool
                          InnerDetector/InDetRecTools/TrackVertexAssociationTool
                          Reconstruction/Jet/JetEDM
                          Reconstruction/Jet/JetInterface
                          Reconstruction/Jet/JetRec
                          Reconstruction/PFlow/PFlowUtils 
                          ${extra_deps} )

# External dependencies
find_package( FastJet )
find_package( FastJetContrib COMPONENTS SoftKiller ConstituentSubtractor )

# Component(s) in the package:
# JAF tried adding library
atlas_add_library( JetRecToolsLib
                   JetRecTools/*.h Root/*.cxx
                   PUBLIC_HEADERS JetRecTools
                   INCLUDE_DIRS ${FASTJET_INCLUDE_DIRS} 
                   PRIVATE_INCLUDE_DIRS ${FASTJETCONTRIB_INCLUDE_DIRS}
                   LINK_LIBRARIES ${FASTJET_LIBRARIES} AthContainers AsgTools xAODPFlow 
                   xAODTracking xAODCore xAODBase xAODCaloEvent xAODTruth xAODEgamma 
                   xAODJet TrackVertexAssociationToolLib JetEDM 
                   JetInterface JetRecLib InDetTrackSelectionToolLib
                   PRIVATE_LINK_LIBRARIES ${FASTJETCONTRIB_LIBRARIES} )

if( NOT XAOD_STANDALONE )
atlas_add_component( JetRecTools
                     JetRecTools/*.h Root/*.cxx src/components/*.cxx
                     LINK_LIBRARIES ${FASTJET_LIBRARIES} AthContainers AsgTools xAODPFlow 
                     xAODTracking xAODCore xAODBase xAODCaloEvent xAODTruth xAODEgamma 
                     xAODJet GaudiKernel TrackVertexAssociationToolLib JetEDM JetInterface JetRecLib JetRecToolsLib 
                     PRIVATE_LINK_LIBRARIES ${FASTJETCONTRIB_LIBRARIES})
endif()

# Install files from the package:
atlas_install_headers( JetRecTools )
atlas_install_python_modules( python/*.py )
