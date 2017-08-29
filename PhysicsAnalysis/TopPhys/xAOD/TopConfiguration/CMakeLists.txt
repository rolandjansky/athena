# Auto-generated on: 2017-03-08 14:47:35.608258

# Declare the name of this package:
atlas_subdir( TopConfiguration None )

# This package depends on other packages:
atlas_depends_on_subdirs( PUBLIC
			  xAODCore
                          PATInterfaces )

# This package uses ROOT:
find_package( ROOT REQUIRED COMPONENTS Core Gpad Tree Hist RIO MathCore Graf )
find_package( Boost REQUIRED COMPONENTS iostreams )

message( STATUS "${Boost_LIBRARIES}" )

# Generate a CINT dictionary source file:
atlas_add_root_dictionary( TopConfiguration _cintDictSource
                           ROOT_HEADERS Root/LinkDef.h
                           EXTERNAL_PACKAGES ROOT )

# Build a library that other components can link against:
atlas_add_library( TopConfiguration Root/*.cxx Root/*.h Root/*.icc
                   TopConfiguration/*.h TopConfiguration/*.icc TopConfiguration/*/*.h
                   TopConfiguration/*/*.icc ${_cintDictSource} 
                   PUBLIC_HEADERS TopConfiguration
                   LINK_LIBRARIES xAODCore
		   		   PATInterfaces  
                                  ${ROOT_LIBRARIES}
				  ${Boost_LIBRARIES} 
                   INCLUDE_DIRS ${ROOT_INCLUDE_DIRS} 
		                ${Boost_INCLUDE_DIRS})

# Install data files from the package:
atlas_install_data( share/* )

# Install the python modules from the package
atlas_install_python_modules( python/*.py )

