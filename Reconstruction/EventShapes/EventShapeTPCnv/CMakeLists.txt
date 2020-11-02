# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( EventShapeTPCnv )

# Component(s) in the package:
atlas_add_library( EventShapeTPCnv
                   src/*.cxx
                   PUBLIC_HEADERS EventShapeTPCnv
                   LINK_LIBRARIES EventShapeEvent AthenaPoolCnvSvcLib
                   PRIVATE_LINK_LIBRARIES DataModelAthenaPoolLib )

atlas_add_dictionary( EventShapeTPCnvDict
                      EventShapeTPCnv/EventShapeTPCnvDict.h
                      EventShapeTPCnv/selection.xml
                      LINK_LIBRARIES EventShapeTPCnv )
