################################################################################
# Package: PixelDigitization
################################################################################

# Declare the package name:
atlas_subdir( PixelRadDamDigitization )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          GaudiKernel
                          PRIVATE
                          Commission/CommissionEvent
                          Control/AthenaBaseComps
                          Control/AthenaKernel
                          Control/PileUpTools
                          Control/StoreGate
                          DetectorDescription/Identifier
                          Generators/GeneratorObjects
                          InnerDetector/InDetConditions/InDetConditionsSummaryService
                          InnerDetector/InDetConditions/InDetCondTools
                          InnerDetector/InDetConditions/PixelConditionsTools
                          InnerDetector/InDetConditions/PixelConditionsData
                          InnerDetector/InDetConditions/SiPropertiesTool
                          InnerDetector/InDetDetDescr/InDetIdentifier
                          InnerDetector/InDetDetDescr/InDetReadoutGeometry
                          InnerDetector/InDetDetDescr/PixelCabling
                          InnerDetector/InDetDigitization/SiDigitization
                          InnerDetector/InDetRawEvent/InDetRawData
                          InnerDetector/InDetRawEvent/InDetSimData
                          InnerDetector/InDetSimEvent
                          Simulation/HitManagement
                          Tools/PathResolver )

# External dependencies:
find_package( CLHEP )
find_package( HepMC )
find_package( ROOT COMPONENTS Core Tree MathCore Hist RIO pthread Matrix TreePlayer )

# Component(s) in the package:
atlas_add_component( PixelRadDamDigitization
                     src/*.cxx
                     src/components/*.cxx
                     INCLUDE_DIRS ${ROOT_INCLUDE_DIRS} ${CLHEP_INCLUDE_DIRS} ${HEPMC_INCLUDE_DIRS}
                     LINK_LIBRARIES ${ROOT_LIBRARIES} ${CLHEP_LIBRARIES} ${HEPMC_LIBRARIES} AthenaBaseComps GaudiKernel CommissionEvent AthenaKernel PileUpToolsLib StoreGateLib SGtests Identifier GeneratorObjects InDetCondTools PixelConditionsData SiPropertiesToolLib InDetIdentifier InDetReadoutGeometry SiDigitization InDetRawData InDetSimData InDetSimEvent HitManagement PathResolver )

# Install files from the package:
atlas_install_python_modules( python/*.py )

