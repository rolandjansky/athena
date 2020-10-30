# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( xAODCaloEventCnv )

# Component(s) in the package:
atlas_add_library( xAODCaloEventCnvLib
                   xAODCaloEventCnv/*.h
                   INTERFACE
                   PUBLIC_HEADERS xAODCaloEventCnv
                   LINK_LIBRARIES GaudiKernel xAODCaloEvent )

# Component(s) in the package:
atlas_add_component( xAODCaloEventCnv
                     src/*.cxx
                     src/components/*.cxx
                     LINK_LIBRARIES xAODCaloEventCnvLib xAODCaloEvent CaloEvent AthenaBaseComps AthenaKernel GaudiKernel )

# Install files from the package:
atlas_install_joboptions( share/*.py )

