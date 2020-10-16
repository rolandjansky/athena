# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( egammaEventTPCnv )

# Component(s) in the package:
atlas_add_tpcnv_library( egammaEventTPCnv
                         src/*.cxx
                         PUBLIC_HEADERS egammaEventTPCnv
                         LINK_LIBRARIES DataModelAthenaPoolLib AthenaPoolCnvSvcLib EventCommonTPCnv ParticleEventTPCnv egammaEvent
                         PRIVATE_LINK_LIBRARIES AthenaKernel GaudiKernel )

atlas_add_dictionary( egammaEventTPCnvDict
                      egammaEventTPCnv/egammaEventTPCnvDict.h
                      egammaEventTPCnv/selection.xml
                      LINK_LIBRARIES egammaEventTPCnv )

atlas_add_dictionary( OLD_egammaEventTPCnvDict
                      egammaEventTPCnv/egammaEventTPCnvDict.h
                      egammaEventTPCnv/OLD_selection.xml
                      LINK_LIBRARIES egammaEventTPCnv )
