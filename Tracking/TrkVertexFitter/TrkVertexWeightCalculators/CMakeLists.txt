################################################################################
# Package: TrkVertexWeightCalculators
################################################################################

# Declare the package name:
atlas_subdir( TrkVertexWeightCalculators )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          Control/AthenaBaseComps
                          DetectorDescription/GeoPrimitives
                          Event/xAOD/xAODTracking
                          GaudiKernel
                          Tracking/TrkVertexFitter/TrkVertexFitterInterfaces
                          PRIVATE
                          Generators/GeneratorObjects
                          Tracking/TrkEvent/TrkParameters
                          Tracking/TrkEvent/VxVertex
                          Tracking/TrkUtilityPackages/TrkNeuralNetworkUtils )

# External dependencies:
find_package( Eigen )
find_package( ROOT COMPONENTS Core Tree MathCore Hist RIO pthread MathMore Minuit Minuit2 Matrix Physics HistPainter Rint )

# Component(s) in the package:
atlas_add_component( TrkVertexWeightCalculators
                     src/*.cxx
                     src/components/*.cxx
                     INCLUDE_DIRS ${ROOT_INCLUDE_DIRS} ${EIGEN_INCLUDE_DIRS}
                     LINK_LIBRARIES ${ROOT_LIBRARIES} ${EIGEN_LIBRARIES} AthenaBaseComps GeoPrimitives xAODTracking GaudiKernel TrkVertexFitterInterfaces 
                     GeneratorObjects TrkParameters VxVertex TrkNeuralNetworkUtilsLib)

# Install files from the package:
atlas_install_headers( TrkVertexWeightCalculators )
atlas_install_runtime( share/*.root )

