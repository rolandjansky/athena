# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( xAODTrigL1CaloCnv )

# Component(s) in the package:
atlas_add_library( xAODTrigL1CaloCnvLib
                   xAODTrigL1CaloCnv/*.h
                   INTERFACE
                   PUBLIC_HEADERS xAODTrigL1CaloCnv
                   LINK_LIBRARIES GaudiKernel xAODTrigL1Calo )

atlas_add_component( xAODTrigL1CaloCnv
                     src/*.cxx
                     src/components/*.cxx
                     LINK_LIBRARIES AthenaBaseComps AthenaKernel TrigT1CaloEventLib xAODTrigL1CaloCnvLib )

# Install files from the package:
atlas_install_python_modules( python/*.py )
atlas_install_joboptions( share/*.py )

