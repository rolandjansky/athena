################################################################################
# Package: FastSiDigitization
################################################################################

# Declare the package name:
atlas_subdir( FastSiDigitization )

# External dependencies:
find_package( CLHEP )
find_package( ROOT COMPONENTS Core Tree MathCore Hist RIO pthread )

# Component(s) in the package:
atlas_add_component( FastSiDigitization
                     src/*.cxx
                     src/components/*.cxx
                     INCLUDE_DIRS ${ROOT_INCLUDE_DIRS} ${CLHEP_INCLUDE_DIRS}
                     LINK_LIBRARIES ${ROOT_LIBRARIES} ${CLHEP_LIBRARIES} AtlasHepMCLib AthenaBaseComps AthenaKernel PileUpToolsLib EventPrimitives xAODEventInfo GaudiKernel InDetReadoutGeometry PixelReadoutGeometry SCT_ReadoutGeometry InDetSimData InDetPrepRawData SiClusterizationToolLib InDetSimEvent HitManagement ISF_FatrasDetDescrModel ISF_FatrasEvent TrkTruthData StoreGateLib SGtests Identifier GeneratorObjects InDetIdentifier TrkDetDescrInterfaces TrkDigEvent TrkDigInterfaces TrkGeometry TrkSurfaces TrkExUtils )

# Install files from the package:
atlas_install_python_modules( python/*.py )
atlas_install_joboptions( share/*.py )

