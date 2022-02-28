# Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( PixelCalibAlgs )

# External dependencies:
find_package( ROOT COMPONENTS Graf Core Tree MathCore Hist RIO MathMore Physics
   Matrix Gpad )

# Libraries in the package:
atlas_add_library( PixelCalibAlgsLib
   PixelCalibAlgs/*.h src/*.cxx
   PUBLIC_HEADERS PixelCalibAlgs
   INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
   LINK_LIBRARIES ${ROOT_LIBRARIES} AthenaBaseComps CxxUtils GaudiKernel InDetConditionsSummaryService 
   InDetPrepRawData InDetReadoutGeometry PixelReadoutGeometryLib PixelConditionsData StoreGateLib
   PRIVATE_LINK_LIBRARIES AthenaPoolUtilities EventInfo InDetIdentifier InDetRawData PixelGeoModelLib )

atlas_add_component( PixelCalibAlgs
   src/components/*.cxx
   LINK_LIBRARIES GaudiKernel InDetByteStreamErrors PixelConditionsData PixelCalibAlgsLib )

atlas_add_executable( makeInactiveModuleList
   Application/makeInactiveModuleList.C
   INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
   LINK_LIBRARIES ${ROOT_LIBRARIES} PixelConditionsData PixelCalibAlgsLib )

atlas_install_runtime ( share/HashVSdcsID.dat share/table_Run2.txt share/PixelMapping_Run2.dat )
