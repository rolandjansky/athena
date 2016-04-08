################################################################################
# Package: UserAnalysisUtils
################################################################################

# Declare the package name:
atlas_subdir( UserAnalysisUtils )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          Calorimeter/CaloEvent
                          Control/AthenaBaseComps
                          Event/NavFourMom
                          GaudiKernel
                          PhysicsAnalysis/AnalysisCommon/AnalysisTools
                          Reconstruction/Jet/JetEvent
                          Reconstruction/MissingETEvent
                          Reconstruction/MuonIdentification/muonEvent
                          Reconstruction/Particle
                          Reconstruction/egamma/egammaEvent
                          Reconstruction/tauEvent
                          Tracking/TrkEvent/TrkSegment
                          Tracking/TrkEvent/VxVertex )

# External dependencies:
find_package( CLHEP )

# tag ROOTBasicLibs was not recognized in automatic conversion in cmt2cmake

# tag ROOTMathLibs was not recognized in automatic conversion in cmt2cmake

# tag ROOTSTLDictLibs was not recognized in automatic conversion in cmt2cmake

# tag ROOTGraphicsLibs was not recognized in automatic conversion in cmt2cmake

# tag ROOTTableLibs was not recognized in automatic conversion in cmt2cmake

# Component(s) in the package:
atlas_add_library( UserAnalysisUtilsLib
                   src/*.cxx
                   PUBLIC_HEADERS UserAnalysisUtils
                   PRIVATE_INCLUDE_DIRS ${CLHEP_INCLUDE_DIRS}
                   PRIVATE_DEFINITIONS ${CLHEP_DEFINITIONS}
                   LINK_LIBRARIES CaloEvent AthenaBaseComps NavFourMom GaudiKernel JetEvent MissingETEvent muonEvent Particle egammaEvent tauEvent TrkSegment VxVertex AthAnalysisToolsLib
                   PRIVATE_LINK_LIBRARIES ${CLHEP_LIBRARIES} )

atlas_add_component( UserAnalysisUtils
                     src/components/*.cxx
                     INCLUDE_DIRS ${CLHEP_INCLUDE_DIRS}
                     LINK_LIBRARIES ${CLHEP_LIBRARIES} CaloEvent AthenaBaseComps NavFourMom GaudiKernel AthAnalysisToolsLib JetEvent MissingETEvent muonEvent Particle egammaEvent tauEvent TrkSegment VxVertex UserAnalysisUtilsLib )

# Install files from the package:
atlas_install_joboptions( share/*.py )

