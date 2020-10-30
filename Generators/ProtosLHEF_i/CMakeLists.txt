# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( ProtosLHEF_i )

# Component(s) in the package:
atlas_add_component( ProtosLHEF_i
                     src/*.cxx
                     src/*.F
                     src/components/*.cxx
                     LINK_LIBRARIES AthenaBaseComps GaudiKernel GeneratorFortranCommonLib )

# Install files from the package:
atlas_install_headers( ProtosLHEF_i )
atlas_install_joboptions( share/*.py )
atlas_install_runtime( share/*.events share/*.dat )

