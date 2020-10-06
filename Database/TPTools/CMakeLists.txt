# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( TPTools )

# Component(s) in the package:
atlas_add_library( TPTools
                   src/*.cxx
                   PUBLIC_HEADERS TPTools
                   LINK_LIBRARIES AthenaKernel AthenaPoolUtilities CxxUtils GaudiKernel
                   PRIVATE_LINK_LIBRARIES PersistentDataModel )

atlas_add_dictionary( TPToolsDict
                      TPTools/TPToolsDict.h
                      TPTools/selection.xml
                      LINK_LIBRARIES TPTools )
