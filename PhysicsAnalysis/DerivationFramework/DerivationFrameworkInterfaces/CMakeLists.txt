################################################################################
# Package: DerivationFrameworkInterfaces
################################################################################

# Declare the package name:
atlas_subdir( DerivationFrameworkInterfaces )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          GaudiKernel )

# Component(s) in the package:
atlas_add_library( DerivationFrameworkInterfaces
   DerivationFrameworkInterfaces/*.h
   INTERFACE
   PUBLIC_HEADERS DerivationFrameworkInterfaces
   LINK_LIBRARIES GaudiKernel )

atlas_add_dictionary( DerivationFrameworkInterfacesDict
   DerivationFrameworkInterfaces/DerivationFrameworkInterfacesDict.h
   DerivationFrameworkInterfaces/selection.xml
   LINK_LIBRARIES DerivationFrameworkInterfaces )
