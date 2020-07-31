################################################################################
# Package: TRT_Digitization
################################################################################

# Declare the package name:
atlas_subdir( TRT_Digitization )

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
                     LINK_LIBRARIES ${ROOT_LIBRARIES} ${HEPPDT_LIBRARIES} ${CLHEP_LIBRARIES} ${EIGEN_LIBRARIES} GaudiKernel AthenaBaseComps AthenaKernel CxxUtils PileUpToolsLib AthenaPoolUtilities GeoModelUtilities GeoPrimitives Identifier EventInfo GeneratorObjects TRT_ConditionsData TRT_ConditionsServicesLib InDetIdentifier InDetReadoutGeometry TRT_ReadoutGeometry InDetRawData InDetSimData InDetSimEvent HitManagement MagFieldElements MagFieldConditions TRT_PAI_ProcessLib )

# Install files from the package:
atlas_install_python_modules( python/*.py )
atlas_install_joboptions( share/*.py )

