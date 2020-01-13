################################################################################
# Package: PerfMonComps
################################################################################

# Declare the package name:
atlas_subdir( PerfMonComps )

# Declare the package's dependencies:
atlas_depends_on_subdirs(
   PRIVATE
   Control/AthenaBaseComps
   Control/AthenaKernel
   Control/RootUtils
   Control/CxxUtils
   Control/PerformanceMonitoring/PerfMonEvent
   Control/PerformanceMonitoring/PerfMonKernel
   Control/SGTools
   Control/StoreGate
   GaudiKernel
   Tools/PyUtils )

# External dependencies:
find_package( AIDA )
find_package( Boost )
find_package( Python COMPONENTS Development )
find_package( ROOT COMPONENTS Core PyROOT )
find_package( nlohmann_json )

# Component(s) in the package:
atlas_add_component( PerfMonComps
   src/*.cxx src/components/*.cxx
   INCLUDE_DIRS ${AIDA_INCLUDE_DIRS} ${Boost_INCLUDE_DIRS} ${ROOT_INCLUDE_DIRS}
   ${PYTHON_INCLUDE_DIRS}
   LINK_LIBRARIES ${Boost_LIBRARIES} ${ROOT_LIBRARIES} ${PYTHON_LIBRARIES}
   ${CMAKE_DL_LIBS} AthenaBaseComps AthenaKernel RootUtils CxxUtils
   PerfMonEvent PerfMonKernel SGTools StoreGateLib GaudiKernel
   AthDSoCallBacks nlohmann_json::nlohmann_json)

# Install files from the package:
atlas_install_python_modules( python/*.py )
atlas_install_joboptions( share/*.py )
