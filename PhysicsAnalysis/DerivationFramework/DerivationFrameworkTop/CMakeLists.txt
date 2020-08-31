# Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration

################################################################################
# Package: DerivationFrameworkTop
################################################################################

# Declare the package name:
atlas_subdir( DerivationFrameworkTop )

# Declare the package's dependencies:
atlas_depends_on_subdirs(
   PUBLIC
   Control/AthenaBaseComps
   Event/xAOD/xAODTruth
   GaudiKernel
   PhysicsAnalysis/DerivationFramework/DerivationFrameworkInterfaces
   PRIVATE
   Control/AthenaKernel
   PhysicsAnalysis/CommonTools/ExpressionEvaluation
   PhysicsAnalysis/JetTagging/FlavorTagDiscriminants
   Event/xAOD/xAODJet
   Event/xAOD/xAODEventInfo
   Event/xAOD/xAODBTagging
   Event/xAOD/xAODTracking )

# Component(s) in the package:
atlas_add_library( DerivationFrameworkTopLib
   DerivationFrameworkTop/*.h src/*.cxx
   PUBLIC_HEADERS DerivationFrameworkTop
   LINK_LIBRARIES AthenaBaseComps xAODTruth GaudiKernel
   DerivationFrameworkInterfacesLib
   FlavorTagDiscriminants
   PRIVATE_LINK_LIBRARIES AthenaKernel ExpressionEvaluationLib xAODJet
   xAODEventInfo xAODBTagging xAODTracking )

atlas_add_component( DerivationFrameworkTop
   src/components/*.cxx
   LINK_LIBRARIES GaudiKernel DerivationFrameworkTopLib )

# Install files from the package:
atlas_install_python_modules( python/*.py )
atlas_install_joboptions( share/*.py )
