# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( PrimaryDPDMaker )

# External dependencies:
find_package( ROOT COMPONENTS Core MathCore )

# Component(s) in the package:
atlas_add_library( PrimaryDPDMakerLib
                   src/*.cxx
                   PUBLIC_HEADERS PrimaryDPDMaker
                   PRIVATE_INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
                   LINK_LIBRARIES AthenaBaseComps DerivationFrameworkInterfaces GaudiKernel MuonAnalysisInterfacesLib StoreGateLib TrkToolInterfaces TrkTrack
                   PRIVATE_LINK_LIBRARIES ${ROOT_LIBRARIES} CxxUtils EventKernel LArRecEvent NavFourMom TileEvent TileIdentifier xAODEventInfo xAODMuon xAODTracking )

atlas_add_component( PrimaryDPDMaker
                     src/components/*.cxx
                     LINK_LIBRARIES PrimaryDPDMakerLib )

atlas_add_dictionary( PrimaryDPDMakerDict
                      PrimaryDPDMaker/PrimaryDPDMakerDict.h
                      PrimaryDPDMaker/selection.xml
                      LINK_LIBRARIES PrimaryDPDMakerLib )

# Install files from the package:
atlas_install_python_modules( python/*.py POST_BUILD_CMD ${ATLAS_FLAKE8} )
atlas_install_joboptions( share/*.py )
