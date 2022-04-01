################################################################################
# Package: TrigHTTConfTools
################################################################################

# Declare the package name:
atlas_subdir( TrigHTTConfTools )

# External dependencies:
find_package( ROOT COMPONENTS Core Tree MathCore Hist RIO pthread Physics)
find_package( Boost )

# Component(s) in the package:
atlas_add_library(
  TrigHTTConfToolsLib    src/*.cxx TrigHTTConfTools/*.h
  PUBLIC_HEADERS      TrigHTTConfTools
  INCLUDE_DIRS        ${ROOT_INCLUDE_DIRS} ${Boost_INCLUDE_DIRS}
  LINK_LIBRARIES      ${ROOT_LIBRARIES} ${Boost_LIBRARIES}
                      AthContainers AthenaBaseComps AthenaKernel TrigHTTObjectsLib PathResolver
)

atlas_add_component(
  TrigHTTConfTools    src/components/*.cxx
  LINK_LIBRARIES      TrigHTTConfToolsLib 
)

# Install files from the package:
atlas_install_python_modules( python/*.py )
atlas_install_scripts( test/InputTestSetup.sh )

atlas_install_generic(  config/map_file/*
  DESTINATION           share/htt_configuration/map_files
)

atlas_add_test( HTTRegionSlices_test
    SOURCES         test/HTTRegionSlices_test.cxx
    LINK_LIBRARIES  TrigHTTConfToolsLib
)
 
atlas_add_test( HTTEventSelectionSvc_test
                SCRIPT python -m TrigHTTConfTools.HTTConfigCompInit
                POST_EXEC_SCRIPT noerror.sh )
