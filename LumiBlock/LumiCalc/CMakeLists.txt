################################################################################
# Package: LumiCalc
################################################################################

# Declare the package name:
atlas_subdir( LumiCalc )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          Control/AthenaKernel
                          DataQuality/GoodRunsLists
                          Event/xAOD/xAODLuminosity
                          LumiBlock/LumiBlockData
                          PRIVATE
                          Control/DataModelRoot
                          Database/APR/CollectionBase
                          Database/AthenaPOOL/DBDataModel
                          Database/CoolConvUtilities
                          Database/CoraCool )

# External dependencies:
find_package( COOL COMPONENTS CoolKernel )
find_package( CORAL COMPONENTS CoralBase CoralKernel RelationalAccess )
find_package( ROOT COMPONENTS Core Tree MathCore Hist RIO pthread )

# Component(s) in the package:
atlas_add_library( LumiBlockCoolQuery
                   src/CoolQuery.cxx
                   src/LumiBlockRangeContainerConverter.cxx
                   src/LumiCalculator.cxx
                   src/ReplicaSorter.cxx
                   src/LumiBlockCollectionConverter.cxx
                   PUBLIC_HEADERS LumiCalc
                   INCLUDE_DIRS ${COOL_INCLUDE_DIRS} ${ROOT_INCLUDE_DIRS} ${CORAL_INCLUDE_DIRS}
                   LINK_LIBRARIES ${COOL_LIBRARIES} ${ROOT_LIBRARIES} ${CORAL_LIBRARIES} AthenaKernel xAODLuminosity LumiBlockData GoodRunsListsLib
                   PRIVATE_LINK_LIBRARIES DataModelRoot CollectionBase DBDataModel )

atlas_add_executable( iLumiCalc
                      src/iLumiCalc.cxx
                      src/cmdline.cxx
                      src/CoolQuery.cxx
                      src/LumiBlockRangeContainerConverter.cxx
                      src/LumiCalculator.cxx
                      src/ReplicaSorter.cxx
                      INCLUDE_DIRS ${COOL_INCLUDE_DIRS} ${ROOT_INCLUDE_DIRS} ${CORAL_INCLUDE_DIRS}
                      LINK_LIBRARIES ${COOL_LIBRARIES} ${ROOT_LIBRARIES} ${CORAL_LIBRARIES} AthenaKernel GoodRunsListsLib xAODLuminosity LumiBlockData DataModelRoot CollectionBase DBDataModel LumiBlockCoolQuery )

# Install files from the package:
atlas_install_python_modules( python/*.py )
atlas_install_scripts( share/*.py )

