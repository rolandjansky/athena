################################################################################
# Package: JetResolution
################################################################################

# Declare the package name:
atlas_subdir( JetResolution )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          Control/AthToolSupport/AsgTools
                          Event/xAOD/xAODJet
                          PhysicsAnalysis/AnalysisCommon/PATInterfaces
                          PRIVATE
                          Control/AthAnalysisBaseComps
                          Control/AthenaBaseComps
                          Event/xAOD/xAODEventInfo
                          GaudiKernel
                          Tools/PathResolver )

# External dependencies:
find_package( ROOT COMPONENTS Hist Core Tree MathCore RIO pthread MathMore Minuit Minuit2 Matrix Physics HistPainter Rint )

# Component(s) in the package:
atlas_add_library( JetResolutionLib
                   src/*.cxx
                   Root/*.cxx
                   PUBLIC_HEADERS JetResolution
                   PRIVATE_INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
                   LINK_LIBRARIES AsgTools xAODJet PATInterfaces
                   PRIVATE_LINK_LIBRARIES ${ROOT_LIBRARIES} AthAnalysisBaseComps AthenaBaseComps xAODEventInfo GaudiKernel PathResolver )

atlas_add_component( JetResolution
                     src/components/*.cxx
                     INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
                     LINK_LIBRARIES ${ROOT_LIBRARIES} AsgTools xAODJet PATInterfaces AthAnalysisBaseComps AthenaBaseComps xAODEventInfo GaudiKernel PathResolver JetResolutionLib )

atlas_add_dictionary( JetResolutionDict
                      JetResolution/JetResolutionDict.h
                      JetResolution/selection.xml
                      INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
                      LINK_LIBRARIES ${ROOT_LIBRARIES} AsgTools xAODJet PATInterfaces AthAnalysisBaseComps AthenaBaseComps xAODEventInfo GaudiKernel PathResolver JetResolutionLib )

# Install files from the package:
atlas_install_joboptions( share/*.py )

