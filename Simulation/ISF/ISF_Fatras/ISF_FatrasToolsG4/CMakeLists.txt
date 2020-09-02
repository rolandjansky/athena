################################################################################
# Package: ISF_FatrasToolsG4
################################################################################

# Declare the package name:
atlas_subdir( ISF_FatrasToolsG4 )

# External dependencies:
find_package( Boost COMPONENTS filesystem thread system )
find_package( CLHEP )
find_package( Geant4 )
find_package( ROOT COMPONENTS Core Tree MathCore Hist RIO pthread )
find_package( XercesC )

# Component(s) in the package:
atlas_add_component( ISF_FatrasToolsG4
                     src/*.cxx
                     src/components/*.cxx
                     INCLUDE_DIRS ${ROOT_INCLUDE_DIRS} ${Boost_INCLUDE_DIRS} ${XERCESC_INCLUDE_DIRS} ${CLHEP_INCLUDE_DIRS}  ${GEANT4_INCLUDE_DIRS}
                     LINK_LIBRARIES ${ROOT_LIBRARIES} ${Boost_LIBRARIES} ${XERCESC_LIBRARIES} ${CLHEP_LIBRARIES} AtlasHepMCLib ${GEANT4_LIBRARIES} AthenaBaseComps AthenaKernel GaudiKernel ISF_Event ISF_FatrasInterfaces TrkEventPrimitives StoreGateLib SGtests ISF_InterfacesLib ISF_Geant4ToolsLib TrkGeometry )

