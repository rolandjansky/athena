################################################################################
# Package: TrigEgammaValidation
################################################################################

# Declare the package name:
atlas_subdir( TrigEgammaValidation )

# Install files from the package:
atlas_install_docs( doc/Summary-dev/*.php doc/Summary-dev/*.html doc/Summary-dev/content.xml doc/Summary-dev/style.css doc/Summary-dev/*.gif )
atlas_install_joboptions( share/testAthenaTrigRDOtoAOD_RTT.py share/testNavZeeTPExample.py share/TrigEgammaValidation_RTT_Chains.py )
atlas_install_runtime( share/*.C share/*.py test/TrigEgammaValidation_TestConfiguration.xml )

