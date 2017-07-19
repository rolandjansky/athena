################################################################################
# Package: TrigTimeMonitor
################################################################################

# Declare the package name:
atlas_subdir( TrigTimeMonitor )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          Control/AthenaMonitoring
                          GaudiKernel
                          Trigger/TrigMonitoring/TrigMonitorBase
                          Trigger/TrigTools/TrigTimeAlgs )

# External dependencies:
find_package( Boost COMPONENTS filesystem thread system )
find_package( ROOT COMPONENTS Core Tree MathCore Hist RIO pthread )

# Component(s) in the package:
atlas_add_component( TrigTimeMonitor
                     src/*.cxx
                     src/components/*.cxx
                     INCLUDE_DIRS ${Boost_INCLUDE_DIRS} ${ROOT_INCLUDE_DIRS} 
                     LINK_LIBRARIES ${Boost_LIBRARIES} ${ROOT_LIBRARIES} AthenaMonitoringLib GaudiKernel TrigMonitorBaseLib TrigTimeAlgsLib )

# Install files from the package:
atlas_install_headers( TrigTimeMonitor )
atlas_install_python_modules( python/*.py )
atlas_install_joboptions( share/jobOfragment_TrigTimeMonitor.py )

