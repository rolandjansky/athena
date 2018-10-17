################################################################################
# Package: TrigT1NSW
################################################################################

# Declare the package name:
atlas_subdir( TrigT1NSW )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          Control/AthenaBaseComps
                          GaudiKernel
                          Trigger/TrigSteer/TrigInterfaces
                          Trigger/TrigT1/TrigT1NSWSimTools
                          PRIVATE
                          Control/AthenaMonitoring
                          Control/StoreGate
                          Event/EventInfo )

# External dependencies:
find_package( Boost )
find_package( TBB )
find_package( ROOT COMPONENTS Core Tree MathCore Hist RIO pthread )

# Component(s) in the package:
atlas_add_component( TrigT1NSW
                     src/*.cxx
                     src/components/*.cxx
                     INCLUDE_DIRS ${ROOT_INCLUDE_DIRS} ${BOOST_INCLUDE_DIRS} ${TBB_INCLUDE_DIRS} 
                     LINK_LIBRARIES ${ROOT_LIBRARIES} ${BOOST_LIBRARIES} ${TBB_LIBRARIES} 
                     AthenaBaseComps GaudiKernel TrigInterfacesLib TrigT1NSWSimToolsLib AthenaMonitoringLib
                     StoreGateLib SGtests EventInfo )

# Install files from the package:
atlas_install_headers( TrigT1NSW )
atlas_install_python_modules( python/*.py )
atlas_install_joboptions( share/*.py )

