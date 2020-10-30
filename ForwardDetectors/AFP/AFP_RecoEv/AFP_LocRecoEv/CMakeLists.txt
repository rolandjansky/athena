# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( AFP_LocRecoEv )

# Component(s) in the package:
atlas_add_library( AFP_LocRecoEv
                   src/*.cxx
                   PUBLIC_HEADERS AFP_LocRecoEv
                   LINK_LIBRARIES AthContainers AthenaKernel )

atlas_add_dictionary( AFP_LocRecoEvDict
                      AFP_LocRecoEv/AFP_LocRecoEvDict.h
                      AFP_LocRecoEv/selection.xml
                      LINK_LIBRARIES AFP_LocRecoEv )
