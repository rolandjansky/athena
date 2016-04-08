################################################################################
# Package: D3PDMakerCoreComps
################################################################################

# Declare the package name:
atlas_subdir( D3PDMakerCoreComps )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PRIVATE
                          Control/AthContainers
                          Control/AthenaBaseComps
                          Control/AthenaKernel
                          Control/CxxUtils
                          Control/DataModel
                          Control/Navigation
                          Control/RootUtils
                          Control/SGTools
                          Database/AthenaPOOL/AthenaPoolCnvSvc
                          GaudiKernel
                          PhysicsAnalysis/AnalysisCommon/ParticleEvent
                          PhysicsAnalysis/D3PDMaker/D3PDMakerInterfaces
                          PhysicsAnalysis/D3PDMaker/D3PDMakerUtils
                          TestPolicy )

# External dependencies:
find_package( Boost COMPONENTS filesystem thread system )
find_package( ROOT COMPONENTS Core Tree MathCore Hist RIO pthread )

# Component(s) in the package:
atlas_add_component( D3PDMakerCoreComps
                     src/*.cxx
                     src/components/*.cxx
                     INCLUDE_DIRS ${Boost_INCLUDE_DIRS} ${ROOT_INCLUDE_DIRS}
                     LINK_LIBRARIES ${Boost_LIBRARIES} ${ROOT_LIBRARIES} AthContainers AthenaBaseComps AthenaKernel CxxUtils DataModel Navigation RootUtils RootUtilsPyROOT SGTools AthenaPoolCnvSvcLib GaudiKernel ParticleEvent D3PDMakerUtils )

# Install files from the package:
atlas_install_python_modules( python/*.py )

