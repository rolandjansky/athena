################################################################################
# Package: TrigInDetAnalysisUser
################################################################################

# Declare the package name:
atlas_subdir( TrigInDetAnalysisUser )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PRIVATE
                          Trigger/TrigAnalysis/TrigInDetAnalysis
                          Trigger/TrigAnalysis/TrigInDetAnalysisExample
                          Trigger/TrigAnalysis/TrigInDetAnalysisUtils )

# External dependencies:
find_package( ROOT COMPONENTS Graf Gpad Cint Core Tree MathCore Hist RIO pthread )

include_directories(Resplot/src Readcards/src)

# Component(s) in the package:
atlas_add_root_dictionary( Resplot
                           ResplotDictSource
                           ROOT_HEADERS Resplot/src/Resplot.h
                           EXTERNAL_PACKAGES ROOT )

atlas_add_library( Resplot
                   Resplot/src/Resplot.cxx
                   Resplot/src/generate.cxx
                   Resplot/src/rmsFrac.cxx
                   ${ResplotDictSource}
                   NO_PUBLIC_HEADERS
                   PRIVATE_INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
                   LINK_LIBRARIES TrigInDetAnalysisExampleLib
                   PRIVATE_LINK_LIBRARIES ${ROOT_LIBRARIES} TrigInDetAnalysis TrigInDetAnalysisUtils )

atlas_add_library( Readcards
                   Readcards/src/IReadCards.cxx
                   Readcards/src/ReadCards.cxx
                   Readcards/src/Value.cxx
                   Readcards/src/utils.cxx
                   NO_PUBLIC_HEADERS
                   PRIVATE_INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
                   LINK_LIBRARIES TrigInDetAnalysisExampleLib Resplot
                   PRIVATE_LINK_LIBRARIES ${ROOT_LIBRARIES} TrigInDetAnalysis TrigInDetAnalysisUtils )

atlas_add_library( TIDA
                   Analysis/src/ConfAnalysis.cxx
                   Analysis/src/ConfVtxAnalysis.cxx
                   Analysis/src/PurityAnalysis.cxx
                   Analysis/src/globals.cxx
                   NO_PUBLIC_HEADERS
                   PRIVATE_INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
                   LINK_LIBRARIES TrigInDetAnalysisExampleLib Resplot Readcards
                   PRIVATE_LINK_LIBRARIES ${ROOT_LIBRARIES} TrigInDetAnalysis TrigInDetAnalysisUtils )

atlas_add_library( TIDAcomputils
                   Analysis/src/computils.cxx
                   NO_PUBLIC_HEADERS
                   PRIVATE_INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
                   LINK_LIBRARIES TrigInDetAnalysisExampleLib Resplot Readcards TIDA
                   PRIVATE_LINK_LIBRARIES ${ROOT_LIBRARIES} TrigInDetAnalysis TrigInDetAnalysisUtils )

atlas_add_executable( TIDAreader
                      Analysis/src/reader.cxx
                      INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
                      LINK_LIBRARIES ${ROOT_LIBRARIES} TrigInDetAnalysis TrigInDetAnalysisExampleLib TrigInDetAnalysisUtils Resplot Readcards TIDA TIDAcomputils )

atlas_add_executable( TIDArdict
                      Analysis/src/rmain.cxx
                      Analysis/src/TagNProbe.cxx
                      INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
                      LINK_LIBRARIES ${ROOT_LIBRARIES} TrigInDetAnalysis TrigInDetAnalysisExampleLib TrigInDetAnalysisUtils Resplot Readcards TIDA TIDAcomputils )

atlas_add_executable( TIDAcomparitor
                      Analysis/src/comparitor.cxx
                      Analysis/src/AtlasStyle.cxx
                      Analysis/src/AtlasLabels.cxx
                      INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
                      LINK_LIBRARIES ${ROOT_LIBRARIES} TrigInDetAnalysis TrigInDetAnalysisExampleLib TrigInDetAnalysisUtils Resplot Readcards TIDA TIDAcomputils )

atlas_add_executable( TIDAcpucost
                      Analysis/src/cpucost.cxx
                      INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
                      LINK_LIBRARIES ${ROOT_LIBRARIES} TrigInDetAnalysis TrigInDetAnalysisExampleLib TrigInDetAnalysisUtils Resplot Readcards TIDA TIDAcomputils )

atlas_add_executable( TIDAchains
                      Analysis/src/chains.cxx
                      INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
                      LINK_LIBRARIES ${ROOT_LIBRARIES} TrigInDetAnalysis TrigInDetAnalysisExampleLib TrigInDetAnalysisUtils Resplot Readcards TIDA TIDAcomputils )

