# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( AthExThinning )

# Component(s) in the package:
atlas_add_library( AthExThinningEvent
                   src_lib/AthExIParticle.cxx
                   src_lib/AthExIParticles.cxx
                   src_lib/AthExParticle.cxx
                   src_lib/AthExParticles.cxx
                   src_lib/AthExElephantino.cxx
                   src_lib/AthExDecay.cxx
                   src_lib/AthExFatObject.cxx
                   src_lib/AthExElephantino_p1.cxx
                   src_lib/AthExDecay_p1.cxx
                   src_lib/AthExParticles_p1.cxx
                   src_lib/AthExFatObject_p1.cxx
                   PUBLIC_HEADERS AthExThinning
                   LINK_LIBRARIES AthContainers AthLinks AthenaBaseComps AthenaKernel DataModelAthenaPoolLib GaudiKernel StoreGateLib )

atlas_add_component( AthExThinningAlgs
                     src_lib/CreateData.cxx
                     src_lib/WriteThinnedData.cxx
                     src_lib/ReadThinnedData.cxx
                     src_lib/components/*.cxx
                     LINK_LIBRARIES AthExThinningEvent )

atlas_add_poolcnv_library( AthExThinningPoolCnv
                           src/*.cxx
                           FILES AthExThinning/AthExParticles.h AthExThinning/AthExIParticles.h AthExThinning/AthExDecay.h AthExThinning/AthExElephantino.h AthExThinning/AthExFatObject.h
                           LINK_LIBRARIES AthExThinningEvent AthenaPoolCnvSvcLib )

atlas_add_dictionary( AthExThinningEventDict
                      AthExThinning/AthExThinningEventDict.h
                      AthExThinning/selection.xml
                      LINK_LIBRARIES AthExThinningEvent
                      ELEMENT_LINKS AthExParticles AthExIParticles )

# Install files from the package:
atlas_install_python_modules( python/*.py
                              POST_BUILD_CMD ${ATLAS_FLAKE8} )
atlas_install_joboptions( share/*.py )
atlas_install_runtime( test/*.ref )


atlas_add_test( test_thinning
                SCRIPT test/test_thinning.sh
                PROPERTIES TIMEOUT 900
                LOG_IGNORE_PATTERN ":::running" )

atlas_add_test( test_pythinning
                SCRIPT test/test_pythinning.sh
                PROPERTIES TIMEOUT 900
                LOG_IGNORE_PATTERN ":::running" )
set_tests_properties( AthExThinning_test_pythinning_ctest
                      PROPERTIES DEPENDS AthExThinning_test_thinning_ctest )
