# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( InDetSLHC_Example )

# Install files from the package:
atlas_install_python_modules( python/*.py POST_BUILD_CMD ${ATLAS_FLAKE8} )
atlas_install_joboptions( share/*.py )
atlas_install_runtime( data/*.txt data/*.xml test/InDetSLHC_Example_TestConfiguration.xml scripts/make*.C scripts/IDPerf*.py ExtraFiles/ALL*.html )
atlas_install_xmls( data/*.xml )
