################################################################################
# Package: TrigTimeAlgs
################################################################################

# Declare the package name:
atlas_subdir( TrigTimeAlgs )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          Control/AthenaBaseComps
                          Control/AthenaKernel
                          Control/DataModel
                          Control/SGTools
                          GaudiKernel
                          PRIVATE
                          Control/CxxUtils
                          AtlasTest/TestTools )

# External dependencies:
find_package( Boost COMPONENTS filesystem thread system )
find_package( ROOT COMPONENTS Core Tree MathCore Hist RIO pthread )

# Component(s) in the package:
atlas_add_library( TrigTimeAlgsLib
                   src/*.cxx
                   PUBLIC_HEADERS TrigTimeAlgs
                   INCLUDE_DIRS ${Boost_INCLUDE_DIRS} ${ROOT_INCLUDE_DIRS}
                   LINK_LIBRARIES ${Boost_LIBRARIES} ${ROOT_LIBRARIES} AthenaBaseComps AthenaKernel DataModel SGTools GaudiKernel
                   PRIVATE_LINK_LIBRARIES TestTools )

atlas_add_component( TrigTimeAlgs
                     src/components/*.cxx
                     INCLUDE_DIRS ${Boost_INCLUDE_DIRS} ${ROOT_INCLUDE_DIRS}
                     LINK_LIBRARIES ${Boost_LIBRARIES} ${ROOT_LIBRARIES} AthenaBaseComps AthenaKernel DataModel SGTools GaudiKernel TestTools TrigTimeAlgsLib )

atlas_add_test( TimerSvc_test
                SOURCES
                test/TimerSvc_test.cxx
                INCLUDE_DIRS ${Boost_INCLUDE_DIRS} ${ROOT_INCLUDE_DIRS}
                LINK_LIBRARIES ${Boost_LIBRARIES} ${ROOT_LIBRARIES} AthenaBaseComps AthenaKernel DataModel SGTools GaudiKernel TestTools TrigTimeAlgsLib
                EXTRA_PATTERNS ".*" 
                ENVIRONMENT "JOBOPTSEARCHPATH=${CMAKE_CURRENT_SOURCE_DIR}/share" )

# Install files from the package:
atlas_install_joboptions( share/jobOfragment_TrigTimerSvc.py )

