################################################################################
# Package: JetTagCalibration
################################################################################

# Declare the package name:
atlas_subdir( JetTagCalibration )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          Control/AthenaBaseComps
                          Control/StoreGate
                          GaudiKernel
                          PRIVATE
                          Database/AthenaPOOL/AthenaPoolUtilities
                          DetectorDescription/DetDescrCond/DetDescrCondTools )

# External dependencies:
find_package( ROOT COMPONENTS Core Tree MathCore Hist RIO pthread )

# Component(s) in the package:
atlas_add_library( JetTagCalibrationLib
                   src/*.cxx
                   PUBLIC_HEADERS JetTagCalibration
                   INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
                   LINK_LIBRARIES ${ROOT_LIBRARIES} AthenaBaseComps GaudiKernel StoreGateLib SGtests
                   PRIVATE_LINK_LIBRARIES AthenaPoolUtilities )

atlas_add_component( JetTagCalibration
                     src/components/*.cxx
                     INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
                     LINK_LIBRARIES ${ROOT_LIBRARIES} AthenaBaseComps StoreGateLib SGtests GaudiKernel AthenaPoolUtilities JetTagCalibrationLib )

# Install files from the package:
atlas_install_joboptions( share/*.py )
atlas_install_runtime( share/*.db )

