################################################################################
# Package: Photospp_i
################################################################################

# Declare the package name:
atlas_subdir( Photospp_i )

# Declare the package's dependencies:
atlas_depends_on_subdirs(
   PUBLIC
   Control/AthenaBaseComps
   PRIVATE
   Control/AthenaKernel
   GaudiKernel
   Generators/GeneratorObjects )

# External dependencies:
find_package( CLHEP )
if (HEPMC3_USE)
find_package( Photospp COMPONENTS Photospp PhotosppHepMC3 )
else()
find_package( Photospp COMPONENTS Photospp PhotosppHepMC )
endif()

# Remove the --as-needed linker flags:
atlas_disable_as_needed()

# Component(s) in the package:
atlas_add_library( Photospp_iLib
   Photospp_i/Photospp_i.h src/Photospp_i.cxx
   PUBLIC_HEADERS Photospp_i
   INCLUDE_DIRS ${PHOTOSPP_INCLUDE_DIRS}
   PRIVATE_INCLUDE_DIRS ${CLHEP_INCLUDE_DIRS}
   PRIVATE_DEFINITIONS ${CLHEP_DEFINITIONS}
   LINK_LIBRARIES ${PHOTOSPP_LIBRARIES} AthenaBaseComps
   PRIVATE_LINK_LIBRARIES ${CLHEP_LIBRARIES} AthenaKernel GaudiKernel
   GeneratorObjects )

atlas_add_component( Photospp_i
   src/components/*.cxx
   LINK_LIBRARIES GaudiKernel Photospp_iLib )

# Install files from the package:
atlas_install_joboptions( share/common/*.py )

