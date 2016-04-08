################################################################################
# Package: ValgrindRTTJobs
################################################################################

# Declare the package name:
atlas_subdir( ValgrindRTTJobs )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PRIVATE
                          Control/AthenaBaseComps
                          GaudiKernel
                          TestPolicy
                          Tools/PyJobTransformsCore )

# Component(s) in the package:
atlas_add_component( ValgrindRTTJobs
                     src/*.cxx
                     src/components/*.cxx
                     LINK_LIBRARIES AthenaBaseComps GaudiKernel )

# Install files from the package:
atlas_install_runtime( test/ValgrindRTTJobs_TestConfiguration.xml share/PostProcessValgrind.py share/memConsumption.C share/historyNightlies.C share/*.supp share/processValgrindOutput.sh share/dhat-postprocess.pl )

