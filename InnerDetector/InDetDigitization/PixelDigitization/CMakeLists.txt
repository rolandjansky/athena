# Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( PixelDigitization )

# External dependencies:
find_package( CLHEP )
find_package( Boost )
find_package( ROOT COMPONENTS Core Tree MathCore Hist RIO pthread Matrix TreePlayer )

# Component(s) in the package:
atlas_add_component( PixelDigitization
                     src/*.cxx
                     src/components/*.cxx
                     INCLUDE_DIRS ${ROOT_INCLUDE_DIRS} ${CLHEP_INCLUDE_DIRS}
                     LINK_LIBRARIES ${ROOT_LIBRARIES} ${CLHEP_LIBRARIES} AtlasHepMCLib AthenaBaseComps GaudiKernel AthenaKernel PileUpToolsLib StoreGateLib GeneratorObjects PixelConditionsData SiPropertiesToolLib InDetIdentifier ReadoutGeometryBase InDetReadoutGeometry PixelReadoutGeometryLib SiDigitization InDetCondTools InDetRawData InDetSimData InDetSimEvent HitManagement PathResolver InDetConditionsSummaryService )

atlas_add_test( BichselData_test
   SOURCES test/BichselData_test.cxx  src/BichselData.cxx src/PixelDigitizationUtilities.cxx
   INCLUDE_DIRS ${Boost_INCLUDE_DIRS}
   LINK_LIBRARIES ${Boost_LIBRARIES} CxxUtils 
   POST_EXEC_SCRIPT "nopost.sh" )
   
atlas_add_test( PixelDigitizationUtilities_test
   SOURCES test/PixelDigitizationUtilities_test.cxx src/PixelDigitizationUtilities.cxx src/BichselData.cxx
   INCLUDE_DIRS ${Boost_INCLUDE_DIRS}
   LINK_LIBRARIES ${Boost_LIBRARIES} GaudiKernel AthenaKernel CxxUtils TestTools PathResolver
   POST_EXEC_SCRIPT "nopost.sh" )


# Install files from the package:
atlas_install_python_modules( python/*.py POST_BUILD_CMD ${ATLAS_FLAKE8} )
atlas_install_joboptions( share/PixelDigitization_jobOptions.py share/PixelDigiTool_jobOptions.py )
atlas_install_data( test/BichselTest.dat )

