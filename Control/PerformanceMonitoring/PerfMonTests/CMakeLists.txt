################################################################################
# Package: PerfMonTests
################################################################################

# Declare the package name:
atlas_subdir( PerfMonTests )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PRIVATE
                          Control/AthenaBaseComps
                          Control/AthenaKernel
                          Control/AthAllocators
                          Control/AthContainers
                          Control/PerformanceMonitoring/PerfMonComps
                          Control/PerformanceMonitoring/PerfMonKernel
                          Control/StoreGate
                          GaudiKernel
                          PhysicsAnalysis/TruthParticleID/McParticleTests
                          Reconstruction/Jet/JetRec )

# External dependencies:
find_package( CLHEP )

# Component(s) in the package:
atlas_add_component( PerfMonTests
                     src/*.cxx
                     src/components/*.cxx
                     INCLUDE_DIRS ${CLHEP_INCLUDE_DIRS}
                     LINK_LIBRARIES ${CLHEP_LIBRARIES} AthenaBaseComps AthenaKernel AthAllocators AthContainers PerfMonKernel StoreGateLib SGtests GaudiKernel )

# Install files from the package:
atlas_install_python_modules( python/*.py )
atlas_install_joboptions( share/*.py share/tests/*.py )


atlas_add_test( NoopAlg
   SCRIPT test/NoopAlg.sh
   PROPERTIES TIMEOUT 600
   LOG_IGNORE_PATTERN "running" )


atlas_add_test( NoopAlg_WriteDataHdr
   SCRIPT test/NoopAlg_WriteDataHdr.sh
   PROPERTIES TIMEOUT 600
   LOG_IGNORE_PATTERN "running" )


atlas_add_test( NoopAlg_WriteEvtInfo
   SCRIPT test/NoopAlg_WriteEvtInfo.sh
   PROPERTIES TIMEOUT 600
   LOG_IGNORE_PATTERN "running" )


atlas_add_test( LeakyAlg
   SCRIPT test/LeakyAlg.sh
   PROPERTIES TIMEOUT 600
   LOG_IGNORE_PATTERN "running" )


atlas_add_test( BasicAlg
   SCRIPT test/BasicAlg.sh
   PROPERTIES TIMEOUT 600
   LOG_IGNORE_PATTERN "running" )


atlas_add_test( BaseLine
   SCRIPT test/BaseLine.sh
   PROPERTIES TIMEOUT 600
   LOG_IGNORE_PATTERN "running" )
