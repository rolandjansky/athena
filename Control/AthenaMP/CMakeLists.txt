################################################################################
# Package: AthenaMP
################################################################################

# Declare the package name:
atlas_subdir( AthenaMP )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PRIVATE
                          Control/AthenaBaseComps
                          Control/AthenaInterprocess
                          Control/AthenaMPTools
                          Control/StoreGate
                          GaudiKernel )

# External dependencies:
find_package( Boost COMPONENTS filesystem thread system )
find_package( ROOT COMPONENTS Core PyROOT Tree MathCore Hist RIO pthread )
find_package( Threads )

# Component(s) in the package:
atlas_add_component( AthenaMP
                     src/*.cxx
                     src/components/*.cxx
                     src/memory-profiler/getPss.cc
                     INCLUDE_DIRS ${Boost_INCLUDE_DIRS} ${ROOT_INCLUDE_DIRS} 
                     LINK_LIBRARIES ${Boost_LIBRARIES} ${ROOT_LIBRARIES} AthenaBaseComps AthenaInterprocess StoreGateLib SGtests GaudiKernel )

atlas_add_executable( getSharedMemory
                      src/memory-profiler/getSharedMemory.cc
                      src/memory-profiler/getPss.cc
                      INCLUDE_DIRS ${Boost_INCLUDE_DIRS} ${ROOT_INCLUDE_DIRS}
                      LINK_LIBRARIES ${Boost_LIBRARIES} ${ROOT_LIBRARIES} AthenaBaseComps AthenaInterprocess StoreGateLib SGtests GaudiKernel )

atlas_add_executable( MemoryMonitor
                      src/memory-monitor/MemoryMonitor.cxx
                      LINK_LIBRARIES Threads::Threads )

# Install files from the package:
atlas_install_python_modules( python/*.py python/tests/*.py )
atlas_install_joboptions( share/AthenaMP_EventService.py )

