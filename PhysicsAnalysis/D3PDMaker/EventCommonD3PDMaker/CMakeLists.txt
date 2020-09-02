################################################################################
# Package: EventCommonD3PDMaker
################################################################################

# Declare the package name:
atlas_subdir( EventCommonD3PDMaker )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PRIVATE
                          Control/AthenaBaseComps
                          Control/AthenaKernel
                          Control/Navigation
                          Database/AthenaPOOL/AthenaPoolUtilities
                          Database/IOVDbDataModel
                          Event/EventBookkeeperMetaData
                          Event/EventKernel
                          Event/FourMomUtils
                          Event/NavFourMom
                          Event/xAOD/xAODBase
                          Event/xAOD/xAODEventInfo
                          GaudiKernel
                          PhysicsAnalysis/AnalysisCommon/ParticleEvent
                          PhysicsAnalysis/D3PDMaker/D3PDMakerInterfaces
                          PhysicsAnalysis/D3PDMaker/D3PDMakerUtils
                          Reconstruction/Jet/JetEvent )

# External dependencies:
find_package( Boost COMPONENTS filesystem thread system )
find_package( CLHEP )
find_package( ROOT COMPONENTS Physics Core Tree MathCore Hist RIO pthread )

# Component(s) in the package:
atlas_add_component( EventCommonD3PDMaker
                     src/*.cxx
                     src/components/*.cxx
                     INCLUDE_DIRS ${ROOT_INCLUDE_DIRS} ${Boost_INCLUDE_DIRS} ${CLHEP_INCLUDE_DIRS}
                     LINK_LIBRARIES ${ROOT_LIBRARIES} ${Boost_LIBRARIES} ${CLHEP_LIBRARIES} AthenaBaseComps AthenaKernel Navigation AthenaPoolUtilities IOVDbDataModel EventBookkeeperMetaData EventKernel FourMomUtils NavFourMom xAODBase xAODEventInfo GaudiKernel ParticleEvent D3PDMakerUtils JetEvent )

# Install files from the package:
atlas_install_python_modules( python/*.py )

