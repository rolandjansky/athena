################################################################################
# Package: SiLorentzAngleSvc
################################################################################

# Declare the package name:
atlas_subdir( SiLorentzAngleSvc )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          Control/AthenaBaseComps
                          Control/AthenaKernel
                          Control/StoreGate
                          DetectorDescription/GeoPrimitives
                          GaudiKernel
                          InnerDetector/InDetConditions/InDetCondTools
                          InnerDetector/InDetConditions/SiPropertiesTool
                          InnerDetector/InDetConditions/PixelConditionsData
                          InnerDetector/InDetConditions/PixelConditionsTools
                          InnerDetector/InDetConditions/SCT_ConditionsData
                          MagneticField/MagFieldInterfaces
                          PRIVATE
                          Database/AthenaPOOL/AthenaPoolUtilities
                          DetectorDescription/Identifier
                          InnerDetector/InDetConditions/InDetConditionsSummaryService
                          InnerDetector/InDetDetDescr/InDetIdentifier
                          InnerDetector/InDetDetDescr/InDetReadoutGeometry )

# External dependencies:
find_package( Eigen )

# Component(s) in the package:
atlas_add_component( SiLorentzAngleSvc
                     src/*.cxx
                     src/components/*.cxx
                     INCLUDE_DIRS ${EIGEN_INCLUDE_DIRS}
                     LINK_LIBRARIES ${EIGEN_LIBRARIES} AthenaBaseComps AthenaKernel StoreGateLib SGtests GeoPrimitives GaudiKernel SiPropertiesToolLib MagFieldInterfaces AthenaPoolUtilities Identifier InDetIdentifier InDetReadoutGeometry )

# Run tests:
atlas_add_test( TestSCTLorentzAngle
                SCRIPT athena.py --threads=5 SiLorentzAngleSvc/testSCTLorentzAngle.py 
                PROPERTIES TIMEOUT 300
                ENVIRONMENT THREADS=5 )

# Install files from the package:
atlas_install_headers( SiLorentzAngleSvc )
atlas_install_python_modules( python/*.py )
atlas_install_joboptions( share/*.py )
