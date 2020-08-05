# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( LumiCalc )

# External dependencies:
find_package( COOL COMPONENTS CoolKernel )
find_package( ROOT COMPONENTS Core Tree MathCore Hist RIO pthread )

# Component(s) in the package:
atlas_add_library( LumiBlockCoolQuery
                   src/CoolQuery.cxx
                   src/LumiBlockRangeContainerConverter.cxx
                   src/LumiCalculator.cxx
                   src/ReplicaSorter.cxx
                   src/LumiBlockCollectionConverter.cxx
                   PUBLIC_HEADERS LumiCalc
                   INCLUDE_DIRS ${COOL_INCLUDE_DIRS} ${ROOT_INCLUDE_DIRS} 
                   LINK_LIBRARIES ${COOL_LIBRARIES} ${ROOT_LIBRARIES} AthenaKernel GoodRunsListsLib LumiBlockData xAODLuminosity )

atlas_add_executable( iLumiCalc
                      src/iLumiCalc.cxx
                      src/cmdline.cxx
                      src/CoolQuery.cxx
                      src/LumiBlockRangeContainerConverter.cxx
                      src/LumiCalculator.cxx
                      src/ReplicaSorter.cxx
                      LINK_LIBRARIES LINK_LIBRARIES CollectionBase CxxUtils DBDataModel DataModelRoot LumiBlockCoolQuery )

# Install files from the package:
atlas_install_python_modules( python/*.py POST_BUILD_CMD ${ATLAS_FLAKE8} )
atlas_install_scripts( share/*.py )

