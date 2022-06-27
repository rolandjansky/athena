################################################################################
# Package: TrigHTTBankGen
################################################################################

# Declare the package name:
atlas_subdir( TrigHTTBankGen )


# External dependencies:
find_package( ROOT COMPONENTS Core Tree MathCore Hist RIO pthread Matrix )

# Create a library for non-Gaudi components
atlas_add_library(
    TrigHTTBankGenLib       src/*.cxx 
    PUBLIC_HEADERS          TrigHTTBankGen
    INCLUDE_DIRS            ${HEPPDT_INCLUDE_DIRS} ${ROOT_INCLUDE_DIRS} ${CLHEP_INCLUDE_DIRS} ${EIGEN_INCLUDE_DIRS} ${TBB_INCLUDE_DIRS} ${Boost_INCLUDE_DIRS}
    LINK_LIBRARIES          ${HEPPDT_LIBRARIES} ${ROOT_LIBRARIES} ${CLHEP_LIBRARIES} ${EIGEN_LIBRARIES} ${TBB_LIBRARIES} ${Boost_LIBRARIES}
    AthenaBaseComps         GaudiKernel TrigHTTObjectsLib TrigHTTConfToolsLib TrigHTTMapsLib TrigHTTInputLib TrigHTTSGInputLib TrigHTTBanksLib TrigHTTAlgorithmsLib TrigHTTHoughLib
)

# Declare Gaudi component(s) in the package
atlas_add_component(
    TrigHTTBankGen          src/components/*.cxx
    INCLUDE_DIRS            ${HEPPDT_INCLUDE_DIRS} ${ROOT_INCLUDE_DIRS} ${CLHEP_INCLUDE_DIRS} ${EIGEN_INCLUDE_DIRS} ${TBB_INCLUDE_DIRS} ${Boost_INCLUDE_DIRS}
    LINK_LIBRARIES          TrigHTTBankGenLib
)

# Install files from the package:
atlas_install_python_modules( python/*.py )
atlas_install_runtime( scripts/*_tf.py )


