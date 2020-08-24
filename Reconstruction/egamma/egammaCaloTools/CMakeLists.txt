################################################################################
# Package: egammaCaloTools
################################################################################

# Declare the package name:
atlas_subdir( egammaCaloTools )

# Component(s) in the package:
atlas_add_component( egammaCaloTools
                     src/*.cxx
                     src/components/*.cxx
                     INCLUDE_DIRS
                     LINK_LIBRARIES CaloDetDescrLib CaloEvent CaloGeoHelpers CaloIdentifier CaloUtilsLib AthenaBaseComps
		     SGTools FourMomUtils xAODCaloEvent GaudiKernel egammaUtils egammaInterfacesLib )

# Install files from the package:
atlas_install_python_modules( python/*.py )

