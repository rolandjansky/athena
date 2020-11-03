# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( JetTagInfoAthenaPool )

# Component(s) in the package:
atlas_add_library( JetTagInfoAthenaPoolLib
                   JetTagInfoAthenaPool/*.h
                   INTERFACE
                   PUBLIC_HEADERS JetTagInfoAthenaPool )

atlas_add_poolcnv_library( JetTagInfoAthenaPoolPoolCnv
                           src/*.cxx
                           FILES JetTagInfoAthenaPool/JetTagInfo.h
                           LINK_LIBRARIES AthenaPoolCnvSvcLib JetTagInfoTPCnv JetTagInfoAthenaPoolLib )
