################################################################################
# Package: ISF_FastCaloSimInterfaces
################################################################################

# Declare the package name:
atlas_subdir( ISF_FastCaloSimInterfaces )

# Component(s) in the package:
atlas_add_library( ISF_FastCaloSimInterfaces
                   PUBLIC_HEADERS ISF_FastCaloSimInterfaces
                   LINK_LIBRARIES GaudiKernel ISF_Event )

