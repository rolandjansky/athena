# Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( McParticleEventTPCnv )

# External dependencies:
find_package( Boost )
find_package( ROOT COMPONENTS EG Core Tree MathCore Hist RIO pthread )

# Component(s) in the package:
atlas_add_tpcnv_library( McParticleEventTPCnv
                         src/*.cxx
                         PUBLIC_HEADERS McParticleEventTPCnv
                         INCLUDE_DIRS ${Boost_INCLUDE_DIRS}
                         PRIVATE_INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
                         LINK_LIBRARIES LINK_LIBRARIES ${Boost_LIBRARIES} AthenaPoolCnvSvcLib DataModelAthenaPoolLib GeneratorObjects McParticleEvent McParticleKernel
                         PRIVATE_LINK_LIBRARIES ${ROOT_LIBRARIES} AthenaKernel AtlasHepMCLib GaudiKernel )

atlas_add_dictionary( McParticleEventTPCnvDict
                      McParticleEventTPCnv/McParticleEventTPCnvDict.h
                      McParticleEventTPCnv/selection.xml
                      LINK_LIBRARIES McParticleEventTPCnv )

# Tests in the package:
atlas_add_test( TruthParticleContainerCnv_p5_test
                SOURCES
                test/TruthParticleContainerCnv_p5_test.cxx
                LINK_LIBRARIES AthenaKernel AthenaPoolCnvSvcLib DataModelAthenaPoolLib GaudiKernel GeneratorObjects McParticleEvent McParticleEventTPCnv SGTools TestTools )

atlas_add_test( TruthParticleContainerCnv_p6_test
                SOURCES
                test/TruthParticleContainerCnv_p6_test.cxx
                LINK_LIBRARIES AthenaKernel AthenaPoolCnvSvcLib DataModelAthenaPoolLib GaudiKernel GeneratorObjects McParticleEvent McParticleEventTPCnv SGTools TestTools )

atlas_add_test( TruthEtIsolationsCnv_p1_test
                SOURCES
                test/TruthEtIsolationsCnv_p1_test.cxx
                INCLUDE_DIRS ${Boost_INCLUDE_DIRS}
                LINK_LIBRARIES ${Boost_LIBRARIES} AthenaPoolCnvSvcLib DataModelAthenaPoolLib GaudiKernel GeneratorObjects McParticleEvent McParticleEventTPCnv SGTools TestTools CxxUtils )
