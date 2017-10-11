################################################################################
# Package: FastSiDigitization
################################################################################

# Declare the package name:
atlas_subdir( FastSiDigitization )
# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          Control/AthenaBaseComps
                          Control/AthenaKernel
                          Control/PileUpTools
                          Event/EventPrimitives
                          Event/xAOD/xAODEventInfo
                          GaudiKernel
                          InnerDetector/InDetConditions/InDetConditionsSummaryService
                          InnerDetector/InDetConditions/PixelConditionsTools
                          InnerDetector/InDetConditions/SiPropertiesSvc
                          InnerDetector/InDetDetDescr/InDetReadoutGeometry
                          InnerDetector/InDetRawEvent/InDetSimData
                          InnerDetector/InDetRecEvent/InDetPrepRawData
                          InnerDetector/InDetRecTools/SiClusterizationTool
                          InnerDetector/InDetSimEvent
                          Simulation/HitManagement
                          Simulation/ISF/ISF_Fatras/ISF_FatrasDetDescrModel
                          Simulation/ISF/ISF_Fatras/ISF_FatrasEvent
                          Tracking/TrkEvent/TrkTruthData
                          Tracking/TrkEventCnv/TrkEventTPCnv
                          PRIVATE
                          Control/CxxUtils
                          Control/StoreGate
                          DetectorDescription/Identifier
                          Generators/GeneratorObjects
                          InnerDetector/InDetDetDescr/InDetIdentifier
                          Simulation/Tools/AtlasCLHEP_RandomGenerators
                          Tracking/TrkDetDescr/TrkDetDescrInterfaces
                          Tracking/TrkDetDescr/TrkGeometry
                          Tracking/TrkDetDescr/TrkSurfaces
                          Tracking/TrkExtrapolation/TrkExUtils
                          Tracking/TrkDigitization/TrkDigEvent
                          Tracking/TrkDigitization/TrkDigInterfaces
                          Tracking/TrkDigitization/GeoPrimitives )

# External dependencies:
find_package( Boost COMPONENTS filesystem thread system )
find_package( CLHEP )
find_package( HepMC )
find_package( ROOT COMPONENTS Core Tree MathCore Hist RIO pthread )

# Component(s) in the package:
atlas_add_component( FastSiDigitization
                     src/*.cxx
                     src/components/*.cxx
                     INCLUDE_DIRS ${ROOT_INCLUDE_DIRS} ${Boost_INCLUDE_DIRS} ${CLHEP_INCLUDE_DIRS} ${HEPMC_INCLUDE_DIRS}
                     LINK_LIBRARIES ${ROOT_LIBRARIES} ${Boost_LIBRARIES} ${CLHEP_LIBRARIES} ${HEPMC_LIBRARIES} AthenaBaseComps AthenaKernel PileUpToolsLib EventPrimitives xAODEventInfo GaudiKernel SiPropertiesSvcLib InDetReadoutGeometry InDetSimData InDetPrepRawData SiClusterizationToolLib InDetSimEvent HitManagement ISF_FatrasDetDescrModel ISF_FatrasEvent TrkTruthData TrkEventTPCnv CxxUtils StoreGateLib SGtests Identifier GeneratorObjects InDetIdentifier AtlasCLHEP_RandomGenerators TrkDetDescrInterfaces TrkDigEvent TrkGeometry TrkSurfaces TrkExUtils )

# Install files from the package:
atlas_install_headers( FastSiDigitization )
atlas_install_python_modules( python/*.py )
atlas_install_joboptions( share/*.py )

