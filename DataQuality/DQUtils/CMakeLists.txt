# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( DQUtils )

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
                   PRIVATE_INCLUDE_DIRS ${Boost_INCLUDE_DIRS} ${COOL_INCLUDE_DIRS} ${Python_INCLUDE_DIRS} ${CORAL_INCLUDE_DIRS}
                   LINK_LIBRARIES ${ORACLE_LIBRARIES} ${ROOT_LIBRARIES}
                   PRIVATE_LINK_LIBRARIES ${Boost_LIBRARIES} ${COOL_LIBRARIES} ${Python_LIBRARIES} ${CORAL_LIBRARIES} CoraCool )

atlas_add_dictionary( DQUtilsDict
                      dict/dictionary.h
                      dict/dictionary.xml
                      INCLUDE_DIRS ${ORACLE_INCLUDE_DIRS} ${Boost_INCLUDE_DIRS} ${COOL_INCLUDE_DIRS} ${ROOT_INCLUDE_DIRS} ${Python_INCLUDE_DIRS} ${CORAL_INCLUDE_DIRS}
                      LINK_LIBRARIES ${ORACLE_LIBRARIES} ${Boost_LIBRARIES} ${COOL_LIBRARIES} ${ROOT_LIBRARIES} ${Python_LIBRARIES} ${CORAL_LIBRARIES} CoraCool DQUtils )

# Install files from the package:
atlas_install_python_modules( python/*.py python/ext python/sugar
   POST_BUILD_CMD ${ATLAS_FLAKE8} --extend-select E11,E402,W1 --extend-ignore E731 )

atlas_install_scripts( share/count_filled_lbs.py share/dqu_grldiff.py share/dqu_lumi.py share/dqu_grl_extract_period.py share/dqu_dump_periods.py )
