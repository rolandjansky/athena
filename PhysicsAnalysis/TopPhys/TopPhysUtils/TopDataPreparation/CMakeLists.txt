
# Declare the name of this package:
atlas_subdir( TopDataPreparation )

# This package uses ROOT:
find_package( ROOT REQUIRED COMPONENTS Core Gpad Tree Hist RIO MathCore Graf )

# Generate a CINT dictionary source file:
atlas_add_root_dictionary( TopDataPreparation _cintDictSource
   ROOT_HEADERS TopDataPreparation/SampleXsection.h
   TopDataPreparation/SampleXsectionSvc.h Root/LinkDef.h
   EXTERNAL_PACKAGES ROOT )

# Build a library that other components can link against:
atlas_add_library( TopDataPreparation Root/*.cxx Root/*.h Root/*.icc
                   TopDataPreparation/*.h TopDataPreparation/*.icc TopDataPreparation/*/*.h
                   TopDataPreparation/*/*.icc ${_cintDictSource} 
                   PUBLIC_HEADERS TopDataPreparation
                   LINK_LIBRARIES ${ROOT_LIBRARIES}
                   INCLUDE_DIRS ${ROOT_INCLUDE_DIRS} )

# Install the Python modules of the package:
atlas_install_python_modules( python/*.py )

# Install data files from the package:
atlas_install_data( data/* )
