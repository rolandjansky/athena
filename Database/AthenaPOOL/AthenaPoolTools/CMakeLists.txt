# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( AthenaPoolTools )

# Component(s) in the package:
atlas_add_component( AthenaPoolTools
                     src/EventCount.cxx
                     src/MetadataTest.cxx
                     src/RequireUniqueEvent.cxx
                     src/components/*.cxx
                     LINK_LIBRARIES AthenaBaseComps AthenaKernel ByteStreamData EventInfo GaudiKernel PersistentDataModel StoreGateLib xAODCutFlow xAODEventInfo )

# Install files from the package:
atlas_install_joboptions( share/*.py )
atlas_install_runtime( share/PoolMove )

