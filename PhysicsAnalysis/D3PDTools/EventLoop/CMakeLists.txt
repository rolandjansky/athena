# $Id: CMakeLists.txt 804732 2017-05-11 22:02:44Z krumnack $

# The name of the package:
atlas_subdir( EventLoop )

# The package's dependencies:
atlas_depends_on_subdirs(
   PUBLIC
   PhysicsAnalysis/D3PDTools/RootCoreUtils
   PhysicsAnalysis/D3PDTools/SampleHandler
   PRIVATE
   Control/AthToolSupport/AsgTools
   Control/xAODRootAccess )

# Find the needed external(s):
find_package( ROOT COMPONENTS Core RIO Hist Tree TreePlayer )
find_package( GTest )

# Component(s) in the package:
atlas_add_root_dictionary( EventLoop
   EventLoopDictSource
   ROOT_HEADERS EventLoop/Algorithm.h EventLoop/AnaAlgorithmWrapper.h 
   EventLoop/BatchDriver.h
   EventLoop/BatchJob.h EventLoop/BatchSegment.h EventLoop/BatchSample.h
   EventLoop/BatchWorker.h EventLoop/CondorDriver.h EventLoop/DirectDriver.h
   EventLoop/GEDriver.h EventLoop/Job.h
   EventLoop/LLDriver.h EventLoop/LSFDriver.h EventLoop/LocalDriver.h
   EventLoop/OutputStream.h EventLoop/MetricsSvc.h EventLoop/SoGEDriver.h
   EventLoop/StatusCode.h EventLoop/TorqueDriver.h
   EventLoop/VomsProxySvc.h
   EventLoop/Worker.h EventLoop/SlurmDriver.h
   Root/LinkDef.h
   EXTERNAL_PACKAGES ROOT )

atlas_add_library( EventLoop
   EventLoop/*.h EventLoop/*.ihh Root/*.cxx ${EventLoopDictSource}
   PUBLIC_HEADERS EventLoop
   INCLUDE_DIRS ${ROOT_INCLUDE_DIRS} ${GTEST_INCLUDE_DIRS}
   LINK_LIBRARIES ${ROOT_LIBRARIES} ${GTEST_LIBRARIES} RootCoreUtils
   SampleHandler AnaAlgorithmLib
   PRIVATE_LINK_LIBRARIES AsgTools xAODRootAccess )

target_compile_definitions (EventLoop PUBLIC USE_CMAKE)

file (GLOB util_sources RELATIVE "${CMAKE_CURRENT_SOURCE_DIR}" "${CMAKE_CURRENT_SOURCE_DIR}/util/[a-zA-Z0-9]*.cxx")
foreach (source ${util_sources})
  string (REGEX REPLACE "util/(.*).cxx" "\\1" util ${source})
  atlas_add_executable (${util} ${source} LINK_LIBRARIES EventLoop)
endforeach (source ${util_sources})

atlas_add_test (EventLoop_gt_OutputStreamData
      SOURCES test/gt_OutputStreamData.cxx
      INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
      LINK_LIBRARIES ${ROOT_LIBRARIES} EventLoop AsgTestingLib )

# Install files from the package:
atlas_install_scripts( scripts/el_retrieve scripts/el_resubmit scripts/el_wait scripts/el_build_docker )
atlas_install_data( data/*.root data/Dockerfile data/docker_analysis_setup.sh )
