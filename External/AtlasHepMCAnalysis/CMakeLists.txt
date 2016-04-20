# $Id: CMakeLists.txt 741754 2016-04-20 19:02:54Z krasznaa $
#
# CMake configuration file building HepMCAnalysis ad part of the
# offline build.
#

# The name of the package:
atlas_subdir( AtlasHepMCAnalysis )

# In release recompilation mode finish here:
if( ATLAS_RELEASE_MODE )
   return()
endif()

# LCG packages that the build depends on:
find_package( HepMC )
find_package( ROOT )

# Build HepMCAnalysis for the build area:
ExternalProject_Add( HepMCAnalysis
  PREFIX ${CMAKE_BINARY_DIR}
  URL http://cern.ch/service-spi/external/MCGenerators/distribution/hepmcanalysis/hepmcanalysis-3.4.14-src.tgz
  BUILD_IN_SOURCE 1
  PATCH_COMMAND ${CMAKE_COMMAND} -E copy_directory
  <SOURCE_DIR>/3.4.14/ <SOURCE_DIR>
  COMMAND ${CMAKE_COMMAND} -E remove_directory <SOURCE_DIR>/3.4.14
  COMMAND patch -p1 <
  ${CMAKE_CURRENT_SOURCE_DIR}/src/hepmcanalysis-3.4.14-src.patch
  CONFIGURE_COMMAND ${CMAKE_COMMAND} -E echo
  "No configuration step for HepMCAnalysis"
  BUILD_COMMAND make HepMCdir=${HEPMC_ROOT}
  FastJetdir=${CMAKE_BINARY_DIR}/${ATLAS_PLATFORM}
  ROOTSYS=${ROOT_ROOT}
  CLHEPdir=${CMAKE_BINARY_DIR}/${ATLAS_PLATFORM}
  INSTALL_COMMAND ${CMAKE_COMMAND} -E copy_directory lib/
  ${CMAKE_CURRENT_BINARY_DIR}/build/lib/
  COMMAND ${CMAKE_COMMAND} -E copy_directory lib/
  ${CMAKE_LIBRARY_OUTPUT_DIRECTORY}/
  COMMAND ${CMAKE_COMMAND} -E copy_directory include/
  ${CMAKE_CURRENT_BINARY_DIR}/build/include/
  COMMAND ${CMAKE_COMMAND} -E copy_directory include/
  ${CMAKE_INCLUDE_OUTPUT_DIRECTORY}/ )
add_dependencies( HepMCAnalysis CLHEP FastJet )
add_dependencies( Package_AtlasHepMCAnalysis HepMCAnalysis )

# Install HepMCAnalysis:
install( DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/build/
  DESTINATION . USE_SOURCE_PERMISSIONS OPTIONAL )
