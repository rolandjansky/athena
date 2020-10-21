# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( TRT_RecoConditionsServices )

# Component(s) in the package:
atlas_add_component( TRT_RecoConditionsServices
                     src/*.cxx src/components/*.cxx
                     LINK_LIBRARIES GaudiKernel AthenaBaseComps StoreGateLib InDetIdentifier TRT_ReadoutGeometry TRT_ConditionsServicesLib InDetRecToolInterfaces )
