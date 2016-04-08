################################################################################
# Package: BoostAfterburner
################################################################################

# Declare the package name:
atlas_subdir( BoostAfterburner )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          Control/AthenaBaseComps
                          PRIVATE
                          GaudiKernel
                          Generators/GeneratorObjects )

# External dependencies:
find_package( CLHEP )

# Component(s) in the package:
atlas_add_library( BoostAfterburnerLib
                   src/*.cxx
                   PUBLIC_HEADERS BoostAfterburner
                   PRIVATE_INCLUDE_DIRS ${CLHEP_INCLUDE_DIRS}
                   PRIVATE_DEFINITIONS ${CLHEP_DEFINITIONS}
                   LINK_LIBRARIES AthenaBaseComps
                   PRIVATE_LINK_LIBRARIES ${CLHEP_LIBRARIES} GaudiKernel GeneratorObjects )

atlas_add_component( BoostAfterburner
                     src/components/*.cxx
                     INCLUDE_DIRS ${CLHEP_INCLUDE_DIRS}
                     LINK_LIBRARIES ${CLHEP_LIBRARIES} AthenaBaseComps GaudiKernel GeneratorObjects BoostAfterburnerLib )

# Install files from the package:
atlas_install_joboptions( share/*.py )

