# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( AthExFortranAlgorithm )

# Component(s) in the package:
atlas_add_component( AthExFortranAlgorithm
                     src/*.cxx
                     src/*.F
                     src/components/*.cxx
                     LINK_LIBRARIES AthenaBaseComps GaudiKernel )

# Install files from the package:
atlas_install_joboptions( share/FortranAlgorithmOptions.py )
atlas_install_runtime( share/FortranAlgorithmInput.data )
atlas_install_scripts( share/preFortAlgEx.sh )
