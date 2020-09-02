################################################################################
# Package: TrigPSC
################################################################################

# Declare the package name:
atlas_subdir( TrigPSC )

# External dependencies:
find_package( Boost COMPONENTS filesystem thread system )
find_package( Python COMPONENTS Development )
find_package( tdaq-common COMPONENTS ers eformat eformat_write hltinterface )

# Component(s) in the package:
atlas_add_library( TrigPSC
                   src/*.cxx
                   PUBLIC_HEADERS TrigPSC
                   INCLUDE_DIRS ${TDAQ-COMMON_INCLUDE_DIRS}
                   PRIVATE_INCLUDE_DIRS ${Boost_INCLUDE_DIRS} ${Python_INCLUDE_DIRS}
                   LINK_LIBRARIES ${TDAQ-COMMON_LIBRARIES} GaudiKernel
                   PRIVATE_LINK_LIBRARIES ${Boost_LIBRARIES} ${Python_LIBRARIES} TrigKernel PathResolver TrigConfBase )

# Install files from the package:
atlas_install_python_modules( python/*.py )
atlas_install_joboptions( share/*.py )

