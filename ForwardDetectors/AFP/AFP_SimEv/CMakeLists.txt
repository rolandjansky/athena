# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( AFP_SimEv )

# Component(s) in the package:
atlas_add_library( AFP_SimEv
                   src/*.cxx
                   PUBLIC_HEADERS AFP_SimEv
                   LINK_LIBRARIES AthenaKernel HitManagement )

atlas_add_dictionary( AFP_SimEvDict
                      AFP_SimEv/AFP_SimEvDict.h
                      AFP_SimEv/selection.xml
                      LINK_LIBRARIES AFP_SimEv )
