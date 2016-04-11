# $Id: CMakeLists.txt 739324 2016-04-11 13:17:04Z krasznaa $
################################################################################
# Package: ISF_FatrasEventTPCnv
################################################################################

# Declare the package name:
atlas_subdir( ISF_FatrasEventTPCnv )

# Declare the package's dependencies:
atlas_depends_on_subdirs(
   PUBLIC
   Database/AthenaPOOL/AthenaPoolCnvSvc
   Database/AthenaPOOL/AthenaPoolUtilities
   DetectorDescription/Identifier
   InnerDetector/InDetEventCnv/InDetEventTPCnv
   Simulation/ISF/ISF_Fatras/ISF_FatrasDetDescrModel
   Simulation/ISF/ISF_Fatras/ISF_FatrasEvent
   Tracking/TrkEventCnv/TrkEventTPCnv
   PRIVATE
   AtlasTest/TestTools
   Control/AthenaKernel
   Control/CxxUtils
   Control/DataModel
   Control/StoreGate
   DetectorDescription/GeoPrimitives
   DetectorDescription/IdDictParser
   Event/EventPrimitives
   GaudiKernel
   InnerDetector/InDetDetDescr/InDetIdentifier
   Tracking/TrkDetDescr/TrkGeometry )

# Component(s) in the package:
atlas_add_tpcnv_library( ISF_FatrasEventTPCnv
   ISF_FatrasEventTPCnv/*.h ISF_FatrasEventTPCnv/ISF_FatrasEvent/*.h
   src/*.cxx src/ISF_FatrasEvent/*.cxx
   PUBLIC_HEADERS ISF_FatrasEventTPCnv
   LINK_LIBRARIES AthenaPoolCnvSvcLib AthenaPoolUtilities Identifier
   InDetEventTPCnv ISF_FatrasDetDescrModel ISF_FatrasEvent TrkEventTPCnv
   PRIVATE_LINK_LIBRARIES AthenaKernel CxxUtils DataModel StoreGateLib
   EventPrimitives GaudiKernel InDetIdentifier )

atlas_add_dictionary( ISF_FatrasEventTPCnvDict
   ISF_FatrasEventTPCnv/ISF_FatrasEventTPCnvDict.h
   ISF_FatrasEventTPCnv/selection.xml
   LINK_LIBRARIES ISF_FatrasEventTPCnv )

# Helper macro for setting up the tests:
macro( _add_test name )
   atlas_add_test( ${name}
      SOURCES test/${name}.cxx
      LINK_LIBRARIES TestTools CxxUtils StoreGateLib GeoPrimitives IdDictParser
      GaudiKernel InDetIdentifier TrkGeometry ISF_FatrasEventTPCnv 
      ENVIRONMENT "JOBOPTSEARCHPATH=${CMAKE_CURRENT_SOURCE_DIR}/share" )
endmacro( _add_test )

# Test(s) in the package:
_add_test( PlanarClusterCnv_p1_test )
_add_test( PlanarClusterCnv_p2_test )

_add_test( PlanarClusterContainerCnv_p1_test )
_add_test( PlanarClusterContainerCnv_p2_test )

# Clean up:
unset( _add_test )
