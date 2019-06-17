################################################################################
# Package: OverlayCommonAlgs
################################################################################

# Declare the package name:
atlas_subdir( OverlayCommonAlgs )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          GaudiKernel
                          PRIVATE
                          Calorimeter/CaloSimEvent
                          Control/AthenaBaseComps
                          Event/xAOD/xAODEventInfo
                          Event/xAOD/xAODJet
                          Generators/GeneratorObjects
                          Reconstruction/RecEvent
                          Simulation/G4Sim/TrackRecord )

# Component(s) in the package:
atlas_add_component( OverlayCommonAlgs
                     src/*.cxx
                     src/components/*.cxx
                     LINK_LIBRARIES GaudiKernel AthenaBaseComps
                     CaloSimEvent GeneratorObjects RecEvent
                     xAODEventInfo xAODJet )

# Install files from the package:
atlas_install_python_modules( python/*.py )
atlas_install_joboptions( share/*.py )

# Setup and run tests
atlas_add_test( EventInfoOverlayConfigTest
                SCRIPT test/EventInfoOverlay_test.py
                PROPERTIES TIMEOUT 300 )

atlas_add_test( EventInfoOverlayLegacyConfigTest
                SCRIPT test/EventInfoOverlayLegacy_test.py
                PROPERTIES TIMEOUT 300 )

atlas_add_test( EventInfoOverlayTest
                SCRIPT athena.py OverlayCommonAlgs/EventInfoOverlayTest.py
                PROPERTIES TIMEOUT 300 )

atlas_add_test( EventInfoOverlayTestMT
                SCRIPT athena.py OverlayCommonAlgs/EventInfoOverlayTest.py --threads=3
                PROPERTIES TIMEOUT 300 )

atlas_add_test( EventInfoOverlayLegacyTest
                SCRIPT athena.py OverlayCommonAlgs/EventInfoOverlayLegacyTest.py
                PROPERTIES TIMEOUT 300 )

atlas_add_test( EventInfoOverlayLegacyTestMT
                SCRIPT athena.py OverlayCommonAlgs/EventInfoOverlayLegacyTest.py --threads=3
                PROPERTIES TIMEOUT 300 )
