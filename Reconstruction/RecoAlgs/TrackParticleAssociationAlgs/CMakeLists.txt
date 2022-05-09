# Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( TrackParticleAssociationAlgs )

# Component(s) in the package:
atlas_add_component( TrackParticleAssociationAlgs
                     src/*.cxx
                     src/components/*.cxx
                     LINK_LIBRARIES AthContainers AthenaBaseComps CaloUtilsLib FourMomUtils GaudiKernel RecoToolInterfaces StoreGateLib TrackToCaloLib TrackVertexAssociationToolLib TrkCaloExtension xAODAssociations xAODCaloEvent xAODTracking )

# Install files from the package:
atlas_install_python_modules( python/*.py POST_BUILD_CMD ${ATLAS_FLAKE8} )
