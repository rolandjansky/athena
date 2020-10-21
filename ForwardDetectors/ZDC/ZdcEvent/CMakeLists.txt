# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( ZdcEvent )

# Component(s) in the package:
atlas_add_library( ZdcEvent
                   src/*.cxx
                   PUBLIC_HEADERS ZdcEvent
                   LINK_LIBRARIES AthContainers AthenaKernel Identifier
                   PRIVATE_LINK_LIBRARIES ZdcConditions ZdcIdentifier )

atlas_add_dictionary( ZdcEventDict
                      ZdcEvent/ZdcEventDict.h
                      ZdcEvent/selection.xml
                      LINK_LIBRARIES AthContainers ZdcEvent )
