################################################################################
# Package: TrigMonitorBase
################################################################################

# Declare the package name:
atlas_subdir( TrigMonitorBase )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          Control/AthenaBaseComps
                          Control/AthenaMonitoring
                          GaudiKernel
                          PRIVATE
                          AtlasTest/TestTools
                          Trigger/TrigSteer/TrigInterfaces )

# External dependencies:
find_package( Boost COMPONENTS filesystem thread system )
find_package( ROOT COMPONENTS Core Tree MathCore Hist RIO pthread )

# Component(s) in the package:
atlas_add_library( TrigMonitorBaseLib
                   src/*.cxx
                   PUBLIC_HEADERS TrigMonitorBase
                   INCLUDE_DIRS ${Boost_INCLUDE_DIRS} ${ROOT_INCLUDE_DIRS}
                   LINK_LIBRARIES ${Boost_LIBRARIES} ${ROOT_LIBRARIES} AthenaBaseComps GaudiKernel AthenaMonitoringLib TrigInterfacesLib
                   PRIVATE_LINK_LIBRARIES TestTools )

atlas_add_component( TrigMonitorBase
                     src/components/*.cxx
                     INCLUDE_DIRS ${Boost_INCLUDE_DIRS} ${ROOT_INCLUDE_DIRS}
                     LINK_LIBRARIES ${Boost_LIBRARIES} ${ROOT_LIBRARIES} AthenaBaseComps AthenaMonitoringLib GaudiKernel TestTools TrigInterfacesLib TrigMonitorBaseLib )

atlas_add_test( HistoOperationLock_test
                SOURCES
                test/HistoOperationLock_test.cxx
                INCLUDE_DIRS ${Boost_INCLUDE_DIRS} ${ROOT_INCLUDE_DIRS}
                LINK_LIBRARIES ${Boost_LIBRARIES} ${ROOT_LIBRARIES} AthenaBaseComps AthenaMonitoringLib GaudiKernel TestTools TrigInterfacesLib TrigMonitorBaseLib )

atlas_add_test( LBNHist_test
                SOURCES
                test/LBNHist_test.cxx
                INCLUDE_DIRS ${Boost_INCLUDE_DIRS} ${ROOT_INCLUDE_DIRS}
                LINK_LIBRARIES ${Boost_LIBRARIES} ${ROOT_LIBRARIES} AthenaBaseComps AthenaMonitoringLib GaudiKernel TestTools TrigInterfacesLib TrigMonitorBaseLib )

# Install files from the package:
atlas_install_python_modules( python/__init__.py python/TrigGenericMonitoringToolConfig.py )

