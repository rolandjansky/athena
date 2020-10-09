# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( LArRecEvent )

# Component(s) in the package:
atlas_add_library( LArRecEvent
                   src/*.cxx
                   PUBLIC_HEADERS LArRecEvent
                   LINK_LIBRARIES CaloEvent CaloIdentifier AthContainers AthenaKernel Identifier
                   PRIVATE_LINK_LIBRARIES FourMomUtils )

atlas_add_dictionary( LArRecEventDict
                      LArRecEvent/LArRecEventDict.h
                      LArRecEvent/selection.xml
                      LINK_LIBRARIES LArRecEvent )
