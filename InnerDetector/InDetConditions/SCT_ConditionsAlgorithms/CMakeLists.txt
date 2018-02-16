###############################################################################
# Package: SCT_ConditionsAlgorithms
################################################################################

# Declare the package name:
atlas_subdir( SCT_ConditionsAlgorithms )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          Control/AthenaBaseComps
                          Control/AthenaKernel
                          Control/StoreGate
                          DetectorDescription/Identifier
                          GaudiKernel
                          PRIVATE
                          InnerDetector/InDetConditions/SCT_ConditionsServices )

# External dependencies:

# Component(s) in the package:
atlas_add_component( SCT_ConditionsAlgorithms
                     src/components/*.cxx
                     LINK_LIBRARIES AthenaBaseComps AthenaKernel StoreGateLib SGtests Identifier GaudiKernel SCT_ConditionsAlgorithmsLib )

atlas_add_library ( SCT_ConditionsAlgorithmsLib
		    src/*.cxx
		    # PUBLIC_HEADERS SCT_ConditionsAlgorithms
		    LINK_LIBRARIES AthenaBaseComps AthenaKernel Identifier GaudiKernel StoreGateLib SGtests )

# Add unit tests

# Install files from the package:
