# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( tauEventTPCnv )

# External dependencies:
find_package( CLHEP )

# Component(s) in the package:
atlas_add_tpcnv_library( tauEventTPCnv
                         src/*.cxx
                         PUBLIC_HEADERS tauEventTPCnv
                         INCLUDE_DIRS ${CLHEP_INCLUDE_DIRS}
                         DEFINITIONS ${CLHEP_DEFINITIONS}
                         LINK_LIBRARIES ${CLHEP_LIBRARIES} DataModelAthenaPoolLib AthenaPoolCnvSvcLib AthenaPoolUtilities RootConversions EventCommonTPCnv ParticleEventTPCnv tauEvent TrkEventTPCnv
                         PRIVATE_LINK_LIBRARIES AthenaKernel )

atlas_add_dictionary( tauEventTPCnvDict
                      tauEventTPCnv/tauEventTPCnvDict.h
                      tauEventTPCnv/selection.xml
                      LINK_LIBRARIES tauEventTPCnv )
