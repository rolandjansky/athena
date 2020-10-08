# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( MuonEfficiencyCorrections )

# External dependencies:
find_package( ROOT COMPONENTS Core Tree RIO Hist Physics )

# Libraries in the package:
atlas_add_library( MuonEfficiencyCorrectionsLib
   MuonEfficiencyCorrections/*.h Root/*.cxx
   PUBLIC_HEADERS MuonEfficiencyCorrections
   INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
   LINK_LIBRARIES ${ROOT_LIBRARIES} AsgTools xAODMuon xAODEventInfo MuonAnalysisInterfacesLib PATInterfaces AsgDataHandlesLib
   PRIVATE_LINK_LIBRARIES FourMomUtils xAODTrigger PathResolver )

if( NOT XAOD_STANDALONE )
   atlas_add_component( MuonEfficiencyCorrections
      src/*.h src/*.cxx src/components/*.cxx
      LINK_LIBRARIES AsgAnalysisInterfaces AthContainers AthenaBaseComps FourMomUtils GaudiKernel MuonEfficiencyCorrectionsLib StoreGateLib xAODJet )
endif()

atlas_add_dictionary( MuonEfficiencyCorrectionsDict
   MuonEfficiencyCorrections/MuonEfficiencyCorrectionsDict.h
   MuonEfficiencyCorrections/selection.xml
   LINK_LIBRARIES MuonEfficiencyCorrectionsLib )

# Executable(s) in the package:
macro( _add_exec name )
   atlas_add_executable( ${name}
      util/${name}.cxx
      LINK_LIBRARIES AsgAnalysisInterfaces MuonEfficiencyCorrectionsLib )
endmacro( _add_exec )

if( XAOD_STANDALONE )
   _add_exec( MuonEfficiencyScaleFactorsTest )
   _add_exec( MuonEfficiencyScaleFactorsSFFileTester )
   _add_exec( MuonTriggerSFRootCoreTest )
   _add_exec( MuonTriggerSFFilesTest )
   _add_exec( MuonTriggerSFConfGenerator )
endif()

# Install files from the package:
atlas_install_joboptions( share/*.py )
atlas_install_python_modules( python/*.py )
