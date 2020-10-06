# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( CaloTests )

# External dependencies:
find_package( AIDA )

# Component(s) in the package:
atlas_add_component( CaloTests
                     src/*.cxx
                     src/components/*.cxx
                     INCLUDE_DIRS ${AIDA_INCLUDE_DIRS}
                     LINK_LIBRARIES AtlasHepMCLib CaloDetDescrLib CaloIdentifier AthenaBaseComps StoreGateLib SGtests Identifier GaudiKernel CaloEvent CaloGeoHelpers CaloSimEvent AthenaKernel AtlasDetDescr GeneratorObjects LArIdentifier LArRawEvent LArRawUtilsLib LArSimEvent LArCablingLib )

# Install files from the package:
atlas_install_joboptions( share/*.py )
atlas_install_runtime( test/CaloTests_TestConfiguration.xml share/*.C share/rttTest.css share/CaloTests_HistoComparison.txt share/*Checks.py )

