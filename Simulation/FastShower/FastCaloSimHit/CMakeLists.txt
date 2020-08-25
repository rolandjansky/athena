################################################################################
# Package: FastCaloSimHit
################################################################################

# Declare the package name:
atlas_subdir( FastCaloSimHit )

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
                     LINK_LIBRARIES FastCaloSimHitLib )

# Install files from the package:
atlas_install_python_modules( python/*.py POST_BUILD_CMD ${ATLAS_FLAKE8} )
atlas_install_joboptions( share/*.py POST_BUILD_CMD ${ATLAS_FLAKE8} --extend-ignore=F401,F821 )
