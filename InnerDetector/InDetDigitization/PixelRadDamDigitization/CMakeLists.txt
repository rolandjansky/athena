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
                          Control/CxxUtils
                          Control/PileUpTools
                          Control/StoreGate
                          DetectorDescription/Identifier
                          Generators/GeneratorObjects
                          InnerDetector/InDetConditions/InDetConditionsSummaryService
                          InnerDetector/InDetConditions/InDetCondTools
                          InnerDetector/InDetConditions/PixelConditionsServices
                          InnerDetector/InDetConditions/PixelConditionsTools
                          InnerDetector/InDetConditions/SiPropertiesTool
                          InnerDetector/InDetDetDescr/InDetIdentifier
                          InnerDetector/InDetDetDescr/InDetReadoutGeometry
                          InnerDetector/InDetDigitization/SiDigitization
                          InnerDetector/InDetRawEvent/InDetRawData
                          InnerDetector/InDetRawEvent/InDetSimData
                          InnerDetector/InDetSimEvent
                          Simulation/HitManagement
                          Tools/PathResolver )

# External dependencies:
find_package( CLHEP )
find_package( HepMC )
find_package( ROOT COMPONENTS Core Tree MathCore Hist RIO pthread )

# Component(s) in the package:
atlas_add_component( PixelRadDamDigitization
                     src/*.cxx
                     src/components/*.cxx
                     INCLUDE_DIRS ${ROOT_INCLUDE_DIRS} ${CLHEP_INCLUDE_DIRS} ${HEPMC_INCLUDE_DIRS}
                     LINK_LIBRARIES ${ROOT_LIBRARIES} ${CLHEP_LIBRARIES} ${HEPMC_LIBRARIES} AthenaBaseComps GaudiKernel CommissionEvent AthenaKernel PileUpToolsLib StoreGateLib SGtests Identifier xAODEventInfo GeneratorObjects InDetCondTools PixelConditionsData SiPropertiesToolLib InDetIdentifier InDetReadoutGeometry SiDigitization InDetRawData InDetSimData InDetSimEvent HitManagement PathResolver )

# Install files from the package:
atlas_install_python_modules( python/*.py )
atlas_install_joboptions( share/PixelDigitization_jobOptions.py share/PixelDigiTool_jobOptions.py )
atlas_install_runtime( share/3DFEI3-3E-problist-1um_v1.txt share/3DFEI4-2E-problist-1um_v0.txt share/Bichsel_*.dat share/maps*.root )

