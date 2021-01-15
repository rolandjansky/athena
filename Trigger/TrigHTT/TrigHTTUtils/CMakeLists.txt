################################################################################
# Package: TrigHTTUtils
################################################################################

# Declare the package name:
atlas_subdir( TrigHTTUtils )


# Declare the package's dependencies:
atlas_depends_on_subdirs(
    PUBLIC
        AtlasPolicy
        Control/AthenaBaseComps
        Control/AthenaKernel
        GaudiKernel
)

# External dependencies:
find_package( Boost COMPONENTS iostreams program_options filesystem thread system )
find_package( ROOT COMPONENTS Core Tree MathCore Hist RIO pthread )


# Install files from the package:
atlas_install_python_modules( python/*.py )


# Component(s) in the package:
atlas_add_library(
    TrigHTTUtilsLib src/*.cxx TrigHTTUtils/*.h ${TrigHTTUtilsLibDictSource}
    PUBLIC_HEADERS TrigHTTUtils
    INCLUDE_DIRS ${ROOT_INCLUDE_DIRS} ${Boost_INCLUDE_DIRS}
    LINK_LIBRARIES ${ROOT_LIBRARIES} ${Boost_LIBRARIES} GaudiKernel
)

# Define units tests
atlas_add_test(
    TrigHTTVectorMapTest
    SOURCES test/TrigHTTVectorMapTest.cxx
    LINK_LIBRARIES TrigHTTUtilsLib
)

atlas_add_test(
    TrigHTTFunctionsTest
    SOURCES test/TrigHTTFunctionsTest.cxx
    LINK_LIBRARIES TrigHTTUtilsLib
)
