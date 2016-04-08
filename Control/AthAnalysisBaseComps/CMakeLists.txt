################################################################################
# Package: AthAnalysisBaseComps
################################################################################

# Declare the package name:
atlas_subdir( AthAnalysisBaseComps )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          Control/AthenaBaseComps
                          Control/StoreGate
                          Database/IOVDbDataModel
                          GaudiKernel )

# Component(s) in the package:
atlas_add_library( AthAnalysisBaseComps
                   src/*.cxx
                   PUBLIC_HEADERS AthAnalysisBaseComps
                   LINK_LIBRARIES AthenaBaseComps IOVDbDataModel GaudiKernel StoreGateLib SGtests )

# Install files from the package:
atlas_install_joboptions( share/*.py )

