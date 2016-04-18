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
find_package( PythonLibs )
find_package( ROOT COMPONENTS Core Tree MathCore Hist RIO pthread )
find_package( pyanalysis )
find_package( pygraphics )
find_package( pytools )

string(REPLACE "-Wl,--as-needed" "" CMAKE_SHARED_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS}")

# tag NEEDS_PYCOOL was not recognized in automatic conversion in cmt2cmake

# Component(s) in the package:
atlas_add_library( DQUtils
                   src/quick_retrieve.cxx
                   src/pythonic_coracool.cxx
                   PUBLIC_HEADERS DQUtils
                   INCLUDE_DIRS ${PYTOOLS_INCLUDE_DIRS} ${ORACLE_INCLUDE_DIRS} ${ROOT_INCLUDE_DIRS}
                   PRIVATE_INCLUDE_DIRS ${Boost_INCLUDE_DIRS} ${COOL_INCLUDE_DIRS} ${PYTHON_INCLUDE_DIRS} ${CORAL_INCLUDE_DIRS} ${PYGRAPHICS_INCLUDE_DIRS} ${PYANALYSIS_INCLUDE_DIRS}
                   LINK_LIBRARIES ${PYTOOLS_LIBRARIES} ${ORACLE_LIBRARIES} ${ROOT_LIBRARIES}
                   PRIVATE_LINK_LIBRARIES ${Boost_LIBRARIES} ${COOL_LIBRARIES} ${PYTHON_LIBRARIES} ${CORAL_LIBRARIES} ${PYGRAPHICS_LIBRARIES} ${PYANALYSIS_LIBRARIES} CoraCool )

atlas_add_dictionary( DQUtilsDict
                      dict/dictionary.h
                      dict/dictionary.xml
                      INCLUDE_DIRS ${PYTOOLS_INCLUDE_DIRS} ${ORACLE_INCLUDE_DIRS} ${Boost_INCLUDE_DIRS} ${COOL_INCLUDE_DIRS} ${ROOT_INCLUDE_DIRS} ${PYTHON_INCLUDE_DIRS} ${CORAL_INCLUDE_DIRS} ${PYGRAPHICS_INCLUDE_DIRS} ${PYANALYSIS_INCLUDE_DIRS}
                      LINK_LIBRARIES ${PYTOOLS_LIBRARIES} ${ORACLE_LIBRARIES} ${Boost_LIBRARIES} ${COOL_LIBRARIES} ${ROOT_LIBRARIES} ${PYTHON_LIBRARIES} ${CORAL_LIBRARIES} ${PYGRAPHICS_LIBRARIES} ${PYANALYSIS_LIBRARIES} CoraCool DQUtils )

# Install files from the package:
atlas_install_python_modules( python/*.py python/ext python/sugar )
atlas_install_scripts( share/count_filled_lbs.py share/dqu_grldiff.py share/dqu_lumi.py share/dqu_grl_extract_period.py share/dqu_dump_periods.py )

