# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( TrkDetDescrTPCnv )

# Component(s) in the package:
atlas_add_library( TrkDetDescrTPCnv
                   src/*.cxx
                   src/TrkDetDescrUtils/*.cxx
                   src/TrkGeometry/*.cxx
                   PUBLIC_HEADERS TrkDetDescrTPCnv
                   LINK_LIBRARIES AthenaPoolUtilities TrkDetDescrUtils TrkGeometry AthenaPoolCnvSvcLib )

atlas_add_dictionary( TrkDetDescrTPCnvDict
                      TrkDetDescrTPCnv/TrkDetDescrTPCnvDict.h
                      TrkDetDescrTPCnv/selection.xml
                      LINK_LIBRARIES TrkDetDescrTPCnv )

# Tests in the package:
function( _add_test name )
   atlas_add_test( ${name}
                   SOURCES test/${name}.cxx
                   LINK_LIBRARIES GaudiKernel TestTools TrkDetDescrTPCnv )
endfunction( _add_test )

_add_test( BinnedLayerMaterialCnv_p1_test )
_add_test( BinUtilityCnv_p1_test )
_add_test( CompressedLayerMaterialCnv_p1_test )
_add_test( CompoundLayerMaterialCnv_p1_test )
_add_test( ElementTableCnv_p1_test )
_add_test( HomogeneousLayerMaterialCnv_p1_test )
_add_test( LayerMaterialMapCnv_p1_test )
_add_test( MaterialCnv_p1_test )
_add_test( MaterialPropertiesCnv_p1_test )
_add_test( MaterialStepCnv_p1_test )
_add_test( MaterialStepCollectionCnv_p1_test )
