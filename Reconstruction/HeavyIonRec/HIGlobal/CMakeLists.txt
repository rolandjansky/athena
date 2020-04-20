################################################################################
# Package: HIGlobal
################################################################################

# Declare the package name:
atlas_subdir( HIGlobal )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          Calorimeter/CaloEvent
                          Control/AthToolSupport/AsgTools
                          Control/AthenaBaseComps
                          Event/NavFourMom
                          Event/xAOD/xAODHIEvent
                          PhysicsAnalysis/HeavyIonPhys/HIEventUtils
                          PRIVATE
                          Control/Navigation
                          GaudiKernel )

# External dependencies:
find_package( ROOT COMPONENTS Core Tree MathCore Hist RIO pthread )

# Component(s) in the package:
atlas_add_library( HIGlobalLib
                   HIGlobal/*.h
                   INTERFACE
                   PUBLIC_HEADERS HIGlobal
                   LINK_LIBRARIES AsgTools AthenaBaseComps CaloEvent GaudiKernel HIEventUtilsLib NavFourMom xAODHIEvent )

atlas_add_component( HIGlobal
                     src/*.cxx
                     src/components/*.cxx
                     LINK_LIBRARIES GaudiKernel Navigation HIGlobalLib )

# Install files from the package:
atlas_install_python_modules( python/*.py )
atlas_install_joboptions( share/*.py )

