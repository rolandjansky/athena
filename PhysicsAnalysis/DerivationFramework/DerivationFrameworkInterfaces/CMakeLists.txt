################################################################################
# Package: DerivationFrameworkInterfaces
################################################################################

# Declare the package name:
atlas_subdir( DerivationFrameworkInterfaces )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          GaudiKernel )

# Install files from the package:
atlas_install_headers( DerivationFrameworkInterfaces )

# reflex dictionary
atlas_add_dictionary( DerivationFrameworkInterfacesDict
		      DerivationFrameworkInterfaces/DerivationFrameworkInterfacesDict.h
                      DerivationFrameworkInterfaces/selection.xml
	                      INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
	                      LINK_LIBRARIES ${ROOT_LIBRARIES} GaudiKernel )
