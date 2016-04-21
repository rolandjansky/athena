################################################################################
# Package: ElectronEfficiencyCorrection
################################################################################

# Declare the package name:
atlas_subdir( ElectronEfficiencyCorrection )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          Control/AthContainers
                          Control/AthToolSupport/AsgTools
                          Event/xAOD/xAODEgamma
                          PhysicsAnalysis/AnalysisCommon/PATCore
                          PhysicsAnalysis/AnalysisCommon/PATInterfaces
                          PRIVATE
                          Control/AthenaBaseComps
                          Event/xAOD/xAODCaloEvent
                          Event/xAOD/xAODCore
                          Event/xAOD/xAODEventInfo
                          Event/xAOD/xAODTracking
                          GaudiKernel
                          Tools/PathResolver )

# External dependencies:
find_package( Boost COMPONENTS filesystem thread system )
find_package( ROOT COMPONENTS Core Tree MathCore Hist RIO pthread MathMore Minuit Minuit2 Matrix Physics HistPainter Rint PyROOT )

# tag ROOTBasicLibs was not recognized in automatic conversion in cmt2cmake

# Component(s) in the package:
atlas_add_library( ElectronEfficiencyCorrectionLib
                   src/*.cxx
                   Root/*.cxx
                   PUBLIC_HEADERS ElectronEfficiencyCorrection
                   INCLUDE_DIRS ${Boost_INCLUDE_DIRS} ${ROOT_INCLUDE_DIRS}
                   LINK_LIBRARIES ${Boost_LIBRARIES} ${ROOT_LIBRARIES} AthContainers AsgTools xAODEgamma PATInterfaces PATCoreLib
                   PRIVATE_LINK_LIBRARIES AthenaBaseComps xAODCaloEvent xAODCore xAODEventInfo xAODTracking GaudiKernel PathResolver )

atlas_add_component( ElectronEfficiencyCorrection
                     src/components/*.cxx
                     INCLUDE_DIRS ${Boost_INCLUDE_DIRS} ${ROOT_INCLUDE_DIRS}
                     LINK_LIBRARIES ${Boost_LIBRARIES} ${ROOT_LIBRARIES} AthContainers AsgTools xAODEgamma PATCoreLib PATInterfaces AthenaBaseComps xAODCaloEvent xAODCore xAODEventInfo xAODTracking GaudiKernel PathResolver ElectronEfficiencyCorrectionLib )

atlas_add_dictionary( ElectronEfficiencyCorrectionDict
                      ElectronEfficiencyCorrection/ElectronEfficiencyCorrectionDict.h
                      ElectronEfficiencyCorrection/selection.xml
                      INCLUDE_DIRS ${Boost_INCLUDE_DIRS} ${ROOT_INCLUDE_DIRS}
                      LINK_LIBRARIES ${Boost_LIBRARIES} ${ROOT_LIBRARIES} AthContainers AsgTools xAODEgamma PATCoreLib PATInterfaces AthenaBaseComps xAODCaloEvent xAODCore xAODEventInfo xAODTracking GaudiKernel PathResolver ElectronEfficiencyCorrectionLib )

# Install files from the package:
atlas_install_joboptions( share/*.py )
atlas_install_data( data/*.root )

