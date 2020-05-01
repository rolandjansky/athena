################################################################################
# Package: SCT_ConditionsData
################################################################################

# Declare the package name:
atlas_subdir( SCT_ConditionsData )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          Control/AthenaBaseComps
                          Control/AthenaKernel
                          Database/AthenaPOOL/AthenaPoolUtilities
                          DetectorDescription/Identifier )

atlas_add_dictionary( SCT_ConditionsDataDict
                      SCT_ConditionsData/SCT_ConditionsDataDict.h
                      SCT_ConditionsData/selection.xml )

# Component(s) in the package:
atlas_add_library( SCT_ConditionsData
                   src/*.cxx
                   PUBLIC_HEADERS SCT_ConditionsData
                   LINK_LIBRARIES AthenaPoolUtilities Identifier AthenaBaseComps )

