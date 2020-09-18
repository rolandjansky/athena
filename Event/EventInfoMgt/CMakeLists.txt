# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( EventInfoMgt )

atlas_add_library( EventInfoMgtLib
                   INTERFACE
                   PUBLIC_HEADERS EventInfoMgt
                   LINK_LIBRARIES AthenaKernel GaudiKernel )

# Component(s) in the package:
atlas_add_component( EventInfoMgt
                     src/TagInfoMgr.cxx
                     src/EventInfoMgt_entries.cxx
                     LINK_LIBRARIES AthenaBaseComps AthenaPoolUtilities EventInfo EventInfoMgtLib EventInfoUtils IOVDbMetaDataToolsLib SGTools StoreGateLib )

# Install files from the package:
atlas_install_python_modules( python/*.py POST_BUILD_CMD ${ATLAS_FLAKE8} )
atlas_install_joboptions( share/EventInfoMgt_jobOptions.py )
atlas_install_scripts( share/dumpRunNumber.py )

atlas_add_test( TagInfoMgrCfg
                SCRIPT python -m EventInfoMgt.TagInfoMgrConfig
                PROPERTIES TIMEOUT 300
                POST_EXEC_SCRIPT nopost.sh )
