################################################################################
# Package: PixelDigitization
################################################################################

# Declare the package name:
atlas_subdir( PixelDigitization )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          Control/AthenaBaseComps
                          GaudiKernel
                          PRIVATE
                          Commission/CommissionEvent
                          Control/AthenaKernel
                          Control/PileUpTools
                          Control/StoreGate
                          DetectorDescription/AtlasDetDescr
			  DetectorDescription/GeoPrimitives
                          DetectorDescription/Identifier
                          Event/EventInfo
                          Event/xAOD/xAODEventInfo
                          Generators/GeneratorObjects
                          InnerDetector/InDetConditions/InDetConditionsSummaryService
                          InnerDetector/InDetConditions/PixelConditionsData
                          InnerDetector/InDetConditions/PixelConditionsServices
                          InnerDetector/InDetConditions/PixelConditionsTools
                          InnerDetector/InDetConditions/SiPropertiesSvc
                          InnerDetector/InDetDetDescr/InDetIdentifier
                          InnerDetector/InDetDetDescr/InDetReadoutGeometry
                          InnerDetector/InDetDetDescr/PixelCabling
                          InnerDetector/InDetDetDescr/PixelGeoModel
                          InnerDetector/InDetDigitization/SiDigitization
                          InnerDetector/InDetRawEvent/InDetRawData
                          InnerDetector/InDetRawEvent/InDetSimData
                          InnerDetector/InDetSimEvent
                          Simulation/HitManagement
                          Simulation/Tools/AtlasCLHEP_RandomGenerators
			  Tracking/TrkDetDescr/TrkSurfaces
			  Tracking/TrkDigitization/TrkDigInterfaces
			  Tracking/TrkDigitization/TrkDigEvent
                          Tools/PathResolver )


# External dependencies:
find_package( CLHEP )
find_package( HepMC )
find_package( ROOT COMPONENTS Core Tree MathCore Hist RIO pthread )

# Component(s) in the package:
atlas_add_component( PixelDigitization
                     src/*.cxx
                     src/components/*.cxx
                     INCLUDE_DIRS ${ROOT_INCLUDE_DIRS} ${CLHEP_INCLUDE_DIRS} ${HEPMC_INCLUDE_DIRS}
                     LINK_LIBRARIES ${ROOT_LIBRARIES} ${CLHEP_LIBRARIES} ${HEPMC_LIBRARIES} AthenaBaseComps GaudiKernel CommissionEvent AthenaKernel PileUpToolsLib StoreGateLib SGtests AtlasDetDescr GeoPrimitives Identifier EventInfo xAODEventInfo GeneratorObjects PixelConditionsData SiPropertiesSvcLib InDetIdentifier InDetReadoutGeometry SiDigitization InDetRawData InDetSimData InDetSimEvent HitManagement AtlasCLHEP_RandomGenerators TrkSurfaces TrkDigInterfaces TrkDigEvent PathResolver )

# Install files from the package:
atlas_install_headers( PixelDigitization )
atlas_install_python_modules( python/*.py )
atlas_install_joboptions( share/PixelDigitization_jobOptions.py share/PixelDigiTool_jobOptions.py )
atlas_install_runtime( share/3DFEI3-3E-problist-1um_v1.txt share/3DFEI4-2E-problist-1um_v0.txt share/Bichsel_*.dat )

