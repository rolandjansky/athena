################################################################################
# Package: TrkVertexSeedFinderTools
################################################################################

# Declare the package name:
atlas_subdir( TrkVertexSeedFinderTools )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          Control/AthenaBaseComps
                          GaudiKernel
                          Tracking/TrkVertexFitter/TrkVertexFitterInterfaces
                          Tracking/TrkVertexFitter/TrkVertexSeedFinderUtils
                          PRIVATE
                          DetectorDescription/GeoPrimitives
                          Event/EventPrimitives
                          Event/xAOD/xAODEventInfo
                          Generators/GenAnalysisTools/TruthHelper
                          Generators/GeneratorObjects
                          Tracking/TrkEvent/TrkEventPrimitives
                          Tracking/TrkEvent/TrkParameters
                          Tracking/TrkEvent/TrkTrack
                          Tracking/TrkEvent/VxVertex )

# External dependencies:
find_package( Eigen )
find_package( HepMC )
find_package( HepPDT )
find_package( ROOT COMPONENTS Core Tree MathCore Hist RIO pthread )

# Component(s) in the package:
atlas_add_component( TrkVertexSeedFinderTools
                     src/*.cxx
                     src/components/*.cxx
                     INCLUDE_DIRS ${HEPPDT_INCLUDE_DIRS} ${ROOT_INCLUDE_DIRS} ${EIGEN_INCLUDE_DIRS} ${HEPMC_INCLUDE_DIRS}
                     LINK_LIBRARIES ${HEPPDT_LIBRARIES} ${ROOT_LIBRARIES} ${EIGEN_LIBRARIES} ${HEPMC_LIBRARIES} AthenaBaseComps GaudiKernel TrkVertexFitterInterfaces GeoPrimitives EventPrimitives xAODEventInfo TruthHelper GeneratorObjects TrkEventPrimitives TrkParameters TrkTrack VxVertex )

# Install files from the package:
atlas_install_headers( TrkVertexSeedFinderTools )

