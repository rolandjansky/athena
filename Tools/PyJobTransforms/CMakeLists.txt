################################################################################
# Package: PyJobTransforms
################################################################################

# Declare the package name:
atlas_subdir( PyJobTransforms )

# External dependencies:
find_package( cx_Oracle )
find_package( future )

# Install files from the package:
atlas_install_python_modules( python/*.py POST_BUILD_CMD ${ATLAS_FLAKE8} )
atlas_install_joboptions( share/*.py )
atlas_install_runtime( scripts/*.py POST_BUILD_CMD ${ATLAS_FLAKE8} )
atlas_install_scripts( scripts/HelloWorld_tf.py POST_BUILD_CMD ${ATLAS_FLAKE8} )
atlas_install_generic( share/*.db
                       DESTINATION share
                       EXECUTABLE )
foreach( test Echo_tf LogFileScanner Sleep_tf
         transform trfAMI trfArgs trfDecorators trfEnv
         trfExceptions trfExe trfExitCodes trfFileUtils trfGraph trfJobOptions trfLogger
         trfMPTools trfReports trfReportsSignaledExit trfSignal trfSignatures
         trfUtils trfUtilsDBRelease trfUtilsParallelJobProcessor trfUtilsParallelJobProcessorData
         trfValidateRootFile trfValidation )
   atlas_add_test( ${test}
      SCRIPT test/test_${test}.py
      PROPERTIES TIMEOUT 300
   )
endforeach()

if( NOT SIMULATIONBASE AND NOT GENERATIONBASE )
  foreach( test RAWMerge_tf
           trfSubstepIntegration )
     atlas_add_test( ${test}
        SCRIPT test/test_${test}.py
     )
  endforeach()

  atlas_add_test( HelloWorldTrf
        SCRIPT test/test_HelloWorldTrf.sh
	)	
endif()

#The following tests currently fail in master and should be revised
#AODMerge_tf
#AtlasG4_SimTTBar_tf
#ESDMerge_tf
#EVNTMerge_tf
#FullChain_tf
#HISTMerge_tf
#LogFileScanner
#NTUPMerge_tf
#Reco_AthenaMP_tf
#Reco_EOS_tf
#Reco_MC_tf
#Reco_Tier0_tf
#Reco_q222_tf
#Reco_tf
#TAGMerge_tf
#trfArgClasses
#trfArgClassesATLAS
#trfReports
#trfReportsSignaledExit
#trfValidation
