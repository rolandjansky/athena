################################################################################
# Package: TrigHTTSGInput
################################################################################

# Declare the package name:
atlas_subdir( TrigHTTSGInput )


# External dependencies:
find_package( Boost COMPONENTS iostreams filesystem thread system )
find_package( HepPDT )
find_package( ROOT COMPONENTS Core Tree MathCore Hist RIO pthread )

# Component(s) in the package:
atlas_add_library(
  TrigHTTSGInputLib         src/*.cxx TrigHTTSGInput/*.h ${TrigHTTSGInputLibDictSource}
  PUBLIC_HEADERS            TrigHTTSGInput
  INCLUDE_DIRS              ${ROOT_INCLUDE_DIRS} ${Boost_INCLUDE_DIRS}
  PRIVATE_INCLUDE_DIRS      ${HEPPDT_INCLUDE_DIRS}
  LINK_LIBRARIES            ${ROOT_LIBRARIES} ${Boost_LIBRARIES}
                            AthenaBaseComps GeneratorObjects InDetIdentifier InDetPrepRawData SCT_CablingLib SGtests StoreGateLib TrigCaloEvent  TrigDecisionToolLib TrigHTTObjectsLib TrkExInterfaces TrkToolInterfaces TrkTrack TrkTruthData BeamSpotConditionsData InDetSimData AtlasHepMCLib
  PRIVATE_LINK_LIBRARIES    ${HEPPDT_LIBRARIES}
                            AtlasDetDescr EventInfo EventPrimitives GaudiKernel IdDict IdDictDetDescr Identifier InDetRawData InDetReadoutGeometry InDetRIO_OnTrack ReadoutGeometryBase TileIdentifier TrkFitterInterfaces TrkRIO_OnTrack TrkSpacePoint VxVertex 
)

atlas_add_component(
  TrigHTTSGInput            src/components/*.cxx
  INCLUDE_DIRS              ${ROOT_INCLUDE_DIRS} ${Boost_INCLUDE_DIRS} ${HEPPDT_INCLUDE_DIRS}
  LINK_LIBRARIES            TrigHTTSGInputLib
)

# Install files from the package:
atlas_install_joboptions( share/*jobOptions*.py )
atlas_install_python_modules( python/*.py POST_BUILD_CMD ${ATLAS_FLAKE8} )

# unit tests
