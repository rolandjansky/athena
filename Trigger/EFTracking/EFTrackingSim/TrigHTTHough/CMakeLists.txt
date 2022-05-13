################################################################################
# Package: TrigHTTHough
################################################################################

# Declare the package name:
atlas_subdir( TrigHTTHough )


# External dependencies:
find_package( Boost )
find_package( ROOT COMPONENTS Core Tree MathCore Hist RIO pthread )

# Component(s) in the package:
atlas_add_library(
    TrigHTTHoughLib        src/*.cxx TrigHTTHough/*.h
    PUBLIC_HEADERS              TrigHTTHough
    INCLUDE_DIRS                ${ROOT_INCLUDE_DIRS} ${Boost_INCLUDE_DIRS} ${EIGEN_INCLUDE_DIRS} ${LWTNN_INCLUDE_DIRS}
    LINK_LIBRARIES              ${ROOT_LIBRARIES} ${Boost_LIBRARIES} ${EIGEN_LIBRARIES} ${LWTNN_LIBRARIES}
                                AthenaBaseComps GaudiKernel TrigHTTBanksLib TrigHTTConfToolsLib TrigHTTMapsLib TrigHTTObjectsLib 
)

atlas_add_component(
    TrigHTTHough           src/components/*.cxx
    LINK_LIBRARIES              TrigHTTHoughLib
)

# Install files from the package:
atlas_install_python_modules( python/*.py )
atlas_install_scripts( scripts/Hough_plots.py )


