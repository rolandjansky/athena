# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( AssociationComps )

# Component(s) in the package:
atlas_add_component( AssociationComps
                     src/*.cxx
                     src/components/*.cxx
                     LINK_LIBRARIES CaloEvent AthenaKernel AthenaBaseComps AthContainers AthLinks Navigation EventKernel FourMomUtils NavFourMom GaudiKernel AssociationKernel JetEvent muonEvent egammaEvent tauEvent TrigObjectMatchingLib TrigCaloEvent TrigMuonEvent TrigParticle )

# Install files from the package:
atlas_install_joboptions( share/*.py )

