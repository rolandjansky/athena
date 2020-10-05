# $Id: CMakeLists.txt 785966 2016-11-23 12:31:31Z will $
################################################################################
# Package: ReweightUtils
################################################################################

# Declare the package name:
atlas_subdir( ReweightUtils )

# Extra dependencies, based on the environment:
set( extra_dep )
if( NOT XAOD_STANDALONE )
   set( extra_dep Control/AthenaBaseComps GaudiKernel )
endif()

# External dependencies:
find_package( Lhapdf )
find_package( ROOT COMPONENTS Core Tree MathCore Hist RIO )

# Libraries in the package:
atlas_add_root_dictionary( ReweightUtilsLib ReweightUtilsDictSource
   ROOT_HEADERS ReweightUtils/APWeightEntry.h ReweightUtils/APReweightBase.h
   ReweightUtils/APReweight.h ReweightUtils/APReweight2D.h
   ReweightUtils/APReweight3D.h ReweightUtils/APReweightND.h
   ReweightUtils/APWeightHist.h ReweightUtils/APEvtWeight.h
   ReweightUtils/APWeightSum.h ReweightUtils/APWeightSumEnsemble.h
   Root/LinkDef.h
   EXTERNAL_PACKAGES ROOT )

atlas_add_library( ReweightUtilsLib
   ReweightUtils/*.h Root/*.cxx ${ReweightUtilsDictSource}
   PUBLIC_HEADERS ReweightUtils
   INCLUDE_DIRS ${LHAPDF_INCLUDE_DIRS} ${ROOT_INCLUDE_DIRS}
   LINK_LIBRARIES ${LHAPDF_LIBRARIES} ${ROOT_LIBRARIES} AsgTools xAODEventInfo
   xAODBase PATInterfaces AsgAnalysisInterfaces
   PRIVATE_LINK_LIBRARIES xAODParticleEvent xAODTruth PathResolver xAODCutFlow )

if( NOT XAOD_STANDALONE )
   atlas_add_component( ReweightUtils
      src/*.h src/*.cxx src/components/*.cxx
      INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
      LINK_LIBRARIES ${ROOT_LIBRARIES} AthenaBaseComps xAODCutFlow 
      GaudiKernel ReweightUtilsLib )
endif()

atlas_add_dictionary( ReweightUtilsDict
   ReweightUtils/ReweightUtilsDict.h
   ReweightUtils/selection.xml
   LINK_LIBRARIES ReweightUtilsLib )

# Test(s) in the package:
atlas_add_test( ut_ParticleScaleFactorTool_test
   SOURCES test/ut_ParticleScaleFactorTool_test.cxx
   INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
   LINK_LIBRARIES ${ROOT_LIBRARIES} AsgTools AsgAnalysisInterfaces
   PATInterfaces xAODEgamma CxxUtils
   PROPERTIES TIMEOUT 600 )

# Install files from the package:
atlas_install_python_modules( python/*.py  POST_BUILD_CMD ${ATLAS_FLAKE8} )
