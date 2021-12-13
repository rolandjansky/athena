# Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration

# Minimum CMake requirement for the package.
cmake_minimum_required( VERSION 3.21 )

# Declare the package name:
atlas_subdir( QGSJet_i )

# External dependencies:
find_package( CLHEP )
find_package( Crmc COMPONENTS QgsjetII04 )

# Remove the --as-needed linker flags:
atlas_disable_as_needed()

# Component(s) in the package:
atlas_add_library( QGSJet_iLib
                   src/QGSJet.cxx
                   src/Ranfini.F
                   src/DRanf.F
                   src/Rangen.F
                   PUBLIC_HEADERS QGSJet_i
                   INCLUDE_DIRS ${CRMC_INCLUDE_DIRS}
                   PRIVATE_INCLUDE_DIRS ${CLHEP_INCLUDE_DIRS}
                   PRIVATE_DEFINITIONS ${CLHEP_DEFINITIONS}
                   LINK_LIBRARIES ${CRMC_LIBRARIES} GeneratorModulesLib
                   PRIVATE_LINK_LIBRARIES ${CLHEP_LIBRARIES} AtlasHepMCLib AtlasHepMCfioLib AthenaKernel GaudiKernel TruthUtils )

atlas_add_component( QGSJet_i
                     src/components/*.cxx
                     INCLUDE_DIRS ${CRMC_INCLUDE_DIRS} ${CLHEP_INCLUDE_DIRS}
                     LINK_LIBRARIES ${CRMC_LIBRARIES} ${CLHEP_LIBRARIES} AtlasHepMCLib AtlasHepMCfioLib GeneratorModulesLib AthenaKernel GaudiKernel TruthUtils QGSJet_iLib )

# Install files from the package:
atlas_install_runtime( share/file/qgsjet_crmc.param )

# Install files from Crmc.
foreach( _file "sectnu-II-04" "qgsdat-II-04.lzma" )
   unset( _datFile )
   find_file( _datFile "${_file}"
      PATHS "${CRMC_LCGROOT}"
      PATH_SUFFIXES "tabs" "share/crmc"
      NO_CACHE )
   if( _datFile )
      atlas_install_runtime( "${_datFile}" )
   else()
      message( WARNING "Could not find data file \"${_file}\"" )
   endif()
endforeach()

# Set up the runtime environment for the package.
set( QGSJetEnvironment_DIR ${CMAKE_CURRENT_SOURCE_DIR}
   CACHE PATH "Location of QGSJetEnvironment.cmake" )
find_package( QGSJetEnvironment )
