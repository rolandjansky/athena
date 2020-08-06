# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( xAODTrigBphysCnv )

atlas_add_library( xAODTrigBphysCnvLib
                   xAODTrigBphysCnv/*.h
                   INTERFACE
                   PUBLIC_HEADERS xAODTrigBphysCnv
                   LINK_LIBRARIES GaudiKernel xAODTrigBphys )

# Component(s) in the package:
atlas_add_component( xAODTrigBphysCnv
                     src/*.cxx
                     src/components/*.cxx
                     LINK_LIBRARIES AthenaBaseComps AthenaKernel TrigParticle xAODTrigBphysCnvLib )

# Install files from the package:
atlas_install_joboptions( share/*.py )
