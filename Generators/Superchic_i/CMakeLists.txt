# $Id: CMakeLists.txt 762209 2016-07-15 14:15:34Z krasznaa $
################################################################################
# Package: Superchic_i
################################################################################

# Declare the package name:
atlas_subdir( Superchic_i )

# Install files from the package:
atlas_install_python_modules( python/*.py )
atlas_install_joboptions( share/*.py )

atlas_add_component( Superchic_i 
   src/components/*.cxx
)

# Set Superchic specific environment variable(s).
set( SuperchicEnvironment_DIR ${CMAKE_CURRENT_SOURCE_DIR}
   CACHE PATH "Location of SuperchicEnvironment.cmake" )
find_package( SuperchicEnvironment )
