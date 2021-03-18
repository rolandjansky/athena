################################################################################
# Package: TrigT1CaloFexPerf
################################################################################

# Declare the package name:
atlas_subdir( TrigT1CaloFexPerf )

# External dependencies:
find_package( Boost REQUIRED )
find_package( ROOT COMPONENTS MathCore )


# athena library for the package:
atlas_add_component( TrigT1CaloFexPerf
                      src/*.cxx src/components/*.cxx
                      INCLUDE_DIRS ${ROOT_INCLUDE_DIRS} ${Boost_INCLUDE_DIRS} 
                      LINK_LIBRARIES ${Boost_LIBRARIES}  
                      AthenaBaseComps CaloEvent CaloIdentifier
                      TileEvent xAODTrigCalo xAODTrigger xAODTrigL1Calo
                      )


atlas_install_python_modules( python/*.py POST_BUILD_CMD ${ATLAS_FLAKE8} )
