################################################################################
# Package: FastCaloSimHit
################################################################################

# Declare the package name:
atlas_subdir( FastCaloSimHit )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          Calorimeter/CaloInterface
                          Control/AthenaBaseComps
                          Control/StoreGate
                          GaudiKernel
                          LArCalorimeter/LArElecCalib
                          LArCalorimeter/LArSimEvent
                          TileCalorimeter/TileSimEvent
                          PRIVATE
                          Calorimeter/CaloEvent
                          Calorimeter/CaloIdentifier
                          Control/CxxUtils
                          Control/PileUpTools
                          Event/EventInfo
                          TileCalorimeter/TileConditions
                          TileCalorimeter/TileEvent )

# this line failed automatic conversion in cmt2cmake :
# macro_append FastCaloSimHit_cppflags " -DFastCaloSimHit_project_release_v1=`echo $(FastCaloSimHit_project_release) | awk -F. '{printf("'"'%d'"'",$$1)}'` "

# this line failed automatic conversion in cmt2cmake :
# macro_append FastCaloSimHit_cppflags " -DFastCaloSimHit_project_release_v2=`echo $(FastCaloSimHit_project_release) | awk -F. '{printf("'"'%d'"'",$$2)}'` "

# this line failed automatic conversion in cmt2cmake :
# macro_append FastCaloSimHit_cppflags " -DFastCaloSimHit_project_release_v3=`echo $(FastCaloSimHit_project_release) | awk -F. '{printf("'"'%d'"'",$$3)}'` "

# Component(s) in the package:
atlas_add_library( FastCaloSimHitLib
                   src/*.cxx
                   PUBLIC_HEADERS FastCaloSimHit
                   LINK_LIBRARIES AthenaBaseComps GaudiKernel LArSimEvent TileSimEvent StoreGateLib SGtests PileUpToolsLib TileConditionsLib
                   PRIVATE_LINK_LIBRARIES CaloEvent CaloIdentifier CxxUtils EventInfo TileEvent )

atlas_add_component( FastCaloSimHit
                     src/components/*.cxx
                     LINK_LIBRARIES AthenaBaseComps StoreGateLib SGtests GaudiKernel LArSimEvent TileSimEvent CaloEvent CaloIdentifier CxxUtils PileUpToolsLib EventInfo TileConditionsLib TileEvent FastCaloSimHitLib )

# Install files from the package:
atlas_install_python_modules( python/*.py )
atlas_install_joboptions( share/*.py )

