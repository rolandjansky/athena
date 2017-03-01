################################################################################
# Package: TauDiscriminant
################################################################################

# Declare the package name: 
atlas_subdir( TauDiscriminant )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          Control/AthToolSupport/AsgTools
                          Control/AthenaBaseComps
                          Event/xAOD/xAODEgamma
                          Event/xAOD/xAODTau
                          GaudiKernel
                          PhysicsAnalysis/AnalysisCommon/PATInterfaces
                          Reconstruction/tauRecTools
                          Tools/PathResolver
                          PRIVATE
                          Control/CxxUtils
                          Control/StoreGate
      			  Event/FourMomUtils
			  Event/xAOD/xAODEventInfo
                          Event/xAOD/xAODTracking
                          PhysicsAnalysis/ElectronPhotonID/ElectronPhotonSelectorTools )

# External dependencies:
find_package( ROOT COMPONENTS Core Tree MathCore Hist RIO pthread MathMore Minuit Minuit2 Matrix Physics HistPainter Rint PyROOT )

# tag ROOTBasicLibs was not recognized in automatic conversion in cmt2cmake

# Component(s) in the package:
atlas_add_library( TauDiscriminantLib
                   Root/*.cxx
                   src/*.cxx
                   PUBLIC_HEADERS TauDiscriminant
                   PRIVATE_INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
                   LINK_LIBRARIES AsgTools AthenaBaseComps xAODEgamma xAODTau GaudiKernel PATInterfaces PathResolver tauRecToolsLib StoreGateLib SGtests ElectronPhotonSelectorToolsLib
                   PRIVATE_LINK_LIBRARIES ${ROOT_LIBRARIES} CxxUtils xAODEventInfo xAODTracking )

atlas_add_component( TauDiscriminant
                     src/components/*.cxx
                     INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
                     LINK_LIBRARIES ${ROOT_LIBRARIES} AsgTools AthenaBaseComps xAODEgamma xAODTau FourMomUtils GaudiKernel PATInterfaces tauRecToolsLib PathResolver CxxUtils StoreGateLib SGtests xAODEventInfo xAODTracking ElectronPhotonSelectorToolsLib TauDiscriminantLib )

atlas_add_dictionary( TauDiscriminantDict
                      TauDiscriminant/TauDiscriminantDict.h
                      TauDiscriminant/selection.xml
                      INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
                      LINK_LIBRARIES ${ROOT_LIBRARIES} AsgTools AthenaBaseComps xAODEgamma xAODTau FourMomUtils GaudiKernel PATInterfaces tauRecToolsLib PathResolver CxxUtils StoreGateLib SGtests xAODEventInfo xAODTracking ElectronPhotonSelectorToolsLib TauDiscriminantLib )

# Install files from the package:
atlas_install_python_modules( python/*.py )
atlas_install_joboptions( share/*.py )

