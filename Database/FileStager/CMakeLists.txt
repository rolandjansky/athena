# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( FileStager )

# External dependencies:
find_package( ROOT COMPONENTS Hist Tree RIO Core )

# Component(s) in the package:
atlas_add_root_dictionary( FileStagerLib
                           FileStagerLibDictSource
                           ROOT_HEADERS FileStager/TStagerInfo.h FileStager/TStageFileInfo.h FileStager/TStageManager.h FileStager/TCopyChain.h FileStager/TCopyFile.h Root/LinkDef.h
                           EXTERNAL_PACKAGES ROOT )

atlas_add_library( FileStagerLib
                   Root/*.cxx
                   src/*.cxx
                   ${FileStagerLibDictSource}
                   PUBLIC_HEADERS FileStager
                   INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
                   LINK_LIBRARIES ${ROOT_LIBRARIES} GaudiKernel )

atlas_add_component( FileStager
                     src/components/*.cxx
                     LINK_LIBRARIES FileStagerLib )

atlas_add_executable( StageMonitor
                      bin/StageMonitor.cxx
                      LINK_LIBRARIES FileStagerLib )

# Install files from the package:
atlas_install_python_modules( python/*.py )
atlas_install_joboptions( share/*.py )
atlas_install_scripts( scripts/define_* scripts/interpret_* scripts/wrapper_* )

