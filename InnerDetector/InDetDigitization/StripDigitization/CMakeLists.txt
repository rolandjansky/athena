################################################################################
# Package: SCT_Digitization
################################################################################

# Declare the package name:
atlas_subdir( StripDigitization )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          Commission/CommissionEvent
                          Control/AthenaBaseComps
                          Control/AthenaKernel
                          Control/PileUpTools
                          DetectorDescription/Identifier
                          Event/xAOD/xAODEventInfo
                          GaudiKernel
                          InnerDetector/InDetDigitization/SiDigitization
                          InnerDetector/InDetDigitization/SCT_Digitization
                          InnerDetector/InDetRawEvent/InDetRawData
                          InnerDetector/InDetSimEvent
                          Simulation/HitManagement
                          PRIVATE
                          Generators/GeneratorObjects
                          InnerDetector/InDetConditions/InDetConditionsSummaryService
                          InnerDetector/InDetConditions/SCT_ConditionsServices
                          InnerDetector/InDetConditions/SiPropertiesSvc
                          InnerDetector/InDetDetDescr/InDetIdentifier
                          InnerDetector/InDetDetDescr/SCT_ReadoutGeometry
                          InnerDetector/InDetDetDescr/InDetReadoutGeometry
                          InnerDetector/InDetDetDescr/SCT_ModuleDistortions
                          InnerDetector/InDetRawEvent/InDetSimData
                          Simulation/Tools/AtlasCLHEP_RandomGenerators 
                          Tools/PathResolver)

# External dependencies:
find_package( Boost COMPONENTS filesystem thread system )
find_package( CLHEP )
find_package( ROOT COMPONENTS Core Tree MathCore Hist RIO pthread )

# Component(s) in the package:
atlas_add_component( StripDigitization
                     src/*.cxx
                     src/components/*.cxx
                     INCLUDE_DIRS ${ROOT_INCLUDE_DIRS} ${Boost_INCLUDE_DIRS} ${CLHEP_INCLUDE_DIRS}
                     LINK_LIBRARIES ${ROOT_LIBRARIES} ${Boost_LIBRARIES} ${CLHEP_LIBRARIES} CommissionEvent AthenaBaseComps AthenaKernel PileUpToolsLib Identifier xAODEventInfo GaudiKernel SiDigitization InDetRawData InDetSimEvent HitManagement GeneratorObjects SiPropertiesSvcLib InDetIdentifier InDetReadoutGeometry SCT_ReadoutGeometry InDetSimData AtlasCLHEP_RandomGenerators PathResolver )

# Install files from the package:
atlas_install_headers( StripDigitization )
atlas_install_python_modules( python/*.py )
# atlas_install_joboptions( share/*.py )

