################################################################################
# Package: InDetConfig
################################################################################

# Declare the package name:
atlas_subdir( InDetConfig )

# Install files from the package:
atlas_install_python_modules( python/*.py POST_BUILD_CMD ${ATLAS_FLAKE8} )
#atlas_install_scripts( test/*.py POST_BUILD_CMD ${ATLAS_FLAKE8} )
#atlas_install_joboptions( share/*.py POST_BUILD_CMD ${ATLAS_FLAKE8} )
atlas_install_runtime( test/ITkTest.py )

atlas_add_test( TrackingCutsFlags_test
    SCRIPT python -m InDetConfig.TrackingCutsFlags
    POST_EXEC_SCRIPT nopost.sh)
 
atlas_add_test( BackTrackingConfig_test
    SCRIPT python -m InDetConfig.BackTrackingConfig --norun
    POST_EXEC_SCRIPT nopost.sh)

atlas_add_test( TRTSegmentFindingConfig_test
    SCRIPT python -m InDetConfig.TRTSegmentFindingConfig
    POST_EXEC_SCRIPT nopost.sh)

atlas_add_test( TrackRecoConfig_test
    SCRIPT python -m InDetConfig.TrackRecoConfig
    PROPERTIES TIMEOUT 300
    POST_EXEC_SCRIPT nopost.sh)

atlas_add_test( ITkTrackRecoConfig_test
    SCRIPT python -m InDetConfig.ITkTrackRecoConfig --norun
    POST_EXEC_SCRIPT nopost.sh)

atlas_add_test( VertexFindingConfig_test
    SCRIPT python -m InDetConfig.VertexFindingConfig 
    POST_EXEC_SCRIPT nopost.sh)

atlas_add_test( VertexFindingConfigActs_test
    SCRIPT python -m InDetConfig.VertexFindingConfig ActsGaussAdaptiveMultiFinding
    POST_EXEC_SCRIPT nopost.sh)

atlas_add_test( VertexFindingConfigIterative_test
    SCRIPT python -m InDetConfig.VertexFindingConfig IterativeFinding
    POST_EXEC_SCRIPT nopost.sh)

    