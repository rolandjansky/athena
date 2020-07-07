################################################################################
# Package: TileL2Algs
################################################################################

# Declare the package name:
atlas_subdir( TileL2Algs )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          Control/AthenaBaseComps
                          GaudiKernel
                          TileCalorimeter/TileEvent
                          TileCalorimeter/TileIdentifier
                          PRIVATE
                          Calorimeter/CaloDetDescr
                          Calorimeter/CaloIdentifier
                          Control/AthenaKernel
                          TileCalorimeter/TileCalib/TileCalibBlobObjs
                          TileCalorimeter/TileConditions
                          TileCalorimeter/TileDetDescr )

# Component(s) in the package:
atlas_add_library( TileL2AlgsLib
                   src/*.cxx
                   PUBLIC_HEADERS TileL2Algs
                   LINK_LIBRARIES AthenaBaseComps GaudiKernel TileEvent TileIdentifier CaloDetDescrLib TileConditionsLib
                   PRIVATE_LINK_LIBRARIES CaloIdentifier AthenaKernel TileCalibBlobObjs TileDetDescr )

atlas_add_component( TileL2Algs
                     src/components/*.cxx
                     LINK_LIBRARIES TileL2AlgsLib )

# Install files from the package:
atlas_install_python_modules( python/*.py POST_BUILD_CMD ${ATLAS_FLAKE8} )
atlas_install_joboptions( share/*.py )

# Tests:
atlas_add_test( TileL2Config_test
                SCRIPT python -m TileL2Algs.TileL2Config
                PROPERTIES TIMEOUT 300
                POST_EXEC_SCRIPT nopost.sh)
