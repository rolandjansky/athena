# Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( FastSiDigitization )

# External dependencies:
find_package( CLHEP )
find_package( ROOT COMPONENTS Core Tree RIO )

# Component(s) in the package:
atlas_add_component( FastSiDigitization
                     src/*.cxx
                     src/components/*.cxx
                     INCLUDE_DIRS ${ROOT_INCLUDE_DIRS} ${CLHEP_INCLUDE_DIRS}
                     LINK_LIBRARIES ${ROOT_LIBRARIES} ${CLHEP_LIBRARIES} AtlasHepMCLib AthenaBaseComps AthenaKernel PileUpToolsLib EventPrimitives xAODEventInfo GaudiKernel GeoPrimitives InDetReadoutGeometry PixelCablingLib PixelConditionsData ReadoutGeometryBase PixelReadoutGeometry SCT_ReadoutGeometry InDetCondTools InDetSimData InDetPrepRawData SiClusterizationToolLib InDetSimEvent HitManagement TrkTruthData StoreGateLib Identifier GeneratorObjects InDetIdentifier TrkDetDescrInterfaces TrkDigEvent TrkDigInterfaces TrkSurfaces TrkExUtils )

# Install files from the package:
atlas_install_python_modules( python/*.py POST_BUILD_CMD ${ATLAS_FLAKE8} )
atlas_install_joboptions( share/*.py )

