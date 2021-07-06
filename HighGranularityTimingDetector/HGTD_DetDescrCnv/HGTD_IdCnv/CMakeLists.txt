# Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( HGTD_IdCnv )

# Component(s) in the package:
atlas_add_component( HGTD_IdCnv
                     src/*.cxx
                     LINK_LIBRARIES StoreGateLib SGtests DetDescrCnvSvcLib IdDictDetDescr GaudiKernel HGTD_Identifier AthenaBaseComps AthenaKernel )
