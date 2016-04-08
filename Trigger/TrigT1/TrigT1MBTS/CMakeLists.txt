################################################################################
# Package: TrigT1MBTS
################################################################################

# Declare the package name:
atlas_subdir( TrigT1MBTS )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          Control/AthenaBaseComps
                          GaudiKernel
                          TileCalorimeter/TileEvent
                          TileCalorimeter/TileIdentifier
                          Trigger/TrigConfiguration/TrigConfInterfaces
                          PRIVATE
                          Trigger/TrigConfiguration/TrigConfL1Data
                          Trigger/TrigT1/TrigT1Interfaces )

# External dependencies:
find_package( Boost COMPONENTS filesystem thread system )

# Component(s) in the package:
atlas_add_component( TrigT1MBTS
                     src/*.cxx
                     src/components/*.cxx
                     INCLUDE_DIRS ${Boost_INCLUDE_DIRS}
                     LINK_LIBRARIES ${Boost_LIBRARIES} AthenaBaseComps GaudiKernel TileEvent TileIdentifier TrigConfL1Data TrigT1Interfaces )

# Install files from the package:
atlas_install_headers( TrigT1MBTS )