atlas_add_executable( TIDAskim
                      Analysis/src/skim.cxx
                      INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
                      LINK_LIBRARIES ${ROOT_LIBRARIES} TrigInDetAnalysis TrigInDetAnalysisExampleLib TrigInDetAnalysisUtils Resplot Readcards TIDA TIDAcomputils )

atlas_add_executable( TIDAfastadd
                      Analysis/src/fastadd.cxx
                      INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
                      LINK_LIBRARIES ${ROOT_LIBRARIES} TrigInDetAnalysis TrigInDetAnalysisExampleLib TrigInDetAnalysisUtils Resplot Readcards TIDA TIDAcomputils )

atlas_add_executable( TIDArefit
                      Analysis/src/refit.cxx
                      INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
                      LINK_LIBRARIES ${ROOT_LIBRARIES} TrigInDetAnalysis TrigInDetAnalysisExampleLib TrigInDetAnalysisUtils Resplot Readcards TIDA TIDAcomputils )

atlas_add_executable( TIDAlistroot
                      Analysis/src/listroot.cxx
                      INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
                      LINK_LIBRARIES ${ROOT_LIBRARIES} TrigInDetAnalysis TrigInDetAnalysisExampleLib TrigInDetAnalysisUtils Resplot Readcards TIDA TIDAcomputils )

atlas_add_executable( TIDAmakeSmallRefFile
                      Analysis/src/makeSmallRefFile.cxx
                      INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
                      LINK_LIBRARIES ${ROOT_LIBRARIES} TrigInDetAnalysis TrigInDetAnalysisExampleLib TrigInDetAnalysisUtils Resplot Readcards TIDA TIDAcomputils )

atlas_add_executable( TIDAruntool
                      Analysis/src/runtool.cxx 
                      Analysis/src/computils.cxx
                      INCLUDE_DIRS  ${ROOT_INCLUDE_DIRS}
                      LINK_LIBRARIES  ${ROOT_LIBRARIES}   )

atlas_add_executable( TIDAsb
                      Analysis/src/chainparser.cxx
                      INCLUDE_DIRS 
                      LINK_LIBRARIES  Readcards  )


# Disable naming convention checker.
# FIXME: This should be fixed properly once run2 is finished.
if( ${CMAKE_CXX_FLAGS} MATCHES "libchecker_gccplugins" )
  set_target_properties( Resplot PROPERTIES COMPILE_FLAGS -fplugin-arg-libchecker_gccplugins-checkers=no-naming )
  set_target_properties( Readcards PROPERTIES COMPILE_FLAGS -fplugin-arg-libchecker_gccplugins-checkers=no-naming )
  set_target_properties( TIDA PROPERTIES COMPILE_FLAGS -fplugin-arg-libchecker_gccplugins-checkers=no-naming )
  set_target_properties( TIDAcomputils PROPERTIES COMPILE_FLAGS -fplugin-arg-libchecker_gccplugins-checkers=no-naming )
  set_target_properties( TIDAruntool PROPERTIES COMPILE_FLAGS -fplugin-arg-libchecker_gccplugins-checkers=no-naming )
  set_target_properties( TIDAcpucost PROPERTIES COMPILE_FLAGS -fplugin-arg-libchecker_gccplugins-checkers=no-naming )
  set_target_properties( TIDAsb PROPERTIES COMPILE_FLAGS -fplugin-arg-libchecker_gccplugins-checkers=no-naming )
  set_target_properties( TIDAreader PROPERTIES COMPILE_FLAGS -fplugin-arg-libchecker_gccplugins-checkers=no-naming )
  set_target_properties( TIDArefit PROPERTIES COMPILE_FLAGS -fplugin-arg-libchecker_gccplugins-checkers=no-naming )
  set_target_properties( TIDAcomparitor PROPERTIES COMPILE_FLAGS -fplugin-arg-libchecker_gccplugins-checkers=no-naming )
  set_target_properties( TIDArdict PROPERTIES COMPILE_FLAGS -fplugin-arg-libchecker_gccplugins-checkers=no-naming )
  set_target_properties( TIDAlistroot PROPERTIES COMPILE_FLAGS -fplugin-arg-libchecker_gccplugins-checkers=no-naming )
  set_target_properties( TIDAskim PROPERTIES COMPILE_FLAGS -fplugin-arg-libchecker_gccplugins-checkers=no-naming )
  set_target_properties( TIDAfastadd PROPERTIES COMPILE_FLAGS -fplugin-arg-libchecker_gccplugins-checkers=no-naming )
endif()

# Install files from the package:
atlas_install_runtime( share/TIDA*.dat share/Test*.dat share/TIDA*.py )

atlas_install_scripts( scripts/TIDA*.sh )
