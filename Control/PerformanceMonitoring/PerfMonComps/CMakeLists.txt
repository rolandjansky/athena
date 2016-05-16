# $Id: CMakeLists.txt 725991 2016-02-24 18:16:09Z krasznaa $
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
find_package( PythonLibs )
find_package( ROOT COMPONENTS Core PyROOT )

# Component(s) in the package:
atlas_add_component( PerfMonComps
   src/*.cxx src/components/*.cxx
   INCLUDE_DIRS ${AIDA_INCLUDE_DIRS} ${Boost_INCLUDE_DIRS} ${ROOT_INCLUDE_DIRS}
   ${PYTHON_INCLUDE_DIRS}
   LINK_LIBRARIES ${Boost_LIBRARIES} ${ROOT_LIBRARIES} ${PYTHON_LIBRARIES}
   AthenaBaseComps AthenaKernel CxxUtils PerfMonEvent PerfMonKernel SGTools
   StoreGateLib SGtests GaudiKernel AthDSoCallBacks )

# Install files from the package:
atlas_install_python_modules( python/*.py )
atlas_install_joboptions( share/*.py )
