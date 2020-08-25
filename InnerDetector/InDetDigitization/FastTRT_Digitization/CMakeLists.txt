################################################################################
# Package: FastTRT_Digitization
################################################################################

# Declare the package name:
atlas_subdir( FastTRT_Digitization )

# External dependencies:
find_package( CLHEP )
find_package( Eigen )
find_package( ROOT COMPONENTS Core Tree MathCore Hist RIO pthread )

# Component(s) in the package:
atlas_add_component( FastTRT_Digitization
                     src/*.cxx
                     src/components/*.cxx
                     INCLUDE_DIRS ${ROOT_INCLUDE_DIRS} ${CLHEP_INCLUDE_DIRS} ${EIGEN_INCLUDE_DIRS}
                     LINK_LIBRARIES ${ROOT_LIBRARIES} ${CLHEP_LIBRARIES} ${EIGEN_LIBRARIES} AthenaBaseComps AthenaKernel PileUpToolsLib StoreGateLib SGtests Identifier EventInfo xAODEventInfo GaudiKernel TRT_ConditionsServicesLib InDetPrepRawData InDetSimEvent HitManagement TrkParameters TrkTruthData TrkToolInterfaces GeoPrimitives GeneratorObjects InDetIdentifier InDetReadoutGeometry TRT_ReadoutGeometry InDetSimData TrkDetElementBase EventContainers TRT_DriftFunctionToolLib )

# Install files from the package:
atlas_install_python_modules( python/*.py )
atlas_install_joboptions( share/*.py )

