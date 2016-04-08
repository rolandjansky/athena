################################################################################
# Package: ISF_FastCaloSimInterfaces
################################################################################

# Declare the package name:
atlas_subdir( ISF_FastCaloSimInterfaces )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          GaudiKernel
                          Simulation/ISF/ISF_Core/ISF_Event )

# Component(s) in the package:
atlas_add_library( ISF_FastCaloSimInterfaces
                   PUBLIC_HEADERS ISF_FastCaloSimInterfaces
                   LINK_LIBRARIES GaudiKernel ISF_Event )

