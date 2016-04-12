################################################################################
# Package: IsolationCorrections
################################################################################

# Declare the package name:
atlas_subdir( IsolationCorrections )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          Control/AthToolSupport/AsgTools
                          Event/xAOD/xAODEgamma
                          Event/xAOD/xAODEventInfo
                          Event/xAOD/xAODPrimitives
                          Event/xAOD/xAODTracking
                          PhysicsAnalysis/AnalysisCommon/PATInterfaces
                          PRIVATE
                          GaudiKernel
                          Tools/PathResolver )

# External dependencies:
find_package( ROOT COMPONENTS Core Tree MathCore Hist RIO pthread )

# Component(s) in the package:
atlas_add_library( IsolationCorrectionsLib
                   Root/*.cxx
                   PUBLIC_HEADERS IsolationCorrections
                   INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
                   LINK_LIBRARIES ${ROOT_LIBRARIES} AsgTools xAODEgamma xAODEventInfo xAODPrimitives xAODTracking PATInterfaces
                   PRIVATE_LINK_LIBRARIES GaudiKernel PathResolver )

atlas_add_component( IsolationCorrections
                     src/components/*.cxx
                     INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
                     LINK_LIBRARIES ${ROOT_LIBRARIES} AsgTools xAODEgamma xAODEventInfo xAODPrimitives xAODTracking PATInterfaces GaudiKernel PathResolver IsolationCorrectionsLib )

atlas_add_dictionary( IsolationCorrectionsDict
                      IsolationCorrections/IsolationCorrectionsDict.h
                      IsolationCorrections/selection.xml
                      INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
                      LINK_LIBRARIES ${ROOT_LIBRARIES} AsgTools xAODEgamma xAODEventInfo xAODPrimitives xAODTracking PATInterfaces GaudiKernel PathResolver IsolationCorrectionsLib )

# Install files from the package:
atlas_install_data( data/*.root )

