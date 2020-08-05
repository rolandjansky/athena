# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( xAODTrigMuonCnv )

atlas_add_library( xAODTrigMuonCnvLib
                   xAODTringMuonCnv/*.h
                   INTERFACE
                   PUBLIC_HEADERS xAODTrigMuonCnv
                   LINK_LIBRARIES GaudiKernel xAODTrigMuon xAODMuon xAODTracking )

# Component(s) in the package:
atlas_add_component( xAODTrigMuonCnv
                     src/*.cxx
                     src/components/*.cxx
                     LINK_LIBRARIES AthLinks AthenaBaseComps AthenaKernel FourMomUtils TrigMuonEvent xAODTrigMuonCnvLib )

# Install files from the package:
atlas_install_python_modules( python/xAODTrigMuonCnvConfig.py python/xAODTrigMuonCnvConfigDb.py )
atlas_install_joboptions( share/addEFMuonxAODCnv_jobOptions.py share/runEFMuonAODCnvAlg_topOptions.py )

