################################################################################
# Package: Photospp_i
################################################################################

# Declare the package name:
atlas_subdir( Photospp_i )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          Control/AthenaBaseComps
                          PRIVATE
                          Control/AthenaKernel
                          GaudiKernel
                          Generators/GeneratorObjects )

# Set the version of Photospp to use:
#set( PHOTOSPP_VERSION 3.61 )
#set( PHOTOSPP_ROOT
#   ${LCG_RELEASE_DIR}/MCGenerators/photos++/${PHOTOSPP_VERSION}/${LCG_PLATFORM} )

# External dependencies:
find_package( CLHEP )
find_package( Photospp COMPONENTS Photospp PhotosppHepMC )
#find_package( Photospp )

# Remove the --as-needed linker flags:
atlas_disable_as_needed()

# Component(s) in the package:
atlas_add_library( Photospp_iLib
                   src/Photospp_i.cxx
                   PUBLIC_HEADERS Photospp_i
                   INCLUDE_DIRS ${PHOTOSPP_INCLUDE_DIRS}
                   PRIVATE_INCLUDE_DIRS ${CLHEP_INCLUDE_DIRS}
                   PRIVATE_DEFINITIONS ${CLHEP_DEFINITIONS}
                   LINK_LIBRARIES ${PHOTOSPP_LIBRARIES} AthenaBaseComps
                   PRIVATE_LINK_LIBRARIES ${CLHEP_LIBRARIES} AthenaKernel GaudiKernel GeneratorObjects )

atlas_add_component( Photospp_i
                     src/components/*.cxx
                     INCLUDE_DIRS ${CLHEP_INCLUDE_DIRS} ${PHOTOSPP_INCLUDE_DIRS}
                     LINK_LIBRARIES ${CLHEP_LIBRARIES} ${PHOTOSPP_LIBRARIES} AthenaBaseComps AthenaKernel GaudiKernel GeneratorObjects Photospp_iLib )

# Install files from the package:
atlas_install_joboptions( share/*.py )

