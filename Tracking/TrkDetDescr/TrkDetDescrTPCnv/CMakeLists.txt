################################################################################
# Package: TrkDetDescrTPCnv
################################################################################

# Declare the package name:
atlas_subdir( TrkDetDescrTPCnv )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          Database/AthenaPOOL/AthenaPoolCnvSvc
                          Database/AthenaPOOL/AthenaPoolUtilities
                          Tracking/TrkDetDescr/TrkDetDescrUtils
                          Tracking/TrkDetDescr/TrkGeometry
                          PRIVATE
                          AtlasTest/TestTools
                          Control/CxxUtils
                          GaudiKernel )

# External dependencies:
find_package( ROOT COMPONENTS Core Tree MathCore Hist RIO pthread )

# Component(s) in the package:
atlas_add_library( TrkDetDescrTPCnv
                   src/*.cxx
                   src/TrkDetDescrUtils/*.cxx
                   src/TrkGeometry/*.cxx
                   PUBLIC_HEADERS TrkDetDescrTPCnv
                   PRIVATE_INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
                   LINK_LIBRARIES AthenaPoolUtilities TrkDetDescrUtils TrkGeometry AthenaPoolCnvSvcLib
                   PRIVATE_LINK_LIBRARIES ${ROOT_LIBRARIES} TestTools CxxUtils GaudiKernel )

atlas_add_dictionary( TrkDetDescrTPCnvDict
                      TrkDetDescrTPCnv/TrkDetDescrTPCnvDict.h
                      TrkDetDescrTPCnv/selection.xml
                      INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
                      LINK_LIBRARIES ${ROOT_LIBRARIES} AthenaPoolCnvSvcLib AthenaPoolUtilities TrkDetDescrUtils TrkGeometry TestTools CxxUtils GaudiKernel TrkDetDescrTPCnv )

atlas_add_test( BinnedLayerMaterialCnv_p1_test
                SOURCES
                test/BinnedLayerMaterialCnv_p1_test.cxx
                INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
                LINK_LIBRARIES ${ROOT_LIBRARIES} AthenaPoolCnvSvcLib AthenaPoolUtilities TrkDetDescrUtils TrkGeometry TestTools CxxUtils GaudiKernel TrkDetDescrTPCnv )

atlas_add_test( BinUtilityCnv_p1_test
                SOURCES
                test/BinUtilityCnv_p1_test.cxx
                INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
                LINK_LIBRARIES ${ROOT_LIBRARIES} AthenaPoolCnvSvcLib AthenaPoolUtilities TrkDetDescrUtils TrkGeometry TestTools CxxUtils GaudiKernel TrkDetDescrTPCnv )

atlas_add_test( CompressedLayerMaterialCnv_p1_test
                SOURCES
                test/CompressedLayerMaterialCnv_p1_test.cxx
                INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
                LINK_LIBRARIES ${ROOT_LIBRARIES} AthenaPoolCnvSvcLib AthenaPoolUtilities TrkDetDescrUtils TrkGeometry TestTools CxxUtils GaudiKernel TrkDetDescrTPCnv )

atlas_add_test( CompoundLayerMaterialCnv_p1_test
                SOURCES
                test/CompoundLayerMaterialCnv_p1_test.cxx
                INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
                LINK_LIBRARIES ${ROOT_LIBRARIES} AthenaPoolCnvSvcLib AthenaPoolUtilities TrkDetDescrUtils TrkGeometry TestTools CxxUtils GaudiKernel TrkDetDescrTPCnv )

atlas_add_test( ElementTableCnv_p1_test
                SOURCES
                test/ElementTableCnv_p1_test.cxx
                INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
                LINK_LIBRARIES ${ROOT_LIBRARIES} AthenaPoolCnvSvcLib AthenaPoolUtilities TrkDetDescrUtils TrkGeometry TestTools CxxUtils GaudiKernel TrkDetDescrTPCnv )

atlas_add_test( HomogeneousLayerMaterialCnv_p1_test
                SOURCES
                test/HomogeneousLayerMaterialCnv_p1_test.cxx
                INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
                LINK_LIBRARIES ${ROOT_LIBRARIES} AthenaPoolCnvSvcLib AthenaPoolUtilities TrkDetDescrUtils TrkGeometry TestTools CxxUtils GaudiKernel TrkDetDescrTPCnv )

atlas_add_test( LayerMaterialMapCnv_p1_test
                SOURCES
                test/LayerMaterialMapCnv_p1_test.cxx
                INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
                LINK_LIBRARIES ${ROOT_LIBRARIES} AthenaPoolCnvSvcLib AthenaPoolUtilities TrkDetDescrUtils TrkGeometry TestTools CxxUtils GaudiKernel TrkDetDescrTPCnv )

atlas_add_test( MaterialCnv_p1_test
                SOURCES
                test/MaterialCnv_p1_test.cxx
                INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
                LINK_LIBRARIES ${ROOT_LIBRARIES} AthenaPoolCnvSvcLib AthenaPoolUtilities TrkDetDescrUtils TrkGeometry TestTools CxxUtils GaudiKernel TrkDetDescrTPCnv )

atlas_add_test( MaterialPropertiesCnv_p1_test
                SOURCES
                test/MaterialPropertiesCnv_p1_test.cxx
                INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
                LINK_LIBRARIES ${ROOT_LIBRARIES} AthenaPoolCnvSvcLib AthenaPoolUtilities TrkDetDescrUtils TrkGeometry TestTools CxxUtils GaudiKernel TrkDetDescrTPCnv )

atlas_add_test( MaterialStepCnv_p1_test
                SOURCES
                test/MaterialStepCnv_p1_test.cxx
                INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
                LINK_LIBRARIES ${ROOT_LIBRARIES} AthenaPoolCnvSvcLib AthenaPoolUtilities TrkDetDescrUtils TrkGeometry TestTools CxxUtils GaudiKernel TrkDetDescrTPCnv )

atlas_add_test( MaterialStepCollectionCnv_p1_test
                SOURCES
                test/MaterialStepCollectionCnv_p1_test.cxx
                INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
                LINK_LIBRARIES ${ROOT_LIBRARIES} AthenaPoolCnvSvcLib AthenaPoolUtilities TrkDetDescrUtils TrkGeometry TestTools CxxUtils GaudiKernel TrkDetDescrTPCnv )

