# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( OutputStreamAthenaPool )

# Component(s) in the package:
atlas_add_component( OutputStreamAthenaPool
                     src/MakeEventStreamInfo.cxx
                     src/CopyEventStreamInfo.cxx
                     src/MakeInputDataHeader.cxx
                     src/EventInfoAttListTool.cxx
                     src/EventInfoTagBuilder.cxx
                     src/components/*.cxx
                     LINK_LIBRARIES ${CORAL_LIBRARIES} AthenaBaseComps AthenaKernel EventInfo EventInfoUtils GaudiKernel PersistentDataModel StoreGateLib xAODEventInfo )

# Install files from the package:
atlas_install_python_modules( python/*.py POST_BUILD_CMD ${ATLAS_FLAKE8} )
atlas_install_joboptions( share/*.py )
