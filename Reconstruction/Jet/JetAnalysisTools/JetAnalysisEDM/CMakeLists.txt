################################################################################
# Package: JetAnalysisEDM
################################################################################

# Declare the package name:
atlas_subdir( JetAnalysisEDM )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          Calorimeter/CaloEvent
                          Reconstruction/Jet/JetEvent )

# External dependencies:
find_package( ROOT COMPONENTS Core Tree MathCore Hist RIO pthread )

# Install files from the package:
atlas_install_headers( JetAnalysisEDM )

