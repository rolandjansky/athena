################################################################################
# Package: EventCommonAlgs
################################################################################

# Declare the package name:
atlas_subdir( EventCommonAlgs )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          Control/AthenaBaseComps
                          GaudiKernel
                          PRIVATE
                          Event/FourMomUtils
                          Event/NavFourMom )

# External dependencies:
find_package( Boost COMPONENTS filesystem thread system )

# Component(s) in the package:
atlas_add_component( EventCommonAlgs
                     src/*.cxx
                     src/components/*.cxx
                     INCLUDE_DIRS ${Boost_INCLUDE_DIRS}
                     LINK_LIBRARIES ${Boost_LIBRARIES} AthenaBaseComps GaudiKernel FourMomUtils NavFourMom )

# Install files from the package:
atlas_install_headers( EventCommonAlgs )
atlas_install_joboptions( share/*.py )

