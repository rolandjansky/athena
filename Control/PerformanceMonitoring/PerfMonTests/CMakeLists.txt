################################################################################
# Package: PerfMonTests
################################################################################

# Declare the package name:
atlas_subdir( PerfMonTests )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PRIVATE
                          Control/AthenaBaseComps
                          Control/AthenaKernel
                          Control/DataModel
                          Control/PerformanceMonitoring/PerfMonComps
                          Control/PerformanceMonitoring/PerfMonKernel
                          Control/SGTools
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
                     LINK_LIBRARIES ${CLHEP_LIBRARIES} AthenaBaseComps AthenaKernel DataModel PerfMonKernel SGTools StoreGateLib SGtests GaudiKernel )

# Install files from the package:
atlas_install_python_modules( python/*.py )
atlas_install_joboptions( share/*.py share/tests/*.py )

