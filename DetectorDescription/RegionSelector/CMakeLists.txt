################################################################################
# Package: RegionSelector
################################################################################

# Declare the package name:
atlas_subdir( RegionSelector )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          Control/AthenaKernel
                          DetectorDescription/IRegionSelector
                          DetectorDescription/Identifier
                          GaudiKernel
                          PRIVATE
                          Control/AthenaBaseComps
                          Control/StoreGate
                          Database/AthenaPOOL/AthenaPoolUtilities
                          DetectorDescription/GeoModel/GeoModelInterfaces
                          DetectorDescription/RegSelLUT
                          InnerDetector/InDetConditions/InDetConditionsSummaryService
                          InnerDetector/InDetDetDescr/SCT_Cabling
                          Tools/PathResolver )

# Component(s) in the package:
atlas_add_library( RegionSelectorLib
                   src/*.cxx
                   PUBLIC_HEADERS RegionSelector
                   LINK_LIBRARIES AthenaKernel IRegionSelector Identifier GaudiKernel StoreGateLib SGtests
                   PRIVATE_LINK_LIBRARIES AthenaBaseComps AthenaPoolUtilities RegSelLUT PathResolver SCT_CablingLib GeoModelInterfaces InDetConditionsSummaryService )

atlas_add_component( RegionSelector
                     src/components/*.cxx
                     LINK_LIBRARIES AthenaKernel IRegionSelector Identifier GaudiKernel AthenaBaseComps StoreGateLib SGtests AthenaPoolUtilities RegSelLUT PathResolver RegionSelectorLib SCT_CablingLib GeoModelInterfaces )

# Install files from the package:
atlas_install_python_modules( python/*.py )
atlas_install_joboptions( share/*.py )


atlas_add_test( RegSelConfigTest
                SCRIPT python -m RegionSelector.RegSelConfig
                PROPERTIES TIMEOUT 300
                POST_EXEC_SCRIPT nopost.sh )
