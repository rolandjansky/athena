# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( AFP_DigiEv )

# Component(s) in the package:
atlas_add_library( AFP_DigiEv
                   src/*.cxx
                   PUBLIC_HEADERS AFP_DigiEv
                   LINK_LIBRARIES AthContainers AthenaKernel HitManagement )

atlas_add_dictionary( AFP_DigiEvDict
                      AFP_DigiEv/AFP_DigiEvDict.h
                      AFP_DigiEv/selection.xml
                      LINK_LIBRARIES AFP_DigiEv )
