################################################################################
# Package: JetCPInterfaces
################################################################################

# Declare the package name:
atlas_subdir( JetCPInterfaces )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          PhysicsAnalysis/AnalysisCommon/PATInterfaces
                          Reconstruction/Jet/JetInterface )

# Install files from the package:
atlas_install_headers( JetCPInterfaces )

