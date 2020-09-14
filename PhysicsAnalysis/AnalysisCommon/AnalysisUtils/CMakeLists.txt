################################################################################
# Package: AnalysisUtils
################################################################################

# Declare the package name:
atlas_subdir( AnalysisUtils )

if (NOT XAOD_STANDALONE)

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          Control/AthenaBaseComps
                          Control/AthenaKernel
                          Control/CxxUtils
                          Control/AthContainers
                          Control/StoreGate
                          Event/EventKernel
                          Event/NavFourMom
                          GaudiKernel
                          PhysicsAnalysis/AnalysisCommon/ParticleEvent
                          PhysicsAnalysis/AraTool
                          PhysicsAnalysis/TruthParticleID/McParticleEvent
                          PRIVATE
                          Reconstruction/Particle
                          Reconstruction/egamma/egammaEvent )

# External dependencies:
find_package( Boost COMPONENTS filesystem thread system )
find_package( CLHEP )
find_package( HepPDT )
find_package( ROOT COMPONENTS Core Tree MathCore Hist RIO pthread )

# Component(s) in the package:
atlas_add_library( AnalysisUtilsLib
                   src/*.cxx
                   PUBLIC_HEADERS AnalysisUtils
                   INCLUDE_DIRS ${Boost_INCLUDE_DIRS} ${HEPPDT_INCLUDE_DIRS} ${CLHEP_INCLUDE_DIRS}
                   PRIVATE_INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
                   DEFINITIONS ${CLHEP_DEFINITIONS}
                   LINK_LIBRARIES ${Boost_LIBRARIES} ${HEPPDT_LIBRARIES} ${CLHEP_LIBRARIES} AthenaBaseComps AthenaKernel CxxUtils AthContainers EventKernel NavFourMom GaudiKernel ParticleEvent AraTool McParticleEvent StoreGateLib SGtests
                   PRIVATE_LINK_LIBRARIES ${ROOT_LIBRARIES} Particle egammaEvent )

atlas_add_component( AnalysisUtils
                     src/components/*.cxx
                     INCLUDE_DIRS ${Boost_INCLUDE_DIRS} ${HEPPDT_INCLUDE_DIRS} ${ROOT_INCLUDE_DIRS} ${CLHEP_INCLUDE_DIRS}
                     LINK_LIBRARIES ${Boost_LIBRARIES} ${HEPPDT_LIBRARIES} ${ROOT_LIBRARIES} ${CLHEP_LIBRARIES} AthenaBaseComps AthenaKernel CxxUtils AthContainers StoreGateLib SGtests EventKernel NavFourMom GaudiKernel ParticleEvent AraTool McParticleEvent Particle egammaEvent AnalysisUtilsLib )

atlas_add_dictionary( AnalysisUtilsDict
                      AnalysisUtils/AnalysisUtilsDict.h
                      AnalysisUtils/selection.xml
                      INCLUDE_DIRS ${Boost_INCLUDE_DIRS} ${HEPPDT_INCLUDE_DIRS} ${ROOT_INCLUDE_DIRS} ${CLHEP_INCLUDE_DIRS}
                      LINK_LIBRARIES ${Boost_LIBRARIES} ${HEPPDT_LIBRARIES} ${ROOT_LIBRARIES} ${CLHEP_LIBRARIES} AthenaBaseComps AthenaKernel CxxUtils AthContainers StoreGateLib SGtests EventKernel NavFourMom GaudiKernel ParticleEvent AraTool McParticleEvent Particle egammaEvent AnalysisUtilsLib )

else()

find_package( Boost COMPONENTS )

# Component(s) in the package:
atlas_add_library( AnalysisUtilsLib INTERFACE_ONLY
                   PUBLIC_HEADERS AnalysisUtils
                   INCLUDE_DIRS ${Boost_INCLUDE_DIRS}
                   PRIVATE_INCLUDE_DIRS
                   DEFINITIONS
                   LINK_LIBRARIES ${Boost_LIBRARIES} CxxUtils AthContainers
                   PRIVATE_LINK_LIBRARIES )

endif()
