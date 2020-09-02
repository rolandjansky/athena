################################################################################
# Package: PixelCoralClientUtils
################################################################################

# Declare the package name:
atlas_subdir( PixelCoralClientUtils )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          Control/CxxUtils
                          Database/CoraCool )

# External dependencies:
find_package( COOL COMPONENTS CoolKernel )
find_package( CORAL COMPONENTS CoralBase CoralKernel RelationalAccess )
find_package( ROOT COMPONENTS Core Tree MathCore Hist RIO pthread )

# Component(s) in the package:
atlas_add_library( PixelCoralClientUtils
                   src/CoralClient.cc
                   src/PCDio.cc
                   src/SpecialPixelMap.cc
                   src/PCDDb.cxx
                   src/PixCalibCoralCoolDb.cc
                   src/PixCalibKnowledgeDb.cc
                   PUBLIC_HEADERS PixelCoralClientUtils
                   INCLUDE_DIRS ${CORAL_INCLUDE_DIRS}
                   PRIVATE_INCLUDE_DIRS ${COOL_INCLUDE_DIRS} ${ROOT_INCLUDE_DIRS}
                   LINK_LIBRARIES ${CORAL_LIBRARIES} CxxUtils CoraCool
                   PRIVATE_LINK_LIBRARIES ${COOL_LIBRARIES} ${ROOT_LIBRARIES} )

atlas_add_executable( CORAL-read
                      src/CORAL-read.cc
                      INCLUDE_DIRS ${COOL_INCLUDE_DIRS} ${ROOT_INCLUDE_DIRS} ${CORAL_INCLUDE_DIRS}
                      LINK_LIBRARIES ${COOL_LIBRARIES} ${ROOT_LIBRARIES} ${CORAL_LIBRARIES} CxxUtils CoraCool PixelCoralClientUtils )

atlas_add_executable( CORAL-create
                      src/CORAL-create.cc
                      INCLUDE_DIRS ${COOL_INCLUDE_DIRS} ${ROOT_INCLUDE_DIRS} ${CORAL_INCLUDE_DIRS}
                      LINK_LIBRARIES ${COOL_LIBRARIES} ${ROOT_LIBRARIES} ${CORAL_LIBRARIES} CxxUtils CoraCool PixelCoralClientUtils )

atlas_add_executable( CORAL-fill
                      src/CORAL-fill.cc
                      INCLUDE_DIRS ${COOL_INCLUDE_DIRS} ${ROOT_INCLUDE_DIRS} ${CORAL_INCLUDE_DIRS}
                      LINK_LIBRARIES ${COOL_LIBRARIES} ${ROOT_LIBRARIES} ${CORAL_LIBRARIES} CxxUtils CoraCool PixelCoralClientUtils )

atlas_add_executable( CORAL-listVariables
                      src/CORAL-listVariables.cc
                      INCLUDE_DIRS ${COOL_INCLUDE_DIRS} ${ROOT_INCLUDE_DIRS} ${CORAL_INCLUDE_DIRS}
                      LINK_LIBRARIES ${COOL_LIBRARIES} ${ROOT_LIBRARIES} ${CORAL_LIBRARIES} CxxUtils CoraCool PixelCoralClientUtils )

atlas_add_executable( extractAnalysis
                      src/extractAnalysis.cc
                      INCLUDE_DIRS ${COOL_INCLUDE_DIRS} ${ROOT_INCLUDE_DIRS} ${CORAL_INCLUDE_DIRS}
                      LINK_LIBRARIES ${COOL_LIBRARIES} ${ROOT_LIBRARIES} ${CORAL_LIBRARIES} CxxUtils CoraCool PixelCoralClientUtils )

atlas_add_executable( fixTimewalk
                      src/fixTimewalk.cc
                      INCLUDE_DIRS ${COOL_INCLUDE_DIRS} ${ROOT_INCLUDE_DIRS} ${CORAL_INCLUDE_DIRS}
                      LINK_LIBRARIES ${COOL_LIBRARIES} ${ROOT_LIBRARIES} ${CORAL_LIBRARIES} CxxUtils CoraCool PixelCoralClientUtils )

atlas_add_executable( extractSPM
                      src/extractSPM.cc
                      INCLUDE_DIRS ${COOL_INCLUDE_DIRS} ${ROOT_INCLUDE_DIRS} ${CORAL_INCLUDE_DIRS}
                      LINK_LIBRARIES ${COOL_LIBRARIES} ${ROOT_LIBRARIES} ${CORAL_LIBRARIES} CxxUtils CoraCool PixelCoralClientUtils )

atlas_add_executable( talkToPCDDb
                      src/talkToPCDDb.cc
                      INCLUDE_DIRS ${COOL_INCLUDE_DIRS} ${ROOT_INCLUDE_DIRS} ${CORAL_INCLUDE_DIRS}
                      LINK_LIBRARIES ${COOL_LIBRARIES} ${ROOT_LIBRARIES} ${CORAL_LIBRARIES} CxxUtils CoraCool PixelCoralClientUtils )

atlas_add_executable( calibDbInterface
                      src/calibDbInterface.cc
                      INCLUDE_DIRS ${COOL_INCLUDE_DIRS} ${ROOT_INCLUDE_DIRS} ${CORAL_INCLUDE_DIRS}
                      LINK_LIBRARIES ${COOL_LIBRARIES} ${ROOT_LIBRARIES} ${CORAL_LIBRARIES} CxxUtils CoraCool PixelCoralClientUtils )

