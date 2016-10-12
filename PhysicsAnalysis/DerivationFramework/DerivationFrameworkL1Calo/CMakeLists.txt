################################################################################
# Package: DerivationFrameworkL1Calo
################################################################################

# Declare the package name:
atlas_subdir( DerivationFrameworkL1Calo )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          Control/AthenaBaseComps
                          PhysicsAnalysis/DerivationFramework/DerivationFrameworkInterfaces
                          PRIVATE
                          Event/xAOD/xAODTrigL1Calo
                          GaudiKernel )

# External dependencies:
find_package( ROOT COMPONENTS Core Tree MathCore Hist RIO pthread MathMore Minuit Minuit2 Matrix Physics HistPainter Rint )

# Component(s) in the package:
atlas_add_library( DerivationFrameworkL1CaloLib
                   src/*.cxx
                   PUBLIC_HEADERS DerivationFrameworkL1Calo
                   PRIVATE_INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
                   LINK_LIBRARIES AthenaBaseComps
                   PRIVATE_LINK_LIBRARIES ${ROOT_LIBRARIES} xAODTrigL1Calo GaudiKernel )

atlas_add_component( DerivationFrameworkL1Calo
                     src/components/*.cxx
                     INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
                     LINK_LIBRARIES ${ROOT_LIBRARIES} AthenaBaseComps xAODTrigL1Calo GaudiKernel DerivationFrameworkL1CaloLib )

# Install files from the package:
atlas_install_python_modules( python/*.py )
atlas_install_joboptions( share/*.py )

