################################################################################
# Package: McParticleEventTPCnv
################################################################################

# Declare the package name:
atlas_subdir( McParticleEventTPCnv )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          Control/DataModelAthenaPool
                          Database/AthenaPOOL/AthenaPoolCnvSvc
                          GaudiKernel
                          PhysicsAnalysis/TruthParticleID/McParticleEvent
                          PRIVATE
                          Control/AthenaKernel
                          Control/StoreGate
                          Generators/GeneratorObjects
                          PhysicsAnalysis/TruthParticleID/McParticleKernel )

# External dependencies:
find_package( Boost COMPONENTS filesystem thread system )
find_package( CLHEP )
find_package( HepMC )
find_package( ROOT COMPONENTS EG Core Tree MathCore Hist RIO pthread )

# Component(s) in the package:
atlas_add_tpcnv_library( McParticleEventTPCnv
                         src/*.cxx
                         PUBLIC_HEADERS McParticleEventTPCnv
                         INCLUDE_DIRS ${Boost_INCLUDE_DIRS} ${ROOT_INCLUDE_DIRS} ${CLHEP_INCLUDE_DIRS}
                         PRIVATE_INCLUDE_DIRS ${HEPMC_INCLUDE_DIRS}
                         DEFINITIONS ${CLHEP_DEFINITIONS}
                         LINK_LIBRARIES ${Boost_LIBRARIES} ${ROOT_LIBRARIES} ${CLHEP_LIBRARIES} ${HEPMC_LIBRARIES} DataModelAthenaPoolLib AthenaPoolCnvSvcLib GaudiKernel McParticleEvent AthenaKernel StoreGateLib SGtests GeneratorObjects McParticleKernel )

atlas_add_dictionary( McParticleEventTPCnvDict
                      McParticleEventTPCnv/McParticleEventTPCnvDict.h
                      McParticleEventTPCnv/selection.xml
                      INCLUDE_DIRS ${Boost_INCLUDE_DIRS} ${ROOT_INCLUDE_DIRS} ${CLHEP_INCLUDE_DIRS} ${HEPMC_INCLUDE_DIRS}
                      LINK_LIBRARIES ${Boost_LIBRARIES} ${ROOT_LIBRARIES} ${CLHEP_LIBRARIES} ${HEPMC_LIBRARIES} DataModelAthenaPoolLib AthenaPoolCnvSvcLib GaudiKernel McParticleEvent AthenaKernel StoreGateLib SGtests GeneratorObjects McParticleKernel McParticleEventTPCnv )

atlas_add_dictionary( OLD_McParticleEventTPCnvDict
                      McParticleEventTPCnv/McParticleEventTPCnvDict.h
                      McParticleEventTPCnv/OLD_selection.xml
                      INCLUDE_DIRS ${Boost_INCLUDE_DIRS} ${ROOT_INCLUDE_DIRS} ${CLHEP_INCLUDE_DIRS} ${HEPMC_INCLUDE_DIRS}
                      LINK_LIBRARIES ${Boost_LIBRARIES} ${ROOT_LIBRARIES} ${CLHEP_LIBRARIES} ${HEPMC_LIBRARIES} DataModelAthenaPoolLib AthenaPoolCnvSvcLib GaudiKernel McParticleEvent AthenaKernel StoreGateLib SGtests GeneratorObjects McParticleKernel McParticleEventTPCnv )

