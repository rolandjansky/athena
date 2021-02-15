# Declare the name of this package:
atlas_subdir( TopDataPreparation )

# This package uses ROOT:
find_package( ROOT REQUIRED COMPONENTS Core Gpad Tree Hist RIO MathCore Graf )

# Generate a CINT dictionary source file:
atlas_add_root_dictionary( TopDataPreparation _cintDictSource
                           ROOT_HEADERS Root/LinkDef.h
                           EXTERNAL_PACKAGES ROOT )

# Build a library that other components can link against:
atlas_add_library( TopDataPreparation Root/*.cxx Root/*.h
                   TopDataPreparation/*.h ${_cintDictSource}
                   PUBLIC_HEADERS TopDataPreparation
                   LINK_LIBRARIES ${ROOT_LIBRARIES}
                   INCLUDE_DIRS ${ROOT_INCLUDE_DIRS} )

# Install data files from the package:
atlas_install_data( data/* )
