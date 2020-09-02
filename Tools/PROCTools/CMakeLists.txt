################################################################################
# Package: PROCTools
################################################################################

# Declare the package name:
atlas_subdir( PROCTools )

# Install files from the package:
atlas_install_python_modules( python/*.py )
atlas_install_scripts( python/compareTCTs.py python/UploadAMITag.py python/UploadTfAMITag.py python/getCurrentCOMCONDTag.py python/RunTier0Tests.py python/ExtractEvents.py python/checkUPD1.py python/runDiffRootOnChanged.py python/provideTwikiSummary.py python/evaluateDiffRoot.py python/outputTest_v2.py  python/CreateTierZeroArgdict.py python/root_lsr_rank.py python/xAODDigest.py )

# Install digest reference files for easy access
atlas_install_data( data/*.ref )
