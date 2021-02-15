# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( PixelDigitization )

# External dependencies:
find_package( CLHEP )
find_package( ROOT COMPONENTS Core Tree MathCore Hist RIO pthread Matrix TreePlayer )

# Component(s) in the package:
atlas_add_component( PixelDigitization
                     src/*.cxx
                     src/components/*.cxx
                     INCLUDE_DIRS ${ROOT_INCLUDE_DIRS} ${CLHEP_INCLUDE_DIRS}
                     LINK_LIBRARIES ${ROOT_LIBRARIES} ${CLHEP_LIBRARIES} AtlasHepMCLib AthenaBaseComps GaudiKernel AthenaKernel PileUpToolsLib StoreGateLib GeneratorObjects PixelConditionsData SiPropertiesToolLib InDetIdentifier ReadoutGeometryBase InDetReadoutGeometry PixelReadoutGeometry SiDigitization InDetCondTools InDetRawData InDetSimData InDetSimEvent HitManagement PathResolver PixelCablingLib InDetConditionsSummaryService )

# Install files from the package:
atlas_install_python_modules( python/*.py POST_BUILD_CMD ${ATLAS_FLAKE8} )
atlas_install_joboptions( share/PixelDigitization_jobOptions.py share/PixelDigiTool_jobOptions.py )

