# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( JetEventTPCnv )

# External dependencies:
find_package( Boost COMPONENTS filesystem thread system )

# Component(s) in the package:
atlas_add_tpcnv_library( JetEventTPCnv
                         src/*.cxx
                         PUBLIC_HEADERS JetEventTPCnv
                         INCLUDE_DIRS ${Boost_INCLUDE_DIRS}
                         LINK_LIBRARIES ${Boost_LIBRARIES} AthenaPoolCnvSvcLib AthenaPoolUtilities DataModelAthenaPoolLib EventCommonTPCnv JetEvent ParticleEventTPCnv
                         PRIVATE_LINK_LIBRARIES AthAllocators AthenaKernel MuonIDEvent VxVertex egammaEvent )

atlas_add_dictionary( JetEventTPCnvDict
                      JetEventTPCnv/JetEventTPCnvDict.h
                      JetEventTPCnv/selection.xml
                      LINK_LIBRARIES JetEventTPCnv )

atlas_add_dictionary( OLD_JetEventTPCnvDict
                      JetEventTPCnv/JetEventTPCnvDict.h
                      JetEventTPCnv/OLD_selection.xml
                      LINK_LIBRARIES JetEventTPCnv )
