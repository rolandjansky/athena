# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( ToyConversion )

# Component(s) in the package:
atlas_add_library( ToyConversionLib
                   src/*.cxx
                   PUBLIC_HEADERS ToyConversion
                   LINK_LIBRARIES AthenaKernel GaudiKernel )

atlas_add_component( ToyConversion
                     src/components/*.cxx
                     LINK_LIBRARIES ToyConversionLib )

# Install files from the package:
atlas_install_joboptions( share/ToyConversionOpts.txt share/ToyConversionOpts.py )
