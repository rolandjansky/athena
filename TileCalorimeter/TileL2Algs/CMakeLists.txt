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
                     LINK_LIBRARIES AthenaBaseComps GaudiKernel TileEvent TileIdentifier CaloDetDescrLib CaloIdentifier AthenaKernel TileCalibBlobObjs TileConditionsLib TileDetDescr TileL2AlgsLib )

# Install files from the package:
atlas_install_python_modules( python/*.py )
atlas_install_joboptions( share/*.py )

