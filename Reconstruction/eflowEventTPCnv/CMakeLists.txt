# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( eflowEventTPCnv )

# External dependencies:
find_package( CLHEP )

# Component(s) in the package:
atlas_add_tpcnv_library( eflowEventTPCnv
                         src/*.cxx
                         PUBLIC_HEADERS eflowEventTPCnv
                         PRIVATE_INCLUDE_DIRS ${CLHEP_INCLUDE_DIRS}
                         PRIVATE_DEFINITIONS ${CLHEP_DEFINITIONS}
                         LINK_LIBRARIES AthenaPoolCnvSvcLib CaloEvent DataModelAthenaPoolLib Particle VxVertex eflowEvent muonEvent
                         PRIVATE_LINK_LIBRARIES AthLinks AthenaKernel Navigation )

atlas_add_dictionary( eflowEventTPCnvDict
                      eflowEventTPCnv/eflowEventTPCnvDict.h
                      eflowEventTPCnv/selection.xml
                      LINK_LIBRARIES LINK_LIBRARIES DataModelAthenaPoolLib eflowEventTPCnv )

# Tests in the package:
function( _add_test name )
   atlas_add_test( ${name}
                   SOURCES test/${name}.cxx
                   INCLUDE_DIRS ${CLHEP_INCLUDE_DIRS}
                   LINK_LIBRARIES ${CLHEP_LIBRARIES} AthenaPoolCnvSvcLib CaloEvent DataModelAthenaPoolLib Particle SGTools TestTools 
		                  VxVertex eflowEvent eflowEventTPCnv CxxUtils)
endfunction( _add_test )

_add_test( eflowObjectCnv_p1_test )
_add_test( eflowObjectCnv_p2_test )
_add_test( eflowObjectCnv_p3_test )
_add_test( eflowObjectCnv_p4_test )
_add_test( eflowObjectCnv_p5_test )
_add_test( eflowObjectContainerCnv_p1_test )
_add_test( eflowObjectContainerCnv_p2_test )
_add_test( eflowObjectContainerCnv_p3_test )
_add_test( eflowObjectContainerCnv_p4_test )
_add_test( eflowObjectContainerCnv_p5_test )
