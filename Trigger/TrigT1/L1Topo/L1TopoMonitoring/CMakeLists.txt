# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( L1TopoMonitoring )

# Component(s) in the package:
atlas_add_component( L1TopoMonitoring
                     src/*.cxx
                     src/components/*.cxx
                     LINK_LIBRARIES AthenaBaseComps L1TopoConfig L1TopoRDO TrigConfInterfaces TrigT1Result )

# Install files from the package:
atlas_install_joboptions( share/*.py )

