# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( RecExRecoTest )

# Install files from the package:
atlas_install_joboptions( share/*.py )
atlas_install_runtime( test/*.sh )

#Make test scripts available
atlas_install_scripts( test/test*.sh )

atlas_install_scripts( test/MetaDataTest.py )
atlas_add_alias( metadata-test "MetaDataTest.py" )
