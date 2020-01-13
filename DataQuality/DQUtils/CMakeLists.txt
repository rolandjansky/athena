################################################################################
# Package: DQUtils
################################################################################

# Declare the package name:
atlas_subdir( DQUtils )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PRIVATE
                          Database/CoraCool )

# External dependencies:
find_package( Boost COMPONENTS filesystem thread system )
find_package( COOL COMPONENTS CoolKernel CoolApplication )
find_package( CORAL COMPONENTS CoralBase CoralKernel RelationalAccess )
find_package( Oracle )
find_package( Python COMPONENTS Development )
find_package( ROOT COMPONENTS Core Tree MathCore Hist RIO pthread )
find_package( six )
find_package( sqlalchemy )

string(REPLACE "-Wl,--as-needed" "" CMAKE_SHARED_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS}")


# Component(s) in the package:
atlas_add_library( DQUtils
                   src/quick_retrieve.cxx
                   src/pythonic_coracool.cxx
                   PUBLIC_HEADERS DQUtils
                   INCLUDE_DIRS ${ORACLE_INCLUDE_DIRS} ${ROOT_INCLUDE_DIRS}
                   PRIVATE_INCLUDE_DIRS ${Boost_INCLUDE_DIRS} ${COOL_INCLUDE_DIRS} ${PYTHON_INCLUDE_DIRS} ${CORAL_INCLUDE_DIRS}
                   LINK_LIBRARIES ${ORACLE_LIBRARIES} ${ROOT_LIBRARIES}
                   PRIVATE_LINK_LIBRARIES ${Boost_LIBRARIES} ${COOL_LIBRARIES} ${PYTHON_LIBRARIES} ${CORAL_LIBRARIES} CoraCool )

atlas_add_dictionary( DQUtilsDict
                      dict/dictionary.h
                      dict/dictionary.xml
                      INCLUDE_DIRS ${ORACLE_INCLUDE_DIRS} ${Boost_INCLUDE_DIRS} ${COOL_INCLUDE_DIRS} ${ROOT_INCLUDE_DIRS} ${PYTHON_INCLUDE_DIRS} ${CORAL_INCLUDE_DIRS}
                      LINK_LIBRARIES ${ORACLE_LIBRARIES} ${Boost_LIBRARIES} ${COOL_LIBRARIES} ${ROOT_LIBRARIES} ${PYTHON_LIBRARIES} ${CORAL_LIBRARIES} CoraCool DQUtils )

# Install files from the package:
atlas_install_python_modules( python/*.py python/ext python/sugar )
atlas_install_scripts( share/count_filled_lbs.py share/dqu_grldiff.py share/dqu_lumi.py share/dqu_grl_extract_period.py share/dqu_dump_periods.py )

# Code quality check
atlas_add_test( flake8
   SCRIPT flake8 --select=ATL,E11,E402,E71,E72,E9,W1,W6,F ${CMAKE_CURRENT_SOURCE_DIR}/python/
   POST_EXEC_SCRIPT nopost.sh )
