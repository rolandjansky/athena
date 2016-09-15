# $Id: CMakeLists.txt 773457 2016-09-15 13:17:29Z krasznaa $
################################################################################
# Package: UserAnalysisUtils
################################################################################

# Declare the package name:
atlas_subdir( UserAnalysisUtils )

# Declare the package's dependencies:
atlas_depends_on_subdirs(
   PUBLIC
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

# Component(s) in the package:
atlas_add_library( UserAnalysisUtilsLib
   UserAnalysisUtils/*.h src/*.cxx
   PUBLIC_HEADERS UserAnalysisUtils
   PRIVATE_INCLUDE_DIRS ${CLHEP_INCLUDE_DIRS}
   PRIVATE_DEFINITIONS ${CLHEP_DEFINITIONS}
   LINK_LIBRARIES CaloEvent AthenaBaseComps NavFourMom GaudiKernel JetEvent
   MissingETEvent muonEvent Particle egammaEvent tauEvent TrkSegment VxVertex
   AthAnalysisToolsLib
   PRIVATE_LINK_LIBRARIES ${CLHEP_LIBRARIES} )

atlas_add_component( UserAnalysisUtils
   src/components/*.cxx
   LINK_LIBRARIES UserAnalysisUtilsLib )

# Install files from the package:
atlas_install_joboptions( share/*.py )
