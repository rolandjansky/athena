################################################################################
# Package: HIInDetValidation
################################################################################

# Declare the package name:
atlas_subdir( HIInDetValidation )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PRIVATE
                          TestPolicy )

# Install files from the package:
atlas_install_joboptions( share/*.py )
atlas_install_runtime( test/HIInDetValidation_TestConfiguration.xml share/HIInDetValidation*.py macros/HIInDetValidation_Eff*.C macros/HIInDetValidation_Fakes*.C macros/HIInDetValidation_CoolDozer.py share/ARTT_Plots.html macros/png_file.png macros/HIInDetValidation_Loose_Vs_Tight.C macros/HIInDetValidation_Vertexing.C share/HIInDetTrkD3PD_jobOptions.py )

