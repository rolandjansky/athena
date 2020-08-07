# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( DataModelAthenaPool )

# External dependencies:
find_package( CLHEP )
find_package( ROOT COMPONENTS Core RIO )

# Component(s) in the package:
atlas_add_library( DataModelAthenaPoolLib
                   src/*.cxx
                   PUBLIC_HEADERS DataModelAthenaPool
                   PRIVATE_INCLUDE_DIRS ${ROOT_INCLUDE_DIRS} ${CLHEP_INCLUDE_DIRS}
                   PRIVATE_DEFINITIONS ${CLHEP_DEFINITIONS}
                   LINK_LIBRARIES AthLinks AthenaBaseComps AthenaKernel AthenaPoolCnvSvcLib GaudiKernel Navigation RootUtils
                   PRIVATE_LINK_LIBRARIES ${CLHEP_LIBRARIES} ${ROOT_LIBRARIES} AthContainers RootConversions )

atlas_add_component( DataModelAthenaPool
                     src/components/*.cxx
                     LINK_LIBRARIES DataModelAthenaPoolLib )

atlas_add_dictionary( DataModelAthenaPoolDictDict
                      DataModelAthenaPool/DataModelAthenaPoolDict.h
                      DataModelAthenaPool/selection.xml
                      LINK_LIBRARIES DataModelAthenaPoolLib )

# Install files from the package:
atlas_install_python_modules( python/*.py POST_BUILD_CMD ${ATLAS_FLAKE8} )
atlas_install_joboptions( share/DataModelAthenaPool_jobOptions.py )

# Tests in the package:
function( _add_test name )
   atlas_add_test( ${name}
                   SOURCES
                   test/${name}.cxx
                   LINK_LIBRARIES DataModelAthenaPoolLib SGTools )
endfunction( _add_test )

_add_test( DataLink_p2_test )
_add_test( ElementLinkCnv_p1_test )
_add_test( ElementLinkCnv_p2_test )
_add_test( ElementLinkCnv_p3_test )
_add_test( NavigableCnv_p1_test )
_add_test( NavigableCnv_p2_test )
_add_test( ElementLinkVectorCnv_p1_test )
_add_test( VectorElementLinkCnv_p1_test )

