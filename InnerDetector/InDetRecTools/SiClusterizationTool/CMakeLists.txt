# Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( SiClusterizationTool )

# External dependencies:
find_package( lwtnn )
find_package( Eigen )
find_package( CLHEP )
find_package( ROOT COMPONENTS Core MathCore Hist )
find_package( COOL COMPONENTS CoolKernel CoolApplication )
find_package( Boost )

# Component(s) in the package:
atlas_add_library( SiClusterizationToolLib
   SiClusterizationTool/*.h src/*.cxx
   PUBLIC_HEADERS SiClusterizationTool
   INCLUDE_DIRS ${ROOT_INCLUDE_DIRS} ${LWTNN_INCLUDE_DIRS} ${EIGEN_INCLUDE_DIRS}
   PRIVATE_INCLUDE_DIRS ${CLHEP_INCLUDE_DIRS} ${COOL_INCLUDE_DIRS}
   PRIVATE_DEFINITIONS ${CLHEP_DEFINITIONS}
   LINK_LIBRARIES ${CLHEP_LIBRARIES} ${LWTNN_LIBRARIES} ${EIGEN_LIBRARIES} ${ROOT_LIBRARIES} AthenaBaseComps AthenaKernel BeamSpotConditionsData EventPrimitives GaudiKernel GeoPrimitives Identifier InDetCondTools InDetConditionsSummaryService InDetIdentifier InDetPrepRawData InDetRawData ReadoutGeometryBase InDetReadoutGeometry InDetRecToolInterfaces InDetSimData PixelConditionsData PixelGeoModelLib PixelReadoutGeometryLib PoolSvcLib StoreGateLib TrkNeuralNetworkUtilsLib TrkParameters TrkSurfaces LwtnnUtils
   PRIVATE_LINK_LIBRARIES ${Boost_LIBRARIES} ${COOL_LIBRARIES} AthenaPoolUtilities AtlasDetDescr AtlasHepMCLib DetDescrCondToolsLib FileCatalog SCT_ReadoutGeometry TrkEventPrimitives VxVertex CxxUtils  )

atlas_add_component( SiClusterizationTool
   src/components/*.cxx
   LINK_LIBRARIES SiClusterizationToolLib )

# Install files from the package:
atlas_install_joboptions( share/*.py )
# These files can be added by the user for testing in Grid environments,
# in which case un-comment this line and re-cmake
# atlas_install_data( share/*.db )
