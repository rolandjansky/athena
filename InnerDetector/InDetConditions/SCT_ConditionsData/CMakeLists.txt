################################################################################
# Package: SCT_ConditionsData
################################################################################

# Declare the package name:
atlas_subdir( SCT_ConditionsData )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          Control/CLIDSvc
                          Database/AthenaPOOL/AthenaPoolUtilities
                          DetectorDescription/Identifier )

# Component(s) in the package:
atlas_add_library( SCT_ConditionsData
                   src/*.cxx
                   PUBLIC_HEADERS SCT_ConditionsData
                   LINK_LIBRARIES AthenaPoolUtilities Identifier )

