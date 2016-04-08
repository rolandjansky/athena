################################################################################
# Package: AlgorithmToolSetup
################################################################################

# Declare the package name:
atlas_subdir( AlgorithmToolSetup )

# Install files from the package:
atlas_install_runtime( src/*.templ src/*.txt share/AlgorithmToolSetup.sh )
atlas_install_scripts( share/*.sh )

