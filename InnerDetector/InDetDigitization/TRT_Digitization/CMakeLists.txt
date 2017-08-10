################################################################################
# Package: TRT_Digitization
################################################################################

# Declare the package name:
atlas_subdir( TRT_Digitization )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          GaudiKernel
                          PRIVATE
                          Control/AthenaBaseComps
                          Control/AthenaKernel
                          Control/PileUpTools
                          Database/AthenaPOOL/AthenaPoolUtilities
                          DetectorDescription/GeoModel/GeoModelInterfaces
                          DetectorDescription/GeoModel/GeoModelUtilities
                          DetectorDescription/GeoPrimitives
                          DetectorDescription/Identifier
                          Event/xAOD/xAODEventInfo
                          Generators/GeneratorObjects
                          InnerDetector/InDetConditions/TRT_ConditionsData
                          InnerDetector/InDetConditions/TRT_ConditionsServices
                          InnerDetector/InDetDetDescr/InDetIdentifier
                          InnerDetector/InDetDetDescr/InDetReadoutGeometry
                          InnerDetector/InDetRawEvent/InDetRawData
                          InnerDetector/InDetRawEvent/InDetSimData
                          InnerDetector/InDetSimEvent
                          InnerDetector/InDetSimUtils/TRT_PAI_Process
                          MagneticField/MagFieldInterfaces
                          Simulation/HitManagement )

# External dependencies:
find_package( CLHEP )
find_package( Eigen )
find_package( HepPDT )
find_package( ROOT COMPONENTS Core Tree MathCore Hist RIO pthread )

# Component(s) in the package:
atlas_add_component( TRT_Digitization
                     src/*.cxx
                     src/components/*.cxx
                     INCLUDE_DIRS ${ROOT_INCLUDE_DIRS} ${HEPPDT_INCLUDE_DIRS} ${CLHEP_INCLUDE_DIRS} ${EIGEN_INCLUDE_DIRS}
                     LINK_LIBRARIES ${ROOT_LIBRARIES} ${HEPPDT_LIBRARIES} ${CLHEP_LIBRARIES} ${EIGEN_LIBRARIES} GaudiKernel AthenaBaseComps AthenaKernel PileUpToolsLib AthenaPoolUtilities GeoModelUtilities GeoPrimitives Identifier EventInfo GeneratorObjects TRT_ConditionsData TRT_ConditionsServicesLib InDetIdentifier InDetReadoutGeometry InDetRawData InDetSimData InDetSimEvent MagFieldInterfaces HitManagement )

# Install files from the package:
atlas_install_headers( TRT_Digitization )
atlas_install_python_modules( python/*.py )
atlas_install_joboptions( share/*.py )

