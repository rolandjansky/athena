################################################################################
# Package: SCT_Cabling
################################################################################

# Declare the package name:
atlas_subdir( SCT_Cabling )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          Control/AthenaBaseComps
                          DetectorDescription/Identifier
                          GaudiKernel
                          PRIVATE
                          Control/StoreGate
                          Database/AthenaPOOL/AthenaPoolUtilities
                          Event/EventInfo
                          InnerDetector/InDetDetDescr/InDetIdentifier
                          Tools/PathResolver )

# Component(s) in the package:
atlas_add_library( SCT_CablingLib
                   src/*.cxx
                   PUBLIC_HEADERS SCT_Cabling
                   LINK_LIBRARIES AthenaBaseComps Identifier GaudiKernel StoreGateLib SGtests
                   PRIVATE_LINK_LIBRARIES AthenaPoolUtilities EventInfo InDetIdentifier PathResolver )

atlas_add_component( SCT_Cabling
                     src/components/*.cxx
                     LINK_LIBRARIES AthenaBaseComps Identifier GaudiKernel StoreGateLib SGtests AthenaPoolUtilities EventInfo InDetIdentifier PathResolver SCT_CablingLib )

# Install files from the package:
atlas_install_joboptions( share/*.py )
atlas_install_runtime( share/*.dat )

