################################################################################
# Package: SiLorentzAngleTool
################################################################################

# Declare the package name:
atlas_subdir( SiLorentzAngleTool )

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
                          PRIVATE
                          Database/AthenaPOOL/AthenaPoolUtilities
                          DetectorDescription/Identifier
                          InnerDetector/InDetConditions/InDetConditionsSummaryService
                          InnerDetector/InDetDetDescr/InDetIdentifier
                          InnerDetector/InDetDetDescr/InDetReadoutGeometry
			  InnerDetector/InDetDetDescr/PixelReadoutGeometry
                          MagneticField/MagFieldElements
                          MagneticField/MagFieldConditions
			  )

# External dependencies:
find_package( Eigen )

# Component(s) in the package:
atlas_add_component( SiLorentzAngleTool
                     src/*.cxx
                     src/components/*.cxx
                     INCLUDE_DIRS ${EIGEN_INCLUDE_DIRS}
                     LINK_LIBRARIES ${EIGEN_LIBRARIES} AthenaBaseComps AthenaKernel StoreGateLib SGtests GeoPrimitives GaudiKernel SiPropertiesToolLib AthenaPoolUtilities Identifier InDetIdentifier InDetReadoutGeometry PixelReadoutGeometry MagFieldElements MagFieldConditions InDetConditionsSummaryService )

# Run tests:
atlas_add_test( TestSCTLorentzAngle
                SCRIPT athena.py --threads=5 SiLorentzAngleTool/testSCTLorentzAngle.py 
                PROPERTIES TIMEOUT 300
                ENVIRONMENT THREADS=5 )

atlas_add_test( SiLorentzAngleConfig_test
                SCRIPT test/SiLorentzAngleConfig_test.py
                PROPERTIES TIMEOUT 30 )

# Install files from the package:
atlas_install_headers( SiLorentzAngleTool )
atlas_install_python_modules( python/*.py )
atlas_install_joboptions( share/*.py )
