# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( xAODTrigCaloCnv )

atlas_add_library( xAODTrigCaloCnvLib
                   xAODTrigCaloCnv/*.h
                   INTERFACE
                   PUBLIC_HEADERS xAODTrigCaloCnv
                   LINK_LIBRARIES GaudiKernel xAODTrigCalo )

# Component(s) in the package:
atlas_add_component( xAODTrigCaloCnv
                     src/*.cxx
                     src/components/*.cxx
                     LINK_LIBRARIES AthenaBaseComps AthenaKernel TrigCaloEvent xAODTrigCaloCnvLib )

# Install files from the package:
atlas_install_python_modules( python/*.py )
atlas_install_joboptions( share/*.py )

