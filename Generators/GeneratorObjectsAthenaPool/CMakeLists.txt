# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( GeneratorObjectsAthenaPool )

# Component(s) in the package:
atlas_add_poolcnv_library( GeneratorObjectsAthenaPoolPoolCnv
                           src/*.cxx
                           FILES GeneratorObjects/HijingEventParams.h GeneratorObjects/McEventCollection.h
                           LINK_LIBRARIES AthenaPoolUtilities AthenaPoolCnvSvcLib GeneratorObjects GeneratorObjectsTPCnv )

# Install files from the package:
atlas_install_joboptions( share/*.py )

