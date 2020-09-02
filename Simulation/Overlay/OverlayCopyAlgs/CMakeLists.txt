################################################################################
# Package: OverlayCopyAlgs
################################################################################

# Declare the package name:
atlas_subdir( OverlayCopyAlgs )

# Component(s) in the package:
atlas_add_component( OverlayCopyAlgs
                     src/*.cxx
                     src/components/*.cxx
                     LINK_LIBRARIES GaudiKernel AthenaBaseComps
                     CaloSimEvent GeneratorObjects RecEvent
                     TrackRecordLib xAODJet )

# Install files from the package:
atlas_install_python_modules( python/*.py
                              POST_BUILD_CMD ${ATLAS_FLAKE8} )

# Setup and run tests
atlas_add_test( OverlayCopyAlgsConfigTest
                SCRIPT test/OverlayCopyAlgs_test.py
                PROPERTIES TIMEOUT 300 )

atlas_add_test( OverlayCopyAlgsConfigTestMT
                SCRIPT test/OverlayCopyAlgs_test.py -n 20 -t 3
                PROPERTIES TIMEOUT 300 )
