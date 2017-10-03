################################################################################
# Package: DigitizationTests
################################################################################

# Declare the package name:
atlas_subdir( DigitizationTests )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          Control/AthenaBaseComps
                          GaudiKernel
                          PRIVATE
                          Control/AthenaKernel
                          Event/EventInfo
                          Generators/GeneratorObjects
                          InnerDetector/InDetDetDescr/InDetIdentifier
                          InnerDetector/InDetRawEvent/InDetRawData
                          InnerDetector/InDetRawEvent/InDetSimData
                          TestPolicy )

# External dependencies:
find_package( Boost COMPONENTS filesystem thread system )
find_package( ROOT COMPONENTS Core Tree MathCore Hist RIO pthread )

# Component(s) in the package:
atlas_add_component( DigitizationTests
                     src/*.cxx
                     src/components/*.cxx
                     INCLUDE_DIRS ${Boost_INCLUDE_DIRS} ${ROOT_INCLUDE_DIRS}
                     LINK_LIBRARIES ${Boost_LIBRARIES} ${ROOT_LIBRARIES} AthenaBaseComps GaudiKernel AthenaKernel EventInfo GeneratorObjects InDetIdentifier InDetRawData InDetSimData )

# Install files from the package:
atlas_install_headers( DigitizationTests )
atlas_install_python_modules( python/*.py )
atlas_install_joboptions( share/*.py )
atlas_install_runtime( test/DigitizationTests_TestConfiguration.xml )
atlas_install_scripts( scripts/*.sh test/*.sh )

