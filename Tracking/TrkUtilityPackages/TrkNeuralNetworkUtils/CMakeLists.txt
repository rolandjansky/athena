################################################################################
# Package: TrkNeuralNetworkUtils
################################################################################

# Declare the package name:
atlas_subdir( TrkNeuralNetworkUtils )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          Control/AthenaBaseComps
                          PRIVATE
                          GaudiKernel )

# External dependencies:
find_package( Boost COMPONENTS filesystem thread system )
find_package( ROOT COMPONENTS Matrix Cint Core Tree MathCore Hist RIO pthread MathMore Minuit Minuit2 Physics HistPainter Rint )

# Component(s) in the package:
atlas_add_root_dictionary( TrkNeuralNetworkUtilsLib
                           TrkNeuralNetworkUtilsLibDictSource
                           ROOT_HEADERS TrkNeuralNetworkUtils/TTrainedNetwork.h TrkNeuralNetworkUtils/LinkDef.h
                           EXTERNAL_PACKAGES ROOT Boost )

atlas_add_library( TrkNeuralNetworkUtilsLib
                   src/*.cxx
                   ${TrkNeuralNetworkUtilsLibDictSource}
                   PUBLIC_HEADERS TrkNeuralNetworkUtils
                   INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
                   PRIVATE_INCLUDE_DIRS ${Boost_INCLUDE_DIRS}
                   LINK_LIBRARIES ${ROOT_LIBRARIES} AthenaBaseComps
                   PRIVATE_LINK_LIBRARIES ${Boost_LIBRARIES} GaudiKernel )

atlas_add_component( TrkNeuralNetworkUtils
                     src/components/*.cxx
                     INCLUDE_DIRS ${ROOT_INCLUDE_DIRS} ${Boost_INCLUDE_DIRS}
                     LINK_LIBRARIES ${ROOT_LIBRARIES} ${Boost_LIBRARIES} AthenaBaseComps GaudiKernel TrkNeuralNetworkUtilsLib )

