# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

# Declare the name of this package:
atlas_subdir( TopExamples )

# This package uses ROOT:
find_package( ROOT REQUIRED COMPONENTS Core Hist RIO Tree )

# Install the Python modules of the package:
atlas_install_python_modules( python/*.py )

# Install data files from the package:
atlas_install_data( share/* )

# Build the executables of the package:
atlas_add_executable( combineATOutputs
                      util/combineATOutputs.cxx
                      INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
                      LINK_LIBRARIES ${ROOT_LIBRARIES} )

atlas_install_scripts( scripts/* )
