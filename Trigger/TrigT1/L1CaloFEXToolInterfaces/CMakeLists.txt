# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( L1CaloFEXToolInterfaces )

atlas_add_library( L1CaloFEXToolInterfaces
                   L1CaloFEXToolInterfaces/*.h
                   INTERFACE
                   PUBLIC_HEADERS L1CaloFEXToolInterfaces
                   LINK_LIBRARIES AthenaBaseComps CaloEvent CaloIdentifier GaudiKernel L1CaloFEXSimLib xAODTrigL1Calo )
