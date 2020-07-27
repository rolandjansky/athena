################################################################################
# Package: ISF_FastCaloSimServices
################################################################################

# Declare the package name:
atlas_subdir( ISF_FastCaloSimServices )

# External dependencies:
find_package( CLHEP )
find_package(lwtnn)

# Component(s) in the package:
atlas_add_component( ISF_FastCaloSimServices
                     src/*.cxx
                     src/components/*.cxx
                     INCLUDE_DIRS ${CLHEP_INCLUDE_DIRS}  ${LWTNN_INCLUDE_DIRS}
                     LINK_LIBRARIES ${CLHEP_LIBRARIES} AtlasHepMCLib ${LWTNN_LIBRARIES} AthenaBaseComps AthenaKernel GaudiKernel IdDictParser ISF_InterfacesLib TrkEventPrimitives TrkExInterfaces CaloEvent StoreGateLib SGtests NavFourMom GeneratorObjects FastCaloSimLib ISF_Event ISF_FastCaloSimEvent ISF_FastCaloSimInterfaces ISF_FastCaloSimParametrizationLib PathResolver)

# Install files from the package:
atlas_install_python_modules( python/*.py )

atlas_add_test( ISF_FastCaloSimServices_Config_test
                SCRIPT test/FastCaloSimServices_test.py)