# Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( ISF_FatrasTools )

# External dependencies:
find_package( CLHEP )
find_package( Eigen )
find_package( ROOT COMPONENTS Core Tree MathCore Hist RIO pthread )

# Component(s) in the package:
atlas_add_library( ISF_FatrasToolsLib
                   NO_PUBLIC_HEADERS
                   INCLUDE_DIRS ${ROOT_INCLUDE_DIRS} ${CLHEP_INCLUDE_DIRS} ${EIGEN_INCLUDE_DIRS}
                   LINK_LIBRARIES ${ROOT_LIBRARIES} ${CLHEP_LIBRARIES} ${EIGEN_LIBRARIES} AthenaBaseComps AthenaKernel GeoPrimitives EventPrimitives FastSimulationEventLib GaudiKernel ISF_Event ISF_InterfacesLib ISF_FatrasInterfaces TrkDetDescrUtils TrkGeometry TrkEventPrimitives TrkParameters TrkExInterfaces TrkExUtils StoreGateLib SGtests AtlasDetDescr TrkDetDescrInterfaces TrkSurfaces TrkVolumes TrkMaterialOnTrack TrkNeutralParameters TrkTrack )
 
atlas_add_component( ISF_FatrasTools
                     src/*.cxx src/components/*.cxx
                     LINK_LIBRARIES ISF_FatrasToolsLib )

# Test(s) in the package:
atlas_add_test( PhotonConversionTool_test
                SOURCES test/PhotonConversionTool_test.cxx
                LINK_LIBRARIES ISF_FatrasToolsLib TestTools
                ENVIRONMENT "JOBOPTSEARCHPATH=${CMAKE_CURRENT_SOURCE_DIR}/share" )
