# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( xAODEventInfoCnv )

# Optional dependencies:
set( extra_libs )
if( NOT SIMULATIONBASE AND NOT GENERATIONBASE AND NOT XAOD_ANALYSIS )
   set( extra_libs BeamSpotConditionsData LumiBlockData )
endif()

# Component(s) in the package:
atlas_add_component( xAODEventInfoCnv
   src/*.h src/*.cxx src/components/*.cxx
   LINK_LIBRARIES xAODCnvInterfaces  AthenaBaseComps AthenaKernel
   AthenaPoolUtilities EventInfo xAODEventInfo xAODTruth GaudiKernel
   ${extra_libs} )

# Install files from the package:
atlas_install_python_modules( python/*.py
                              POST_BUILD_CMD ${ATLAS_FLAKE8} )
atlas_install_joboptions( share/*.py )

# Setup and run tests
set( _extraFlags )
if( SIMULATIONBASE OR GENERATIONBASE OR XAOD_ANALYSIS )
   set( _extraFlags --noBeamSpot )
endif()
atlas_add_test( EvenInfoCnvConfigTest
                SCRIPT test/EventInfoCnv_test.py ${_extraFlags}
                PROPERTIES TIMEOUT 300 )

atlas_add_test( EvenInfoCnvConfigTestMT
                SCRIPT test/EventInfoCnv_test.py -n 25 -t 3 ${_extraFlags}
                PROPERTIES TIMEOUT 300 )

if( NOT SIMULATIONBASE AND NOT GENERATIONBASE AND NOT XAOD_ANALYSIS )
   atlas_add_test( EventInfoOverlayConfigTest
                   SCRIPT test/EventInfoOverlay_test.py
                   PROPERTIES TIMEOUT 300 )

   atlas_add_test( EventInfoOverlayConfigTestMT
                   SCRIPT test/EventInfoOverlay_test.py -n 25 -t 3
                   PROPERTIES TIMEOUT 300 )

   atlas_add_test( EventInfoOverlayLegacyConfigTest
                   SCRIPT test/EventInfoOverlayLegacy_test.py
                   PROPERTIES TIMEOUT 300 )

   atlas_add_test( EventInfoOverlayTest
                   SCRIPT athena.py xAODEventInfoCnv/EventInfoOverlayTest.py
                   PROPERTIES TIMEOUT 300 )

   atlas_add_test( EventInfoOverlayLegacyTest
                   SCRIPT athena.py xAODEventInfoCnv/EventInfoOverlayLegacyTest.py
                   PROPERTIES TIMEOUT 300 )
endif()
