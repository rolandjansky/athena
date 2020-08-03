# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( AthLinks )

# External dependencies:
find_package( Boost )
find_package( ROOT COMPONENTS Core )

# Component(s) in the package:
atlas_add_library( AthLinks
                   src/*.cxx
                   PUBLIC_HEADERS AthLinks
                   INCLUDE_DIRS ${Boost_INCLUDE_DIRS}
                   PRIVATE_INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
                   LINK_LIBRARIES ${Boost_LIBRARIES} AthenaKernel CxxUtils SGTools GaudiKernel
                   PRIVATE_LINK_LIBRARIES ${ROOT_LIBRARIES} )

atlas_add_dictionary( AthLinksDict
                      AthLinks/AthLinksDict.h
                      AthLinks/selection.xml
                      LINK_LIBRARIES AthLinks )

# Tests in the package:
function( _add_test name )
   atlas_add_test( ${name}
                   SOURCES test/${name}.cxx
                   LINK_LIBRARIES AthLinks TestTools )
endfunction( _add_test )

_add_test( exceptions_test )
_add_test( DataProxyHolder_test )
_add_test( IndexHolder_test )
_add_test( IdentContIndex_test )
_add_test( ForwardIndexingPolicy_test )
_add_test( SetIndexingPolicy_test )
_add_test( MapIndexingPolicy_test )
_add_test( IdentContIndexingPolicy_test )
_add_test( IsSTLSequence_test )
_add_test( DataLinkBase_test )
_add_test( DataLink_test )
_add_test( ElementLinkBase_test )
_add_test( GenericElementLinkBase_test )
_add_test( ElementLink_test )
_add_test( ElementLinkFwd_test )
_add_test( AssociationMap_test )
