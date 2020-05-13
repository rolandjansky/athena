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
                   LINK_LIBRARIES AthenaBaseComps GaudiKernel CaloInterfaceLib LArSimEvent TileSimEvent StoreGateLib SGtests PileUpToolsLib TileConditionsLib
                   PRIVATE_LINK_LIBRARIES CaloEvent CaloIdentifier EventInfo TileEvent )

atlas_add_component( FastCaloSimHit
                     src/components/*.cxx
                     LINK_LIBRARIES AthenaBaseComps StoreGateLib SGtests GaudiKernel LArSimEvent TileSimEvent CaloEvent CaloIdentifier PileUpToolsLib EventInfo TileConditionsLib TileEvent FastCaloSimHitLib )

# Install files from the package:
atlas_install_python_modules( python/*.py POST_BUILD_CMD ${ATLAS_FLAKE8} )
atlas_install_joboptions( share/*.py )

atlas_add_test( flake8_share
                SCRIPT flake8 --select=ATL,F,E7,E9,W6 --ignore=F401,F821,ATL900 ${CMAKE_CURRENT_SOURCE_DIR}/share
                POST_EXEC_SCRIPT nopost.sh )
