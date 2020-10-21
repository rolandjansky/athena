# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( iPatRecEvent )

# Component(s) in the package:
atlas_add_library( iPatRecEvent
                   src/iPatTrackContainer.cxx
                   PUBLIC_HEADERS iPatRecEvent
                   LINK_LIBRARIES AthContainers AthenaKernel iPatTrack )

atlas_add_dictionary( iPatTrackContainerDict
                      iPatRecEvent/iPatTrackContainerDict.h
                      iPatRecEvent/selection.xml
                      LINK_LIBRARIES iPatRecEvent )
