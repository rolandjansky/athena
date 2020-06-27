# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( TrigUpgradeTest )

atlas_add_component( TrigUpgradeTest
                     src/*.cxx
                     src/components/*.cxx
                     LINK_LIBRARIES AthenaBaseComps TrigSteeringEvent DecisionHandlingLib TrigCompositeUtilsLib
                     )

# Install files from the package:
atlas_install_joboptions( share/*.py )
atlas_install_data( share/*.ref share/*.conf )
atlas_install_python_modules( python/*.py POST_BUILD_CMD ${ATLAS_FLAKE8} )
atlas_install_scripts( test/exec*.sh test/test*.sh )
