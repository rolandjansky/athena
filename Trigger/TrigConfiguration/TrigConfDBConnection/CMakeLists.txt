################################################################################
# Package: TrigConfDBConnection
################################################################################

# Declare the package name:
atlas_subdir( TrigConfDBConnection )

# External dependencies:
find_package( Boost COMPONENTS filesystem thread system
   OPTIONAL_COMPONENTS python python27 )
find_package( Frontier_Client )
find_package( PythonLibs )

# Component(s) in the package:
atlas_add_library( TrigConfDBConnection
                   src/*.cxx
                   PUBLIC_HEADERS TrigConfDBConnection
                   PRIVATE_INCLUDE_DIRS ${Boost_INCLUDE_DIRS} ${FRONTIER_CLIENT_INCLUDE_DIRS} ${PYTHON_INCLUDE_DIRS}
                   PRIVATE_LINK_LIBRARIES ${Boost_LIBRARIES} ${FRONTIER_CLIENT_LIBRARIES} ${PYTHON_LIBRARIES} )

atlas_add_library( TrigConfDBFrontier
                   src/*.cxx
                   PUBLIC_HEADERS TrigConfDBConnection
                   PRIVATE_INCLUDE_DIRS ${Boost_INCLUDE_DIRS} ${FRONTIER_CLIENT_INCLUDE_DIRS} ${PYTHON_INCLUDE_DIRS}
                   PRIVATE_LINK_LIBRARIES ${Boost_LIBRARIES} ${FRONTIER_CLIENT_LIBRARIES} ${PYTHON_LIBRARIES} )


atlas_add_executable( TriggerFrontierClientTest
                      src/exe/TriggerFrontierClientTest.cxx
                      INCLUDE_DIRS ${Boost_INCLUDE_DIRS} ${FRONTIER_CLIENT_INCLUDE_DIRS} ${PYTHON_INCLUDE_DIRS}
                      LINK_LIBRARIES ${Boost_LIBRARIES} ${FRONTIER_CLIENT_LIBRARIES} ${PYTHON_LIBRARIES} TrigConfDBConnection )

# Install files from the package:
atlas_install_python_modules( python/*.py )

