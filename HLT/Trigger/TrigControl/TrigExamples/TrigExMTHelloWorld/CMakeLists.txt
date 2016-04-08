################################################################################
# Package: TrigExMTHelloWorld
################################################################################

# Declare the package name:
atlas_subdir( TrigExMTHelloWorld )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          Control/AthenaBaseComps
                          PRIVATE
                          GaudiKernel )

# Component(s) in the package:
atlas_add_component( TrigExMTHelloWorld
                     src/*.cxx
                     src/components/*.cxx
                     LINK_LIBRARIES AthenaBaseComps GaudiKernel )

# Install files from the package:
atlas_install_headers( TrigExMTHelloWorld )
atlas_install_joboptions( share/*.py )

