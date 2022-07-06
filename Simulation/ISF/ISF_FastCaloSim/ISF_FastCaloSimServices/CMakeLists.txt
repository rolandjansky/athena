################################################################################
# Package: ISF_FastCaloSimServices
################################################################################

# Declare the package name:
atlas_subdir( ISF_FastCaloSimServices )

# External dependencies:
find_package( CLHEP )
find_package(lwtnn)

if(USE_GPU AND CMAKE_CUDA_COMPILER)
# add an option switch on the GPU
  add_definitions(-DUSE_GPU)
  message("compiling the ISF_FastCaloSimServices with cuda")
  # Component(s) in the package:
  atlas_add_component( ISF_FastCaloSimServices
    src/*.cxx
    src/components/*.cxx
    INCLUDE_DIRS ${CLHEP_INCLUDE_DIRS}  ${LWTNN_INCLUDE_DIRS}
    LINK_LIBRARIES ${CLHEP_LIBRARIES} AtlasHepMCLib ${LWTNN_LIBRARIES} AthenaBaseComps AthenaKernel GaudiKernel IdDictParser ISF_InterfacesLib TrkEventPrimitives TrkExInterfaces CaloEvent StoreGateLib NavFourMom GeneratorObjects FastCaloSimLib ISF_Event ISF_FastCaloSimEvent ISF_FastCaloSimInterfaces ISF_FastCaloSimParametrizationLib PathResolver ISF_FastCaloGpuLib AthCUDACoreLib)
else()
  # Component(s) in the package:
  atlas_add_component( ISF_FastCaloSimServices
    src/*.cxx
    src/components/*.cxx
    INCLUDE_DIRS ${CLHEP_INCLUDE_DIRS}  ${LWTNN_INCLUDE_DIRS}
    LINK_LIBRARIES ${CLHEP_LIBRARIES} AtlasHepMCLib ${LWTNN_LIBRARIES} AthenaBaseComps AthenaKernel GaudiKernel IdDictParser ISF_InterfacesLib TrkEventPrimitives TrkExInterfaces CaloEvent StoreGateLib NavFourMom GeneratorObjects FastCaloSimLib ISF_Event ISF_FastCaloSimEvent ISF_FastCaloSimInterfaces ISF_FastCaloSimParametrizationLib PathResolver)
endif()

# Install files from the package:
atlas_install_python_modules( python/*.py POST_BUILD_CMD ${ATLAS_FLAKE8} )

atlas_add_test( ISF_FastCaloSimServices_Config_test
                SCRIPT test/FastCaloSimServicesTest.py -n 3
                PRIVATE_WORKING_DIRECTORY
                POST_EXEC_SCRIPT nopost.sh
                PROPERTIES TIMEOUT 2000)
